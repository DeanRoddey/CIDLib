//
// FILE NAME: CIDLib_DirIter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/2018
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the directory contents change notification support.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TDirChangeInfo, TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDLib_DirChange
{
    //
    //  The max changes we will accumulate at a time. If the client doesn't read
    //  them before then, we post an outof sync error.
    //
    const tCIDLib::TCard4 c4MaxChangeBlock = 4096;
}



// ---------------------------------------------------------------------------
//   CLASS: TDirChangeInfo
//  PREFIX: dchi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDirChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TDirChangeInfo::TDirChangeInfo() :

    m_eChange(tCIDLib::EDirChanges::None)
{
}

TDirChangeInfo::TDirChangeInfo(const tCIDLib::EDirChanges  eChange) :

    m_eChange(eChange)
{
}

TDirChangeInfo::TDirChangeInfo(const TDirChangeInfo& dchiSrc) :

    m_eChange(dchiSrc.m_eChange)
    , m_strName(dchiSrc.m_strName)
    , m_strNew(dchiSrc.m_strNew)
{
}

TDirChangeInfo::~TDirChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TDirChangeInfo: Public operator
// ---------------------------------------------------------------------------
TDirChangeInfo& TDirChangeInfo::operator=(const TDirChangeInfo& dchiSrc)
{
    if (&dchiSrc != this)
    {
        m_eChange   = dchiSrc.m_eChange;
        m_strName   = dchiSrc.m_strName;
        m_strNew    = dchiSrc.m_strNew;
    }
    return *this;
}




// ---------------------------------------------------------------------------
//   CLASS: MDirChangeMon
//  PREFIX: mdchm
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  MDirChangeMon: Constructors and Destructor
// -------------------------------------------------------------------
MDirChangeMon::MDirChangeMon() :

    m_thrMonitor
    (
        facCIDLib().strNextThreadName(L"MDirChangeMon")
        , TMemberFunc<MDirChangeMon>(this, &MDirChangeMon::eMonThread)
    )
{
}

MDirChangeMon::~MDirChangeMon()
{
    // Make sure we get stopped
    try
    {
        StopDirMonitor();
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


// -------------------------------------------------------------------
//  MDirChangeMon: Public, non-virtual methods
// -------------------------------------------------------------------

// This is out of line since we need to throw if we are not set up
const TString& MDirChangeMon::strDirChangePath() const
{
    if (!m_kdchmImpl.bIsReady())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDirCh_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return m_strDirChangePath;
}


// -------------------------------------------------------------------
//  MDirChangeMon: Protected, non-virtual methods
// -------------------------------------------------------------------

tCIDLib::TVoid
MDirChangeMon::StartDirMonitor( const   TString&                strTar
                                , const tCIDLib::EDirChFilters  eFilters
                                , const tCIDLib::TBoolean       bDoSubDirs)
{
    if (m_kdchmImpl.bIsReady())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDirCh_Already
            , m_strDirChangePath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
        );
    }

    if (!m_kdchmImpl.bStartMonitor(strTar.pszBuffer(), eFilters, bDoSubDirs))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDirCh_Start
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
            , TString(m_kdchmImpl.kstrTargetDir().pszValue())
        );
    }

    // It worked so store the directory and start up the thread
    m_strDirChangePath = strTar;
    m_thrMonitor.Start();
}


tCIDLib::TVoid MDirChangeMon::StopDirMonitor()
{
    //
    //  If the thread is running, let's stop it. This is tricky because the read
    //  command is blocking on Windows. So we check to see if the thread is running.
    //  if so, then we call a stop method on the kernel object which will do a
    //  cancel on the directory handle and cause the thread to wake up. We do an
    //  async shutdown request first, so that he'll see the shutdown request before
    //  he goes back to read again and break out.
    //
    if (m_thrMonitor.bIsRunning())
    {
        m_thrMonitor.ReqShutdownNoSync();
        m_kdchmImpl.bCancel();

        try
        {
            m_thrMonitor.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcDirCh_Stop
                , m_strDirChangePath
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Term
            );
        }

        catch(...)
        {
        }
    }

    // The thread normally will do this when it exits, but just in case.
    m_kdchmImpl.bCleanup();
    m_strDirChangePath.Clear();
}



