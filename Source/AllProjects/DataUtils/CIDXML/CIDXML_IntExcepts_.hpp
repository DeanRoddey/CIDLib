//
// FILE NAME: CIDXML_IntExcepts_.hpp
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
//  There are a few places internally where we need to throw exceptions for
//  purely internal reasons. They don't propogate out. So we don't need the
//  overhead of the standard CIDLib exception mechanism. So a couple of very
//  simple exception classes are defined here.
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
//  CLASS: TXMLEndOfEntity
// PREFIX: exc
// ---------------------------------------------------------------------------
class TXMLEndOfEntity
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEndOfEntity() = delete;

        TXMLEndOfEntity(const   TXMLEntityDecl& xdeclEnded
                        , const tCIDLib::TCard4 c4SpoolerId) :

            m_c4SpoolerId(c4SpoolerId)
            , m_pxdeclEntity(&xdeclEnded)
        {
        }

        TXMLEndOfEntity(const TXMLEndOfEntity&) = default;
        TXMLEndOfEntity(TXMLEndOfEntity&&) = default;

        ~TXMLEndOfEntity() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLEndOfEntity& operator=(const TXMLEndOfEntity&) = default;
        TXMLEndOfEntity& operator=(TXMLEndOfEntity&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SpoolerId() const
        {
            return m_c4SpoolerId;
        }

        const TXMLEntityDecl& xdeclEntityEnded() const
        {
            return *m_pxdeclEntity;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c4SpoolerId
        //      This is the id of the spooler which was spooling this
        //      particular instance of the entity that ended.
        //
        //  m_xdeclEntity
        //      This is a pointer to the entity which has ended. We don't own
        //      it, we just reference it.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4SpoolerId;
        const TXMLEntityDecl*   m_pxdeclEntity;
};

#pragma CIDLIB_POPPACK

