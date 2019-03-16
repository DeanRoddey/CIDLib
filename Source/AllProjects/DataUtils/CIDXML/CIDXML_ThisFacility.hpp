//
// FILE NAME: CIDXML_ThisFacility.hpp
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
//  This is the header file for the CIDXML_ThisFacility.Cpp file. This
//  file implements the facility class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TFacCIDXML
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDXMLEXP TFacCIDXML : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsAllSpaces
        (
            const   TString&                strToTest
        );

        static tCIDXML::EErrTypes eTypeOfError
        (
            const   tCIDLib::TErrCode       errcCode
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDXML();

        ~TFacCIDXML();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid EscapeFor
        (
            const   tCIDLib::TCh* const     pszInText
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
        );

        tCIDLib::TVoid EscapeFor
        (
            const   TString&                strInText
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
        );

        tCIDLib::TVoid EscapeFor
        (
            const   tCIDLib::TCh* const     pszInText
            ,       TString&                strOutText
            , const tCIDXML::EEscTypes      eType
        );

        tCIDLib::TVoid EscapeFor
        (
            const   TString&                strInText
            ,       TString&                strOutText
            , const tCIDXML::EEscTypes      eType
        );

        tCIDLib::TVoid EscapeFor
        (
            const   tCIDLib::TCh            chInText
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
        );

        tCIDLib::TVoid FormatAttr
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TCh* const     pszName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid FormatAttr
        (
                    TTextOutStream&         strmOut
            , const TString&                strName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid FormatAttrNL
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TCh* const     pszName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid FormatAttrNL
        (
                    TTextOutStream&         strmOut
            , const TString&                strName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid NLFormatAttr
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TCh* const     pszName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceAfter = kCIDLib::False
        );

        tCIDLib::TVoid NLFormatAttr
        (
                    TTextOutStream&         strmOut
            , const TString&                strName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bSpaceAfter = kCIDLib::False
        );

        tCIDLib::TVoid FormatTextElem
        (
                    TTextOutStream&         strmOut
            , const TString&                strElemName
            , const TString&                strElemText
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDXML,TFacility)
};

#pragma CIDLIB_POPPACK


inline tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   TString&            strInText
                        ,       TTextOutStream&     strmOut
                        , const tCIDXML::EEscTypes  eType)
{
    EscapeFor(strInText.pszBuffer(), strmOut, eType);
}

inline tCIDLib::TVoid
TFacCIDXML::EscapeFor(  const   TString&            strInText
                        ,       TString&            strOutText
                        , const tCIDXML::EEscTypes  eType)
{
    EscapeFor(strInText.pszBuffer(), strOutText, eType);
}

