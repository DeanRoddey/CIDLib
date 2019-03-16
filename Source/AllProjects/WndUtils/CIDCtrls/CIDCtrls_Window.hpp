//
// FILE NAME: CIDCtrls_Window.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This file implements the base TWindow class from which all other window classes
//  are derived.
//
// CAVEATS/GOTCHAS:
//
//  1)  Window's version of Z-order is backwards from what any normal human would
//      assume, and what all other system's use. For Windows the bottom of the
//      z-order is closest to the viewer and the top the farthest away.
//
//      We hide this and make it work the other way around. So pwndNext() actually
//      gets the previous window. pwndToBottom() moves the the top as far as Windows
//      is concerned, etc...
//
//      Creating windows puts them at the bottom from Window's point of view, the
//      top from ours, which is what we want.
//
//  2)  For convenience and performance we want to be able to return window text as
//      a string object reference. So we have to play some tricks with window text.
//      For any custom windows, they can just use our m_strWndText member directly to
//      manage their text, so it's always immediately available.
//
//      For standard controls, they will overwrite our virtuals (WndTextPreQuery and
//      WndTextStore.) When the caller wants the window text, calls our strWndText()
//      method, we call the prequery to get the derived class' text into our text
//      member. If our setter is called, we call WndTextStore to let the derived class
//      update his control's internal text representation.
//
//      It's a bit of overhead but gives us a lot more control and consistency. The
//      TStdCtrlWnd class is a derivative that works for most all of the wrapped standard
//      controls all of whom derive from that class. It just implements the above two
//      methods in terms of the standard Windows get/set text stuff.
//
//      For windows with higher overhead text (lots of it) it would be best if they could
//      keep track of whether changes have occurred since the last pre-query call and
//      don't return the same text again if not. The text member is not cleared before
//      making that call for this reason. If the derived class doesn't change it, it
//      will keep what was returned last time.
//
//      Any with special needs will either derive from something else, or they will
//      override those methods and handle them specially.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TWndPaintJanitor;