// -------------------------------------------------------------------
//  MDirChangeMon: Private, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::EExitCodes
MDirChangeMon::eMonThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // These can be just local to us since only we need to interact with them
    TKrnlDirChangeMon::TChangeList  kllstChanges;
    tCIDLib::TDirChanges            colChanges;
    TLogLimiter                     loglimLogger;
    const TError                    errFailed
    (
        facCIDLib().strName()
        , CID_FILE
        , CID_LINE
        , kCIDErrs::errcDirCh_ThreadError
        , facCIDLib().strMsg(kCIDErrs::errcDirCh_ThreadError)
        , m_strDirChangePath
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );

    while (!thrThis.bCheckShutdownRequest())
    {
        try
        {
            // Read any current changes and get them into our form in our list
            tCIDLib::TCard4 c4Count = 0;
            if (!m_kdchmImpl.bReadChanges(kllstChanges, c4Count))
            {
                //
                //  If an out of sync error, report that to the derived class. Else
                //  throw so it gets logged below
                //
                TKrnlError kerrLast = TKrnlError::kerrLast();
                if (kerrLast.errcId() == kKrnlErrs::errcGen_OutOfSync)
                {
                    DirChangeOutOfSync();
                }
                 else
                {
                    facCIDLib().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcDirCh_Read
                        , kerrLast
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                        , m_strDirChangePath
                    );
                }
            }

            // If any are valid
            if (c4Count)
            {
                // Reset the list so we can iterate it
                kllstChanges.bResetCursor();

                // Convert the kernel level objects to ours
                const tCIDLib::TCard4 c4OrgCount = colChanges.c4ElemCount();
                TKrnlDirChangeInfo* pkdchiCur = nullptr;
                TDirChangeInfo* pdchiCur = nullptr;
                TDirChangeInfo dchiEmpty;
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    //
                    //  If false this is bad, we were told how many were good
                    //  ones in the list
                    //
                    if (kllstChanges.bNext(pkdchiCur))
                    {
                        //
                        //  If we haven't used all the originals yet, then take
                        //  one of those, else add a new one.
                        //
                        if (c4Index < c4OrgCount)
                            pdchiCur = &colChanges[c4Index];
                        else
                            pdchiCur = &colChanges.objAdd(dchiEmpty);

                        pdchiCur->m_eChange = pkdchiCur->m_eChange;
                        pdchiCur->m_strName = pkdchiCur->m_kstrName.pszValue();
                        pdchiCur->m_strNew = pkdchiCur->m_kstrNew.pszValue();
                    }
                     else
                    {
                        CIDAssert2
                        (
                            L"Failed to get next expected dir change event from list"
                        )
                    }
                }

                // Let the containing class handle these, break out if asked
                if (!bProcessDirChanges(colChanges, c4Count))
                    break;
            }

            // Sleep a bit, break out if asked to stop
            if (!thrThis.bSleep(250))
                break;
        }

        catch(TError& errToCatch)
        {
            if (loglimLogger.bLogErr(errToCatch, CID_FILE, CID_LINE))
                TModule::LogEventObj(errFailed);
        }

        catch(...)
        {
            loglimLogger.bLogErr(errFailed);
        }
    }

    // Clean up our kernel object
    if (!m_kdchmImpl.bCleanup())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDirCh_Stop
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Shutdown
            , m_strDirChangePath
        );
    }

    return tCIDLib::EExitCodes::Normal;
}





// ---------------------------------------------------------------------------
//   CLASS: TDirChangeMon
//  PREFIX: dchm
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TDirChangeMon: Constructors and Destructor
// -------------------------------------------------------------------
TDirChangeMon::TDirChangeMon() :

    m_bOverflow(kCIDLib::False)
    , m_colChanges(1024)
{

}

