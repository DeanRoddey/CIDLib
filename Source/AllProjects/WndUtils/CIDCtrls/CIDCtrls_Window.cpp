//
// FILE NAME: CIDCtrls_Window.cpp
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
//  This file implements the
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"
#include    <shellapi.h>
#include    <Dbt.h>


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCtrlNotify,TObject)
RTTIDecls(TWindow, TObject)



// ---------------------------------------------------------------------------
//  A simple class to store a window handle to window object map item. We have one of these
//  per window currently opened in the process. The window handle is the key field.
//
//  Because the key of a keyed hash set has to be an object, we have to provide a small
//  class to go around that that does nothing.
// ---------------------------------------------------------------------------
class TWndMapItemH : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWndMapItemH() : m_hwndThis(kCIDCtrls::hwndInvalid)
        {
        }

        TWndMapItemH(const tCIDCtrls::TWndHandle hwndToSet) : m_hwndThis(hwndToSet)
        {
        }

        TWndMapItemH(const TWndMapItemH& wmihSrc) : m_hwndThis(wmihSrc.m_hwndThis)
        {
        }

        ~TWndMapItemH() {}


        tCIDCtrls::TWndHandle   m_hwndThis;

    private :
        RTTIDefs(TWndMapItemH, TObject)
};

RTTIDecls(TWndMapItemH, TObject)

class TWndMapItem
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TWndMapItemH& wmihKey(const TWndMapItem& wmiSrc)
        {
            return wmiSrc.m_wmihKey;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWndMapItem() :

            m_pwndWnd(nullptr)
        {
        }

        TWndMapItem(const tCIDCtrls::TWndHandle hwndWnd, TWindow* const pwndWnd) :

            m_pwndWnd(pwndWnd)
            , m_wmihKey(hwndWnd)
        {
        }

        TWndMapItem(const TWndMapItem& wmiSrc) :

            m_pwndWnd(wmiSrc.m_pwndWnd)
            , m_wmihKey(wmiSrc.m_wmihKey)
        {
        }

        ~TWndMapItem()
        {
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndMapItem& operator=(const TWndMapItem& wmiSrc)
        {
            if (this != &wmiSrc)
            {
                m_pwndWnd   = wmiSrc.m_pwndWnd;
                m_wmihKey   = wmiSrc.m_wmihKey;
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::TWndHandle hwndThis()
        {
            return m_wmihKey.m_hwndThis;
        }

        tCIDLib::TVoid Reset()
        {
            m_wmihKey.m_hwndThis = kCIDCtrls::hwndInvalid;
            m_pwndWnd = nullptr;
        }


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_pwndWnd
        //      The pointer to the window object for this window.
        //
        //  m_wmihKey
        //      The window handle, in it's little wrapper
        // -------------------------------------------------------------------
        TWindow*        m_pwndWnd;
        TWndMapItemH    m_wmihKey;
};


// A simple key ops handler for our window map item class, whcih doesn't have to do much
class TWndMapItemKeyOps
{
    public :
        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys(const   TWndMapItemH&   wmih1
                                    , const TWndMapItemH&   wmih2)const
        {
            return (wmih1.m_hwndThis == wmih2.m_hwndThis);
        }

        tCIDLib::THashVal hshKey(const  TWndMapItemH&       wmihToHash
                                , const tCIDLib::TCard4     c4Modulus) const
        {
            return tCIDLib::TCard4(wmihToHash.m_hwndThis) % c4Modulus;
        }
};


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCtrls_Window
{
    namespace
    {
        // ------------------------------------------------------------------------
        //  We have to be able to map from window handles (which we get in the window proc) and
        //  our window objects. Storing a pointer in the window's window's extra data is kludgey
        //  and can't tell the difference between non-CIDLib windows and CIDLib windows until
        //  its too late. So we need a mapping mechanism.
        //
        //  So we just use a keyed hash set.
        //
        //  Syncrhonization is not required since this map is only accessed from window messages
        //  and they are serialized within a single process.
        // ------------------------------------------------------------------------
        using TWndItemMap = TRefKeyedHashSet<TWndMapItem, TWndMapItemH, TWndMapItemKeyOps>;
        TWndItemMap& colWndItemMap()
        {
            static TWndItemMap* pcolMap = new TWndItemMap
            (
                tCIDLib::EAdoptOpts::Adopt
                , 1024
                , TWndMapItemKeyOps()
                , &TWndMapItem::wmihKey
            );
            return *pcolMap;
        }
    }
}




// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------

//
//  Translate an app command key to one of our extended key enum values
//
static tCIDCtrls::EExtKeys eXlatAppComamnd(const tCIDCtrls::TLParam lParam)
{
    switch(GET_APPCOMMAND_LPARAM(lParam))
    {
        case APPCOMMAND_BROWSER_BACKWARD :
            return tCIDCtrls::EExtKeys::Browser_Back;
            break;

        case APPCOMMAND_BROWSER_FAVORITES :
            return tCIDCtrls::EExtKeys::Browser_Favorites;
            break;

        case APPCOMMAND_BROWSER_FORWARD :
            return tCIDCtrls::EExtKeys::Browser_Forward;
            break;

        case APPCOMMAND_BROWSER_HOME :
            return tCIDCtrls::EExtKeys::Browser_Home;
            break;

        case APPCOMMAND_BROWSER_REFRESH :
            return tCIDCtrls::EExtKeys::Browser_Refresh;
            break;

        case APPCOMMAND_BROWSER_SEARCH :
            return tCIDCtrls::EExtKeys::Browser_Search;
            break;

        case APPCOMMAND_BROWSER_STOP :
            return tCIDCtrls::EExtKeys::Browser_Stop;
            break;

        case APPCOMMAND_VOLUME_DOWN :
            return tCIDCtrls::EExtKeys::Volume_Down;
            break;

        case APPCOMMAND_VOLUME_MUTE :
            return tCIDCtrls::EExtKeys::Volume_Mute;
            break;

        case APPCOMMAND_VOLUME_UP :
            return tCIDCtrls::EExtKeys::Volume_Up;
            break;

        case APPCOMMAND_MEDIA_NEXTTRACK :
            return tCIDCtrls::EExtKeys::Media_NextTrack;
            break;

        case APPCOMMAND_MEDIA_PLAY_PAUSE :
            return tCIDCtrls::EExtKeys::Media_PlayPause;
            break;

        case APPCOMMAND_MEDIA_PREVIOUSTRACK :
            return tCIDCtrls::EExtKeys::Media_PrevTrack;
            break;

        case APPCOMMAND_MEDIA_STOP :
            return tCIDCtrls::EExtKeys::Media_Stop;
            break;

        default :
            break;
    };
    return tCIDCtrls::EExtKeys::None;
}


//
//  We have to use a separate procedure for dialogs because there is no way to
//  otherwise know whether to call defaults or not. We can only know that once
//  the window
//
//
tCIDCtrls::TMsgResult CALLBACK
mresCIDCtrlsDispatch(const  tCIDCtrls::TWndHandle   hwndThis
                    , const tCIDLib::TUInt          uMsg
                    , const tCIDCtrls::TWParam      wParam
                    , const tCIDCtrls::TLParam      lParam)
{
    //
    //  The WM_MOUSELEAVE msg only works during tracking, but we want to be able to
    //  always sense the mouse exiting a window because it's very useful. So we just
    //  do it ourself. On any mouse move we see what the handle is. If it's not the
    //  one it was in before, we see if the previous one is one of our windows and
    //  call his mouse exit. So we have to statically remember a window handle.
    //
    static tCIDCtrls::TWndHandle hwndLastMouse = kCIDCtrls::hwndInvalid;

    //
    //  We have to deal with bootstrapping here, and get the window tracking
    //  structure created and stored away on the window object first of all,
    //  so we handle this one message here. It will be seen again by the window
    //  class' dispatcher after we get done with it, and he will call the
    //  window's Created() method.
    //
    if ((uMsg == WM_INITDIALOG) || (uMsg == WM_CREATE))
    {
        // Store a pointer to our window structure in the extra window data
        TWndMapItem* pwmiData = new TWndMapItem(hwndThis, nullptr);
        if (uMsg == WM_CREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pwmiData->m_pwndWnd = (TWindow*)pCreate->lpCreateParams;
        }
         else
        {
            pwmiData->m_pwndWnd = (TWindow*)lParam;

            //
            //  Mark this window as a dialog. This must be done here on the first
            //  msg we'll see for dialog boxes. It insures we don't call any default
            //  handlers.
            //
            pwmiData->m_pwndWnd->SetIsDialog();
        }

        CIDCtrls_Window::colWndItemMap().Add(pwmiData);

        // And store the window handle on the window object
        pwmiData->m_pwndWnd->StoreHandle(hwndThis);
    }

    // Get the window object for this window, if any
    TWindow* pwndTar = TWindow::pwndGetWndLinkPtr(hwndThis);

    if (uMsg == WM_MOUSEMOVE)
    {
        if (hwndThis != hwndLastMouse)
        {
            if (hwndLastMouse != kCIDCtrls::hwndInvalid)
            {
                TWindow* pwndTmp = TWindow::pwndGetWndLinkPtr(hwndLastMouse);
                if (pwndTmp)
                {
                    try
                    {
                        pwndTmp->MouseExit();
                    }

                    catch(TError& errToCatch)
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }

                    catch(...)
                    {
                    }
                }
            }

            // Store this one as a new last mouse
            hwndLastMouse = hwndThis;
        }
    }

    //
    //  If its one of our windows, then send it to the window object to process,
    //  else pass it to the default.
    //
    tCIDCtrls::TMsgResult mresRet = 0;
    if (pwndTar)
    {
        //
        //  Pass it on to the derived class to process. Catch any exceptions
        //  because we can't allow them to propogate in the GUI thread. If
        //  the application has registered an error handler, invoke that.
        //
        try
        {
            mresRet = pwndTar->mresDispatch(*pwndTar, uMsg, wParam, lParam);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            TString strMsg(errToCatch.strErrText());
            if (errToCatch.bHasAuxText())
            {
                strMsg.Append(L"\n\n");
                strMsg.Append(errToCatch.strAuxText());
            }
            TErrBox msgbErr(L"Unhandled exception in GUI Thread", strMsg);
            msgbErr.ShowIt(*pwndTar);
        }

        catch(TKrnlError& kerrToCatch)
        {
            TString strErrText(L"KrnlErr=");
            strErrText.AppendFormatted(kerrToCatch.errcId());
            strErrText.Append(L", HostErr=");
            strErrText.AppendFormatted(kerrToCatch.errcHostId());

            TErrBox msgbErr
            (
                L"Unhandled CIDKernel exception in GUI Thread"
                , strErrText
            );
            msgbErr.ShowIt(*pwndTar);
        }

        catch(...)
        {
            TErrBox msgbErr
            (
                L"Unhandled system exception in GUI Thread"
                , L"No information available"
            );
            msgbErr.ShowIt(*pwndTar);
        }


        //
        //  If this is a destroy, we need to clean up our window memory. But also give the
        //  window object a chance to clean up. It could be used again, so need to make sure
        //  it gets set to a 'not created' state.
        //
        if (uMsg == WM_DESTROY)
        {
            //
            //  Just to help make sure things go smoothly. See if this is the currently
            //  set top level active window. If so, then zero it out until some other
            //  window activates and sets itself.
            //
            if (pwndTar == facCIDCtrls().pwndActiveTopWnd())
                facCIDCtrls().SetActiveTopWnd(nullptr);

            try
            {
                // Do the cleanup on this window now
                pwndTar->DoCleanup();
            }

            catch(TError& errToCatch)
            {
                if (!errToCatch.bLogged())
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }
            }

            //
            //  Now clear up the window and remove from the window map. Note that this will
            //  NOT delete the window itself. This window may not be dynamically allocated.
            //  Each window that creates children is responsible for cleaning them up.
            //
            if (!CIDCtrls_Window::colWndItemMap().bRemoveKey(hwndThis, kCIDLib::False))
            {
                #if CID_DEBUG_ON
                TAudio::Beep(880, 50);
                #endif
            }

            // And let the window clean itself up
            pwndTar->Cleanup();
        }
    }
     else
    {
        //
        //  Until we get the init/create, pass anything to the window proc, not
        //  the dialog proc, even if we ultimately are a dialog. If we don't do
        //  this, the init dlg msg will have a null window handle.
        //
        mresRet = ::DefWindowProc(hwndThis, uMsg, wParam, lParam);
    }
    return mresRet;
}



// ---------------------------------------------------------------------------
//   CLASS: TCtrlNotify
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCtrlNotify: Public destructor (ctors are hidden)
// ---------------------------------------------------------------------------
TCtrlNotify::~TCtrlNotify()
{
}


// ---------------------------------------------------------------------------
//  TCtrlNotify: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::TWndId TCtrlNotify::widSource() const
{
    return m_widSource;
}


const TString& TCtrlNotify::strSrcName() const
{
    return m_strSrcName;
}


// ---------------------------------------------------------------------------
//  TCtrlNotify: Hidden constructors and operators
// ---------------------------------------------------------------------------
TCtrlNotify::TCtrlNotify() :

   m_widSource(0)
{
}

TCtrlNotify::TCtrlNotify(const TWindow& wndSrc) :

   m_widSource(wndSrc.widThis())
   , m_strSrcName(wndSrc.strName())
{
}




// ---------------------------------------------------------------------------
//   CLASS: TimerSusJanitor
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TimerSusJanitor: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTimerSusJanitor::ThrowUnderflow()
{
    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcTimer_SuspUnderflow
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppError
    );
}



// ---------------------------------------------------------------------------
//  CLASS: TWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWindow: Public, static methods
// ---------------------------------------------------------------------------

//
//  Check that both the source and target of a notification is non-null, and any
//  other checks we can do. This is called when a notification is being registered.
//
tCIDLib::TVoid
TWindow::CheckHandlerRegParms(  const   TWindow* const  pwndSrc
                                , const TWindow* const  pwndTarget)
{
    if (!pwndSrc || !pwndTarget)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNot_NullTargetOrSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
}


// Lazy eval for the null window object
TWindow& TWindow::Nul_TWindow()
{
    static TWindow wndNull(kCIDLib::False);
    return wndNull;
}


//
//  See if the passed window is one of our windows, by seeing if it's in our window map.
//
TWindow* TWindow::pwndGetWndLinkPtr(const tCIDCtrls::TWndHandle hwndSrc)
{
    TWndMapItem* pwmiCur = CIDCtrls_Window::colWndItemMap().pobjFindByKey
    (
        hwndSrc, kCIDLib::False
    );
    if (pwmiCur)
        return pwmiCur->m_pwndWnd;

    return nullptr;
}



// Allows these to be used in a keyed collection
const TString& TWindow::strExtractName(const TWindow& wndSrc)
{
    return wndSrc.m_strName;
}


// Provides a frame window wrapper around the desktop
TFrameWnd& TWindow::wndDesktop()
{
    static TFrameWnd* m_pwndDesktop = nullptr;
    if (!TAtomic::pFencedGet<TFrameWnd>(&m_pwndDesktop))
    {
        TBaseLock lockInit;
        if (!TAtomic::pFencedGet<TFrameWnd>(&m_pwndDesktop))
        {
            TFrameWnd* pwndNew = new TFrameWnd;

            //
            //  Note that this adds it to the window list. We tell it not to
            //  adopt the handle.
            //
            pwndNew->UseHandle
            (
                ::GetDesktopWindow(), kCIDLib::False, kCIDLib::False
            );
            TAtomic::FencedSet<TFrameWnd>(&m_pwndDesktop, pwndNew);
        }
    }
    return *m_pwndDesktop;
}



// For internal use
tCIDLib::TVoid
TWindow::ThrowBadCast(const TString& strSrcType, const TString& strTarType)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcRTTI_BadCast
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::TypeMatch
        , strSrcType
        , strTarType
    );
}

// For internal use
tCIDLib::TVoid TWindow::ThrowNoChildWithId(const tCIDCtrls::TWndId widNotFound)
{
    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcWnd_NoChildWithId
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , TCardinal(widNotFound)
    );
}




// ---------------------------------------------------------------------------
// TWindow: Destructor. Ctors are hidden
// ---------------------------------------------------------------------------
TWindow::~TWindow()
{
    //
    //  If we created a named window collection, clean it up. It doesn't own its
    //  contents, so there's no danger of errors.
    //
    if (m_pcolNamedWnds)
    {
        delete m_pcolNamedWnds;
        m_pcolNamedWnds = nullptr;
    }
}


// ---------------------------------------------------------------------------
// TWindow: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  Called to see if this window has an accelerator table that maps the incoming
//  msg. If not overridden, we just say no.
//
tCIDLib::TBoolean TWindow::bProcessAccel(const tCIDLib::TVoid* const) const
{
    return kCIDLib::False;
}


// Empty default implementation
tCIDLib::TVoid TWindow::ChildFocusChange(const  TWindow&
                                        , const tCIDCtrls::TWndId
                                        , const tCIDLib::TBoolean)
{
}


//
//  Set the background color brush for this window. It can optionally only set the
//  color if it's not been overridden already. This lets parents set their children's
//  colors, but not interfere with ones that have set their color explicitly.
//
tCIDLib::TVoid
TWindow::SetBgnColor(const  TRGBClr&            rgbToSet
                    , const tCIDLib::TBoolean   bOver
                    , const tCIDLib::TBoolean   bRedraw)
{
    // If not overriding and it's already been manually set, then do nothing
    if (!bOver && m_bBgnClrOver)
        return;

    // And now set our color and remember it's been manually set
    m_gbrBgn.SetColor(rgbToSet);
    m_bBgnClrOver = kCIDLib::True;

    if (bRedraw)
        ForceRepaint();
}


//
//  Set the state of a window style, after the fact. This can be overridden if the
//  derived class needs to react to such changes. They have to call us first if they
//  want it applied to themselves. Sometimes they want to pass it on to a nested
//  class.
//
tCIDLib::TVoid
TWindow::SetWndStyle(const  tCIDCtrls::EWndStyles   eToSet
                    , const tCIDLib::TBoolean       bSetIt)
{
    tCIDLib::TCard4 c4Cur = ::GetWindowLong(hwndSafe(), GWL_STYLE);

    if (bSetIt)
        c4Cur |= tCIDLib::TCard4(eToSet);
    else
        c4Cur &= ~tCIDLib::TCard4(eToSet);

    ::SetWindowLong(hwndThis(), GWL_STYLE, c4Cur);
}



// ---------------------------------------------------------------------------
// TWindow: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get our client area, which will be zero origined
TArea TWindow::areaClient() const
{
    RECT rectCur;
    if (!::GetClientRect(m_hwndThis, &rectCur))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_QueryClientArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
    TArea areaRet;
    areaRet.FromRectl(*(tCIDLib::THostRectl*)&rectCur);
    return areaRet;
}


// Get our area relative to our parent
TArea TWindow::areaWnd() const
{
    // Get the window rect, which is relative to the screen
    RECT rectCur;
    if (!::GetWindowRect(m_hwndThis, &rectCur))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_QueryArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }

    //
    //  Translate to it's parent's client coordinates if not the desktop.
    //  We also have to make sure that what we are getting really isn't the
    //  owner. If this is a top level window, then we may get an owner.
    //  So we check to see if the child style is on. If so, then we get
    //  the parent.
    //
    if (!bIsTopLevel())
    {
        tCIDCtrls::TWndHandle hwndPar = ::GetAncestor(m_hwndThis, GA_PARENT);
        if (hwndPar)
        {
            ::ScreenToClient(hwndPar, (POINT*)&rectCur);
            ::ScreenToClient(hwndPar, (POINT*)&rectCur.right);
        }
    }

    TArea areaRet;
    areaRet.FromRectl(*(tCIDLib::THostRectl*)&rectCur);
    return areaRet;
}


//
//  Get our full window size. So it's areaWnd() but with a zero origin
//
TArea TWindow::areaWndSize() const
{
    RECT rectCur;
    if (!::GetWindowRect(m_hwndThis, &rectCur))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_QueryArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
    TArea areaRet;
    areaRet.FromRectl(*(tCIDLib::THostRectl*)&rectCur);
    areaRet.ZeroOrg();
    return areaRet;
}


//
//  These support the notification system. Allowing the installation of a
//  handler for a kind of event, or the desire to send out an event, on
//  this window.
//
tCIDLib::TVoid
TWindow::AddNotifyHandler(          TNotHandlerBase* const  pnothToAdopt
                            , const tCIDLib::EEnds          eEnd)
{
    //
    //  The next node should not be set on this object. If so, then its
    //  already installed somewhere probably.
    //
    if (pnothToAdopt->m_pnothNext)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNot_NotHandlerInUse
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , TString(pnothToAdopt->nidRequested())
        );
    }

    //
    //  Add this new guy at the head or tail of the list, as requested by
    //  the caller. If the list is empty, then just do the simpler
    //  scenario regardless.
    //
    if ((eEnd == tCIDLib::EEnds::First) || !m_pnothHead)
    {
        pnothToAdopt->m_pnothNext = m_pnothHead;
        m_pnothHead = pnothToAdopt;
    }
     else
    {
        // Move up to the end of the list and add at the end
        TNotHandlerBase* pnothCur = m_pnothHead;
        while (pnothCur->m_pnothNext)
            pnothCur = pnothCur->m_pnothNext;
        pnothCur->m_pnothNext = pnothToAdopt;
    }
}


tCIDLib::TVoid TWindow::AddNotifyRequest(TNotRequestBase* const pnotrToAdopt)
{
    //
    //  The next node should not be set on this object. If so, then its
    //  already installed somewhere probably.
    //
    if (pnotrToAdopt->m_pnotrNext)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNot_NotRequestInUse
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , TString(pnotrToAdopt->nidRequested())
        );
    }

    //
    //  Make sure that there isn't already a request on this window for the
    //  same notification id. If so, then we don't want to add another, since
    //  that would cause multiple sends of the same event.
    //
    TNotRequestBase* pnotrCur = m_pnotrHead;
    while (pnotrCur)
    {
        // We already have one, so just return now
        if (pnotrCur->nidRequested() == pnotrToAdopt->nidRequested())
            return;
        pnotrCur = pnotrCur->m_pnotrNext;
    }

    // Add this new guy at the head of the list
    pnotrToAdopt->m_pnotrNext = m_pnotrHead;
    m_pnotrHead = pnotrToAdopt;
}



//
//  Moves a child window of ours forward or back in the z order, if it's not already
//  at the end of the line in the direction requested.
//
//  Keep in mind that Windows' view of Z-order is backwards. So forward means previous
//  to Windows, and vice versa.
//
tCIDLib::TVoid
TWindow::AdjustZOrder(  const   tCIDCtrls::TWndId   widChild
                        , const tCIDLib::TBoolean   bForward)
{
    // See if we have such a child window, throw if not
    TWindow* pwndTar = pwndChildById(widChild, kCIDLib::True);

    //
    //  Get the farthest window from the viewer, which is the last window in
    //  Windows' view.
    //
    tCIDCtrls::TWndHandle hwndFind = ::GetWindow(m_hwndThis, GW_CHILD);

    //
    //  It shouldn't be possible not to find one, since we checked above
    //  that the passed window is a child of ours. So clearly we have at
    //  least one.
    //
    CIDAssert(hwndFind != kCIDCtrls::hwndInvalid, L"Did not find a first child window");

    //
    //  Now we we work our way up to the child window they want to adjust. We
    //  remember the last window so that, if we need to go that direction, we can
    //  use it as the insert point.
    //
    tCIDCtrls::TWndHandle hwndPrev = kCIDCtrls::hwndInvalid;
    while (hwndFind)
    {
        if (pwndTar->m_hwndThis == hwndFind)
            break;

        // Not this one, so let's get the next one. If no more, then we are done
        hwndPrev = hwndFind;
        hwndFind = ::GetWindow(hwndFind, GW_HWNDNEXT);
        if (!hwndFind)
            break;
    }

    //
    //  If we found it, then we need to do the move. If we are moving it forward,
    //  then we want to get the next window after this one, and insert this one
    //  after that.
    //
    //  If we are moving back, then we insert the previous window after this one.
    //
    if (pwndTar->m_hwndThis == hwndFind)
    {
        if (bForward)
        {
            // Insert this one after the next one
            hwndPrev = hwndFind;
            hwndFind = ::GetWindow(hwndFind, GW_HWNDNEXT);
        }

        if (hwndFind)
        {
            const tCIDLib::TCard4 c4Flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER;
            if (!::SetWindowPos(hwndPrev, hwndFind, 0, 0, 0, 0, c4Flags))
            {
                TKrnlError::SetLastHostError(::GetLastError());
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcWnd_SetZOrder
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TCardinal(m_widThis)
                );
            }
        }
    }
}



//
//  After a window is resized, it can call this to auto-adjust the size and/or
//  position of any children that have had an edge anchor indication set on them.
//
//  This is usually done in response to an AreaChanged() notification, just pass
//  along the areas that it gets, and only do it if the size changed and the window
//  is in restored state.
//
tCIDLib::TVoid
TWindow::AutoAdjustChildren(const TArea& areaPrev, const TArea& areaNew)
{
    //
    //  If in a minimized position, don't do anything. Or, if the previous size
    //  is zero, then we are getting called from the initial size change, so we
    //  don't want to do anything.
    //
    if ((ePosState() == tCIDCtrls::EPosStates::Minimized)
    ||  areaPrev.szArea().bAllZero())
    {
        return;
    }

    // If it really hasn't changed size, don't do anything
    if (areaNew.bSameSize(areaPrev))
        return;

    //
    //  We will use the DeferWindowPos scheme for this. We need to know how many
    //  windows are we going to affect, so iterate our children and see how many
    //  have an anchor type set.
    //
    tCIDLib::TCard4 c4Count = 0;
    TWindow* pwndCur = pwndBottomChild();
    while (pwndCur)
    {
        if (pwndCur->eEdgeAnchor() != tCIDCtrls::EEdgeAnchors::None)
            c4Count++;
        pwndCur = pwndNextChild(*pwndCur);
    }

    // If none, then no point in going further
    if (!c4Count)
        return;

    // Allocate the deferred position structure
    HDWP hwdpAdj = ::BeginDeferWindowPos(c4Count);

    //
    //  Our calculations need to be relative to the origin of the parent, whereas
    //  the areas we get are the actual prev/new areas of the parent, This will cause
    //  issues when we do calculations relative to the right/bottom. So we create
    //  zero origined versions of both for our calculations below.
    //
    const TArea areaZPrev(TPoint::pntOrigin, areaPrev.szArea());
    const TArea areaZNew(TPoint::pntOrigin, areaNew.szArea());

    //
    //  Now iterate again and for each one that is marked, figure out where it
    //  would be in the new scheme.
    //
    tCIDLib::TInt4 i4XOfs, i4YOfs;
    TArea areaCur;

    pwndCur = pwndBottomChild();
    while (pwndCur)
    {
        if (pwndCur->eEdgeAnchor() != tCIDCtrls::EEdgeAnchors::None)
        {
            areaCur = pwndCur->areaWnd();

            tCIDLib::TCard4 c4Flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

            switch(pwndCur->eEdgeAnchor())
            {
                case tCIDCtrls::EEdgeAnchors::MoveBottom :
                    //
                    //  Figure out where the top was relative to the previous
                    //  bottom and adjust to the new.
                    //
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Top();
                    areaCur.i4Y(areaZNew.i4Bottom() - i4YOfs);
                    c4Flags |= SWP_NOSIZE;
                    break;

                case tCIDCtrls::EEdgeAnchors::MoveRight :
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Left();
                    areaCur.i4X(areaZNew.i4Right() - i4XOfs);
                    c4Flags |= SWP_NOSIZE;
                    break;

                case tCIDCtrls::EEdgeAnchors::MoveBottomRight :
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Left();
                    areaCur.i4X(areaZNew.i4Right() - i4XOfs);
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Top();
                    areaCur.i4Y(areaZNew.i4Bottom() - i4YOfs);
                    c4Flags |= SWP_NOSIZE;
                    break;

                case tCIDCtrls::EEdgeAnchors::SizeBottom :
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Bottom();
                    areaCur.i4Bottom(areaZNew.i4Bottom() - i4YOfs);
                    c4Flags |= SWP_NOMOVE;
                    break;

                case tCIDCtrls::EEdgeAnchors::SizeRight :
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Right();
                    areaCur.i4Right(areaZNew.i4Right() - i4XOfs);
                    c4Flags |= SWP_NOMOVE;
                    break;

                case tCIDCtrls::EEdgeAnchors::SizeBottomRight :
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Right();
                    areaCur.i4Right(areaZNew.i4Right() - i4XOfs);
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Bottom();
                    areaCur.i4Bottom(areaZNew.i4Bottom() - i4YOfs);
                    c4Flags |= SWP_NOMOVE;
                    break;

                case tCIDCtrls::EEdgeAnchors::MoveBottomSizeRight :
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Top();
                    areaCur.i4Y(areaZNew.i4Bottom() - i4YOfs);
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Right();
                    areaCur.i4Right(areaZNew.i4Right() - i4XOfs);
                    break;

                case tCIDCtrls::EEdgeAnchors::SizeBottomMoveRight :
                    i4YOfs = areaZPrev.i4Bottom() - areaCur.i4Bottom();
                    areaCur.i4Bottom(areaZNew.i4Bottom() - i4YOfs);
                    i4XOfs = areaZPrev.i4Right() - areaCur.i4Left();
                    areaCur.i4X(areaZNew.i4Right() - i4XOfs);
                    break;

                default :
                    break;
            };

            // Add this one to the deferred list
            hwdpAdj = ::DeferWindowPos
            (
                hwdpAdj
                , pwndCur->hwndSafe()
                , 0
                , areaCur.i4X()
                , areaCur.i4Y()
                , areaCur.c4Width()
                , areaCur.c4Height()
                , c4Flags
            );

            if (!hwdpAdj)
            {
                TKrnlError::SetLastHostError(::GetLastError());
                facCIDCtrls().LogKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcWnd_MultiWndRepos
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
                break;
            }
        }
        pwndCur = pwndNextChild(*pwndCur);
    }

    //
    //  If we didn't get to the end, then something failed, so don't do anything.
    //  Else, let's invoke the operation.
    //
    if (hwdpAdj)
        ::EndDeferWindowPos(hwdpAdj);
}