// ---------------------------------------------------------------------------
//   CLASS: TCtrlNotify
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCtrlNotify : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public destructor (ctors are hidden)
        // -------------------------------------------------------------------
        ~TCtrlNotify();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::TWndId widSource() const;

        const TString& strSrcName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCtrlNotify();

        TCtrlNotify
        (
            const   TWindow&                wndSrc
        );

        TCtrlNotify
        (
            const   TCtrlNotify&            wnotToCopy
        );

        TCtrlNotify& operator=
        (
            const   TCtrlNotify&            wnotToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strSrcName
        //      The name of the source, which in most cases since it's a window
        //      is the name assigned to the window if any. Not the text, but the
        //      internal name. This is useful if a target is getting notifications
        //      from non-child windows, hence the ids may not be unique.
        // -------------------------------------------------------------------
        TString             m_strSrcName;
        tCIDCtrls::TWndId   m_widSource;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCtrlNotify,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TWindow : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid CheckHandlerRegParms
        (
            const   TWindow* const          pwndSrc
            , const TWindow* const          pwndTarget
        );

        static TWindow* pwndGetWndLinkPtr
        (
            const   tCIDCtrls::TWndHandle   hwndSrc
        );

        static const TString& strExtractName
        (
            const   TWindow&                wndSrc
        );

        static TFrameWnd& wndDesktop();


        // For internal use only
        static tCIDLib::TVoid ThrowBadCast
        (
            const   TString&                strSrcType
            , const TString&                strTarType
        );

        static tCIDLib::TVoid ThrowNoChildWithId
        (
            const   tCIDCtrls::TWndId       widNotFound
        );


        // -------------------------------------------------------------------
        // Destructor. Ctors are hidden
        // -------------------------------------------------------------------
        TWindow(const TWindow&) = delete;

        ~TWindow();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const TWindow& operator=(const TWindow&) = delete;


        // -------------------------------------------------------------------
        //  Public template members that we need as a practical matter to avoid a lot
        //  error prone grunt work by other code. These let them get child controls
        //  as their actual types, with type validation in debug mode.
        // -------------------------------------------------------------------
        template <class T> T*
        pwndChildAs(const tCIDCtrls::TWndId widChild) const
        {
            TWindow* pwndRet = pwndChildById(widChild);
            #if CID_DEBUG_ON
            if (pwndRet && !pwndRet->bIsA(T::clsThis()))
                ThrowBadCast(pwndRet->clsIsA().strClassName(), T::clsThis().strClassName());
            #endif
            return static_cast<T*>(pwndRet);
        }

        template <class T> T* pwndNamed(const TString& strToFind)
        {
            TWindow* pwndRet = pwndFindNamed(strToFind);
            #if CID_DEBUG_ON
            if (pwndRet && !pwndRet->bIsA(T::clsThis()))
                ThrowBadCast(pwndRet->clsIsA().strClassName(), T::clsThis().strClassName());
            #endif
            return static_cast<T*>(pwndRet);
        }

        template <class T> T& wndNamed(const TString& strToFind)
        {
            TWindow& wndRet = wndFindNamed(strToFind);
            #if CID_DEBUG_ON
            if (!wndRet.bIsA(T::clsThis()))
                ThrowBadCast(wndRet.clsIsA().strClassName(), T::clsThis().strClassName());
            #endif
            return static_cast<T&>(wndRet);
        }


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const;

        virtual tCIDLib::TVoid ChildFocusChange
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bGotFocus
        );

        virtual tCIDLib::TVoid SetBgnColor
        (
            const   TRGBClr&                rgbToSet
            , const tCIDLib::TBoolean       bOver = kCIDLib::True
            , const tCIDLib::TBoolean       bRedraw = kCIDLib::False
        );

        virtual tCIDLib::TVoid SetWndStyle
        (
            const   tCIDCtrls::EWndStyles   eToSet
            , const tCIDLib::TBoolean       bSetIt
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TArea areaClient() const;

        TArea areaWnd() const;

        TArea areaWndSize() const;

        tCIDLib::TVoid AddNotifyHandler
        (
                    TNotHandlerBase* const  pnothToAdopt
            , const tCIDLib::EEnds          eWhichEnd = tCIDLib::EEnds::Last
        );

        tCIDLib::TVoid AddNotifyRequest
        (
                    TNotRequestBase* const  pnotrToAdopt
        );

        tCIDLib::TVoid AutoAdjustChildren
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
        );

        tCIDLib::TVoid AdjustZOrder
        (
            const   tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bForward
        );

        tCIDLib::TBoolean bBgnColorOver() const;

        tCIDLib::TBoolean bContainsFocus() const;

        tCIDLib::TBoolean bDragDetect
        (
            const   TPoint&                 pntFrom
        );

        tCIDLib::TBoolean bExStyleOn
        (
            const   tCIDCtrls::EExWndStyles eToCheck
        )   const;

        tCIDLib::TBoolean bHasChildWithId
        (
            const   tCIDCtrls::TWndId       widToCheck
        )   const;

        tCIDLib::TBoolean bHasFocus() const;

        tCIDLib::TBoolean bHasMouseCapture() const;

        tCIDLib::TBoolean bInDragMode() const;

        tCIDLib::TBoolean bInTrackingMode() const;

        tCIDLib::TBoolean bIsChild
        (
            const   TWindow&                wndTest
        )   const;

        tCIDLib::TBoolean bIsDirectChild
        (
            const   TWindow&                wndTest
        )   const;

        tCIDLib::TBoolean bIsEnabled() const;

        tCIDLib::TBoolean bIsInModalLoop() const;

        tCIDLib::TBoolean bIsThisWindow
        (
            const   TWindow&                wndTest
        )   const;

        tCIDLib::TBoolean bIsTopLevel() const;

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TBoolean bIsVisible
        (
            const   tCIDLib::TBoolean       bCheckValid = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bPaintSuppressed() const;

        tCIDLib::TBoolean bPropagateKeys() const;

        tCIDLib::TBoolean bPropagateKeys
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bSendMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgToSend
            , const tCIDLib::TVoid* const   pMsgData
            , const tCIDLib::ESyncTypes     eSyncType = tCIDLib::ESyncTypes::Asynchronous
        );

        tCIDLib::TBoolean bStyleOn
        (
            const   tCIDCtrls::EWndStyles   eToCheck
        )   const;

        tCIDLib::TBoolean bTimerIsActive
        (
            const   tCIDCtrls::TTimerId     tmidCheck
        );

        tCIDLib::TBoolean bTimerSuspend() const;

        tCIDLib::TBoolean bTimerSuspend
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bTrackArea
        (
            const   TPoint&                 pntStart
            ,       TArea&                  areaTrack
            , const tCIDLib::TCard4         c4BorderWidth
            , const TArea&                  areaConfine
            , const tCIDLib::TBoolean       bAllowInvert = kCIDLib::False
        );

        tCIDLib::TBoolean bTrackRegion
        (
            const   TPoint&                 pntStart
            ,       TGUIRegion&             grgnTrack
            , const TArea&                  areaConfine
        );

        tCIDLib::TBoolean bWantsChildFocusNot() const;

        tCIDLib::TBoolean bWantsChildFocusNot
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4AdjustScrollPos
        (
            const   tCIDLib::TBoolean       bHorz
            , const tCIDLib::TBoolean       bInc
        );

        tCIDLib::TCard4 c4CurScrollPos
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TCard4 c4MaxScrollPos
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TCard4 c4UserId() const;

        tCIDLib::TCard4 c4UserId
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid CaptureMouse();

        tCIDLib::TVoid ClearText();

        tCIDLib::TVoid Destroy();

        tCIDLib::TVoid DestroyAllChildren();

        tCIDLib::TVoid DestroyChild
        (
            const   tCIDCtrls::TWndId       widChild
        );

        tCIDCtrls::EModalRes eDoModalLoop
        (
                    tCIDLib::TBoolean&      bBreakFlag
            , const tCIDCtrls::EModalFlags  eFlags
            ,       TWindow* const          pwndAltTarget = nullptr
            , const tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxCard
        );

        tCIDCtrls::EEdgeAnchors eEdgeAnchor() const;

        tCIDCtrls::EEdgeAnchors eEdgeAnchor
        (
            const   tCIDCtrls::EEdgeAnchors eToSet
        );

        tCIDCtrls::EExWndStyles eExWndStyles() const;

        tCIDCtrls::EPosStates ePosState() const;

        tCIDCtrls::EPosStates ePosState
        (
                    TArea&                  areaNormal
        )   const;

        tCIDCtrls::EWndStyles eWndStyles() const;

        tCIDLib::TVoid ForceRepaint();

        tCIDLib::TVoid FromScreenCoordinates
        (
            const   TArea&                  areaSrc
            ,       TArea&                  areaTarget
        )   const;

        tCIDLib::TVoid FromScreenCoordinates
        (
            const   TPoint&                 pntSrc
            ,       TPoint&                 pntTarget
        )   const;

        tCIDCtrls::TWndHandle hwndThis() const;

        tCIDCtrls::TWndHandle hwndSafe() const;

        tCIDLib::TVoid InsertAfter
        (
            const   TWindow&                wndInsertAfter
        );

        tCIDLib::TVoid InvalidateArea
        (
            const   TArea&                  areaToInvalidate
            , const tCIDLib::TBoolean       bEraseBgn = kCIDLib::True
        );

        tCIDLib::TVoid InvalidateArea
        (
            const   tCIDCtrls::TWndId       widChild
            , const TArea&                  areaToInvalidate
            , const tCIDLib::TBoolean       bEraseBgn = kCIDLib::True
        );

        tCIDLib::TVoid InvalidateArea
        (
            const   TPoint&                 pntUL
            , const TPoint&                 pntLR
            , const tCIDLib::TBoolean       bEraseBgn = kCIDLib::True
        );

        tCIDLib::TVoid InvalidateArea
        (
            const   tCIDCtrls::TWndId       widChild
            , const TPoint&                 pntUL
            , const TPoint&                 pntLR
            , const tCIDLib::TBoolean       bEraseBgn = kCIDLib::True
        );

        tCIDLib::TVoid InvalidateRegion
        (
            const   TGUIRegion&             grgnToInvalidate
            , const tCIDLib::TBoolean       bEraseBgn = kCIDLib::True
        );

        tCIDLib::TVoid MoveAndSize
        (
            const   TArea&                  areaNew
        );

        tCIDLib::TVoid MakeScrollPosVisible
        (
            const   tCIDLib::TBoolean       bHorz
            , const tCIDLib::TCard4         c4Pos
            , const tCIDLib::TBoolean       bMinMove
        );

        TPoint pntOrg() const;

        TPoint pntOrg
        (
            const   tCIDCtrls::TWndId       widChild
        )   const;

        TWindow* pwndBottomChild() const;

        TWindow* pwndChildById
        (
            const   tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        TWindow* pwndChildByUserId
        (
            const   tCIDLib::TCard4         c4UserId
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        TWindow* pwndChildWithFocus() const;

        TWindow* pwndFindNamed
        (
            const   TString&                strToFind
        )   const;

        TWindow* pwndFirstOfGroup
        (
            const   TWindow&                wndStartAt
        )   const;

        TWindow* pwndFirstOfNextGroup
        (
            const   TWindow&                wndStartAt
        )   const;

        TWindow* pwndFindFirstTab() const;

        TWindow* pwndNextChild
        (
            const   TWindow&                wndCurrent
        )   const;

        TWindow* pwndParent() const;

        TWindow* pwndPrevChild
        (
            const   TWindow&                wndCurrent
        )   const;

        TWindow* pwndRoot() const;

        TWindow* pwndTopChild() const;

        tCIDLib::TVoid PauseTimer
        (
            const   tCIDCtrls::TTimerId     tmidTar
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid QueryClientArea
        (
                    TArea&                  areaToFill
            , const tCIDLib::TBoolean       bAvailOnly
            , const tCIDCtrls::TWndList&    colSkip
        )   const;

        tCIDLib::TVoid QueryClientArea
        (
                    TArea&                  areaToFill
            , const tCIDLib::TBoolean       bAvailOnly
            , const TWindow&                wndSkip
        )   const;

        tCIDLib::TVoid QueryClientArea
        (
                    TArea&                  areaToFill
            , const tCIDLib::TBoolean       bAvailOnly
        )   const;

        tCIDLib::TVoid QueryScrollInfo
        (
            const   tCIDLib::TBoolean       bHorz
            ,       tCIDLib::TCard4&        c4CurPos
            ,       tCIDLib::TCard4&        c4MaxPos
            ,       tCIDLib::TCard4&        c4PageSz
        );

        TRGBClr rgbBgnFill() const;

        tCIDLib::TVoid RegisterNamedWnd
        (
            const   TString&                strName
            ,       TWindow* const          pwndToReg
        );

        tCIDLib::TVoid Redraw
        (
            const   tCIDCtrls::ERedrawFlags eFlags
        );

        tCIDLib::TVoid Redraw
        (
            const   TArea&                  areaToRedraw
            , const tCIDCtrls::ERedrawFlags eFlags
        );

        tCIDLib::TVoid Redraw
        (
            const   TGUIRegion&             grgnToRedraw
            , const tCIDCtrls::ERedrawFlags eFlags
        );

        tCIDLib::TVoid ReleaseMouse();

        tCIDLib::TVoid RemoveNotifyHandler
        (
                    TNotHandlerBase* const  pnothToRemove
        );

        tCIDLib::TVoid ResetTimer
        (
            const   tCIDCtrls::TTimerId     tmidTar
            , const tCIDLib::TCard4         c4WaitMillis
        );

        const TString& strChildText
        (
            const   tCIDCtrls::TWndId       widChild
        )   const;

        const TString& strChildText
        (
            const   tCIDCtrls::TWndId       widChild
            , const TString&                strNewText
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );

        const TString& strWndText() const;

        const TString& strWndText
        (
            const   TString&                strNewText
        );

        tCIDLib::TVoid Scroll
        (
            const   TArea&                  areaToScroll
            , const TArea&                  areaClip
            ,       TArea&                  areaInvalid
            , const tCIDLib::TInt4          i4HorzAmount
            , const tCIDLib::TInt4          i4VertAmount
            , const tCIDLib::TBoolean       bInvalidate
        );

        tCIDLib::TVoid Scroll
        (
            const   TArea&                  areaToScroll
            ,       TArea&                  areaInvalid
            , const tCIDLib::TInt4          i4HorzAmount
            , const tCIDLib::TInt4          i4VertAmount
            , const tCIDLib::TBoolean       bInvalidate
        );

        tCIDLib::TVoid SendCode
        (
            const   tCIDLib::TInt4          i4ToPost
            , const tCIDLib::TCard4         c4Data
            , const tCIDLib::ESyncTypes     eSync = tCIDLib::ESyncTypes::Asynchronous
        );

        tCIDLib::TVoid SendData
        (
            const   tCIDLib::TCard4         c4DataId
            ,       tCIDLib::TVoid* const   pToSend
            , const tCIDLib::ESyncTypes     eSync = tCIDLib::ESyncTypes::Asynchronous
        );

        tCIDLib::TVoid SetChildFocus
        (
            const   tCIDCtrls::TWndId       widTarget
        );

        tCIDLib::TVoid SetEnable
        (
            const   tCIDCtrls::TWndId       widChlid
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid SetEnable
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid SetExWndStyle
        (
            const   tCIDCtrls::EExWndStyles eToSet
            , const tCIDLib::TBoolean       bSetIt
        );

        tCIDLib::TVoid SetFont
        (
            const   TGUIFont&               gfontToSet
        );

        tCIDLib::TVoid SetIcon
        (
            const   TIcon&                  icoToSet
            , const tCIDLib::TBoolean       bLarge
        );

        tCIDLib::TVoid SetId
        (
            const   tCIDCtrls::TWndId       widToSet
        );

        tCIDLib::TVoid SetOwner
        (
            const   TWindow&                wndToSet
        );

        tCIDLib::TVoid SetParent
        (
            const   TWindow&                wndToSet
        );

        tCIDLib::TVoid SetPos
        (
            const   TPoint&                 pntNewOrg
        );

        tCIDLib::TVoid SetScrollBar
        (
            const   tCIDLib::TBoolean       bHorz
            , const tCIDLib::TCard4         c4FullSize
            , const tCIDLib::TCard4         c4VisSize
            , const tCIDLib::TCard4         c4LineSz
        );

        tCIDLib::TVoid SetScrollPos
        (
            const   tCIDLib::TBoolean       bHorz
            , const tCIDLib::TCard4         c4Pos
        );

        tCIDLib::TVoid SetSize
        (
            const   TSize&                  szNew
            , const tCIDLib::TBoolean       bCopyBits
        );

        tCIDLib::TVoid SetSize
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Height
            , const tCIDLib::TBoolean       bCopyBits
        );

        tCIDLib::TVoid SetSizePos
        (
            const   TArea&                  areaNew
            , const tCIDLib::TBoolean       bCopyBits
        );

        tCIDLib::TVoid SetVisibility
        (
            const   tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bState
            , const tCIDLib::TBoolean       bActivate = kCIDLib::True
        );

        tCIDLib::TVoid SetVisibility
        (
            const   tCIDLib::TBoolean       bState
            , const tCIDLib::TBoolean       bActivate = kCIDLib::True
        );

        tCIDLib::TVoid SetZOrder
        (
            const   TWindow&                wndInsertAfter
        );

        tCIDLib::TVoid StopAllTimers();

        tCIDLib::TVoid StopTimer
        (
            const   tCIDCtrls::TTimerId     tmidTar
        );

        tCIDCtrls::TTimerId tmidStartTimer
        (
            const   tCIDLib::TCard4         c4WaitMillis
            , const tCIDLib::TBoolean       bStartPaused = kCIDLib::False
        );

        tCIDLib::TVoid TakeFocus();

        tCIDLib::TVoid ToBottom();

        tCIDLib::TVoid ToScreenCoordinates
        (
            const   TArea&                  areaSrc
            ,       TArea&                  areaTarget
        )   const;

        tCIDLib::TVoid ToScreenCoordinates
        (
            const   TPoint&                 pntSrc
            ,       TPoint&                 pntTarget
        )   const;

        tCIDLib::TVoid ToTop
        (
            const   tCIDLib::TBoolean       bActivate = kCIDLib::False
        );

        tCIDCtrls::TWndId widThis() const;

        TWindow& wndChildById
        (
            const   tCIDCtrls::TWndId       widChild
        );

        TWindow& wndParent() const;

        TWindow& wndRoot() const;

        tCIDLib::TVoid XlatCoordinates
        (
                    TArea&                  areaToXlat
            , const TWindow&                wndSrc
        )   const;

        tCIDLib::TVoid XlatCoordinates
        (
                    TPoint&                 pntToXlat
            , const TWindow&                wndSrc
        )   const;


        // Public, but for internal use
        tCIDCtrls::EEvResponses eInvokeHandlers
        (
            const   TNotificationId&        nidBeingSent
            ,       TObject* const          pobjBeingSent
        );

        tCIDLib::TVoid SetParentSub();

        TWindow& wndFindNamed
        (
            const   TString&                strToFind
        );





        //
        //  These are templatized so that they can create the event wrapper object
        //  themselves and assue correct management of the caller's data. For asyncs,
        //  the caller should provide an allocated object. For sync, they are responsible
        //  for it, and can get back information from any handlers (though if
        //  multiple handlers, they will all see the same object.)
        //
        //  Both call a non-templatized helper to do the actual work.
        //
        template <class D> tCIDLib::TVoid
        SendAsyncNotify(        D* const            pwnotToAdopt
                        , const TNotificationId&    nidToSend) const
        {
            //
            //  Tell the wrapper class to adopt the object, since the caller has
            //  created a new one, and we will make other copies as required to send
            //  to registered listeners.
            //
            TWndEventFor<D> wnotSend(nidToSend, pwnotToAdopt, tCIDLib::EAdoptOpts::Adopt);
            DoAsyncNotification(wnotSend);
        }

        template <class D> tCIDLib::TVoid
        SendSyncNotify(         D&                  wnotToSend
                        , const TNotificationId&    nidToSend)
        {
            //
            //  Tell the wrapper class not to adopt, since the caller's owns the data
            //  in this case.
            //
            TWndEventFor<D> wnotSend(nidToSend, &wnotToSend, tCIDLib::EAdoptOpts::NoAdopt);
            DoSyncNotification(wnotSend);
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend tCIDCtrls::TMsgResult WNDPROCAPI mresCIDCtrlsDispatch
        (
            const   tCIDCtrls::TWndHandle   hwndThis
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      iParam
        );

        friend class TCaretJanitor;
        friend class TWndPaintJanitor;


        // -------------------------------------------------------------------
        // Hidden constructors
        // -------------------------------------------------------------------
        explicit TWindow
        (
            const   tCIDLib::TBoolean       bDlgProc
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        );

        virtual tCIDLib::TBoolean bChar
        (
            const   tCIDLib::TCh            chChar
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        );

        virtual tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        );

        virtual tCIDLib::TBoolean bCreated();

        virtual tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        );

        virtual tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        );

        virtual tCIDLib::TBoolean bExtKey
        (
            const   tCIDCtrls::EExtKeys     eKey
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        );

        virtual tCIDLib::TBoolean bFilesDropped
        (
            const   tCIDLib::TStrList&      colFiles
        );

        virtual tCIDLib::TBoolean bMediaKey
        (
            const   tCIDCtrls::EExtKeys     eExtKey
            , const tCIDLib::TBoolean       bAltShift
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        );

        virtual tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bControlDown
            , const tCIDLib::TBoolean       bShiftDown
        );

        virtual tCIDLib::TBoolean bMouseWheel
        (
            const   tCIDLib::TInt4          i4Clicks
        );

        virtual tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        );

        virtual tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        );

        virtual tCIDLib::TBoolean bSetChildPointer
        (
            const   tCIDCtrls::EWndAreas    eArea
            , const TWindow&                wndChild
        );

        virtual tCIDLib::TBoolean bSetPointer
        (
            const   tCIDCtrls::EWndAreas    eArea
        );

        virtual tCIDLib::TBoolean bShowHelp
        (
            const   tCIDCtrls::TWndId       widCtrl
            , const TPoint&                 pntMouse
        );

        virtual tCIDLib::TBoolean bVisibilityChange
        (
            const   tCIDLib::TBoolean       bShowing
        );

        virtual tCIDLib::TVoid ChildAreaChanged
        (
            const   tCIDCtrls::TWndId       widChild
            , const TArea&                  areaNew
        );

        virtual tCIDLib::TVoid CodeReceived
        (
            const   tCIDLib::TInt4          i4Code
            , const tCIDLib::TCard4         c4Data
        );

        virtual tCIDLib::TVoid CustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Id
            , const tCIDCtrls::ECustDrFlags eFlags
            , const TArea&                  areaTar
        );

        virtual tCIDLib::TVoid DataReceived
        (
            const   tCIDLib::TCard4         c4DataId
            ,       tCIDLib::TVoid* const   pData
        );

        virtual tCIDLib::TVoid Destroyed();

        virtual tCIDLib::TVoid DisplayChanged
        (
            const   tCIDLib::TCard4         c4NewBitsPerPel
            , const TSize&                  szNew
        );

        virtual tCIDLib::TVoid DragEnd();

        virtual tCIDLib::TVoid DragStart();

        virtual tCIDCtrls::EMouseActs eMouseActivate();

        virtual tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        );

        virtual tCIDLib::TVoid EnableStateChanged();

        virtual tCIDLib::TVoid GettingFocus();

        virtual tCIDLib::TVoid LosingFocus();

        virtual tCIDCtrls::TMsgResult mresDispatch
        (
            const   TWindow&                wndThis
            , const tCIDCtrls::TWndMsg      wmsgCur
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      iParam
        );

        virtual tCIDLib::TVoid MeasureItem
        (
            const   tCIDCtrls::TWndId       widCtrl
            , const tCIDLib::TCard4         c4ItemId
            ,       tCIDLib::TCard4&        c4Height
        );

        virtual tCIDLib::TVoid MediaChange
        (
            const   tCIDCtrls::EMediaChTypes eType
        );

        virtual tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        );

        virtual tCIDLib::TVoid ModDlgItem
        (
                    TDlgItem&               dlgiToMod
        );

        virtual tCIDLib::TVoid MouseExit();

        virtual tCIDLib::TVoid PowerStateChange
        (
            const   tCIDLib::TBoolean       bResuming
        );

        virtual tCIDLib::TVoid ScrollEvent
        (
            const   tCIDCtrls::EScrollEvs   eEvent
            , const tCIDLib::TBoolean       bHorizontal
        );

        virtual tCIDLib::TVoid ScrollDrag
        (
            const   tCIDLib::TBoolean       bHorizontal
            , const tCIDLib::TCard4         c4Position
            , const tCIDLib::TBoolean       bFinal
        );

        virtual tCIDLib::TVoid StaticChildClick
        (
            const   tCIDCtrls::TWndId       widChild
            , const tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
        );

        virtual tCIDLib::TVoid TrackingAreaChanged
        (
            const   TArea&                  areaNew
        );

        virtual tCIDLib::TVoid Timer
        (
            const   tCIDCtrls::TTimerId     tmidSrc
        );

        virtual tCIDLib::TVoid UserMsg
        (
            const   tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        );

        virtual tCIDLib::TVoid WndTextPreQuery
        (
                    TString&                strToFill
        )   const;

        virtual tCIDLib::TVoid WndTextStore
        (
            const   TString&                strToStore
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasRequestFor
        (
            const   TNotificationId&        nidToTest
        )   const;

        tCIDLib::TBoolean bMarqueeSelect
        (
            const   TPoint&                 pntStart
            ,       TArea&                  areaTrack
            , const TArea&                  areaConfine
        );

        tCIDLib::TCard4 c4CreateDlgBox
        (
            const   TWindow&                wndOwner
            , const TDlgDesc&               dlgdCreate
        );

        tCIDLib::TVoid CheckWindowHandle() const;

        tCIDLib::TVoid Cleanup();

        tCIDLib::TVoid CreateCaret
        (
            const   TSize&                  szInit
        );

        tCIDLib::TVoid CreateDlgItems
        (
            const   TDlgDesc&               dlgdInfo
            ,       tCIDLib::TCard4&        c4InitFocus
        );

        tCIDLib::TVoid CreateWnd
        (
            const   tCIDCtrls::TWndHandle   hwndOwnerOrParent
            , const tCIDLib::TCh* const     pszClass
            , const tCIDLib::TCh* const     pszText
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles
            , const tCIDCtrls::TWndId       widThis
        );

        tCIDLib::TVoid DecModalCounter();

        tCIDLib::TVoid DisableNotReflection();

        tCIDLib::TVoid DoAsyncNotification
        (
            const   TWndEvent&              wevToSend
        )   const;

        tCIDLib::TVoid DoSyncNotification
        (
            const   TWndEvent&              wevToSend
        );

        tCIDLib::TVoid DrawBorder
        (
                    TGraphDrawDev&          gdevTar
            , const TArea&                  areaTar
            , const tCIDLib::TBoolean       bInner
            , const tCIDLib::TBoolean       bRaised
        );

        tCIDCtrls::EExtKeys eMapExtKey
        (
            const   tCIDLib::TCard4         c4VKey
            , const tCIDLib::TBoolean       bShifted
        );

        const TGUIBrush& gbrBgn() const;

        tCIDCtrls::TWndHandle hwndChildById
        (
            const   tCIDCtrls::TWndId       widToFind
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid HandleNotification
        (
            const   TWindow&                wndThis
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        );

        tCIDLib::TVoid IncModalCounter();

        tCIDLib::TVoid KillCaret();

        tCIDCtrls::TMsgResult mresCallSubClass
        (
            const   tCIDCtrls::TWndMsg      wmsgCur
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        );

        tCIDLib::TVoid MinimalRedraw
        (
            const   TSize&                  szNew
            , const TSize&                  szLast
        );

        TWindow* pwndFindOwner
        (
            const   tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        TWindow* pwndFindParent
        (
            const   tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        tCIDLib::TVoid PassStaticChildClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
        );

        tCIDLib::TVoid RegWndClass
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TBoolean       bHVRedraw
            , const tCIDLib::TBoolean       bDblClicks
            , const tCIDLib::TCard4         c4Cursor
            , const TRGBClr&                rgbBgn
            , const tCIDLib::TBoolean       bDlgProc
        );

        tCIDLib::TVoid SendChildFocusNot
        (
            const   TWindow&                wndTar
            , const TWindow&                wndParent
            , const tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bGetting
        );

        tCIDLib::TVoid SetCaretAt
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid SetCaretVis
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid SetSBarPos
        (
            const   tCIDLib::TBoolean       bHorz
            , const tCIDLib::TCard4         c4Pos
        );

        tCIDLib::TVoid SetSubclass();

        tCIDLib::TVoid SetWantInput
        (
            const   tCIDCtrls::EWantInputs  eToSet
        );

        tCIDLib::TVoid StoreHandle
        (
            const   tCIDCtrls::TWndHandle   hwndToStore
        );

        tCIDLib::TVoid SuppressPaint
        (
            const   tCIDLib::TBoolean       bIncrement
        );

        tCIDLib::TVoid UseHandle
        (
            const   tCIDCtrls::TWndHandle   hwndToUse
            , const tCIDLib::TBoolean       bAdopt
            , const tCIDLib::TBoolean       bSubClass
        );


    private :
        // -------------------------------------------------------------------
        //  A structure that we use to track timers. We allow up to a particular
        //  number of times per window. The derived class will ask to create one
        //  and we find a free slot. We create the timer and return to them a
        //  an id to use with it subsequently.
        //
        //  We use ids that (minus 1) are the index into the list, to make it
        //  efficient and easy.
        // -------------------------------------------------------------------
        struct TTimer
        {
            tCIDLib::TBoolean   bEntered;
            tCIDLib::TBoolean   bPaused;
            tCIDLib::TBoolean   bStarted;
        };



        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleFileDrop
        (
            const   tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        );

        tCIDLib::TVoid CheckTimerId
        (
            const   tCIDCtrls::TTimerId     tmidBad
        )   const;

        tCIDLib::TVoid CheckTimerId
        (
            const   tCIDCtrls::TTimerId     tmidBad
            , const tCIDLib::TBoolean       bCheckActive
        )   const;

        tCIDLib::TVoid DoCleanup();

        tCIDLib::TVoid SetIsDialog();

        tCIDLib::TVoid XlatButtonMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgCur
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::EMouseButts& eButton
            ,       tCIDCtrls::EMouseClicks& eClick
            ,       TPoint&                 pntAt
            ,       tCIDLib::TBoolean&      bCtrlShift
            ,       tCIDLib::TBoolean&      bShift
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaPrev
        //      We remember the current area so that, upon getting a size/pos change
        //      notification the previous info is available. So we update this
        //      upon return from telling the window about the size change, therefore
        //      it's always the previous size before the change.
        //
        //  m_aTimers
        //      A list of TTimer structures, that we use to track timers created
        //      by this window.
        //
        //  m_bAreaTracking
        //      This is set if we are currently tracking an area drag.
        //
        //  m_bBgnClrOver
        //      Normally, we let the background color setting messages go to the
        //      parent window that they are sent to. But, if our own background color
        //      has been explicitly set, this flag is set, so that we instead let
        //      it reflect back to the sending control so the set color gets used.
        //
        //  m_bDlgProc
        //      Indicates we want unhandled msgs to go to the dialog proc instead
        //      of the window proc.
        //
        //  m_bDragMode
        //      This is set while a size/move drag is happening on this window.
        //
        //  m_bIsDialog
        //      Indicates if this is a dialog window. m_bDlgProc above indicates if
        //      the window wants to use the def dialog proc, which doesn't mean it
        //      is a dialog box. In fact it means it's not one, because you can't
        //      call that from a dialog. If this is set, we don't pass on to any
        //      default handler, we just return a value. It is set if we get a
        //      WM_INITDIALOG msg.
        //
        //  m_bNoReflect
        //      In some cases, such as where a window is not wrapping a standard control
        //      but embedding one, we don't want notifications to be reflected. We want
        //      them to go to the target, since it will be the wrapper. Otherwise, no one
        //      would see them since the embedded control doesn't handle them.
        //
        //  m_bOwnsHandle
        //      In some cases we have to set up window objects around windows we
        //      don't own. So we don't want to try to destroy it.
        //
        //  m_bPropagateKeys
        //      This defaults to false. But it can be set to cause us to pass any
        //      keys to the parent if the derived class doesn't handle it or passes it
        //      down to us. If this is set, then 'parent' versions of the keyboard input
        //      methods are called.
        //
        //  m_bWantsChildFocusNot
        //      Wants to be notified of child focus changes. This will be sent via an
        //      async post that will come to us and we'll call ChildFocusChange(). This
        //      insures that focus change has completed before it is called.
        //
        //  m_c4EnteredLoops
        //      Any time one of our modal loop methods is called, this is bumped
        //      while we are in it. This lets us provide info to the outside
        //      world as to whether we are currently being modally processed.
        //      Since they can be recursive, we have ot use a counter and bump
        //      it up and down.
        //
        //  m_c4SuppressPaint
        //      This is used to supress redrawing during complex repositioning
        //      operations, or updates to complex controls. It is incremented
        //      and decremented by handling the WM_SETREDRAW message, which we
        //      also allow to pass on to the original handler. So, for code at
        //      our level, they can call bPaintSuppressed() to check if they
        //      should draw or not. For standard controls that we sub-class they
        //      can still handle it at their level as well.
        //
        //  m_c4TimerSuspend
        //      This flag will stop calls to the eTimer() callback, to supress
        //      all timers. We provide janitor (below) to do this on a scoped
        //      basis. When it's zero, timers are allowed, else they are not
        //      passed on to the window object.
        //
        //  m_c4UserId
        //      An id that can be assigned to windows purely for the use of the
        //      client code. We don't force them to be unique, the client code must
        //      do that if it's important. We provide a method to find a child by
        //      id. Generally they only need to be unique within a given parent, but
        //      that's up to the client code.
        //
        //  m_eEdgeAnchor
        //      If this is not None, then this will be used to auto-move/resize this
        //      window if the parent calls AutoAdjustChildren after its size changes.
        //
        //  m_eLastPosState
        //      The last pos state we saw, so that we know when it changes and only bother
        //      to call the derived window if it has.
        //
        //  m_eWantInput
        //      Indicates what type of inputs we want. This allows the system to
        //      pass on unwanted input to possible special handlers. This is defaulted
        //      to EWantInput_None. Call SetWantInput to set it to something else.
        //      This only applies to custom windows. Wrapped system controls will
        //      return what they want, though you could of course handle
        //      bEatSubClassMsg for one and force it to something different.
        //
        //  m_gbrBgn
        //      This is used to fill our bacgkround and to set the background color
        //      of child controls, so that they match. By default it's set to the
        //      standard background color from the faciliyt class, but can be set
        //      via SetBgnColor().
        //
        //  m_hwndThis
        //      The handle to our window. If zero, then it's not be created yet.
        //
        //  m_pfOrgProc
        //      We provide our derived classes the option to sub-class standard
        //      controls, via the SetSubClass() method. It subclasses the window
        //      to our internal msg dispatcher. If this is non-zero, then any
        //      unhandled msgs are passed to this instead of the default proc.
        //
        //  m_strName
        //      A name that can be applied to any windows, which has nothing to
        //      do with the window's functionality, it is just for the client
        //      code's housekeeping purposes. We provide a static method that will
        //      extract this in the way required to create a keyed collection of
        //      windows, which is often useful.
        //
        //  m_strWndText
        //      We store the text here, or for standard controls it's a copy of the
        //      text. This allows us to access it in a natural, efficient way by
        //      providing access to this string. See the header comments.
        //
        //      It has to be mutable so that we can load it from a derived class
        //      during the getting if needed.
        //
        //  m_widThis
        //      The window id for this window. Only meaningful really if it's
        //      a child window.
        // -------------------------------------------------------------------
        TArea                       m_areaPrev;
        TTimer                      m_aTimers[kCIDCtrls::c4MaxWndTimers];
        tCIDLib::TBoolean           m_bAreaTracking;
        tCIDLib::TBoolean           m_bBgnClrOver;
        tCIDLib::TBoolean           m_bDlgProc;
        tCIDLib::TBoolean           m_bDragMode;
        tCIDLib::TBoolean           m_bIsDialog;
        tCIDLib::TBoolean           m_bNoReflect;
        tCIDLib::TBoolean           m_bOwnsHandle;
        tCIDLib::TBoolean           m_bPropagateKeys;
        tCIDLib::TBoolean           m_bWantsChildFocusNot;
        tCIDLib::TCard4             m_c4EnteredLoops;
        tCIDLib::TCard4             m_c4SuppressPaint;
        tCIDLib::TCard4             m_c4TimerSuspend;
        tCIDLib::TCard4             m_c4UserId;
        tCIDCtrls::EEdgeAnchors     m_eEdgeAnchor;
        tCIDCtrls::EPosStates       m_eLastPosState;
        tCIDCtrls::EWantInputs      m_eWantInput;
        TSolidBrush                 m_gbrBgn;
        tCIDCtrls::TWndHandle       m_hwndThis;
        tCIDCtrls::TKeyedWndList*   m_pcolNamedWnds;
        tCIDCtrls::TDispatchFunc    m_pfOrgProc;
        TNotHandlerBase*            m_pnothHead;
        TNotRequestBase*            m_pnotrHead;
        TString                     m_strName;
        mutable TString             m_strWndText;
        tCIDCtrls::TWndId           m_widThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWindow, TObject)
        NulObject(TWindow)
};


