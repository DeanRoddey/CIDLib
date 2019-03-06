//
// FILE NAME: CIDXML_EntityEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        MXMLEntityEvents(const MXMLEntityEvents&);
        tCIDLib::TVoid operator=(const MXMLEntityEvents&);
};

#pragma CIDLIB_POPPACK

