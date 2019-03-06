//
// FILE NAME: CIDKernel.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1992
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main facility file. This guy provides the initialization for
//  the facility. Because of the special nature of this facility, it does
//  not use a lazy evaluation scheme for constructing static data upon
//  demand, as the rest of the system does. This facility needs to make sure
//  that stuff gets done up front because its stuff that might not ever get
//  kicked off otherwise.
//
//  Initialization is done by way of the 'facility object' ctor (the only
//  static/global object in the whole DLL), which turns around and calls each
//  file's init method in the required order. The destructor calls them in
//  the reverse order, so that each file can clean up if required.
//
//  Note that at this level there is no TFacility class, so the 'facility
//  object' here is just a simple TKrnlModule derived class.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDKernel
{
    tCIDLib::TBoolean   bServiceMode = kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  Public data
//
//  kmodCIDKernel
//      This is the kernel's version of a facility object. It allows access
//      to things like the kernel's strings and such, to CIDLib. And its
//      ctor/dtor handle init and term for all other files.
// ---------------------------------------------------------------------------
TCIDKrnlModule  kmodCIDKernel;



//
//  If debugging, force use of some templatized stuff so that we catch syntax errors now
//  and now hours later and have to rebuild again. These are not functional tests, those
//  are elsewhere.
//
#if CID_DEBUG_ON
enum ETestBits
{
    ETestBit_None   = 0x0000
    , ETestBit_One  = 0x0001
    , ETestBit_Two  = 0x0002
    , ETestBit_Four = 0x0004

    , ETestBits_Six = 0x0006
    , ETestBits_Nine= 0x0009
};

ETestBits eVal1 = ETestBit_One;
ETestBits eVal2 = ETestBit_Two;
ETestBits eMask = ETestBits_Six;

static void LocalTmplCheckFunc()
{
    tCIDLib::bAnyBitsOn(eVal1, eMask);
    tCIDLib::bAllBitsOn(eVal1, eMask);
    tCIDLib::bBitsChanged(eVal1, eVal2, eMask);
    tCIDLib::eClearEnumBits(eVal1, eVal2);
    tCIDLib::eMaskEnumBits(eVal1, eVal2, eMask);
    tCIDLib::eOREnumBits(eVal1, eVal2);

    tCIDLib::TBoolean b1 = kCIDLib::True;
    const tCIDLib::TBoolean b2 = kCIDLib::False;
    tCIDLib::TBoolean b3 = kCIDLib::True;
    const tCIDLib::TBoolean b4 = kCIDLib::False;

    tCIDLib::TBoolean* pB1 = &b1;
    const tCIDLib::TBoolean* pB2 = &b2;
    tCIDLib::TBoolean* pB3 = &b3;
    const tCIDLib::TBoolean* pB4 = &b4;
    {
        TPtrJanitor<tCIDLib::TBoolean> janTest(pB1, pB3);
    }
    {
        TPtrJanitor<const tCIDLib::TBoolean> janTest(pB2, pB4);
    }
}
#endif


// ---------------------------------------------------------------------------
//   CLASS: TCIDKrnlModule
//  PREFIX: kmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKrnlModule::bServiceMode()
{
    return CIDKernel::bServiceMode;
}


//
//  The magic startup macro will call us here to provide us with some info
//  that only it can easily know.
//
tCIDLib::TVoid
TCIDKrnlModule::InitKrnlModule(const tCIDLib::TBoolean bServiceMode)
{
    CIDKernel::bServiceMode = bServiceMode;
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDKrnlModule::TCIDKrnlModule() :

    TKrnlModule()
    , m_bCanLoadText(kCIDLib::False)
    , m_c4InitCount(0)
    , m_pafInitMethods(0)
    , m_pszKrnlInitErr(kCIDKernel_::pszKrnlInitErr)
    , m_pszKrnlTermErr(kCIDKernel_::pszKrnlTermErr)
    , m_pszPopUpTitle1(kCIDKernel_::pszDefTitle1)
    , m_pszPopUpTitle2(kCIDKernel_::pszDefTitle2)
    , m_pszMsgNotFound(kCIDKernel_::pszMsgNotFound)
    , m_pszSysInitErr(kCIDKernel_::pszSysInitErr)
    , m_pszSysTermErr(kCIDKernel_::pszSysTermErr)
    , m_pszUnknownErr(kCIDKernel_::pszUnknownErr)
{
    try
    {
        //
        //  Init the error handler module, which puts into place the very
        //  fundamental mechanism to get/set the last error, that everyone
        //  else will use.
        //
        InitTermError(tCIDLib::EInitTerm::Initialize);

        //
        //  The next thing we have to do is to load the locale support. We
        //  need this in order to load our message file (which will be the
        //  first one loaded.) All the other init methods are called below
        //  in a special scheme, but this one has to be done specially.
        //
        //  Of course, if these guys throw an exception, we can only popup
        //  with the default error messages defaulted in above in our
        //  member init list. But its very primal init and should seldom
        //  fail.
        //
        InitTermLocale(tCIDLib::EInitTerm::Initialize);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        //
        //  We show a hard coded message here since we know we cannot load
        //  any text if the module did not initialize. Its not translatable,
        //  but what can we do?
        //
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , m_pszPopUpTitle1
            , m_pszPopUpTitle2
            , 0
            , kerrToCatch.errcId()
            , kerrToCatch.errcHostId()
            , kCIDKernel_::pszModInitErr
            , kCIDLib::pszEmptyZStr
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
    }

    //
    //  And now we can do the deferred loading of the module info and
    //  the message text file. If this fails, we can once again only do
    //  a hard coded text display.
    //
    if (!bQueryFromName
    (
        L"CIDKernel"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , tCIDLib::EModFlags::HasMsgFile))
    {
        const TKrnlError& kerrToShow = TKrnlError::kerrLast();
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , m_pszPopUpTitle1
            , m_pszPopUpTitle2
            , 0
            , kerrToShow.errcId()
            , kerrToShow.errcHostId()
            , kCIDKernel_::pszModInitErr
            , L"CIDKernel"
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
    }

    //
    //  If we survived the basic initialization, then our message text file
    //  was probably found. So lets load up some messages that we cache for
    //  quick access.
    //
    //  If any failure happened before now, then the popup methods (below)
    //  will use hard coded English versions for the two title strings.
    //
    //  Check the first one to see if we get it loaded. If not, then assume
    //  that our message file was not found and don't bother with the rest
    //  because we just need to give up now.
    //
    tCIDLib::TBoolean bOk;
    m_pszPopUpTitle1 = pszLoadCIDFacMsg(kKrnlMsgs::midGen_Title1, bOk);

    if (!bOk)
    {
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , m_pszPopUpTitle1
            , m_pszPopUpTitle2
            , 0
            , 0
            , 0
            , kCIDKernel_::pszNoMsgFile
            , kCIDLib::pszEmptyZStr
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
    }

    //
    //  Looks like we have message text, so lets cache the rest of the
    //  special messages.
    //
    m_pszPopUpTitle2 = pszLoadCIDFacMsg(kKrnlMsgs::midGen_Title2);
    m_pszKrnlInitErr = pszLoadCIDFacMsg(kKrnlErrs::errcInit_KrnlInitError);
    m_pszKrnlTermErr = pszLoadCIDFacMsg(kKrnlErrs::errcInit_KrnlTermError);
    m_pszSysInitErr  = pszLoadCIDFacMsg(kKrnlErrs::errcInit_SysInitError);
    m_pszSysTermErr  = pszLoadCIDFacMsg(kKrnlErrs::errcInit_SysTermError);
    m_pszMsgNotFound = pszLoadCIDFacMsg(kKrnlErrs::errcMod_MsgNotFound);
    m_pszUnknownErr  = pszLoadCIDFacMsg(kKrnlErrs::errcUnknown);

    // Indicate that we are up far enough to load text
    m_bCanLoadText = kCIDLib::True;

    //
    //  Build up the list of init methods and point our member pointer at
    //  it. Then also set the member that holds the count of methods.
    //
    //  Note that the ordering here is important. This is the order that
    //  we want to boot up the kernel, and the (reverse) order for cleanup
    //  at program shutdown.
    //
    static TInitRec arecInit[] =
    {
            { &TCIDKrnlModule::bInitTermCore        , kKrnlMsgs::midInit_Core }
        ,   { &TCIDKrnlModule::bInitTermSigHandler  , kKrnlMsgs::midInit_SigHandler }
        ,   { &TCIDKrnlModule::bInitTermThread      , kKrnlMsgs::midInit_Thread }
        ,   { &TCIDKrnlModule::bInitTermConsole     , kKrnlMsgs::midInit_Console }
        ,   { &TCIDKrnlModule::bInitTermAudio       , kKrnlMsgs::midInit_Audio }
        ,   { &TCIDKrnlModule::bInitTermIP          , kKrnlMsgs::midInit_IP }
        ,   { &TCIDKrnlModule::bInitTermTime        , kKrnlMsgs::midInit_Time }
        ,   { &TCIDKrnlModule::bInitTermPerThread   , kKrnlMsgs::midInit_PerThread }
        ,   { &TCIDKrnlModule::bInitTermSysInfo     , kKrnlMsgs::midInit_SysInfo }
        ,   { &TCIDKrnlModule::bInitTermEnvironment , kKrnlMsgs::midInit_Environment }
        ,   { &TCIDKrnlModule::bInitTermFileSys     , kKrnlMsgs::midInit_FileSys }
        ,   { &TCIDKrnlModule::bInitTermRemMedia    , kKrnlMsgs::midInit_RemMedia }
    };
    m_parecInit = arecInit;
    m_c4InitCount = tCIDLib::c4ArrayElems(arecInit);

    //
    //  Now kick off all of the initialization methods, in the correct order
    //  of bootstrapping. Tell them that they are in init mode this time.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4InitCount; c4Index++)
    {
        try
        {
            if (!(this->*arecInit[c4Index].pfnInitTerm)(tCIDLib::EInitTerm::Initialize))
            {
                const TKrnlError& kerrLast = TKrnlError::kerrLast();

                //
                //  A kernel error occured during init, so show the information
                //  to the user and give up.
                //
                const tCIDLib::TCard4 c4MaxChars = 4095;
                tCIDLib::TCh szMsg[c4MaxChars+1];
                TRawStr::CopyCatStr
                (
                    szMsg
                    , c4MaxChars
                    , m_pszKrnlInitErr
                    , pszLoadCIDFacMsg(arecInit[c4Index].midDescription)
                );

                TKrnlPopup::Show
                (
                    CID_FILE
                    , CID_LINE
                    , m_pszPopUpTitle1
                    , m_pszPopUpTitle2
                    , 0
                    , kerrLast.errcId()
                    , kerrLast.errcHostId()
                    , szMsg
                    , pszLoadCIDFacMsg(kerrLast.errcId())
                );
                TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
            }
        }

        catch(...)
        {
            //
            //  A system exception occured during init, so show the
            //  information to the user and give up.
            //
            const tCIDLib::TCard4 c4MaxChars = 4095;
            tCIDLib::TCh szMsg[c4MaxChars+1];
            TRawStr::CopyCatStr
            (
                szMsg
                , c4MaxChars
                , m_pszSysInitErr
                , pszLoadCIDFacMsg(arecInit[c4Index].midDescription)
            );


            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , m_pszPopUpTitle1
                , m_pszPopUpTitle2
                , 0
                , 0
                , 0
                , szMsg
                , kCIDLib::pszEmptyZStr
            );
            TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
        }
    }
}


TCIDKrnlModule::~TCIDKrnlModule()
{
    //
    //  Call the init/term methods in reverse order and this time tell
    //  them that we are terminating.
    //
    tCIDLib::TCard4 c4Index = m_c4InitCount - 1;
    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        // Catch any errors so that they don't propogate out of here
        try
        {
            (this->*m_parecInit[c4Index].pfnInitTerm)(tCIDLib::EInitTerm::Terminate);
        }

        catch(const TKrnlError& kerrToCatch)
        {
            //
            //  A kernel error occured during init, so show the information
            //  to the user, but don't exit since we are exiting anyway.
            //
            const tCIDLib::TCard4 c4MaxChars = 4095;
            tCIDLib::TCh szMsg[c4MaxChars+1];
            TRawStr::CopyCatStr
            (
                szMsg
                , c4MaxChars
                , m_pszKrnlTermErr
                , pszLoadCIDFacMsg(m_parecInit[c4Index].midDescription)
            );

            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , m_pszPopUpTitle1
                , m_pszPopUpTitle2
                , 0
                , kerrToCatch.errcId()
                , kerrToCatch.errcHostId()
                , szMsg
                , pszLoadCIDFacMsg(kerrToCatch.errcId())
            );
        }

        catch(...)
        {
            //
            //  A system exception occured during init, so show the
            //  information to the user, but don't exit since we are exiting
            //  anyway.
            //
            const tCIDLib::TCard4 c4MaxChars = 4095;
            tCIDLib::TCh szMsg[c4MaxChars+1];
            TRawStr::CopyCatStr
            (
                szMsg
                , c4MaxChars
                , m_pszSysTermErr
                , pszLoadCIDFacMsg(m_parecInit[c4Index].midDescription)
            );

            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , m_pszPopUpTitle1
                , m_pszPopUpTitle2
                , 0
                , 0
                , 0
                , szMsg
                , kCIDLib::pszEmptyZStr
            );
        }

        // Break out if this was the 0th one, else move down for next
        if (!c4Index)
            break;
        c4Index--;
    }
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Do a low level popup with an error code that we load the text for
tCIDLib::TVoid
TCIDKrnlModule::ErrorPopUp( const   tCIDLib::TErrCode   errcToDisplay
                            , const tCIDLib::TCh* const pszFileName
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCh* const pszAuxMsg)
{
    //
    //  If we have not gotten far enough along to try to load text, then
    //  we just use the default message. Otherwise, load it.
    //
    const tCIDLib::TCh* pszMsg = m_pszMsgNotFound;
    if (m_bCanLoadText)
        pszMsg = pszLoadCIDFacMsg(errcToDisplay);

    TKrnlPopup::Show
    (
        pszFileName
        , c4Line
        , m_pszPopUpTitle1
        , m_pszPopUpTitle2
        , 0
        , errcToDisplay
        , 0
        , pszMsg
        , pszAuxMsg
    );
}