// ---------------------------------------------------------------------------
//  Since is VERY common to need to get a child window, and downcast it to its
//  actual type, we provide this specialized template function to do it. This
//  can all by done using the standard CIDLib RTTI, but we really need to
//  reduce the verbosity for something that happens this often.
//
//  A second one is provided for scenarions where they just want a temp cast
//  pointer.
// ---------------------------------------------------------------------------
template <class T>
tCIDLib::TVoid CastChildWnd(        TWindow&            wndParent
                            , const tCIDCtrls::TWndId   widChild
                            ,       T*&                 pwndToFill)
{
    // Get the child window, throw if no child with that id
    TWindow* pwndChild = wndParent.pwndChildById(widChild);
    if (!pwndChild)
        TWindow::ThrowNoChildWithId(widChild);

    // Try the dynamic cast. If it fails, throw
    pwndToFill = dynamic_cast<T*>(pwndChild);
    if (!pwndToFill)
        TWindow::ThrowBadCast(pwndChild->clsIsA().strClassName(), T::clsThis().strClassName());
}



// ---------------------------------------------------------------------------
//  A janitor to hide the caret on a scoped basis.
// ---------------------------------------------------------------------------
class TCaretJanitor
{
    public :
        TCaretJanitor(TWindow* const pwndTar) :