// Return whether our bgn color has been explicitly overrriden
tCIDLib::TBoolean TWindow::bBgnColorOver() const
{
    return m_bBgnClrOver;
}


// Returns true if the focus window is somewhere under us, or is us
tCIDLib::TBoolean TWindow::bContainsFocus() const
{
    // If we are not created, then clearly not
    if (m_hwndThis == kCIDCtrls::hwndInvalid)
        return kCIDLib::False;

    // Get the focus window
    tCIDCtrls::TWndHandle hwndFocus = ::GetFocus();

    // If it's us, then yes
    if (hwndFocus == m_hwndThis)
        return kCIDLib::True;

    // Else see if it's a child of ours
    if (::IsChild(m_hwndThis, hwndFocus))
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Wait for a drag to start. If it doesn't, i.e. the user releases without moving,
//  it returns false. Else it returns true and the caller can start tracking.
//
//  We assume the passed point is in our own coordinates, so we convert it to the
//  screen coordinates required by the call.
//
tCIDLib::TBoolean TWindow::bDragDetect(const TPoint& pntFrom)
{
    POINT ptTest;
    ptTest.x = pntFrom.i4X();
    ptTest.y = pntFrom.i4Y();
    ::ClientToScreen(m_hwndThis, &ptTest);

    if (::DragDetect(m_hwndThis, ptTest))
        return kCIDLib::True;
    return kCIDLib::False;
}



// Return whether a particular extended style is enabled on us
tCIDLib::TBoolean TWindow::bExStyleOn(const tCIDCtrls::EExWndStyles eToCheck) const
{
    return (::GetWindowLong(hwndSafe(), GWL_EXSTYLE) & tCIDLib::c4EnumOrd(eToCheck)) != 0;
}



// Sees if this window has a child window with the passed window id
tCIDLib::TBoolean
TWindow::bHasChildWithId(const tCIDCtrls::TWndId widToCheck) const
{
    CheckWindowHandle();

    if (!::GetDlgItem(m_hwndThis, widToCheck))
        return kCIDLib::False;
    return kCIDLib::True;
}


// Indicate if we are the focus window or not
tCIDLib::TBoolean TWindow::bHasFocus() const
{
    if ((m_hwndThis != kCIDCtrls::hwndInvalid) && (::GetFocus() == m_hwndThis))
        return kCIDLib::True;
    return kCIDLib::False;
}


// Indicates whether this window has the mouse captured
tCIDLib::TBoolean TWindow::bHasMouseCapture() const
{
    return (::GetCapture() == m_hwndThis);
}


// Returns whether we are in drag mode or not
tCIDLib::TBoolean TWindow::bInDragMode() const
{
    return m_bDragMode;
}


// Returns whether we are in tracking mode or not
tCIDLib::TBoolean TWindow::bInTrackingMode() const
{
    return m_bAreaTracking;
}


// Checks if the window is a child (direct or not) of us
tCIDLib::TBoolean TWindow::bIsChild(const TWindow& wndTest) const
{
    if (::IsChild(m_hwndThis, wndTest.m_hwndThis))
        return kCIDLib::True;
    return kCIDLib::False;
}


// Checks if the window is a direct child of ours
tCIDLib::TBoolean TWindow::bIsDirectChild(const TWindow& wndTest) const
{
    if (wndTest.bIsTopLevel())
        return kCIDLib::False;
    return (wndTest.wndParent().m_hwndThis == m_hwndThis);
}


// Returns whether this window is enabled or not
tCIDLib::TBoolean TWindow::bIsEnabled() const
{
    CheckWindowHandle();
    return ::IsWindowEnabled(m_hwndThis) != 0;
}


//
//  Returns true if the passed window is this window. Not this window object, but this
//  system window. So if we are not a valid window, we return false.
//
tCIDLib::TBoolean TWindow::bIsThisWindow(const TWindow& wndTest) const
{
    return (bIsValid() && (m_hwndThis == wndTest.m_hwndThis));
}


// Returns true if this window is a top level window (not a child)
tCIDLib::TBoolean TWindow::bIsTopLevel() const
{
    // If the CHILD style is not on, we are top level
    return (::GetWindowLong(m_hwndThis, GWL_STYLE) & WS_CHILD) == 0;
}


// Tells the caller if we are in a modal loop or not
tCIDLib::TBoolean TWindow::bIsInModalLoop() const
{
    return (m_c4EnteredLoops != 0);
}


// Returns true if this window is valid, i.e. has been created at the OS level
tCIDLib::TBoolean TWindow::bIsValid() const
{
    if (::IsWindow(m_hwndThis))
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Returns true if this window and all the parent windows have the visible style.
//  Normaly we check the window handle for validity, but they can optinoally tell
//  us not to, treating not created as not visible. This is often convenient.
//
tCIDLib::TBoolean TWindow::bIsVisible(const tCIDLib::TBoolean bCheckValid) const
{
    if (bCheckValid)
        CheckWindowHandle();

    if (::IsWindowVisible(m_hwndThis))
        return kCIDLib::True;
    return kCIDLib::False;
}


// Get or set the propagate keys flag
tCIDLib::TBoolean TWindow::bPropagateKeys() const
{
    return m_bPropagateKeys;
}

tCIDLib::TBoolean TWindow::bPropagateKeys(const tCIDLib::TBoolean bToSet)
{
    m_bPropagateKeys = bToSet;
    return m_bPropagateKeys;
}


//
//  Send a sync or async message to this window. Note that this guy allows
//  a pointer to be sent. Don't use this across processes since the pointer
//  will not be valid, and be very careful that the data passed gets cleaned
//  up eventually. It's possible that the message will never arrive (the
//  window get's closed before it gets there perhaps.) So be very careful
//  with this method.
//
tCIDLib::TBoolean
TWindow::bSendMsg(  const   tCIDCtrls::TWndMsg      wmsgToSend
                    , const tCIDLib::TVoid* const   pMsgData
                    , const tCIDLib::ESyncTypes     eSyncType)
{
    if (eSyncType == tCIDLib::ESyncTypes::Synchronous)
    {
        if (!::SendMessageW(m_hwndThis, wmsgToSend, tCIDLib::TCard4(pMsgData), 0))
            return kCIDLib::False;
    }
     else
    {
        if (!::PostMessageW(m_hwndThis, wmsgToSend, tCIDLib::TCard4(pMsgData), 0))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Returns whether this method has had painting suppressed. This allows each
//  window to do optimized operations during suppressed painting.
//
tCIDLib::TBoolean TWindow::bPaintSuppressed() const
{
    return (m_c4SuppressPaint > 0);
}


// Return whether a particular style is enabled on us
tCIDLib::TBoolean TWindow::bStyleOn(const tCIDCtrls::EWndStyles eToCheck) const
{
    return (::GetWindowLong(hwndSafe(), GWL_STYLE) & tCIDLib::TCard4(eToCheck)) != 0;
}


//
//  Return true if the indicated timer has been started. It may be paused, but it
//  has been started.
//
tCIDLib::TBoolean TWindow::bTimerIsActive(const tCIDCtrls::TTimerId tmidCheck)
{
    // If the check doesn't throw, then it's started
    CheckTimerId(tmidCheck, kCIDLib::True);
    return kCIDLib::True;
}


//
//  Set or clear the 'suspend all timers' flag
tCIDLib::TBoolean TWindow::bTimerSuspend() const
{
    // If the counter is non-zero, then they are suspended
    return (m_c4TimerSuspend != 0);
}

tCIDLib::TBoolean TWindow::bTimerSuspend(const tCIDLib::TBoolean bToSet)
{
    if (bToSet)
    {
        m_c4TimerSuspend++;
    }
     else
    {
        // Don't allow an underflow
        if (!m_c4TimerSuspend)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTimer_SuspUnderflow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
        m_c4TimerSuspend--;
    }
    return (m_c4TimerSuspend != 0);
}



//
//  Does the standard tracking of an area, allowing any side or corner to
//  be moved around to change the size and/or position of the area. The
//  tracking can be confined to a specific area, and inversion of the area
//  can be allowed or prevented.
//
//  Drag detection is done, so that if they never start moving the mouse,
//  nothing is done.
//
//  To drastically simplify this common operation, we do it modally, within
//  our own loop, and grabbing messages right off the message queue.
//
tCIDLib::TBoolean
TWindow::bTrackArea(const   TPoint&             pntStart
                    ,       TArea&              areaTrack
                    , const tCIDLib::TCard4     c4BorderWidth
                    , const TArea&              areaConfine
                    , const tCIDLib::TBoolean   bAllowInvert)
{
    //
    //  See what area the mouse is over. We use the generic hit testing
    //  method, which does what we want.
    //
    const tCIDCtrls::EWndAreas eArea = facCIDCtrls().eGenericHitCalc
    (
        pntStart, areaTrack, c4BorderWidth
    );

    // If we get a 'nowwhere' back, that's bad
    if (eArea == tCIDCtrls::EWndAreas::Nowhere)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_BadTrackPnt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , pntStart
            , areaTrack
        );
    }

    //
    //  See if the button has already been released. This can happen sometimes
    //  and leaves us tracking but with the button up.
    //
    if (!facCIDCtrls().bMouseButtonDown(tCIDCtrls::EMouseButts::Left))
        return kCIDLib::False;

    // Do drag detection to see if we need to do something
    if (!bDragDetect(pntStart))
        return kCIDLib::False;

    //
    //  We need a temp copy of the tracking rectangle that we can move
    //  around.
    //
    TArea areaCur(areaTrack);

    // Use a flag janitor to set the tracking flag true while in here
    TBoolJanitor janTrack(&m_bAreaTracking, kCIDLib::True);

    //
    //  Precalculate the offset of the start point to the area origin. This
    //  lets us keep it the same as we drag. This only matters if we are
    //  doing a move op, but its not worth checking.
    //
    const TPoint pntOfs(pntStart - areaTrack.pntOrg());

    //
    //  Get a device contect we'll use during this, and do the initial
    //  draw of the XOR'd line.
    //
    TGraphWndDev gdevTrack(*this);

    TCosmeticPen gpenTrack(facCIDGraphDev().rgbPaleGrey, tCIDGraphDev::ELineStyles::Dot);
    TPenJanitor  janPen(&gdevTrack, &gpenTrack);
    gdevTrack.eMixMode(tCIDGraphDev::EMixModes::XorPen);
    gdevTrack.Stroke(areaCur);

    tCIDLib::TBoolean bRet = kCIDLib::False;
    MSG msgCur;
    try
    {
        // Bump the modal loop entered counter for this window
        TCardJanitor janEntered(&m_c4EnteredLoops, m_c4EnteredLoops + 1);

        //
        //  Set the cursor confinement to the confine area. There are a
        //  couple of options here. If they don't want to allow inversion,
        //  then we have to confine the mouse possibly even more than the
        //  caller said to. We limit it to the confine area in the direction
        //  of travel, and keep it to no more than 4 pixels from the other
        //  size/corner (i.e. they can't get the moving side/corner closer
        //  than 4 pixels to the other side/corner.
        //
        //  If is a move operation then the inversion doesn't matter, but
        //  we have to offset the confinement area by the offset from the
        //  edgets of the track area.
        //
        //  Otherwise, we just take the confine area as is.
        //
        TArea areaTmp(areaConfine);
        if (eArea == tCIDCtrls::EWndAreas::Client)
        {
            areaTmp.AdjustOrg(pntOfs);
            TPoint pntDef(areaTrack.pntLR() - pntStart);
            pntDef += pntOfs;
            pntDef.Negate();
            areaTmp.AdjustSize(pntDef);
        }
         else if (!bAllowInvert)
        {
            areaTmp = areaTrack;
            switch(eArea)
            {
                case tCIDCtrls::EWndAreas::Left :
                    areaTmp.i4Left(areaConfine.i4Left());
                    areaTmp.i4Right(areaTrack.i4Right() - 8);
                    break;

                case tCIDCtrls::EWndAreas::Right :
                    areaTmp.i4Left(areaTrack.i4Left() + 8);
                    areaTmp.i4Right(areaConfine.i4Right());
                    break;

                case tCIDCtrls::EWndAreas::Bottom :
                    areaTmp.i4Top(areaTrack.i4Top() + 8);
                    areaTmp.i4Bottom(areaConfine.i4Bottom());
                    break;

                case tCIDCtrls::EWndAreas::Top :
                    areaTmp.i4Top(areaConfine.i4Top());
                    areaTmp.i4Bottom(areaTrack.i4Bottom() - 8);
                    break;

                case tCIDCtrls::EWndAreas::TopLeft :
                    areaTmp.i4Left(areaConfine.i4Left());
                    areaTmp.i4Right(areaTrack.i4Right() - 8);
                    areaTmp.i4Top(areaConfine.i4Top());
                    areaTmp.i4Bottom(areaTrack.i4Bottom() - 8);
                    break;

                case tCIDCtrls::EWndAreas::TopRight :
                    areaTmp.i4Left(areaTrack.i4Left() + 8);
                    areaTmp.i4Right(areaConfine.i4Right());
                    areaTmp.i4Top(areaConfine.i4Top());
                    areaTmp.i4Bottom(areaTrack.i4Bottom() - 8);
                    break;

                case tCIDCtrls::EWndAreas::BottomLeft :
                    areaTmp.i4Left(areaConfine.i4Left());
                    areaTmp.i4Right(areaTrack.i4Right() - 8);
                    areaTmp.i4Top(areaTrack.i4Top() + 8);
                    areaTmp.i4Bottom(areaConfine.i4Bottom());
                    break;

                case tCIDCtrls::EWndAreas::BottomRight :
                    areaTmp.i4Left(areaTrack.i4Left() + 8);
                    areaTmp.i4Right(areaConfine.i4Right());
                    areaTmp.i4Top(areaTrack.i4Top() + 8);
                    areaTmp.i4Bottom(areaConfine.i4Bottom());
                    break;
            }
        }
        TArea areaScreenCoord;
        ToScreenCoordinates(areaTmp, areaScreenCoord);
        facCIDCtrls().ConfinePointer(areaScreenCoord);

        // Capture the mouse
        CaptureMouse();

        // And enter the loop
        while (::GetMessage(&msgCur, 0, 0, 0))
        {
            //
            //  Watch for a loss of focus or loss of capture or an escape
            //  key.  If we get either of them, we bail out, don't update the
            //  caller's area, and return false.
            //
            //  And watch for a left button up, that indicates the end of
            //  the tracking operation. We update the caller's area and return
            //  true.
            //
            //  And we watch for the mouse move message, in which case we update
            //  the pointer to show the correct shape for the tracking operation
            //  and update our rubber band.
            //
            if ((msgCur.message >= WM_KEYFIRST)
            &&  (msgCur.message <= WM_KEYLAST))
            {
                if ((msgCur.message == WM_KEYDOWN) && (msgCur.wParam == VK_ESCAPE))
                    break;

                // Not an escape, so eat it
                continue;
            }
             else if (msgCur.message == WM_LBUTTONUP)
            {
                // Get the area back to the caller's parm
                areaTrack = areaCur;
                bRet = kCIDLib::True;
                break;
            }
             else if (msgCur.message == WM_MOUSEMOVE)
            {
                // Get the of the mouse
                const TPoint pntPos
                (
                    TRawBits::i2Low16From32(msgCur.lParam)
                    , TRawBits::i2High16From32(msgCur.lParam)
                );

                //
                //  If its a move operation, then move the rubberbanded
                //  area to the new location. Else, its moving one of the
                //  corners or sides, so adjust the area and redraw it.
                //
                gdevTrack.Stroke(areaCur);
                if (eArea == tCIDCtrls::EWndAreas::Client)
                {
                    areaCur.SetOrg(pntPos - pntOfs);
                }
                 else
                {
                    switch(eArea)
                    {
                        case tCIDCtrls::EWndAreas::Left :
                            areaCur.i4X(pntPos.i4X(), kCIDLib::True);
                            break;

                        case tCIDCtrls::EWndAreas::Right :
                            areaCur.i4Right(pntPos.i4X());
                            break;

                        case tCIDCtrls::EWndAreas::Bottom :
                            areaCur.i4Bottom(pntPos.i4Y());
                            break;

                        case tCIDCtrls::EWndAreas::Top :
                            areaCur.i4Y(pntPos.i4Y(), kCIDLib::True);
                            break;

                        case tCIDCtrls::EWndAreas::TopLeft :
                            areaCur.i4X(pntPos.i4X(), kCIDLib::True);
                            areaCur.i4Y(pntPos.i4Y(), kCIDLib::True);
                            break;

                        case tCIDCtrls::EWndAreas::TopRight :
                            areaCur.i4Right(pntPos.i4X());
                            areaCur.i4Y(pntPos.i4Y(), kCIDLib::True);
                            break;

                        case tCIDCtrls::EWndAreas::BottomLeft :
                            areaCur.i4X(pntPos.i4X(), kCIDLib::True);
                            areaCur.i4Bottom(pntPos.i4Y());
                            break;

                        case tCIDCtrls::EWndAreas::BottomRight :
                            areaCur.i4Right(pntPos.i4X());
                            areaCur.i4Bottom(pntPos.i4Y());
                            break;
                    }
                }
                gdevTrack.Stroke(areaCur);

                // Update the pointer based on the area we are tracking
                tCIDLib::TCh* pszId;
                switch(eArea)
                {
                    case tCIDCtrls::EWndAreas::Left :
                    case tCIDCtrls::EWndAreas::Right :
                        pszId = IDC_SIZEWE;
                        break;

                    case tCIDCtrls::EWndAreas::Top :
                    case tCIDCtrls::EWndAreas::Bottom :
                        pszId = IDC_SIZENS;
                        break;

                    case tCIDCtrls::EWndAreas::TopLeft :
                    case tCIDCtrls::EWndAreas::BottomRight :
                        pszId = IDC_SIZENWSE;
                        break;

                    case tCIDCtrls::EWndAreas::TopRight :
                    case tCIDCtrls::EWndAreas::BottomLeft :
                        pszId = IDC_SIZENESW;
                        break;

                    default :
                        pszId = IDC_SIZEALL;
                        break;
                }
                ::SetCursor(::LoadCursorW(NULL, pszId));
            }
             else
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }
        }

        // Get rid of the tracking area
        gdevTrack.Stroke(areaCur);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Get rid of the tracking area
        gdevTrack.Stroke(areaCur);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean
TWindow::bTrackRegion(  const   TPoint&             pntStart
                        ,       TGUIRegion&         grgnTrack
                        , const TArea&              areaConfine)
{
    // Do drag detection to see if we need to do something
    if (!bDragDetect(pntStart))
        return kCIDLib::False;

    //
    //  We only do the 'drag' type tracking for regions, meaning that we
    //  move the region, we don't allow any sizing by moving the edges or
    //  corners, since it is used generally to move multiple objects.
    //
    //  But we need an overall area in order to keep up with movement,
    //  so get the bounding area of the region.
    //
    TArea areaTrack = grgnTrack.areaBounds();

    //
    //  Calculate the offset of the start point to the area origin. This
    //  is used below to calculate the confinement, since it must reflect
    //  the movement of the mouse, not of the area.
    //
    const TPoint pntOfs(pntStart - areaTrack.pntOrg());

    //
    //  Get a device contect we'll use during this, and do the initial
    //  draw of the XOR'd line.
    //
    TGraphWndDev gdevTrack(*this);
    gdevTrack.eMixMode(tCIDGraphDev::EMixModes::XorPen);
    gdevTrack.Stroke(grgnTrack, facCIDGraphDev().rgbWhite);

    //
    //  To drastically simplify this common operation, we do it modally,
    //  within our own loop, and grabbing messages right off the message
    //  queue.
    //
    //  We support the standard area tracking ops, which is moving one of
    //  the corners, moving one of the sides, or moving the whole area.
    //
    tCIDLib::TBoolean bRet = kCIDLib::False;
    MSG msgCur;
    try
    {
        // Bump the modal loop entered counter for this window
        TCardJanitor janEntered(&m_c4EnteredLoops, m_c4EnteredLoops + 1);

        // Set the cursor confinement to the confine area
        TArea areaTmp(areaConfine);
        areaTmp.AdjustOrg(pntOfs);
        TPoint pntDef(areaTrack.pntLR() - pntStart);
        pntDef += pntOfs;
        pntDef.Negate();
        areaTmp.AdjustSize(pntDef);

        TArea areaScreenCoord;
        ToScreenCoordinates(areaTmp, areaScreenCoord);
        facCIDCtrls().ConfinePointer(areaScreenCoord);

        // Capture the mouse
        CaptureMouse();

        //
        //  Get the current mouse position. We'll use this to keep up with
        //  previous position, so that we can calculate how much we've moved
        //  since the last time.
        //
        TPoint pntLastMouse = facCIDCtrls().pntPointerPos(*this);

        // And enter the loop
        while (::GetMessage(&msgCur, 0, 0, 0))
        {
            //
            //  Watch for a loss of focus or loss of capture or an escape
            //  key.  If we get either of them, we bail out, don't update the
            //  caller's area, and return false.
            //
            //  And watch for a left button up, that indicates the end of
            //  the tracking operation. We update the caller's area and return
            //  true.
            //
            //  And we watch for the mouse move message, in which case we update
            //  the pointer to show the correct shape for the tracking operation
            //  and update our rubber band.
            //
            if ((msgCur.message >= WM_KEYFIRST)
            &&  (msgCur.message <= WM_KEYLAST))
            {
                if ((msgCur.message == WM_KEYDOWN) && (msgCur.wParam == VK_ESCAPE))
                    break;

                // Not an escape, so eat it
                continue;
            }
             else if (msgCur.message == WM_LBUTTONUP)
            {
                // We are done
                bRet = kCIDLib::True;
                break;
            }
             else if (msgCur.message == WM_MOUSEMOVE)
            {
                // Get the of the mouse
                const TPoint pntPos
                (
                    TRawBits::i2Low16From32(msgCur.lParam)
                    , TRawBits::i2High16From32(msgCur.lParam)
                );

                //
                //  Gid rid of the previous region, adjust it's origin and
                //  redraw it.
                //
                TPoint pntAdjust(pntPos - pntLastMouse);
                gdevTrack.Stroke(grgnTrack, facCIDGraphDev().rgbWhite);
                grgnTrack.Offset(pntAdjust);
                gdevTrack.Stroke(grgnTrack, facCIDGraphDev().rgbWhite);

                // Make this the new last mouse position and adjust the area
                pntLastMouse = pntPos;
                areaTrack.AdjustOrg(pntAdjust);

                // Update the cursor
                ::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));

                // Call the tracking callback
                TrackingAreaChanged(areaTrack);
            }
             else
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }
        }

        // Get rid of the tracking area
        gdevTrack.Stroke(grgnTrack, facCIDGraphDev().rgbWhite);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Get rid of the tracking area
        gdevTrack.Stroke(grgnTrack, facCIDGraphDev().rgbWhite);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
        throw;
    }
    return bRet;
}


//
//  Get/set the flag that controls whether this window will receive child focus change
//  notifications (where child means any recursively contained window getting or losing
//  the focus.)
//
tCIDLib::TBoolean TWindow::bWantsChildFocusNot() const
{
    return m_bWantsChildFocusNot;
}

tCIDLib::TBoolean TWindow::bWantsChildFocusNot(const tCIDLib::TBoolean bToSet)
{
    m_bWantsChildFocusNot = bToSet;
    return m_bWantsChildFocusNot;
}


//
//  Adjust our scroll position up or down, if possible. We return the new position.
//  If they want to know if it changed, they have to get the original first.
//
tCIDLib::TCard4
TWindow::c4AdjustScrollPos( const   tCIDLib::TBoolean   bHorz
                            , const tCIDLib::TBoolean   bInc)
{
    tCIDLib::TCard4 c4NewPos = c4CurScrollPos(bHorz);
    if (bInc)
    {
        const tCIDLib::TCard4 c4MaxPos = c4MaxScrollPos(bHorz);
        if (c4NewPos < c4MaxPos)
        {
            c4NewPos++;
            SetScrollPos(bHorz, c4NewPos);
        }
    }
     else if (!bInc && c4NewPos)
    {
        c4NewPos--;
        SetScrollPos(bHorz, c4NewPos);
    }
    return c4NewPos;
}


