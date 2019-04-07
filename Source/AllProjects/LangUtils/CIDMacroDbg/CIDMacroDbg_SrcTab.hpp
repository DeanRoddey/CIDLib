//
// FILE NAME: CIDMacroDbg_SrcTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
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
//  This is the header for windows that we insert as tabs into the top pane, one
//  per opened source file. We create a source editor window and keep it filling
//  our client area.
//
//  Tab windows are always named, which is really just passed through to the base
//  TWindow name attribute. We set it to the class path, so that we can look up
//  a tab by class path easily.
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
//   CLASS: TSrcTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TSrcTab : public TTabWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSrcTab
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bIsSessionClass
        );

        TSrcTab(const TSrcTab&) = delete;

        ~TSrcTab();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSrcTab& operator=(const TSrcTab&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bChanged() const;

        tCIDLib::TStrList& colGotoList();

        tCIDLib::TVoid CreateSrcTab
        (
                    TTabbedWnd&             wndParent
            , const TString&                strTabText
        );

        tCIDMacroEng::EResModes eMode() const;

        tCIDMacroEng::EResModes eMode
        (
            const   tCIDMacroEng::EResModes    eToSet
        );

        tCIDLib::TVoid GoToLine
        (
            const   tCIDLib::TCard4         c4LineNum
        );

        const TSrcEditor* pwndEditor() const;

        TSrcEditor* pwndEditor();

        const TString& strClassPath() const;

        tCIDLib::TVoid SetText
        (
            const   TString&                strToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid GettingFocus() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bChanged
        //      The editor window remembers if it is changed. But we don't want the editor
        //      window having to update the indicator every time we type a character. So
        //      our bChanged() returns this first and then stores the value from the editor
        //      window.
        //
        //  m_bSessionClass
        //      Indicates that this is the session class, which is the class that the session
        //      was opened for, so it cannot be closed until the session is closed.
        //
        //  m_colGotoList
        //      We keep the most recent goto lines for this class here. The main window
        //      will pass this back in for any Alt-G operation when we are active.
        //
        //  m_eMode
        //      Indicates the mode that this file was loaded in.
        //
        //  m_pnothEdit
        //      We store the notification handler that we install from the the editor window
        //      to the parent source editor frame, so that we can remove it when this editor
        //      is destroyed.
        //
        //  m_pwndEditor
        //      Our source editor window.
        // -------------------------------------------------------------------
        mutable tCIDLib::TBoolean   m_bChanged;
        tCIDLib::TBoolean           m_bSessionClass;
        tCIDLib::TStrList           m_colGotoList;
        tCIDMacroEng::EResModes        m_eMode;
        TNotHandlerBase*            m_pnothEdit;
        TSrcEditor*                 m_pwndEditor;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSrcTab, TTabWindow)
};

#pragma CIDLIB_POPPACK



