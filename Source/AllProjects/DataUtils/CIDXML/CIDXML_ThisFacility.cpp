//
// FILE NAME: CIDXML_ThisFacility.Cpp
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
//  This file implements TFacCIDXML class.
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
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDXML,TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDXML
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDXML: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacCIDXML::bIsAllSpaces(const TStringView& strvToTest)
{
    const tCIDLib::TCh*     pszBuf = strvToTest.pszBuffer();
    const tCIDLib::TCard4   c4Len  = strvToTest.c4Length();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Len; c4Index++)
    {
        if (!TXMLCharFlags::bIsSpace(*pszBuf))
            return kCIDLib::False;
        pszBuf++;
    }
    return kCIDLib::True;
}


tCIDXML::EErrTypes TFacCIDXML::eTypeOfError(const tCIDLib::TErrCode errcCode)
{
    //
    //  Decide what type of error it is. There are marker values in the
    //  error ids to do this.
    //
    if ((errcCode > kXMLErrs::errcXMLW_Start)
    &&  (errcCode < kXMLErrs::errcXMLW_End))
    {
        return tCIDXML::EErrTypes::Warning;
    }
     else if ((errcCode > kXMLErrs::errcXMLE_Start)
          &&  (errcCode < kXMLErrs::errcXMLE_End))
    {
        return tCIDXML::EErrTypes::Error;
    }
     else if ((errcCode > kXMLErrs::errcXMLV_Start)
          &&  (errcCode < kXMLErrs::errcXMLV_End))
    {
        return tCIDXML::EErrTypes::Validation;
    }
    return tCIDXML::EErrTypes::Unknown;
}


// ---------------------------------------------------------------------------
//  TFacCIDXML: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDXML::TFacCIDXML() :

    TFacility
    (
        L"CIDXML"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDXML::~TFacCIDXML()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDXML: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  XML, being a markup language, cannot allow markup characters to be inside
//  various types of content, so we provide this method to help programs do
//  the required escaping of prohibited characters by replacing them with
//  character references.
//
//  The use an enum parameter to tell us the context that the text is being
//  formatted in.
//
tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   TStringView&        strvInText
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType
                        , const TStringView&        strvOtherChars
                        , const tCIDLib::TBoolean   bSkipLTSpace)
{
    //
    //  Get the start/end pointers. If skipping leading/trailing space we have to probe
    //  the source text for those points.
    //
    const tCIDLib::TCh* pszStart = nullptr;
    tCIDLib::TCard4 c4Count = kCIDLib::c4MaxCard;
    if (bSkipLTSpace)
    {
        //  If there is no non-space text, we are done
        const tCIDLib::TCh* pszEnd = nullptr;
        if (!strvInText.bFindTextBody(pszStart, pszEnd))
            return;

        c4Count = pszEnd - pszStart;
    }
    else
    {
        pszStart = strvInText.pszBuffer();

        //
        //  If we have the length already, get the end, else leave null and we will just
        //  break out on the null.
        //
        if (strvInText.bHaveLength())
            c4Count = strvInText.c4Length();
    }

    EscapeFor(pszStart, c4Count, strmOut, eType, strvOtherChars);
}


// Count can be max card and we'll just look for the null
tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   tCIDLib::TCh* const pszStart
                        , const tCIDLib::TCard4     c4Count
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType
                        , const TStringView&        strvOtherChars)
{
    const tCIDLib::TCh* pszCur = pszStart;
    const tCIDLib::TCh* pszEnd = (c4Count == kCIDLib::c4MaxCard) ? nullptr : pszStart + c4Count;

    // Loop at worst till we hit the null (or break out below at the end)
    TString strCharRef;
    while (*pszCur)
    {
        const tCIDLib::TCh chCur = *pszCur++;

        tCIDLib::TCard4 c4EscLen = 0;
        const tCIDLib::TCh* pszEsc = nullptr;

        switch(chCur)
        {
            case kCIDLib::chApostrophe :
                pszEsc = L"&apos;";
                break;

            case kCIDLib::chQuotation :
                pszEsc = L"&quot;";
                break;

            case kCIDLib::chAmpersand :
                pszEsc = L"&amp;";
                break;

            case kCIDLib::chLessThan :
                pszEsc = L"&lt;";
                break;

            default :
                break;
        };

        if (!pszEsc)
        {
            if (strvOtherChars.bContainsChar(chCur))
            {
                strCharRef = L"&#x";
                strCharRef.AppendFormatted(static_cast<tCIDLib::TCard4>(chCur), tCIDLib::ERadices::Hex);
                strCharRef.Append(kCIDLib::chSemiColon);
                pszEsc = strCharRef.pszBuffer();
            }
        }

        // If we got an escape, write that out, else write out the character
        if (pszEsc)
            strmOut.WriteChars(pszEsc, c4EscLen);
        else
            strmOut << chCur;

        // If we have an end and have reached it, we are done
        if (pszEnd && (pszCur >= pszEnd))
            break;
    }
}


tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   TStringView&        strvInText
                        ,       TString&            strOutText
                        , const tCIDXML::EEscTypes  eType
                        , const TStringView&        strvOtherChars
                        , const tCIDLib::TBoolean   bSkipLTSpace)
{
    //
    //  Set up an output stream on the output text, and call the other
    //  version. Reset it so that we start outputing at the start.
    //
    TTextStringOutStream strmOut(&strOutText);
    strmOut.Reset();
    EscapeFor(strvInText, strmOut, eType, strvOtherChars, bSkipLTSpace);
}


tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   tCIDLib::TCh        chInText
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType
                        , const TStringView&        strvOtherChars)
{
    const tCIDLib::TCh* pszEsc = nullptr;
    TString strCharRef;
    switch(eType)
    {
        case tCIDXML::EEscTypes::Attribute :
        {
            if (chInText == kCIDLib::chApostrophe)
                pszEsc = L"&apos;";
            else if (chInText == kCIDLib::chQuotation)
                pszEsc = L"&quot;";
            else if (chInText == kCIDLib::chAmpersand)
                pszEsc = L"&amp;";
            else if (chInText == kCIDLib::chLessThan)
                pszEsc = L"&lt;";
            break;
        }

        case tCIDXML::EEscTypes::ElemText :
        {
            if (chInText == kCIDLib::chLessThan)
                pszEsc = L"&lt;";
            else if (chInText == kCIDLib::chAmpersand)
                pszEsc = L"&amp;";
            else if (chInText == kCIDLib::chQuotation)
                pszEsc = L"&quot;";
            break;
        }

        default :
        {
            // <TBD> Throw something
            break;
        }
    };

    if (!pszEsc)
    {
        if (strvOtherChars.bContainsChar(chInText))
        {
            strCharRef = L"&#x";
            strCharRef.AppendFormatted(static_cast<tCIDLib::TCard4>(chInText), tCIDLib::ERadices::Hex);
            strCharRef.Append(kCIDLib::chSemiColon);
            pszEsc = strCharRef.pszBuffer();
        }
    }

    if (pszEsc)
        strmOut << pszEsc;
    else
        strmOut << chInText;
}



//
//  A helper that adds onto the escaping mechanism above and does the whole
//  job of formatting out an attribute, by putting out the name the equal
//  size and the quoted and escaped value.
//
tCIDLib::TVoid
TFacCIDXML::FormatAttr(          TTextOutStream&    strmOut
                        , const TStringView&        strvName
                        , const TStringView&        strvValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    if (bSpaceBefore)
        strmOut << L' ';
    strmOut << strvName << L"=\"";
    EscapeFor(strvValue, strmOut, tCIDXML::EEscTypes::Attribute, TString::strEmpty(), kCIDLib::False);
    strmOut << L"\"";
}


//
//  Same as above, but outputs a new line before or after foramtting out the
//  attribute text.
//
tCIDLib::TVoid
TFacCIDXML::FormatAttrNL(       TTextOutStream&     strmOut
                        , const TStringView&        strvName
                        , const TStringView&        strvValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    if (bSpaceBefore)
        strmOut << L' ';
    strmOut << strvName << L"=\"";
    EscapeFor(strvValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute, TString::strEmpty(), kCIDLib::False);
    strmOut << L"\"";

    // And the put out a new line
    strmOut << kCIDLib::NewLn;
}


tCIDLib::TVoid
TFacCIDXML::NLFormatAttr(       TTextOutStream&     strmOut
                        , const TStringView&        strvName
                        , const TStringView&        strvValue
                        , const tCIDLib::TBoolean   bSpaceAfter)
{
    // Put out a new line
    strmOut << kCIDLib::NewLn;

    strmOut << strvName << L"=\"";
    EscapeFor(strvValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute, TString::strEmpty(), kCIDLib::False);
    strmOut << L"\"";

    if (bSpaceAfter)
        strmOut << L' ';
}


//
//  Handles the common scenario of outputting an element that surrounds
//  some text that needs to be escaped.
//
tCIDLib::TVoid
TFacCIDXML::FormatTextElem(         TTextOutStream&     strmOut
                            , const TStringView&        strvElemName
                            , const TStringView&        strvElemText)
{
    strmOut << L'<' << strvElemName << L'>';
    EscapeFor(strvElemText, strmOut, tCIDXML::EEscTypes::ElemText, TString::strEmpty(), kCIDLib::False);
    strmOut << L"</" << strvElemName << L'>';
}