//
//  Get the current scroll position. We have to use the more generic query, since
//  the dedicated current position call retardly returns zero for a failure, when
//  zero can be a valid position.
//
tCIDLib::TCard4 TWindow::c4CurScrollPos(const tCIDLib::TBoolean bHorz) const
{
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);
    Info.fMask = SIF_POS;

    if (!GetScrollInfo(hwndSafe(), bHorz ? SB_HORZ : SB_VERT, &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_GetScrollPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    return tCIDLib::TCard4(Info.nPos);
}


// Get the maximum legal scroll position
tCIDLib::TCard4 TWindow::c4MaxScrollPos(const tCIDLib::TBoolean bHorz) const
{
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);
    Info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;

    if (!::GetScrollInfo(hwndSafe(), bHorz ? SB_HORZ : SB_VERT, &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_GetScrollPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return tCIDLib::TCard4(Info.nMax - (Info.nPage - 1));
}



// Get or set the user id
tCIDLib::TCard4 TWindow::c4UserId() const
{
    return m_c4UserId;
}

tCIDLib::TCard4 TWindow::c4UserId(const tCIDLib::TCard4 c4ToSet)
{
    m_c4UserId = c4ToSet;
    return m_c4UserId;
}


// Puts us into mouse capture mode
tCIDLib::TVoid TWindow::CaptureMouse()
{
    ::SetCapture(m_hwndThis);
}


// Clear our text at our level and the Windows level
tCIDLib::TVoid TWindow::ClearText()
{
    m_strWndText.Clear();
    ::SendMessage
    (
        hwndSafe(), WM_SETTEXT, 0, tCIDCtrls::TLParam(L"")
    );
}


//
//  Destroy our window. If it's not already destroyed, this will cause a destroy
//  msg to be sent which will clean up the link between this object and the
//  window and reset us so that we could be used again.
//
tCIDLib::TVoid TWindow::Destroy()
{
    if (m_hwndThis != kCIDCtrls::hwndInvalid)
    {
        //
        //  If we don't own the handle, we need to remove the key from our map, because
        //  WM_DESTROY isn't going to get seen by this window and it wouldn't then get
        //  removed otherwise.
        //
        if (m_bOwnsHandle)
            ::DestroyWindow(m_hwndThis);
        else
            CIDCtrls_Window::colWndItemMap().bRemoveKey(m_hwndThis, kCIDLib::False);
    }
}


//
//  This method will iterate all child windows of this window and destroy them.
//  IT ASSUMES that they are dynamically allocated windows. Any that are not must
//  be destroyed before calling this.
//
//  This is called from the DoCleanup() method (which is invoked when we ourself
//  are destroyed, to get any child windows cleanedup. In that case, the derived
//  class must override Destroyed() and destroy any non-dynamically allocated
//  windows before passing the call to his parent.
//
tCIDLib::TVoid TWindow::DestroyAllChildren()
{
    //
    //  So we iterate all of the children and see which ones have stored window handle
    //  info. Those we destroy.
    //
    tCIDCtrls::TWndHandle hwndFind = ::GetWindow(m_hwndThis, GW_CHILD);
    while (hwndFind)
    {
        // Get the next window before we destroy this one
        tCIDCtrls::TWndHandle hwndNext = ::GetWindow(hwndFind, GW_HWNDNEXT);

        // See if this is a CIDLib window. If so, then clean it up
        TWindow* pwndCur = TWindow::pwndGetWndLinkPtr(hwndFind);
        if (pwndCur)
        {
            if (pwndCur->bIsValid())
            {
                try
                {
                    pwndCur->Destroy();
                }

                catch(TError& errToCatch)
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }

                catch(...)
                {
                }
            }

            // Now delete the window object itself
            if (pwndCur)
                delete pwndCur;
        }

        // And now move forward to the next child that we stored above
        hwndFind = hwndNext;
    }
}


// If the indicated child window exists, we destroy it
tCIDLib::TVoid TWindow::DestroyChild(const tCIDCtrls::TWndId widChild)
{
    TWindow* pwndChild = pwndChildById(widChild, kCIDLib::False);
    if (pwndChild)
    {
        pwndChild->Destroy();
        delete pwndChild;
    }
}


// Get or set the edge anchor type for this window
tCIDCtrls::EEdgeAnchors TWindow::eEdgeAnchor() const
{
    return m_eEdgeAnchor;
}

tCIDCtrls::EEdgeAnchors
TWindow::eEdgeAnchor(const tCIDCtrls::EEdgeAnchors eToSet)
{
    m_eEdgeAnchor = eToSet;
    return m_eEdgeAnchor;
}


// Return the current window styles
tCIDCtrls::EExWndStyles TWindow::eExWndStyles() const
{
    return tCIDCtrls::EExWndStyles(::GetWindowLong(hwndSafe(), GWL_EXSTYLE));
}


//
//  This is a modal loop that is used in some places where it is necessary outside
//  of the places it would normally be done automatically by the system or within
//  a couple of special controls.
//
tCIDLib::TVoid DoDispatch(TWindow& wndTar, MSG msgCur)
{
    if (!::IsDialogMessage(wndTar.hwndThis(), &msgCur))
    {
        // Not eaten by the focus management, so send along
        ::TranslateMessage(&msgCur);
        ::DispatchMessage(&msgCur);
    }
}

tCIDCtrls::EModalRes
TWindow::eDoModalLoop(          tCIDLib::TBoolean&      bBreak
                        , const tCIDCtrls::EModalFlags  eFlags
                        ,       TWindow* const          pwndAltTarget
                        , const tCIDLib::TCard4         c4Timeout)
{
    // Default return if we don't set it
    tCIDCtrls::EModalRes eRes = tCIDCtrls::EModalRes::Unknown;

    // Calc the end time, even if we aren't actually doing one
    tCIDLib::TEncodedTime enctEnd = 0;
    if (c4Timeout != kCIDLib::c4MaxCard)
        enctEnd = TTime::enctNowPlusMSs(c4Timeout);

    // If we have an alt target, then get it's handle out
    tCIDCtrls::TWndHandle hwndAlt = 0;
    if (pwndAltTarget)
        hwndAlt = pwndAltTarget->hwndThis();

    // Get some flags out
    const tCIDLib::TBoolean bBreakOnClick
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::BreakOnExtClick)
    );
    const tCIDLib::TBoolean bBreakOnEsc
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::BreakOnEsc)
    );
    const tCIDLib::TBoolean bNoChildMouse
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::NoChildMouse)
    );
    const tCIDLib::TBoolean bNoClicks
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::NoClicks)
    );
    const tCIDLib::TBoolean bNoKeys
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::NoKeys)
    );
    const tCIDLib::TBoolean bNoInput
    (
        tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::NoInput)
    );

    tCIDLib::TBoolean bDone = kCIDLib::False;
    try
    {
        // Bump the modal loop entered counter for this window
        TCardJanitor janEntered(&m_c4EnteredLoops, m_c4EnteredLoops + 1);

        // If they asked to stop window timers for this window, then do so
        if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::StopTimers))
            bTimerSuspend(kCIDLib::True);

        //
        //  We always start our own timer, so that we are sure to keep
        //  getting at least a periodic message. This way, we can watch
        //  for the break flag to be set. And, if we are doing a timeout,
        //  we use it to watch for the timeout period.
        //
        TLocalTimerJanitor janModalTimer(this, 50);

        // Get our process id and keep it around for use below
        const tCIDLib::TProcessId pidUs = ::GetCurrentProcessId();

        //
        //  We use two different flags for the loop. One is for our internal
        //  use to let us have a double loop that we need. And the other is
        //  the incoming break flag which the outside world can set to make us
        //  fall out.
        //
        MSG msgCur;
        while (!bDone && !bBreak)
        {
            ::WaitMessage();

            while (::PeekMessage(&msgCur, 0, 0, 0, PM_REMOVE) != 0)
            {
                // If the break flag, break out
                if (bBreak)
                    break;

                // Get the message out for convenience below
                tCIDCtrls::TWndMsg wmsgMsg = msgCur.message;

                // Fall out if we get a quit
                if (wmsgMsg == WM_QUIT)
                {
                    eRes = tCIDCtrls::EModalRes::Quit;
                    bDone = kCIDLib::True;
                    break;
                }

                // If we get an escape, then break out if asked to
                if (msgCur.wParam == VK_ESCAPE)
                {
                    if ((wmsgMsg == WM_KEYDOWN) && bBreakOnEsc)
                    {
                        eRes = tCIDCtrls::EModalRes::Escape;
                        bDone = kCIDLib::True;
                    }
                    continue;
                }

                // Get our root owner. We need that multiple times below
                const tCIDLib::TBoolean bIsOurChild(::IsChild(m_hwndThis, msgCur.hwnd) != 0);
                HWND hwndOurRootOwner = facCIDCtrls().hwndRootOwner(m_hwndThis);

                // If it's a timer for us, then deal with it specially
                if ((wmsgMsg == WM_TIMER) && (msgCur.hwnd == m_hwndThis))
                {
                    //
                    //  If it's our internal timer and we were asked to do a timeout,
                    //  then check it.
                    //
                    //  Else dispatch it. If timers were stopped on entry above, and it
                    //  is a CIDCtrls standard timer, it will not end up going anywhere,
                    //  but we don't want to try duplicate checking for CIDCtrls window
                    //  timers here.
                    //
                    if (msgCur.wParam == janModalTimer.tmidThis())
                    {
                        // If a timeout and we've hit the end time, then break out
                        if (enctEnd && (TTime::enctNow() >= enctEnd))
                        {
                            eRes = tCIDCtrls::EModalRes::Timeout;
                            bDone = kCIDLib::True;
                            break;
                        }
                    }
                     else
                    {
                        DoDispatch(*this, msgCur);
                    }
                    continue;
                }

                // See if it's for the alt window or one of it's children
                const tCIDLib::TBoolean bToAltWnd =
                (
                    hwndAlt
                    && ((msgCur.hwnd == hwndAlt)
                    ||  ::IsChild(hwndAlt, msgCur.hwnd))
                );


                //
                //  Decide if it's something that we should allow input
                //  to propogate to. If the no input flag is set, then we
                //  never dispatch any.
                //
                //  We don't check whether it's us or not. If it's us, we
                //  always pass it unless one of the incoming flags prevents
                //  it.
                //
                tCIDLib::TBoolean bPassInput = kCIDLib::False;
                if (!bNoInput)
                {
                    //
                    //  If it is a child of us. or its root parent or owner is a
                    //  child of us, then it can get messages. Otherwise, its root
                    //  owner must have no parent and must not be owned, which means
                    //  that it's part of a secondary unowned frame.
                    //
                    if (bIsOurChild)
                    {
                        bPassInput = kCIDLib::True;
                    }
                     else
                    {
                        HWND hwndOtherRoot = ::GetAncestor(msgCur.hwnd, GA_ROOT);

                        if (!hwndOtherRoot
                        ||  (hwndOtherRoot && ::IsChild(m_hwndThis, hwndOtherRoot)))
                        {
                            //
                            //  It's got to be some unowned top level window,
                            //  or it's owned by one of our children, so it
                            //  should get input.
                            //
                            bPassInput = kCIDLib::True;
                        }
                         else if ((hwndOtherRoot != hwndOurRootOwner)
                              &&  hwndOtherRoot
                              &&  !::GetParent(hwndOtherRoot))
                        {
                            //
                            //  It's root owner is not our root owner and has
                            //  no parent or owner itself, so it's a child of
                            //  some top level secondary frame can not related
                            //  to us.
                            //
                            bPassInput = kCIDLib::True;
                        }
                    }
                }

                //
                //  We have to deal with user input events. These have to be
                //  selectively suppressed. If it's for us, then we know
                //  unconditionally we'll dispatch it.
                //
                switch(wmsgMsg)
                {
                    case WM_KEYDOWN :
                    {
                        // If the no-keys flag is on, then don't dispatch
                        if ((msgCur.hwnd == m_hwndThis) && !bNoKeys)
                        {
                            DoDispatch(*this, msgCur);
                        }
                         else
                        {
                            // See if it's someone we should let see mouse events
                            if (bToAltWnd || bPassInput)
                                DoDispatch(*this, msgCur);
                        }
                        break;
                    }

                    case WM_CHAR :
                    case WM_SYSKEYDOWN :
                    case WM_SYSCHAR :
                    case WM_SYSDEADCHAR :
                    case WM_UNICHAR :
                    {
                        //
                        //  If the no-keys flag is on, then don't dispatch
                        //  to us.
                        //
                        if ((msgCur.hwnd == m_hwndThis) && !bNoKeys)
                        {
                            DoDispatch(*this, msgCur);
                        }
                         else
                        {
                            // See if it's someone we should let see key events
                            if (bToAltWnd || bPassInput)
                            {
                                DoDispatch(*this, msgCur);
                            }
                        }
                        break;
                    }

                    case WM_NCLBUTTONDOWN :
                    case WM_NCLBUTTONDBLCLK :
                    case WM_NCRBUTTONDOWN :
                    case WM_NCRBUTTONDBLCLK :
                    case WM_NCMBUTTONDOWN :
                    case WM_NCMBUTTONDBLCLK :
                    case WM_LBUTTONDOWN :
                    case WM_LBUTTONDBLCLK :
                    case WM_RBUTTONDOWN :
                    case WM_RBUTTONDBLCLK :
                    case WM_MBUTTONDOWN :
                    case WM_MBUTTONDBLCLK :
                    case WM_XBUTTONDOWN :
                    case WM_XBUTTONDBLCLK :
                    {
                        if (msgCur.hwnd == m_hwndThis)
                        {
                            if (!bNoClicks)
                            {
                                // It's for us and clicks are not disabled
                                DoDispatch(*this, msgCur);
                            }
                        }
                         else
                        {
                            // See if it's someone we should let see mouse events
                            if (bToAltWnd || bPassInput)
                            {
                                DoDispatch(*this, msgCur);
                            }
                             else
                            {
                                facCIDCtrls().FlashWindow(m_hwndThis);
                                ::SetForegroundWindow(m_hwndThis);
                            }

                            //
                            //  If the clicked window isn't a child of this
                            //  window, and they asked to break on click, then
                            //  do that.
                            //
                            if (!bIsOurChild && bBreakOnClick)
                            {
                                eRes = tCIDCtrls::EModalRes::ExtClick;
                                bDone = kCIDLib::True;
                            }
                        }
                        break;
                    }

                    case WM_NCLBUTTONUP :
                    case WM_NCRBUTTONUP :
                    case WM_NCMBUTTONUP :
                    case WM_LBUTTONUP :
                    case WM_RBUTTONUP :
                    case WM_MBUTTONUP :
                    case WM_XBUTTONUP :
                    {
                        if (msgCur.hwnd == m_hwndThis)
                        {
                            // If it's for us and clicks are not disabled
                            if (!bNoClicks)
                                DoDispatch(*this, msgCur);
                        }
                         else
                        {
                            // See if it's someone we should let see mouse events
                            if (bToAltWnd || bPassInput)
                            {
                                DoDispatch(*this, msgCur);
                            }
                             else
                            {
                                facCIDCtrls().FlashWindow(m_hwndThis);
                                ::SetForegroundWindow(m_hwndThis);
                            }
                        }
                        break;
                    }

                    case WM_MOUSEMOVE :
                    {
                        if (!bIsOurChild || !bNoChildMouse)
                            DoDispatch(*this, msgCur);
                        break;
                    }

                    default :
                    {
                        // Not a special case, so just process
                        DoDispatch(*this, msgCur);
                        break;
                    }
                };
            }
        }

        // If we paused timers, then reenable them
        if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::StopTimers))
            bTimerSuspend(kCIDLib::False);
    }

    catch(TError& errToCatch)
    {
        // If we paused timers, then reenable them
        if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::StopTimers))
            bTimerSuspend(kCIDLib::False);

        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        // If we paused timers, then reenable them
        if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::EModalFlags::StopTimers))
            bTimerSuspend(kCIDLib::False);
    }
    return eRes;
}



// Get or set our position state
tCIDCtrls::EPosStates TWindow::ePosState() const
{
    // Get the window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);

    if (WndPlace.showCmd == SW_SHOWMAXIMIZED)
        return tCIDCtrls::EPosStates::Maximized;
    else if (WndPlace.showCmd == SW_SHOWMINIMIZED)
        return tCIDCtrls::EPosStates::Minimized;
    return tCIDCtrls::EPosStates::Restored;
}

tCIDCtrls::EPosStates TWindow::ePosState(TArea& areaNormal) const
{
    // Get the window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);

    // Figure out the opsition state
    tCIDCtrls::EPosStates eRet;
    if (WndPlace.showCmd == SW_SHOWMAXIMIZED)
        eRet = tCIDCtrls::EPosStates::Maximized;
    else if (WndPlace.showCmd == SW_SHOWMINIMIZED)
        eRet = tCIDCtrls::EPosStates::Minimized;
    else
        eRet = tCIDCtrls::EPosStates::Restored;

    // And store the last stored normal position
    areaNormal.FromRectl
    (
        *reinterpret_cast<tCIDLib::THostRectl*>(&WndPlace.rcNormalPosition)
    );

    return eRet;
}


// Return the current window styles
tCIDCtrls::EWndStyles TWindow::eWndStyles() const
{
    return tCIDCtrls::EWndStyles(::GetWindowLong(hwndSafe(), GWL_STYLE));
}


// Force a redraw of our whole window
tCIDLib::TVoid TWindow::ForceRepaint()
{
    CheckWindowHandle();

    if (!::InvalidateRect(m_hwndThis, 0, 1))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_APIFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"ForceRepaint")
            , TCardinal(m_widThis)
        );
    }
}


//
//  These are just conveniences, for this very common conversion from a
//  window's coordinate system to the screen system.
//
//  NOTE: We are converting to client coordinates, so it results in coordinates
//  in the calling window's client area. Don't call this on a child window and
//  apply the results to that child window. Call it on the parent and apply to
//  the child window.
//
tCIDLib::TVoid
TWindow::FromScreenCoordinates(const TArea& areaSrc, TArea& areaTarget) const
{
    POINT ptTmp;
    ptTmp.x = areaSrc.i4X();
    ptTmp.y = areaSrc.i4Y();
    ::ScreenToClient(m_hwndThis, &ptTmp);
    areaTarget.i4X(ptTmp.x);
    areaTarget.i4Y(ptTmp.y);
    areaTarget.c4Width(areaSrc.c4Width());
    areaTarget.c4Height(areaSrc.c4Height());
}

tCIDLib::TVoid
TWindow::FromScreenCoordinates(const TPoint& pntSrc, TPoint& pntTarget) const
{
    POINT ptTmp;
    ptTmp.x = pntSrc.i4X();
    ptTmp.y = pntSrc.i4Y();
    ::ScreenToClient(m_hwndThis, &ptTmp);
    pntTarget.i4X(ptTmp.x);
    pntTarget.i4Y(ptTmp.y);
}


//
//  Make our window handle available. One just returns the handle as is, and
//  another that will throw if its not valid.
//
tCIDCtrls::TWndHandle TWindow::hwndSafe() const
{
    if (!m_hwndThis || !::IsWindow(m_hwndThis))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotValidWnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return m_hwndThis;
}

tCIDCtrls::TWndHandle TWindow::hwndThis() const
{
    return m_hwndThis;
}


// Move this window to put it after the indicated window
tCIDLib::TVoid TWindow::InsertAfter(const TWindow& wndInsertAfter)
{
    if (!::SetWindowPos(m_hwndThis
                        , wndInsertAfter.m_hwndThis
                        , 0
                        , 0
                        , 0
                        , 0
                        , SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetOrder
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


//
//  Invalidate an area in various ways.
//
tCIDLib::TVoid
TWindow::InvalidateArea(const   TArea&              areaToInvalidate
                        , const tCIDLib::TBoolean   bEraseBgn)
{
    tCIDLib::THostRectl rectlInvalidate;
    areaToInvalidate.ToRectl(rectlInvalidate);
    if (!::InvalidateRect(m_hwndThis, (RECT*)&rectlInvalidate, bEraseBgn))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_Invalidate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , areaToInvalidate
            , TCardinal(m_widThis)
        );
    }
}

tCIDLib::TVoid
TWindow::InvalidateArea(const   tCIDCtrls::TWndId   widChild
                        , const TArea&              areaToInvalidate
                        , const tCIDLib::TBoolean   bEraseBgn)
{
    TWindow* pwndChild = pwndChildById(widChild, kCIDLib::True);
    pwndChild->InvalidateArea(areaToInvalidate, bEraseBgn);
}

// We assume non-inclusive points
tCIDLib::TVoid
TWindow::InvalidateArea(const   TPoint&             pntUL
                        , const TPoint&             pntLR
                        , const tCIDLib::TBoolean   bEraseBgn)
{
    InvalidateArea(TArea(pntUL, pntLR), bEraseBgn);
}

tCIDLib::TVoid
TWindow::InvalidateArea(const   tCIDCtrls::TWndId   widChild
                        , const TPoint&             pntUL
                        , const TPoint&             pntLR
                        , const tCIDLib::TBoolean   bEraseBgn)
{
    InvalidateArea(widChild, TArea(pntUL, pntLR), bEraseBgn);
}


//
//  Invalidate the indicated region. Indicate whether the underlying
//  background should be erased before redrawing.
//
tCIDLib::TVoid
TWindow::InvalidateRegion(  const   TGUIRegion&         grgnToInvalidate
                            , const tCIDLib::TBoolean   bEraseBgn)
{
    if (!::InvalidateRgn(m_hwndThis, grgnToInvalidate.hrgnThis(), bEraseBgn))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_InvalidateRgn
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


//
//  When a window wants to send out a notification via our notification system, it
//  calls this. We iterate any registered handlers and send the notification to them.
//
tCIDCtrls::EEvResponses
TWindow::eInvokeHandlers(const  TNotificationId&    nidBeingSent
                        ,       TObject* const      pobjBeingSent)
{
    //
    //  Check for any handlers for this event, which there should
    //  be if we got this message. For each one, invoke the
    //  handler for it.
    //
    tCIDLib::TBoolean   bHandled = kCIDLib::False;
    TNotHandlerBase* pnothCur = m_pnothHead;
    while (pnothCur)
    {
        if (pnothCur->nidRequested() == nidBeingSent)
        {
            // Invoke the handler with the data
            if (pnothCur->eInvoke(pobjBeingSent) == tCIDCtrls::EEvResponses::Handled)
            {
                bHandled = kCIDLib::True;
                break;
            }
        }
        pnothCur = pnothCur->pnothNext();
    }

    if (bHandled)
        return tCIDCtrls::EEvResponses::Handled;

    return tCIDCtrls::EEvResponses::Propagate;
}


//
//  Make the indicated scroll position visible if not already. We can do the minimal
//  move possible, which is appropriate for scrolling, to just keep moving it within
//  the visible range. Or, we can do non-minimal where we try to get it to the center
//  or close as possible, which is appropriate for things like moving to a particular
//  target position.
//
tCIDLib::TVoid
TWindow::MakeScrollPosVisible(  const   tCIDLib::TBoolean   bHorz
                                , const tCIDLib::TCard4     c4NewPos
                                , const tCIDLib::TBoolean   bMinMove)
{
    tCIDLib::TCard4 c4CurPos;
    tCIDLib::TCard4 c4MaxPos;
    tCIDLib::TCard4 c4PageSz;
    QueryScrollInfo(bHorz, c4CurPos, c4MaxPos, c4PageSz);

    //
    //  If it's within the first page, then we just go to the 0th position.
    //  Else, let's try to center it. If centering it would go beyond the
    //  max position, then we go to the max position.
    //
    if (bMinMove)
    {
        if (c4NewPos < c4CurPos)
        {
            // It's above/left, so make it the top/left
            ScrollDrag(bHorz, c4NewPos, kCIDLib::True);
        }
         else if (c4NewPos >= c4CurPos + c4PageSz)
        {
            // It's below, right, so move back just enoujh to show it
            tCIDLib::TCard4 c4New = c4NewPos - (c4PageSz - 1);
            ScrollDrag(bHorz, c4New, kCIDLib::True);
        }
    }
     else
    {
        const tCIDLib::TInt4 i4Center
        (
            tCIDLib::TInt4(c4NewPos) - tCIDLib::TInt4(c4PageSz / 2)
        );

        tCIDLib::TCard4 c4NewPos;
        if (i4Center < 0)
            c4NewPos = 0;
        else if (i4Center > tCIDLib::TInt4(c4MaxPos))
            c4NewPos = c4MaxPos;
        else
            c4NewPos = tCIDLib::TCard4(i4Center);

        if (c4CurPos != c4NewPos)
            ScrollDrag(bHorz, c4NewPos, kCIDLib::True);
    }
}


// Return the origin of this window, in terms of the parent window's coordinates
TPoint TWindow::pntOrg() const
{
    tCIDLib::THostRectl rectlArea;
    if (!::GetWindowRect(m_hwndThis, (RECT*)&rectlArea))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_QueryArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }

    // Translate to it's parent's client coordinates if not the desktop
    tCIDCtrls::TWndHandle hwndPar = ::GetAncestor(m_hwndThis, GA_PARENT);
    if (hwndPar)
        ::ScreenToClient(hwndPar, (POINT*)&rectlArea);
    return TPoint(rectlArea.i4Left, rectlArea.i4Top);
}

TPoint TWindow::pntOrg(const tCIDCtrls::TWndId widChild) const
{
    TWindow* pwndChild = pwndChildById(widChild, kCIDLib::True);
    return pwndChild->pntOrg();
}



//
//  Return the bottom most child of this window (furthest back in the z-order). In
//  Window's reversed view of things, this is the 'last' window.
//
TWindow* TWindow::pwndBottomChild() const
{
    CheckWindowHandle();

    // Get the bottom-most window
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(m_hwndThis, GW_CHILD);
    if (!hwndCur)
        return nullptr;

    // Now move forward until we find a CIDLib window
    while (hwndCur)
    {
        TWindow* pwndCur = TWindow::pwndGetWndLinkPtr(hwndCur);
        if (pwndCur)
            return pwndCur;

        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
    }
    return nullptr;
}


//
//  Look up a child window by its id and return the window object. If the id isn't found,
//  or it's not a CIDCtrls type window, then we can either return a null pointer or
//  throw an exception.
//
TWindow*
TWindow::pwndChildById( const   tCIDCtrls::TWndId   widChild
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    CheckWindowHandle();

    // See if we have a child with this id
    tCIDCtrls::TWndHandle hwndChild = ::GetDlgItem(m_hwndThis, widChild);
    if (!hwndChild)
    {
        // If they are not tolerant of a miss, then throw
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_NoChildWithId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TCardinal(widChild)
            );
        }
        return 0;
    }

    // Extract the window object pointer that should be there
    TWindow* pwndChild = TWindow::pwndGetWndLinkPtr(hwndChild);
    if (!pwndChild)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_NotCIDWindow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TCardinal(widChild)
            );
        }
        return nullptr;
    }
    return pwndChild;
}


// Find a child by user id, returns null if not
TWindow*
TWindow::pwndChildByUserId( const   tCIDLib::TCard4     c4UserId
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(hwndSafe(), GW_CHILD);
    while (hwndCur)
    {
        TWindow* pwndChild = TWindow::pwndGetWndLinkPtr(hwndCur);
        if (pwndChild && (pwndChild->c4UserId() == c4UserId))
            return pwndChild;

        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
    }
    return nullptr;
}


//
//  Move to the next (higher) child in the z-order. Keep in mind that Windows is backwards,
//  so we get the previous child to go to the one higher in the z-order.
//
//  We can't just do a simple call to GetWindow because there might be siblines
//  that are not CIDCtrls windows.
//
TWindow* TWindow::pwndNextChild(const TWindow& wndCur) const
{
    CheckWindowHandle();

    // Make sure its really a child of ours
    if (!::IsChild(m_hwndThis, wndCur.m_hwndThis))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotAChild
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(wndCur.widThis())
        );
    }

    // Get the next child from the starting one and loop till we find one or end
    tCIDCtrls::TWndHandle hwndNext = ::GetWindow(wndCur.m_hwndThis, GW_HWNDNEXT);
    while (hwndNext)
    {
        // See if it's one of our windows
        TWindow* pwndChild = TWindow::pwndGetWndLinkPtr(hwndNext);
        if (pwndChild)
            return pwndChild;

        // Try again
        hwndNext = ::GetWindow(hwndNext, GW_HWNDNEXT);
    }

    // Never found one
    return nullptr;
}


// Return the direct child window with the focus, if any
TWindow* TWindow::pwndChildWithFocus() const
{
    TWindow* pwndRet = nullptr;

    // If we have a focus window and its parent is us, see if it's a CIDLib window
    tCIDCtrls::TWndHandle hwndFocus = ::GetFocus();
    if (hwndFocus && (::GetParent(hwndFocus) == hwndSafe()))
        pwndRet = TWindow::pwndGetWndLinkPtr(hwndFocus);

    return pwndRet;
}


// Look up a registered named window and return a pointer, null if not found
TWindow* TWindow::pwndFindNamed(const TString& strToFind) const
{
    // If the collection isn't faulted in yet, just say no
    if (!m_pcolNamedWnds)
        return nullptr;

    return m_pcolNamedWnds->pobjFindByKey(strToFind, kCIDLib::False);
}


// Finds the first child with the tab stop style
TWindow* TWindow::pwndFindFirstTab() const
{
    // Get the bottom-most window
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(m_hwndThis, GW_CHILD);
    if (!hwndCur)
        return nullptr;

    while (hwndCur)
    {
        if (::GetWindowLong(hwndCur, GWL_STYLE) & WS_TABSTOP)
            break;

        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
    }

    // If we hit bottom and didn't find one, then return a null
    if (!hwndCur)
        return nullptr;

    // Otherwise, get the CIDLib object for this window and return it
    TWindow* pwndRet = TWindow::pwndGetWndLinkPtr(hwndCur);

    // If not one, then its not a CIDLib window
    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotCIDWindow
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::AppError
            , TInteger(::GetWindowLong(hwndCur, GWL_ID))
        );
    }
    return pwndRet;
}


