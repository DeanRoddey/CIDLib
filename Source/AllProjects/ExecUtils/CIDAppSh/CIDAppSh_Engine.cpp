//
// FILE NAME: CIDAppSh_Engine.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
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
//  This file implements the TAppShEngine class.
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
#include    "CIDAppSh_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAppShEngine,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TAppShEngine
//  PREFIX: aseng
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAppShEngine: Constructors and Destructor
// ---------------------------------------------------------------------------
TAppShEngine::TAppShEngine() :

    m_colRecs(tCIDLib::EAdoptOpts::Adopt)
    , m_thrMonitor
      (
        facCIDLib().strNextThreadName(L"CIDAppShell")
        , TMemberFunc<TAppShEngine>(this, &TAppShEngine::eMonThread)
      )
{
    // Start the thread
    m_thrMonitor.Start();
}

TAppShEngine::~TAppShEngine()
{
    // Shut down our monitor thread if its running
    if (m_thrMonitor.bIsRunning())
    {
        try
        {
            m_thrMonitor.ReqShutdownSync();
            m_thrMonitor.eWaitForDeath(2000);
        }

        catch(...)
        {
        }
    }

    // Try to shut down any processes that are running and flush the list
    try
    {
        if (!m_colRecs.bIsEmpty())
        {
            StopAll(30000);
            m_colRecs.RemoveAll();
        }
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TAppShEngine: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TAppShEngine::ActivateAll()
{
    //
    //  Lock the mutex, then iterate the records and, for any that are marked
    //  inactive, set them to 'wait run' status.
    //
    TMtxLocker lockSync(&m_mtxSync);

    // Send a message about this
    ShellMessage(facCIDAppSh().strMsg(kAppShMsgs::midStatus_ActivatingAll));

    const tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAppShRecord* pasrCur = m_colRecs[c4Index];
        if (pasrCur->eStatus() == tCIDAppSh::EStatus::Inactive)
            pasrCur->eStatus(tCIDAppSh::EStatus::WaitRun);
    }
}


tCIDLib::TVoid TAppShEngine::AddApp(TAppShRecord* const pasrToAdd)
{
    // Lock the mutex while we update the collection
    TMtxLocker lockSync(&m_mtxSync);

    //
    //  Put it into the list, sorted by app level. Within a level the order
    //  doesn't matter, so just find an app of the same or greater level and
    //  insert before that.
    //
    tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        if (m_colRecs[c4Index]->eLevel() >= pasrToAdd->eLevel())
            break;
    }

    // Either insert, or add if this is the new highest level
    if (c4Index == c4Count)
        m_colRecs.Add(pasrToAdd);
    else
        m_colRecs.InsertAt(pasrToAdd, c4Index);

    // Send a message about this
    ShellMessage
    (
        facCIDAppSh().strMsg
        (
            kAppShMsgs::midStatus_AddingApp, pasrToAdd->strTitle()
        )
    );

    // If it's current status is 'wait run, try an intial start
    if (pasrToAdd->eStatus() == tCIDAppSh::EStatus::WaitRun)
    {
        try
        {
            if (pasrToAdd->bStart())
            {
                // It worked so report to the derived class and mark it running
                pasrToAdd->eStatus(tCIDAppSh::EStatus::Running);
                Started(*pasrToAdd);
            }
        }

        catch(TError& errToCatch)
        {
            ShellException(errToCatch, *pasrToAdd);
        }
    }
}


tCIDLib::TCard4
TAppShEngine::c4QueryAppsInfo(tCIDLib::TKVPList& colToFill)
{
    // Lock while we do this, since the monitor thread stays active
    TMtxLocker lockSync(&m_mtxSync);

    tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAppShRecord* const pasrCur = m_colRecs[c4Index];
        colToFill.objPlace(pasrCur->strApp(), pasrCur->strParms());
    }
    return c4Count;
}


tCIDLib::TVoid TAppShEngine::Cycle(const tCIDLib::TCard4 c4WaitFor)
{
    // Lock while we do this, since the monitor thread stays active
    TMtxLocker lockSync(&m_mtxSync);

    // Send a message about this
    ShellMessage(facCIDAppSh().strMsg(kAppShMsgs::midStatus_CycleAll));

    //
    //  Call the helper method to shut down all of our processes. Tell it
    //  not to mark them inactive, so they'll just get started back up again
    //  by the monitor thread, effectively cycling them.
    //
    ShutdownAllProcesses(c4WaitFor, kCIDLib::False);
}