            m_pwndTar(pwndTar)
        {
            if (pwndTar)
                pwndTar->SetCaretVis(kCIDLib::False);
        }

        TCaretJanitor(const TCaretJanitor&) = delete;

        ~TCaretJanitor()
        {
            if (m_pwndTar && m_pwndTar->bIsValid())
                m_pwndTar->SetCaretVis(kCIDLib::True);
        }

        TCaretJanitor& operator=(const TCaretJanitor&) = delete;

        tCIDLib::TVoid Orphan()
        {
            m_pwndTar = nullptr;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TWindow*    m_pwndTar;
};



// ---------------------------------------------------------------------------
//  A janitor to capture the mouse on a scoped bseis
// ---------------------------------------------------------------------------
class TMouseCapJanitor
{
    public :
        TMouseCapJanitor(TWindow* const pwndTar) :

            m_pwndTar(pwndTar)
        {
            if (pwndTar)
                pwndTar->CaptureMouse();
        }

        TMouseCapJanitor(const TMouseCapJanitor&) = delete;

        ~TMouseCapJanitor()
        {
            if (m_pwndTar && m_pwndTar->bIsValid())
                m_pwndTar->ReleaseMouse();
        }

        TMouseCapJanitor& operator=(const TMouseCapJanitor&) = delete;

