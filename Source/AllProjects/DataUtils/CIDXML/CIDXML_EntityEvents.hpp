//
// FILE NAME: CIDXML_EntityEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/1999
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
//  This header defines the abstract interface via which the parsing code
//  informs client code of entity events. These include the beginning and
//  end of entities, and entity resolution.
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
//  CLASS: MXMLEntityEvents
// PREFIX: mxev
// ---------------------------------------------------------------------------
class CIDXMLEXP MXMLEntityEvents
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        MXMLEntityEvents(const MXMLEntityEvents&) = delete;
        MXMLEntityEvents(MXMLEntityEvents&&) = delete;
        MXMLEntityEvents& operator=(const MXMLEntityEvents&) = delete;
        MXMLEntityEvents& operator=(MXMLEntityEvents&&) = delete;

        virtual ~MXMLEntityEvents()
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TDTDValidator;
        friend class TXMLEntityMgr;
        friend class TXMLParserCore;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MXMLEntityEvents()
        {
        }


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid EndEntity
        (
            const   TXMLEntityDecl&         xdeclEnding
        ) = 0;

        virtual tCIDLib::TVoid ResetEntities() = 0;

        virtual tCIDLib::TVoid StartEntity
        (
            const   TXMLEntityDecl&         xdeclStarting
        ) = 0;
};

#pragma CIDLIB_POPPACK

