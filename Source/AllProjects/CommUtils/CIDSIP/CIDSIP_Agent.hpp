//
// FILE NAME: CIDSIP_Agent.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 4/28/2013
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
//  This is the header for the CIDSIP_Agent.cpp file. This implements the
//  basic SIP agent functionality, for sending and receiving msgs and some
//  utility classes. Client and server side agents will be derived from
//  this class. They tell us which side we are, so that we can do side
//  sensitive stuff without them having to handle all decisions necessarily.
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
//   CLASS: TSIPAgent
//  PREFIX: sipag
// ---------------------------------------------------------------------------
class CIDSIPEXP TSIPAgent : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TSIPAgent();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSIPAgent
        (
            const   tCIDLib::ECSSides       eSide
            , const TString&                strLocAddr
            , const TString&                strLocContact
            , const TString&                strLocDisplay
            , const TString&                strCallID
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TSIPAgent(const TSIPAgent&);
        tCIDLib::TVoid operator=(const TSIPAgent&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eSide
        //      Indicates if this is a client or server side agent. We do
        //      things different depending.
        //
        //  m_strCallID
        //      A call is represented by a user agent, and each call has a
        //      unique call id.
        //
        //  m_strLocAddr
        //  m_strLocContact
        //  m_strLocDisplay
        //  m_strLocTag
        //      The calling app provides us with the local call session
        //      info when creating an agent. We pass this info on to any
        //      dialogs we create.
        //
        //  m_strTmp1
        //  m_strTmp2
        //      We have lots of string formatting requirements, so we keep
        //      some temp strings around.
        //
        //  m_urlFrom
        //  m_urlTo
        //      When a new call is started, in addition to storing the raw
        //      to and from URIs, we parse them into these objects as well,
        //      so that we can insure they are SIP URIs and this lets us get
        //      to the individual bits reliably.
        // -------------------------------------------------------------------
        tCIDLib::ECSSides   m_eSide;
        TString             m_strCallID;
        TString             m_strLocAddr;
        TString             m_strLocContact;
        TString             m_strLocDisplay;
        TString             m_strLocTag;
        TString             m_strTmp1;
        TString             m_strTmp2;
        TURL                m_urlFrom;
        TURL                m_urlTo;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSIPAgent,TObject)
};


#pragma CIDLIB_POPPACK


