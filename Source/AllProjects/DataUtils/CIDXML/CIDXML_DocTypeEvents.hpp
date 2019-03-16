//
// FILE NAME: CIDXML_DocTypeEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/1999
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
//  This header defines the abstract interface via which the DTD Validator
//  calls out with DTD markup events. You can implement this interface and
//  plug your implementation into the DTDValidator. When the validator is
//  parsing the DTD, it will call the virtual methods defined here.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TDTDValidator;


// ---------------------------------------------------------------------------
//  CLASS: MXMLDTDEvents
// PREFIX: mxev
// ---------------------------------------------------------------------------
class CIDXMLEXP MXMLDTDEvents
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        virtual ~MXMLDTDEvents()
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TDTDValidator;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AttrDef
        (
            const   TDTDAttrDef&            xadAttrDef
            , const tCIDLib::TBoolean       bIgnored
        );

        virtual tCIDLib::TVoid DocType
        (
            const   TDTDElemDecl&           xdeclRoot
            , const TString&                strPublicId
            , const TString&                strSystemId
        );

        virtual tCIDLib::TVoid DocTypeComment
        (
            const   TString&                strCommentText
        );

        virtual tCIDLib::TVoid DocTypePI
        (
            const   TString&                strTarget
            , const TString&                strValue
            , const tCIDXML::ELocations     eLocation
        );

        virtual tCIDLib::TVoid DocTypeWS
        (
            const   TString&                strWhitespace
        );

        virtual tCIDLib::TVoid ElementDecl
        (
            const   TDTDElemDecl&           xdeclElement
            , const tCIDLib::TBoolean       bIgnored
        );

        virtual tCIDLib::TVoid EndAttList
        (
            const   TDTDElemDecl&           xdeclParentElement
        );

        virtual tCIDLib::TVoid EndExtSubset();

        virtual tCIDLib::TVoid EndIntSubset();

        virtual tCIDLib::TVoid EntityDecl
        (
            const   TDTDEntityDecl&         xdeclEntity
            , const tCIDLib::TBoolean       bIgnored
        );

        virtual tCIDLib::TVoid EndDocType();

        virtual tCIDLib::TVoid NotationDecl
        (
            const   TDTDNotationDecl&       xdeclNotation
            , const tCIDLib::TBoolean       bIgnored
        );

        virtual tCIDLib::TVoid ResetDocType() = 0;

        virtual tCIDLib::TVoid StartAttList
        (
            const   TDTDElemDecl&           xdeclParentElement
        );

        virtual tCIDLib::TVoid StartExtSubset();

        virtual tCIDLib::TVoid StartIntSubset();

        virtual tCIDLib::TVoid TextDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
        );


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MXMLDTDEvents()
        {
        }


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        MXMLDTDEvents(const MXMLDTDEvents&);
        tCIDLib::TVoid operator=(const MXMLDTDEvents&);
};

#pragma CIDLIB_POPPACK

