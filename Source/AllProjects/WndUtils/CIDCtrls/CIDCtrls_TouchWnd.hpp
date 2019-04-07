//
// FILE NAME: CIDCtrls_TouchWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/19/2015
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
//  This is the base class for any windows that want to use touch input. You can do all
//  of this stuff yourself if you need to, but it's just a nice convenience.
//
//  This guy will handle both mouse and touch input messages and convert them into a
//  consistent touch interface that is easy to use. So the caller doesn't need to care if
//  the monitor is multi-touch or only mouse (or old style single-touch which looks
//  like a mouse to us.)
//
//  We use the pluggable gesture handler interface, and just ask it to give us a gesture
//  handler based on the local system capabilities. We just feed that gesture handler the
//  appropriate window messages, and he generates the gesture output calls. We implement
//  his gesture target mixin and just leave those methods unhandled so that the derived
//  class has to provide them.
//
//  So the way it works is:
//
//      Us sending wnd msgs input to
//      -> multi-touch or mouse enabled gesture handler
//      -> which digests them and calls gesture methods on the gesture target
//      -> which come to use since we implement the target and pass ourself
//      -> which he derived class overrides and uses as appropriate
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
//  CLASS: TTouchWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTouchWnd : public TWindow, public MCIDGestTarget
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTouchWnd(const TTouchWnd&) = delete;

        ~TTouchWnd();


        // -------------------------------------------------------------------
        //  Public oprators
        // -------------------------------------------------------------------
        TTouchWnd& operator=(const TTouchWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateTouchWnd
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTouchWnd();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFlag
        //      We maintain a flag which we pass into the gesture handler's msg handler.
        //      So we basically are just persisting a flag for him across invocations.
        //      He can change it and we pass it back in next time.
        //
        //  m_pgesthCur
        //      The gesture handler that we got upon startup. We just ask the gesture
        //      handler engine for an appropriate handler and use it.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bFlag;
        TCIDWndGestHandler* m_pgesthCur;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTouchWnd, TWindow)
};

#pragma CIDLIB_POPPACK




