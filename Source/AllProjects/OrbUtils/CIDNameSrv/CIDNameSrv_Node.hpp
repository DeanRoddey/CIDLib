//
// FILE NAME: CIDNameSrv_Node.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDNameSrv_Node.cpp file, which implements
//  the TNSNode class. This is the representation of a terminal node in
//  the name server hierarchy. It allows us to store the Orb object id, a
//  little housekeeping info, and a couple of strings which let users store
//  some browsable info with the objects.
//
// CAVEATS/GOTCHAS:
//
//  1)  We allow nodes to be marked as permanent, in which case the leas time
//      is ignored and bLeaseIsUp() always returns false. This is safe since
//      we don't expose the time stamp directly, so no one can get confused
//      by looking directly at the time stamp and having it disagree with
//      the value returned by bLeaseIsUp().
//
//      So, keep this in mind if the time stamp is ever exposed.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TNSNode
//  PREFIX: node
// ---------------------------------------------------------------------------
class TNSNode : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNSNode();

        TNSNode
        (
            const   TOrbObjId&              ooidTar
            , const TString&                strExtra1
            , const TString&                strExtra2
        );

        TNSNode
        (
            const   TOrbObjId&              ooidTar
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
        );

        TNSNode
        (
            const   TOrbObjId&              ooidTar
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
        );

        TNSNode(const TNSNode&) = default;

        ~TNSNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNSNode& operator=(const TNSNode&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPermanent() const;

        tCIDLib::TBoolean bPermanent
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bLeaseIsUp
        (
            const   tCIDLib::TEncodedTime   enctCheckAgainst
        );

        const TOrbObjId& ooidThis() const;

        const TString& strExtra1() const;

        const TString& strExtra1
        (
            const   TString&                strToSet
        );

        const TString& strExtra2() const;

        const TString& strExtra2
        (
            const   TString&                strToSet
        );

        const TString& strExtra3() const;

        const TString& strExtra3
        (
            const   TString&                strToSet
        );

        const TString& strExtra4() const;

        const TString& strExtra4
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid RenewLease();

        tCIDLib::TVoid UpdateRenewalStamp();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bPermanent
        //      This node is permanent, so it doesn't have to be renewed.
        //      Set on some special name server entries.
        //
        //  m_enctLastRenewal
        //      The time of the last lease renewal. If it is not renewed
        //      within the timeout period, it will be removed if not marked
        //      permanent. This is not exposed to the outside world. See
        //      the file level notes above.
        //
        //  m_ooidThis
        //      The Orb object id for the server side object that this node
        //      represents.
        //
        //  m_strExtraX
        //      The extra text values that can be stored along with the object
        //      id for non-terminals.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bPermanent;
        tCIDLib::TEncodedTime   m_enctLastRenewal;
        TOrbObjId               m_ooidThis;
        TString                 m_strExtra1;
        TString                 m_strExtra2;
        TString                 m_strExtra3;
        TString                 m_strExtra4;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TNSNode,TObject)
};

#pragma CIDLIB_POPPACK

