//
// FILE NAME: CIDLib_PubSub2.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2018
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
//  To avoid an order of inclusion issue, this is broken out of the main Pub/Sub
//  header. See the class comments in the main header.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TPubSubAsyncSub
// PREFIX: psasub
// ---------------------------------------------------------------------------
class CIDLIBEXP TPubSubAsyncSub : public TObject, private MPubSubscription
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TPubSubAsyncSub
        (
            const   tCIDLib::TBoolean       bCreateEvent
        );

        TPubSubAsyncSub(const TPubSubAsyncSub&) = delete;
        TPubSubAsyncSub(TPubSubAsyncSub&&) = delete;

        ~TPubSubAsyncSub();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPubSubAsyncSub& operator=(const TPubSubAsyncSub&) = delete;
        TPubSubAsyncSub& operator=(TPubSubAsyncSub&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetNextMsg
        (
                    TPubSubMsg&             psmsgToFill
            , const tCIDLib::TCard4         c4WaitMSs
        );

        tCIDLib::TCard4 c4MsgCount() const;

        TEvent& evWait();

        tCIDLib::TVoid FlushMsgs();

        tCIDLib::TVoid SubscribeTo
        (
            const   TString&                strTopicPath
            , const tCIDLib::TBoolean       bMustExist
        );

        tCIDLib::TVoid UnsubscribeFrom
        (
            const   TString&                strTopicPath
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ProcessPubMsg
        (
            const   TPubSubMsg&             psmsgIn
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private types
        // -------------------------------------------------------------------
        using TMsgList = TQueue<TPubSubMsg>;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BlockingCnt
        //      This is bumped for each thread that has called bGetNextMsg(). If we are
        //      assigned to and this is non-zero, we throw, because that would be very
        //      bad.
        //
        //  m_colMsgs
        //      Our lists of msgs that we have stored. It is thread safe so that the
        //      caller can block while waiting for msgs if desired.
        //
        //  m_pevWait
        //      We can be optionally asked to create and manage an event that the client
        //      code can use in multi-event waits. If not used, it is zero.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4BlockingCnt;
        TMsgList        m_colMsgs;
        TEvent*         m_pevWait;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPubSubAsyncSub,TObject)
};

#pragma CIDLIB_POPPACK

