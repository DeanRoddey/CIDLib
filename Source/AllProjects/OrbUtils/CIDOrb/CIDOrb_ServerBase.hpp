//
// FILE NAME: CIDOrb_ServerBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2000
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
//  This file defines the IDL server base class. The IDL compiler makes
//  all the server stub classes derivatives of this class. We also define a
//  little 'thread entry counter' janitor that bumps up and down a thread
//  safe counter in each object. This lets us know how many threads are in
//  an object.
//
// CAVEATS/GOTCHAS:
//
//  1)  For the server side, we only accept the interface id, which is provided
//      by the IDL generated derived class. We generate a unique instance id
//      for each newly created server side instance.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TOrbSrvEnteredJan;

// ---------------------------------------------------------------------------
//   CLASS: TOrbServerBase
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbServerBase : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public destructor
        // -------------------------------------------------------------------
        TOrbServerBase() = delete;

        TOrbServerBase(const TOrbServerBase&) = delete;
        TOrbServerBase(TOrbServerBase&&) = delete;

        ~TOrbServerBase() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbServerBase& operator=(const TOrbServerBase&) = delete;
        TOrbServerBase& operator=(TOrbServerBase&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCleanup() const
        {
            return m_bCleanup;
        }

        const TOrbObjId& ooidThis() const
        {
            return m_ooidThis;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TFacCIDOrb;
        friend class TOrbSrvEnteredJan;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TOrbServerBase
        (
            const   TString&                strInterfaceId
            , const TString&                strClientProxyClass
        );

        TOrbServerBase
        (
            const   TOrbObjId&              ooidThis
        );


        // --------------------------------------------------------------------
        // Protected, virtual methods
        // --------------------------------------------------------------------
        virtual tCIDLib::TVoid Dispatch
        (
            const   TString&                strCmd
            ,       TOrbCmd&                orbcToDispatch
        );

        virtual tCIDLib::TVoid Initialize() = 0;

        virtual tCIDLib::TVoid Terminate() = 0;


        // --------------------------------------------------------------------
        //  Protected, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TCard4 c4DecEnteredCount()
        {
            // We want to return the post-dec count so use prefix
            return --m_scntEntered;
        }

        tCIDLib::TCard4 c4EnteredCount() const
        {
            return m_scntEntered.c4Value();
        }

        tCIDLib::TCard4 c4IncEnteredCount()
        {
            // We want to return the post-inc count so use prefix
            return ++m_scntEntered;
        }

        tCIDLib::TVoid SetCleanupFlag()
        {
            m_bCleanup = kCIDLib::True;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid InitializeOrbServer();

        static tCIDLib::TVoid TerminateOrbServer();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCleanup
        //      This is settable by the ORB as a hint to threads that might be in the
        //      object that it's time to give up and get out so that the object can be
        //      cleaned up.
        //
        //  m_ooidThis
        //      This is our object id. It uniquely identifies this object instance, and
        //      indicates what interface it implements, and where it lives.
        //
        //      It also contains the key that is used to store this object in a hash
        //      table on the server side, and which is used to vector incoming calls to it.
        //
        //  m_scntEntered
        //      We have to know if threads are in a particular object so that when removing
        //      objects we can wait for them to exit the object before we kill it. The
        //      TOrbSrvEnteredJan class will inc/dec this for us.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bCleanup;
        TOrbObjId           m_ooidThis;
        TSafeCard4Counter   m_scntEntered;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbServerBase,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TOrbSrvEnteredJan
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class TOrbSrvEnteredJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbSrvEnteredJan() = delete;

        TOrbSrvEnteredJan(TOrbServerBase* const porbsEntered) :

            m_porbsEntered(porbsEntered)
        {
            m_porbsEntered->c4IncEnteredCount();
        }

        TOrbSrvEnteredJan(const TOrbSrvEnteredJan&) = delete;
        TOrbSrvEnteredJan(TOrbSrvEnteredJan&&) = delete;

        ~TOrbSrvEnteredJan()
        {
            m_porbsEntered->c4DecEnteredCount();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbSrvEnteredJan& operator=(const TOrbSrvEnteredJan&) = delete;
        TOrbSrvEnteredJan& operator=(TOrbSrvEnteredJan&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_porbsEntered
        //      The object that was entered upon ctor, and that we need to
        //      exit upon dtor.
        // -------------------------------------------------------------------
        TOrbServerBase* m_porbsEntered;
};

#pragma CIDLIB_POPPACK

