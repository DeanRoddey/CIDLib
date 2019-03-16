//
// FILE NAME: TestCIDCtrls.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/97
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
//  This is the main header of the program. It brings in any needed
//  underlying headers, and any program headers.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDCtrls.hpp"
// #include    "CIDWUtils.hpp"
#include    "TestCIDCtrls_ResourceIds.hpp"



// ----------------------------------------------------------------------------
//  Global data from TestCIDCtrls.Cpp
// ----------------------------------------------------------------------------
extern TGUIFacility facTestCIDCtrls;



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
//
//  Our main client window. We use a standard frame window object, and we set one
//  of these as the client on the frame.
// ----------------------------------------------------------------------------
class TTestFrameWnd : public TFrameWnd, public MDragAndDrop
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestFrameWnd();

        ~TTestFrameWnd();


        tCIDLib::TVoid DragExit() override;

        tCIDLib::TBoolean bCanAcceptFiles
        (
            const   tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   const override;

        tCIDLib::TBoolean bDragDropped
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   override;

        tCIDLib::TBoolean bDragEnter
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   override;

        tCIDLib::TBoolean bDragOver
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   override;


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

        tCIDLib::TBoolean bAllowShutdown() override;

        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid Destroyed() override;

        tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        )   override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TTestFrameWnd(const TTestFrameWnd&);
        tCIDLib::TVoid operator=(const TTestFrameWnd&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eEFHandler
        (
                    TEFChangeInfo&          wnotEvent
        );

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDCtrls::EEvResponses eSliderHandler
        (
                    TSliderChangeInfo&      wnotEvent
        );

        tCIDCtrls::EEvResponses eTreeHandler
        (
                    TTreeEventInfo&         wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //
        // -------------------------------------------------------------------
        tCIDCtrls::TWndId   m_widCancel;
        tCIDCtrls::TWndId   m_widCheckBox;
        tCIDCtrls::TWndId   m_widEntryFld;
        tCIDCtrls::TWndId   m_widMCListBox;
        tCIDCtrls::TWndId   m_widOK;
        tCIDCtrls::TWndId   m_widTabs;
        tCIDCtrls::TWndId   m_widTest;
        tCIDCtrls::TWndId   m_widTreeView;

        TActivityBar*       m_pwndActBar;
        TCalendar*          m_pwndCalendar;
        TPushButton*        m_pwndCancel;
        TCheckBox*          m_pwndCheckBox;
        TComboBox*          m_pwndComboBox;
        TEntryField*        m_pwndEntryFld;
        TImgPushButton*     m_pwndImgButton;
        TListBox*           m_pwndListBox;
        TMultiEdit*         m_pwndMultiEdit;
//        TCheckedMCListWnd*  m_pwndMCListBox;
        TMultiColListBox*   m_pwndMCListBox;
        TObjView*           m_pwndObjView;
        TPushButton*        m_pwndOK;
        TProgressBar*       m_pwndProgBar;
        TSlider*            m_pwndSlider;
        TStaticImg*         m_pwndStaticImg;
        TStatusBar*         m_pwndStatusBar;
        TTreeView*          m_pwndTree;
        TStaticMultiText*   m_pwndMultiStaticText;
        TStaticText*        m_pwndStaticText;
        TTabbedWnd*         m_pwndTabs;
        TPushButton*        m_pwndTest;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};