// Find the first window that is in the same group of windows as the start window
TWindow* TWindow::pwndFirstOfGroup(const TWindow& wndStartAt) const
{
    tCIDCtrls::TWndHandle hwndCur = wndStartAt.hwndSafe();

    // Make sure its really a child of ours
    if (!::IsChild(m_hwndThis, hwndCur))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotAChild
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(wndStartAt.widThis())
        );
    }

    // Start iterating at the start child and work towards the back
    while (hwndCur)
    {
        if (::GetWindowLong(hwndCur, GWL_STYLE) & WS_GROUP)
            break;

        hwndCur = ::GetWindow(hwndCur, GW_HWNDPREV);
    }

    // If we hit bottom and didn't find one, then return a null
    if (!hwndCur)
        return nullptr;

    // Otherwise, get the CIDLib object for this window and return it
    TWindow* pwndRet = TWindow::pwndGetWndLinkPtr(hwndCur);

    // If not one, then its not a CIDLib window
    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotCIDWindow
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::AppError
            , TInteger(::GetWindowLong(hwndCur, GWL_ID))
        );
    }
    return pwndRet;
}


//
//  Find the first window that is in the next group of windows after the group that the
//  start window is in.
//
TWindow* TWindow::pwndFirstOfNextGroup(const TWindow& wndStartAt) const
{
    // Make sure its really a child of ours
    if (!::IsChild(m_hwndThis, wndStartAt.hwndSafe()))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotAChild
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(wndStartAt.widThis())
        );
    }

    // Iterate until we find the next group (ignoring the start window)
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(wndStartAt.m_hwndThis, GW_HWNDNEXT);
    while (hwndCur)
    {
        if (::GetWindowLong(hwndCur, GWL_STYLE) & WS_GROUP)
            break;

        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);

        // Wrap around at the end
        if (!hwndCur)
        {
            hwndCur = ::GetWindow(m_hwndThis, GW_CHILD);

            // Shouldn't happen, but never say never
            if (!hwndCur)
                return nullptr;
        }

        // If we get back to the start, then do nothing
        if (hwndCur == wndStartAt.m_hwndThis)
            return nullptr;
    }

    // Otherwise, get the CIDLib object for this window and return it
    TWindow* pwndRet = TWindow::pwndGetWndLinkPtr(hwndCur);

    // If not one, then its not a CIDLib window
    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotCIDWindow
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::AppError
            , TInteger(::GetWindowLong(hwndCur, GWL_ID))
        );
    }
    return pwndRet;
}




// Return the parent window if we have one and it's a CIDLib window
TWindow* TWindow::pwndParent() const
{
    // Get the parent handle. If zero or the desktop, return null
    tCIDCtrls::TWndHandle hwndParent = ::GetAncestor(m_hwndThis, GA_PARENT);
    if (!hwndParent || (::GetDesktopWindow() == hwndParent))
        return nullptr;

    // See if there is an entry in the window map
    return TWindow::pwndGetWndLinkPtr(hwndParent);
}


//
//  Move to the previous child in the z-order. Keep in mind that Windows is backwards,
//  so we get the next child to go to the one lower in the z-order.
//
//  We can't just do a simple call to GetWindow because there might be siblines
//  that are not CIDCtrls windows.
//
TWindow* TWindow::pwndPrevChild(const TWindow& wndCur) const
{
    CheckWindowHandle();

    // Make sure its really a child of ours
    if (!::IsChild(m_hwndThis, wndCur.m_hwndThis))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotAChild
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(wndCur.widThis())
        );
    }

    // Get the next child from the starting one and loop till we find one or end
    tCIDCtrls::TWndHandle hwndPrev = ::GetWindow(wndCur.m_hwndThis, GW_HWNDPREV);
    while (hwndPrev)
    {
        // See if it's one of our windows
        TWindow* pwndChild = TWindow::pwndGetWndLinkPtr(hwndPrev);
        if (pwndChild)
            return pwndChild;

        // Try again
        hwndPrev = ::GetWindow(hwndPrev, GW_HWNDPREV);
    }

    // Never found one
    return nullptr;
}


//
//  Returns the top-most (non-child) window that contains this window. We just
//  work our way up the parentage chain. If none, we return a null pointer.
//
TWindow* TWindow::pwndRoot() const
{
    // If we found something, see if it's one of our windows
    tCIDCtrls::TWndHandle hwndRes = ::GetAncestor(hwndSafe(), GA_ROOT);
    if (!hwndRes)
        return nullptr;

    // If we got a handle, see if it's one of our windows
    TWindow* pwndTmp = nullptr;
    if (hwndRes)
        pwndTmp = TWindow::pwndGetWndLinkPtr(hwndRes);

    // Rturn what we got, if anything
    return pwndTmp;
}


//
//  Returns the top z-order child of this window, if any, else zero. In Windows'
//  reversed view of things, this is the 'first' child, not the last one.
//
TWindow* TWindow::pwndTopChild() const
{
    CheckWindowHandle();

    // Get the first child, which we need to start
    tCIDCtrls::TWndHandle hwndTop = ::GetWindow(m_hwndThis, GW_CHILD);

    // Now get the top-most window
    hwndTop = ::GetWindow(hwndTop, GW_HWNDLAST);

    // Move downwards through the z-order until we find a CIDLib window
    while (hwndTop)
    {
        // See if it's one of our windows
        TWindow* pwndChild = TWindow::pwndGetWndLinkPtr(hwndTop);
        if (pwndChild)
            return pwndChild;

        // Try again, getting the next one closer to the user
        hwndTop = ::GetWindow(hwndTop, GW_HWNDPREV);
    }
    return nullptr;
}


// Pause/unpause the indicated timer, if it has been started
tCIDLib::TVoid
TWindow::PauseTimer(const tCIDCtrls::TTimerId tmidTar, const tCIDLib::TBoolean bState)
{
    // Must be an existing timer and started
    CheckTimerId(tmidTar, kCIDLib::True);
    m_aTimers[tmidTar - 1].bPaused = bState;
}


//
//  Return the area of our client area, optionally with all 'decorations' areas
//  removed. We also can skip some, since often the client is looking to make some
//  child windows fit in the available space. Otherwise we'd remove them as well
//  and and there'd be nothing left if they already basically filled the client
//  area.
//
tCIDLib::TVoid
TWindow::QueryClientArea(       TArea&              areaToFill
                        , const tCIDLib::TBoolean   bAvailOnly) const
{
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    // Query the client rect and get it to an area object
    RECT rectCur;
    ::GetClientRect(hwndUs, &rectCur);

    areaToFill.FromRectl(*(tCIDLib::THostRectl*)&rectCur);

    //
    //  If asked for only the available area, then iterate all of the child windows
    //  and remove their area from this the client area.
    //
    if (bAvailOnly)
    {
        TArea areaCur;
        tCIDCtrls::TWndHandle hwndCur = ::GetWindow(hwndUs, GW_CHILD);
        while (hwndCur)
        {
            //
            //  If it has the visible style, and it's not the skip window, then check
            //  it.
            //
            if (::GetWindowLong(hwndCur, GWL_STYLE) & WS_VISIBLE)
            {
                //
                //  Get this guy's rectangle, to an area then remove it. We have
                //  to convert to client area coordinates.
                //
                ::GetWindowRect(hwndCur, &rectCur);
                ::MapWindowPoints(0, hwndUs, (POINT*)&rectCur, 2);
                areaCur.FromRectl(*(tCIDLib::THostRectl*)&rectCur);
                areaToFill -= areaCur;
            }

            hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
        }
    }
}


tCIDLib::TVoid
TWindow::QueryClientArea(       TArea&              areaToFill
                        , const tCIDLib::TBoolean   bAvailOnly
                        , const TWindow&            wndSkip) const
{
    tCIDCtrls::TWndList colSkip(tCIDLib::EAdoptOpts::NoAdopt, 1);
    colSkip.Add(&wndSkip);
    QueryClientArea(areaToFill, bAvailOnly, colSkip);
}

tCIDLib::TVoid
TWindow::QueryClientArea(       TArea&                  areaToFill
                        , const tCIDLib::TBoolean       bAvailOnly
                        , const tCIDCtrls::TWndList&    colSkip) const
{
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    // Query the client rect and get it to an area object
    RECT rectCur;
    ::GetClientRect(hwndUs, &rectCur);

    areaToFill.FromRectl(*(tCIDLib::THostRectl*)&rectCur);

    //
    //  If asked for only the available area, then iterate all of the child windows
    //  and remove their area from this the client area.
    //
    //  In this case we use low level windows iteration, because we want to insure
    //  we take anything into account, including stuff that we might not create.
    //
    if (bAvailOnly)
    {
        TArea areaCur;
        tCIDCtrls::TWndHandle hwndCur = ::GetWindow(hwndUs, GW_CHILD);
        while (hwndCur)
        {
            //
            //  If it has the visible style, and it's not the skip window, then check
            //  it.
            //
            if (::GetWindowLong(hwndCur, GWL_STYLE) & WS_VISIBLE)
            {
                // See if it's in the skip list
                const tCIDLib::TCard4 c4SkipCnt = colSkip.c4ElemCount();
                tCIDLib::TCard4 c4SkipInd = 0;
                while (c4SkipInd < c4SkipCnt)
                {
                    if (colSkip[c4SkipInd]->hwndThis() == hwndCur)
                        break;
                    c4SkipInd++;
                }

                if (c4SkipInd >= c4SkipCnt)
                {
                    //
                    //  Get this guy's rectangle, to an area then remove it. We have
                    //  to convert to client area coordinates.
                    //
                    ::GetWindowRect(hwndCur, &rectCur);
                    ::MapWindowPoints(0, hwndUs, (POINT*)&rectCur, 2);
                    areaCur.FromRectl(*(tCIDLib::THostRectl*)&rectCur);
                    areaToFill -= areaCur;
                }
            }

            hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
        }
    }
}


// Get full scroll info
tCIDLib::TVoid
TWindow::QueryScrollInfo(const  tCIDLib::TBoolean   bHorz
                        ,       tCIDLib::TCard4&    c4CurPos
                        ,       tCIDLib::TCard4&    c4MaxPos
                        ,       tCIDLib::TCard4&    c4PageSz)
{
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);
    Info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;

    if (!::GetScrollInfo(hwndSafe(), bHorz ? SB_HORZ : SB_VERT, &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_GetScrollPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    c4CurPos = Info.nPos;
    c4MaxPos = Info.nMax - (Info.nPage - 1);
    c4PageSz = Info.nPage;
}


//
//  Provide access to our background color. If our color has been explicitly set,
//  or if we have no parent, then we return our color. Else we return our parent's
//  color (which might in turn recurse again, up to the top level.)
//
TRGBClr TWindow::rgbBgnFill() const
{
    if (m_bBgnClrOver)
        return m_gbrBgn.rgbColor();

    TWindow* pwndPar = pwndParent();
    if (!pwndPar)
        return m_gbrBgn.rgbColor();

    return pwndPar->rgbBgnFill();
}


//
//  Forces a redraw of an area, region or client area, immediate or not, including
//  children or not
//
tCIDLib::TVoid TWindow::Redraw(const tCIDCtrls::ERedrawFlags eFlags)
{
    tCIDLib::TCard4 c4Flags = RDW_INVALIDATE;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Erase))
        c4Flags |= RDW_ERASE;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Immediate))
        c4Flags |= RDW_UPDATENOW;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Children))
        c4Flags |= RDW_ALLCHILDREN;
    else
        c4Flags |= RDW_NOCHILDREN;

    if (!::RedrawWindow(m_hwndThis, nullptr, 0, c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_Redraw
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}

tCIDLib::TVoid TWindow::Redraw( const   TArea&                  areaToRedraw
                                , const tCIDCtrls::ERedrawFlags eFlags)
{
    tCIDLib::THostRectl rectlRedraw;
    areaToRedraw.ToRectl(rectlRedraw);

    tCIDLib::TCard4 c4Flags = RDW_INVALIDATE;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Erase))
        c4Flags |= RDW_ERASE;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Immediate))
        c4Flags |= RDW_UPDATENOW;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Children))
        c4Flags |= RDW_ALLCHILDREN;
    else
        c4Flags |= RDW_NOCHILDREN;

    if (!::RedrawWindow(m_hwndThis, (RECT*)&rectlRedraw, 0, c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_Redraw
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}

tCIDLib::TVoid TWindow::Redraw( const   TGUIRegion&             grgnToRedraw
                                , const tCIDCtrls::ERedrawFlags eFlags)
{
    tCIDLib::TCard4 c4Flags = RDW_INVALIDATE;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Erase))
        c4Flags |= RDW_ERASE;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Immediate))
        c4Flags |= RDW_UPDATENOW;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ERedrawFlags::Children))
        c4Flags |= RDW_ALLCHILDREN;
    else
        c4Flags |= RDW_NOCHILDREN;

    if (!::RedrawWindow(m_hwndThis, 0, grgnToRedraw.hrgnThis(), c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_Redraw
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


//
//  Regiter the passed name window, as long as the name is not a duplicate of
//  one already registered.
//
tCIDLib::TVoid
TWindow::RegisterNamedWnd(const TString& strName, TWindow* const pwndToReg)
{
    // If the collection isn't faulted in yet, do that
    if (!m_pcolNamedWnds)
    {
        m_pcolNamedWnds = new tCIDCtrls::TKeyedWndList
        (
            tCIDLib::EAdoptOpts::NoAdopt
            , 29
            , TStringKeyOps()
            , &TWindow::strExtractName
        );
    };

    if (m_pcolNamedWnds->bKeyExists(strName))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNWnd_DupWndName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Duplicate
            , strName
        );
    }

    // Set the name on the window and add it
    pwndToReg->strName(strName);
    m_pcolNamedWnds->Add(pwndToReg);
}


tCIDLib::TVoid TWindow::ReleaseMouse()
{
    if (!::ReleaseCapture())
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_ReleaseMouse
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  Called when the window is destroyed, to remove any notificatio handlers that have
//  be registered on it.
//
tCIDLib::TVoid
TWindow::RemoveNotifyHandler(TNotHandlerBase* const pnothToRemove)
{
    // If the list is empty, then nothing to do
    if (!m_pnothHead)
        return;

    // The list isn't empty, so iterate the list and find this guy
    TNotHandlerBase* pnothCur = m_pnothHead;
    TNotHandlerBase* pnothPrev = nullptr;

    while (pnothCur)
    {
        if (pnothCur == pnothToRemove)
        {
            if (pnothPrev)
            {
                // Point the previous at our next
                pnothPrev->m_pnothNext = pnothCur->m_pnothNext;
            }
             else
            {
                // We removed the head, so make the next one the head
                m_pnothHead = pnothCur->m_pnothNext;
            }

            // Delete the one we removed, then break out
            delete pnothCur;
            break;
        }
        pnothPrev = pnothCur;
        pnothCur = pnothCur->m_pnothNext;
    }
}


// Reset a timer to make it kick off at the indicated time from now
tCIDLib::TVoid
TWindow::ResetTimer(const tCIDCtrls::TTimerId   tmidTar
                    , const tCIDLib::TCard4     c4WaitMillis)
{
    CheckTimerId(tmidTar, kCIDLib::True);
    if (!::SetTimer(hwndSafe(), tmidTar, c4WaitMillis, 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTimer_Reset
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(tmidTar)
        );
    }
}


// Get/set the text of a child window
const TString& TWindow::strChildText(const tCIDCtrls::TWndId widChild) const
{
    TWindow* const pwndChild = pwndChildById(widChild, kCIDLib::True);
    return pwndChild->strWndText();
}

const TString& TWindow::strChildText(const  tCIDCtrls::TWndId   widChild
                                    , const TString&            strNewText)
{
    TWindow* const pwndChild = pwndChildById(widChild, kCIDLib::True);
    return pwndChild->strWndText(strNewText);
}


// Get set the name value, whic is just for client code use
const TString& TWindow::strName() const
{
    return m_strName;
}

const TString& TWindow::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


//
//  Get or set this window's window text
//
const TString& TWindow::strWndText() const
{
    //
    //  See the header comments. If not a custom window that uses our m_strWndText
    //  member directly, it should load up the text from wherever it stores it.
    //
    //  WE DO NOT clear the member first. If the derived class can know that no
    //  changes were made since the last call, it can do nothing and we just keep
    //  the same text, which is much more efficient.
    //
    WndTextPreQuery(m_strWndText);
    return m_strWndText;
}

const TString& TWindow::strWndText(const TString& strNewText)
{
    // If not changed, then don't do anything, since it might be a fairy heavy operation
    if (m_strWndText == strNewText)
        return m_strWndText;

    // Store it at our level, even if the derived class might have its own storage
    m_strWndText = strNewText;

    //
    //  And allow for any other storage. Our default override does nothing, which is fine
    //  for custom windows. Else this should be overridden and the control updated as is
    //  appropriate.
    //
    WndTextStore(strNewText);

    return m_strWndText;
}


//
//  Scroll the indicated area of the windwo by the indicated H/V amount.
//  Clip the output to the clip area. We have another one that assumes the
//  clip area is the scroll area, for convenience.
//
tCIDLib::TVoid
TWindow::Scroll(const   TArea&              areaToScroll
                , const TArea&              areaClip
                ,       TArea&              areaInvalid
                , const tCIDLib::TInt4      i4HorzAmount
                , const tCIDLib::TInt4      i4VertAmount
                , const tCIDLib::TBoolean   bInvalidate)
{
    tCIDLib::THostRectl  rectScroll;
    areaToScroll.ToRectl(rectScroll);

    tCIDLib::THostRectl  rectClip;
    areaClip.ToRectl(rectClip);

    tCIDLib::THostRectl  rectInvalid;

    DWORD Flags = 0;
    if (bInvalidate)
        Flags = SW_INVALIDATE;

    if (::ScrollWindowEx(m_hwndThis
                        , i4HorzAmount
                        , i4VertAmount
                        , reinterpret_cast<RECT*>(&rectScroll)
                        , reinterpret_cast<RECT*>(&rectClip)
                        , 0
                        , reinterpret_cast<RECT*>(&rectInvalid)
                        , Flags) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_Scroll
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    areaInvalid.FromRectl(rectInvalid);
}


tCIDLib::TVoid
TWindow::Scroll(const   TArea&              areaToScroll
                ,       TArea&              areaInvalid
                , const tCIDLib::TInt4      i4HorzAmount
                , const tCIDLib::TInt4      i4VertAmount
                , const tCIDLib::TBoolean   bInvalidate)
{
    Scroll
    (
        areaToScroll, areaToScroll, areaInvalid, i4HorzAmount, i4VertAmount, bInvalidate
    );
}


//
//  This will cause a call to this window's CodeReceived() method, either
//  synchronously or asynchronously. See the common message procedure for
//  the dispatching.
//
tCIDLib::TVoid
TWindow::SendCode(  const   tCIDLib::TInt4      i4ToPost
                    , const tCIDLib::TCard4     c4Data
                    , const tCIDLib::ESyncTypes eSync)
{
    if (eSync == tCIDLib::ESyncTypes::Synchronous)
        ::SendMessage(m_hwndThis, kCIDCtrls::wmsgAsyncEvent2, c4Data, i4ToPost);
    else
        ::PostMessage(m_hwndThis, kCIDCtrls::wmsgAsyncEvent2, c4Data, i4ToPost);
}


//
//  This will cause a call to this window's DataReceived() method, either
//  synchronously or asynchronously. See the common message procedure for
//  the dispatching.
//
tCIDLib::TVoid
TWindow::SendData(  const   tCIDLib::TCard4         c4DataId
                    ,       tCIDLib::TVoid* const   pToSend
                    , const tCIDLib::ESyncTypes     eSync)
{
    if (eSync == tCIDLib::ESyncTypes::Synchronous)
        ::SendMessage(m_hwndThis, kCIDCtrls::wmsgSyncEvent1, (WPARAM)pToSend, c4DataId);
    else
        ::PostMessage(m_hwndThis, kCIDCtrls::wmsgSyncEvent1, (WPARAM)pToSend, c4DataId);
}


// Set the scroll position on a standard scroll bar, horz or vert
tCIDLib::TVoid
TWindow::SetScrollPos(  const   tCIDLib::TBoolean   bHorz
                        , const tCIDLib::TCard4     c4Pos)
{
    if (c4Pos > c4MaxScrollPos(bHorz))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcScrA_BadPos
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Pos)
            , TCardinal(widThis())
        );
    }

    ::SetScrollPos
    (
        hwndSafe()
        , bHorz ? SB_HORZ : SB_VERT
        , c4Pos
        , kCIDLib::True
    );
}


// Give the focus to a child of ours
tCIDLib::TVoid TWindow::SetChildFocus(const tCIDCtrls::TWndId widTarget)
{
    TWindow* pwndTar = pwndChildById(widTarget, kCIDLib::False);
    if (pwndTar)
    {
        if (m_bDlgProc)
        {
            ::SendMessage
            (
                hwndSafe()
                , WM_NEXTDLGCTL
                , tCIDCtrls::TWParam(pwndTar->hwndSafe())
                , TRUE
            );
        }
         else
        {
            pwndTar->TakeFocus();
        }
    }
}


//
// Enable or disable this window or a child of this window
//
tCIDLib::TVoid TWindow::SetEnable(const tCIDLib::TBoolean bState)
{
    CheckWindowHandle();

    ::EnableWindow(m_hwndThis, bState ? 1 : 0);

}
tCIDLib::TVoid
TWindow::SetEnable(const tCIDCtrls::TWndId widChild, const tCIDLib::TBoolean bState)
{
    CheckWindowHandle();

    tCIDCtrls::TWndHandle hwndChild = hwndChildById(widChild, kCIDLib::True);
    ::EnableWindow(hwndChild, bState ? 1 : 0);
}



// Set the state of an extended window style, after the fact
tCIDLib::TVoid
TWindow::SetExWndStyle( const   tCIDCtrls::EExWndStyles eToSet
                        , const tCIDLib::TBoolean       bSetIt)
{
    tCIDLib::TCard4 c4Cur = ::GetWindowLong(hwndSafe(), GWL_EXSTYLE);

    if (bSetIt)
        c4Cur |= tCIDLib::TCard4(eToSet);
    else
        c4Cur &= ~tCIDLib::TCard4(eToSet);

    ::SetWindowLong(hwndThis(), GWL_EXSTYLE, c4Cur);
}


//
//  Allow encapsulated standard controls to set their font. For custom windows,
//  this isn't used, they just use an appropriate font during their own drawing.
//  It can be used to set an on the fly font on standard controls as well, which is
//  not often needed but sometimes, such as a font selection dialog sample text
//  window. For the most part though, we very much want the standard fonts being
//  used.
//
//  NOTE that the font object must be kept around until this window is closed! If
//  a window itself is setting a new font, then that's easy, he just makes the
//  font a member of himself. But if doing it for a child, keep the font around in
//  the parent.
//
tCIDLib::TVoid TWindow::SetFont(const TGUIFont& gfontToSet)
{
    ::SendMessage
    (
        hwndThis()
        , WM_SETFONT
        , tCIDCtrls::TWParam(gfontToSet.hfontThis())
        , 0
    );
}


//
//  Set up the large or small icon on this window
//
tCIDLib::TVoid
TWindow::SetIcon(const TIcon& icoToSet, const tCIDLib::TBoolean bLarge)
{
    //
    //  Tell the system about the change.
    //
    //  NOTE:   We dup the handles, because he owns them now and the caller
    //          expects to keep his own handles.
    //
    tCIDCtrls::TWParam wToSet(bLarge ? ICON_BIG : ICON_SMALL);
    ::SendMessage
    (
        hwndSafe(), WM_SETICON, wToSet, (LPARAM)::CopyIcon(icoToSet.hicoThis())
    );
}


