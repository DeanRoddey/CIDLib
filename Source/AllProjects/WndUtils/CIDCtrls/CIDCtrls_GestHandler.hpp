//
// FILE NAME: CIDCtrls_GestHandler.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This file provides three classes. One is a gesture handler (engine) that accepts user
//  input information from somewhere, and uses that to generate output gesture calls that
//  the client code can use as needed.
//
//  The second is a 'gesture target' mixin. This is what the gesture handler uses to make
//  those outgoing gesture calls. The client code will implement this interface wherever
//  it feels is appropriate, to accept the gesture output and react to it.
//
//  These two classes define an abstracted gesture engine that can be used in various ways.
//  In other files in this facility two common derivatives are defined, one that supports
//  the multi-touch capabilities in Win7 and beyond. The other that supports mouse input
//  which was all that was really available up through XP. Those are the most commonly
//  used ones, and we provide a static method that will gen up the correct one based on
//  local system capabilities. They both assume that the source of user input and the
//  consumer of the output is a window.
//
//  However that isn't necessary that input come from window messages. This scheme allows
//  for virtualization of gesture/mouse input. For instance, a remoted interface might
//  accept user input from a remote client and feed it into the gesture handler to get
//  back out gesture calls.
//
//  Since the two main (and window based) gesture handler derivatives do take their input
//  from window messages, and we want to be able to deal with them generically (based on
//  whether multi-touch or only mouse input is available), we create the third class which
//  just adds a virtual method for handling window messages. The Win7/XP gesture handlers
//  both derive from this.
//
//  There is a TTouchWnd class that provides easily extensible support for gesture based
//  input (using the Win7/XP handlers based on system capabilities.) The derived class just
//  overrides virtuals to handle the generated gesture output calls. And it fees the appropriate
//  window messages into the gesture handler for you.
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
//  CLASS: MCIDGestTarget
// PREFIX: mgestt
// ---------------------------------------------------------------------------
class CIDCTRLSEXP MCIDGestTarget
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MCIDGestTarget(const MCIDGestTarget&) = delete;

        virtual ~MCIDGestTarget();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MCIDGestTarget& operator=(const MCIDGestTarget&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bProcessGestEv
        (
            const   tCIDCtrls::EGestEvs     eEv
            , const TPoint&                 pntStart
            , const TPoint&                 pntAt
            , const TPoint&                 pntDelta
            , const tCIDLib::TBoolean       bTwoFingers
        ) = 0;

        virtual tCIDLib::TVoid GestClick
        (
            const   TPoint&                 pntAt
        ) = 0;

        virtual tCIDLib::TVoid GestInertiaIdle
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::EDirs          eDir
        );

        virtual tCIDLib::TVoid GestReset
        (
            const   tCIDCtrls::EGestReset   eType
        ) = 0;

        virtual tCIDLib::TVoid PerGestOpts
        (
            const   TPoint&                 pntAt
            , const tCIDLib::EDirs          eDir
            , const tCIDLib::TBoolean       bTwoFingers
            ,       tCIDCtrls::EGestOpts&   eOpts
            ,       tCIDLib::TFloat4&       f4VScale
        )   = 0;

        virtual tCIDLib::TVoid GestFlick
        (
            const   tCIDLib::EDirs          eDirection
            , const TPoint&                 pntStartPos
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        MCIDGestTarget();
};