TDirChangeMon::~TDirChangeMon()
{
    // Make sure we are stopped
    try
    {
        Stop();
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


// -------------------------------------------------------------------
//  TDirChangeMon: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TDirChangeMon::bReadChanges(tCIDLib::TDirChanges& colToFill)
{
    colToFill.RemoveAll();

    // We need to sync this
    TMtxLocker mtxlSync(&m_mtxSync);

    //
    //  If in overflow state, clear that. There will be an out of sync event
    //  in the list to return to the caller if so. He's been warned so our
    //  obligations are fulfilled.
    //
    m_bOverflow = kCIDLib::False;

    const tCIDLib::TCard4 c4Count = m_colChanges.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        colToFill.objAdd(m_colChanges[c4Index]);

    // Clear our list now for the next round
    m_colChanges.RemoveAll();

    // Return true if we are returning events
    return !colToFill.bIsEmpty();
}


tCIDLib::TVoid
TDirChangeMon::Start(const  TString&                strTargetDir
                    , const tCIDLib::EDirChFilters  eFilters
                    , const tCIDLib::TBoolean       bDoSubDirs)
{
    // Just pass through to the mixin for now
    StartDirMonitor(strTargetDir, eFilters, bDoSubDirs);
}


tCIDLib::TVoid TDirChangeMon::Stop()
{
    // Just pass through to the mixin for now
    StopDirMonitor();
}


// -------------------------------------------------------------------
//  TDirChangeMon: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TBoolean
TDirChangeMon::bProcessDirChanges(  const   tCIDLib::TDirChanges&   colNewChanges
                                    , const tCIDLib::TCard4         c4Count)
{
    TMtxLocker mtxlSync(&m_mtxSync);

    // IF the overflow flag is already on, just ignore these
    if (m_bOverflow)
        return kCIDLib::True;

    //
    //  See if we can take these new changes. If not, set the overflow flag and
    //  add an out of sync event to the list to be read by the client code.
    //
    if ((m_colChanges.c4ElemCount() + c4Count) > CIDLib_DirChange::c4MaxChangeBlock)
    {
        m_colChanges.RemoveAll();
        m_colChanges.objAdd(TDirChangeInfo(tCIDLib::EDirChanges::OutOfSync));
        m_bOverflow = kCIDLib::True;

        return kCIDLib::True;
    }

    // Else copy over the new stuff
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colChanges.objAdd(colNewChanges[c4Index]);

    return kCIDLib::True;
}


//
//  Somehow our thread isn't reading from the system fast enough so his buffer
//  overlowed. So we clear our list set the overflow flag and put an out of sync
//  event into the list, if not already in overflow mode.
//
tCIDLib::TVoid TDirChangeMon::DirChangeOutOfSync()
{
    TMtxLocker mtxlSync(&m_mtxSync);
    if (!m_bOverflow)
    {
        m_colChanges.RemoveAll();
        m_colChanges.objAdd(TDirChangeInfo(tCIDLib::EDirChanges::OutOfSync));
        m_bOverflow = kCIDLib::True;
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TDirChangePub
//  PREFIX: dchm
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  Constructors and destructor
// -------------------------------------------------------------------
TDirChangePub::TDirChangePub()
{
}

TDirChangePub::~TDirChangePub()
{
    // Stop us if not stopped already
    try
    {
        Stop();
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


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TVoid
TDirChangePub::Start(const  TString&                strTargetDir
                    , const tCIDLib::EDirChFilters  eFilters
                    , const tCIDLib::TBoolean       bDoSubDirs
                    , const TString&                strPubTopic)
{
    // Try to create the topic if not already
    if (!TPubSubTopic::bTopicExists(strPubTopic))
        m_pstopChanges = TPubSubTopic::pstopCreateTopic(strPubTopic, TDirChangeInfo::clsThis());

    // And let the mixin start up
    try
    {
        StartDirMonitor(strTargetDir, eFilters, bDoSubDirs);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        // Drop the topic since we were not successful
        m_pstopChanges.DropTopic();
        throw;
    }

    catch(...)
    {
        // Drop the topic since we were not successful
        m_pstopChanges.DropTopic();
        throw;
    }
}


tCIDLib::TVoid TDirChangePub::Stop()
{
    // Let the mixin clean up
    try
    {
        StopDirMonitor();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // And drop the topic
    m_pstopChanges.DropTopic();

}


// -------------------------------------------------------------------
//  TDirChangeMon: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TBoolean
TDirChangePub::bProcessDirChanges(  const   tCIDLib::TDirChanges&   colNewChanges
                                    , const tCIDLib::TCard4         c4Count)
{
    // Publish these changes
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_pstopChanges.Publish(new TDirChangeInfo(colNewChanges[c4Index]));

    return kCIDLib::True;
}


//
//  Somehow our thread isn't reading from the system fast enough so his buffer
//  overlowed. So we publish an out of sync error.
//
tCIDLib::TVoid TDirChangePub::DirChangeOutOfSync()
{
    m_pstopChanges.Publish(new TDirChangeInfo(tCIDLib::EDirChanges::OutOfSync));
}

