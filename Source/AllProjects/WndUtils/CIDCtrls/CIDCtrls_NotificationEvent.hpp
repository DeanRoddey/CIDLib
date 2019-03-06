//
// FILE NAME: CIDCtrls_NotificationEvent.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the window event classes. These classes are used
//  to send a notification event to a window. The actual data sent is what
//  is of use to the client code. The classes below are just the wrapper
//  used internally to send that data.
//
//  This was moved over from the old CIDWnd when we moved to wrapping standard
//  controls instead of doing our own.
//
// CAVEATS/GOTCHAS:
//
//  1)  For various reasons, these classes are not derived from TObject and
//      implement their own little polymorphic duplication scheme instead
//      of using the standard MDuplicable interface.
//
//  2)  Synchronous notifications don't have to copy the object, while async ones
//      do. Syncs don't want to, because the notification object may be used to
//      return information. So the TWndEventFor class takes an adopt flag that
//      controls how it manages the data object.
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
//  CLASS: TWndEvent
// PREFIX: wev
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TWndEvent
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TWndEvent
        (
            const   TNotificationId&        nidBeingSent
        );

        TWndEvent
        (
            const   TWndEvent&              wevToCopy
        );

        virtual ~TWndEvent();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndEvent& operator=(const TWndEvent& wevToAssign);


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual TObject* pobjToSend() const;

        virtual TWndEvent* pwevDuplicate() const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TNotificationId& nidBeingSent() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TWndEvent();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_nidBeingSent
        //      The notification id of the event being sent.
        // -------------------------------------------------------------------
        TNotificationId     m_nidBeingSent;
};


template <class D> class TWndEventFor : public TWndEvent
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TWndEventFor(   const   TNotificationId&    nidBeingSent
                        ,       D* const            pobjToSend
                        , const tCIDLib::EAdoptOpts eAdopt = tCIDLib::EAdoptOpts::Adopt) :

            TWndEvent(nidBeingSent)
            , m_eAdopt(eAdopt)
            , m_pobjToSend(pobjToSend)
        {
        }

        TWndEventFor(const TWndEventFor<D>& wevSrc) :

            TWndEvent(wevSrc)
            , m_eAdopt(wevSrc.m_eAdopt)
            , m_pobjToSend(0)
        {
            //
            //  If there's a data object and we are adopting, then we need to make
            //  a copy of his. Else we just copy the pointer.
            //
            if (wevSrc.m_pobjToSend)
            {
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    m_pobjToSend = ::pDupObject<D>(*wevSrc.m_pobjToSend);
                else
                    m_pobjToSend = wevSrc.m_pobjToSend;
            }
        }

        ~TWndEventFor()
        {
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete m_pobjToSend;
            m_pobjToSend = 0;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndEventFor<D>& operator=(const TWndEventFor<D>& wevSrc)
        {
            if (this != &wevSrc)
            {
                // Call our parent first
                TWndEvent::operator=(wevSrc);

                // Clean up any existing object if we own it
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete m_pobjToSend;
                m_pobjToSend = 0;

                // Now take the caller's adopt flag
                m_eAdopt = wevSrc.m_eAdopt;

                //
                //  If we are now adopting, make a copy of the caller's object, if
                //  any. Else just copy the pointer.
                //
                if (wevSrc.m_pobjToSend)
                {
                    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                        m_pobjToSend = ::pDupObject<D>(*wevSrc.m_pobjToSend);
                    else
                        m_pobjToSend = wevSrc.
                }
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TObject* pobjToSend() const override
        {
            return m_pobjToSend;
        }

        TWndEvent* pwevDuplicate() const override
        {
            return new TWndEventFor<D>(*this);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        D& objNotInfo()
        {
            return *m_pobjToSend;
        }


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TWndEventFor();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      Indicates whether we own the data object or not
        //
        //  m_pobjToSend
        //      The data being passed to the event handler. It is mutable because
        //      the events themselves are constant but the data is not, so that it
        //      can be modified, in order to return info to the sending control in
        //      the case of synchronous notification.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        mutable D*          m_pobjToSend;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TWndEvent: Constructors and destructor
// ---------------------------------------------------------------------------
inline TWndEvent::TWndEvent(const TNotificationId& nidBeingSent) :

    m_nidBeingSent(nidBeingSent)
{
}

inline TWndEvent::TWndEvent(const TWndEvent& wevToCopy) :

    m_nidBeingSent(wevToCopy.m_nidBeingSent)
{
}

inline TWndEvent::~TWndEvent()
{
}


// ---------------------------------------------------------------------------
//  TWndEvent: Public operators
// ---------------------------------------------------------------------------
inline TWndEvent& TWndEvent::operator=(const TWndEvent& wevToAssign)
{
    if (this != &wevToAssign)
        m_nidBeingSent = wevToAssign.m_nidBeingSent;
    return *this;
}


// ---------------------------------------------------------------------------
//  TWndEvent: Public, virtual methods
// ---------------------------------------------------------------------------
inline TObject* TWndEvent::pobjToSend() const
{
    // At this level we have no object, so return zero
    return 0;
}

inline TWndEvent* TWndEvent::pwevDuplicate() const
{
    return new TWndEvent(*this);
}


// ---------------------------------------------------------------------------
//  TWndEventBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline const TNotificationId& TWndEvent::nidBeingSent() const
{
    return m_nidBeingSent;
}