// ---------------------------------------------------------------------------
//  CLASS: TCIDGestHandler
// PREFIX: gesth
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCIDGestHandler
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TInt4 i4AdjustForSpeed
        (
            const   tCIDLib::TInt4          i4MaxPels
            , const tCIDLib::TInt4          i4SpanPels
            , const tCIDLib::TFloat8        f8Speed
        );


        // -------------------------------------------------------------------
        //  Public constructors and Destructor
        // -------------------------------------------------------------------
        TCIDGestHandler() = delete;

        virtual ~TCIDGestHandler();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Initialize();

        virtual tCIDLib::TVoid Terminate();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CancelGesture();

        const TPoint& pntStart() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDGestHandler
        (
                    MCIDGestTarget* const   pmgesttToUse
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Reset
        (
            const   tCIDCtrls::EGestReset   eType
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods. The derived class call these
        //  to pass us raw gesture events. We in turn calls the target window.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAllowClickStart();

        tCIDLib::TBoolean bStartGesture
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid Clicked
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid EndGesture
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid Move
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bTwoFingers
        );

        MCIDGestTarget& mgesttCur();


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  EStates
        //      We use a simple state machine to know what we are doing at
        //      any one time.
        // -------------------------------------------------------------------
        enum class EStates
        {
            Idle
            , WaitFirst
            , DoingFlick
            , DoingPan
            , DoingInertia
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCalcDelta
        (
            const   TPoint&                 pntAt
            ,       TPoint&                 pntDelta
            ,       TPoint&                 pntNewPos
        );

        tCIDLib::TBoolean bHorizontal() const;

        tCIDLib::TVoid CalcInertialDelta();

        tCIDLib::EDirs eCalcDirection
        (
            const   TPoint&                 pntStartPos
            , const TPoint&                 pntNew
        );

        tCIDLib::TVoid GenerateInertia();

        tCIDLib::TInt4 i4CalcDelta
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid StoreInertiaPnt
        (
            const   TPoint&                 pntNewPos
        );


        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_bTwoFingers
        //      If we see two fingers down before we report the start of
        //      gesture (upon first reported move), then this is set to
        //      let us know. We report it in the gesture events we send out.
        //      Once we see two fingers it stays that way till the end of
        //      the gesture, even if one is removed at some point.
        //
        //  m_eCurState
        //      Our current processing state.
        //
        //  m_eDir
        //      Once we see the direction we are going in, we set this to
        //      remember that. For flicks, this is the literal direction, and
        //      any movement other than 'forward' in this direction cancels
        //      the flick. For gestures, Left means horizontal and Down means
        //      vertical, since they can go either direction along the axis
        //      of movement.
        //
        //  m_eGestOpts
        //      Storage for the gesture ops set by the window. This is set by
        //      calling the window when we see the start of a gesture. It tells
        //      us what is valid in the direction that the gesture has been
        //      determined to be going in.
        //
        //  m_entStart
        //      The start time, which we use to decide if the operation has
        //      completed within the gesture time limit.
        //
        //  m_f4InertiaVelo
        //      When we get the end of gesture, we go through the stored position
        //      samples and calculate a velocity value from the ones that fell
        //      within the last so many milliseconds. This is used as the starting
        //      point for inertia.
        //
        //  m_f4PerGestVScale
        //      Each potential target that implements gestures might have a slightly
        //      different amount of data to display. The less data, the faster it'll
        //      move through the list of things, whatever they are. When we call
        //      the per-gest opts method to ask them if we can start a gesture, we
        //      get back a scale factor. We'll use this to adjust the initial
        //      per-round factor a bit, to reduce the amount of movement.
        //
        //  m_pntDelta
        //      Used as a temp for calculating delta and sending to the window.
        //
        //  m_pntLastPos
        //      In order to report deltas we have to remember the previous
        //      point to compare against.
        //
        //  m_pntNewPos
        //      Just a temp for getting the new position in to pass to the
        //      target window.
        //
        //  m_pntStartPos
        //      The original point where the user pressed.
        //
        //  m_pmgesttToUse
        //      The caller provided us a pointer to this target handler for our gesture
        //      callbacks. We don't own it, we just reference it.
        // -------------------------------------------------------------------
        tCIDLib::TCard8         m_ac8InertiaBuf[kCIDCtrls::c4InertiaBufSz];
        tCIDLib::TInt4          m_ai4InertiaBuf[kCIDCtrls::c4InertiaBufSz];
        tCIDLib::TBoolean       m_bTwoFingers;
        EStates                 m_eCurState;
        tCIDLib::EDirs          m_eDir;
        tCIDCtrls::EGestOpts    m_eGestOpts;
        tCIDLib::TEncodedTime   m_enctStart;
        tCIDLib::TFloat4        m_f4InertiaVelo;
        tCIDLib::TFloat4        m_f4PerGestVScale;
        TPoint                  m_pntDelta;
        TPoint                  m_pntLastPos;
        TPoint                  m_pntStartPos;
        TPoint                  m_pntNewPos;
        MCIDGestTarget*         m_pmgesttToUse;
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDWndGestHandler
// PREFIX: gesth
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCIDWndGestHandler : public TCIDGestHandler
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static TCIDWndGestHandler* pgesthMakeNew
        (
                    MCIDGestTarget* const   pmgesttToUse
            , const tCIDCtrls::TWndHandle   hwndTar
        );


        // -------------------------------------------------------------------
        //  Public constructors and Destructor
        // -------------------------------------------------------------------
        TCIDWndGestHandler() = delete;

        ~TCIDWndGestHandler();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bHandleMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
            ,       tCIDLib::TBoolean&      bFlag
        ) = 0;

    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDWndGestHandler
        (
                    MCIDGestTarget* const   pmgesttToUse
        );
};

#pragma CIDLIB_POPPACK

