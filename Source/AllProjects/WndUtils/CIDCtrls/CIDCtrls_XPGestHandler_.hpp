//
// FILE NAME: CIDCtrls_XPGestHandler_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This class is a derivative of the window based gesture handler, in the file
//  CIDCtrls_GestHandler.hpp. This one works in terms of the standard mouse based
//  input system and converts them into faux gesture outputs.
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
//   CLASS: TXPGestHandler
//  PREFIX: gesth
// ---------------------------------------------------------------------------
class TXPGestHandler : public TCIDWndGestHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXPGestHandler()  = delete;

        TXPGestHandler
        (
                    MCIDGestTarget* const   pmgesttToUse
            , const tCIDCtrls::TWndHandle   hwndTar
        );

        ~TXPGestHandler();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
            ,       tCIDLib::TBoolean&      bFlag
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset
        (
            const   tCIDCtrls::EGestReset   eType
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  We need a small state enum to keep up with what we are doing.
        // -------------------------------------------------------------------
        enum class EXPStates
        {
            Idle
            , GotPress
            , InGesture
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaBreakout
        //      The area around the touch point that we have to move out of
        //      to be considered a gesture.
        //
        //  m_eCurState
        //      Our current state, so we know what we need to do when we
        //      receive messages.
        //
        //  m_hwndTar
        //      In our case we know we are dealing with a window, and we need to have
        //      access to the window handle internally, so the caller provides us with
        //      that.
        //
        //  m_pntStart
        //      When we see a button down, we remember where it happened. If
        //      we never move out of a small area around this point before
        //      releasing, we treat it as a click, so we send a press,
        //      move, and release all at once.
        //
        //      Else, we assume it's a gesture, and start that process.
        //      Just for temp use doing our message handling
        //
        //  m_pntTmp
        //      Otherwise this is just used as a temp for building the point
        //      into to report.
        // -------------------------------------------------------------------
        TArea                   m_areaBreakout;
        EXPStates               m_eCurState;
        tCIDCtrls::TWndHandle   m_hwndTar;
        TPoint                  m_pntStart;
        TPoint                  m_pntTmp;
};

#pragma CIDLIB_POPPACK