    private :
        TWindow* m_pwndTar;
};



// ---------------------------------------------------------------------------
//  A specialized janitor for windows, because we want to call the Destroy()
//  method before deleting it, if its been creatded. Its very simple and call
//  inlined.
// ---------------------------------------------------------------------------
template <class T> class TWndJanitor
{
    public :
        TWndJanitor(T* const pwndToSanitize) :

            m_pwndToSanitize(pwndToSanitize)
        {
        }

        TWndJanitor(const TWndJanitor&) = delete;

        ~TWndJanitor()
        {
            try
            {
                if (m_pwndToSanitize)
                {
                    if (m_pwndToSanitize->bIsValid())
                        m_pwndToSanitize->Destroy();
                    delete m_pwndToSanitize;
                }
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        TWndJanitor& operator=(const TWndJanitor&) = delete;

        tCIDLib::TVoid Orphan()
        {
            m_pwndToSanitize = nullptr;
        }

        T* pwndOrphan()
        {
            T* pwndRet = m_pwndToSanitize;
            m_pwndToSanitize = nullptr;
            return pwndRet;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        T*  m_pwndToSanitize;
};


// ---------------------------------------------------------------------------
//   CLASS: TLocalTimerJanitor
//  PREFIX: jan
//
//  Starts a timer on a scoped basis and makes sure it goes away when the scope
//  exists.
// ---------------------------------------------------------------------------
class TLocalTimerJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TLocalTimerJanitor(TWindow* const pwndTar, const tCIDLib::TCard4 c4Millis) :

            m_tmidTar(0)
            , m_pwndTar(pwndTar)
        {
            m_tmidTar = pwndTar->tmidStartTimer(c4Millis);
        }

        TLocalTimerJanitor(const TLocalTimerJanitor&) = delete;

        ~TLocalTimerJanitor()
        {
            if (m_pwndTar && m_pwndTar->bIsValid() && (m_tmidTar != 0))
                m_pwndTar->StopTimer(m_tmidTar);
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocalTimerJanitor& operator=(const TLocalTimerJanitor&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::TTimerId tmidThis() const
        {
            return m_tmidTar;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TWindow*            m_pwndTar;
        tCIDCtrls::TTimerId m_tmidTar;
};



// ---------------------------------------------------------------------------
//   CLASS: TTimerJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class TTimerJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TTimerJanitor(          TWindow* const      pwndToSanitize
                        , const tCIDCtrls::TTimerId tmidPause) :

            m_tmidPause(tmidPause)
            , m_pwndToSanitize(pwndToSanitize)
        {
            m_pwndToSanitize->PauseTimer(m_tmidPause, kCIDLib::True);
        }

        TTimerJanitor(const TTimerJanitor&) = delete;

        ~TTimerJanitor()
        {
            if (m_pwndToSanitize && m_pwndToSanitize->bIsValid())
                m_pwndToSanitize->PauseTimer(m_tmidPause, kCIDLib::False);
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTimerJanitor& operator=(const TTimerJanitor&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_pwndToSanitize = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TWindow*            m_pwndToSanitize;
        tCIDCtrls::TTimerId m_tmidPause;
};


// ---------------------------------------------------------------------------
//   CLASS: TTimerSusJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class TTimerSusJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TTimerSusJanitor(TWindow* const pwndToSanitize) :

            m_pwndToSanitize(pwndToSanitize)
        {
            if (m_pwndToSanitize)
                m_pwndToSanitize->bTimerSuspend(kCIDLib::True);
        }

        TTimerSusJanitor(const TTimerSusJanitor&) = delete;

        ~TTimerSusJanitor()
        {
            if (m_pwndToSanitize)
                m_pwndToSanitize->bTimerSuspend(kCIDLib::False);
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTimerSusJanitor& operator=(const TTimerSusJanitor&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_pwndToSanitize = nullptr;
        }

        tCIDLib::TVoid Release()
        {
            if (m_pwndToSanitize)
                m_pwndToSanitize->bTimerSuspend(kCIDLib::False);
        }

        tCIDLib::TVoid Reacquire()
        {
            if (m_pwndToSanitize)
                m_pwndToSanitize->bTimerSuspend(kCIDLib::True);
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowUnderflow();


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TWindow*    m_pwndToSanitize;
};


// ---------------------------------------------------------------------------
//  A janitor to hide a window on a scoped basis.
// ---------------------------------------------------------------------------
class TWndVisJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TWndVisJanitor(TWindow* const pwndToHide) :

            m_bWasVis(kCIDLib::False)
            , m_pwndToHide(pwndToHide)
        {
            if (pwndToHide)
            {
                m_bWasVis = pwndToHide->bIsVisible();
                if (m_bWasVis)
                    pwndToHide->SetVisibility(kCIDLib::False);
            }
        }

        TWndVisJanitor(const TWndVisJanitor&) = delete;

        ~TWndVisJanitor()
        {
            if (m_pwndToHide && m_bWasVis)
            {
                if (m_pwndToHide->bIsValid())
                    m_pwndToHide->SetVisibility(kCIDLib::True);
            }
        }

        TWndVisJanitor& operator=(const TWndVisJanitor&) = delete;

        tCIDLib::TVoid Orphan()
        {
            m_pwndToHide = nullptr;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bWasVis;
        TWindow*            m_pwndToHide;
};



// ---------------------------------------------------------------------------
//  A janitor to suppress drawing for a window on a scoped basis
// ---------------------------------------------------------------------------
class TWndPaintJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TWndPaintJanitor(TWindow* const pwndTar) : m_pwndTar(pwndTar)
        {
            if (pwndTar)
                pwndTar->SuppressPaint(kCIDLib::True);
        }

        TWndPaintJanitor(const TWndPaintJanitor&) = delete;

        ~TWndPaintJanitor()
        {
            if (m_pwndTar)
                m_pwndTar->SuppressPaint(kCIDLib::False);
        }

        TWndPaintJanitor& operator=(const TWndPaintJanitor&) = delete;

    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TWindow*    m_pwndTar;
};


#pragma CIDLIB_POPPACK


