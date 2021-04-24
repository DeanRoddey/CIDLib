//
// FILE NAME: CIDXML_EntitySpooler.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TXMLEntSpooler class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  We have to bring in some encodings directly. Normally they aren't exposed
//  because folks just gen them up dynamically through the CIDEncode faciltiy
//  object. But in this case we need some static helper calls for dealing
//  with the encoding declaration.
// ---------------------------------------------------------------------------
#include    "CIDEncode_EBCDIC_037_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLEntSpooler,TObject)



namespace CIDXML_EntitySpooler
{
    namespace
    {
        // ---------------------------------------------------------------------------
        //  Local const data
        //
        //  c4SeqLen
        //      The length of each of the byte arrays in the aac1EncodingSeqs
        //      array.
        //
        //  aac1EncodingSeqs
        //      An array of byte arrays. Each of these arrays contains the bytes
        //      necessary to recogize the XMLDecl of a particular base encoding. Its
        //      not the whole XMLDecl, just enough to recognize which encoding its
        //      in.
        //
        //  ac4DeclBytes
        //      This is an array that contains the number of bytes in each of the
        //      apc1Decls sub-arrays.
        //
        //  aac1Decls
        //      This is an array of pointers to byte sequences. Each sequence
        //      is the set of bytes that spell out "<?xml " in that encoding. The
        //      order is driven by the EBaseEncodings enum.
        //
        //  apszEncodingNames
        //      This is an array of strings that hold the names of the encodings
        //      in the EBaseEncodings enum.
        //
        //  pszDeclString
        //      A static const XMLDecl string in native wide char format, for use in
        //      a number of places in the code below.
        // ---------------------------------------------------------------------------
        constexpr tCIDLib::TCard4 c4SeqLen = 6;
        constexpr tCIDLib::TCard1
        aac1EncodingSeqs[tCIDLib::c4EnumOrd(tCIDXML::EBaseEncodings::Count)][c4SeqLen] =
        {
                { 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00 }
            ,   { 0x00, 0x3C, 0x00, 0x3F, 0x00, 0x78 }
            ,   { 0x3C, 0x00, 0x00, 0x00, 0x3F, 0x00 }
            ,   { 0x3C, 0x00, 0x3F, 0x00, 0x78, 0x00 }
            ,   { 0x3C, 0x3F, 0x78, 0x6D, 0x6C, 0x20 }
            ,   { 0x4C, 0x6F, 0xA7, 0x94, 0x93, 0x40 }
        };
        constexpr tCIDLib::TCard1
        aac1Decls[tCIDLib::c4EnumOrd(tCIDXML::EBaseEncodings::Count)][24] =
        {
            {
                0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x78
                , 0x00, 0x00, 0x00, 0x6D, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x20
            }
            , { 0x00, 0x3C, 0x00, 0x3F, 0x00, 0x78, 0x00, 0x6D, 0x00, 0x6C, 0x00, 0x20 }
            , {
                0x3C, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00
                , 0x6D, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00
              }
            , { 0x3C, 0x00, 0x3F, 0x00, 0x78, 0x00, 0x6D, 0x00, 0x6C, 0x00, 0x20, 0x00 }
            , { 0x3C, 0x3F, 0x78, 0x6D, 0x6C, 0x20 }
            , { 0x4C, 0x6F, 0xA7, 0x94, 0x93, 0x40 }
        };
        constexpr tCIDLib::TCard4 ac4DeclBytes[tCIDLib::c4EnumOrd(tCIDXML::EBaseEncodings::Count)] =
        {
            24, 12, 24, 12, 6, 6
        };


        const TString astrEncodingVals[] =
        {
            TString(L"UCS4-BE")
            , TString(L"UTF16-BE")
            , TString(L"UCS4-LE")
            , TString(L"UTF16-LE")
            , TString(L"UTF-8")
            , TString(L"EBCDIC-US")
        };
        TEArray<TString, tCIDXML::EBaseEncodings, tCIDXML::EBaseEncodings::Count> astrEncodingNames
        (
            astrEncodingVals
        );

        const tCIDLib::TCh* const pszXMLDeclString = L"<?xml ";
        constexpr tCIDLib::TCard4 c4DeclStrLen = 6;
        constexpr tCIDLib::TCard4 c4DeclStrBytes = 6 * kCIDLib::c4CharBytes;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TXMLEntSpooler
// PREFIX: xesp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLEntSpooler: Public, static methods
// ---------------------------------------------------------------------------

//
//  This method is called to probe a buffer to see what its base encoding is.
//  The base encoding is the one that will let us get through the XMLDecl
//  statement. XML files have to have an XMLDecl unless they are UTF8 or
//  UTF16 (with a Unicode marker.)
//
tCIDXML::EBaseEncodings
TXMLEntSpooler::eProbeBaseEncoding( const   tCIDLib::TCard1* const  pc1Buffer
                                    , const tCIDLib::TCard4         c4Count)
{
    // If its less than 2 bytes, it can't be anything but UTF8
    if (c4Count < 2)
        return tCIDXML::EBaseEncodings::UTF8;

    //
    //  Lets filter the bytes until we get a match or fail. As soon as we
    //  fail, we fall through to return UTF8.
    //
    const tCIDLib::TCard1* pc1Cur = pc1Buffer;

    //
    //  Check first for the UTF-16 Unicode BOM, in either orientation. We don't
    //  have to have an XMLDecl for this one.
    //
    if ((pc1Cur[0] == 0xFE) && (pc1Cur[1] == 0xFF))
        return tCIDXML::EBaseEncodings::UTF16_B;
     else if ((pc1Cur[0] == 0xFF) && (pc1Cur[1] == 0xFE))
        return tCIDXML::EBaseEncodings::UTF16_L;

    // See if it's a UTF-8 BOM
    if (c4Count > 3)
    {
        if ((pc1Cur[0] == 0xEF)
        &&  (pc1Cur[1] == 0xBB)
        &&  (pc1Cur[2] == 0xBF))
        {
            return tCIDXML::EBaseEncodings::UTF8;
        }
    }

    // If there are less than a sequence's worth of  chars now, got to be UTF-8
    if (c4Count < CIDXML_EntitySpooler::c4SeqLen)
        return tCIDXML::EBaseEncodings::UTF8;

    //
    //  Lets move through the sequence of encodings, moving forward for each
    //  byte we match.
    //
    tCIDLib::TInt4  i4EncIndex = 0;
    tCIDLib::TInt4  i4SeqIndex = 0;
    while ((i4EncIndex < tCIDLib::i4EnumOrd(tCIDXML::EBaseEncodings::Count))
    &&     (i4SeqIndex < CIDXML_EntitySpooler::c4SeqLen))
    {
        //
        //  If we get a match, then move forward in the sequence. Else we
        //  move forward in the encodings.
        //
        if (CIDXML_EntitySpooler::aac1EncodingSeqs[i4EncIndex][i4SeqIndex] == *pc1Cur)
        {
            i4SeqIndex++;
            pc1Cur++;
        }
         else
        {
            i4EncIndex++;
        }
    }

    // If we got to the sequence length, we got a match
    if (i4SeqIndex == CIDXML_EntitySpooler::c4SeqLen)
        return tCIDXML::EBaseEncodings(i4EncIndex);

    // Oh well, we have to assume its UTF8
    return tCIDXML::EBaseEncodings::UTF8;
}



// ---------------------------------------------------------------------------
//  TXMLEntSpooler: Private, static data members
// ---------------------------------------------------------------------------
TSafeCard4Counter   TXMLEntSpooler::s_scntNextSerialNum;



// ---------------------------------------------------------------------------
//  TXMLEntSpooler: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLEntSpooler::TXMLEntSpooler( const   TString&                strSystemId
                                ,       TBinInStream* const     pstrmToAdopt
                                , const TString&                strEncoding
                                , const tCIDXML::ERefFrom       eFrom
                                , const tCIDXML::EEntityTypes   eType
                                , const tCIDLib::TBoolean       bIgnoreBadChars) :

