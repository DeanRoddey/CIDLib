//
// FILE NAME: CIDCtrls_NotificationHandler.hpp
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
//  This is the header for the CIDCtrls_NotificationHandler.cpp file. This file
//  implements the classes that sit on the receiving window and handle and
//  dispatch incoming events.
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
class TWindow;


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TNotHandlerBase
// PREFIX: wev
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TNotHandlerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotHandlerBase
        (
            const   TNotificationId&        nidRequested
        );

        virtual ~TNotHandlerBase();


        // -------------------------------------------------------------------
        //  Public , virtual methods
        // -------------------------------------------------------------------
        virtual tCIDCtrls::EEvResponses eInvoke(TObject* const) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TNotificationId& nidRequested() const;

        const TNotHandlerBase* pnothNext() const;

        TNotHandlerBase* pnothNext();


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TWindow;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TNotHandlerBase();
        TNotHandlerBase(const TNotHandlerBase&);
        tCIDLib::TVoid operator=(const TNotHandlerBase&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_nidRequested
        //      This is the notification id that the target window is
        //      requesting to be notified of.
        //
        //  m_pnothNext
        //      This is a pointer to another request. This allows the
        //      TWindow class to maintain a list of installed requests.
        // -------------------------------------------------------------------
        TNotificationId     m_nidRequested;
        TNotHandlerBase*    m_pnothNext;
};


template <class T> class TNotifyHandler : public TNotHandlerBase
{
    public :
        // -------------------------------------------------------------------
        //  Class specific data
        //
        //  This is the function type that must be provided, to which the
        //  notification is sent.
        // -------------------------------------------------------------------
        typedef tCIDCtrls::EEvResponses (T::* THandlerFunc)();


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotifyHandler(         T* const            pTarget
                        , const TNotificationId&    nidRequested
                        ,       THandlerFunc        pfnTarget) :

            TNotHandlerBase(nidRequested)
            , m_pTarget(pTarget)
            , m_pfnTarget(pfnTarget)
        {
        }

        ~TNotifyHandler()
        {
            // We don't own the target, we just reference it
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eInvoke(TObject* const) override
        {
            // The object is always ignored in this case
            return (m_pTarget->*m_pfnTarget)();
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pfnTarget
        //      This is the method pointer in the target object that is going
        //      to get the event.
        //
        //  m_pTarget
        //      This is a pointer to the target object that is going to
        //      receive the notification.
        // -------------------------------------------------------------------
        THandlerFunc    m_pfnTarget;
        T*              m_pTarget;
};


template <class T, class D> class TNotifyHandlerFor : public TNotHandlerBase
{
    public :
        // -------------------------------------------------------------------
        //  Class specific data
        //
        //  This is the function type that must be provided, to which the
        //  notification is sent.
        // -------------------------------------------------------------------
        typedef tCIDCtrls::EEvResponses (T::* THandlerFunc)(D& objData);


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotifyHandlerFor(          T* const            pTarget
                            , const TNotificationId&    nidRequested
                            ,       THandlerFunc        pfnTarget) :

            TNotHandlerBase(nidRequested)
            , m_pTarget(pTarget)
            , m_pfnTarget(pfnTarget)
        {
        }

        ~TNotifyHandlerFor()
        {
            // We don't own the target, we just reference it
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eInvoke(TObject* const pobjBeingSent)
        {
            // We know this is safe now
            #if CID_DEBUG_ON
            D* pobjActual = dynamic_cast<D*>(pobjBeingSent);
            #else
            D* pobjActual = static_cast<D*>(pobjBeingSent);
            #endif

            // And now we can invoke the target function
            return (m_pTarget->*m_pfnTarget)(*pobjActual);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pfnTarget
        //      This is the method pointer in the target object that is going
        //      to get the event.
        //
        //  m_pTarget
        //      This is a pointer to the target object that is going to
        //      receive the notification.
        // -------------------------------------------------------------------
        THandlerFunc    m_pfnTarget;
        T*              m_pTarget;
};


#pragma CIDLIB_POPPACK