// Do a low level popup with a kernel error object that we load up the text for
tCIDLib::TVoid
TCIDKrnlModule::KrnlErrorPopUp( const   TKrnlError&         kerrToDisplay
                                , const tCIDLib::TCh* const pszFileName
                                , const tCIDLib::TCard4     c4Line
                                , const tCIDLib::TCh* const pszAuxMsg)
{
    //
    //  If we have not gotten far enough along to try to load text, then
    //  we just use the default message. Otherwise, load it.
    //
    const tCIDLib::TCh* pszMsg = m_pszMsgNotFound;
    if (m_bCanLoadText)
        pszMsg = pszLoadCIDFacMsg(kerrToDisplay.errcId());

    TKrnlPopup::Show
    (
        pszFileName
        , c4Line
        , m_pszPopUpTitle1
        , m_pszPopUpTitle2
        , 0
        , kerrToDisplay.errcId()
        , kerrToDisplay.errcHostId()
        , pszMsg
        , pszAuxMsg
    );
}


// Do a low level popup with just a message to display
tCIDLib::TVoid
TCIDKrnlModule::MsgPopUp(   const tCIDLib::TCh* const   pszMsg
                            , const tCIDLib::TCh* const pszFileName
                            , const tCIDLib::TCard4     c4Line)
{
    TKrnlPopup::Show
    (
        pszFileName
        , c4Line
        , m_pszPopUpTitle1
        , m_pszPopUpTitle2
        , 0
        , 0
        , 0
        , pszMsg
        , kCIDLib::pszEmptyZStr
    );
}
