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
            const   TStringView&            strvToTest
        );

        static tCIDXML::EErrTypes eTypeOfError
        (
            const   tCIDLib::TErrCode       errcCode
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDXML();

        TFacCIDXML(const TFacCIDXML&) = delete;
        TFacCIDXML(TFacCIDXML&&) = delete;

        ~TFacCIDXML();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDXML& operator=(const TFacCIDXML&) = delete;
        TFacCIDXML& operator=(TFacCIDXML&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid EscapeFor
        (
            const   TStringView&            strvInText
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
            , const TStringView&            strvOtherChars
            , const tCIDLib::TBoolean       bSkipLTSpace = kCIDLib::False
        );

        tCIDLib::TVoid EscapeFor
        (
            const   tCIDLib::TCh* const     pszStart
            , const tCIDLib::TCard4         c4Count
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
            , const TStringView&            strvOtherChars
        );

        tCIDLib::TVoid EscapeFor
        (
            const   TStringView&            strvInText
            ,       TString&                strOutText
            , const tCIDXML::EEscTypes      eType
            , const TStringView&            strvOtherChars
            , const tCIDLib::TBoolean       bSkipLTSpace = kCIDLib::False
        );

        tCIDLib::TVoid EscapeFor
        (
            const   tCIDLib::TCh            chInText
            ,       TTextOutStream&         strmOut
            , const tCIDXML::EEscTypes      eType
            , const TStringView&            strvOtherChars
        );

        tCIDLib::TVoid FormatAttr
        (
                    TTextOutStream&         strmOut
            , const TStringView&            strvName
            , const TStringView&            strvValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid FormatAttrNL
        (
                    TTextOutStream&         strmOut
            , const TStringView&            strName
            , const TStringView&            strValue
            , const tCIDLib::TBoolean       bSpaceBefore = kCIDLib::False
        );

        tCIDLib::TVoid NLFormatAttr
        (
                    TTextOutStream&         strmOut
            , const TStringView&            strvName
            , const TStringView&            strvValue
            , const tCIDLib::TBoolean       bSpaceAfter = kCIDLib::False
        );

        tCIDLib::TVoid FormatTextElem
        (
                    TTextOutStream&         strmOut
            , const TStringView&            strElemName
            , const TStringView&            strElemText
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDXML,TFacility)
};

#pragma CIDLIB_POPPACK


