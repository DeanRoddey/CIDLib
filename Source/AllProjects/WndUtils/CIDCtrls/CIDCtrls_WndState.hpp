//
// FILE NAME: CIDCtrls_WndState.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2015
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
//  This class implements some small classes that are used in frame/dialog window
//  state management. These windows often end up with complex requirements wrt
//  to hiding/showing, enabling/disabling items depending on what state the user
//  has gotten the window into, and also movement of focus and default buttons
//  as well.
//
//  This stuff can end up stupidly complex and tedious to do ad hoc in every such
//  window. This system allows you to set a set of states on a window, each of
//  which has a state name and includes various state options to implement when
//  that state is set.
//
//  This guy works in terms of resource ids, not objects, so they don't need access
//  to the internals of the window. So it just provides a method to set a new
//  state and takes a ref to the window to affect. So it can ultimately be applied
//  to any window type desired.
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
//  CLASS: TWndState
// PREFIX: wstate
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TWndState
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWndState() = delete;

        TWndState
        (
            const   TString&                strName
        );

        TWndState
        (
            const   TWndState&              wstateSrc
        );

        ~TWndState();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndState& operator=
        (
            const   TWndState&              wstateSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMenuDisable
        (
            const   tCIDLib::TResId         ridToadd
        );

        tCIDLib::TVoid AddMenuEnable
        (
            const   tCIDLib::TResId         ridToadd
        );

        tCIDLib::TVoid AddWndDisable
        (
            const   tCIDLib::TResId         ridToAdd
        );

        tCIDLib::TVoid AddWndEnable
        (
            const   tCIDLib::TResId         ridToadd
        );

        tCIDLib::TVoid ApplyState
        (
                    TWindow&                wndTar
            ,       TMenu* const            pmenuTar = 0
        );

        tCIDLib::TVoid Invert();

        tCIDLib::TVoid RemoveMenuDisable
        (
            const   tCIDLib::TResId         ridToRemove
        );

        tCIDLib::TVoid RemoveMenuEnable
        (
            const   tCIDLib::TResId         ridToRemove
        );

        tCIDLib::TVoid RemoveWndDisable
        (
            const   tCIDLib::TResId         ridToRemove
        );

        tCIDLib::TVoid RemoveWndEnable
        (
            const   tCIDLib::TResId         ridToRemove
        );

        tCIDLib::TVoid Reset
        (
            const   TString&                strName
        );

        const TString& strName() const;

        tCIDLib::TVoid SetFocus
        (
            const   tCIDLib::TResId         ridToadd
            , const tCIDLib::TBoolean       bIfNeeded
        );

        tCIDLib::TVoid SetDefButt
        (
            const   tCIDLib::TResId         ridToadd
            , const tCIDLib::TBoolean       bIfNeeded
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  bDefButtIfNeeded
        //  bFocusIfNeeded
        //      They can tell us to only move the focus or default button if it
        //      is needed (because the current focus/def button got disabled.)
        //
        //  m_fcolMenuDisable
        //  m_fcolMenuEnable
        //      This is the list of menu bar items to disable or enable.
        //
        //  m_fcolWndDisable
        //  m_fcolWndEnable
        //      This is the list of child windows to disable or enable.
        //
        //  m_strName
        //      The name of this state.
        //
        //  m_widFocus
        //      The control to put the focus on in this state. This can be zero,
        //      and no focus change will be done, other than to move it if the
        //      focus control gets disabled.
        //
        //  m_widDefButt
        //      The id of the push button that should be marked as the default
        //      button in this state. It can be left zero. It only needs to be
        //      moved if the default button is going to be disabled, in which
        //      case it needs to be moved, and then put back later when it can
        //      go back to the original default.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDefButtIfNeeded;
        tCIDLib::TBoolean   m_bFocusIfNeeded;

        tCIDLib::TCardList  m_fcolMenuDisable;
        tCIDLib::TCardList  m_fcolMenuEnable;

        tCIDLib::TCardList  m_fcolWndDisable;
        tCIDLib::TCardList  m_fcolWndEnable;

        TString             m_strName;

        tCIDCtrls::TWndId   m_widFocus;
        tCIDCtrls::TWndId   m_widDefButt;
};


// Create a vector of states, which is how they will be stored
namespace tCIDCtrls
{
    typedef TVector<TWndState>      TWndStateList;
}


#pragma CIDLIB_POPPACK

