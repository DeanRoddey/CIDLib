//
// FILE NAME: CIDXML_DocumentEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/04/1999
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
//  This file defines a mixin class that represents the event callouts for
//  the events from an XML document's content (prologue, content, misc.) The
//  parser core accepts an object of this type and will call back on it for
//  the events that the client code wants to see.
//
//  To avoid making every implementor of this mixin implement every method,
//  default dummy implementations are provided for most of them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TXMLParserCore;


// ---------------------------------------------------------------------------
//  CLASS: MXMLDocEvents
// PREFIX: mxev
// ---------------------------------------------------------------------------
class CIDXMLEXP MXMLDocEvents
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        MXMLDocEvents(const MXMLDocEvents&) = delete;
        MXMLDocEvents(MXMLDocEvents&&) = delete;

        virtual ~MXMLDocEvents()
        {
        }

        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        MXMLDocEvents& operator=(const MXMLDocEvents&) = delete;
        MXMLDocEvents& operator=(MXMLDocEvents&&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends.
        //
        //  The parser core is our friend because only he can call the event
        //  methods
        // -------------------------------------------------------------------
        friend class TXMLParserCore;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MXMLDocEvents() = default;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DocCharacters
        (
            const   TString&                strChars
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDXML::ELocations     eLocation
            , const tCIDLib::TBoolean       bAllSpaces
        );

        virtual tCIDLib::TVoid DocComment
        (
            const   TString&                strCommentText
            , const tCIDXML::ELocations     eLocation
        );

        virtual tCIDLib::TVoid DocPI
        (
            const   TString&                strTarget
            , const TString&                strValue
            , const tCIDXML::ELocations     eLocation
        );

        virtual tCIDLib::TVoid EndDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        );

        virtual tCIDLib::TVoid EndTag
        (
            const   TXMLElemDecl&           xdeclElem
        );

        virtual tCIDLib::TVoid ResetDocument() = 0;

        virtual tCIDLib::TVoid StartDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        );

        virtual tCIDLib::TVoid StartTag
        (
                    TXMLParserCore&         xprsSrc
            , const TXMLElemDecl&           xdeclElem
            , const tCIDLib::TBoolean       bEmpty
            , const TVector<TXMLAttr>&      colAttrList
            , const tCIDLib::TCard4         c4AttrListSize
        );

        virtual tCIDLib::TVoid XMLDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
            , const TString&                strStandalone
        );
};

#pragma CIDLIB_POPPACK

