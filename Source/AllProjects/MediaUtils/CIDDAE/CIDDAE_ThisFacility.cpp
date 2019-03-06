//
// FILE NAME: CIDDAE_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TFacCIDDAE class.
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
#include    "CIDDAE_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDDAE,TFacility)



// ---------------------------------------------------------------------------
//  TFacCIDDAE: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDDAE::TFacCIDDAE() :

    TFacility
    (
        L"CIDDAE"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDDAE::~TFacCIDDAE()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDAE: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Formats a runtime in seconds into a string in the form Xh Xm Xs. If there
//  are no minutes or hours it doesn't format those.
//
tCIDLib::TVoid
TFacCIDDAE::FormatRuntime(  const   tCIDLib::TCard4 c4Seconds
                            ,       TString&        strFmt)
{
    tCIDLib::TCard4 c4Left = c4Seconds;
    tCIDLib::TCard4 c4Val;
    strFmt.Clear();

    c4Val = c4Left / 3600;
    if (c4Val)
    {
        strFmt.AppendFormatted(c4Val);
        strFmt.Append(kCIDLib::chLatin_h);
        strFmt.Append(kCIDLib::chSpace);
    }
    c4Left -= c4Val * 3600;

    c4Val = c4Left / 60;
    if (c4Val || !strFmt.bIsEmpty())
    {
        strFmt.AppendFormatted(c4Val);
        strFmt.Append(kCIDLib::chLatin_m);
        strFmt.Append(kCIDLib::chSpace);
    }
    c4Left -= c4Val * 60;

    strFmt.AppendFormatted(c4Left);
    strFmt.Append(kCIDLib::chLatin_s);
}



//
//  This method applies standard rules to turn a track name into a legal
//  file name.
//
//  We use a simple scheme somewhat like URL encoding except that it's Unicode
//  based, so any characters that are not alpha-numeric from a Unicode point
//  of view, or a couple of common other chars like underscore, space, hyphen
//  etc..., we do a %xxxx type encoding.
//
tCIDLib::TVoid TFacCIDDAE::MungeTrackName(TString& strToMunge)
{
    // Get a copy of the original to use to fill back in the target
    const tCIDLib::TCard4 c4Len = strToMunge.c4Length();
    const TString strTmp(strToMunge);

    // Clear the output and put a stream over it
    strToMunge.Clear();
    TTextStringOutStream strmOut(&strToMunge);

    // And then process each character
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Len; c4Index++)
    {
        const tCIDLib::TCh chCur = strTmp[c4Index];

        if (TRawStr::bIsAlphaNum(chCur)
        ||  (chCur == kCIDLib::chSpace)
        ||  (chCur == kCIDLib::chUnderscore)
        ||  (chCur == kCIDLib::chHyphenMinus))
        {
            strmOut << chCur;
        }
         else
        {
            strmOut << kCIDLib::chPercentSign << TTextOutStream::strmfHex4()
                    << tCIDLib::TCard4(chCur) << TTextOutStream::strmfDefault();
        }
    }
    strmOut.Flush();
}

