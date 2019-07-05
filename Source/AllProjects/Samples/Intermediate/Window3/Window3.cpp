//
// CID_FILE NAME: Window3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1019
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
//  This sample program is step up from Window2. The clickable client window from
//  the 2 version becomes a separate custom control in this one. We set up the
//  layout using the resource editor, so we can load the controls from that and
//  avoid a lot of work. And it also keeps everything scaled to the local default
//  font, since the editor works in terms of dialog units.
//
//  The controls have anchor attributes which tell the client window to reposition
//  them as it is resized, so it's all automatically resizable without any extra
//  effort.
//
//  However, because there's a custom control involved (our little circle clicker
//  editor) we just put a place holder into the dialog layout. When the dialog
//  description is loaded, we update the C++ class for that guy to make him create
//  the correct window class. The resource editor needs to be updated to allow
//  that class to be just put into the dialog description.
//
//  The resource editor is also creating the .MsgText file which contains any
//  loadable text I defined, in this case just a couple things that I wante to refer
//  to in the dialog definition.
//
//  The resource compiler spits out headers that have error and message ids for
//  the loadable text, and resource ids for the dialogs and controls so that we can
//  refer to them in our program.
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
#include  "Window3.hpp"


// ----------------------------------------------------------------------------
//  Magic main module code. Note this one is different from the console based
//  samples!
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_CtrlsModule(TThread(L"Window2MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local data
// ----------------------------------------------------------------------------

//
//  To load resources we need a facility object, so just create a generic one.
//  We want to load our content from an external dialog layout, and the text
//  that dialog layout is also loaded.
//
static TGUIFacility facWindow3
(
    L"Window3"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgsAndRes
);


// ----------------------------------------------------------------------------
//   CLASS: TW3FrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TW3FrameWnd : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TW3FrameWnd() = default;
        TW3FrameWnd(const TW3FrameWnd&) = delete;
        ~TW3FrameWnd() = default;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------

        // We have to handle this to create our child windows
        tCIDLib::TBoolean bCreated() override
        {
            // Call our parent first
            TFrameWnd::bCreated();

            //
            //  Tell our parent we want to use a generic window as a client, which
            //  does what we need, and tell it what id to give it, just the next
            //  available one for this window.
            //
            TGenericWnd* pwndClient = pwndInstallGenericClientWnd(widNext());

            //
            //  Load up our dialog description. Since we have a custom control,
            //  we used a generic place holder window and we need to update
            //  it's actual C++ class type to be our editor control.
            //
            TDlgDesc dlgdChildren;
            facWindow3.bCreateDlgDesc(kWindow3::ridMain, dlgdChildren);
            dlgdChildren.SetCppType(kWindow3::ridMain_Points, L"TWindow3Ed");

            //
            //  And tell our client window to populate itself from the dialog info. It
            //  will do an initial adjustment of the controls to fit.
            //
            tCIDCtrls::TWndId widInitFocus;
            pwndClient->PopulateFromDlg(dlgdChildren, widInitFocus, kCIDLib::True);

            // Get typed pointers to the important controls
            m_pwndClose = pwndClient->pwndChildAs<TPushButton>(kWindow3::ridMain_Close);
            m_pwndCount = pwndClient->pwndChildAs<TStaticText>(kWindow3::ridMain_Count);
            m_pwndDiscard = pwndClient->pwndChildAs<TPushButton>(kWindow3::ridMain_Discard);
            m_pwndEditor = pwndClient->pwndChildAs<TWindow3Ed>(kWindow3::ridMain_Points);

            // Set up click handlers for our two buttons and editor
            m_pwndClose->pnothRegisterHandler(this, &TW3FrameWnd::eClickHandler);
            m_pwndDiscard->pnothRegisterHandler(this, &TW3FrameWnd::eClickHandler);
            m_pwndEditor->pnothRegisterHandler(this, &TW3FrameWnd::eEditorHandler);

            return kCIDLib::True;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // Handle clicks from our buttons
        tCIDCtrls::EEvResponses eClickHandler(TButtClickInfo& wnotEvent)
        {
            if (wnotEvent.widSource() == kWindow3::ridMain_Close)
                facCIDCtrls().ExitLoop(0);
            else if (wnotEvent.widSource() == kWindow3::ridMain_Discard)
                m_pwndEditor->DiscardPoints();
            return tCIDCtrls::EEvResponses::Handled;
        }

        //
        //  Handle notifications from our editor window, which is just count
        //  of points currently, and we only have one editor, so no need to
        //  check what we got.
        //
        tCIDCtrls::EEvResponses eEditorHandler(TWindow3EdNot& wnotEvent)
        {
            TString strCount(8UL);
            strCount.SetFormatted(wnotEvent.m_c4Count);
            m_pwndCount->strWndText(strCount);
            return tCIDCtrls::EEvResponses::Handled;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndXXX
        //      We keep typed pointers to some of our controls. To just have a
        //      defaulted ctor, we set them to null here.
        // -------------------------------------------------------------------
        TPushButton*        m_pwndClose = nullptr;
        TStaticText*        m_pwndCount = nullptr;
        TPushButton*        m_pwndDiscard = nullptr;
        TWindow3Ed*         m_pwndEditor = nullptr;
};


// ----------------------------------------------------------------------------
//  We start up the main thread here
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  Create an instance of our frame window, let the system decide
        //  on placement.
        //
        TW3FrameWnd wndMain;
        wndMain.CreateFrame
        (
            nullptr
            , facCIDCtrls().areaDefWnd()
            , L"Window 3 Sample (Click Me)"
            , tCIDCtrls::EWndStyles::StdFrame
            , tCIDCtrls::EExWndStyles::None
            , tCIDCtrls::EFrameStyles::StdFrame
            , kCIDLib::False
        );

        // Now just process msgs until the user closes us
        facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        // Use the desktop as owner, since our frame window may not have worked
        TErrBox msgbErr(L"Window 3 Sample", errToCatch.strErrText());
        msgbErr.ShowIt(TWindow::wndDesktop());
    }

    return tCIDLib::EExitCodes::Normal;
}