// Give this window a new id. It can't be a top level window
tCIDLib::TVoid TWindow::SetId(const tCIDCtrls::TWndId widToSet)
{
    if (!::SetWindowLong(m_hwndThis, GWL_ID, widToSet))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetId
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Set a new owner for this widnow
tCIDLib::TVoid TWindow::SetOwner(const TWindow& wndNewOwner)
{
    if (!::SetParent(m_hwndThis, wndNewOwner.hwndSafe()))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetOwner
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Set a new parent for this window
tCIDLib::TVoid TWindow::SetParent(const TWindow& wndNewParent)
{
    if (!::SetParent(m_hwndThis, wndNewParent.hwndSafe()))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetParent
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Change the origin of this window
tCIDLib::TVoid TWindow::SetPos(const TPoint& pntNewOrg)
{
    tCIDLib::TCard4 c4Flags = SWP_NOOWNERZORDER | SWP_NOSIZE
                              | SWP_NOZORDER
                              | SWP_NOACTIVATE;

    if (!::SetWindowPos
    (
        m_hwndThis
        , kCIDCtrls::hwndInvalid
        , pntNewOrg.i4X()
        , pntNewOrg.i4Y()
        , 0
        , 0
        , c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pntNewOrg
            , TCardinal(m_widThis)
        );
    }
}


//
//  Set up one of the standard scroll bar. The caller gives us the full available
//  content (the stuff to scroll) and the visible area to scroll it within, and the
//  size of a single line. We figure out everythine we need from that.
//
//  NOTE WE do not deal with partial rows/columns here. For scrolling purposes you
//  want partials to be ignored so that if half a line is showing at the end, a
//  page down will take you to the top of that partial line so that you can see it.
//
tCIDLib::TVoid
TWindow::SetScrollBar(  const   tCIDLib::TBoolean   bHorz
                        , const tCIDLib::TCard4     c4FullSize
                        , const tCIDLib::TCard4     c4VisSize
                        , const tCIDLib::TCard4     c4LineSz)
{
    // We'll get info on both scroll bars
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);

    Info.fMask = SIF_PAGE | SIF_RANGE;
    Info.nMin = 0;

    // Calculate the page size and max value
    Info.nPage = c4VisSize / c4LineSz;
    Info.nMax = (c4FullSize / c4LineSz) - 1;

    ::SetScrollInfo(hwndSafe(), bHorz ? SB_HORZ : SB_VERT, &Info, TRUE);
}


//
//  Set our size, leaving our origin unchanged. We can retain existing bits or
//  just have the whole window invalidated.
//
tCIDLib::TVoid TWindow::SetSize(const   TSize&              szNew
                                , const tCIDLib::TBoolean   bCopyBits)
{
    SetSize(szNew.c4Width(), szNew.c4Height(), bCopyBits);
}

tCIDLib::TVoid
TWindow::SetSize(const  tCIDLib::TCard4     c4CX
                , const tCIDLib::TCard4     c4CY
                , const tCIDLib::TBoolean   bCopyBits)
{
    CheckWindowHandle();

    tCIDLib::TCard4 c4Flags = SWP_NOOWNERZORDER | SWP_NOMOVE
                              | SWP_NOZORDER | SWP_NOACTIVATE;

    // If we aren't copying bits, then add that flags
    if (!bCopyBits)
        c4Flags |= SWP_NOCOPYBITS;

    if (!::SetWindowPos
    (
        m_hwndThis
        , 0
        , 0
        , 0
        , c4CX
        , c4CY
        , c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetSize
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
            , TSize(c4CX, c4CY)
        );
    }
}


//
//  Set the size or size and positoin of this window
//
tCIDLib::TVoid
TWindow::SetSizePos(const TArea& areaNew, const tCIDLib::TBoolean bCopyBits)
{
    CheckWindowHandle();

    tCIDLib::TCard4 c4Flags = SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE;

    // If we aren't copying bits, then add that flags
    if (!bCopyBits)
        c4Flags |= SWP_NOCOPYBITS;

    if (!::SetWindowPos
    (
        m_hwndThis
        , 0
        , areaNew.i4X()
        , areaNew.i4Y()
        , areaNew.c4Width()
        , areaNew.c4Height()
        , c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
            , areaNew
        );
    }
}



//
// Hide or show this window or a child of this window
//
tCIDLib::TVoid
TWindow::SetVisibility(const    tCIDLib::TBoolean   bState
                        , const tCIDLib::TBoolean   bActivate)
{
    CheckWindowHandle();

    int iCmd;
    if (bState)
    {
        if (bActivate)
            iCmd = SW_SHOW;
        else
            iCmd = SW_SHOWNA;
    }
     else
    {
        iCmd = SW_HIDE;
    }
    ::ShowWindow(m_hwndThis, iCmd);
}


tCIDLib::TVoid
TWindow::SetVisibility( const   tCIDCtrls::TWndId   widChild
                        , const tCIDLib::TBoolean   bState
                        , const tCIDLib::TBoolean   bActivate)
{
    tCIDCtrls::TWndHandle hwndChild = hwndChildById(widChild, kCIDLib::True);

    int iCmd;
    if (bState)
    {
        if (bActivate)
            iCmd = SW_SHOW;
        else
            iCmd = SW_SHOWNA;
    }
     else
    {
        iCmd = SW_HIDE;
    }
    ::ShowWindow(hwndChild, iCmd);
}


//
//  Change the passed window's z-order. It changes this window's z-order such that
//  it is after the passed window (which must be a sibling.) That means that it will
//  be in front of the passed window.
//
tCIDLib::TVoid TWindow::SetZOrder(const TWindow& wndInsertAfter)
{
    if (!::SetWindowPos(m_hwndThis
                        , wndInsertAfter.m_hwndThis
                        , 0
                        , 0
                        , 0
                        , 0
                        , SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetZOrder
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


//
//  Stops all active window timers.
//
tCIDLib::TVoid TWindow::StopAllTimers()
{
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    tCIDCtrls::TTimerId tmidCur = 0;
    while (tmidCur < kCIDCtrls::c4MaxWndTimers)
    {
        if (m_aTimers[tmidCur].bStarted)
            StopTimer(tmidCur + 1);
        tmidCur++;
    }
}


//
// Stop a timer if it has been started
//
//  NOTE we might be in the callback for this timer right now. And will clear the
//  entered flag when it exits. So we don't want to clear it ourself because that
//  could allow it to reenter. The only way it would be set is if we are in the
//  callback, and he will clear it when do.
//
//  Note that tmidStartTimer will not use one whose entered flag is set, even if the
//  started flag is false.
//
tCIDLib::TVoid TWindow::StopTimer(const tCIDCtrls::TTimerId tmidTar)
{
    // Make sure it's valid
    CheckTimerId(tmidTar);

    // And, if it is active, then kill it at the OS level
    if (m_aTimers[tmidTar].bStarted)
    {
        if (!::KillTimer(hwndSafe(), tmidTar))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTimer_Stop
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(tmidTar)
            );
        }
}

    m_aTimers[tmidTar - 1].bPaused = kCIDLib::False;
    m_aTimers[tmidTar - 1].bStarted = kCIDLib::False;
}


//
//  Start a new timer and return the id. NOTE THAT we will not choose one whose
//  entered flag is set, even if its started flag is not. See the StopTimer() method
//  commands for why.
//
tCIDCtrls::TTimerId
TWindow::tmidStartTimer(const   tCIDLib::TCard4     c4WaitMillis
                        , const tCIDLib::TBoolean   bStartPaused)
{
    // Find an id to use
    tCIDCtrls::TTimerId tmidRet = 0;
    while (tmidRet < kCIDCtrls::c4MaxWndTimers)
    {
        if (!m_aTimers[tmidRet].bStarted && !m_aTimers[tmidRet].bEntered)
            break;
        tmidRet++;
    }

    // If none available, then throw
    if (tmidRet == kCIDCtrls::c4MaxWndTimers)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTimer_NoIdAvail
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    // Set it up before we start it in case we need to start paused
    m_aTimers[tmidRet].bEntered = kCIDLib::False;
    m_aTimers[tmidRet].bStarted = kCIDLib::True;
    m_aTimers[tmidRet].bPaused  = bStartPaused;

    // Bump the id up now to make it the actual id
    tmidRet++;

    // And now start up the actual timer and return the id
    if (!::SetTimer(hwndSafe(), tmidRet, c4WaitMillis, 0))
    {
        // It failed, so undo the above
        m_aTimers[tmidRet - 1].bEntered = kCIDLib::False;
        m_aTimers[tmidRet - 1].bStarted = kCIDLib::False;
        m_aTimers[tmidRet - 1].bPaused  = kCIDLib::False;

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTimer_Start
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(tmidRet)
        );
    }
    return tmidRet;
}


// Take the focus to ourself
tCIDLib::TVoid TWindow::TakeFocus()
{
    CheckWindowHandle();
    ::SetFocus(m_hwndThis);
}


//
//  Move this window to the bottom of the Z-order. In this case, Window's terminology
//  works for our purposes, bottom means furthest away here.
//
tCIDLib::TVoid TWindow::ToBottom()
{
    if (!::SetWindowPos(m_hwndThis
                        , HWND_BOTTOM
                        , 0
                        , 0
                        , 0
                        , 0
                        , SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetZOrder
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


//
//  These are just conveniences, for this very common conversion from a
//  window's client coordinate system to the screen system.
//
//  NOTE: We are converting from client coordinates, so you should call this on the
//  containing window with the coordinates of things inside that window. Don't call
//  it on a window using its own coordinates, i.e. don't try to translate the origin
//  of child control X by calling this on X. Call it on X's parent.
//
tCIDLib::TVoid
TWindow::ToScreenCoordinates(const TArea& areaSrc, TArea& areaTarget) const
{
    POINT ptTmp;
    ptTmp.x = areaSrc.i4X();
    ptTmp.y = areaSrc.i4Y();
    ::ClientToScreen(m_hwndThis, &ptTmp);
    areaTarget.i4X(ptTmp.x);
    areaTarget.i4Y(ptTmp.y);
    areaTarget.c4Width(areaSrc.c4Width());
    areaTarget.c4Height(areaSrc.c4Height());
}

tCIDLib::TVoid
TWindow::ToScreenCoordinates(const TPoint& pntSrc, TPoint& pntTarget) const
{
    POINT ptTmp;
    ptTmp.x = pntSrc.i4X();
    ptTmp.y = pntSrc.i4Y();
    ::ClientToScreen(m_hwndThis, &ptTmp);
    pntTarget.i4X(ptTmp.x);
    pntTarget.i4Y(ptTmp.y);
}


// Move this window to the top of the zorder
tCIDLib::TVoid TWindow::ToTop(const tCIDLib::TBoolean bActivate)
{
    // Set up the flags
    tCIDLib::TCard4 c4Flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER;
    if (!bActivate)
        c4Flags |= SWP_NOACTIVATE;

    if (!::SetWindowPos(m_hwndThis, HWND_TOP, 0, 0, 0, 0, c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetZOrder
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_widThis)
        );
    }
}


// Proivde access to our window id. It will be zero if this window isn't active
tCIDCtrls::TWndId TWindow::widThis() const
{
    return m_widThis;
}


// Get a child of this window. It'll throw if not found
TWindow& TWindow::wndChildById(const tCIDCtrls::TWndId widChild)
{
    return *pwndChildById(widChild, kCIDLib::True);
}


// Look up a registered named window and return a ref. Thrown if not found or valid
TWindow& TWindow::wndFindNamed(const TString& strToFind)
{
    TWindow* pwndRet = nullptr;
    if (m_pcolNamedWnds)
        pwndRet = m_pcolNamedWnds->pobjFindByKey(strToFind, kCIDLib::False);

    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNWnd_WndNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
        );
    }

    if (!pwndRet->bIsValid())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNWnd_WndNotValid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , strToFind
        );
    }
    return *pwndRet;
}


//
//  Returns the parent of this window. In this case, if there is not one, it will
//  throw.
//
TWindow& TWindow::wndParent() const
{
    TWindow* pwndRet = pwndParent();
    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_GetParent
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return *pwndRet;
}


//
//  Returns the top-most (non-child) window that contains this window. We call the
//  pointer returning version and throw if he returns null.
//
TWindow& TWindow::wndRoot() const
{
    // Call the pointer returning version. If it returns null, then throw
    TWindow* pwndRet = pwndRoot();
    if (!pwndRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NoTopLevelWnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return *pwndRet;
}


//
//  These methods xlat points and areas from the coordinate system of another
//  window into the coordinate system of this window.
//
//  NOTE:   The value returned from MapWindowPoints() can legitimately be
//          zero, but zero also can be an error. So we have to check manually.
//
tCIDLib::TVoid
TWindow::XlatCoordinates(TArea& areaToXlat, const TWindow& wndSrc) const
{
    tCIDLib::THostRectl rectlXlat;
    areaToXlat.ToRectl(rectlXlat);
    ::SetLastError(0);
    if (!::MapWindowPoints(wndSrc.m_hwndThis, m_hwndThis, (POINT*)&rectlXlat, 2))
    {
        const tCIDLib::TCard4 c4Err = ::GetLastError();
        if (c4Err)
        {
            TKrnlError::SetLastHostError(c4Err);
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_MapWndCoordinates
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToXlat
            );
        }
    }
    areaToXlat.FromRectl(rectlXlat);
}

tCIDLib::TVoid
TWindow::XlatCoordinates(TPoint& pntToXlat, const TWindow& wndSrc) const
{
    tCIDLib::THostPoint ptXlat;
    ptXlat.i4X = pntToXlat.i4X();
    ptXlat.i4Y = pntToXlat.i4Y();
    ::SetLastError(0);
    if (!::MapWindowPoints(wndSrc.m_hwndThis, m_hwndThis, (POINT*)&ptXlat, 1))
    {
        const tCIDLib::TCard4 c4Err = ::GetLastError();
        if (c4Err)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_MapWndCoordinates
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntToXlat
            );
        }
    }
    pntToXlat = ptXlat;
}



// ---------------------------------------------------------------------------
// TWindow: Hidden constructors
// ---------------------------------------------------------------------------
TWindow::TWindow(const  tCIDLib::TBoolean bDlgProc) :

    m_areaPrev(kCIDLib::i2MinInt, kCIDLib::i2MinInt, 0, 0)
    , m_bAreaTracking(kCIDLib::False)
    , m_bBgnClrOver(kCIDLib::False)
    , m_bDlgProc(bDlgProc)
    , m_bDragMode(kCIDLib::False)
    , m_bIsDialog(kCIDLib::False)
    , m_bNoReflect(kCIDLib::False)
    , m_bOwnsHandle(kCIDLib::True)
    , m_bPropagateKeys(kCIDLib::False)
    , m_bWantsChildFocusNot(kCIDLib::False)
    , m_c4EnteredLoops(0)
    , m_c4SuppressPaint(0)
    , m_c4TimerSuspend(0)
    , m_c4UserId(kCIDLib::c4MaxCard)
    , m_eWantInput(tCIDCtrls::EWantInputs::None)
    , m_eEdgeAnchor(tCIDCtrls::EEdgeAnchors::None)
    , m_eLastPosState(tCIDCtrls::EPosStates::Count)
    , m_hwndThis(0)
    , m_pcolNamedWnds(nullptr)
    , m_pfOrgProc(nullptr)
    , m_pnothHead(nullptr)
    , m_pnotrHead(nullptr)
    , m_widThis(0)
{
    // Initialize the timer structures
    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDCtrls::c4MaxWndTimers; c4Index++)
    {
        m_aTimers[c4Index].bEntered = kCIDLib::False;
        m_aTimers[c4Index].bPaused  = kCIDLib::False;
        m_aTimers[c4Index].bStarted = kCIDLib::False;
    }
}


// ---------------------------------------------------------------------------
// TWindow: Protected, virtual methods
// ---------------------------------------------------------------------------

// Our size and/or position has changed relative to our parent
tCIDLib::TVoid
TWindow::AreaChanged(const  TArea&
                    , const TArea&
                    , const tCIDCtrls::EPosStates
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean)
{
    // Default is to do nothing
}


tCIDLib::TBoolean
TWindow::bChar( const   tCIDLib::TCh        chChar
                , const tCIDLib::TBoolean   bAlt
                , const tCIDLib::TBoolean   bCtrl
                , const tCIDLib::TBoolean   bShift
                , const TWindow* const      )
{
    // If propagate keys is set, then pass to our parent if we have one
    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (m_bPropagateKeys)
    {
        TWindow* pwndPar = pwndParent();
        if (pwndPar)
            bRes = pwndPar->bChar(chChar, bAlt, bCtrl, bShift, this);
    }
    return bRes;
}


// Handle any mouse clicks in our window
tCIDLib::TBoolean
TWindow::bClick(const   tCIDCtrls::EMouseButts
                , const tCIDCtrls::EMouseClicks
                , const TPoint&
                , const tCIDLib::TBoolean
                , const tCIDLib::TBoolean)

{
    // Default is to say we didn't process it
    return kCIDLib::False;
}


// We've just been created. If not overridden, just return true to continue
tCIDLib::TBoolean TWindow::bCreated()
{
    return kCIDLib::True;
}


//
//  Called to give classes that subclass existing system controls a change to eat
//  msgs, or let them be passed on.
//
tCIDLib::TBoolean
TWindow::bEatSubClassMsg(const  tCIDCtrls::TWndMsg
                        , const tCIDCtrls::TWParam
                        , const tCIDCtrls::TLParam
                        ,       tCIDCtrls::TMsgResult&)
{
    //
    //  Default is to not eat, though this should never be called except for those
    //  derived classes that call SetSubclass() and therefore they should always have
    //  overridden this or there's no point in it.
    //
    return kCIDLib::False;
}


//
//  If not overridden, we will fill the background with the default background color.
//  We just fill the whole client area. The clipping region will clip out the fill except
//  where it's needed.
//
tCIDLib::TBoolean TWindow::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    gdevToUse.Fill(areaClient(), gbrBgn());
    return kCIDLib::True;
}


// Handle an extended key press
tCIDLib::TBoolean
TWindow::bExtKey(const  tCIDCtrls::EExtKeys eKeys
                , const tCIDLib::TBoolean   bAlt
                , const tCIDLib::TBoolean   bCtrl
                , const tCIDLib::TBoolean   bShift
                , const TWindow* const      )
{
    // If propagate keys is set, then pass to our parent if we have one
    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (m_bPropagateKeys)
    {
        TWindow* pwndPar = pwndParent();
        if (pwndPar)
            bRes = pwndPar->bExtKey(eKeys, bAlt, bCtrl, bShift, this);
    }
    return bRes;
}


//
//  A file was dropped on us (and we were marked as accepting it). Just say we did
//  not handle it.
//
tCIDLib::TBoolean TWindow::bFilesDropped(const  tCIDLib::TStrList&)
{
    return kCIDLib::False;
}


// Handle a media key
tCIDLib::TBoolean
TWindow::bMediaKey( const   tCIDCtrls::EExtKeys     eKey
                    , const tCIDLib::TBoolean       bAltShift
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift
                    , const TWindow* const          )
{
    // If propagate keys is set, then pass to our parent if we have one
    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (m_bPropagateKeys)
    {
        TWindow* pwndPar = pwndParent();
        if (pwndPar)
            bRes = pwndPar->bMediaKey(eKey, bAltShift, bCtrlShift, bShift, this);
    }
    return bRes;
}


// If not overridden, just say we didn't handle it
tCIDLib::TBoolean
TWindow::bMouseMove(const   TPoint&
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean)
{
    return kCIDLib::False;
}


// If not overridden, say we didn't handle it, so it will propagate
tCIDLib::TBoolean TWindow::bMouseWheel(const tCIDLib::TInt4 i4Clicks)
{
    return kCIDLib::False;
}


//
//  WM_NOTIFY and WM_COMMAND messages are reflected back to the sending window
//  object by a call to this method. Derived classes override this and send out
//  notifications through our own notification system, which is must better than
//  the Windows stuff.
//
//  wndTar is the original target window, so that's who the notification needs
//  to be sent to.
//
tCIDLib::TBoolean
TWindow::bNotReflect(       TWindow&
                    , const tCIDCtrls::TWndMsg
                    , const tCIDCtrls::TWParam
                    , const tCIDCtrls::TLParam
                    ,       tCIDCtrls::TMsgResult&)
{
    // If not handled, we just return false
    return kCIDLib::False;
}


//
//  Called when a paint msg arrives. If we return false then the default action is
//  taken. If not overridden, we just return false.
//
tCIDLib::TBoolean TWindow::bPaint(TGraphDrawDev&, const TArea&)
{
    return kCIDLib::False;
}


// Called on parent to let it set the pointer over a child
tCIDLib::TBoolean
TWindow::bSetChildPointer(const tCIDCtrls::EWndAreas, const TWindow&)
{
    // Say we don't handle it
    return kCIDLib::False;
}


//
//  Most of the time, derived classes, if they set the pointer at all, will set it
//  for the standard reasons, an arrow when in the client and the sizing arrows when
//  on edges or corners. This method provides that functionality, based on a hit
//  test area reported to us by the system as the user moves the mouse.
//
//  If they want something else, they can overrride this method.
//
//  If the window is not sizeable, then the system will report the 'nowhere' area,
//  and we'll just let it propogate to the system, for a default.
//
tCIDLib::TBoolean TWindow::bSetPointer(const tCIDCtrls::EWndAreas eHitTest)
{
    // Let this one propogate to the system
    if (eHitTest == tCIDCtrls::EWndAreas::Nowhere)
        return kCIDLib::False;

    tCIDLib::TCh* pszId;
    switch(eHitTest)
    {
        case tCIDCtrls::EWndAreas::Client :
            pszId = IDC_ARROW;
            break;

        case tCIDCtrls::EWndAreas::Left :
        case tCIDCtrls::EWndAreas::Right :
            pszId = IDC_SIZEWE;
            break;

        case tCIDCtrls::EWndAreas::Top :
        case tCIDCtrls::EWndAreas::Bottom :
            pszId = IDC_SIZENS;
            break;

        case tCIDCtrls::EWndAreas::TopLeft :
        case tCIDCtrls::EWndAreas::BottomRight :
            pszId = IDC_SIZENWSE;
            break;

        case tCIDCtrls::EWndAreas::TopRight :
        case tCIDCtrls::EWndAreas::BottomLeft :
            pszId = IDC_SIZENESW;
            break;

        default :
            pszId = IDC_ARROW;
            break;
    }

    ::SetCursor(LoadCursor(NULL, pszId));
    return kCIDLib::True;
}


// If not overridden say we didn't handle it, so it'll propagate
tCIDLib::TBoolean TWindow::bShowHelp(const tCIDCtrls::TWndId, const TPoint&)
{
    return kCIDLib::False;
}


// If not overridden, do nothing
tCIDLib::TBoolean TWindow::bVisibilityChange(const tCIDLib::TBoolean bShowing)
{
    return kCIDLib::False;
}


// Default is to do nothing
tCIDLib::TVoid TWindow::ChildAreaChanged(const tCIDCtrls::TWndId, const TArea&)
{
}


// Empty default implementation
tCIDLib::TVoid TWindow::CodeReceived(const tCIDLib::TInt4, const tCIDLib::TCard4)
{
}


// If not overridden we just do nothing
tCIDLib::TVoid
TWindow::CustomDraw(        TGraphDrawDev&
                    , const tCIDLib::TCard4
                    , const tCIDCtrls::ECustDrFlags
                    , const TArea&)
{
}


// Empty default implementation
tCIDLib::TVoid
TWindow::DataReceived(const tCIDLib::TCard4, tCIDLib::TVoid* const)
{
}


// Default is not to do anything
tCIDLib::TVoid TWindow::Destroyed()
{
}


// Default is to do nothing
tCIDLib::TVoid TWindow::DisplayChanged(const tCIDLib::TCard4, const TSize&)
{
}


// Default is to do nothing
tCIDLib::TVoid TWindow::DragEnd()
{
}


// Default is to do nothing
tCIDLib::TVoid TWindow::DragStart()
{
}


//
//  If not overridden, we return false to let it be handled by default. Mostly
//  only custom controls that accept keyboard input need to respond to this, to
//  get themselves actived and give themselves the focus.
//
tCIDCtrls::EMouseActs TWindow::eMouseActivate()
{
    return tCIDCtrls::EMouseActs::None;
}


//
//  If not overridden, just say let propogate to the parent window. The TStdCtrlWnd
//  class will override this and by default return Pass, to let it go to the
//  underlying standard control for any default popup it provides. So the parent will
//  never see this for a standard control unless the control wrapper class provides
//  an option to propogate it, or the application creates a derivative of the control
//  wrapper and overrides this method.
//
tCIDCtrls::ECtxMenuOpts
TWindow::eShowContextMenu(const TPoint&, const tCIDCtrls::TWndId)
{
    return tCIDCtrls::ECtxMenuOpts::Propogate;
}


