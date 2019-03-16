//
// FILE NAME: CIDCtrls_NotificationRequest.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1997
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
//  This is the header file for the CIDCtrls_NotificationRequest.cpp file. This
//  file implements the event notification classes. These objects are
//  installed on source windows to indicate that someone has requested an
//  event from it.
//
//  This was moved over from the old CIDWnd when we moved to wrapping standard
//  controls instead of doing our own.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TWindow;
class   TWndEvent;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TNotRequestBase
// PREFIX: notr
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TNotRequestBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotRequestBase
        (
            const   TNotificationId&        nidRequested
        );

        virtual ~TNotRequestBase();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDCtrls::EEvResponses eInvokeOnTarget
        (
                    TObject* const          pobjBeingSent
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TNotificationId& nidRequested() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TWindow;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bPostEvent
        (
                    TWndEvent* const        pwevToPost
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TNotRequestBase();
        TNotRequestBase(const TNotRequestBase&);
        tCIDLib::TVoid operator=(const TNotRequestBase&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_nidRequested
        //      This is the notification id that the target window is
        //      requesting to be notified of.
        //
        //  m_pnotrNext
        //      This is a pointer to another request. This allows the
        //      TWindow class to maintain a list of installed requests.
        // -------------------------------------------------------------------
        TNotificationId     m_nidRequested;
        TNotRequestBase*    m_pnotrNext;
};


template <class T> class TNotifyRequest : public TNotRequestBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotifyRequest(         T* const            pTarget
                        , const TNotificationId&    nidRequested) :

            TNotRequestBase(nidRequested)
            , m_pTarget(pTarget)
        {
        }

        ~TNotifyRequest()
        {
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eInvokeOnTarget(TObject* const)
        {
            return m_pTarget->eInvokeHandlers(nidRequested(), 0);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const T* pTarget() const
        {
            return m_pTarget;
        }

        T* pTarget()
        {
            return m_pTarget;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPostEvent(TWndEvent* const pwevToPost)
        {
            return m_pTarget->bSendMsg(kCIDCtrls::wmsgAsyncEvent, pwevToPost);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pTarget
        //      This is a pointer to the target object that is going to
        //      receive the notification.
        // -------------------------------------------------------------------
        T*  m_pTarget;
};


template <class T, class D> class TNotifyRequestFor : public TNotRequestBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotifyRequestFor(          T* const            pTarget
                            , const TNotificationId&    nidToHandle) :

            TNotRequestBase(nidToHandle)
            , m_pTarget(pTarget)
        {
        }

        ~TNotifyRequestFor()
        {
            // We don't own the target, we just reference it
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eInvokeOnTarget(TObject* const pobjBeingSent)
        {
            return m_pTarget->eInvokeHandlers(nidRequested(), pobjBeingSent);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const T* pTarget() const
        {
            return m_pTarget;
        }

        T* pTarget()
        {
            return m_pTarget;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPostEvent(TWndEvent* const pwevToPost)
        {
            return m_pTarget->bSendMsg(kCIDCtrls::wmsgAsyncEvent1, pwevToPost);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pTarget
        //      This is a pointer to the target object that is going to
        //      receive the notification.
        // -------------------------------------------------------------------
        T*  m_pTarget;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TNotRequestBase: Constructors and destructor
// ---------------------------------------------------------------------------
inline
TNotRequestBase::TNotRequestBase(const TNotificationId& nidRequested) :

    m_nidRequested(nidRequested)
    , m_pnotrNext(0)
{
}

inline TNotRequestBase::~TNotRequestBase()
{
}

// ---------------------------------------------------------------------------
//  TNotRequestBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline const TNotificationId& TNotRequestBase::nidRequested() const
{
    return m_nidRequested;
}
