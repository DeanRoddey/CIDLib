//
// FILE NAME: CIDLib_URL.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/13/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TURL class, which implements the standard
//  URL processing mechanisms as defined in the relevant RFCs.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDSock_.hpp"



// ---------------------------------------------------------------------------
//  Do our magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TURL,TObject)



namespace CIDSock_URL
{
    // -----------------------------------------------------------------------
    //  Local const data
    //
    //  c2XXXMask
    //      We use an array of 16 bit values to store character attributes.
    //      Each bit in a character's entry indicates some attribute of that
    //      character. These attributes are driven by the RFC that specifies
    //      URIs. These constants are used to make the bits more self
    //      describing.
    //
    //  c4MaxChar
    //      This is the size of the ac2CharTable, which means the maximum
    //      value of a legal URL char. Its the US_ASCII char set so 0x7F is
    //      the biggest valid char.
    //
    //  achProtoChars
    //      The list of characters (other than alphanumerics) that can be
    //      a part of a protocol name.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2DigitMask       = 0x0001;
    const tCIDLib::TCard2   c2AlphaMask       = 0x0002;
    const tCIDLib::TCard2   c2HexDigitMask    = 0x0004;
    const tCIDLib::TCard2   c2ProtoMask       = 0x0008;
    const tCIDLib::TCard2   c2IllegalMask     = 0x0010;
    const tCIDLib::TCard2   c2HostMask        = 0x0020;
    const tCIDLib::TCh      chMaxChar = 0x7F;
    const tCIDLib::TCh      achProtoChars[] =
    {
        L'+', L'.', L'-', kCIDLib::chNull
    };


    // -----------------------------------------------------------------------
    //  Local static data
    //
    //  bTableLoaded
    //  ac2CharTable
    //      This is the character attributes table. Each ASCII char (all that
    //      are allowed directly in URIs) can be used to index this table and
    //      see what attributes it has.
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean   bTableLoaded = kCIDLib::False;
    tCIDLib::TCard2     ac2CharTable[chMaxChar + 1];


    // -----------------------------------------------------------------------
    //  Local static data
    //
    //  ac4DefPorts
    //      This is an array of default ports for each protocol. If a protocol
    //      does not support ports, then its set to zero. Note that it is in
    //      the same order as the EProtos type, so that enum can be used to
    //      index this array.
    //
    //  aeSockProtos
    //      This is an array of socket protocol types for each protocol. We
    //      have to know this in order to create a socket for the protocol.
    //
    //  c2FmtVersion
    //      A format version we write out when streamed, to allow for later
    //      upgrade of the format.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 ac4PortVals[] =
    {
        0       // None
        , 0     // File
        , 80    // HTTP
        , 21    // FTP
        , 25    // Mailto
        , 119   // News
        , 443   // HTTPS
        , 554   // RTSP
        , 322   // RTSPS
        , 5060  // SIP
        , 80    // WS
        , 443   // SS
    };
    const TEArray<tCIDLib::TCard4, tCIDSock::EProtos, tCIDSock::EProtos::Count>
    ac4DefPorts(ac4PortVals);

    const tCIDSock::ESockProtos aeProtoVals[] =
    {
        tCIDSock::ESockProtos::IP       // Place holder
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
        , tCIDSock::ESockProtos::TCP
    };
    const TEArray<tCIDSock::ESockProtos, tCIDSock::EProtos, tCIDSock::EProtos::Count>
    aeSockProtos(aeProtoVals);

    const tCIDLib::TCard2   c2FmtVersion = 1;
}


// ---------------------------------------------------------------------------
//  Local static functions
// ---------------------------------------------------------------------------

//
//  These two methods are used to initialize characters table we use for
//  efficient lookup of characters as valid or not valid in specific situations.
//  The tables are faulted in on first use.
//
static tCIDLib::TVoid SetTableMasks(const   tCIDLib::TCh* const pchTable
                                    , const tCIDLib::TCard2     c2Mask)
{
    // Run through the chars to the null and set mask on each char's entry
    const tCIDLib::TCh* pchTmp = pchTable;
    while (*pchTmp)
        CIDSock_URL::ac2CharTable[*pchTmp++] |= c2Mask;
}

