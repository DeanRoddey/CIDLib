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
tCIDLib::TBoolean TFacCIDXML::bIsAllSpaces(const TString& strToTest)
{
    const tCIDLib::TCh*     pszBuf = strToTest.pszBuffer();
    const tCIDLib::TCard4   c4Len  = strToTest.c4Length();

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
        , tCIDLib::EModTypes::Dll
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
TFacCIDXML::EscapeFor(  const   tCIDLib::TCh* const pszInText
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType)
{
    //
    //  Loop through the in text and for each character, either move it over
    //  to the output or escape it, according to the escape type.
    //
    const tCIDLib::TCard4 c4SrcCount = TRawStr::c4StrLen(pszInText);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcCount; c4Index++)
    {
        const tCIDLib::TCh chCur = pszInText[c4Index];

        tCIDLib::TCh* pszEsc = 0;
        switch(eType)
        {
            case tCIDXML::EEscTypes::Attribute :
            {
                if (chCur == kCIDLib::chApostrophe)
                    pszEsc = L"&apos;";
                else if (chCur == kCIDLib::chQuotation)
                    pszEsc = L"&quot;";
                else if (chCur == kCIDLib::chAmpersand)
                    pszEsc = L"&amp;";
                else if (chCur == kCIDLib::chLessThan)
                    pszEsc = L"&lt;";
                break;
            }

            case tCIDXML::EEscTypes::ElemText :
            {
                if (chCur == kCIDLib::chLessThan)
                    pszEsc = L"&lt;";
                else if (chCur == kCIDLib::chAmpersand)
                    pszEsc = L"&amp;";
                break;
            }

            default :
            {
                // <TBD> Throw something
                break;
            }
        };

        if (pszEsc)
            strmOut << pszEsc;
        else
            strmOut << chCur;
    }
}


tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   tCIDLib::TCh* const pszInText
                        ,       TString&            strOutText
                        , const tCIDXML::EEscTypes  eType)
{
    //
    //  Set up an output stream on the output text, and call the other
    //  version. Reset it so that we start outputing at the start.
    //
    TTextStringOutStream strmOut(&strOutText);
    strmOut.Reset();
    EscapeFor(pszInText, strmOut, eType);
}


tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   tCIDLib::TCh        chInText
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType)
{
    tCIDLib::TCh* pszEsc = 0;
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
            break;
        }

        default :
        {
            // <TBD> Throw something
            break;
        }
    };

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
TFacCIDXML::FormatAttr(         TTextOutStream&     strmOut
                        , const tCIDLib::TCh* const pszName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    if (bSpaceBefore)
        strmOut << L' ';
    strmOut << pszName << L"=\"";
    EscapeFor(strValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute);
    strmOut << L"\"";
}

tCIDLib::TVoid
TFacCIDXML::FormatAttr(          TTextOutStream&    strmOut
                        , const TString&            strName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    if (bSpaceBefore)
        strmOut << L' ';
    strmOut << strName << L"=\"";
    EscapeFor(strValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute);
    strmOut << L"\"";
}


//
//  Same as above, but outputs a new line before or after foramtting out the
//  attribute text.
//
tCIDLib::TVoid
TFacCIDXML::FormatAttrNL(       TTextOutStream&     strmOut
                        , const tCIDLib::TCh* const pszName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    // Call the other version
    FormatAttr(strmOut, pszName, strValue, bSpaceBefore);

    // And the put out a new line
    strmOut << kCIDLib::NewLn;
}

tCIDLib::TVoid
TFacCIDXML::FormatAttrNL(       TTextOutStream&     strmOut
                        , const TString&            strName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceBefore)
{
    if (bSpaceBefore)
        strmOut << L' ';
    strmOut << strName << L"=\"";
    EscapeFor(strValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute);
    strmOut << L"\"";

    // And the put out a new line
    strmOut << kCIDLib::NewLn;
}


tCIDLib::TVoid
TFacCIDXML::NLFormatAttr(       TTextOutStream&     strmOut
                        , const tCIDLib::TCh* const pszName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceAfter)
{
    // Put out a new line
    strmOut << kCIDLib::NewLn;

    // Call the other version
    FormatAttr(strmOut, pszName, strValue);

    if (bSpaceAfter)
        strmOut << L' ';
}

tCIDLib::TVoid
TFacCIDXML::NLFormatAttr(       TTextOutStream&     strmOut
                        , const TString&            strName
                        , const TString&            strValue
                        , const tCIDLib::TBoolean   bSpaceAfter)
{
    // Put out a new line
    strmOut << kCIDLib::NewLn;

    strmOut << strName << L"=\"";
    EscapeFor(strValue.pszBuffer(), strmOut, tCIDXML::EEscTypes::Attribute);
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
                            , const TString&            strElemName
                            , const TString&            strElemText)
{
    strmOut << L'<' << strElemName << L'>';
    EscapeFor(strElemText, strmOut, tCIDXML::EEscTypes::ElemText);
    strmOut << L"</" << strElemName << L'>';
}