//
//  If not overridden we just do an immediate redraw. This is only called for custom windows,
//  not standard ones. The standard ones handle their own affairs on this.
//
tCIDLib::TVoid TWindow::EnableStateChanged()
{
    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


// Default is not to do anything
tCIDLib::TVoid TWindow::GettingFocus()
{
}


// Default is not to do anything
tCIDLib::TVoid TWindow::LosingFocus()
{
}


//
//  The default implementation of the dispatcher. If not overridden, then we
//  just call the default handler, or maybe do some basic processing here for
//  ourselves.
//
//  We break out some stuff to allow us to call it from multiple places, basically
//  for standard controls and user windows. To keep things from getting crazy, we
//  specially deal with standard controls, but that means we have to sometimes invoke
//  callbacks in two places.
//
tCIDLib::TVoid
TWindow::HandleNotification(const   TWindow&            wndThis
                            , const tCIDCtrls::TWParam  wParam
                            , const tCIDCtrls::TLParam  lParam)
{
    TWndEvent* pwevData = reinterpret_cast<TWndEvent*>(wParam);

    //
    //  Check for any handlers for this event, which there should
    //  be if we got this message. For each one, invoke the
    //  handler for it.
    //
    try
    {
        TNotHandlerBase* pnothCur = wndThis.m_pnothHead;
        while (pnothCur)
        {
            if (pnothCur->nidRequested() == pwevData->nidBeingSent())
            {
                //
                //  Invoke the handler with the data. If it returns that
                //  it has handled the event, then don't do any more
                //  handlers.
                //
                const tCIDCtrls::EEvResponses eEvRes = pnothCur->eInvoke
                (
                    pwevData->pobjToSend()
                );
                if (eEvRes == tCIDCtrls::EEvResponses::Handled)
                    break;
            }
            pnothCur = pnothCur->pnothNext();
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        delete pwevData;
        throw;
    }

    // And now delete the event we got
    delete pwevData;
}


tCIDCtrls::TMsgResult
TWindow::mresDispatch(  const   TWindow&            wndThis
                        , const tCIDCtrls::TWndMsg  wmsgCur
                        , const tCIDCtrls::TWParam  wParam
                        , const tCIDCtrls::TLParam  lParam)
{
    tCIDCtrls::TMsgResult mresRet = 0;

    //
    //  Allow any derived class to look at raw messages and eat them if it
    //  chooses to. Mostly this is for sub-classes standard controls, but sometimes
    //  it allows specialized custom classes to handle msgs that we don't have any
    //  virtual methods for.
    //
    mresRet = 0;
    if (bEatSubClassMsg(wmsgCur, wParam, lParam, mresRet))
        return mresRet;

    //
    //  If this is a standard control, which we will have sub-classed, we still
    //  want to allow our wrapper class to see some msgs so that it can process them.
    //  Otherwise, we just let him do his own thing.
    //
    if (m_pfOrgProc)
    {
        try
        {
            switch(wmsgCur)
            {
                case kCIDCtrls::wmsgAsyncEvent1 :
                {
                    //
                    //  This is an async notification that has been sent by some window to
                    //  us. We need to see if we have a handler for this and, if so, invoke
                    //  it.
                    //
                    HandleNotification(wndThis, wParam, lParam);
                    return 0;
                }

                // An async post of of a code
                case kCIDCtrls::wmsgAsyncEvent2 :
                {
                    // Just call the code recieved callback
                    CodeReceived(lParam, wParam);
                    break;
                }

                case kCIDCtrls::wmsgSyncEvent1 :
                {
                    // Just call the data received callback
                    DataReceived(tCIDLib::TCard4(lParam), (tCIDLib::TVoid*)wParam);
                    break;
                }

                case kCIDCtrls::wmsgChildFocusNot :
                {
                    TWindow* pwndPar = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(wParam));

                    //
                    //  Should always be a valid window, but just in case, and it's possible
                    //  it's dead now as well, since this is as async msg.
                    //
                    if (pwndPar && pwndPar->bIsValid())
                    {
                        ChildFocusChange
                        (
                            *pwndPar
                            , tCIDCtrls::TWndId(tCIDCtrls::TWndId(lParam & 0x7FFFFFFF))
                            , (lParam & 0x80000000) != 0
                        );
                    }
                    break;
                };

                case WM_CHAR :
                {
                    if (!(lParam & KF_UP) && !(lParam & KF_EXTENDED))
                    {
                        tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
                        tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
                        tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

                        tCIDLib::TCh chChar = tCIDLib::TCh(wParam);

                        //
                        //  If control shifted and the char is 1 to 26 then it's a control
                        //  shifted A to Z, so make life easier and translate that.
                        //
                        if (bCtrl && !bAlt && !bShift && (chChar >= 1) && (chChar <= 26))
                            chChar = tCIDLib::TCh(kCIDLib::chLatin_a + (chChar - 1));

                        if ((chChar != kCIDLib::chBS)
                        &&  (chChar != kCIDLib::chCR)
                        &&  (chChar != kCIDLib::chEsc)
                        &&  (chChar != kCIDLib::chTab))
                        {
                            if (bChar(chChar, bAlt, bCtrl, bShift))
                                return 0;
                        }
                    }
                    break;
                }

                case WM_COMMAND :
                {
                    //
                    //  Even if it's a standard control, we may still want it
                    //  to be able to handle translated accelerator commands if
                    //  we derive from it. We still let it propogate just in case.
                    //
                    if ((wParam >> 16) == 1)
                        MenuCommand(wParam & 0xFFFF, kCIDLib::False, kCIDLib::True);
                    break;
                }

                case WM_CONTEXTMENU :
                {
                    //
                    //  Give the class a chance to either provide a context menu or
                    //  to override the default one.
                    //
                    TPoint pntAt
                    (
                        TRawBits::c2Low16From32(lParam)
                        , TRawBits::c2High16From32(lParam)
                    );

                    if ((pntAt.i4X() == 65535) && (pntAt.i4Y() == 65535))
                        pntAt.Set(-1, -1);

                    tCIDCtrls::TWndId widSrc = kCIDLib::c4MaxCard;
                    tCIDCtrls::TWndHandle hwndSrc = (tCIDCtrls::TWndHandle)wParam;
                    if (hwndSrc != kCIDCtrls::hwndInvalid)
                        widSrc = ::GetWindowLong(hwndSrc, GWL_ID);

                    //
                    //  We have to handle propagation ourself, otherwise the original
                    //  window id is is lost if it goes through more than one
                    //  containing window. We start off reflecting it back to the
                    //  original window.
                    //
                    TWindow* pwndCur = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(hwndSrc));
                    while (pwndCur)
                    {
                        const tCIDCtrls::ECtxMenuOpts eRes = pwndCur->eShowContextMenu
                        (
                            pntAt, widSrc
                        );

                        // If they want it to pass on the underlying control
                        if (eRes == tCIDCtrls::ECtxMenuOpts::Pass)
                            break;

                        // If they handled it, we are done
                        if (eRes == tCIDCtrls::ECtxMenuOpts::Done)
                            return 0;

                        // Else they want to propagate, so move to the next level if any
                        pwndCur = pwndCur->pwndParent();
                    }
                    break;
                }

                case WM_DESTROY :
                {
                    Destroyed();
                    break;
                }

                case WM_GETDLGCODE :
                {
                    //
                    //  If they have set something specific, return that. Else let it
                    //  go to the control.
                    //
                    if (m_eWantInput != tCIDCtrls::EWantInputs::None)
                        return tCIDCtrls::TMsgResult(m_eWantInput);
                    break;
                }

                case WM_HELP :
                {
                    // We only send this for windows
                    HELPINFO* pInfo = reinterpret_cast<HELPINFO*>(lParam);
                    if (pInfo->iContextType == HELPINFO_WINDOW)
                    {
                        // If not handled, let it propagate upwards
                        TPoint pntAt(pInfo->MousePos.x, pInfo->MousePos.y);
                        if (bShowHelp(pInfo->iCtrlId, pntAt))
                            return 1;
                   }
                   break;
                }

                case WM_KILLFOCUS :
                {
                    // Let the window know it's losing focus
                    LosingFocus();

                    //  Let everyone know up the parent chain
                    TWindow* const pwndPar = pwndParent();
                    TWindow* pwndCur = pwndPar;
                    const tCIDCtrls::TWndId widUs = widThis();
                    while (pwndCur)
                    {
                        if (pwndCur->bWantsChildFocusNot())
                            SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::False);
                        pwndCur = pwndCur->pwndParent();
                    }

                    // Let it go on to the underlying control
                    break;
                }

                case WM_LBUTTONDBLCLK :
                case WM_LBUTTONDOWN :
                case WM_LBUTTONUP :
                case WM_MBUTTONDBLCLK :
                case WM_MBUTTONDOWN :
                case WM_MBUTTONUP :
                case WM_RBUTTONDBLCLK :
                case WM_RBUTTONDOWN :
                case WM_RBUTTONUP :
                {
                    tCIDLib::TBoolean       bCtrlShift;
                    tCIDLib::TBoolean       bShift;
                    tCIDCtrls::EMouseButts  eButton;
                    tCIDCtrls::EMouseClicks eClick;
                    TPoint                  pntAt;
                    XlatButtonMsg
                    (
                        wmsgCur, wParam, lParam, eButton, eClick, pntAt, bCtrlShift, bShift
                    );
                    bClick(eButton, eClick, pntAt, bCtrlShift, bShift);
                    break;
                }

                case WM_KEYDOWN :
                {
                    tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
                    tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
                    tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

                    //
                    //  Use the map the character code to one of our extended key
                    //  codes. If no mapping or there is one and the derived class
                    //  handles it, we just return zero to eat this msg.
                    //
                    const tCIDCtrls::EExtKeys eKey = eMapExtKey(wParam, bShift);
                    if ((eKey == tCIDCtrls::EExtKeys::None) || bExtKey(eKey, bAlt, bCtrl, bShift))
                        return 0;
                    break;
                }

                case WM_HSCROLL :
                case WM_VSCROLL :
                {
                    bNotReflect(*this, wmsgCur, wParam, lParam, mresRet);
                    break;
                }

                case WM_SETFOCUS :
                {
                    // Let the window know it's losing focus
                    GettingFocus();

                    //  Let everyone know up the parent chain
                    TWindow* const pwndPar = pwndParent();
                    TWindow* pwndCur = pwndPar;
                    const tCIDCtrls::TWndId widUs = widThis();
                    while (pwndCur)
                    {
                        if (pwndCur->bWantsChildFocusNot())
                            SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::True);
                        pwndCur = pwndCur->pwndParent();
                    }

                    // Let it go on to the underlying control
                    break;
                }

                case WM_SETREDRAW :
                {
                    // Inc/dec our flag then let it propogate
                    if (!wParam)
                    {
                        m_c4SuppressPaint++;
                    }
                     else
                    {
                        if (m_c4SuppressPaint)
                        {
                            m_c4SuppressPaint--;
                        }
                         else
                        {
                            #if CID_DEBUG_ON
                            facCIDCtrls().LogMsg
                            (
                                CID_FILE
                                , CID_LINE
                                , L"Paint suppression underflow"
                                , tCIDLib::ESeverities::Status
                                , tCIDLib::EErrClasses::Underflow
                            );
                            #endif
                        }
                    }
                    break;
                }

                case WM_WINDOWPOSCHANGED :
                {
                    WINDOWPOS& wPos = *(WINDOWPOS*)lParam;

                    //
                    //  Get our min, max restored state. Any others we just ignore.
                    //
                    WINDOWPLACEMENT WndPlace = { 0 };
                    WndPlace.length = sizeof(WINDOWPLACEMENT);
                    ::GetWindowPlacement(m_hwndThis, &WndPlace);
                    tCIDCtrls::EPosStates eState = tCIDCtrls::EPosStates::Restored;
                    if (WndPlace.showCmd == SW_SHOWMINIMIZED)
                        eState = tCIDCtrls::EPosStates::Minimized;
                    else if (WndPlace.showCmd == SW_SHOWMAXIMIZED)
                        eState = tCIDCtrls::EPosStates::Maximized;
                    else if (WndPlace.showCmd == SW_SHOWNORMAL)
                        eState = tCIDCtrls::EPosStates::Restored;
                    else
                        break;

                    // Get the new area
                    TArea areaNew(wPos.x, wPos.y, wPos.cx, wPos.cy);

                    // And figure out what changed
                    const tCIDLib::TBoolean bOrgChanged
                    (
                        (wPos.x != m_areaPrev.i4X()) || (wPos.y != m_areaPrev.i4Y())
                    );
                    const tCIDLib::TBoolean bSizeChanged
                    (
                        (wPos.cx != m_areaPrev.c4Width())
                        || (wPos.cy != m_areaPrev.c4Height())
                    );

                    const tCIDLib::TBoolean bStateChanged(eState != m_eLastPosState);
                    if (bOrgChanged || bSizeChanged || bStateChanged)
                    {
                        try
                        {
                            AreaChanged
                            (
                                m_areaPrev
                                , areaNew
                                , eState
                                , bOrgChanged
                                , bSizeChanged
                                , bStateChanged
                            );

                            // If not minimized now store the new area as the previous area
                            if (eState != tCIDCtrls::EPosStates::Minimized)
                                m_areaPrev = areaNew;
                            m_eLastPosState = eState;
                        }

                        catch(TError& errToCatch)
                        {
                            if (eState != tCIDCtrls::EPosStates::Minimized)
                                m_areaPrev = areaNew;
                            m_eLastPosState = eState;

                            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                            throw;
                        }
                    }

                    // If restored, tell our parent our area changed
                    if (wParam == SIZE_RESTORED)
                    {
                        TWindow* pwndPar = pwndParent();
                        if (pwndPar)
                            pwndPar->ChildAreaChanged(m_widThis, areaNew);
                    }
                    break;
                }

                default :
                    break;
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        // Call the underlying control procedure and return his return
        return ::CallWindowProc
        (
            (WNDPROC)m_pfOrgProc, wndThis.hwndSafe(), wmsgCur, wParam, lParam
        );
    }


    // Not an overridden standard window, so do the usual thing
    switch(wmsgCur)
    {
        //
        //  Handle our own internal messages
        //
        case kCIDCtrls::wmsgAsyncEvent1 :
        {
            //
            //  This is an async notification that has been sent by some window to
            //  us. We need to see if we have a handler for this and, if so, invoke
            //  it.
            //
            HandleNotification(wndThis, wParam, lParam);
            return 0;
        }

        // An async post of of a code
        case kCIDCtrls::wmsgAsyncEvent2 :
        {
            // Just call the code recieved callback
            CodeReceived(lParam, wParam);
            break;
        }

        case kCIDCtrls::wmsgSyncEvent1 :
        {
            // Just call the data received callback
            DataReceived(tCIDLib::TCard4(lParam), (tCIDLib::TVoid*)wParam);
            break;
        }

        case kCIDCtrls::wmsgChildFocusNot :
        {
            TWindow* pwndPar = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(wParam));

            //
            //  Should always be a valid window, but just in case, and it's possible
            //  it's dead now as well, since this is as async msg.
            //
            if (pwndPar && pwndPar->bIsValid())
            {
                ChildFocusChange
                (
                    *pwndPar, tCIDCtrls::TWndId(lParam & 0x7FFFFFFF), (lParam & 0x80000000) != 0
                );
            }
            break;
        };


        //
        //  Handle system level messages.
        //
        case WM_APPCOMMAND :
        {
            tCIDCtrls::EExtKeys eKey = eXlatAppComamnd(lParam);
            if (eKey != tCIDCtrls::EExtKeys::None)
            {
                tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
                tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
                tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;
                if (bMediaKey(eKey, bAlt, bCtrl, bShift))
                    return 0;
            }
            break;
        }

        case WM_CHAR :
        {
            //
            //  We only report key down events, since it just simplifies things
            //  and there's no need in general for applications to care other than
            //  that a key was pressed.
            //
            if (!(lParam & KF_UP))
            {
                // Ignore extended key code here, do those via WM_KEYDOWN
                if (!(lParam & KF_EXTENDED))
                {
                    tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
                    tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
                    tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

                    tCIDLib::TCh chChar = tCIDLib::TCh(wParam);

                    //
                    //  If control shifted and the char is 1 to 26 then it's a control
                    //  shifted A to Z, so make life easier and translate that.
                    //
                    if (bCtrl && !bAlt && !bShift && (chChar >= 1) && (chChar <= 26))
                        chChar = tCIDLib::TCh(kCIDLib::chLatin_a + (chChar - 1));

                    //
                    //  Filter out some that we only want to show up as extended
                    //  keys but which also get reported here.
                    //
                    if ((chChar != kCIDLib::chBS)
                    &&  (chChar != kCIDLib::chCR)
                    &&  (chChar != kCIDLib::chEsc)
                    &&  (chChar != kCIDLib::chTab))
                    {
                        if (bChar(chChar, bAlt, bCtrl, bShift))
                            return 0;
                    }
                }
            }
             else
            {
                // Just eat the ups
                return 0;
            }
            break;
        }

        case WM_DISPLAYCHANGE :
        {
            // The bit depth or size of the screen has changed
            const TSize szNew
            (
                TRawBits::c2Low16From32(lParam), TRawBits::c2High16From32(lParam)
            );
            DisplayChanged(wParam, szNew);
            break;
        }

        case WM_DRAWITEM :
        {
            //
            //  We can use a basic draw dev here because we aren't adopt it and don't
            //  need to clean it up.
            //
            DRAWITEMSTRUCT* pDraw = (DRAWITEMSTRUCT*)lParam;
            TGraphDrawDev gdevTar(pDraw->hDC, tCIDLib::EAdoptOpts::NoAdopt);

            // Make sure any changes are undone before we go back
            TGraphicDevJanitor janDev(&gdevTar);

            tCIDCtrls::ECustDrFlags eFlags = tCIDCtrls::ECustDrFlags::None;
            if (pDraw->itemState & ODS_DISABLED)
                eFlags |= tCIDCtrls::ECustDrFlags::Disabled;

            if (pDraw->itemState & ODS_SELECTED)
                eFlags |= tCIDCtrls::ECustDrFlags::Selected;

            if (pDraw->itemState & ODS_FOCUS)
                eFlags |= tCIDCtrls::ECustDrFlags::Focus;

            TArea areaTar(*(tCIDLib::THostRectl*)&pDraw->rcItem);
            CustomDraw(gdevTar, pDraw->itemID, eFlags, areaTar);
            return 1;
        };


        case WM_CLOSE :
        {
            ::EndDialog(wndThis.hwndThis(), kCIDCtrls::widCancel);
            return 0;
        }

        case WM_CTLCOLOREDIT :
        case WM_CTLCOLORSTATIC :
        {
            //
            //  See if the sending control is one of ours. If so, see if he has
            //  had his background explicitly set. If so we use his. Else, we
            //  use this window's (the parent of the sender) color.
            //
            TWindow* pwndSrcCtrl = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(lParam));
            TWindow* pwndBgnClr = pwndSrcCtrl;
            if (!pwndBgnClr || !pwndBgnClr->m_bBgnClrOver)
                pwndBgnClr = this;

            // We can use a basic draw dev here since we don't adopt or clean up the handle
            TGraphDrawDev gdevToUse
            (
                tCIDGraphDev::TDeviceHandle(wParam), tCIDLib::EAdoptOpts::NoAdopt
            );
            gdevToUse.SetBgnColor(pwndBgnClr->m_gbrBgn.rgbColor());

            //
            //  We also reflect it back to the control, so he can set a foreground
            //  color if he wants.
            //
            if (pwndSrcCtrl)
                pwndSrcCtrl->bNotReflect(*this, wmsgCur, wParam, lParam, mresRet);

            // Give back the handle to our standard bgn brush
            return tCIDCtrls::TMsgResult(pwndBgnClr->m_gbrBgn.hbrThis());
        }

        case WM_CREATE :
        case WM_INITDIALOG :
        {
            // Call the created method on this window
            try
            {
                //
                //  Initialize our background color brush to the standard colors
                //  we use, for windows and dialogs. These may get overridden later.
                //
                //  DO NOT call SetBgnClr here, since that will set the explicit
                //  override flag. This is just an implicit initial value that can
                //  overridden later.
                //
                if (wmsgCur == WM_CREATE)
                {
                    m_gbrBgn.SetColor
                    (
                        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window)
                    );
                    mresRet = 0;
                }
                 else
                {
                    m_gbrBgn.SetColor
                    (
                        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DialogBgn)
                    );
                }

                // Set an initial previous area
                m_areaPrev = areaWnd();

                // And now let the derived class handle creation
                if (bCreated())
                {
                    // For a dialog this means it set the focus, so we shouldn't
                    if (wmsgCur == WM_INITDIALOG)
                        mresRet = FALSE;
                }
                 else
                {
                    // For a dialog, do default focus. For a window, failure
                    if (wmsgCur == WM_INITDIALOG)
                        mresRet = TRUE;
                    else
                        mresRet = -1;
                }
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }
            return mresRet;
        }


        case WM_COMMAND :
        {
            //
            //  If from a control we reflect it back to the sender, who can then send out
            //  our own type of notifications, unless asked not to. If reflecting, get the
            //  pointer to the source window and call him, passing us along as the ultimate
            //  target. If not reflecting, just send it to us.
            //
            //  If it's an accelerator, we map it to a menu command if possible, so that we
            //  can pass along some info about the item.
            //
            //  Else we send it like a menu command but with the checked/enabled flags just
            //  set to false.
            //
            if (lParam != 0)
            {
                // It's a control sending it
                TWindow* pwndSrc = nullptr;
                if (m_bNoReflect)
                    pwndSrc = this;
                else
                    pwndSrc = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(lParam));
                if (pwndSrc)
                {
                    tCIDCtrls::TMsgResult mresRet = 0;
                    if (pwndSrc->bNotReflect(*this, wmsgCur, wParam, lParam, mresRet))
                        return mresRet;
                }
            }
             else if ((wParam >> 16) == 1)
            {
                tCIDLib::TBoolean bChecked = kCIDLib::False;
                tCIDLib::TBoolean bEnabled = kCIDLib::False;

                // Get the menu handle and see if has such a menu item
                tCIDCtrls::TMenuHandle hmenuBar = ::GetMenu(m_hwndThis);
                if (hmenuBar != kCIDCtrls::hmenuInvalid)
                {
                    MENUITEMINFO Info = { 0 };
                    Info.cbSize = sizeof(MENUITEMINFO);
                    Info.fMask = MIIM_STATE;

                    if (::GetMenuItemInfo(hmenuBar, wParam & 0xFFFF, kCIDLib::False, &Info))
                    {
                        bChecked = (Info.fState & MFS_CHECKED) != 0;
                        bEnabled = (Info.fState & MFS_ENABLED) != 0;
                    }
                }

                MenuCommand(wParam & 0xFFFF, bChecked, bEnabled);
                return 0;
            }
            break;
        }

        case WM_CONTEXTMENU :
        {
            //
            //  Give the class a chance to do a context menu. THe point could be
            //  -1, -1 if done by keyboard, in which case the derived class has
            //  to figure out where to show it. Actually it's 16 bit values, so we
            //  have to check for that specially.
            //
            TPoint pntAt
            (
                TRawBits::c2Low16From32(lParam), TRawBits::c2High16From32(lParam)
            );

            if ((pntAt.i4X() == 65535) && (pntAt.i4Y() == 65535))
                pntAt.Set(-1, -1);

            tCIDCtrls::TWndId widSrc = kCIDLib::c4MaxCard;
            tCIDCtrls::TWndHandle hwndSrc = (tCIDCtrls::TWndHandle)wParam;
            if (hwndSrc != kCIDCtrls::hwndInvalid)
                widSrc = ::GetWindowLong(hwndSrc, GWL_ID);

            //
            //  We have to handle propagation ourself, otherwise the original window
            //  id is is lost if it goes through more than one containing window. We
            //  start off reflecting it back to the original window.
            //
            TWindow* pwndCur = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(hwndSrc));
            while (pwndCur)
            {
                const tCIDCtrls::ECtxMenuOpts eRes = pwndCur->eShowContextMenu
                (
                    pntAt, widSrc
                );

                // This has no meaning for custom controls, so just break out
                if (eRes == tCIDCtrls::ECtxMenuOpts::Pass)
                    break;

                // If they handled it, we are done
                if (eRes == tCIDCtrls::ECtxMenuOpts::Done)
                    return 0;

                // Else they want to propagate, so move to the next level if any
                pwndCur = pwndCur->pwndParent();
            }
            break;
        }

        case WM_DESTROY :
        {
            Destroyed();
            return 0;
        }

        // -------------------------------------------------------------------
        // There's beena change in attached devices
        // -------------------------------------------------------------------
        case WM_DEVICECHANGE :
        {
            // Inform of media coming or going, if the window is visible
            if (bIsVisible()
            &&  ((wParam == DBT_DEVICEARRIVAL)
            ||   (wParam == DBT_DEVICEREMOVECOMPLETE)
            ||   (wParam == DBT_DEVICEREMOVEPENDING)))
            {
                DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)lParam;

                //
                //  We only care about volume events and that it was an
                //  event that affected the physical drive, not a change
                //  in media.
                //
                if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
                {
                    // We know we can look at it as a volume structure
                    DEV_BROADCAST_VOLUME* pVol = (DEV_BROADCAST_VOLUME*)lParam;

                    // Figure out what type of event we are sending
                    tCIDCtrls::EMediaChTypes eType = tCIDCtrls::EMediaChTypes::Count;
                    if (wParam == DBT_DEVICEARRIVAL)
                        eType = tCIDCtrls::EMediaChTypes::Arrived;
                    else if (wParam == DBT_DEVICEREMOVECOMPLETE)
                        eType = tCIDCtrls::EMediaChTypes::RemoveComplete;
                    else if (wParam == DBT_DEVICEREMOVEPENDING)
                        eType = tCIDCtrls::EMediaChTypes::RemovePending;

                    if (eType != tCIDCtrls::EMediaChTypes::Count)
                        MediaChange(eType);
                }
            }
            break;
        }

        case WM_DROPFILES :
        {
            //
            //  If we have the accept files style, then process this. Else we just
            //  let it propogate.
            //
            if (tCIDLib::bAllBitsOn(eExWndStyles(), tCIDCtrls::EExWndStyles::AcceptFiles))
            {
                if (bHandleFileDrop(wParam, lParam))
                    return 0;
            }
            break;
        }

        case WM_ENABLE :
        {
            // The enabled state of this window has changed, so let it know
            EnableStateChanged();
            break;
        }

        case WM_ENTERSIZEMOVE :
        {
            m_bDragMode = kCIDLib::True;
            DragStart();
            return 0;
        }

        case WM_EXITSIZEMOVE :
        {
            m_bDragMode = kCIDLib::False;
            DragEnd();
            return 0;
        }

        case WM_ERASEBKGND :
        {
            // We can use a basic draw dev here since we don't adopt or clean up the handle
            TGraphDrawDev gdevToUse
            (
                tCIDGraphDev::TDeviceHandle(wParam), tCIDLib::EAdoptOpts::NoAdopt
            );

            if (bEraseBgn(gdevToUse))
                return 1;
            break;
        }

        case WM_GETDLGCODE :
        {
            //
            //  If they have set something specific, return that. Else we just
            //  let it to go the controll.
            //
            if (m_eWantInput != tCIDCtrls::EWantInputs::None)
                return tCIDCtrls::TMsgResult(m_eWantInput);
            break;
        }

        case WM_HSCROLL :
        case WM_VSCROLL :
        {
            //
            //  If from a scroll bar, slider, or a track bar, then we either reflect it back
            //  to the sender or send it to this window if asked to, who can then send out
            //  its own type of notifications.
            //
            //  Otherwise it's from the embedded scroll bars and always goes to the actual
            //  window itself, and we send those though a virtual method call.
            //
            const tCIDLib::TBoolean bHorz(wmsgCur == WM_HSCROLL);
            if (lParam)
            {
                TWindow* pwndSrc = nullptr;
                if (m_bNoReflect)
                    pwndSrc = this;
                else
                    pwndSrc = pwndGetWndLinkPtr(tCIDCtrls::TWndHandle(lParam));
                if (pwndSrc)
                {
                    tCIDCtrls::TMsgResult mresRet = 0;
                    pwndSrc->bNotReflect(*this, wmsgCur, wParam, lParam, mresRet);
                }
            }
             else
            {
                //
                //  Depending on the type of event, we call the appropriate callback
                //  method.
                //
                tCIDLib::TCard4 c4Pos;
                tCIDLib::TCard2 c2Event(wParam & 0xFFFF);

                if ((c2Event == SB_THUMBPOSITION) || (c2Event == SB_THUMBTRACK))
                {
                    c4Pos = tCIDLib::TCard4(wParam >> 16);
                    ScrollDrag(bHorz, c4Pos, (c2Event == SB_THUMBPOSITION));
                }
                 else
                {
                    tCIDCtrls::EScrollEvs eEv = tCIDCtrls::EScrollEvs::None;
                    if (c2Event == SB_LEFT)
                        eEv = tCIDCtrls::EScrollEvs::Home;
                    else if (c2Event == SB_RIGHT)
                        eEv = tCIDCtrls::EScrollEvs::End;
                    else if (c2Event == SB_LINELEFT)
                        eEv = tCIDCtrls::EScrollEvs::PrevLn;
                    else if (c2Event == SB_LINERIGHT)
                        eEv = tCIDCtrls::EScrollEvs::NextLn;
                    else if (c2Event == SB_PAGELEFT)
                        eEv = tCIDCtrls::EScrollEvs::PrevPg;
                    else if (c2Event == SB_PAGERIGHT)
                        eEv = tCIDCtrls::EScrollEvs::NextPg;

                    if (eEv != tCIDCtrls::EScrollEvs::None)
                        ScrollEvent(eEv, bHorz);
                }
            }
            return 0;
        }

        case WM_HELP :
        {
            // We only send this for windows
            HELPINFO* pInfo = reinterpret_cast<HELPINFO*>(lParam);
            if (pInfo->iContextType == HELPINFO_WINDOW)
            {
                // If not handled, let it propagate upwards
                TPoint pntAt(pInfo->MousePos.x, pInfo->MousePos.y);
                if (bShowHelp(pInfo->iCtrlId, pntAt))
                    return 1;
           }
           break;
        }

        case WM_KEYDOWN :
        {
            tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
            tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
            tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

            //
            //  Use the map the character code to one of our extended key codes. If
            //  no mapping, or ther eis one and the derived class handles it, then
            //  eat this msg.
            //
            const tCIDCtrls::EExtKeys eKey = eMapExtKey(wParam, bShift);
            if ((eKey == tCIDCtrls::EExtKeys::None) || bExtKey(eKey, bAlt, bCtrl, bShift))
                return 0;
            break;
        }

        case WM_KILLFOCUS :
        {
            // Let the window know it's losing focus
            LosingFocus();

            //  Let everyone know up the parent chain
            TWindow* const pwndPar = pwndParent();
            TWindow* pwndCur = pwndPar;
            const tCIDCtrls::TWndId widUs = widThis();
            while (pwndCur)
            {
                if (pwndCur->bWantsChildFocusNot())
                    SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::False);
                pwndCur = pwndCur->pwndParent();
            }

            return 0;
        }

        case WM_LBUTTONDBLCLK :
        case WM_LBUTTONDOWN :
        case WM_LBUTTONUP :
        case WM_MBUTTONDBLCLK :
        case WM_MBUTTONDOWN :
        case WM_MBUTTONUP :
        case WM_RBUTTONDBLCLK :
        case WM_RBUTTONDOWN :
        case WM_RBUTTONUP :
        {
            tCIDLib::TBoolean       bCtrlShift;
            tCIDLib::TBoolean       bShift;
            tCIDCtrls::EMouseButts  eButton;
            tCIDCtrls::EMouseClicks eClick;
            TPoint                  pntAt;
            XlatButtonMsg
            (
                wmsgCur, wParam, lParam, eButton, eClick, pntAt, bCtrlShift, bShift
            );
            if (bClick(eButton, eClick, pntAt, bCtrlShift, bShift))
                return 0;
            break;
        }

        case WM_MEASUREITEM :
        {
            MEASUREITEMSTRUCT* pStruct = (MEASUREITEMSTRUCT*)lParam;
            tCIDLib::TCard4 c4Height;
            MeasureItem(pStruct->CtlID, pStruct->itemID, c4Height);
            pStruct->itemHeight = c4Height;
            return 1;
        }

        case WM_MENUCOMMAND :
        {
            // Get the handle of the menu
            tCIDCtrls::TMenuHandle hmenu = tCIDCtrls::TMenuHandle(lParam);

            // Get the info for the selected item
            MENUITEMINFO Info = {0};
            Info.cbSize = sizeof(Info);
            Info.fMask = MIIM_ID | MIIM_STATE;

            if (::GetMenuItemInfo(hmenu, wParam, 1, &Info))
            {
                MenuCommand
                (
                    Info.wID, (Info.fState & MFS_CHECKED) != 0, (Info.fState & MFS_ENABLED) != 0
                );
            }
             else
            {
                TString strErr = L"Could not get information on selected menu item. Error=";
                strErr.AppendFormatted(::GetLastError());

                TErrBox msgbErr(L"Menu Command Error", strErr);
                msgbErr.ShowIt(wndThis);
            }
            return 0;
        }

        case WM_MOUSEACTIVATE :
        {
            // We can return this value as is, cast to the result type
            return tCIDCtrls::TMsgResult(eMouseActivate());
        }

        case WM_MOUSELEAVE :
        {
            //TrackExit();
            return 0;
        };

        case WM_MOUSEMOVE :
        {
            // Get the point information out to pass on
            TPoint pntPos
            (
                TRawBits::i2Low16From32(lParam), TRawBits::i2High16From32(lParam)
            );

            const tCIDLib::TBoolean bCtrlShift = (wParam & MK_CONTROL) != 0;
            const tCIDLib::TBoolean bShift = (wParam & MK_SHIFT) != 0;
            if (!bMouseMove(pntPos, bCtrlShift, bShift))
                return 0;

            break;
        }

        case WM_MOUSEWHEEL :
        {
            tCIDLib::TInt4 i4Clicks = TRawBits::i2High16From32(wParam) / 120;
            if (i4Clicks && bMouseWheel(i4Clicks))
                return 0;
            break;
        }

        case WM_NOTIFY :
        {
            //
            //  These we either reflect back to the sender, or send to this window if asked
            //  not to reflect, who can then send out his own type of (much tighter)
            //  notifications.
            //
            NMHDR* pHdr = (NMHDR*)lParam;
            if (pHdr && pHdr->hwndFrom)
            {
                TWindow* pwndSrc = nullptr;
                if (m_bNoReflect)
                    pwndSrc = this;
                else
                    pwndSrc = pwndGetWndLinkPtr(pHdr->hwndFrom);
                if (pwndSrc)
                {
                    tCIDCtrls::TMsgResult mresRet = 0;
                    if (pwndSrc->bNotReflect(*this, wmsgCur, wParam, lParam, mresRet))
                    {
                        if (m_bIsDialog)
                        {
                            ::SetWindowLong(hwndThis(), DWL_MSGRESULT, mresRet);
                            return TRUE;
                        }
                        return mresRet;
                    }
                }
            }
            break;
        }

        case WM_PAINT :
        {
            // Get the info we need
            TArea areaUpdate;
            tCIDGraphDev::TDeviceHandle hdevPaint;

            PAINTSTRUCT PaintStruct;
            hdevPaint = ::BeginPaint(m_hwndThis, &PaintStruct);
            if (!hdevPaint)
            {
                TKrnlError::SetLastHostError(::GetLastError());
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcWnd_BeginPaint
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::ProcFatal
                    , tCIDLib::EErrClasses::Internal
                );
            }

            // Convert the update rect to an area structure
            areaUpdate.FromRectl(*(tCIDLib::THostRectl*)&PaintStruct.rcPaint);

            // Create a device object for the device
            TGraphPaintDev gdevPaint(hdevPaint, PaintStruct);

            //
            //  For our custom windows, we just don't call paint if the suppress
            //  counter is non-zero. This is simple and works well enough.
            //
            if (m_c4SuppressPaint || bPaint(gdevPaint, areaUpdate))
                return 0;
            break;
        }


        // -------------------------------------------------------------------
        // We report the basic powering down/powering up changes
        // -------------------------------------------------------------------
        case WM_POWERBROADCAST :
        {
            if ((wParam == PBT_APMRESUMEAUTOMATIC)
            ||  (wParam == PBT_APMSUSPEND)
            ||  (wParam == PBT_APMRESUMECRITICAL))
            {
                // We pass true if resuming, else it's going to sleep
                PowerStateChange
                (
                    (wParam == PBT_APMRESUMEAUTOMATIC) || (wParam == PBT_APMRESUMECRITICAL)
                );
            }
            break;
        }


        case WM_SETCURSOR :
        {
            // Get the window area being reported
            const tCIDCtrls::EWndAreas eArea = tCIDCtrls::EWndAreas
            (
                TRawBits::c2Low16From32(lParam)
            );

            //
            //  If it's in our window, call one method, else it's for a child,
            //  so call another.
            //
            tCIDCtrls::TWndHandle hwndIn = tCIDCtrls::TWndHandle(wParam);
            if (m_hwndThis == hwndIn)
            {
                if (bSetPointer(eArea))
                    return 1;
            }
             else
            {
                TWindow* pwndChild = pwndGetWndLinkPtr(hwndIn);
                if (pwndChild)
                {
                    if (bSetChildPointer(eArea, *pwndChild))
                        return 1;
                }
            }

            return 0;
        }
        break;

        case WM_SETFOCUS :
        {
            // Let the window know it's getting focus
            GettingFocus();

            //  Let everyone know up the parent chain
            TWindow* const pwndPar = pwndParent();
            TWindow* pwndCur = pwndPar;
            const tCIDCtrls::TWndId widUs = widThis();
            while (pwndCur)
            {
                if (pwndCur->bWantsChildFocusNot())
                    SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::True);
                pwndCur = pwndCur->pwndParent();
            }
            return 0;
        }

        case WM_SETREDRAW :
        {
            // Inc/dec our flag then let it propogate
            if (!wParam)
            {
                m_c4SuppressPaint++;
            }
             else
            {
                if (m_c4SuppressPaint)
                {
                    m_c4SuppressPaint--;
                }
                 else
                {
                    #if CID_DEBUG_ON
                    facCIDCtrls().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , L"Paint suppression underflow"
                        , tCIDLib::ESeverities::Status
                        , tCIDLib::EErrClasses::Underflow
                    );
                    #endif
                }
            }
            break;
        }

        case WM_SHOWWINDOW :
        {
            if (bVisibilityChange(wParam != 0))
                return 0;
            break;
        }

        case WM_TIMER :
        {
            //
            //  If it's potentially one of our timers, and it's been started, and
            //  it's not paused, and it's not already been entered, and the overall
            //  timer suspend isn't set, then dispatch it.
            //
            if (((wParam >= 1) && (wParam < kCIDCtrls::c4MaxWndTimers))
            &&  !m_c4TimerSuspend
            &&  !m_aTimers[wParam - 1].bPaused
            &&  !m_aTimers[wParam - 1].bEntered)
            {
                // Set it entered while we are processing it
                TBoolJanitor janEntered
                (
                    &m_aTimers[wParam - 1].bEntered, kCIDLib::True
                );
                Timer(wParam);
            }
            break;
        }


        case WM_WINDOWPOSCHANGED :
        {
            WINDOWPOS& wPos = *(WINDOWPOS*)lParam;

            //
            //  Get our min, max restored state, since typically windows only want
            //  to do anything if restored.
            //
            WINDOWPLACEMENT WndPlace = {0};
            WndPlace.length = sizeof(WINDOWPLACEMENT);
            ::GetWindowPlacement(m_hwndThis, &WndPlace);
            tCIDCtrls::EPosStates eState = tCIDCtrls::EPosStates::Restored;
            if (WndPlace.showCmd == SW_SHOWMINIMIZED)
                eState = tCIDCtrls::EPosStates::Minimized;
            else if (WndPlace.showCmd == SW_SHOWMAXIMIZED)
                 eState = tCIDCtrls::EPosStates::Maximized;
            else if (WndPlace.showCmd == SW_SHOWNORMAL)
                 eState = tCIDCtrls::EPosStates::Restored;
            else
                break;

            // Get the new area
            TArea areaNew(wPos.x, wPos.y, wPos.cx, wPos.cy);

            // And figure out what changed
            const tCIDLib::TBoolean bOrgChanged
            (
                (wPos.x != m_areaPrev.i4X()) || (wPos.y != m_areaPrev.i4Y())
            );
            const tCIDLib::TBoolean bSizeChanged
            (
                (wPos.cx != m_areaPrev.c4Width()) || (wPos.cy != m_areaPrev.c4Height())
            );

            const tCIDLib::TBoolean bStateChanged(eState != m_eLastPosState);
            if (bOrgChanged || bSizeChanged || bStateChanged)
            {
                try
                {
                    AreaChanged
                    (
                        m_areaPrev
                        , areaNew
                        , eState
                        , bOrgChanged
                        , bSizeChanged
                        , bStateChanged
                    );

                    if (eState != tCIDCtrls::EPosStates::Minimized)
                        m_areaPrev = areaNew;
                    m_eLastPosState = eState;
                }

                catch(TError& errToCatch)
                {
                    if (eState != tCIDCtrls::EPosStates::Minimized)
                        m_areaPrev = areaNew;
                    m_eLastPosState = eState;

                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    throw;
                }
            }

            // If restored, tell our parent our area changed
            if (eState == tCIDCtrls::EPosStates::Restored)
            {
                TWindow* pwndPar = pwndParent();
                if (pwndPar)
                    pwndPar->ChildAreaChanged(m_widThis, areaNew);
            }

            //
            //  Let it be processed as well and send out move/size msgs in
            //  case they are required by underlying functionality. It definitely
            //  will affect initial frame window display if we eat it.
            //
            break;
        }

        default :
            //
            //  If it's anything in the user defined range, pass it to the generic
            //  user message handler, else just let it to go to the default handler.
            //
            if ((wmsgCur >= kCIDCtrls::wmsgFirstUser)
            &&  (wmsgCur <= kCIDCtrls::wmsgLastUser))
            {
                UserMsg(wParam, lParam);
                return 0;
            }
            break;
    };

    // Call the default handler, since it wasn't eaten above
    if (!m_bIsDialog)
    {
        const tCIDCtrls::TWndHandle hwndThis = wndThis.hwndThis();
        if (m_bDlgProc)
            return ::DefDlgProc(hwndThis, wmsgCur, wParam, lParam);
        return ::DefWindowProc(hwndThis, wmsgCur, wParam, lParam);
    }
    return 0;
}