tCIDLib::TVoid TAppShEngine::StartAll()
{
    // Lock while we do this, since the monitor thread stays active
    TMtxLocker lockSync(&m_mtxSync);

    // Send a message about this
    ShellMessage(facCIDAppSh().strMsg(kAppShMsgs::midStatus_StartingAll));

    //
    //  Run through all of the records and, for any that are marked inactive,
    //  mark them waiting to run so that the monitor thread will start them.
    //
    tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAppShRecord* const pasrCur = m_colRecs[c4Index];
        if (pasrCur->eStatus() == tCIDAppSh::EStatus::Inactive)
            pasrCur->eStatus(tCIDAppSh::EStatus::WaitRun);
    }
}


tCIDLib::TVoid TAppShEngine::StopAll(const tCIDLib::TCard4 c4WaitFor)
{
    // Lock while we do this, since the monitor thread stays active
    TMtxLocker lockSync(&m_mtxSync);

    // Send a message about this
    ShellMessage(facCIDAppSh().strMsg(kAppShMsgs::midStatus_StoppingAll));

    //
    //  Call the helper method to shut down all of our processes. Tell it
    //  to mark them as inactive so that the monitor thread won't try to
    //  start them back up again.
    //
    ShutdownAllProcesses(c4WaitFor, kCIDLib::True);
}


tCIDLib::TVoid TAppShEngine::StopEngine(const tCIDLib::TCard4 c4WaitFor)
{
    // Shut down our monitor thread if its running
    if (m_thrMonitor.bIsRunning())
    {
        m_thrMonitor.ReqShutdownSync();
        m_thrMonitor.eWaitForDeath(5000);
    }

    // Call the helper method to shut down all of our processes
    ShutdownAllProcesses(c4WaitFor, kCIDLib::True);

    // And flush the list, so they will not be restarted
    m_colRecs.RemoveAll();
}


// ---------------------------------------------------------------------------
//  TAppShEngine: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  After a call to bStartShutdown, this will be called periodically to
//  check whether the shutdown has completed. If the app is ended, it should
//  return true and set the eToFil parm with the exit code. Else, it should
//  return false.
//
tCIDLib::TBoolean
TAppShEngine::bCheckShutdownComplete(TAppShRecord&, tCIDLib::EExitCodes&)
{
    return kCIDLib::False;
}


//
//  This is called to let the derived class know that an app has exited for
//  some reason. Our default impl does nothing.
//
tCIDLib::TVoid TAppShEngine::Exited(TAppShRecord&, const tCIDLib::EExitCodes)
{
}


//
//  This is called while waiting for apps to shutdown, so that the derived
//  class can do needed things. In particular, it allows a service based
//  app shell to tell the service manage to wait for it to finish.
//
tCIDLib::TVoid TAppShEngine::IdleCallback()
{
}


tCIDLib::TVoid TAppShEngine::Restarted(TAppShRecord&)
{
    //
    //  This is just a means to report restarts to the derived class. Our
    //  default impl does nothing.
    //
}


//
//  This is just a means to report exceptions that occur when managing
//  process. Our default impl does nothing.
//
tCIDLib::TVoid TAppShEngine::ShellException(TError&, TAppShRecord&)
{
}


//
//  This is just a means to send status messages to the derived class, such
//  "starting stop all" and that kind of thing, to give some context to the
//  start/stop messages that the user will see. Our default impl does nothing.
//
tCIDLib::TVoid TAppShEngine::ShellMessage(const TString&)
{
}


//
//  This is just a means to report exceptions that occur when managing
//  processes. Our default impl does nothing.
//
tCIDLib::TVoid TAppShEngine::ShellUnknownException(TAppShRecord&)
{
}


tCIDLib::TVoid TAppShEngine::Started(TAppShRecord&)
{
    //
    //  This is just a means to report starts to the derived class. Our
    //  default impl does nothing.
    //
}


//
//  We have no clue how this process might be convinced to shut down. Derived
//  classes that know something about the processes being managed should
//  override this. We return false to indicate we didn't do anything. This
//  API only starts the shutdown, and should return immediately. The
//  bCheckShutdownComplete() method above will then be called periodically
//  to ask if it is complete.
//
tCIDLib::TVoid TAppShEngine::StartShutdown(TAppShRecord&)
{
}


