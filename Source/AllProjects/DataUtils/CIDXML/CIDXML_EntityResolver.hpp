//
// FILE NAME: CIDXML_EntityResolver.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2001
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
//  This header defines the abstract interface via which the parser core
//  can allow user code to redirect entity references. This is useful in many
//  ways, e.g. implementing a cache, taking advantage of PUBLIC ids to create
//  non-location specific references, impose different content into XML
//  files on the fly, and so forth.
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
//  CLASS: MXMLEntityResolver
// PREFIX: mxev
// ---------------------------------------------------------------------------
class CIDXMLEXP MXMLEntityResolver
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        MXMLEntityResolver(const MXMLEntityResolver&) = delete;

        virtual ~MXMLEntityResolver()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MXMLEntityResolver& operator=(const MXMLEntityResolver&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TDTDValidator;
        friend class TXMLParserCore;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MXMLEntityResolver() {}


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDXML::TEntitySrcRef esrResolve
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
            , const TString&                strEntityName
            , const TString&                strParentId
            , const tCIDXML::EResolveTypes  eResType
        ) = 0;

        virtual tCIDLib::TVoid ResetResolver() = 0;
};

#pragma CIDLIB_POPPACK

