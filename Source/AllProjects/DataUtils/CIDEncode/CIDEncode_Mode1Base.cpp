//
// FILE NAME: CIDEncode_Mode1Base.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/1999
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
//  This file implements the TMode1EncodingBase, which is the base class
//  for all of the 'mode 1' encodings.
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
#include    "CIDEncode_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros.
// ---------------------------------------------------------------------------
RTTIDecls(TMode1EncodingBase,TTextConverter)



// ---------------------------------------------------------------------------
//  TMode1EncodingBase: Destructor
// ---------------------------------------------------------------------------
TMode1EncodingBase::~TMode1EncodingBase()
{
    // We don't own the arrays
}


// ---------------------------------------------------------------------------
//  TMode1EncodingBase: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  This tests is provided to make tests easier. Since all mode 1 encodings
//  derive from this class, and they all use tables, we can do some testing
//  on behalf of all of them. The test framework tests invoke this method
//  for that purpose.
//
tCIDLib::TBoolean TMode1EncodingBase::bDoTests(TString& strError)
{
    //
    //  We just do this one character at a time, looping through each source
    //  character.
    //
    for (tCIDLib::TCard4 c4Ext = 0; c4Ext < 256; c4Ext++)
    {
        //
        //  If this one isn't used, skip it. But make sure it's not in
        //  the from map, since it shouldn't be.
        //
        if (m_achToWide[c4Ext] == 0xFFFF)
        {
            for (tCIDLib::TCard4 c4FIndex = 0; c4FIndex < m_c4FromMapSize; c4FIndex++)
            {
                if (m_acmFromWide[c4FIndex].c1Byte == c4Ext)
                {
                    strError = L"Code point ";
                    strError.AppendFormatted(c4Ext, tCIDLib::ERadices::Hex);
                    strError.Append(L" was marked unused but was in the from map of encoding ");
                    strError.Append(strEncodingName());
                    return kCIDLib::False;
                }
            }

            continue;
        }

        //
        //  So we look up the entry that represents the internal character
        //  that the current external char mapped to.
        //
        tCIDLib::TCard4 c4Res;
        if (!TArrayOps::bSearch<tCIDEncode::TCharMap, tCIDLib::TCh>
        (
            m_acmFromWide
            , m_achToWide[c4Ext]
            , c4Res
            , m_c4FromMapSize
            , &eCharMapComp))
        {
            strError = L"The Unicode point for code point ";
            strError.AppendFormatted(c4Ext, tCIDLib::ERadices::Hex);
            strError.Append(L" was not found in the from map of encoding ");
            strError.Append(strEncodingName());
            return kCIDLib::False;
        }

        // And make sure it maps back to the original byte
        if (m_acmFromWide[c4Res].c1Byte != c4Ext)
        {
            strError = L"Code point ";
            strError.AppendFormatted(c4Ext, tCIDLib::ERadices::Hex);
            strError.Append(L" did not round trip for encoding ");
            strError.Append(strEncodingName());
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::EBaseTextFmts TMode1EncodingBase::eBaseFmt() const
{
    return tCIDLib::EBaseTextFmts::SingleByte;
}


tCIDLib::TVoid TMode1EncodingBase::Reset()
{
    // This is a no-op for us
}


// ---------------------------------------------------------------------------
//  TMode1EncodingBase: Hidden Constructors and Operators
// ---------------------------------------------------------------------------
TMode1EncodingBase
::TMode1EncodingBase(   const   TString&                    strEncodingName
                        , const tCIDLib::TCh* const         achToWide
                        , const tCIDEncode::TCharMap* const acmFromWide
                        , const tCIDLib::TCard4             c4FromSize) :

    TTextConverter(strEncodingName)
    , m_achToWide(achToWide)
    , m_acmFromWide(acmFromWide)
    , m_c4FromMapSize(c4FromSize)
{
}

TMode1EncodingBase::TMode1EncodingBase(const TMode1EncodingBase& tcvtToCopy) :

    TTextConverter(tcvtToCopy)
    , m_achToWide(tcvtToCopy.m_achToWide)
    , m_acmFromWide(tcvtToCopy.m_acmFromWide)
    , m_c4FromMapSize(tcvtToCopy.m_c4FromMapSize)
{
}

TMode1EncodingBase&
TMode1EncodingBase::operator=(const TMode1EncodingBase& tcvtToAssign)
{
    if (this != &tcvtToAssign)
    {
        TParent::operator=(tcvtToAssign);
        m_achToWide     = tcvtToAssign.m_achToWide;
        m_acmFromWide   = tcvtToAssign.m_acmFromWide;
        m_c4FromMapSize = tcvtToAssign.m_c4FromMapSize;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMode1EncodingBase: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TMode1EncodingBase::c4BlockFrom(const   tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       tCIDLib::TCh* const     pszToFill
                                , const tCIDLib::TCard4         c4MaxChars
                                ,       tCIDLib::TCard4&        c4OutChars
                                ,       tCIDLib::TBoolean&      bStop)
{
    const tCIDLib::TCh          chRep = chRepChar();
    const tCIDLib::ETCvtActions eAct = eErrorAction();
    bStop = kCIDLib::False;

    //
    //  There is a one to one correspondence, so the bytes we convert is the
    //  smaller of the available bytes and the max chars.
    //
    const tCIDLib::TCard4 c4Actual = tCIDLib::MinVal(c4SrcBytes, c4MaxChars);

    //
    //  Just look up the current character in the first map. If it maps to
    //  0xFFFF, then its not a valid out encoding char so we must use the
    //  replacement char or throw. Else we take the value at that index.
    //
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Actual; c4Index++)
    {
        // Map the byte to its equivalent wide char
        const tCIDLib::TCh chCur = m_achToWide[pc1Src[c4Index]];

        if (chCur == 0xFFFF)
        {
            if ((eAct == tCIDLib::ETCvtActions::StopThenThrow) && c4Index)
            {
                bStop = kCIDLib::True;
                break;
            }

            if (eAct == tCIDLib::ETCvtActions::Replace)
            {
                pszToFill[c4Index] = chRep;
            }
             else
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcTCvt_BadSource
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , strEncodingName()
                );
            }
        }
         else
        {
            pszToFill[c4Index] = chCur;
        }
    }

    // Return the bytes we processed and chars we output
    c4OutChars = c4Index;
    return c4Index;
}


tCIDLib::TCard4
TMode1EncodingBase::c4BlockTo(  const   tCIDLib::TCh* const     pszSrc
                                , const tCIDLib::TCard4         c4SrcChars
                                ,       tCIDLib::TCard1* const  pc1ToFill
                                , const tCIDLib::TCard4         c4MaxBytes
                                ,       tCIDLib::TCard4&        c4OutBytes
                                ,       tCIDLib::TBoolean&      bStop)
{
    //
    //  There is a one to one correspondence, so the bytes we convert is the
    //  smaller of the available chars and the max bytes.
    //
    const tCIDLib::TCard4 c4Actual = tCIDLib::MinVal(c4SrcChars, c4MaxBytes);
    bStop = kCIDLib::False;

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Actual; c4Index++)
    {
        // Get the current source char to convert
        const tCIDLib::TCh chCur = pszSrc[c4Index];

        //
        //  Do a binary search for the wide char in the second map. If its
        //  found, then pull out its associated target encoding char and use
        //  it.
        //
        tCIDLib::TCard4 c4Res;
        if (TArrayOps::bSearch<tCIDEncode::TCharMap, tCIDLib::TCh>
        (
            m_acmFromWide
            , chCur
            , c4Res
            , m_c4FromMapSize
            , &eCharMapComp))
        {
            pc1ToFill[c4Index] = m_acmFromWide[c4Res].c1Byte;
            continue;
        }

        if ((eErrorAction() == tCIDLib::ETCvtActions::StopThenThrow) && c4Index)
        {
            bStop = kCIDLib::True;
            break;
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTCvt_Unrepresentable
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(tCIDLib::TCard4(chCur))
            , strEncodingName()
        );
    }

    // Return the chars we processed and the bytes processed
    c4OutBytes = c4Index;
    return c4Index;
}