    m_bDataExhausted(kCIDLib::False)
    , m_bEmpty(kCIDLib::False)
    , m_bEncodingForced(kCIDLib::False)
    , m_bIgnoreBadChars(bIgnoreBadChars)
    , m_bInterned(kCIDLib::False)
    , m_bRefFromLiteral
      (
        eFrom == tCIDXML::ERefFrom::Literal ? kCIDLib::True : kCIDLib::False
      )
    , m_bSpaceAdded(kCIDLib::False)
    , m_c4CharBufCount(0)
    , m_c4CharBufInd(0)
    , m_c4CurColumn(1)
    , m_c4CurLine(1)
    , m_c4RawBufCount(0)
    , m_c4RawBufInd(0)
    , m_c4SpoolerId(0)
    , m_eType(eType)
    , m_ptcvtDecode(nullptr)
    , m_pstrmSrc(pstrmToAdopt)
    , m_strEncoding(strEncoding)
    , m_strSystemId(strSystemId)
{
    try
    {
        // Set the unique spooler id. THIS IS thread safe!
        m_c4SpoolerId = s_scntNextSerialNum++;

        //
        //  If the encoding is not empty, then we need to set the forced encoding
        //  flag. This will override any auto sensing of the encoding.
        //
        m_bEncodingForced = !m_strEncoding.bIsEmpty();

        //
        //  Do an initial load of the raw data spooling buffer. In many cases
        //  this will load the entire entity, but if not it will be refilled as
        //  required.
        //
        ReloadRawData();

        //
        //  If we got any data, then do an auto sense of the encoding. Even if
        //  there is an encodng override, we still have to decode any XMLDecl
        //  manually.
        //
        if (m_c4RawBufCount)
        {
            const tCIDXML::EBaseEncodings eBaseEncoding = eProbeBaseEncoding
            (
                m_ac1RawBuf, m_c4RawBufCount
            );

            //
            //  Do a decode of the XMLDecl, if any. Tell it the base encoding
            //  so it can quickly to the right thing.
            //
            DecodeDecl(eBaseEncoding);

            // If we don't have an encoding string, use the default
            if (!m_bEncodingForced)
                m_strEncoding = CIDXML_EntitySpooler::astrEncodingNames[eBaseEncoding];
        }

        //
        //  If this is a parameter entity and is not being referenced from within
        //  a literal, we need to put in a leading space.
        //
        if ((m_eType == tCIDXML::EEntityTypes::Parameter) && !m_bRefFromLiteral)
            m_achCharBuf[m_c4CharBufCount++] = kCIDLib::chSpace;

        // If we did not end up with any raw chars or cooked chars, we are empty
        if (!m_c4RawBufCount && !m_c4CharBufCount)
        {
            m_bDataExhausted = kCIDLib::True;
            m_bEmpty = kCIDLib::True;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Clean up the converter and stream since we own them
        delete m_ptcvtDecode;
        delete m_pstrmSrc;
        throw;
    }
}


TXMLEntSpooler::TXMLEntSpooler( const   TString&                strSystemId
                                ,       TBinInStream* const     pstrmToAdopt
                                ,       TTextConverter* const   ptcvtToAdopt
                                , const tCIDXML::ERefFrom       eFrom
                                , const tCIDXML::EEntityTypes   eType
                                , const tCIDLib::TBoolean       bIgnoreBadChars) :

    m_bDataExhausted(kCIDLib::False)
    , m_bEmpty(kCIDLib::False)
    , m_bIgnoreBadChars(bIgnoreBadChars)
    , m_bEncodingForced(kCIDLib::True)
    , m_bInterned(kCIDLib::False)
    , m_bRefFromLiteral
      (
        eFrom == tCIDXML::ERefFrom::Literal ? kCIDLib::True : kCIDLib::False
      )
    , m_bSpaceAdded(kCIDLib::False)
    , m_c4CharBufCount(0)
    , m_c4CharBufInd(0)
    , m_c4CurColumn(1)
    , m_c4CurLine(1)
    , m_c4RawBufCount(0)
    , m_c4RawBufInd(0)
    , m_c4SpoolerId(0)
    , m_eType(eType)
    , m_ptcvtDecode(ptcvtToAdopt)
    , m_pstrmSrc(pstrmToAdopt)
    , m_strEncoding(ptcvtToAdopt->strEncodingName())
    , m_strSystemId(strSystemId)
{
    try
    {
        // Set the unique spooler id. THIS IS thread safe!
        m_c4SpoolerId = s_scntNextSerialNum++;

        // Tell the decoder our desired action on bad source
        m_ptcvtDecode->eErrorAction
        (
            m_bIgnoreBadChars ? tCIDLib::ETCvtActs::Replace
                              : tCIDLib::ETCvtActs::StopThenThrow
        );

        //
        //  Do an initial load of the raw data spooling buffer. In many cases
        //  this will load the entire entity, but if not it will be refilled as
        //  required.
        //
        ReloadRawData();

        //
        //  If we got any raw chars, then lets do a probe of the encoding. Even
        //  though we were given a converter, it still has to be autosenseable,
        //  and we have to decode the XMLDecl.
        //
        if (m_c4RawBufCount)
        {
            const tCIDXML::EBaseEncodings eBaseEncoding = eProbeBaseEncoding
            (
                m_ac1RawBuf, m_c4RawBufCount
            );

            //
            //  Do a decode of the XMLDecl, if any. Tell it the base encoding
            //  so it can quickly to the right thing.
            //
            DecodeDecl(eBaseEncoding);
        }

        //
        //  If this is a parameter entity and is not being referenced from within
        //  a literal, we need to put in a leading space.
        //
        if ((m_eType == tCIDXML::EEntityTypes::Parameter) && !m_bRefFromLiteral)
            m_achCharBuf[m_c4CharBufCount++] = kCIDLib::chSpace;

        // If we did not end up with any raw chars or cooked chars, we are empty
        if (!m_c4RawBufCount && !m_c4CharBufCount)
        {
            m_bDataExhausted = kCIDLib::True;
            m_bEmpty = kCIDLib::True;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Clean up the converter and stream since we own them
        delete m_ptcvtDecode;
        delete m_pstrmSrc;
        throw;
    }
}


TXMLEntSpooler::~TXMLEntSpooler()
{
    delete m_pstrmSrc;
    delete m_ptcvtDecode;
}


// ---------------------------------------------------------------------------
//  TXMLEntSpooler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLEntSpooler::bCharBufEmpty() const
{
    return (m_c4CharBufInd == m_c4CharBufCount);
}

tCIDLib::TBoolean TXMLEntSpooler::bEmpty() const
{
    return m_bEmpty;
}

tCIDLib::TBoolean TXMLEntSpooler::bFromLiteral() const
{
    return m_bRefFromLiteral;
}

tCIDLib::TBoolean
TXMLEntSpooler::bFromLiteral(const tCIDLib::TBoolean bState)
{
    m_bRefFromLiteral = bState;
    return m_bRefFromLiteral;
}


tCIDLib::TBoolean
TXMLEntSpooler::bGetName(TString& strToFill, const tCIDLib::TBoolean bTokenType)
{
    //
    //  If the buffer is empty, then reload it. If we can't get any more
    //  data, then give up.
    //
    if (m_c4CharBufInd == m_c4CharBufCount)
    {
        if (!bReloadCharBuf())
            return kCIDLib::False;
    }

    //
    //  See if the current char is a first name char. If not, then we can
    //  give up now. We only do this one if they want a name as apposed to
    //  a name token.
    //
    strToFill.Clear();
    if (!bTokenType)
    {
        if (!TXMLCharFlags::bIsFirstNameChar(m_achCharBuf[m_c4CharBufInd]))
            return kCIDLib::False;

        // Reset the buffer and append this first char
        strToFill.Append(m_achCharBuf[m_c4CharBufInd++]);
        m_c4CurColumn++;
    }

    //
    //  So now we can just loop and pull out chars until we get a non-
    //  name character, storing them in the passed buffer.
    //
    //  NOTE:   A name will never legally cross an entity boundary or contain
    //          any whitespace so this is safe.
    //
    while (kCIDLib::True)
    {
        // Go through the current buffer contents
        while (m_c4CharBufInd < m_c4CharBufCount)
        {
            // Return on a non-name char
            if (!TXMLCharFlags::bIsNameChar(m_achCharBuf[m_c4CharBufInd]))
                return !strToFill.bIsEmpty();

            // It was ok so add it to the buffer and bump index
            strToFill.Append(m_achCharBuf[m_c4CharBufInd++]);

            // Keep the column up to date
            m_c4CurColumn++;
        }

        // Try to reload again. If we can't, then break out
        if (!bReloadCharBuf())
            break;
    }

    // If we got at least a char, then we succeeded
    return (!strToFill.bIsEmpty());
}


tCIDLib::TBoolean TXMLEntSpooler::bGetNext(tCIDLib::TCh& chToFill)
{
    //
    //  If we have more internalized characters, then get the next one.
    //  Else, we have to reload the char buffer with more internalized chars
    //  from the raw buffer.
    //
    if (m_c4CharBufInd < m_c4CharBufCount)
    {
        chToFill = m_achCharBuf[m_c4CharBufInd++];
    }
     else
    {
        //
        //  If the 'exhausted' flag is set, then just don't have any more and
        //  don't need to spend the time checking.
        //
        if (m_bDataExhausted)
            return kCIDLib::False;

        // There might be more data, so lets try it
        if (!bReloadCharBuf())
            return kCIDLib::False;

        // Ok, we got something, so get the next char
        chToFill = m_achCharBuf[m_c4CharBufInd++];
    }

    //
    //  Now we have to keep the line and colum information up to date, which
    //  is required to give source location info when errors occur.
    //
    if (chToFill == kCIDLib::chCR)
    {
        //
        //  We have to do standard XML normalization. But, we only want to
        //  do this if we are not spooling an internalized entity. If its
        //  internalized, then this normalization is already done.
        //
        if (!m_bInterned)
        {
            //
            //  If its a CR, then we have normalize it. If its followed by
            //  an LF, then we have to normalized them both to a single LF.
            //  We can't assume we have another char, so check and reload
            //  the buffer if required.
            //
            if (chToFill == kCIDLib::chCR)
            {
                if (m_c4CharBufInd == m_c4CharBufCount)
                    bReloadCharBuf();

                // If we have more chars, then check the next one
                if (m_c4CharBufInd < m_c4CharBufCount)
                {
                    // If its an LF, then eat it
                    if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                        m_c4CharBufInd++;
                }

                chToFill = kCIDLib::chLF;
            }
        }

        m_c4CurLine++;
        m_c4CurColumn = 1;
    }
     else if (chToFill == kCIDLib::chLF)
    {
        m_c4CurLine++;
        m_c4CurColumn = 1;
    }
     else
    {
        // We just need to bump the column in this case
        m_c4CurColumn++;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TXMLEntSpooler::bGetNextIfNot(          tCIDLib::TCh&   chToFill
                                , const tCIDLib::TCh    chNotToGet)
{
    //
    //  If we have more internalized characters, then get the next one.
    //  Else, we have to reload the char buffer with more internalized chars
    //  from the raw buffer.
    //
    if (m_c4CharBufInd < m_c4CharBufCount)
    {
        chToFill = m_achCharBuf[m_c4CharBufInd];
    }
     else
    {
        //
        //  If the 'exhausted' flag is set, then just don't have any more and
        //  don't need to spend the time checking.
        //
        if (m_bDataExhausted)
            return kCIDLib::False;

        // There might be more data, so lets try it
        if (!bReloadCharBuf())
            return kCIDLib::False;

        // Ok, we got something, so get the next char
        chToFill = m_achCharBuf[m_c4CharBufInd];
    }

    // If its the char not to get, then return now
    if (chToFill == chNotToGet)
        return kCIDLib::True;

    // Else we are keeping it, so bump the index
    m_c4CharBufInd++;

    //
    //  Now we have to keep the line and colum information up to date, which
    //  is required to give source location info when errors occur.
    //
    if (chToFill == kCIDLib::chCR)
    {
        //
        //  We have to do standard XML normalization. But, we only want to
        //  do this if we are not spooling an internalized entity. If its
        //  internalized, then this normalization is already done.
        //
        if (!m_bInterned)
        {
            //
            //  If its a CR, then we have normalize it. If its followed by
            //  an LF, then we have to normalized them both to a single LF.
            //  We can't assume we have another char, so check and reload
            //  the buffer if required.
            //
            if (chToFill == kCIDLib::chCR)
            {
                if (m_c4CharBufInd == m_c4CharBufCount)
                    bReloadCharBuf();

                // If we have more chars, then check the next one
                if (m_c4CharBufInd < m_c4CharBufCount)
                {
                    // If its an LF, then eat it
                    if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                        m_c4CharBufInd++;
                }

                chToFill = kCIDLib::chLF;
            }
        }

        m_c4CurLine++;
        m_c4CurColumn = 1;
    }
     else if (chToFill == kCIDLib::chLF)
    {
        m_c4CurLine++;
        m_c4CurColumn = 1;
    }
     else
    {
        // We just need to bump the column in this case
        m_c4CurColumn++;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TXMLEntSpooler::bGetSpaces(TString& strToFill)
{
    //
    //  We enter a loop here where we skip chars in the char buffer while
    //  they are spaces. When we eat up the current buffer, we reload. We
    //  break out on a non-whitespace or end of entity.
    //
    while (kCIDLib::True)
    {
        while (m_c4CharBufInd < m_c4CharBufCount)
        {
            tCIDLib::TCh chCur = m_achCharBuf[m_c4CharBufInd];

            // Keep going while we have spaces
            if (TXMLCharFlags::bIsSpace(chCur))
            {
                // We are going to eat this one
                m_c4CharBufInd++;

                //
                //  Keep the line and column info up to date. Doing the
                //  correct normalization if this entity is not interned.
                //
                if (chCur == kCIDLib::chCR)
                {
                    m_c4CurLine++;
                    m_c4CurColumn = 1;

                    //
                    //  If we are not interned, then turn into an LF. And
                    //  look for a following LF to eat.
                    //
                    if (!m_bInterned)
                    {
                        if (m_c4CharBufInd == m_c4CharBufCount)
                        {
                            if (bReloadCharBuf())
                            {
                                if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                                    m_c4CharBufInd++;
                            }
                        }
                         else
                        {
                            if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                                m_c4CharBufInd++;
                        }

                        chCur = kCIDLib::chLF;
                    }
                }
                 else if (chCur == kCIDLib::chLF)
                {
                    m_c4CurLine++;
                    m_c4CurColumn = 1;
                }
                 else
                {
                    m_c4CurColumn++;
                }

                // Put it into the target buffer and go get another char
                strToFill.Append(chCur);
                continue;
            }

            // We hit a non-space, so we are done
            return kCIDLib::True;
        }

        //
        //  We ate up that buffer, so lets try to reload the char buffer. If
        //  we don't get anything, then break out and return false.
        //
        if (!bReloadCharBuf())
            break;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TXMLEntSpooler::bInterned() const
{
    return m_bInterned;
}


tCIDLib::TBoolean TXMLEntSpooler::bInterned(const tCIDLib::TBoolean bState)
{
    m_bInterned = bState;
    return m_bInterned;
}


tCIDLib::TBoolean
TXMLEntSpooler::bPeekAhead(         tCIDLib::TCh&   chToFill
                            , const tCIDLib::TCard4 c4AheadBy)
{
    //
    //  If the amount to peek ahead would go beyond the end of the char
    //  buffer, then reload it and see if we can get enough chars.
    //
    if (m_c4CharBufInd + c4AheadBy >= m_c4CharBufCount)
        bReloadCharBuf();

    //
    //  Check again. If its still beyond the end of the buffer, then we
    //  fail.
    //
    if (m_c4CharBufInd + c4AheadBy >= m_c4CharBufCount)
    {
        chToFill = kCIDLib::chNull;
        return kCIDLib::False;
    }

    // Return the character
    chToFill = m_achCharBuf[m_c4CharBufInd + c4AheadBy];
    return kCIDLib::True;
}


tCIDLib::TBoolean TXMLEntSpooler::bPeekNext(tCIDLib::TCh& chToFill)
{
    //
    //  If there are chars available, then just get the next one and don't
    //  bump the index. Else we have to try to reload the char buffer.
    //
    if (m_c4CharBufInd < m_c4CharBufCount)
    {
        chToFill = m_achCharBuf[m_c4CharBufInd];
    }
     else
    {
        //
        //  If the 'exhausted' flag is set, then just don't have any more and
        //  don't need to spend the time checking.
        //
        if (m_bDataExhausted)
        {
            chToFill = kCIDLib::chNull;
            return kCIDLib::False;
        }

        // There might be more data, so lets try it. If not, return failure
        if (!bReloadCharBuf())
        {
            chToFill = kCIDLib::chNull;
            return kCIDLib::False;
        }

        // Ok, we got something, so return the next char
        chToFill = m_achCharBuf[m_c4CharBufInd];
    }

    //
    //  Do a simplified version of the new line normalization if its a
    //  CR.
    //
    if (!m_bInterned && (chToFill == kCIDLib::chCR))
        chToFill = kCIDLib::chLF;

    return kCIDLib::True;
}


tCIDLib::TBoolean TXMLEntSpooler::bSkippedChar(const tCIDLib::TCh chToSkip)
{
    // If we are out of chars, try to reload. If we cannot, then fail
    if (m_c4CharBufInd == m_c4CharBufCount)
    {
        if (!bReloadCharBuf())
            return kCIDLib::False;
    }

    if (m_achCharBuf[m_c4CharBufInd] == chToSkip)
    {
        m_c4CharBufInd++;

        if (chToSkip == kCIDLib::chCR)
        {
            m_c4CurLine++;
            m_c4CurColumn = 1;

            // If not interned, then look for a following LF to eat
            if (!m_bInterned)
            {
                // If no more chars, try to reload. If none, we are done
                if (m_c4CharBufInd == m_c4CharBufCount)
                {
                    if (!bReloadCharBuf())
                        return kCIDLib::True;
                }

                if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                    m_c4CharBufInd++;
            }
        }
         else if (chToSkip == kCIDLib::chLF)
        {
            m_c4CurLine++;
            m_c4CurColumn = 1;
        }
         else
        {
            m_c4CurColumn++;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TXMLEntSpooler::bSkippedQuote(COP tCIDLib::TCh& chSkipped)
{
    // If we are out of chars, try to reload. If we cannot, then fail
    if (m_c4CharBufInd == m_c4CharBufCount)
    {
        if (!bReloadCharBuf())
        {
            chSkipped = kCIDLib::chNull;
            return kCIDLib::False;
        }
    }

    // Get the next char and check it for a quote char
    chSkipped = m_achCharBuf[m_c4CharBufInd];

    if ((chSkipped == kCIDLib::chApostrophe)
    ||  (chSkipped == kCIDLib::chQuotation))
    {
        m_c4CharBufInd++;
        m_c4CurColumn++;
        return kCIDLib::True;
    }
    chSkipped = kCIDLib::chNull;
    return kCIDLib::False;
}


tCIDLib::TBoolean TXMLEntSpooler::bSkippedSpace()
{
    // If we are out of chars, try to reload. If we cannot, then fail
    if (m_c4CharBufInd == m_c4CharBufCount)
    {
        if (!bReloadCharBuf())
            return kCIDLib::False;
    }

    const tCIDLib::TCh chToCheck = m_achCharBuf[m_c4CharBufInd];
    if (TXMLCharFlags::bIsSpace(chToCheck))
    {
        m_c4CharBufInd++;

        //
        //  Handle normalization and line/col processing if required. If
        //  there is an LF following a CR and we are not interned, then
        //  eat it.
        //
        if (chToCheck == kCIDLib::chCR)
        {
            m_c4CurLine++;
            m_c4CurColumn = 1;

            // If not interned, and the next char is an LF, then eat it
            if (!m_bInterned)
            {
                // If no more chars, try to reload. If none, we are done
                if (m_c4CharBufInd == m_c4CharBufCount)
                {
                    if (!bReloadCharBuf())
                        return kCIDLib::True;
                }

                if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                    m_c4CharBufInd++;
            }
        }
         else if (chToCheck == kCIDLib::chLF)
        {
            m_c4CurLine++;
            m_c4CurColumn = 1;
        }
         else
        {
            m_c4CurColumn++;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TXMLEntSpooler::bSkippedString(const tCIDLib::TCh* const pszToSkip)
{
    // Get the length of the string to skip
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToSkip);

    //
    //  See if this would go beyond the current char buffer. If so, then
    //  reload the char buffer to see if we can get enough.
    //
    if (m_c4CharBufInd + c4Len > m_c4CharBufCount)
        bReloadCharBuf();

    //
    //  Check again. If its still beyond the end of the buffer, then we
    //  fail. These types of strings cannot legally cross a spooler boundary
    //  so we don't have to look at the next one.
    //
    if (m_c4CharBufInd + c4Len > m_c4CharBufCount)
        return kCIDLib::False;

    //
    //  Ok, lets do a string compare of the passed string against the chars
    //  in the buffer. If it doesn't match, then just return false.
    //
    //  NOTE:   No string every contains any CR or LF, so we don't have to
    //          do any normalization or line/col maintenance here.
    //
    if (!TRawStr::bCompareStrN(pszToSkip, &m_achCharBuf[m_c4CharBufInd], c4Len))
        return kCIDLib::False;

    //
    //  It matched, so lets move the char buffer index forward by the length
    //  of the skipped string. And we have to move the current column up too.
    //
    m_c4CharBufInd += c4Len;
    m_c4CurColumn += c4Len;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TXMLEntSpooler::bSkippedString(const TString& strToSkip)
{
    // Get the length of the string to skip
    const tCIDLib::TCard4 c4Len = strToSkip.c4Length();

    //
    //  See if this would go beyond the current char buffer. If so, then
    //  reload the char buffer to see if we can get enough.
    //
    if (m_c4CharBufInd + c4Len > m_c4CharBufCount)
        bReloadCharBuf();

    //
    //  Check again. If its still beyond the end of the buffer, then we
    //  fail. These types of strings cannot legally cross a spooler boundary
    //  so we don't have to look at the next one.
    //
    if (m_c4CharBufInd + c4Len > m_c4CharBufCount)
        return kCIDLib::False;

    //
    //  Ok, lets do a string compare of the passed string against the chars
    //  in the buffer. If it doesn't match, then just return false.
    //
    //  NOTE:   No string every contains any CR or LF, so we don't have to
    //          do any normalization or line/col maintenance here.
    //
    if (!strToSkip.bCompareN(&m_achCharBuf[m_c4CharBufInd], c4Len))
        return kCIDLib::False;

    //
    //  It matched, so lets move the char buffer index forward by the length
    //  of the skipped string. And we have to move the current column up too.
    //
    m_c4CharBufInd += c4Len;
    m_c4CurColumn += c4Len;
    return kCIDLib::True;
}


tCIDLib::TBoolean TXMLEntSpooler::bSkipSpaces(tCIDLib::TBoolean& bSkippedSome)
{
    //
    //  We enter a loop here where we skip chars in the char buffer while
    //  they are spaces. When we eat up the current buffer, we reload. We
    //  break out on a non-whitespace or end of entity.
    //
    const tCIDLib::TCard4   c4OldLine   = m_c4CurLine;
    const tCIDLib::TCard4   c4OldCol    = m_c4CurColumn;
    while (kCIDLib::True)
    {
        while (m_c4CharBufInd < m_c4CharBufCount)
        {
            const tCIDLib::TCh chCur = m_achCharBuf[m_c4CharBufInd];

            // Keep going while we have spaces
            if (TXMLCharFlags::bIsSpace(chCur))
            {
                // We are going to eat this one
                m_c4CharBufInd++;

                //
                //  Keep the line and column info up to date. We don't have
                //  to normalize, but we have to avoid triggering twice on
                //  an LF following a CR.
                //
                if (chCur == kCIDLib::chCR)
                {
                    m_c4CurLine++;
                    m_c4CurColumn = 1;

                    //
                    //  If we are not interned, then look for a following LF
                    //  to eat.
                    //
                    if (!m_bInterned)
                    {
                        if (m_c4CharBufInd == m_c4CharBufCount)
                        {
                            if (bReloadCharBuf())
                            {
                                if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                                    m_c4CharBufInd++;
                            }
                        }
                         else
                        {
                            if (m_achCharBuf[m_c4CharBufInd] == kCIDLib::chLF)
                                m_c4CharBufInd++;
                        }
                    }
                }
                 else if (chCur == kCIDLib::chLF)
                {
                    m_c4CurLine++;
                    m_c4CurColumn = 1;
                }
                 else
                {
                    m_c4CurColumn++;
                }

                // And go grab another char
                continue;
            }

            // We hit a non-space, so we are done.
            bSkippedSome = (c4OldLine != m_c4CurLine) || (c4OldCol != m_c4CurColumn);
            return kCIDLib::True;
        }

        //
        //  We ate up that buffer, so lets try to reload the char buffer. If
        //  we don't get anything, then return false.
        //
        if (!bReloadCharBuf())
        {
            bSkippedSome = (c4OldLine != m_c4CurLine) || (c4OldCol != m_c4CurColumn);
            return kCIDLib::False;
        }
    }

    //
    //  Return whether we skipped anything. We see if the line or column
    //  changed.
    //
    bSkippedSome = (c4OldLine != m_c4CurLine) || (c4OldCol != m_c4CurColumn);
    return kCIDLib::True;
}


tCIDLib::TCard4 TXMLEntSpooler::c4CurColumn() const
{
    return m_c4CurColumn;
}


tCIDLib::TCard4 TXMLEntSpooler::c4CurLine() const
{
    return m_c4CurLine;
}


tCIDLib::TCard4 TXMLEntSpooler::c4SpoolerId() const
{
    return m_c4SpoolerId;
}


const TString& TXMLEntSpooler::strSystemId() const
{
    return m_strSystemId;
}


tCIDLib::TVoid
TXMLEntSpooler::SetDeclEncoding(const TString& strDeclEncoding)
{
    // If the encoding was forced, then we just eat this event
    if (m_bEncodingForced)
        return;

    //
    //  We watch for any of the non-endian specific variants of our base
    //  encodings. If we see one, don't use them, since our autosensed
    //  encoding is more correct.
    //
    if (!strDeclEncoding.bCompareI(L"UTF-16")
    &&  !strDeclEncoding.bCompareI(L"UCS-2")
    &&  !strDeclEncoding.bCompareI(L"CP1200")
    &&  !strDeclEncoding.bCompareI(L"ISO-10646-USC-2")
    &&  !strDeclEncoding.bCompareI(L"UCS-4"))
    {
        m_strEncoding = strDeclEncoding;
    }

    //
    //  We need to create an transcoder at this point. So we are either
    //  creating one for the basic auto-sensed format or the one that was
    //  set in the passed encoding string.
    //
    m_ptcvtDecode = facCIDEncode().ptcvtMake
    (
        m_strEncoding
        , m_bIgnoreBadChars ? tCIDLib::ETCvtActs::Replace
                            : tCIDLib::ETCvtActs::StopThenThrow
    );
    if (!m_ptcvtDecode)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcEnt_UnknownEncoding
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , m_strEncoding
        );
    }
}



// ---------------------------------------------------------------------------
//  TXMLEntSpooler: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  WE try to parse out the XMLDecl text. This is a special case because we can't
//  really know the encoding until after we get the decl parsed. The caller did a
//  probe to figure out the encoding based on the known possible forms of the first
//  handful of characters. We get that and use it to pull out just the XMLDecl
//  chars into m_achCharBuf (which is our spooling buffer.)
//
//  We update m_c4CharBufCount with how many characters we put into the
//  spoolg buffer (i.e. how many characters in the XMLDecl we teased out of the
//  data.) So once that's been eaten, the spool buffer has to be reloaded.
//  We update m_c4RawBufInd to where we left off, so that's where we will start
//  decoding bytes once the caller has parsed out the decl and set the real encoding
//  on us.
//
tCIDLib::TVoid
TXMLEntSpooler::DecodeDecl(const tCIDXML::EBaseEncodings eEncoding)
{
    // Get a short cut pointer to the decl sequence for this encoding
    const tCIDLib::TCard4 c4EncIndex = tCIDLib::c4EnumOrd(eEncoding);
    const tCIDLib::TCard1* pc1MyDecl = CIDXML_EntitySpooler::aac1Decls[c4EncIndex];
    const tCIDLib::TCard4  c4MyDeclBytes = CIDXML_EntitySpooler::ac4DeclBytes[c4EncIndex];

    //
    //  And get a target pointer to the char spooling buffer that we can run up
    //  as we decode decl chars.
    //
    tCIDLib::TCh* pchOut = m_achCharBuf;

    if (eEncoding == tCIDXML::EBaseEncodings::UTF8)
    {
        // If not enough bytes to even match the prefix, then definitely no decl
        if (m_c4RawBufCount < c4MyDeclBytes)
            return;

        // Point a source pointer at the start of the raw input buffer
        const tCIDLib::TCard1* pc1Src = m_ac1RawBuf;

        //
        //  Check for a UTF-8 BOM and skip it. We also push the raw buffer index
        //  forward by the same amount.
        //
        m_c4RawBufInd = 0;
        if (m_c4RawBufCount > 3)
        {
            if ((m_ac1RawBuf[0] == 0xEF)
            &&  (m_ac1RawBuf[1] == 0xBB)
            &&  (m_ac1RawBuf[2] == 0xBF))
            {
                pc1Src += 3;
                m_c4RawBufInd += 3;
            }
        }

        // Check for the prefix to match. If not, then give up
        if (TRawMem::bCompareMemBuf(pc1MyDecl, pc1Src, c4MyDeclBytes))
        {
            //
            //  Get a pointer to the end of the raw buffer data so we know when we
            //  have hit the end.
            //
            const tCIDLib::TCard1* pc1End = pc1Src + m_c4RawBufCount;

            //
            //  Go ahead and set up the decl chars specially, since we know we
            //  have them already. This will save time below.
            //
            TRawMem::CopyMemBuf
            (
                m_achCharBuf
                , CIDXML_EntitySpooler::pszXMLDeclString
                , CIDXML_EntitySpooler::c4DeclStrBytes
            );
            pc1Src += c4MyDeclBytes;
            pchOut += CIDXML_EntitySpooler::c4DeclStrLen;

            while (pc1Src < pc1End)
            {
                //
                //  If the current byte is > 0x7F, its not valid for this
                //  encoding, so reset our index and throw an error.
                //
                if (*pc1Src > 0x7F)
                {
                    facCIDXML().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kXMLErrs::errcEnt_BadCharInXMLDecl
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }


                // Upcast the short char to a Unicode char, and bump the pointer
                const tCIDLib::TCh chCur = static_cast<tCIDLib::TCh>(*pc1Src++);

                // Looks ok, so store it
                *pchOut++ = chCur;

                // Break out when we hit the closing bracket
                if (chCur == kCIDLib::chGreaterThan)
                    break;
            }

            //
            //  Updates the two indexes, they are not the same potentially because
            //  of the BOM. The raw buffer index may already be offset, so we just add
            //  to it the number of chars we pulled out for the decl.
            //
            m_c4CharBufCount = (pchOut - m_achCharBuf);
            m_c4RawBufInd += m_c4CharBufCount;
        }
    }
     else if ((eEncoding == tCIDXML::EBaseEncodings::UTF16_L)
          ||  (eEncoding == tCIDXML::EBaseEncodings::UTF16_B))
    {
        // If less than a char's worth of bytes avail, then nothing to do
        if (m_c4RawBufCount < kCIDLib::c4UniBytes)
            return;

        // See if we need to swap byte order on the input
        tCIDLib::TBoolean bSwapped = kCIDLib::False;
        if (eEncoding == tCIDXML::EBaseEncodings::UTF16_B)
        {
            #if defined(CIDLIB_LITTLEENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }
         else
        {
            #if defined(CIDLIB_BIGENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }

        //
        //  Look at our raw data buffer as UTF16 chars. Also get a pointer
        //  to the end of the available data.
        //
        const tCIDLib::TUniCh* puchSrc = reinterpret_cast<const tCIDLib::TUniCh*>(m_ac1RawBuf);
        const tCIDLib::TUniCh* puchEnd = tCIDLib::pOffsetPtr
        (
            puchSrc, m_c4RawBufCount
        );

        // Look for a BOM and skip over it
        if ((*puchSrc == kCIDLib::chUniBOM)
        ||  (*puchSrc == kCIDLib::chSwappedUniBOM))
        {
            // Make sure its the expected type. If not, throw an error
            if ((bSwapped && (*puchSrc == kCIDLib::chUniBOM))
            ||  (!bSwapped && (*puchSrc == kCIDLib::chSwappedUniBOM)))
            {
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcEnt_BOMConflict
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            // Eat the BOM
            puchSrc++;
            m_c4RawBufInd += kCIDLib::c4UniBytes;
        }

        // If there is not enough left to check for a decl, then just return
        if ((m_c4RawBufCount - m_c4RawBufInd) < c4MyDeclBytes)
            return;

        //
        //  If we don't have at least a decl sequence worth of bytes left,
        //  then we are done.
        //
        if (TRawMem::bCompareMemBuf(puchSrc, pc1MyDecl, c4MyDeclBytes))
        {
            // Go ahead and set up the decl chars, to save time
            TRawStr::CopyStr(m_achCharBuf, CIDXML_EntitySpooler::pszXMLDeclString);
            puchSrc = tCIDLib::pOffsetPtr(puchSrc, c4MyDeclBytes);
            pchOut += CIDXML_EntitySpooler::c4DeclStrLen;

            //
            //  Loop through the XMLDecl. We loop until we hit the end of the
            //  raw input, or we find the > char.
            //
            while (puchSrc < puchEnd)
            {
                // Pull out another char
                tCIDLib::TUniCh uchCur = *puchSrc++;

                // Swap it to our native byte order
                if (bSwapped)
                    uchCur = tCIDLib::TUniCh(TRawBits::c2SwapBytes(uchCur));

                // Store it and break out if its the > char
                *pchOut++ = tCIDLib::TCh(uchCur);
                if (uchCur == kCIDLib::chGreaterThan)
                    break;
            }

            // Update the two indexes by what we ate
            m_c4RawBufInd = TRawMem::c4AdrDiff(m_ac1RawBuf, puchSrc);
            m_c4CharBufCount = pchOut - m_achCharBuf;
        }
    }
     else if ((eEncoding == tCIDXML::EBaseEncodings::UCS4_L)
          ||  (eEncoding == tCIDXML::EBaseEncodings::UCS4_B))
    {
        // See if we have enough chars to do at least the decl seq
        if (m_c4RawBufCount < c4MyDeclBytes)
            return;

        // See if we need to swap byte order on the input
        tCIDLib::TBoolean bSwapped = kCIDLib::False;
        if (eEncoding == tCIDXML::EBaseEncodings::UCS4_B)
        {
            #if defined(CIDLIB_LITTLEENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }
         else
        {
            #if defined(CIDLIB_BIGENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }

        //
        //  If we get a match against our own declaration bytes, then lets
        //  decode it.
        //
        const tCIDLib::TCard4* pc4Src = reinterpret_cast<const tCIDLib::TCard4*>(m_ac1RawBuf);
        const tCIDLib::TCard4* pc4End = tCIDLib::pOffsetPtr(pc4Src, m_c4RawBufCount);

        //
        //  See if we find the UCS4 XMLDecl here. If so, then we go ahead and
        //  decode the rest of it.
        //
        if (TRawMem::bCompareMemBuf(pc4Src, pc1MyDecl, c4MyDeclBytes))
        {
            // Go ahead and set up the decl chars, to save time
            TRawStr::CopyStr(m_achCharBuf, CIDXML_EntitySpooler::pszXMLDeclString);
            pc4Src = tCIDLib::pOffsetPtr(pc4Src, c4MyDeclBytes);
            pchOut += CIDXML_EntitySpooler::c4DeclStrLen;

            //
            //  Loop through the XMLDecl. We loop until we hit the end of the
            //  raw input, or we find the > char.
            //
            while (pc4Src < pc4End)
            {
                // Pull out another char
                tCIDLib::TCard4 c4Cur = *pc4Src++;

                // Swap it to our native byte order
                if (bSwapped)
                    c4Cur = TRawBits::c4SwapBytes(c4Cur);

                //
                //  Within the XMLDecl, we never expect to see anything outside
                //  of the low 256 code points.
                //
                if (c4Cur > 0xFF)
                {
                    facCIDXML().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kXMLErrs::errcEnt_BadCharInXMLDecl
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                // Store it and break out if its the > char
                *pchOut++ = tCIDLib::TCh(c4Cur);
                if (c4Cur == kCIDLib::chGreaterThan)
                    break;
            }

            // Update the two indexes by what we ate
            m_c4RawBufInd += TRawMem::c4AdrDiff(m_ac1RawBuf, pc4Src);
            m_c4CharBufCount = pchOut - m_achCharBuf;
        }
    }
     else if (eEncoding == tCIDXML::EBaseEncodings::EBCDIC_US)
    {
        // See if we have enough chars to do at least the decl seq
        if (m_c4RawBufCount < c4MyDeclBytes)
            return;

        //
        //  If we get a match on our declaration, then let decode the Decl.
        //  We make use of a special static method on the EBCDIC-US text
        //  converter.
        //
        if (TRawMem::bCompareMemBuf(m_ac1RawBuf, pc1MyDecl, c4MyDeclBytes))
        {
            // Get a pointer into the raw bytes and an end pointer
            const tCIDLib::TCard1* pc1Src = m_ac1RawBuf;
            const tCIDLib::TCard1* pc1End = pc1Src + m_c4RawBufCount;

            // We know that the decl bytes are correct so do them now
            TRawStr::CopyStr(m_achCharBuf, CIDXML_EntitySpooler::pszXMLDeclString);
            pc1Src += c4MyDeclBytes;
            pchOut += CIDXML_EntitySpooler::c4DeclStrLen;

            // Now do the rest of the Decl
            while (pc1Src < pc1End)
            {
                const tCIDLib::TCh chCur = TEBCDIC037Converter::chDecodeOne(*pc1Src++);

                // Store the char and break out if its our target
                *pchOut++ = chCur;
                if (chCur == kCIDLib::chGreaterThan)
                    break;
            }

            // Update our indexes, which are the same for this encoding
            m_c4CharBufCount = pchOut - m_achCharBuf;
            m_c4RawBufInd = m_c4CharBufCount;
        }
    }
     else
    {
        #if CID_DEBUG_ON
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcEnt_BadAutoEncoding
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , m_strEncoding
        );
        #endif
    }
}


tCIDLib::TBoolean TXMLEntSpooler::bReloadCharBuf()
{
    //
    //  If there are any remaining chars in the buffer, then we have to move
    //  them down to the bottom of the buffer first. In most cases there won't
    //  be, but there are some circumstances where its advantageous to reload
    //  before its empty (but there will never be many chars left when this
    //  is done.)
    //
    const tCIDLib::TCard4 c4Remainder = m_c4CharBufCount - m_c4CharBufInd;
    if (c4Remainder)
    {
        // Get a target pointer at the bottom of the buffer
        tCIDLib::TCh* pszTarget = m_achCharBuf;

        // And copy down the remainder chars
        for (tCIDLib::TCard4 c4Index = m_c4CharBufInd; c4Index < m_c4CharBufCount; c4Index++)
            *pszTarget++ = m_achCharBuf[c4Index];
    }

    // Set the char count to the remainder char count and reset the index
    m_c4CharBufCount = c4Remainder;
    m_c4CharBufInd = 0;

    //
    //  If the raw buffer index has hit the end, then we need to try to reload
    //  raw buffer.
    //
    //  NOTE:   In order to avoid extra work, we actually see if it's within
    //          16 bytes of being full. Otherwise, we might have some bytes
    //          but not enough for a whole character (in a multi-byte encoding)
    //          and below we'd have to do this below after getting no bytes
    //          back to see if we are really at the end or not. So this way,
    //          we know we always have anough for a char if not at the EOI.
    //          No encoding is going to use 16 bytes per char.
    //
    if (m_c4RawBufInd + 16 > m_c4RawBufCount)
    {
        ReloadRawData();

        //
        //  If we didn't get any more data, then fail. But, fault in a trailing
        //  space if required and not done yet.
        //
        if (!m_c4RawBufCount)
        {
            if ((m_eType == tCIDXML::EEntityTypes::Parameter) && !m_bRefFromLiteral)
            {
                if (!m_bSpaceAdded)
                {
                    m_bSpaceAdded = kCIDLib::True;
                    m_achCharBuf[m_c4CharBufCount++] = kCIDLib::chSpace;
                    return kCIDLib::True;
                }
            }
            return kCIDLib::False;
        }
    }

    // If we don't have a text converter yet, then create it
    if (!m_ptcvtDecode)
    {
        //
        //  There was no Decl, so our forced or autosensed encoding must
        //  be correct. So we can go ahead and create the transcoder for
        //  the encoding. If there was one, then we will put off the
        //  creation of the transcoder until the decl is processed.
        //
        m_ptcvtDecode = facCIDEncode().ptcvtMake
        (
            m_strEncoding
            , m_bIgnoreBadChars ? tCIDLib::ETCvtActs::Replace
                                : tCIDLib::ETCvtActs::StopThenThrow
        );
        if (!m_ptcvtDecode)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcEnt_UnknownEncoding
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , m_strEncoding
            );
        }
    }

    // Calculate the chars we can now load up max
    const tCIDLib::TCard4 c4MaxChars = kCIDXML::c4CharBufSize - m_c4CharBufCount;

    //
    //  Lets ask the transcoder to transcode another block of text for us.
    //  We give him our calculate max chars and all of the raw data that we
    //  have in the buffer.
    //
    tCIDLib::TCard4 c4BytesEaten;
    tCIDLib::TCard4 c4CharsDone;
    c4BytesEaten = m_ptcvtDecode->c4ConvertFrom
    (
        &m_ac1RawBuf[m_c4RawBufInd]
        , m_c4RawBufCount - m_c4RawBufInd
        , &m_achCharBuf[m_c4CharBufCount]
        , c4MaxChars
        , c4CharsDone
    );

    if (c4CharsDone)
    {
        m_c4CharBufCount += c4CharsDone;
        m_c4RawBufInd += c4BytesEaten;
    }
    return (m_c4CharBufCount != 0);
}


tCIDLib::TVoid TXMLEntSpooler::ReloadRawData()
{
    //
    //  Calculate how many bytes of data are left in the buffer, since it can
    //  be reloaded before its totally emptied (to avoid the overhead of
    //  transcoding small remainder bits.)
    //
    const tCIDLib::TCard4 c4Remaining = m_c4RawBufCount - m_c4RawBufInd;

    //
    //  If its totally empty, then we can do a quick and easy version that
    //  does not require any calculations.
    //
    if (!c4Remaining)
    {
        // Just read as much as will fit into the buffer
        m_c4RawBufCount = m_pstrmSrc->c4ReadRawBuffer
        (
            m_ac1RawBuf
            , kCIDXML::c4RawBufSize
            , tCIDLib::EAllData::OkIfNotAll
        );
    }
     else
    {
        //
        //  Oh well, there is some data left in the buffer. So we have to move
        //  it down to the start of the buffer.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Remaining; c4Index++)
            m_ac1RawBuf[c4Index] = m_ac1RawBuf[m_c4RawBufInd + c4Index];

        //
        //  And then read more data into the buffer after the end of the left
        //  over data. We then add back into the bytes read the bytes that were
        //  left over, to give the total bytes in the buffer now.
        //
        m_c4RawBufCount = m_pstrmSrc->c4ReadRawBuffer
        (
            &m_ac1RawBuf[c4Remaining]
            , kCIDXML::c4RawBufSize - c4Remaining
            , tCIDLib::EAllData::OkIfNotAll
        );

        m_c4RawBufCount += c4Remaining;
    }

    // Either way, we reset the buffer index to the start
    m_c4RawBufInd = 0;

    // If we didn't get anything, the set exhausted flag
    if (!m_c4RawBufCount)
        m_bDataExhausted = kCIDLib::True;
}
