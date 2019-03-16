//
// FILE NAME: CIDNameSrv_Node.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
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
//  This file implements the TNSNode class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDNameSrv.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TNSNode,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TNSNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNSNode: Constructors and Destructor
// ---------------------------------------------------------------------------
TNSNode::TNSNode() :

    m_bPermanent(kCIDLib::False)
    , m_enctLastRenewal(0)
{
}

TNSNode::TNSNode(const  TOrbObjId&  ooidThis
                , const TString&    strExtra1
                , const TString&    strExtra2) :

    m_bPermanent(kCIDLib::False)
    , m_enctLastRenewal(TTime::enctNow())
    , m_ooidThis(ooidThis)
    , m_strExtra1(strExtra1)
    , m_strExtra2(strExtra2)
{
}

TNSNode::TNSNode(const  TOrbObjId&  ooidThis
                , const TString&    strExtra1
                , const TString&    strExtra2
                , const TString&    strExtra3) :

    m_bPermanent(kCIDLib::False)
    , m_enctLastRenewal(TTime::enctNow())
    , m_ooidThis(ooidThis)
    , m_strExtra1(strExtra1)
    , m_strExtra2(strExtra2)
    , m_strExtra3(strExtra3)
{
}

TNSNode::TNSNode(const  TOrbObjId&  ooidThis
                , const TString&    strExtra1
                , const TString&    strExtra2
                , const TString&    strExtra3
                , const TString&    strExtra4) :

    m_bPermanent(kCIDLib::False)
    , m_enctLastRenewal(TTime::enctNow())
    , m_ooidThis(ooidThis)
    , m_strExtra1(strExtra1)
    , m_strExtra2(strExtra2)
    , m_strExtra3(strExtra3)
    , m_strExtra4(strExtra4)
{
}

TNSNode::~TNSNode()
{
}


// ---------------------------------------------------------------------------
//  TNSNode: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the permanent flag
tCIDLib::TBoolean TNSNode::bPermanent() const
{
    return m_bPermanent;
}

tCIDLib::TBoolean TNSNode::bPermanent(const tCIDLib::TBoolean bToSet)
{
    m_bPermanent = bToSet;
    return m_bPermanent;
}


// Check to see if the lease for this node is up
tCIDLib::TBoolean
TNSNode::bLeaseIsUp(const tCIDLib::TEncodedTime enctCheckAgainst)
{
    // If a permanent node, just say it's not
    if (m_bPermanent)
        return kCIDLib::False;

    //
    //  Check for psycho scenario of our renewal being later than the time to
    //  check against. If so, say it's up to force a reset.
    //
    if (enctCheckAgainst < m_enctLastRenewal)
        return kCIDLib::True;

    return ((enctCheckAgainst - m_enctLastRenewal) > kCIDOrb::enctBindingLeaseTime);
}


// Read only access to the object id
const TOrbObjId& TNSNode::ooidThis() const
{
    return m_ooidThis;
}


// Read only access to the extra values
const TString& TNSNode::strExtra1() const
{
    return m_strExtra1;
}

const TString& TNSNode::strExtra1(const TString& strToSet)
{
    m_strExtra1 = strToSet;
    return  m_strExtra1;
}


const TString& TNSNode::strExtra2() const
{
    return m_strExtra2;
}

const TString& TNSNode::strExtra2(const TString& strToSet)
{
    m_strExtra2 = strToSet;
    return  m_strExtra2;
}


const TString& TNSNode::strExtra3() const
{
    return m_strExtra3;
}

const TString& TNSNode::strExtra3(const TString& strToSet)
{
    m_strExtra3 = strToSet;
    return  m_strExtra3;
}


const TString& TNSNode::strExtra4() const
{
    return m_strExtra4;
}

const TString& TNSNode::strExtra4(const TString& strToSet)
{
    m_strExtra4 = strToSet;
    return  m_strExtra4;
}


// Update our last renewal stamp
tCIDLib::TVoid TNSNode::RenewLease()
{
    m_enctLastRenewal = TTime::enctNow();
}


tCIDLib::TVoid TNSNode::UpdateRenewalStamp()
{
    m_enctLastRenewal = TTime::enctNow();
}