// ---------------------------------------------------------------------------
//  TAppShEngine: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TAppShEngine::eMonThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the caller go
    thrThis.Sync();

    while (kCIDLib::True)
    {
        try
        {
            //
            //  Sleep for two seconds, using the shutdown checking version. If
            //  we get interrupted by a shutdown request, then it will false
            //  and we just break out. It will have already done the sync to
            //  let the other thread know that we heard him.
            //
            if (!thrThis.bSleep(2000))
                break;

            //
            //  Lock the mutex and loop through the list of apps and check
            //  if any of them are dead.
            //
            {
                TMtxLocker lockSync(&m_mtxSync);

                // If the list is empty, go back to the top
                if (m_colRecs.bIsEmpty())
                    continue;

                //
                //  We've got some records to check, so run through them. If
                //  any are not running and not inactive, then we have to deal
                //  with them.
                //
                tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    TAppShRecord& asrCur = *m_colRecs[c4Index];

                    //
                    //  If this one isn't running and is not inactive, then we
                    //  need to do deal with it.
                    //
                    const tCIDAppSh::EStatus eCurStatus = asrCur.eStatus();
                    if (!asrCur.bIsRunning()
                    &&  (eCurStatus != tCIDAppSh::EStatus::Inactive))
                    {
                        //
                        //  If the last we knew it was running, we have to
                        //  report it as having exited.
                        //
                        tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
                        if (eCurStatus == tCIDAppSh::EStatus::Running)
                        {
                            // Set it back to wait run again
                            asrCur.eStatus(tCIDAppSh::EStatus::WaitRun);

                            try
                            {
                                eRet = asrCur.eWaitForDeath(0);
                            }

                            catch(...)
                            {
                            }
                            Exited(asrCur, eRet);
                        }

                        //
                        //  If the return was a deschedule code, then remove
                        //  this one from our list. Else, try to restart
                        //  it.
                        //
                        if (eRet == tCIDLib::EExitCodes::Deschedule)
                        {
                            c4Count--;
                            m_colRecs.RemoveAt(c4Index);
                        }
                         else
                        {
                            //
                            //  Report errors to the derived class. They can
                            //  log them as they deem necessary.
                            //
                            try
                            {
                                if (asrCur.bStart())
                                    Restarted(asrCur);
                            }

                            catch(TError& errToCatch)
                            {
                                ShellException(errToCatch, asrCur);
                            }

                            catch(...)
                            {
                                ShellUnknownException(asrCur);
                            }

                            //
                            //  Even if an error was thrown, it might be
                            //  running, so check first and set the flag if
                            //  so.
                            //
                            if (asrCur.bIsRunning())
                                asrCur.eStatus(tCIDAppSh::EStatus::Running);
                        }
                    }
                }
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDAppSh().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            if (facCIDAppSh().bLogWarnings())
            {
                facCIDAppSh().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kAppShErrs::errcEng_ExceptInMoThread
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::AppStatus
                );
            }
        }

        catch(...)
        {
            if (facCIDAppSh().bLogWarnings())
            {
                facCIDAppSh().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kAppShErrs::errcEng_ExceptInMoThread
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::AppStatus
                );
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  NO MORE apps should be added while this is being run. We assume the containing
//  program is shutting down the shell and the current number of records is not
//  going to change while we are doing this!
//
tCIDLib::TVoid
TAppShEngine::ShutdownAllProcesses( const   tCIDLib::TCard4     c4WaitFor
                                    , const tCIDLib::TBoolean   bMarkInactive)
{
    TThread& thrThis = *TThread::pthrCaller();

    //
    //  Iterate and give the derived class a chance to shut them down. This
    //  is a kind of complex operation, since we do it asychronously. On
    //  each loop through we find all of the processes of a particular
    //  level, starting high and working downwards, and ask them to start
    //  shutting down. Then we wait for them all to finish. And then move
    //  on to the next lower level. So at any one level, we can have more
    //  than one app in the process of shutting down at once.
    //
    const tCIDLib::TCard4 c4Count = m_colRecs.c4ElemCount();

    // If none, then we can return now
    if (!c4Count)
        return;

    //
    //  Make one pass through and count off all of the records that are
    //  inactive or waiting to run, since they aren't running. So we want
    //  to start the 'so far' count such that it already includes them.
    //
    tCIDLib::TCard4 c4SoFar = 0;
    tCIDLib::TCard4 c4Index = 0;
    TAppShRecord* pasrCur;
    for (; c4Index < c4Count; c4Index++)
    {
        pasrCur = m_colRecs[c4Index];
        if (pasrCur->eStatus() != tCIDAppSh::EStatus::Running)
            c4SoFar++;
    }

    // Calculate our end time
    const tCIDLib::TCard4 c4EndTime = (c4WaitFor == kCIDLib::c4MaxCard)
                                      ? kCIDLib::c4MaxCard
                                      : (TTime::c4Millis() + c4WaitFor);

    //
    //  We have some, so let's do it
    //
    //  To make this simpler, we make a run through to set up a non-adopting
    //  ref vector of app records for each level. So once we have them,
    //  we only have to deal with the apps that we know are relevant for
    //  the current level while we are waiting for them to end.
    //
    TRefVector<TAppShRecord> colCurLev(tCIDLib::EAdoptOpts::NoAdopt, c4Count);
    tCIDAppSh::EAppLevels   eCurLev = tCIDAppSh::EAppLevels::Max;
    while (c4SoFar < c4Count)
    {
        // If we get an exit signal, then exit
        if (thrThis.bCheckShutdownRequest())
            break;

        //
        //  Run through and find any at the current level. Any of them that
        //  are currently marked inactive or waiting to start, we can ignore.
        //  They were already counted above.
        //
        colCurLev.RemoveAll();
        for (c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TAppShRecord* pasrCur = m_colRecs[c4Index];

            if ((pasrCur->eStatus() == tCIDAppSh::EStatus::Running)
            &&  (pasrCur->eLevel() == eCurLev))
            {
                colCurLev.Add(pasrCur);
            }
        }

        //
        //  If we got some, deal with it. First we go through the list
        //  and ask each one to start the shutdown process.
        //
        tCIDLib::TCard4 c4LevCount = colCurLev.c4ElemCount();
        for (c4Index = 0; c4Index < c4LevCount; c4Index++)
        {
            TAppShRecord* pasrCur = colCurLev[c4Index];
            try
            {
                // Tell it so start the shutdown and reset the time
                StartShutdown(*pasrCur);
                pasrCur->StartShutdownClock();
            }

            catch(TError& errToCatch)
            {
                ShellException(errToCatch, *pasrCur);
            }

            catch(...)
            {
                ShellUnknownException(*pasrCur);
            }
        }

        //
        //  Now wait for them all to end. We keep going through until they
        //  all report being shut down. The caller should only take up to
        //  about a second to check. While waiting, we call the idle
        //  callback.
        //
        while (!colCurLev.bIsEmpty())
        {
            tCIDLib::EExitCodes eResult;

            tCIDLib::TCard4 c4LevCount = colCurLev.c4ElemCount();
            c4Index = 0;
            while (c4Index < c4LevCount)
            {
                TAppShRecord* pasrCur = colCurLev[c4Index];

                try
                {
                    // Ask it if it's done yet
                    if (bCheckShutdownComplete(*pasrCur, eResult))
                    {
                        // If asked, mark it as inactive, else waiting to run
                        if (bMarkInactive)
                            pasrCur->eStatus(tCIDAppSh::EStatus::Inactive);
                        else
                            pasrCur->eStatus(tCIDAppSh::EStatus::WaitRun);

                        // Bump the overall count of stopped proceses
                        c4SoFar++;

                        // Remove this one from the collection
                        colCurLev.RemoveAt(c4Index);
                        c4LevCount--;

                        // Report it as having exited
                        Exited(*pasrCur, eResult);
                    }
                     else
                    {
                        // Not yet, go to the next one
                        c4Index++;
                    }
                }

                catch(TError& errToCatch)
                {
                    ShellException(errToCatch, *pasrCur);

                    // Make sure we make progress
                    c4Index++;
                }

                catch(...)
                {
                    ShellUnknownException(*pasrCur);

                    // Make sure we make progress
                    c4Index++;
                }
            }

            // If we've exceeded the wait time, then throw
            if (TTime::c4Millis() > c4EndTime)
            {
                facCIDAppSh().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kAppShErrs::errcEng_ShutdownTimeout
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Timeout
                );
            }

            // Call the idle callback
            IdleCallback();
        }

        // Move to the previous level. Break out if already on the min
        if (eCurLev == tCIDAppSh::EAppLevels::Min)
            break;
        eCurLev--;
    }

    // If we didn't get all the apps, then something is wrong
    if (c4SoFar != c4Count)
    {
        // <TBD>
    }
}