static tCIDLib::TVoid InitCharTable()
{
    TBaseLock lockInit;

    // If someone else beat us to it, return
    if (CIDSock_URL::bTableLoaded)
        return;

    //
    //  Ok, lets run through the lists of characters and set the appropriate
    //  bits for each char we find. We first do some that are simple to do
    //  without hard coded char lists, which are the alpha and digits and the
    //  obviously illegal ones.
    //
    tCIDLib::TCh chCur;
    for (chCur = L'A'; chCur <= L'Z'; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2AlphaMask;
    for (chCur = L'a'; chCur <= L'z'; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2AlphaMask;
    for (chCur = L'0'; chCur <= L'9'; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2DigitMask | CIDSock_URL::c2HexDigitMask;
    for (chCur = L'A'; chCur <= L'F'; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2HexDigitMask;
    for (chCur = L'a'; chCur <= L'f'; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2HexDigitMask;
    for (chCur = 0; chCur < 0x20; chCur++)
        CIDSock_URL::ac2CharTable[chCur] |= CIDSock_URL::c2IllegalMask;
    CIDSock_URL::ac2CharTable[0x7F] |= CIDSock_URL::c2IllegalMask;

    // And now do the ones that are stored in predone tables
    SetTableMasks(CIDSock_URL::achProtoChars, CIDSock_URL::c2ProtoMask);

    // Mark the table loaded last of all
    CIDSock_URL::bTableLoaded = kCIDLib::True;
}




// ---------------------------------------------------------------------------
//   CLASS: TURL
//  PREFIX: url
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TURL: Public, static methods
// ---------------------------------------------------------------------------
TURL& TURL::Nul_TURL()
{
    static TURL* purlNull = nullptr;
    if (!purlNull)
    {
        TBaseLock lockInit;
        if (!purlNull)
            purlNull = new TURL;
    }
    return *purlNull;
}


tCIDLib::TBoolean TURL::bIsFirstProtocolChar(const tCIDLib::TCh chToTest)
{
    if (chToTest > CIDSock_URL::chMaxChar)
        return kCIDLib::False;

    // Fault the table in if needed
    if (!CIDSock_URL::bTableLoaded)
        InitCharTable();
    return (CIDSock_URL::ac2CharTable[chToTest] & CIDSock_URL::c2AlphaMask) != 0;
}


tCIDLib::TBoolean TURL::bIsProtocolChar(const tCIDLib::TCh chToTest)
{
    static const tCIDLib::TCard2 c2ProtoMask = CIDSock_URL::c2AlphaMask
                                               | CIDSock_URL::c2DigitMask
                                               | CIDSock_URL::c2ProtoMask;
    if (chToTest > CIDSock_URL::chMaxChar)
        return kCIDLib::False;

    // Fault the table in if needed
    if (!CIDSock_URL::bTableLoaded)
        InitCharTable();
    return (CIDSock_URL::ac2CharTable[chToTest] & CIDSock_URL::c2ProtoMask) != 0;
}


//
//  Build up a URL from constituent parts, handling encoding of the parts
//  in the process. This allows the caller to avoid the work of doing the
//  encoding work.
//
//  The caller can tell us if we should encode the incoming values, expand
//  them, or take them as is.
//
tCIDLib::TVoid
TURL::BuildURL( const   TString&                    strProto
                , const TString&                    strUser
                , const TString&                    strPassword
                , const TString&                    strHost
                , const tCIDLib::TIPPortNum         ippnHost
                , const TString&                    strPath
                , const TString&                    strParams
                , const TString&                    strFrag
                , const tCIDLib::TKVPCollect* const pcolQuery
                ,       TString&                    strURL
                , const tCIDSock::EExpOpts          eEncOpt)
{
    strURL.Clear();

    const tCIDLib::EAppendOver eOver = tCIDLib::EAppendOver::Append;
    THeapBuf mbufTmp(1024UL);

    // Get the protocol type, whether one we support or not
    const tCIDSock::EProtos eProto = eTextToProto(strProto);

    if (!strProto.bIsEmpty())
    {
        strURL = strProto;
        strURL.Append(L":");

        //
        //  If it's not one of the special protocols that doesn't use the path
        //  then put out the double slashes.
        //
        if ((eProto != tCIDSock::EProtos::MailTo)
        &&  (eProto != tCIDSock::EProtos::SIP))
        {
            strURL.Append(L"//");
        }
    }

    // If user info do that
    if (!strUser.bIsEmpty())
    {
        strURL.Append(strUser);
        if (!strPassword.bIsEmpty())
        {
            strURL.Append(kCIDLib::chColon);
            strURL.Append(strPassword);
        }
        strURL.Append(kCIDLib::chAtSign);
    }

    // Bracket it if it's a numeric IPV6
    if (strHost.bContainsChar(kCIDLib::chColon))
    {
        strURL.Append(kCIDLib::chOpenBracket);
        strURL.Append(strHost);
        strURL.Append(kCIDLib::chCloseBracket);
    }
     else
    {
        strURL.Append(strHost);
    }

    //
    //  If the port isn't zero, and it's not a known protocol, or not the
    //  default of a known protocol, then put the port in explicitly.
    //
    if (ippnHost)
    {
        if ((eProto == tCIDSock::EProtos::None)
        ||  (ippnHost != CIDSock_URL::ac4DefPorts[eProto]))
        {
            strURL.Append(L':');
            strURL.AppendFormatted(ippnHost);
        }
    }

    // If the path doesn't start with a slash, append one
    if (!strPath.bIsEmpty())
    {
        if (strPath.chFirst() != kCIDLib::chForwardSlash)
            strURL.Append(kCIDLib::chForwardSlash);
    }

    if (eEncOpt == tCIDSock::EExpOpts::None)
        strURL.Append(strPath);
    else if (eEncOpt == tCIDSock::EExpOpts::Encode)
        EncodeTo(strPath, strURL, EExpTypes::Path, eOver);
    else
        ExpandTo(strPath, strURL, mbufTmp, EExpTypes::Path, eOver);

    // If there are any parameters, append them
    if (!strParams.bIsEmpty())
    {
        if (strParams[0] != kCIDLib::chSemiColon)
            strURL.Append(kCIDLib::chSemiColon);
        strURL.Append(strParams);
    }

    // If we have any query parameters, do format them out
    if (pcolQuery && !pcolQuery->bIsEmpty())
    {
        strURL.Append(kCIDLib::chQuestionMark);
        FormatQParms(*pcolQuery, strURL, eEncOpt, eOver);
    }

    // If we have a fragment, do that last
    if (!strFrag.bIsEmpty())
    {
        strURL.Append(kCIDLib::chPoundSign);

        if (eEncOpt == tCIDSock::EExpOpts::None)
            strURL.Append(strFrag);
        else if (eEncOpt == tCIDSock::EExpOpts::Encode)
            EncodeTo(strFrag, strURL, EExpTypes::Fragment, eOver);
        else
            ExpandTo(strFrag, strURL, mbufTmp, EExpTypes::Fragment, eOver);
    }
}


//
//  Takes a list of URL encoded key=value items and breaks them out into
//  TKeyValuePair objects, optionally expanding the escaped characters in
//  the process.
//
//  Note that this is for the query part of a GET or the body of a POST.
//  Since we'll get the body of an HTML message as a binary blob, we parse
//  from a stream so that they can set up an input stream over the block
//  (using the Content-Type value to set a text converter on the stream.
//  We also provide a version that takes a string and just creates a temp
//  stream over it.
//
tCIDLib::TCard4
TURL::c4ParseQueryParms(const   TString&                strText
                        ,       tCIDLib::TKVPCollect&   colToFill
                        , const tCIDLib::TBoolean       bExpand
                        , const TURL::EExpTypes         eType)
{
    TTextStringInStream strmSrc(&strText, tCIDLib::EAdoptOpts::NoAdopt);
    return c4ParseQueryParms(strmSrc, colToFill, bExpand, eType);
}

tCIDLib::TCard4
TURL::c4ParseQueryParms(        TTextInStream&          strmSrc
                        ,       tCIDLib::TKVPCollect&   colToFill
                        , const tCIDLib::TBoolean       bExpand
                        , const TURL::EExpTypes         eType)
{
    colToFill.RemoveAll();

    // If already at the EOS, then we got no parms, so we are done
    if (strmSrc.bEndOfStream())
        return 0;

    //
    //  Any & or = or + signs in the actual values must be escaped. If not,
    //  it's the caller's fault and he just gets junk, so we can easily
    //  scan through and find the values.
    //
    tCIDLib::TCh    chCur;
    TString         strKey;
    TString         strValue;
    TString         strKExp;
    TString         strVExp;
    while (!strmSrc.bEndOfStream())
    {
        //
        //  First get the next key, by scanning to the equal sign, or the end
        //  of input.
        //
        strKey.Clear();
        while(!strmSrc.bEndOfStream())
        {
            strmSrc >> chCur;
            if (chCur == kCIDLib::chEquals)
                break;
            strKey.Append(chCur);
        }

        // If at the end of stream, else we need a value
        strValue.Clear();
        if (!strmSrc.bEndOfStream())
        {
            while(!strmSrc.bEndOfStream())
            {
                strmSrc >> chCur;
                if (chCur == kCIDLib::chAmpersand)
                    break;
                strValue.Append(chCur);
            }
        }

        // If they want expansion, then do that, else take them as is
        if (bExpand)
        {
            ExpandTo(strKey, strKExp, eType, tCIDLib::EAppendOver::Overwrite);
            ExpandTo(strValue, strVExp, eType, tCIDLib::EAppendOver::Overwrite);
            colToFill.objAdd(TKeyValuePair(strKExp, strVExp));
        }
         else
        {
            colToFill.objAdd(TKeyValuePair(strKey, strValue));
        }
    }
    return colToFill.c4ElemCount();
}


tCIDSock::EProtos TURL::eTextToProto(const TString& strToXlat)
{
    // Just delegate to the out of line raw string version
    return eTextToProto(strToXlat.pszBuffer());
}

tCIDSock::EProtos TURL::eTextToProto(const tCIDLib::TCh* const pszToXlat)
{
    if (TRawStr::bCompareStrI(pszToXlat, L"file"))
        return tCIDSock::EProtos::File;
    else if (TRawStr::bCompareStrI(pszToXlat, L"http"))
        return tCIDSock::EProtos::HTTP;
    else if (TRawStr::bCompareStrI(pszToXlat, L"ftp"))
        return tCIDSock::EProtos::FTP;
    else if (TRawStr::bCompareStrI(pszToXlat, L"mailto"))
        return tCIDSock::EProtos::MailTo;
    else if (TRawStr::bCompareStrI(pszToXlat, L"news"))
        return tCIDSock::EProtos::News;
    else if (TRawStr::bCompareStrI(pszToXlat, L"https"))
        return tCIDSock::EProtos::HTTPS;
    else if (TRawStr::bCompareStrI(pszToXlat, L"rtsp"))
        return tCIDSock::EProtos::RTSP;
    else if (TRawStr::bCompareStrI(pszToXlat, L"rtsps"))
        return tCIDSock::EProtos::RTSPS;
    else if (TRawStr::bCompareStrI(pszToXlat, L"sip"))
        return tCIDSock::EProtos::SIP;
    else if (TRawStr::bCompareStrI(pszToXlat, L"ws"))
        return tCIDSock::EProtos::WS;
    else if (TRawStr::bCompareStrI(pszToXlat, L"wss"))
        return tCIDSock::EProtos::WSS;

    return tCIDSock::EProtos::None;
}


//
//  Take the source text and encodes it into the target string, based on the
//  rules for the indicated URL expansion type. It can optionally append to
//  the target or overwrite what's there.
//
tCIDLib::TVoid TURL::EncodeTo(  const   tCIDLib::TCh* const     pszSrc
                                ,       TString&                strToFill
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend)
{
    // These are allowed unescaped in the path
    const tCIDLib::TCh* const pszPathChars = L"/:@-._~!$&'()*+,;=";

    // These are allowed unescaped in a fragment
    const tCIDLib::TCh* const pszFragChars = L"/?:@-._~!$&'()*+,;=";


    // If not appending, then clear the target
    if (eAppend == tCIDLib::EAppendOver::Overwrite)
        strToFill.Clear();

    const tCIDLib::TCh* pszCur = pszSrc;
    if (eType == EExpTypes::Path)
    {
        while (*pszCur)
        {
            tCIDLib::TCh chCur = *pszCur++;

            //
            //  If it's alpha numberic, or it's one of the allowed path
            //  special characters, then we can take it as is. Else we have
            //  to encode it.
            //
            if (((chCur < 0x7E) && TRawStr::bIsAlphaNum(chCur))
            ||  TRawStr::pszFindChar(pszPathChars, chCur, 0, kCIDLib::True))
            {
                strToFill.Append(chCur);
            }
             else
            {
                strToFill.Append(L"%");
                strToFill.AppendFormatted
                (
                    tCIDLib::TCard4(chCur), tCIDLib::ERadices::Hex
                );
            }
        }
    }
     else if (eType == EExpTypes::Query)
    {
        while (*pszCur)
        {
            tCIDLib::TCh chCur = *pszCur++;
            if ((chCur < 0x7E)
            &&  (TRawStr::bIsAlphaNum(chCur)
            ||  (chCur == kCIDLib::chHyphenMinus)
            ||  (chCur == kCIDLib::chUnderscore)
            ||  (chCur == kCIDLib::chPeriod)
            ||  (chCur == kCIDLib::chTilde)))
            {
                strToFill.Append(chCur);
            }
             else if (chCur == kCIDLib::chPlusSign)
            {
                strToFill.Append(L"%2B");
            }
             else if (chCur == kCIDLib::chSpace)
            {
                 strToFill.Append(kCIDLib::chPlusSign);
            }
             else
            {
                strToFill.Append(L"%");
                strToFill.AppendFormatted
                (
                    tCIDLib::TCard4(chCur), tCIDLib::ERadices::Hex
                );
            }
        }
    }
     else if ((eType == EExpTypes::Fragment) || (eType == EExpTypes::Params))
    {
        while (*pszCur)
        {
            tCIDLib::TCh chCur = *pszCur++;

            //
            //  If it's alphanumeric, or it's one of the allowed other
            //  fragment chars, we can take it as is, else encode it.
            //
            if (((chCur < 0x7E) && TRawStr::bIsAlphaNum(chCur))
            ||  TRawStr::pszFindChar(pszFragChars, chCur, 0, kCIDLib::True))
            {
                strToFill.Append(chCur);
            }
             else
            {
                strToFill.Append(L"%");
                strToFill.AppendFormatted
                (
                    tCIDLib::TCard4(chCur), tCIDLib::ERadices::Hex
                );
            }
        }
    }
}

tCIDLib::TVoid TURL::EncodeTo(  const   TString&                strSrc
                                ,       TString&                strToFill
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend)
{
    EncodeTo(strSrc.pszBuffer(), strToFill, eType, eAppend);
}


//
//  Take the source text and encodes it into the target string, based on the
//  rules for the indicated URL encoding type. It can optionally append to
//  the target or overwrite what's there.
//
//  This is complicated because we have the URL as Unicode. But, once the
//  characters are expanded, they aren't necessarily UTF-16. So, we have to
//  put the source back into a binary buffer, truncating regular chars to
//  bytes and converting encoded chars to bytes, and then do a trancode to
//  get it back to Unicode. So it's way less efficient than it would
//  otherwise be.
//
//  We have versions that take a buffer to use as a temp, and others that
//  create a local buffer and call the first set. The former are for
//  repetitive expansion. Worst case buffer requirement is the length of
//  the incoming string (since we assume the stuff we have already is legal
//  URL chars that can be byte truncated.) The main versions also take a
//  text encoder to use.
//
tCIDLib::TVoid TURL::ExpandTo(  const   tCIDLib::TCh* const     pszSrc
                                ,       TString&                strToFill
                                ,       TMemBuf&                mbufToUse
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend
                                ,       TTextConverter* const   ptcvtToUse)
{
    const tCIDLib::TCh* pszCur = pszSrc;
    tCIDLib::TCard1 c1Ref;
    tCIDLib::TCard4 c4BufInd = 0;
    while (*pszCur)
    {
        //
        //  If this is a percent sign, then we have to deal with a character
        //  reference, else its just a plain char so copy it over.
        //
        if (*pszCur == kCIDLib::chPercentSign)
        {
            // Move up and look for two digits
            pszCur++;
            if (!*pszCur || !(*pszCur+1))
            {
                facCIDSock().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcURL_BadCharRef
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            // Get the next two chars and check them as been hex digits
            tCIDLib::TCh chDigit1 = *pszCur++;
            tCIDLib::TCh chDigit2 = *pszCur;

            if (!TRawStr::bMakeHexByte(chDigit1, chDigit2, c1Ref))
            {
                facCIDSock().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcURL_BadCharRef
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            mbufToUse[c4BufInd++] = c1Ref;
        }
         else if ((*pszCur == kCIDLib::chPlusSign) && (eType == EExpTypes::Query))
        {
            mbufToUse[c4BufInd++] = 0x20;
        }
         else
        {
            mbufToUse[c4BufInd++] = tCIDLib::TCard1(*pszCur);
        }
        pszCur++;
    }

    //
    //  Ok, we can now transcode that text to the target string. If they
    //  didn't provide a transcoder, we have to gen one up.
    //
    TTextConverter* ptcvtActual = ptcvtToUse;
    TUTF8Converter* ptcvtDefault = 0;
    if (!ptcvtActual)
    {
        ptcvtDefault = new TUTF8Converter;
        ptcvtActual = ptcvtDefault;
    }
    TJanitor<TUTF8Converter> janDefault(ptcvtDefault);

    ptcvtActual->c4ConvertFrom
    (
        mbufToUse.pc1Data(), c4BufInd, strToFill, eAppend
    );
}

tCIDLib::TVoid TURL::ExpandTo(  const   TString&                strSrc
                                ,       TString&                strToFill
                                ,       TMemBuf&                mbufToUse
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend
                                ,       TTextConverter* const   ptcvtToUse)
{
    // Just call the other verison
    ExpandTo(strSrc.pszBuffer(), strToFill, mbufToUse, eType, eAppend, ptcvtToUse);
}


tCIDLib::TVoid TURL::ExpandTo(  const   tCIDLib::TCh* const     pszSrc
                                ,       TString&                strToFill
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend
                                ,       TTextConverter* const   ptcvtToUse)
{
    //
    //  Allocate a buffer and call the main version. The source is zero size,
    //  then empty the target (if not appending, and return.) We cannot
    //  allocate a zero sized buffer.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszSrc);
    if (c4Len)
    {
        THeapBuf mbufToUse(c4Len, c4Len);
        ExpandTo(pszSrc, strToFill, mbufToUse, eType, eAppend, ptcvtToUse);
    }
     else
    {
        // If not appending, then just clear the output string
        if (eAppend == tCIDLib::EAppendOver::Overwrite)
            strToFill.Clear();
    }
}

tCIDLib::TVoid TURL::ExpandTo(  const   TString&                strSrc
                                ,       TString&                strToFill
                                , const EExpTypes               eType
                                , const tCIDLib::EAppendOver    eAppend
                                ,       TTextConverter* const   ptcvtToUse)
{
    //
    //  Allocate a buffer and call the main version. The source is zero size,
    //  then empty the target (if not appending, and return.) We cannot
    //  allocate a zero sized buffer.
    //
    const tCIDLib::TCard4 c4Len = strSrc.c4Length();
    if (c4Len)
    {
        THeapBuf mbufToUse(c4Len, c4Len);
        ExpandTo(strSrc.pszBuffer(), strToFill, mbufToUse, eType, eAppend, ptcvtToUse);
    }
     else
    {
        // If not appending, then just clear the output string
        if (eAppend == tCIDLib::EAppendOver::Overwrite)
            strToFill.Clear();
    }
}


//
//  Given a list of query parameters, format them out in the correct way.
//  The caller can tell us to encode them, expand them, or just take them
//  as is.
//
tCIDLib::TVoid
TURL::FormatQParms( const   tCIDLib::TKVPCollect&   colQParms
                    ,       TString&                strToFill
                    , const tCIDSock::EExpOpts      eEncOpt
                    , const tCIDLib::EAppendOver    eAppend)
{
    if (eAppend == tCIDLib::EAppendOver::Overwrite)
        strToFill.Clear();

    THeapBuf mbufTmp(1024UL);
    TColCursor<TKeyValuePair>* pcursQPs = colQParms.pcursNew();
    TJanitor<TColCursor<TKeyValuePair> > janCurs(pcursQPs);

    tCIDLib::TBoolean bFirst = kCIDLib::True;
    for (; pcursQPs->bIsValid(); pcursQPs->bNext())
    {
        if (bFirst)
            bFirst = kCIDLib::False;
        else
            strToFill.Append(kCIDLib::chAmpersand);

        const TKeyValuePair& kvalCur = pcursQPs->objRCur();


        if (eEncOpt == tCIDSock::EExpOpts::None)
        {
            strToFill.Append(kvalCur.strKey());
        }
         else if (eEncOpt == tCIDSock::EExpOpts::Encode)
        {
            EncodeTo
            (
                kvalCur.strKey()
                , strToFill
                , EExpTypes::Query
                , tCIDLib::EAppendOver::Append
            );
        }
         else
        {

            ExpandTo
            (
                kvalCur.strKey()
                , strToFill
                , mbufTmp
                , EExpTypes::Query
                , tCIDLib::EAppendOver::Append
            );
        }

        // If there's a value, add an equals and the value
        if (!kvalCur.strValue().bIsEmpty())
        {
            strToFill.Append(kCIDLib::chEquals);

            if (eEncOpt == tCIDSock::EExpOpts::None)
            {
                strToFill.Append(kvalCur.strValue());
            }
             else if (eEncOpt == tCIDSock::EExpOpts::Encode)
            {
                EncodeTo
                (
                    kvalCur.strValue()
                    , strToFill
                    , EExpTypes::Query
                    , tCIDLib::EAppendOver::Append
                );
            }
             else
            {
                ExpandTo
                (
                    kvalCur.strValue()
                    , strToFill
                    , mbufTmp
                    , EExpTypes::Query
                    , tCIDLib::EAppendOver::Append
                );
            }
        }
    }
}




// ---------------------------------------------------------------------------
//  TURL: Constuctors and Destructor
// ---------------------------------------------------------------------------
TURL::TURL()
{
    Clear();
}

TURL::TURL( const   TString&                strURLText
            , const tCIDSock::EQualified    eQualified) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Parse the text, enforcing the caller's qualification desires
    Parse(strURLText.pszBuffer(), eQualified);
}

TURL::TURL( const   tCIDLib::TCh* const     pszURLText
            , const tCIDSock::EQualified    eQualified) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Parse the text, enforcing the caller's qualification desires
    Parse(pszURLText, eQualified);
}

TURL::TURL(const TString& strRelTo, const TString& strURLText) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Parse our part first, allowing either type
    Parse(strURLText.pszBuffer(), tCIDSock::EQualified::DontCare);

    //
    //  If relative, then make it relative ot the base part, which must
    //  be fully qualfied.
    //
    if (!bFullyQualified())
    {
        TURL urlTmp(strRelTo, tCIDSock::EQualified::Full);
        MakeRelativeTo(urlTmp);
    }
}

TURL::TURL( const   tCIDLib::TCh* const pszRelTo
            , const tCIDLib::TCh* const pszURLText) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Parse our part first, allowing either type
    Parse(pszURLText, tCIDSock::EQualified::DontCare);

    //
    //  If relative, then make it relative ot the base part, which must
    //  be fully qualfied.
    //
    if (!bFullyQualified())
    {
        TURL urlTmp(pszRelTo, tCIDSock::EQualified::Full);
        MakeRelativeTo(urlTmp);
    }
}

TURL::TURL(const TURL& urlRelTo, const TString& strURLText) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Make sure the base is fully qualified
    if (!urlRelTo.bFullyQualified())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_BaseNotFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Parse our part, which can be either
    Parse(strURLText.pszBuffer(), tCIDSock::EQualified::DontCare);

    // If relative, then make it relative ot the base part
    if (!bFullyQualified())
        MakeRelativeTo(urlRelTo);
}

TURL::TURL( const   TURL&               urlRelTo
            , const tCIDLib::TCh* const pszURLText) :

    m_eProto(tCIDSock::EProtos::None)
    , m_ippnHost(0)
{
    // Make sure the base is fully qualified
    if (!urlRelTo.bFullyQualified())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_BaseNotFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Parse our part, which can be either
    Parse(pszURLText, tCIDSock::EQualified::DontCare);

    // If relative, then make it relative ot the base part
    if (!bFullyQualified())
        MakeRelativeTo(urlRelTo);
}


TURL::~TURL()
{
}


// ---------------------------------------------------------------------------
//  TURL: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TURL::operator==(const TURL& urlSrc) const
{
    if (this != &urlSrc)
    {
        // Do the easy ones first
        if ((m_eProto != urlSrc.m_eProto)
        ||  (m_ippnHost != urlSrc.m_ippnHost))
        {
            return kCIDLib::False;
        }

        // Test the string members
        if ((m_strFragment != urlSrc.m_strFragment)
        ||  (m_strHost != urlSrc.m_strHost)
        ||  (m_strParams != urlSrc.m_strParams)
        ||  (m_strPassword != urlSrc.m_strPassword)
        ||  (m_strPath != urlSrc.m_strPath)
        ||  (m_strUser != urlSrc.m_strUser)
        ||  (m_colQParms != urlSrc.m_colQParms))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TURL::operator!=(const TURL& urlToCompare) const
{
    return !operator==(urlToCompare);
}



// ---------------------------------------------------------------------------
//  TURL: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Add a relative bit to our path. Really only works for files or http
//  type paths.
//
tCIDLib::TVoid TURL::AppendToPath(const TString& strToAppend)
{
    //
    //  If the current path isn't empty, but doesn't end in some sort of
    //  path separator, then append one. We really assume that this is
    //  either a file or http type path.
    //
    if (!m_strPath.bIsEmpty()
    &&  ((m_strPath.chLast() != kCIDLib::chForwardSlash)
    &&   (m_strPath.chLast() != kCIDLib::chBackSlash)))
    {
        if (m_eProto == tCIDSock::EProtos::File)
            m_strPath.Append(kCIDLib::chPathSep);
        else
            m_strPath.Append(kCIDLib::chForwardSlash);
    }
    m_strPath.Append(strToAppend);
}


tCIDLib::TBoolean TURL::bFullyQualified() const
{
    // If it has to have at least a protocol
    return (m_eProto != tCIDSock::EProtos::None);
}


const tCIDLib::TKVPList& TURL::colQParms() const
{
    return m_colQParms;
}


tCIDLib::TVoid TURL::Clear()
{
    m_colQParms.RemoveAll();
    m_eProto = tCIDSock::EProtos::None;
    m_ippnHost = 0;
    m_strFragment.Clear();
    m_strHost.Clear();
    m_strParams.Clear();
    m_strPassword.Clear();
    m_strPath.Clear();
    m_strUser.Clear();
}


// Get or set the protocol of this URL
tCIDSock::EProtos TURL::eProto() const
{
    return m_eProto;
}

tCIDSock::EProtos TURL::eProto(const tCIDSock::EProtos eProto)
{
    m_eProto = eProto;
    return m_eProto;
}


//
//  Get or set the end point of the host for this URL. If the current port is zero
//  and we know the protocol, provide a default.
//
TIPEndPoint TURL::ipepHost(const tCIDSock::EAddrTypes eAddrType) const
{
    tCIDLib::TIPPortNum ippnRet = m_ippnHost;
    if (!ippnRet && (m_eProto != tCIDSock::EProtos::None))
        ippnRet = CIDSock_URL::ac4DefPorts[m_eProto];

    return TIPEndPoint(m_strHost, ippnRet, eAddrType);
}


//
//  We return the literal port set, so that they can know if no explicit port was
//  set. So it can return zero.
//
tCIDLib::TIPPortNum TURL::ippnHost() const
{
    return m_ippnHost;
}


// Create input and output
TBinOutStream*
TURL::pstrmFromURL(         TBinInStream*&          pstrmToFill
                    , const tCIDLib::TEncodedTime   enctWait
                    , const tCIDSock::EAddrTypes    eAddrType) const
{
    //
    //  We optimize for a file protocol on the local host by just creating
    //  a regular binary file input stream.
    //
    TBinOutStream* pstrmRet = 0;
    if ((m_eProto == tCIDSock::EProtos::File)
    &&  ((m_strHost.bCompareI(L"localhost")) || m_strHost.bIsEmpty()))
    {
        //
        //  If its a fully qualified Win32 style path, then we have to throw
        //  away the leading slash by moving the pointer up one.
        //
        const tCIDLib::TCh* pszTmp = m_strPath.pszBuffer();
        if (TRawStr::bIsAlpha(*(pszTmp + 1)))
        {
            if (*(pszTmp + 2) == kCIDLib::chColon)
            {
                if (*(pszTmp + 3) == L'\\')
                    pszTmp++;
            }
        }

        //
        //  Create the output stream, with an 'open if exists', so that it has
        //  to exist.
        //
        pstrmRet = new TBinFileOutStream
        (
            pszTmp
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );

        // And create an input stream on the same file
        pstrmToFill = new TBinFileInStream
        (
            pszTmp
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );
    }
     else
    {
        // Create a server endpoint with the host and port
        TIPEndPoint ipepServer(m_strHost, m_ippnHost, eAddrType);

        //
        //  And now create a client side socket stream for the protocol of
        //  this URL.
        //
        TClientStreamSocket* psockRet = new TClientStreamSocket
        (
            CIDSock_URL::aeSockProtos[m_eProto], ipepServer
        );

        //
        //  Now create a new binary output stream over this socket and
        //  return it. It adopts the socket.
        //
        TBinOutSockStream* pstrmOut(new TBinOutSockStream(psockRet, tCIDLib::EAdoptOpts::Adopt));
        pstrmRet = pstrmOut;

        // And create the input stream from the output stream
        pstrmToFill = new TBinInSockStream(enctWait, *pstrmOut);
    }
    return pstrmRet;
}



//
//  Returns the individual values of the URL in encoded form, which is
//  the format we have it stored in. So we just return the values as is
//  with a little bit of extra stuff.
//
//
tCIDLib::TVoid
TURL::QueryEncodedForm( TString&                strProto
                        , TString&              strUser
                        , TString&              strPassword
                        , TString&              strHost
                        , tCIDLib::TIPPortNum&  ippnHost
                        , TString&              strPath
                        , TString&              strParams
                        , TString&              strFragment
                        , TString&              strQuery) const
{
    if (m_eProto != tCIDSock::EProtos::None)
        strProto = tCIDSock::strAltXlatEProtos(m_eProto);
    else
        strProto.Clear();

    strUser = m_strUser;
    strPassword = m_strPassword;
    strHost = m_strHost;
    ippnHost = m_ippnHost;
    strPath = m_strPath;
    strParams = m_strParams;
    strFragment = m_strFragment;

    //
    //  Format out the query parms. Use a static helper. Tell it take the
    //  values as is since we are passing it encoded values.
    //
    FormatQParms(m_colQParms, strQuery, tCIDSock::EExpOpts::None);
}


//
//  Another variation that is designed for the common needs of things
//  like HTTP clients. We return:
//
//  1. prtocol:/
//  2. user:password@
//  3. host:port
//  4. path;params
//  5. ?queryparms#fragment
//
//  So it's the separated out parts that are relevant for different steps
//  in the use of a URL. It's assumed we'll return the encoded format that
//  we hold, since this is for actual use ont he network.
//
tCIDLib::TVoid
TURL::QueryEncodedForm( TString&                strProto
                        , TString&              strUser
                        , TString&              strHost
                        , TString&              strPath
                        , TString&              strSuffix) const
{
    if (m_eProto != tCIDSock::EProtos::None)
    {
        strProto = tCIDSock::strAltXlatEProtos(m_eProto);
        strProto.Append(L"://");
    }
     else
    {
        strProto.Clear();
    }

    // Output the user and password if there is a user name
    strUser.Clear();
    if (!m_strUser.bIsEmpty())
    {
        strUser.Append(m_strUser);
        if (!m_strPassword.bIsEmpty())
        {
            strUser.Append(L":");
            strUser.Append(m_strPassword);
        }
        strUser.Append(L"@");
    }

    //
    //  We have a special case here of an IPV6 numerical address with
    //  port. In that case, we have to bracket the address, or it can
    //  be impossible to reliably parse.
    //
    strHost.Clear();
    if (!m_strHost.bIsEmpty())
    {
        const tCIDLib::TBoolean bExplicitPort
        (
            m_ippnHost != CIDSock_URL::ac4DefPorts[m_eProto]
        );

        //
        //  If it has any colons in it, then it has to be a numerical
        //  IPV6, so bracket it. Else, take it as is.
        //
        if (m_strHost.bContainsChar(kCIDLib::chColon))
        {
            strHost.Append(L"[");
            strHost.Append(m_strHost);
            strHost.Append(L"]");
        }
         else
        {
            strHost.Append(m_strHost);
        }

        //
        //  If the port number is not the default for the protocol, then
        //  format it out.
        //
        if (bExplicitPort)
        {
            strHost.Append(L':');
            strHost.AppendFormatted(m_ippnHost);
        }
    }

    // If there is any path component, then lets put it out, escaping here
    if (!m_strPath.bIsEmpty())
        strPath = m_strPath;
    else
        strPath.Clear();

    if (!m_strParams.bIsEmpty())
    {
        strPath.Append(kCIDLib::chSemiColon);
        strPath.Append(m_strParams);
    }

    // If there is either a fragment or query, then put them out
    strSuffix.Clear();
    if (!m_colQParms.bIsEmpty())
    {
        strSuffix.Append(L"?");

        //
        //  We append and don't do any expand/encode since they are already
        //  in the required form.
        //
        FormatQParms
        (
            m_colQParms
            , strSuffix
            , tCIDSock::EExpOpts::None
            , tCIDLib::EAppendOver::Append
        );
    }

    if (!m_strFragment.bIsEmpty())
    {
        strSuffix.Append(L"#");
        strSuffix.Append(m_strFragment);
    }
}


//
//  Similar to above, but expands the encoded values we have and hands back
//  those expanded values.
//
tCIDLib::TVoid
TURL::QueryExpandedForm(TString&                strProto
                        , TString&              strUser
                        , TString&              strPassword
                        , TString&              strHost
                        , tCIDLib::TIPPortNum&  ippnHost
                        , TString&              strPath
                        , TString&              strParams
                        , TString&              strFragment
                        , TString&              strQuery) const
{
    if (m_eProto != tCIDSock::EProtos::None)
        strProto = tCIDSock::strAltXlatEProtos(m_eProto);
    else
        strProto.Clear();

    strUser = m_strUser;
    strPassword = m_strPassword;
    strHost = m_strHost;
    ippnHost = m_ippnHost;

    THeapBuf mbufTemp(1024UL);
    ExpandTo
    (
        m_strPath
        , strPath
        , mbufTemp
        , EExpTypes::Path
        , tCIDLib::EAppendOver::Overwrite
    );

    ExpandTo
    (
        m_strFragment
        , strFragment
        , mbufTemp
        , EExpTypes::Fragment
        , tCIDLib::EAppendOver::Overwrite
    );

    ExpandTo
    (
        m_strParams
        , strParams
        , mbufTemp
        , EExpTypes::Params
        , tCIDLib::EAppendOver::Overwrite
    );

    FormatQParms(m_colQParms, strQuery, tCIDSock::EExpOpts::Expand);
}


// Get a copy of the query parms, with keys and value expanded
tCIDLib::TVoid
TURL::QueryExpQParams(tCIDLib::TKVPList& colToFill, const tCIDLib::TBoolean bAppend) const
{
    if (!bAppend)
        colToFill.RemoveAll();

    TString strKey;
    TString strValue;
    const tCIDLib::TCard4 c4Count = m_colQParms.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalSrc = m_colQParms[c4Index];
        TURL::ExpandTo
        (
            kvalSrc.strKey(), strKey, TURL::EExpTypes::Query, tCIDLib::EAppendOver::Overwrite
        );
        TURL::ExpandTo
        (
            kvalSrc.strValue(), strValue, TURL::EExpTypes::Query, tCIDLib::EAppendOver::Overwrite
        );
        colToFill.objAdd(TKeyValuePair(strKey, strValue));
    }
}


//
//  This one provides the URL contents formatted out. We just pass our
//  info to our public helper that builds URLs from the parts. They can
//  ask for the encoded form (which is what we have) or the expanded
//  format. In the latter case we have to expand all the values out first.
//
tCIDLib::TVoid
TURL::QueryText(        TString&            strToFill
                , const tCIDLib::TBoolean   bExpanded
                , const tCIDLib::TBoolean   bInclUserInfo) const
{
    strToFill.Clear();

    //
    //  We can optionally not return username/password info, even if it
    //  is present. So we set these up into temps and pass those into
    //  the URL building helper.
    //
    TString strUser;
    TString strPassword;
    if (bInclUserInfo)
    {
        strUser = m_strUser;
        strPassword = m_strPassword;
    }

    //
    //  Build the URL up. If expanding, tell it to do that, else tell
    //  it to format the values as is (in our encoded form.)
    //
    BuildURL
    (
        tCIDSock::strAltXlatEProtos(m_eProto)
        , strUser
        , strPassword
        , m_strHost
        , m_ippnHost
        , m_strPath
        , m_strParams
        , m_strFragment
        , &m_colQParms
        , strToFill
        , bExpanded ? tCIDSock::EExpOpts::Expand : tCIDSock::EExpOpts::None
    );
}



//
//  Returns the URL from the / forward, which is a commonly required piece
//  of info, with or without any suffix (fragment or query) and optionally
//  expanded.
//
tCIDLib::TVoid TURL::QueryResource(         TString&            strToFill
                                    , const tCIDLib::TBoolean   bExpanded
                                    , const tCIDLib::TBoolean   bWithSuffix) const
{
    const tCIDLib::EAppendOver eOver = tCIDLib::EAppendOver::Append;

    strToFill.Clear();

    // If there is any path component, then lets put it out, expanded if asked
    if (bExpanded)
        ExpandTo(m_strPath, strToFill, EExpTypes::Path, eOver);
    else
        strToFill.Append(m_strPath);

    // If we have any params, append that
    if (!m_strParams.bIsEmpty())
    {
        strToFill.Append(kCIDLib::chSemiColon);
        strToFill.Append(m_strParams);
    }

    //
    //  If there is either a fragment or query and they asked for it, then put
    //  it out, with the correct separator character, expanding if asked.
    //
    if (bWithSuffix)
    {
        if (!m_colQParms.bIsEmpty())
        {
            strToFill.Append(L"?");
            FormatQParms
            (
                m_colQParms
                , strToFill
                , bExpanded ? tCIDSock::EExpOpts::Expand : tCIDSock::EExpOpts::None
                , eOver
            );
        }

        if (!m_strFragment.bIsEmpty())
        {
            strToFill.Append(L"#");
            if (bExpanded)
                ExpandTo(m_strFragment, strToFill, EExpTypes::Fragment, eOver);
            else
                strToFill.Append(m_strFragment);
        }
    }
}


//
//  Reset us to a new URL, possibly relative to some other base URL.
//
tCIDLib::TVoid
TURL::Reset(const TString& strNewText, const tCIDSock::EQualified eQualified)
{
    // Cear us then parse
    Clear();

    Parse(strNewText.pszBuffer(), eQualified);
}

tCIDLib::TVoid TURL::Reset(const TString& strRelTo, const TString& strNewText)
{
    // Cear us first
    Clear();

    // Parse our text, which can be either
    Parse(strNewText.pszBuffer(), tCIDSock::EQualified::DontCare);

    // If relative, then the base has to be full and we become relative to that
    if (!bFullyQualified())
    {
        TURL urlRelTo(strRelTo, tCIDSock::EQualified::Full);
        MakeRelativeTo(urlRelTo);
    }
}

tCIDLib::TVoid TURL::Reset(const TURL& urlRelTo, const TString& strNewText)
{
    // Cear us first
    Clear();

    // Make sure the base is fully qualified
    if (!urlRelTo.bFullyQualified())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_BaseNotFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // And ours can be either
    Parse(strNewText.pszBuffer(), tCIDSock::EQualified::DontCare);

    // If ours is relative, make it relative to the base
    if (!bFullyQualified())
        MakeRelativeTo(urlRelTo);
}

tCIDLib::TVoid TURL::Reset(const tCIDLib::TCh* const pszNewText)
{
    // Cear us first
    Clear();

    // Parse the new text, which must be fully qualified
    Parse(pszNewText, tCIDSock::EQualified::Full);
}

tCIDLib::TVoid TURL::Reset( const   tCIDLib::TCh* const pszRelTo
                            , const tCIDLib::TCh* const pszNewText)
{
    // Cear us first
    Clear();

    // Parse our text, which can be either
    Parse(pszNewText, tCIDSock::EQualified::DontCare);

    // If relative, the base has to be full and we are relative to it
    if (!bFullyQualified())
    {
        // The relative to has to be fully qualified
        TURL urlRelTo(pszRelTo, tCIDSock::EQualified::Full);
        MakeRelativeTo(urlRelTo);
    }
}

tCIDLib::TVoid TURL::Reset( const   TURL&               urlRelTo
                            , const tCIDLib::TCh* const pszNewText)
{
    // Cear us first
    Clear();

    // Make sure the base is fully qualified
    if (!urlRelTo.bFullyQualified())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_BaseNotFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Ours can be either
    Parse(pszNewText, tCIDSock::EQualified::DontCare);

    // If ours is relative, make it relative to the base
    if (!bFullyQualified())
        MakeRelativeTo(urlRelTo);
}


// Return the fragment portion, if any
const TString& TURL::strFragment() const
{
    return m_strFragment;
}


// Return the host portion, if any
const TString& TURL::strHost() const
{
    return m_strHost;
}


// Return the params, if any
const TString& TURL::strParams() const
{
    return m_strParams;
}


// Get the password value, if any
const TString& TURL::strPassword() const
{
    return m_strPassword;
}


// Get/set the path value, if any
const TString& TURL::strPath() const
{
    return m_strPath;
}

const TString& TURL::strPath(const TString& strToSet)
{
    m_strPath = strToSet;
    return m_strPath;
}


// Get the user value, if any
const TString& TURL::strUser() const
{
    return m_strUser;
}


//
//  This allows the caller to set up a URL from the raw values. This lets
//  them set the values without having to pre-encode them, though it can
//  also take pre-encoded values, based on the bEncoded parameter.
//
tCIDLib::TVoid
TURL::Set(  const   tCIDSock::EProtos       eProto
            , const TString&                strUser
            , const TString&                strPass
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnHost
            , const TString&                strPath
            , const TString&                strParams
            , const TString&                strFrag
            , const tCIDLib::TKVPCollect&   colQParms
            , const tCIDLib::TBoolean       bEncoded)
{
    m_eProto = eProto;
    m_ippnHost = ippnHost;
    m_strPassword = strPass;
    m_strHost = strHost;
    m_strUser = strUser;

    // The params can't start with a semicolon since we store it without
    m_strParams = strParams;
    if (!m_strParams.bIsEmpty())
    {
        if (m_strParams.chFirst() == kCIDLib::chSemiColon)
            m_strParams.Cut(0, 1);
    }

    //
    //  For the rest, either just store the values or encode them if they
    //  aren't encoded.
    //
    m_colQParms.RemoveAll();
    if (bEncoded)
    {
        m_strPath = strPath;
        m_strFragment = strFrag;

        TColCursor<TKeyValuePair>* pcursQPs = colQParms.pcursNew();
        TJanitor<TColCursor<TKeyValuePair> > janCurs(pcursQPs);
        for (; pcursQPs->bIsValid(); pcursQPs->bNext())
            m_colQParms.objAdd(pcursQPs->objRCur());
    }
     else
    {
        EncodeTo(strPath, m_strPath, EExpTypes::Path, tCIDLib::EAppendOver::Overwrite);
        EncodeTo(strFrag, m_strFragment, EExpTypes::Fragment, tCIDLib::EAppendOver::Overwrite);


        TColCursor<TKeyValuePair>* pcursQPs = colQParms.pcursNew();
        TJanitor<TColCursor<TKeyValuePair> > janCurs(pcursQPs);
        if (pcursQPs->bIsValid())
        {
            TString strName;
            TString strVal;
            for (; pcursQPs->bIsValid(); pcursQPs->bNext())
            {
                const TKeyValuePair& kvalCur = pcursQPs->objRCur();

                EncodeTo
                (
                    kvalCur.strKey()
                    , strName
                    , EExpTypes::Query
                    , tCIDLib::EAppendOver::Overwrite
                );

                EncodeTo
                (
                    kvalCur.strValue()
                    , strVal
                    , EExpTypes::Query
                    , tCIDLib::EAppendOver::Overwrite
                );

                m_colQParms.objAdd(TKeyValuePair(strName, strVal));
            }
        }
    }
}



// ---------------------------------------------------------------------------
//  TURL: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TURL::FormatTo(TTextOutStream& strmDest) const
{
    //
    //  Format out the full URL, in our native encoded form, and with
    //  use data, then format that out.
    //
    TString strFmt;
    QueryText(strFmt, kCIDLib::False, kCIDLib::True);
    strmDest << strFmt;
}


tCIDLib::TVoid TURL::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDSock_URL::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    //
    //  Stream the text back in into a temp. Allocate it to a size that is
    //  likely to hold most URLs, to avoid having to grow it. But, it will
    //  grow if required.
    //
    TString strTmp(1024UL);
    strmToReadFrom >> strTmp;

    //
    //  And try to parse this as a URL. If we aren't being fooled by being
    //  told to parse from incorrect data, this will work because it could
    //  have only been streamed out from a valid URL to begin with.
    //
    //  Allow it to be partial, since this will allow whatever was streamed
    //  out to be streamed back in.
    //
    Parse(strTmp.pszBuffer(), tCIDSock::EQualified::Partial);

    // And check the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TURL::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  We stream out the full canonical URL. We'll just reparse it when
    //  we stream it back in.
    //
    TString strFmt;
    QueryText(strFmt, kCIDLib::False, kCIDLib::True);

    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDSock_URL::c2FmtVersion
                    << strFmt
                    << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TURL: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TIPPortNum TURL::ippnDefPort() const
{
    return CIDSock_URL::ac4DefPorts[m_eProto];
}


tCIDLib::TVoid TURL::MakeRelativeTo(const TURL& urlRelTo)
{
    // If we have a protocol, then we are done. Else, use his protocol
    if (m_eProto != tCIDSock::EProtos::None)
        return;
    m_eProto = urlRelTo.m_eProto;

    // If we don't have a host, then use his host, user, and password
    if (!m_strHost.bIsEmpty())
        return;
    m_strHost = urlRelTo.m_strHost;
    m_ippnHost = urlRelTo.m_ippnHost;
    m_strUser = urlRelTo.m_strUser;
    m_strPassword = urlRelTo.m_strPassword;

    // If our path is not relative, then we are done
    if (m_strPath.bStartsWith(L"/"))
        return;

    //
    //  Get a pointer to the base of the relative path and then get a pointer to
    //  the last slash character. If there is not one, then there is nothing that
    //  the relative path can contribute and we can go on to the other parts.
    //
    tCIDLib::TCh* pszRelBase = urlRelTo.m_strPath.pszDupBuffer();
    TArrayJanitor<tCIDLib::TCh> janBase(pszRelBase);
    tCIDLib::TCh* pszRelPart = TRawStr::pszFindLastChar(pszRelBase, L'/');

    //
    //  If we have some path content, then lets weave our path into its path.
    //  Else, we take everything else from the parent. Note that params count
    //  as path, however it's a special case, so we handle that one below.
    //
    if (!m_strPath.bIsEmpty())
    {
        //
        //  Work through our path dealing with each . and .. part that we see.
        //  For a '.', we just remove it. For a '..', we remove it and move the
        //  pointer into the relative path back one step.
        //
        while (kCIDLib::True)
        {
            if (m_strPath.bStartsWith(L".."))
            {
                if (m_strPath.bStartsWith(L"../"))
                    m_strPath.Cut(0, 3);
                else
                    m_strPath.Cut(0, 2);
                *pszRelPart = 0;
                pszRelPart = TRawStr::pszFindLastChar(pszRelBase, L'/');
                if (!pszRelPart)
                {
                    // We've gone up further than the relative path can handle
                    facCIDSock().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSockErrs::errcURL_TooManyUps
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }
            }
             else if (m_strPath.bStartsWith(L"."))
            {
                if (m_strPath.bStartsWith(L"./"))
                    m_strPath.Cut(0, 2);
                else
                    m_strPath.Cut(0, 1);
            }
             else
            {
                // Its neither, so we can break out now and make the full path
                break;
            }
        }

        //
        //  Cap off our temp copy of the relative path just past the last
        //  / char we got to. Then prepend this to our path.
        //
        *(pszRelPart + 1) = 0;
        m_strPath.Prepend(pszRelBase);

        //
        //  Note we do not take his fragment info in this case. That only
        //  happens if the relative URL is empty. WE keep our params as
        //  well in this case.
        //
    }
     else if (m_strPath.bIsEmpty() && !m_strParams.bIsEmpty())
    {
        //
        //  We have a path only to the extent that we have parameters. But,
        //  it still means we ignore the bse parms and fragment. We take the
        //  non-relative base path, plus our params.
        //
        *(pszRelPart + 1) = 0;
        m_strPath = pszRelBase;
    }
     else
    {
        //
        //  If we are essentially empty, which at this point means we have
        //  no fragment or query parms, then we take the parent's fragment
        //  if any. Params don't count in this.
        //
        if (m_strFragment.bIsEmpty() && m_colQParms.bIsEmpty())
            m_strFragment = urlRelTo.m_strFragment;

        // If we have no params, take the parent's
        if (m_strParams.bIsEmpty())
            m_strParams = urlRelTo.m_strParams;

        // If we don't have any query, then take our parent's
        if (m_colQParms.bIsEmpty())
            m_colQParms = urlRelTo.m_colQParms;

        // And we take the parent's full path
        m_strPath = pszRelBase;
    }
}


//
//  We parse the incoming text as a URL. The caller can tell us specificall
//  if we should be getting a fully qualified or relative URL and we will
//  check at the end that we got what was indicated or throw. They can also
//  indicate they don't care.
//
//  The parsing here is done based on the suggestions in RFC1088. It's not
//  really conducive to the usual state machine sort of parse.
//
//  NOTE: We have to get fully encoded URL text, else we cannot guarantee
//  that we'll parse it successfully.
//
tCIDLib::TVoid
TURL::Parse(const   tCIDLib::TCh* const     pszURLText
            , const tCIDSock::EQualified    eQualified)
{
    TString strTmp;
    TString strQuery;
    tCIDLib::TCh* pszTmp = 0;

    // Get a copy of the text we can mangle
    tCIDLib::TCh* pszPText = TRawStr::pszReplicate(pszURLText);
    TArrayJanitor<tCIDLib::TCh> janStr(pszPText);

    // We use start/end pointers to eat into the text from either end
    tCIDLib::TCh* pszStart = pszPText;
    tCIDLib::TCh* pszEnd = pszPText + TRawStr::c4StrLen(pszPText);
    CIDAssert(*pszEnd == 0, L"The URL end pointer doesn't point to the null");

    // Just in case, we run the two inwards past any whitespace
    while (pszEnd > pszStart)
    {
        if (!TRawStr::bIsSpace(*(pszEnd - 1)))
            break;
        pszEnd--;
    }

    //
    //  If we only had whitespace, then we failed if they asked for a fully
    //  qualified URL. If it's relative, then we are just the same as the
    //  parent URL.
    //
    if (pszEnd == pszStart)
    {
        if (eQualified == tCIDSock::EQualified::Full)
        {
            facCIDSock().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcURL_EmptyURL
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        // We return with default info (all empty, no proto, etc...)
        return;
    }

    //
    //  Search for the first # chracter. If present, everything after that
    //  is the fragment, and that becomes the new end of the URL.
    //
    pszTmp = TRawStr::pszFindChar(pszStart, kCIDLib::chPoundSign, 0, kCIDLib::True);
    if (pszTmp)
    {
        // Put the end here then move past the pound and take that
        pszEnd = pszTmp;

        pszTmp++;
        m_strFragment = pszTmp;

        // And cap this part off now
        *pszEnd = kCIDLib::chNull;

        // If that's it, then we are done
        if (!(*pszStart))
            return;
    }

    //
    //  Let's see if we have a protocol. We can forward for either a colon
    //  or some other character not legal within a protocol name.
    //
    pszTmp = pszStart;
    while (pszStart < pszEnd)
    {
        const tCIDLib::TCh chCur = *pszTmp;

        if (chCur == kCIDLib::chColon)
        {
            // If it was the first character, can't be a protocol
            if (pszTmp == pszStart)
                break;

            //
            //  We have a protocol. Take everything up to this point as the
            //  the protocol.
            //
            strTmp.Clear();
            while (pszStart < pszTmp)
            {
                strTmp.Append(*pszStart);
                pszStart++;
            }

            // Move the start up past the colon now
            pszStart++;

            // break out now
            break;
        }
         else
        {
            //
            //  If it's not a valid scheme character, we've not hit the colon
            //  yet, so it can't be a scheme.
            //
            if (!TRawStr::bIsAlphaNum(chCur)
            &&  (chCur != kCIDLib::chPeriod)
            &&  (chCur != kCIDLib::chPlusSign)
            &&  (chCur != kCIDLib::chHyphenMinus))
            {
                break;
            }
        }
        pszTmp++;
    }

    // If we got any protocol text, look it up
    if (!strTmp.bIsEmpty())
    {
        m_eProto = eTextToProto(strTmp);
        if (m_eProto == tCIDSock::EProtos::None)
        {
            facCIDSock().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcURL_UnknownProto
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strTmp
            );
        }
    }

    // If that was the end, then we are done
    if (!(*pszStart))
        return;

    //
    //  If we now have a double slash, then it's the network location. We
    //  don't have to worry about going past the end since the first one won't
    //  match if we are already there.
    //
    if ((*pszStart == kCIDLib::chForwardSlash)
    &&  (*(pszStart + 1) == kCIDLib::chForwardSlash))
    {
        //
        //  Everything from here to the next slash (or end) is the network
        //  location. We'll put into the user name field for now, and then
        //  decide if there's any user info when we are done and distribute
        //  the info where appropriate.
        //
        pszStart += 2;
        while (pszStart < pszEnd)
        {
            if (*pszStart == kCIDLib::chForwardSlash)
                break;

            m_strUser.Append(*pszStart);
            pszStart++;
        }
    }

    //
    //  Now let's deal with the query parameters if any. If there is now
    //  a question mark, evertying to the right of that is the query.
    //
    pszTmp = TRawStr::pszFindChar(pszStart, kCIDLib::chQuestionMark, 0, kCIDLib::True);
    if (pszTmp)
    {
        // Move the end here now
        pszEnd = pszTmp;

        // Move past the question mark and take that as the query
        pszTmp++;
        strQuery = pszTmp;

        // And terminate at the new end
        *pszEnd = kCIDLib::chNull;
    }

    //
    //  Now let's deal with any parameters. If there's a a semi colon, then
    //  anything after that are the parameters.
    //
    pszTmp = TRawStr::pszFindChar(pszStart, kCIDLib::chSemiColon, 0, kCIDLib::True);
    if (pszTmp)
    {
        // Move the end here now
        pszEnd = pszTmp;

        // Move past the semi colon and take that as the params
        pszTmp++;
        m_strParams = pszTmp;

        // And terminate at the new end
        *pszEnd = 0;
    }

    //
    //  Anything left is the path if it's a path based protocol, else it's the
    //  network loc info, so fake it into the name value as we did above, so it
    //  gets parsed the same below.
    //
    if ((m_eProto == tCIDSock::EProtos::MailTo)
    ||  (m_eProto == tCIDSock::EProtos::SIP))
    {
        m_strUser = pszStart;
    }
     else
    {
        m_strPath = pszStart;
    }

    //
    //  If they indicated it was supposed to be a fully qualified URL, and
    //  we didn't get a protocol, then it's bad.
    //
    if ((eQualified == tCIDSock::EQualified::Full)
    &&  (m_eProto == tCIDSock::EProtos::None))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_NotFQ
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  And test the other scheme, where they said it had to be a relative
    //  nad we have a prtocol.
    //
    if ((eQualified == tCIDSock::EQualified::Partial)
    &&  (m_eProto != tCIDSock::EProtos::None))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcURL_NotRelative
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Parse out the query parms
    c4ParseQueryParms(strQuery, m_colQParms, kCIDLib::False, EExpTypes::Query);

    //
    //  If we have any network location info, which we stored in the user
    //  member, let's parse that out.
    //
    if (!m_strUser.bIsEmpty())
    {
        //
        //  Search backwards for an @ sign. If we don't find one, then the
        //  whole thing has to be the host, so move it there. If we find one,
        //  move that part to the host and cap it there.
        //
        tCIDLib::TCard4 c4Ofs = 0;
        if (m_strUser.bLastOccurrence(kCIDLib::chAtSign, c4Ofs))
        {
            // Copy out the stuff past the at to the host, and cap it
            m_strUser.CopyOutSubStr(m_strHost, c4Ofs + 1);
            m_strUser.CapAt(c4Ofs);

            // Now if we find a colon, we split that part out as the password
            m_strUser.bSplit(m_strPassword, kCIDLib::chColon, kCIDLib::True);
        }
         else
        {
            // It has to all be the host
            m_strHost = m_strUser;
            m_strUser.Clear();
        }

        //
        //  If it starts and ends with a [ then it's got to be a bracketed
        //  IP address, so we can pull that out and anything after the end
        //  bracket has to be a :port. This is the only way to create
        //  fully unambiguous IPV6 addresses with explicit port indicators.
        //
        //  If no brackets, we just look for the last colon and split it
        //  there. If no colon, the whole thing is what we want.
        //
        if (!m_strHost.bIsEmpty())
        {
            // We'll leave any port part in strTmp, so clear first
            strTmp.Clear();

            if (m_strHost[0] == kCIDLib::chOpenBracket)
            {
                // Search for an end bracket, which must be there
                c4Ofs = 1;
                if (!m_strHost.bLastOccurrence(kCIDLib::chCloseBracket, c4Ofs))
                {
                    facCIDSock().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSockErrs::errcURL_MissingEndBracket
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                m_strHost.Cut(0, 1);
                m_strHost.bSplit(strTmp, kCIDLib::chCloseBracket, kCIDLib::True);

                // If any port port, it must start with a colon
                if (!strTmp.bIsEmpty())
                {
                    if (strTmp[0] != kCIDLib::chColon)
                    {
                        facCIDSock().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kSockErrs::errcURL_BadPortFmt
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    // Remove the port and leave the rest as the port
                    strTmp.Cut(0, 1);
                }
            }
             else
            {
                m_strHost.bSplit(strTmp, kCIDLib::chColon, kCIDLib::True);
            }

            // Either way, if we got any port part, test it
            if (!strTmp.bIsEmpty())
            {
                if (!strTmp.bToCard4(m_ippnHost, tCIDLib::ERadices::Dec))
                {
                    facCIDSock().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSockErrs::errcURL_BadPort
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                        , strTmp
                    );
                }
            }
        }
    }

    //
    //  Looks ok, so store the info. If the port is zero, then set it to
    //  the default port for the protocol. We've already stored the m_eProto
    //  value by the time we call ippnDefPort().
    //
    if (!m_ippnHost)
        m_ippnHost = ippnDefPort();

    // If we have a path component, then simplify it
    if (!m_strPath.bIsEmpty())
        SimplifyPath();
}



//
//  This method will remove redundant parts of the path. In particular it
//  performs the following transformations:
//
//      /./         -> /
//      x/y/../     -> /x/
//
//  If it sees /.., i.e. its trying to move up before the root of the URL,
//  a malformed URL is thrown.
//
tCIDLib::TVoid TURL::SimplifyPath()
{
}


