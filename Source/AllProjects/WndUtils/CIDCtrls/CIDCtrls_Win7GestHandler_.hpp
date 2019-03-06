//
// FILE NAME: CIDCtrls_Win7GestHandler_.hpp
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
//  CIDCtrls_GestHandler.hpp. This one works in terms of the multi-touch system available
//  in Windows 7 and upwards.
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
//   CLASS: Win7GestHandler
//  PREFIX: gesth
// ---------------------------------------------------------------------------
class TWin7GestHandler : public TCIDWndGestHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWin7GestHandler() = delete;

        TWin7GestHandler
        (
                    MCIDGestTarget* const   pmgesttToUse
            , const tCIDCtrls::TWndHandle   hwndTar
        );

        ~TWin7GestHandler() {}


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

        tCIDLib::TVoid Initialize() override;


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
        enum class EW7States
        {
            Idle
            , GotPress
        };

        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaBreakout
        //      If we get a button down, we set up an area around the click
        //      point. If we never move out of that, then when we get the
        //      button up we tell our parent that we were clicked and where.
        //
        //  m_eCurState
        //      Used to keep up with our state when tracking mouse moves.
        //
        //  m_hwndTar
        //      In our case we know we are dealing with a window, and we need to have
        //      access to the window handle internally, so the caller provides us with
        //      that.
        //
        //  m_pntPress
        //      Remember where we get an initial button press.
        //
        //  m_pntTmp
        //      Just a temp point for use during msg handling.
        // -------------------------------------------------------------------
        TArea                   m_areaBreakout;
        EW7States               m_eCurState;
        tCIDCtrls::TWndHandle   m_hwndTar;
        TPoint                  m_pntPress;
        TPoint                  m_pntTmp;
};


#pragma CIDLIB_POPPACK