// If not overridden, return some defaults
tCIDLib::TVoid
TWindow::MeasureItem(const  tCIDCtrls::TWndId   widCtrl
                    , const tCIDLib::TCard4     c4ItemId
                    ,       tCIDLib::TCard4&    c4Height)
{
    c4Height = 12;
}


tCIDLib::TVoid TWindow::MediaChange(const tCIDCtrls::EMediaChTypes)
{
    // Empty default implementation
}


// We've received a command from a menu, either menu bar or popup.
tCIDLib::TVoid
TWindow::MenuCommand(const tCIDLib::TResId, const tCIDLib::TBoolean, const tCIDLib::TBoolean)
{
    // If not overridden, we do nothing
}


// Do nothing if not overridden
tCIDLib::TVoid TWindow::ModDlgItem(TDlgItem&)
{
}


// The mouse has left our window
tCIDLib::TVoid TWindow::MouseExit()
{
    // Do nothing if not overridden
}


// The system power state changed. Do nothing by default
tCIDLib::TVoid TWindow::PowerStateChange(const tCIDLib::TBoolean)
{
}


// Called upon WM_HSCROLL or WM_VSCROLL events
tCIDLib::TVoid
TWindow::ScrollEvent(const tCIDCtrls::EScrollEvs, const tCIDLib::TBoolean)
{
    // Default is to do nothing
}

tCIDLib::TVoid
TWindow::ScrollDrag(const   tCIDLib::TBoolean
                    , const tCIDLib::TCard4
                    , const tCIDLib::TBoolean)
{
    // Default is to do nothing
}


//
//  A static child control was clicked. This lets the parent respond to such clicks.
//
tCIDLib::TVoid
TWindow::StaticChildClick(  const   tCIDCtrls::TWndId
                            , const tCIDCtrls::EMouseButts
                            , const tCIDCtrls::EMouseClicks)
{
}


//
//  Shouldn't happen unless they create a timer, but we can't make it pure, so
//  we have to have a default.
//
tCIDLib::TVoid TWindow::Timer(const tCIDCtrls::TTimerId)
{
}


tCIDLib::TVoid TWindow::TrackingAreaChanged(const TArea&)
{
}


//
//  If not overridden, do nothing. SInce they should handle any message they send, if
//  debugging, we do a popup.
//
tCIDLib::TVoid
TWindow::UserMsg(const tCIDCtrls::TWParam, const tCIDCtrls::TLParam)
{
    #if CID_DEBUG_ON
    TErrBox msgbErr(L"Internal msg error", L"An unhandled user message was received");
    msgbErr.ShowIt(*this);
    #endif
}


//
//  Unless overridde, we assume that our m_strWndText member is used and therefore
//  there is nothing to do.
//
tCIDLib::TVoid TWindow::WndTextPreQuery(TString&) const
{
}


//
//  Unless overridden, we assume that our m_strWndText member is used and therefore
//  there is nothing to do.
//
tCIDLib::TVoid TWindow::WndTextStore(const TString&)
{
}


// ---------------------------------------------------------------------------
// TWindow: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Sees if this window has any notification handlers for the indicated id
tCIDLib::TBoolean
TWindow::bHasRequestFor(const TNotificationId& nidToTest) const
{
    TNotRequestBase* pnotrCur = m_pnotrHead;
    while (pnotrCur)
    {
        if (pnotrCur->m_nidRequested == nidToTest)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Do a rubber band type marquee select starting at the indicated point and
//  confined to the indicated area. If we return True then areaTrack holds the
//  area selected.
//
//  We have another version below that is used when using a composited window
//  scheme. In such a case we cannot just draw into the window since only drawing
//  in WM_PAINT will get picked up.
//
//  SO, we grab the current contents of the window to a bitmap, set up a memory
//  device over that, and we do the rubber banding within that, and invalidate
//  window after each change. WE override the WM_PAINT and just blit our captured
//  bitmap to the window.
//
tCIDLib::TBoolean
TWindow::bMarqueeSelect(const   TPoint& pntStart
                        ,       TArea&  areaTrack
                        , const TArea&  areaConfine)
{
    // The start point must be within the confine area
    if (!areaConfine.bContainsPoint(pntStart))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_BadTrackPnt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , pntStart
            , areaConfine
        );
    }

    //
    //  See if the button has already been released. This can happen sometimes
    //  and leaves us tracking but with the button up.
    //
    if (!facCIDCtrls().bMouseButtonDown(tCIDCtrls::EMouseButts::Left))
        return kCIDLib::False;

    // Set the initial tracking area
    areaTrack.Set(pntStart, TSize(0,0));

    // Use a flag janitor to set the tracking flag true while in here
    TBoolJanitor janTrack(&m_bAreaTracking, kCIDLib::True);

    //
    //  Get a device contect we'll use during this and set up the pen and
    //  XOR mode that we'll use to draw the rubber band.
    //
    //  Note that we start with a 0 sized area, so we don't do an initial
    //  draw of the rubber band.
    //
    TGraphWndDev gdevTrack(*this);
    TCosmeticPen gpenTrack(facCIDGraphDev().rgbPaleGrey, tCIDGraphDev::ELineStyles::Dot);
    TPenJanitor  janPen(&gdevTrack, &gpenTrack);
    gdevTrack.eMixMode(tCIDGraphDev::EMixModes::XorPen);

    //
    //  To drastically simplify this common operation, we do it modally,
    //  within our own loop, and grabbing messages right off the message
    //  queue.
    //
    tCIDLib::TBoolean bRet = kCIDLib::False;
    MSG msgCur;
    try
    {
        // Translate the confinement coordinates to screen coordinates and set it
        TArea areaScreenCoord;
        ToScreenCoordinates(areaConfine, areaScreenCoord);
        facCIDCtrls().ConfinePointer(areaScreenCoord);

        // Capture the mouse
        CaptureMouse();

        // And enter the loop
        while (::GetMessage(&msgCur, 0, 0, 0))
        {
            //
            //  Watch for a loss of focus or loss of capture or an escape
            //  key.  If we get either of them, we bail out, don't update the
            //  caller's area, and return false.
            //
            //  And watch for a left button up, that indicates the end of
            //  the tracking operation. We update the caller's area and return
            //  true.
            //
            //  And we watch for the mouse move message, in which case we update
            //  the pointer to show the correct shape for the tracking operation
            //  and update our rubber band.
            //
            if ((msgCur.message >= WM_KEYFIRST)
            &&  (msgCur.message <= WM_KEYLAST))
            {
                if ((msgCur.message == WM_KEYDOWN) && (msgCur.wParam == VK_ESCAPE))
                    break;

                // Not an escape, so eat it
                continue;
            }
             else if (msgCur.message == WM_LBUTTONUP)
            {
                bRet = kCIDLib::True;
                break;
            }
             else if (msgCur.message == WM_MOUSEMOVE)
            {
                // Get the of the mouse
                const TPoint pntPos
                (
                    TRawBits::i2Low16From32(msgCur.lParam)
                    , TRawBits::i2High16From32(msgCur.lParam)
                );

                //
                //  Turn off the previous one and turn on the new one, if either
                //  of the are non-zero.
                //
                //  If they've gone above the start point in either direction,
                //  we have to change how we assign points to the area.
                //
                if (!areaTrack.bIsEmpty())
                    gdevTrack.Stroke(areaTrack);

                if (pntPos.i4X() <= pntStart.i4X())
                {
                    areaTrack.i4X(pntPos.i4X());
                    areaTrack.i4Right(pntStart.i4X());
                }
                 else
                {
                    areaTrack.i4X(pntStart.i4X());
                    areaTrack.i4Right(pntPos.i4X());
                }

                if (pntPos.i4Y() <= pntStart.i4Y())
                {
                    areaTrack.i4Y(pntPos.i4Y());
                    areaTrack.i4Bottom(pntStart.i4Y());
                }
                 else
                {
                    areaTrack.i4Y(pntStart.i4Y());
                    areaTrack.i4Bottom(pntPos.i4Y());
                }

                if (!areaTrack.bIsEmpty())
                    gdevTrack.Stroke(areaTrack);

                // Update the pointer
                ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
            }
             else
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }
        }

        // Get rid of the tracking area
        if (!areaTrack.bIsEmpty())
            gdevTrack.Stroke(areaTrack);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Get rid of the tracking area
        if (!areaTrack.bIsEmpty())
            gdevTrack.Stroke(areaTrack);

        // Release the mouse capture
        ReleaseMouse();

        // Undo the cursor containment
        facCIDCtrls().ReleasePointer();
        throw;
    }
    return bRet;
}


//
//  A special method for the TDlgBox class, to create a dialog box. We set up an
//  empty dialog box template, with the appropriate styles and size info, and no
//  child controls.
//
//  We invoke the dialog and point it at our standard dispatcher. The derived class
//  will react to Created() and create any child controls.
//
tCIDLib::TCard4
TWindow::c4CreateDlgBox(const   TWindow&    wndOwner
                        , const TDlgDesc&   dlgdCreate)
{
    //
    //  Create an empty Windows dialog structure, that has no controls defined, it just
    //  create the dialog box the right size. We need some extra words plus storage
    //  for the title text, following the dialog template itself.
    //
    const tCIDLib::TCard4 c4TitleLen = dlgdCreate.strTitle().c4Length();
    const tCIDLib::TCard4 c4BufSz
    (
        256 + (c4TitleLen + 1) * kCIDLib::c4CharBytes
    );

    THeapBuf mbufData(c4BufSz, c4BufSz);
    mbufData.Set(0);
    tCIDLib::TCard4 c4Ofs = 0;

    // dlgVer
    mbufData.PutCard2(1, c4Ofs);
    c4Ofs += 2;

    // Signature
    mbufData.PutCard2(0xFFFF, c4Ofs);
    c4Ofs += 2;

    // Help id
    mbufData.PutCard4(0, c4Ofs);
    c4Ofs += 4;

    // Ex style
    mbufData.PutCard4(0, c4Ofs);
    c4Ofs += 4;

    // Styles
    tCIDLib::TCard4 c4Styles =
    (
        DS_MODALFRAME
        | WS_CAPTION
        | WS_SYSMENU
        | WS_VISIBLE
        | WS_CLIPCHILDREN
    );

    if (tCIDLib::bAllBitsOn(dlgdCreate.eFlags(), tCIDCtrls::EDlgFlags::Sizeable))
        c4Styles |= WS_THICKFRAME;

    // May want the origin to be screen relative
    if (tCIDLib::bAllBitsOn(dlgdCreate.eFlags(), tCIDCtrls::EDlgFlags::ScreenOrigin))
        c4Styles |= DS_ABSALIGN;

    // Push it forward
    if (tCIDLib::bAllBitsOn(dlgdCreate.eFlags(), tCIDCtrls::EDlgFlags::SetFgn))
        c4Styles |= DS_SETFOREGROUND;

    mbufData.PutCard4(c4Styles, c4Ofs);
    c4Ofs += 4;

    // Dialog items
    mbufData.PutCard2(0, c4Ofs);
    c4Ofs += 2;

    //
    //  Adjust the window units in the dialog description to dialog units and store
    //  them. Correct it for centering if needed first.
    //
    TArea areaDlg = dlgdCreate.areaPos();

    // If not using the origin, then center it in the monitor
    if(!tCIDLib::bAllBitsOn(dlgdCreate.eFlags(), tCIDCtrls::EDlgFlags::UseOrigin))
    {
        TArea areaMon;
        facCIDCtrls().QueryMonArea(wndOwner, areaMon, kCIDLib::True);
        areaDlg.CenterIn(areaMon);
    }

    TGUIFacility::AdjustWndUnits(areaDlg);
    mbufData.PutCard2(tCIDLib::TCard2(areaDlg.i4X()), c4Ofs);
    c4Ofs += 2;
    mbufData.PutCard2(tCIDLib::TCard2(areaDlg.i4Y()), c4Ofs);
    c4Ofs += 2;
    mbufData.PutCard2(tCIDLib::TCard2(areaDlg.c4Width()), c4Ofs);
    c4Ofs += 2;
    mbufData.PutCard2(tCIDLib::TCard2(areaDlg.c4Height()), c4Ofs);
    c4Ofs += 2;

    // No menu
    mbufData.PutCard2(0, c4Ofs);
    c4Ofs += 2;

    // Default window class
    mbufData.PutCard2(0, c4Ofs);
    c4Ofs += 2;

    // Move up again and put in the title text if any
    if (c4TitleLen)
    {
        // Copy in the text plus null
        const tCIDLib::TCard4 c4TextBytes = (c4TitleLen + 1) * kCIDLib::c4CharBytes;
        mbufData.CopyIn
        (
            dlgdCreate.strTitle().pszBuffer(), c4TextBytes, c4Ofs
        );
        c4Ofs += c4TextBytes;
    }
     else
    {
        // No title
        mbufData.PutCard2(0, c4Ofs);
        c4Ofs += 2;
    }

#if 0
    //
    //  Put in font stuff (point, weight, italic, charsset, typeface. The default
    //  font info comes from the GUI facility class.
    //
    const tCIDLib::TInt2 i2Height = tCIDLib::TInt2(TGUIFacility::c4DefFontHeight()) * -1;
    mbufData.PutInt2(i2Height, c4Ofs);
    c4Ofs += 2;
    mbufData.PutCard2(FW_LIGHT, c4Ofs);
    c4Ofs += 2;
    mbufData.PutCard1(0, c4Ofs++);
    mbufData.PutCard1(DEFAULT_CHARSET, c4Ofs++);

    const TString& strFace = TGUIFacility::strDefFontFace();
    mbufData.CopyIn
    (
        strFace.pszBuffer()
        , (strFace.c4Length() + 1) * kCIDLib::c4CharBytes
        , c4Ofs
    );
    c4Ofs += (strFace.c4Length() + 1) * kCIDLib::c4CharBytes;
#endif


    INT_PTR iRes = ::DialogBoxIndirectParam
    (
        *(HINSTANCE*)&facCIDCtrls().kmodThis().hmodThis().hmodiThis()
        , (LPCDLGTEMPLATE)mbufData.pc1Data()
        , wndOwner.hwndSafe()
        , (DLGPROC)mresCIDCtrlsDispatch
        , tCIDCtrls::TLParam(this)
    );

    // If less than zero, then it failed to load
    if (iRes < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcDlg_LoadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
        );
    }

    // Return the value returned from the dialog proc
    return tCIDLib::TCard4(iRes);
}


// Check that the window handle is valid, else throw
tCIDLib::TVoid TWindow::CheckWindowHandle() const
{
    if (!::IsWindow(m_hwndThis))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_NotValidWindow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}


//
//  Our standard dispatcher above calls us here when we are destroyed. We just
//  need to reset any data to indicate we no longer are an active window. This
//  will be the last thing called until/unless this window object is created
//  again.
//
//  Note we don't reset the id. It will be used again if this window is recreated.
//
tCIDLib::TVoid TWindow::Cleanup()
{
    m_hwndThis = kCIDCtrls::hwndInvalid;
}


// Create a caret of the indicated size
tCIDLib::TVoid TWindow::CreateCaret(const TSize& szInit)
{
    ::CreateCaret(hwndSafe(), 0, szInit.c4Width(), szInit.c4Height());
}


//
//  This is called to create the child widgets of sdialog, from Created(). We go
//  through the dialog description and create all of the controls and initialize
//  them.
//
//  But it's also often used by regular windows to load their contents from a
//  dialog description.
//
tCIDLib::TVoid
TWindow::CreateDlgItems(const TDlgDesc& dlgdInfo, tCIDLib::TCard4& c4InitFocus)
{
    TDlgDesc::TCursor cursItems = dlgdInfo.cursItems();
    c4InitFocus = 0;

    // If none, then just return now
    if (!cursItems.bIsValid())
        return;

    TArea       areaTmp;
    TDlgItem    dlgiCur;
    TRGBClr     rgbOurBgn = rgbBgnFill();
    for (; cursItems; ++cursItems)
    {
        // Get a copy of the current item so we can modify it
        dlgiCur = *cursItems;

        // Let the derived class update the item first
        ModDlgItem(dlgiCur);

        //
        //  Translate the dialog type to one of our classes. If it's been set manually
        //  then we take that. Else we provide a default.
        //
        TString strWndClass;
        if (!dlgiCur.strCppType().bIsEmpty())
            strWndClass =  dlgiCur.strCppType();
        else
            strWndClass = facCIDCtrls().strXlatDlgClass(dlgiCur.strType());

        //
        //  We use the class name in the description to dynamically create an
        //  object of that type. Then we ask it to initialize from the item
        //  description (which all widgets must be able to do.)
        //
        TCtrlWnd* pwndNew = ::pobjMakeNewOfClass<TCtrlWnd>(strWndClass);
        pwndNew->InitFromDesc(*this, dlgiCur, dlgdInfo.eTheme());

        // Give back the id if one is marked with initial focus
        if (tCIDLib::bAllBitsOn(dlgiCur.eFlags(), tCIDCtrls::EDlgItemFlags::InitFocus))
            c4InitFocus = dlgiCur.ridChild();

        // If it has the disabled flag then, disable it
        if (tCIDLib::bAllBitsOn(dlgiCur.eFlags(), tCIDCtrls::EDlgItemFlags::Disabled))
            pwndNew->SetEnable(kCIDLib::False);

        // Set any edge anchor value
        pwndNew->eEdgeAnchor(dlgiCur.eAnchor());
    }
}


//
//  Derived classes call this to create a new window. We do a little massaging
//  of the incoming values, create the window,
//
tCIDLib::TVoid
TWindow::CreateWnd( const   tCIDCtrls::TWndHandle   hwndParOwner
                    , const tCIDLib::TCh* const     pszClass
                    , const tCIDLib::TCh* const     pszText
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EExWndStyles eExStyles
                    , const tCIDCtrls::TWndId       widThis)
{
    //
    //  Init the common controls if not done yet, register our two custom window classes,
    //  and initialize our window mapping data.
    //
    static TAtomicFlag atomInitDone;
    if (!atomInitDone)
    {
        TBaseLock lockInit;
        if (!atomInitDone)
        {
            INITCOMMONCONTROLSEX Init = {0};
            Init.dwSize = sizeof(Init);
            Init.dwICC = ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS
                         | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_USEREX_CLASSES;

            if (!::InitCommonControlsEx(&Init))
            {
                TKrnlError::SetLastHostError(::GetLastError());
            }

            const TRGBClr rgbBgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window);
            RegWndClass
            (
                kCIDCtrls::pszCustClass
                , kCIDLib::False
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::False
            );

            RegWndClass
            (
                kCIDCtrls::pszCustClassHV
                , kCIDLib::True
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::False
            );

            atomInitDone.Set();
        }
    }

    // Make sure it's not already been created
    if (m_hwndThis != kCIDCtrls::hwndInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_AlreadyCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , TString(pszText)
        );
    }

    // Deal with the area stuff, which may be set to default values
    tCIDLib::TInt4 i4XPos = CW_USEDEFAULT;
    tCIDLib::TInt4 i4YPos = CW_USEDEFAULT;
    tCIDLib::TInt4 i4CX = CW_USEDEFAULT;
    tCIDLib::TInt4 i4CY = CW_USEDEFAULT;
    if (!TArea::bIsNullObject(areaInit))
    {
        if (areaInit.i4X() == kCIDLib::i4MinInt)
            i4XPos = CW_USEDEFAULT;
        else
            i4XPos = areaInit.i4X();

        if (areaInit.i4Y() == kCIDLib::i4MinInt)
            i4YPos = CW_USEDEFAULT;
        else
            i4YPos = areaInit.i4Y();

        if (areaInit.c4Width() == kCIDLib::c4MaxCard)
            i4CX = CW_USEDEFAULT;
        else
            i4CX = tCIDLib::TInt4(areaInit.c4Width());

        if (areaInit.c4Height() == kCIDLib::c4MaxCard)
            i4CY = CW_USEDEFAULT;
        else
            i4CY = tCIDLib::TInt4(areaInit.c4Height());
    }

    // Reset some 'last state' stuff and other things that are per-invocation
    m_areaPrev.ZeroAll();
    m_c4EnteredLoops = 0;
    m_c4SuppressPaint = 0;
    m_c4TimerSuspend = 0;
    m_eLastPosState = tCIDCtrls::EPosStates::Count;
    m_pnothHead = nullptr;
    m_pnotrHead= nullptr;

    //
    //  Store the window text at our level. We will also set it at the Windows
    //  level by pass it to create.
    //
    m_strWndText = pszText;

    // Store the window id
    m_widThis = widThis;

    // Create the window
    tCIDCtrls::TWndHandle hwndTmp = ::CreateWindowExW
    (
        tCIDLib::c4EnumOrd(eExStyles)
        , pszClass
        , pszText
        , tCIDLib::TCard4(eStyles)
        , i4XPos
        , i4YPos
        , i4CX
        , i4CY
        , hwndParOwner
        , (HMENU)widThis
        , *(HINSTANCE*)&facCIDCtrls().kmodThis().hmodThis().hmodiThis()
        , this
    );

    if (!hwndTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_CreateWnd
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(pszClass)
        );
    }

    //
    //  If the accept files extended style is on, as of Win7 we have to explicitly
    //  allow the messages required to support it.
    //
    if (tCIDLib::bAllBitsOn(eExStyles, tCIDCtrls::EExWndStyles::AcceptFiles))
    {
        ::ChangeWindowMessageFilterEx(hwndTmp, WM_COPYDATA, MSGFLT_ALLOW, 0);
        ::ChangeWindowMessageFilterEx(hwndTmp, WM_DROPFILES, MSGFLT_ALLOW, 0);
        ::ChangeWindowMessageFilterEx(hwndTmp, 0x0049, MSGFLT_ALLOW, 0);
    }

    //
    //  For those windows that are not our custom ones, we have to set up the
    //  window tracking data and store the handle. The ones that are ours, they
    //  get set up by our low level message dispatcher when he sees the
    //  WM_CREATE/WM_INITDIALOG msg. But others don't get handled by our dispatcher.
    //  So we query the user data and, if not set, we set it here.
    //
    //  And we have to fake a call to the Created method, though in this case
    //  it will be after the window is fully created.
    //
    //  We set our default font on them as well.
    //
    if (!TWindow::pwndGetWndLinkPtr(hwndTmp))
    {
        // Store a pointer to our window structure in the extra window data
        TWndMapItem* pwmiNew = new TWndMapItem(hwndTmp, this);
        CIDCtrls_Window::colWndItemMap().Add(pwmiNew);

        // Store the handle before we do anything else
        m_hwndThis = hwndTmp;

        // And now fake a call to created for our class to do any init
        bCreated();
    }
}


