//
// FILE NAME: CIDCtrls_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/1997
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
//  This is the header file for the CIDCtrls_ThisFacility.cpp file. This
//  file implements the facility class for this facility.
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
//  A simple class for returning info about enumerated monitors
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGUIMonInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TGUIMonInfo();

        TGUIMonInfo
        (
            const   TString&            strDevName
            , const TArea&              areaPhys
            , const TArea&              areaAvail
            , const tCIDLib::TBoolean   bIsPrimary
        );

        ~TGUIMonInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsPrimary() const;

        const TArea& areaAvailable() const;

        const TArea& areaPhysical() const;

        const TString& strDevName() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TArea               m_areaAvail;
        TArea               m_areaPhys;
        tCIDLib::TBoolean   m_bIsPrimary;
        TString             m_strDevName;
};



// ---------------------------------------------------------------------------
//  CLASS: TFacCIDCtrls
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TFacCIDCtrls : public TGUIFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDCtrls();

        ~TFacCIDCtrls();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaDefWnd() const;

        tCIDLib::TBoolean bActivatePreviousInst
        (
            const   TString&                strWndText
            , const tCIDLib::TBoolean       bStartsWith = kCIDLib::False
        );

        tCIDLib::TVoid Activate
        (
            const   tCIDCtrls::TWndHandle   hwndTar
        );

        tCIDLib::TVoid AllowSetForeground();

        tCIDLib::TBoolean bAltShifted() const;

        tCIDLib::TBoolean bAnyShifts() const;

        tCIDLib::TBoolean bCapsLockState() const;

        tCIDLib::TBoolean bCtrlShifted() const;

        tCIDLib::TBoolean bIsOwnedBy
        (
            const   tCIDCtrls::TWndHandle   hwndToCheck
            , const tCIDCtrls::TWndHandle   hwndOwner
            , const tCIDLib::TBoolean       bImmediate
        )   const;

        tCIDLib::TBoolean bIsPointerVisible() const;

        tCIDLib::TBoolean bIsWindowValid
        (
            const   tCIDCtrls::TWndHandle   hwndToCheck
        )   const;

        tCIDLib::TBoolean bIsWindowVisible
        (
            const   tCIDCtrls::TWndHandle   hwndToCheck
        )   const;

        tCIDLib::TBoolean bMouseButtonDown
        (
            const   tCIDCtrls::EMouseButts  eToCheck
        )   const;

        tCIDLib::TBoolean bOpenFileDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInitPath
            ,       tCIDLib::TStrCollect&   colSelected
            , const tCIDLib::TKVPList&      colFileTypes
            , const tCIDCtrls::EFOpenOpts   eOptions
        );

        tCIDLib::TBoolean bOwnsActivePopups
        (
            const   tCIDCtrls::TWndHandle   hwndToCheck
            , const tCIDLib::TBoolean       bModalOnly
        )   const;

        tCIDLib::TBoolean bPopupOK() const;

        tCIDLib::TBoolean bScrollLockState() const;

        tCIDLib::TBoolean bShifted() const;

        tCIDLib::TBoolean bQueryNamedMonArea
        (
            const   TString&                strDevName
            ,       TArea&                  areaToFill
            , const tCIDLib::TBoolean       bAvailOnly
        );

        tCIDLib::TBoolean bSaveFileDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInitPath
            , const TString&                strInitName
            ,       TString&                strTarPath
            , const tCIDLib::TKVPList&      colFileTypes
            , const tCIDCtrls::EFSaveOpts   eOptions
        );

        tCIDLib::TBoolean bLoadSpecialText
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       TString&                strToFill
        );

        tCIDLib::TCard4 c4QueryWndStyles
        (
            const   tCIDCtrls::TWndHandle   hwndSrc
        )   const;

        tCIDLib::TCard4 c4QueryWndText
        (
            const   tCIDCtrls::TWndHandle   hwndSrc
            ,       TString&                strToFill
        );

        tCIDLib::TCard4 c4ScrollBarHeight() const;

        tCIDLib::TCard4 c4ScrollBarWidth() const;

        const tCIDLib::TKVPList& colDlgClasses() const;

        tCIDLib::TVoid ConfinePointer
        (
            const   TArea&                  areaOnScreen
        );

        tCIDCtrls::EWndAreas eGenericHitCalc
        (
            const   TPoint&                 pntToCheck
            , const TArea&                  areaToCheck
            , const tCIDLib::TCard4         c4BorderWidth
        );

        tCIDLib::TVoid ExitLoop
        (
            const   tCIDLib::TCard4         c4Code
        );

        tCIDLib::TVoid ExitLoop
        (
            const   tCIDLib::EExitCodes     eCode
        );

        tCIDLib::TVoid FlashWindow
        (
            const   tCIDCtrls::TWndHandle   hwndTarget
        );

        tCIDLib::TVoid HidePointer();

        tCIDCtrls::TWndHandle hwndChildFromId
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            , const tCIDCtrls::TWndId       widChildId
        );

        tCIDCtrls::TWndHandle hwndChildFromText
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            ,       tCIDCtrls::TWndId&      widChildId
            , const TString&                strText
            , const tCIDLib::TBoolean       bPrefix
            , const tCIDLib::TBoolean       bDirectChild
        );

        tCIDCtrls::TWndHandle hwndChildWithFocus
        (
            const   tCIDCtrls::TWndHandle   hwndParent
        );

        tCIDCtrls::TWndHandle hwndFindAppWnd
        (
            const   TExternalProcess&       extpTarget
            , const TString&                strWndText
            , const TString&                strClass
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDCtrls::TWndHandle hwndFindChild
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            , const tCIDCtrls::TWndId       widChildId
            , const tCIDLib::TBoolean       bDirectChild
        );

        tCIDCtrls::TWndHandle hwndFindTopAppWnd
        (
            const   TExternalProcess&       extpTarget
            , const TString&                strClass
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDCtrls::TWndHandle hwndNthChildFrom
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            , const tCIDLib::TBoolean       bFirst
            , const tCIDLib::TCard4         c4Ofs
        );

        tCIDCtrls::TWndHandle hwndRootOwner
        (
            const   tCIDCtrls::TWndHandle   hwndStart
        );

        tCIDCtrls::TWndHandle hwndTopParent
        (
            const   tCIDCtrls::TWndHandle   hwndStart
        );

        tCIDCtrls::TMsgResult mresSendAsyncMsg
        (
                    tCIDCtrls::TWndHandle   hwndTar
            , const tCIDCtrls::TWndMsg      wmsgToSend
            , const tCIDCtrls::TWParam      waram
            , const tCIDCtrls::TLParam      lParam
        );

        tCIDCtrls::TMsgResult mresSendSyncMsg
        (
                    tCIDCtrls::TWndHandle   hwndTar
            , const tCIDCtrls::TWndMsg      wmsgToSend
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        );

        tCIDLib::TVoid OffsetPointer
        (
            const   TPoint&                 pntAmount
        );

        TPoint pntPointerPos
        (
            const   TWindow&                wndRelativeTo
        )   const;

        tCIDLib::TVoid PrepForeignFocus
        (
            const   tCIDCtrls::TWndHandle   hwndCaller
            , const tCIDCtrls::TWndHandle   hwndTarget
        );

        tCIDLib::TVoid MovePointer
        (
            const   TPoint&                 pntNewPos
            , const TWindow&                wndRelativeTo
        );

        tCIDLib::TVoid MsgYield
        (
            const   tCIDLib::TCard4         c4WaitFor
        );

        const TWindow* pwndActiveTopWnd() const;

        tCIDLib::TVoid QueryMonitors
        (
                TCollection<TGUIMonInfo>&   colToFill
        );

        tCIDLib::TVoid QueryMonArea
        (
            const   TWindow&                wndSrc
            ,       TArea&                  areaToFill
            , const tCIDLib::TBoolean       bAvailOnly
        );

        TRGBClr rgbSysClr
        (
            const   tCIDCtrls::ESysColors   eClr
        )   const;

        tCIDLib::TVoid ReleasePointer();

        const TString& strXlatDlgClass
        (
            const   TString&                strResClassType
        )   const;

        tCIDLib::TVoid SendExtKey
        (
            const   tCIDCtrls::EExtKeys     eKeyToSend
            , const tCIDLib::TBoolean       bAltShifted
            , const tCIDLib::TBoolean       bCtrlShifted
            , const tCIDLib::TBoolean       bShifted
        );

        tCIDLib::TVoid SendKey
        (
            const   tCIDLib::TCh            chToSend
            , const tCIDLib::TBoolean       bAltShifted
            , const tCIDLib::TBoolean       bCtrlShifted
            , const tCIDLib::TBoolean       bShifted
        );

        tCIDLib::TVoid SendShutdown
        (
            const   tCIDCtrls::TWndHandle   hwndTar
        );

        tCIDLib::TVoid SetActiveTopWnd
        (
            const   TWindow* const          pwndToSet
        );

        tCIDLib::TVoid SetFocus
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const tCIDLib::TBoolean       bForce
        );

        tCIDLib::TVoid SetExtOwner
        (
            const   tCIDCtrls::TWndHandle   hwndOwned
            , const tCIDCtrls::TWndHandle   hwndOwner
        );

        tCIDLib::TVoid SetForeground
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const tCIDLib::TBoolean       bForce
        );

        tCIDLib::TVoid SetMonitorPowerState
        (
                    TWindow&                wndCaller
            , const tCIDCtrls::EMonPowerSt  eToSet
        );

        tCIDLib::TVoid SetPointer
        (
            const   tCIDCtrls::ESysPtrs eToSet
        );

        tCIDLib::TVoid SetPosState
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const tCIDCtrls::EPosStates   eToSet
        );

        tCIDLib::TVoid SetInitShowState
        (
            const   tCIDLib::TSInt          iToSet
        );

        tCIDLib::TVoid SetWndPos
        (
            const  tCIDCtrls::TWndHandle    hwndTar
            , const tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
        );

        tCIDLib::TVoid SetWndSize
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Height
        );

        tCIDLib::TVoid SetWndSize
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const TSize&                  szToSet
        );

        tCIDLib::TVoid SetWndText
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const TString&                strToSet
        );

        tCIDLib::TVoid ShowGUI
        (
                    TWindow&                wndMain
        );

        tCIDLib::TVoid ShowPointer();

        tCIDLib::TVoid ShowWindow
        (
            const   tCIDCtrls::TWndHandle   hwndTar
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid ToBottom
        (
            const   tCIDCtrls::TWndHandle   hwndTar
        );

        tCIDLib::TUInt uMainMsgLoop
        (
            const   TWindow&                wndMain
        );

        tCIDLib::TVoid ValidatePrevWindowArea
        (
                    TArea&                  areaToVal
        );

        tCIDLib::TVoid ValidatePrevWindowPos
        (
                    TPoint&                 pntToVal
        );

        tCIDCtrls::TWndId widFromHandle
        (
            const   tCIDCtrls::TWndHandle   hwndSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::TWndHandle hwndFindDescendant
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            , const tCIDCtrls::TWndId       widToFind
        );

        tCIDCtrls::TWndHandle hwndFindDescendant
        (
            const   tCIDCtrls::TWndHandle   hwndParent
            , const TString&                strToFind
            ,       tCIDLib::TCh* const     pszBuf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::TBoolean       bPrefix
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaDefWnd
        //      And area that can be passed to a top level window to have it just
        //      size and position itself to defaults selected by the system.
        //
        //  m_colDlgClasses
        //      A list of key/value pairs, that hold the dialog item class
        //      names. The key being the name in the resource file, and
        //      the value being the C++ class name. This is used in a few
        //      places, so we set up the info once and make it available.
        //
        //  m_iInitShow
        //      The initial show state. This is passed to us by the kicker macro
        //      for GUI programs (see the main hpp file of this facility.) It
        //      calls our SetShowState() method and we store the value. GUI apps,
        //      upon completing init, should call ShowGUI() and we will set this
        //      state as the initial visibility state.
        //
        //  m_pmiSpecialMsgs
        //      This is used to load the special text messages, if they are
        //      present.
        //
        //  m_pwndTopWnd
        //      Windows' msg dispatching is pretty brain dead and cannot handle more than
        //      one top level window at a time. So, we let top level windows set this
        //      pointer when they are activated, using SetActiveTopWnd(). Our main msg
        //      loop method uses this to dispatch msgs.
        // -------------------------------------------------------------------
        TArea                   m_areaDefWnd;
        tCIDLib::TKVPList       m_colDlgClasses;
        tCIDLib::TSInt          m_iInitShow;
        tCIDLib::TMsgIndex*     m_pmiSpecialMsgs;
        const TWindow*          m_pwndTopWnd;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDCtrls,TGUIFacility)
};


// ---------------------------------------------------------------------------
//  CLASS: TWndPtrJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TWndPtrJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWndPtrJanitor
        (
            const   tCIDCtrls::ESysPtrs eToSet
        );

        TWndPtrJanitor(const TWndPtrJanitor&);

        ~TWndPtrJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndPtrJanitor& operator=(const TWndPtrJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data membres
        //
        //  m_pOldInfo
        //      A pointer to the old pointer info. It's opaque here publically
        //      so that we don't have to expose any details of a pointer's
        //      representation.
        // -------------------------------------------------------------------
        tCIDLib::TVoid*     m_pOldInfo;
};


#pragma CIDLIB_POPPACK