// Allows derived classes to decrement our modal loop counter
tCIDLib::TVoid TWindow::DecModalCounter()
{
    if (m_c4EnteredLoops)
        m_c4EnteredLoops--;
}


// Set the flag that prevents notification reflection
tCIDLib::TVoid TWindow::DisableNotReflection()
{
    m_bNoReflect = kCIDLib::True;
}


//
//  Sends an async notification to this window. This is for internal use to support
//  the eventing system. This is called from a public, templatized method.
//
tCIDLib::TVoid TWindow::DoAsyncNotification(const TWndEvent& wevToSend) const
{
    CIDAssert
    (
        wevToSend.pobjToSend() != nullptr, L"Notification data object is not set"
    );

    //
    //  Find all of the notification requests for the notification id in
    //  the passed event. For each one, post to the target of that request
    //  object.
    //
    TNotRequestBase* pnotrCur = m_pnotrHead;
    while (pnotrCur)
    {
        if (pnotrCur->m_nidRequested == wevToSend.nidBeingSent())
        {
            //
            //  Create a new window event object and send it. This will also duplicate
            //  the user data.
            //
            TWndEvent* pwevToSend = wevToSend.pwevDuplicate();

            // If the post did not work, then delete the data and throw
            if (!pnotrCur->bPostEvent(pwevToSend))
            {
                delete pwevToSend;
                facCIDCtrls().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcNot_CantNotify
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TString(pnotrCur->nidRequested())
                );
            }
        }
        pnotrCur = pnotrCur->m_pnotrNext;
    }
}

//
//  Same as SendAsyncNotification above, but does it synchronously. In this case we
//  never copy the event object, so we pass the actual passed event to the handlers,
//  synchronously, so they can update the event object. Though the event object itself
//  is const, the per-event type data in it is not.
//
tCIDLib::TVoid TWindow::DoSyncNotification(const TWndEvent& wevToSend)
{
    CIDAssert
    (
        wevToSend.pobjToSend() != nullptr, L"Notification data object is not set"
    );

    //
    //  Find all of the notification requests for the notification id in
    //  the passed event. For each one, call to the target of that request
    //  object.
    //
    TNotRequestBase* pnotrCur = m_pnotrHead;
    while (pnotrCur)
    {
        //
        //  If we have a request object on this window, then ask its
        //  target object to invoke its handlers for this notification
        //  id.
        //
        if (pnotrCur->m_nidRequested == wevToSend.nidBeingSent())
        {
            //
            //  If this handler indicates that it handled the event, then
            //  don't send it any further.
            //
            if (pnotrCur->eInvokeOnTarget(wevToSend.pobjToSend()) == tCIDCtrls::EEvResponses::Handled)
                break;
        }
        pnotrCur = pnotrCur->m_pnotrNext;
    }
}


// Draws standard control type borders, for use by custom windows
tCIDLib::TVoid
TWindow::DrawBorder(        TGraphDrawDev&      gdevTar
                    , const TArea&              areaTar
                    , const tCIDLib::TBoolean   bInner
                    , const tCIDLib::TBoolean   bRaised)
{
    RECT rectBorder;
    areaTar.ToRectl
    (
        *reinterpret_cast<tCIDLib::THostRectl*>(&rectBorder)
        , tCIDLib::ERectlTypes::NonInclusive
    );

    UINT Edge = 0;
    UINT Flags = 0;

    if (bInner && bRaised)
        Edge = BDR_RAISEDINNER;
    else if (bInner && !bRaised)
        Edge = BDR_SUNKENINNER;

    else if (!bInner && bRaised)
        Edge = BDR_RAISEDOUTER;
    else if (!bInner && !bRaised)
        Edge = BDR_SUNKENOUTER;

    ::DrawEdge(gdevTar.hdevThis(), &rectBorder, Edge, Flags);
}


//
//  Map a system virtual key code to our extended keys enum values
//
tCIDCtrls::EExtKeys
TWindow::eMapExtKey(const tCIDLib::TCard4 c4VKey, const tCIDLib::TBoolean bShifted)
{
    switch(c4VKey)
    {
        case VK_LBUTTON :
            return tCIDCtrls::EExtKeys::Button1;

        case VK_MBUTTON :
            return tCIDCtrls::EExtKeys::Button2;

        case VK_RBUTTON :
            return tCIDCtrls::EExtKeys::Button3;

        case VK_BACK :
            return tCIDCtrls::EExtKeys::BackSpace;

        case VK_TAB :
            if (bShifted)
                return tCIDCtrls::EExtKeys::BackTab;
            return tCIDCtrls::EExtKeys::Tab;

        case VK_SHIFT :
            return tCIDCtrls::EExtKeys::Shift;

        case VK_CONTROL :
            return tCIDCtrls::EExtKeys::Ctrl;

        case VK_MENU :
            return tCIDCtrls::EExtKeys::Alt;

        case VK_PAUSE :
            return tCIDCtrls::EExtKeys::Pause;

        case VK_CAPITAL :
            return tCIDCtrls::EExtKeys::CapsLock;

        case VK_ESCAPE :
            return tCIDCtrls::EExtKeys::Esc;

        case VK_PRIOR :
            return tCIDCtrls::EExtKeys::PageUp;

        case VK_NEXT :
            return tCIDCtrls::EExtKeys::PageDown;

        case VK_END :
            return tCIDCtrls::EExtKeys::End;

        case VK_HOME :
            return tCIDCtrls::EExtKeys::Home;

        case VK_LEFT :
            return tCIDCtrls::EExtKeys::Left;

        case VK_UP :
            return tCIDCtrls::EExtKeys::Up;

        case VK_RIGHT :
            return tCIDCtrls::EExtKeys::Right;

        case VK_DOWN :
            return tCIDCtrls::EExtKeys::Down;

        case VK_PRINT :
            return tCIDCtrls::EExtKeys::PrintScreen;

        case VK_INSERT :
            return tCIDCtrls::EExtKeys::Insert;

        case VK_DELETE :
            return tCIDCtrls::EExtKeys::Delete;

        case VK_SCROLL :
            return tCIDCtrls::EExtKeys::ScrollLock;

        case VK_NUMLOCK :
            return tCIDCtrls::EExtKeys::NumLock;

        case VK_RETURN :
            return tCIDCtrls::EExtKeys::Enter;

        case VK_SPACE :
            return tCIDCtrls::EExtKeys::Space;

        case VK_DIVIDE :
            return tCIDCtrls::EExtKeys::NumPad_Div;

        case VK_MULTIPLY :
            return tCIDCtrls::EExtKeys::NumPad_Mul;

        case VK_SUBTRACT :
            return tCIDCtrls::EExtKeys::NumPad_Sub;

        case VK_ADD :
            return tCIDCtrls::EExtKeys::NumPad_Plus;

        case VK_DECIMAL :
            return tCIDCtrls::EExtKeys::NumPad_Decimal;

        //
        //  Note that these are mapped through a different mechanism from
        //  the regular keystroke mechanism. So we ignore them here. These
        //  will be invoked via the WM_APPCOMMAND msg, which does its own
        //  mapping from APPCOMMAND constants, not VK_ constants.
        //
        //  eXlatAppCommand() above handles these, and they are propogated
        //  to the window via the eMediaKey() method.
        //
        case VK_BROWSER_BACK :
        case VK_BROWSER_FORWARD :
        case VK_BROWSER_STOP :
        case VK_BROWSER_REFRESH :
        case VK_BROWSER_SEARCH :
        case VK_BROWSER_FAVORITES :
        case VK_BROWSER_HOME :
        case VK_VOLUME_MUTE :
        case VK_VOLUME_DOWN :
        case VK_VOLUME_UP :
        case VK_MEDIA_PLAY_PAUSE :
        case VK_MEDIA_STOP :
        case VK_MEDIA_PREV_TRACK :
        case VK_MEDIA_NEXT_TRACK :
            return tCIDCtrls::EExtKeys::None;

        case VK_F1 :
            return tCIDCtrls::EExtKeys::F1;

        case VK_F2 :
            return tCIDCtrls::EExtKeys::F2;

        case VK_F3 :
            return tCIDCtrls::EExtKeys::F3;

        case VK_F4 :
            return tCIDCtrls::EExtKeys::F4;

        case VK_F5 :
            return tCIDCtrls::EExtKeys::F5;

        case VK_F6 :
            return tCIDCtrls::EExtKeys::F6;

        case VK_F7 :
            return tCIDCtrls::EExtKeys::F7;

        case VK_F8 :
            return tCIDCtrls::EExtKeys::F8;

        case VK_F9 :
            return tCIDCtrls::EExtKeys::F9;

        case VK_F10 :
            return tCIDCtrls::EExtKeys::F10;

        case VK_F11 :
            return tCIDCtrls::EExtKeys::F11;

        case VK_F12 :
            return tCIDCtrls::EExtKeys::F12;

        case VK_F13 :
            return tCIDCtrls::EExtKeys::F13;

        case VK_F14 :
            return tCIDCtrls::EExtKeys::F14;

        case VK_F15 :
            return tCIDCtrls::EExtKeys::F15;

        case VK_F16 :
            return tCIDCtrls::EExtKeys::F16;

        case VK_F17 :
            return tCIDCtrls::EExtKeys::F17;

        case VK_F18 :
            return tCIDCtrls::EExtKeys::F18;

        case VK_F19 :
            return tCIDCtrls::EExtKeys::F19;

        case VK_F20 :
            return tCIDCtrls::EExtKeys::F20;

        case VK_F21 :
            return tCIDCtrls::EExtKeys::F21;

        case VK_F22 :
            return tCIDCtrls::EExtKeys::F22;

        case VK_F23 :
            return tCIDCtrls::EExtKeys::F23;

        case VK_F24 :
            return tCIDCtrls::EExtKeys::F24;

        default :
            break;
    };
    return tCIDCtrls::EExtKeys::None;
}


//
//  Find the background brush that is appropriate for this window. If this window
//  has had his color set explicitly, or it has no parent, we use this window's
//  color. Else we use the parent's, who might in turn recurse.
//
const TGUIBrush& TWindow::gbrBgn() const
{
    if (m_bBgnClrOver)
        return m_gbrBgn;

    TWindow* pwndPar = pwndParent();
    if (!pwndPar)
        return m_gbrBgn;

    return pwndPar->gbrBgn();

}


//
//  Return the handle of a child window, optionally throwing if not found or it
//  is not valid. Else we will return zero if not found.
//
tCIDCtrls::TWndHandle
TWindow::hwndChildById( const   tCIDCtrls::TWndId   widToFind
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDCtrls::TWndHandle hwndChild = ::GetDlgItem(m_hwndThis, widToFind);
    if (!hwndChild)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_NoChildWithId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TCardinal(widToFind)
            );
        }
        return 0;
    }
    return hwndChild;
}


// Allows derived classes to bump our modal loop counter
tCIDLib::TVoid TWindow::IncModalCounter()
{
    m_c4EnteredLoops++;
}


// Destroy any current caret
tCIDLib::TVoid TWindow::KillCaret()
{
    ::DestroyCaret();
}


//
//  Allows our wrappers to pass a message on to its sub-classes underlying window, which is
//  necessary sometimes. It may want to modify the return, or just get some info.
//
tCIDCtrls::TMsgResult
TWindow::mresCallSubClass(  const   tCIDCtrls::TWndMsg  wmsgCur
                            , const tCIDCtrls::TWParam  wParam
                            , const tCIDCtrls::TLParam  lParam)
{
    return ::CallWindowProc((WNDPROC)m_pfOrgProc, hwndSafe(), wmsgCur, wParam, lParam);
}


//
//  Does the standard manimial redraw functionality, only redrawing what has
//  been uncovered.
//
tCIDLib::TVoid TWindow::MinimalRedraw(const TSize& szNew, const TSize& szLast)
{
    if ((szNew.c4Width() > szLast.c4Width())
    ||  (szNew.c4Height() > szLast.c4Height()))
    {
        TGraphWndDev gdevTmp(*this);
        TArea areaOne(TPoint::pntOrigin, szNew);
        TArea areaTwo(TPoint::pntOrigin, szLast);

        TGUIRegion grgnTmp(areaOne);
        TGUIRegion grgnTmp2(areaTwo);
        grgnTmp.CombineWith(grgnTmp2, tCIDGraphDev::EClipModes::Diff);
        Redraw(grgnTmp, tCIDCtrls::ERedrawFlags::Full);
    }
}


//
//  Let a window get a pointer to his owner object, if he has one. Optionally
//  throw if not found, else return zero if not found.
//
TWindow* TWindow::pwndFindOwner(const tCIDLib::TBoolean bThrowIfNot) const
{
    tCIDCtrls::TWndHandle hwndOwner = ::GetWindow(m_hwndThis, GW_OWNER);
    if (hwndOwner == kCIDCtrls::hwndInvalid)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_ParOwnerNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
        return nullptr;
    }

    // We have a parent window, see if it one of our windows
    return pwndGetWndLinkPtr(hwndOwner);
}


// Let a child get a pointer to his parent object, if he has one
TWindow* TWindow::pwndFindParent(const tCIDLib::TBoolean bThrowIfNot) const
{
    tCIDCtrls::TWndHandle hwndParent = ::GetAncestor(m_hwndThis, GA_PARENT);
    if (hwndParent == kCIDCtrls::hwndInvalid)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_ParOwnerNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
        return nullptr;
    }

    // We have a parent window, see if it one of our windows
    return pwndGetWndLinkPtr(hwndParent);
}


//
//  Static controls call this in order to get access to the protected viatual
//  method bStaticChildClick. We see if there is a parent window. If so, we call
//  this method on him.
//
tCIDLib::TVoid
TWindow::PassStaticChildClick(  const   tCIDCtrls::EMouseButts  eButton
                                , const tCIDCtrls::EMouseClicks eClickType)
{
    // Get the parent window, if any, and call the child click method
    TWindow* pwndPar = pwndParent();
    if (pwndPar)
        pwndPar->StaticChildClick(widThis(), eButton, eClickType);
}


//
//  Any derived classes that create custom windows will call this method to
//  register their class, which we point at our standard dispatcher above.
//
tCIDLib::TVoid
TWindow::RegWndClass(const  tCIDLib::TCh* const pszName
                    , const tCIDLib::TBoolean   bHVRedraw
                    , const tCIDLib::TBoolean   bDblClicks
                    , const tCIDLib::TCard4     c4Cursor
                    , const TRGBClr&            rgbBgn
                    , const tCIDLib::TBoolean   bDlgProc)
{
    HINSTANCE hInstance = *(HINSTANCE*)&facCIDCtrls().kmodThis().hmodThis().hmodiThis();

    //
    //  Register our window classes. We register two, one for size
    //  redraw type windows and one for non-redraw types. We use the
    //  appropriate ones based on the window styles.
    //
    WNDCLASSEX  wcData = {0};
    wcData.cbSize = sizeof(WNDCLASSEX);

    if (bDlgProc)
    {
        //
        //  Get the standard dialog box class info and just update what we
        //  need to.
        //
        ::GetClassInfoEx(hInstance, L"#32770", &wcData);
    }
     else
    {
        wcData.cbWndExtra = DLGWINDOWEXTRA;

        // If they indicated a cursor, use that, else the normal one
        if (c4Cursor)
            wcData.hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(c4Cursor));
        else
            wcData.hCursor = ::LoadCursor(NULL, IDI_APPLICATION);
    }

    wcData.hbrBackground = 0;

    // This is done the same regardless of the above
    wcData.lpszClassName = pszName;
    wcData.lpfnWndProc = (WNDPROC)mresCIDCtrlsDispatch;
    wcData.hInstance = hInstance;

    if (bDblClicks)
        wcData.style |= CS_DBLCLKS;
    if (bHVRedraw)
        wcData.style |= CS_HREDRAW | CS_VREDRAW;

    if (!::RegisterClassEx(&wcData))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_RegisterClass
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(pszName)
        );
    }
}


// Handles the grunt work of posting a child focus change msg
tCIDLib::TVoid
TWindow::SendChildFocusNot( const   TWindow&            wndTar
                            , const TWindow&            wndParent
                            , const tCIDCtrls::TWndId   widChild
                            , const tCIDLib::TBoolean   bGetting)
{
    // High bit is whether getting or losing, rest is the child id
    const tCIDCtrls::TLParam lParam
    (
        (bGetting ? 0x80000000 : 0) | widChild
    );

    ::PostMessage
    (
        wndTar.hwndThis()
        , kCIDCtrls::wmsgChildFocusNot
        , tCIDCtrls::TWParam(wndParent.hwndThis())
        , lParam
    );
}


// Set our caret position
tCIDLib::TVoid TWindow::SetCaretAt(const TPoint& pntAt)
{
    ::SetCaretPos(pntAt.i4X(), pntAt.i4Y());
}


// Show or hide our caret, if we have one
tCIDLib::TVoid TWindow::SetCaretVis(const tCIDLib::TBoolean bState)
{
    if (bState)
        ::ShowCaret(hwndSafe());
    else
        ::HideCaret(hwndSafe());
}


tCIDLib::TVoid
TWindow::SetSBarPos(const tCIDLib::TBoolean bHorz, const tCIDLib::TCard4 c4Pos)
{
    int iBar = bHorz ? SB_HORZ : SB_VERT;
    ::SetScrollPos(hwndSafe(), iBar, c4Pos, TRUE);
}



//
//  Derived classes that wrap standard controls sometimes need to sub-class them. They
//  can call this and we will sub-class to our internal dispatcher, and it will pass
//  on unhandled msgs to the stored original handler, instead of the default one.
//
//  None of these messages we intercept cause method calls to the window, except for
//  the bEatSubClassMsg to let them pocess and eat msgs if they need to.
//
//  HOWEVER we also need to see the standard control's WM_DESTROY message so that
//  we can clean it up, so the msg dispatcher also watches for that.
//
tCIDLib::TVoid TWindow::SetSubclass()
{
    m_pfOrgProc = (tCIDCtrls::TDispatchFunc)::SetWindowLong
    (
        hwndSafe(), GWLP_WNDPROC, tCIDCtrls::TLParam(&mresCIDCtrlsDispatch)
    );
}


//
//  Derived classes can call this to indicate special input they want to see, that
//  otherwise would be passed on to possible special handlers. We handle WM_GETDLGCODE
//  for the window and return this value.
//
tCIDLib::TVoid TWindow::SetWantInput(const tCIDCtrls::EWantInputs eToSet)
{
    m_eWantInput = eToSet;
}


//
//  This exists purely for the base dispatcher method to get us set up when he
//  sees WM_CREATE. He sets up the data stored in the user data, and he stores the
//  handle on us so that, when the Create() method is called, we are ready to go.
//
tCIDLib::TVoid TWindow::StoreHandle(const tCIDCtrls::TWndHandle hwndToStore)
{
    CIDAssert
    (
        m_hwndThis == kCIDCtrls::hwndInvalid, L"StoreHandle is overwriting non-zero handle"
    );
    m_hwndThis = hwndToStore;
}


//
//  For use by the window paint suppression janitor. We just send ourself a
//  set redraw msg. If this is a standard control it will set its own flag.
//  And our own will be set as well.
//
//  To avoid issues with nested calls, we maintain a counter, which is also
//  used by our own custom windows as a suppression indicator as well. But it
//  lets us also only in/dec when transitioning to/from a zero count.
//
tCIDLib::TVoid TWindow::SuppressPaint(const tCIDLib::TBoolean bIncrement)
{
    if (bIncrement && !m_c4SuppressPaint)
    {
        ::SendMessage(hwndSafe(), WM_SETREDRAW, 0, 0);
    }
     else if (!bIncrement && m_c4SuppressPaint)
    {
        ::SendMessage(hwndSafe(), WM_SETREDRAW, 1, 0);
        Redraw(tCIDCtrls::ERedrawFlags::Full);
    }
}


//
//  Allow a handle to be forced onto an existing window. Basically allowing us to
//  wrap existing windows such as system windows. In those cases bAdopt should not
//  be set.
//
tCIDLib::TVoid
TWindow::UseHandle(         tCIDCtrls::TWndHandle   hwndToUse
                    , const tCIDLib::TBoolean       bAdopt
                    , const tCIDLib::TBoolean       bSubClass)
{
    m_hwndThis = hwndToUse;
    m_bOwnsHandle = bAdopt;

    m_widThis = ::GetDlgCtrlID(hwndToUse);

    // Store a pointer to our window structure in the extra window data
    TWndMapItem* pwmiNew = new TWndMapItem(hwndToUse, this);
    CIDCtrls_Window::colWndItemMap().Add(pwmiNew);

    if (bSubClass)
        SetSubclass();
}



// ---------------------------------------------------------------------------
// TWindow: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we get a WM_DROPFILES and we have the accept files style, then this will be
//  called. We will iterate the files and get them into a string list an dthen call
//  the virtual methods to let the derived class process it.
//
tCIDLib::TBoolean
TWindow::bHandleFileDrop(const  tCIDCtrls::TWParam  wParam
                        , const tCIDCtrls::TLParam  lParam)
{
    HDROP hDrop = (HDROP)wParam;

    // First get the count of files in the list
    const tCIDLib::TCard4 c4Count = ::DragQueryFile
    (
        hDrop, kCIDLib::c4MaxCard, 0, 0
    );

    if (!c4Count)
        return kCIDLib::False;

    // Create a worst case path buffer
    const tCIDLib::TCard4 c4BufSz = kCIDLib::c4MaxPathLen * 2;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    tCIDLib::TStrList colFiles;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (::DragQueryFile(hDrop, c4Index, achBuf, c4BufSz))
            colFiles.objAdd(TString(achBuf));
    }

    // Let the derived class handle it and return his return
    return bFilesDropped(colFiles);
}


//
//  This is the cleanup of all of the related stuff, other than the actual window
//  handle. Its called from the WM_DESTROY event.
//
tCIDLib::TVoid TWindow::DoCleanup()
{
    //
    //  We have to clean up the window objects for all of the child windows of this
    //  window, at least the ones that are CIDCtrls windows. Otherwise, they'd just
    //  leak away.
    //
    DestroyAllChildren();

    // If there are any notification requests, then clean up the list.
    if (m_pnotrHead)
    {
        TNotRequestBase* pnotrCur = m_pnotrHead;
        TNotRequestBase* pnotrNext;
        while (pnotrCur)
        {
            // Get the next before we toast the current
            pnotrNext = pnotrCur->m_pnotrNext;

            // Now delete the current and move up
            delete pnotrCur;
            pnotrCur = pnotrNext;
        }
        m_pnotrHead = nullptr;
    }

    // And do the same for notification handlers
    if (m_pnothHead)
    {
        TNotHandlerBase* pnothCur = m_pnothHead;
        TNotHandlerBase* pnothN;
        while (pnothCur)
        {
            // Get the next before we toast the current
            pnothN = pnothCur->pnothNext();

            // Now delete the current and move up
            delete pnothCur;
            pnothCur = pnothN;
        }
        m_pnothHead = nullptr;
    }
}


//
//  There are a few places where the caller provides a timer id which must be
//  validated. So this method does that. It checks if it's a valid id at all, can't
//  be zero, or > max timers (can be equal since they are 1 based.) And it then
//  checks to make sure it is already started or that it has not been started
//
tCIDLib::TVoid
TWindow::CheckTimerId(const tCIDCtrls::TTimerId tmidCheck) const
{
    // Make sure it's a valid id at all
    if (!tmidCheck || (tmidCheck > kCIDCtrls::c4MaxWndTimers))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTimer_BadId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(tmidCheck)
        );
    }
}

tCIDLib::TVoid
TWindow::CheckTimerId(  const   tCIDCtrls::TTimerId tmidCheck
                        , const tCIDLib::TBoolean   bCheckActive) const
{
    // Make sure it's a valid id at all
    CheckTimerId(tmidCheck);

    // And validate the active/inactive status
    if (bCheckActive)
    {
        if (!m_aTimers[tmidCheck - 1].bStarted)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTimer_NotReady
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotReady
                , TCardinal(tmidCheck)
            );
        }
    }
     else
    {
        if (m_aTimers[tmidCheck - 1].bStarted)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTimer_Already
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
                , TCardinal(tmidCheck)
            );
        }
    }
}


// Called from WM_INITDIALOG, to mark us as a dialog box
tCIDLib::TVoid TWindow::SetIsDialog()
{
    m_bIsDialog = kCIDLib::True;

    // Make sure we aren't marked to use the def dialog proc in this case
    m_bDlgProc = kCIDLib::False;
}


//
//  Given the msg info for a mouse click msg, this guy translates that into our
//  form. We have to do that from more than one place.
//
tCIDLib::TVoid
TWindow::XlatButtonMsg( const   tCIDCtrls::TWndMsg          wmsgCur
                        , const tCIDCtrls::TWParam          wParam
                        , const tCIDCtrls::TLParam          lParam
                        ,       tCIDCtrls::EMouseButts&     eButton
                        ,       tCIDCtrls::EMouseClicks&    eClick
                        ,       TPoint&                     pntAt
                        ,       tCIDLib::TBoolean&          bCtrlShift
                        ,       tCIDLib::TBoolean&          bShift)
{
    //
    //  For our own custom windows, we need to process button clicks and
    //  call a virtual to let the window process them. For standard ctrls,
    //  to the extent they need to deal with clicks, they would do it via the
    //  subclass callback.
    //
    eButton = tCIDCtrls::EMouseButts::Left;
    if ((wmsgCur == WM_MBUTTONDBLCLK)
    ||  (wmsgCur == WM_MBUTTONUP)
    ||  (wmsgCur == WM_MBUTTONDOWN))
    {
        eButton = tCIDCtrls::EMouseButts::Middle;
    }
     else if ((wmsgCur == WM_RBUTTONDBLCLK)
          ||  (wmsgCur == WM_RBUTTONUP)
          ||  (wmsgCur == WM_RBUTTONDOWN))
    {
        eButton = tCIDCtrls::EMouseButts::Right;
    }

    eClick = tCIDCtrls::EMouseClicks::Down;
    if ((wmsgCur == WM_LBUTTONDBLCLK)
    ||  (wmsgCur == WM_MBUTTONDBLCLK)
    ||  (wmsgCur == WM_RBUTTONDBLCLK))
    {
        eClick = tCIDCtrls::EMouseClicks::Double;
    }
     else if ((wmsgCur == WM_LBUTTONUP)
          ||  (wmsgCur == WM_MBUTTONUP)
          ||  (wmsgCur == WM_RBUTTONUP))
    {
        eClick = tCIDCtrls::EMouseClicks::Up;
    }

    //
    //  Create a point that represents the click position. It will be in our
    //  window's client coordinates.
    //
    pntAt.Set(TRawBits::i2Low16From32(lParam), TRawBits::i2High16From32(lParam));

    bCtrlShift = (wParam & MK_CONTROL) != 0;
    bShift = (wParam & MK_SHIFT) != 0;
}


