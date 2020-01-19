//
// FILE NAME: CIDOrbUC_ThisFacility2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 9/29/2012
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
//  This file holds some overflow from the main file, to keep the sizes
//  more manageable. this one contains the stuff related to the name server
//  pinger/rebinder thread.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDOrbUC_ThisFacility
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  If a binding fails this many times in a row, we just remove it because
        //  it's clearly bad.
        // -----------------------------------------------------------------------
        const tCIDLib::TCard4       c4MaxBindFails = 10;


        // -----------------------------------------------------------------------
        //  If we haven't had to talk to the NS for other reasons in this period
        //  of time, we'll do an active ping.
        // -----------------------------------------------------------------------
        const tCIDLib::TEncodedTime enctPingTime(kCIDLib::enctOneSecond * 15);
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDOrbUC
//  PREFIX: oarb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDOrbUC: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Remove a binding from the rebind list
tCIDLib::TBoolean
TFacCIDOrbUC::bDeregRebindObj(  const   TString&            strNodePath
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    // We have to lock during this, since the binder thread is running
    TLocker lockrSync(&m_mtxSync);

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TNSRebindInfo& nsrbiCur = m_colList[c4Index];

        // If this is our guy, then flush it and return
        if (nsrbiCur.strFullPath().bCompareI(strNodePath))
        {
            m_colList.RemoveAt(c4Index);
            return kCIDLib::True;
        }
    }

    // Never found, either throw or return false
    if (bThrowIfNot)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcRebind_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strNodePath
        );
    }
    return kCIDLib::False;
}


//
//  Add a binding to our list for us to keep registered. We have one that
//  doesn't include the extra values, as a convenience. It just calls the
//  other version with empty strings.
//
tCIDLib::TVoid
TFacCIDOrbUC::RegRebindObj( const   TOrbObjId&  ooidSrvObject
                            , const TString&    strNodePath
                            , const TString&    strDescr)
{
    RegRebindObj
    (
        ooidSrvObject
        , strNodePath
        , strDescr
        , TString::strEmpty()
        , TString::strEmpty()
        , TString::strEmpty()
        , TString::strEmpty()
    );
}


tCIDLib::TVoid
TFacCIDOrbUC::RegRebindObj( const   TOrbObjId&  ooidSrvObject
                            , const TString&    strNodePath
                            , const TString&    strDescr
                            , const TString&    strExtra1
                            , const TString&    strExtra2
                            , const TString&    strExtra3
                            , const TString&    strExtra4)
{
    // We have to lock since the binder thread is probably running now
    TLocker lockrSync(&m_mtxSync);

    // See if this path is already in our list. This isn't legal
    TNSRebindInfo* pnsrbiFound = pnsrbiFind(strNodePath);
    if (pnsrbiFound)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcRebind_AlreadyExists
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strNodePath
        );
    }

    //
    //  Make sure its a reasonably legal path, and break out the separate
    //  parts we need to create the rebind info object.
    //
    TString strScope;
    TString strName;
    ParseNSBindingPath(strNodePath, strScope, strName);

    // Create a rebind info object
    TNSRebindInfo nsrbiTmp
    (
        strScope, ooidSrvObject, strName, strDescr, strExtra1, strExtra2, strExtra3, strExtra4
    );

    //
    //  Add it to our list. We want to insert it at the start, because that
    //  will insure the rebinder thread sees it. We sort the list by next
    //  renewal time and ones just added and not bound yet will have a zero
    //  time. So this also insures it is in sorted order without having to
    //  reset.
    //
    m_colList.InsertAt(nsrbiTmp, 0);
}


tCIDLib::TVoid TFacCIDOrbUC::StartRebinder()
{
    // Only start it if it's not already running
    if (!m_thrRebinder.bIsRunning())
        m_thrRebinder.Start();
}


// Stop our rebinder thread and clean up the rebind list
tCIDLib::TVoid TFacCIDOrbUC::StopRebinder() noexcept
{
    //
    //  Shut down the rebinder thread and flush the list
    //
    //  NOTE:   DO NOT lock or we could deadlock with the thread we are
    //          trying to stop!!
    //
    try
    {
        if (m_thrRebinder.bIsRunning())
        {
            m_thrRebinder.ReqShutdownSync(10000);
            m_thrRebinder.eWaitForDeath(2000);
            m_colList.RemoveAll();
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDOrbUC().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }

    catch(...)
    {
    }
}


//
//  This is a convenience method that applications can call to unbind a set of
//  bingings all at once. Typically it would do all of the application level ones
//  it has registered, since this would usually be done in the process of
//  shutting down.
//
//  We throw if anything anything fails. Since we indicate not to thrown for basic
//  issues like the binding not being there, anything else is likely to stop any
//  further progress.
//
tCIDLib::TVoid TFacCIDOrbUC::UnbindObjs(const tCIDLib::TStrCollect& colBindings)
{
    tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy(3000);
    colBindings.bForEach
    (
        [this, &colBindings, &orbcNS](const TString& strBinding) -> tCIDLib::TBoolean
        {
            // Probably it's in the rebinder so remove it
            bDeregRebindObj(strBinding, kCIDLib::False);
            orbcNS->RemoveBinding(strBinding, kCIDLib::False);
            return kCIDLib::True;
        }
    );
}


//
//  The caller wants to update one of the extra values for a binding. If
//  the update of the NS works, we update our local copy of the object.
//
tCIDLib::TVoid
TFacCIDOrbUC::UpdateExtraNSVal( const   TString&        strNodePath
                                , const tCIDLib::TCard4 c4Index
                                , const TString&        strNewVal)
{
    //
    //  Look up the binding and update the data. Throw if we don't find
    //  it.
    //
    //  We have to lock since the binder thread is probably running now.
    //
    {
        TLocker lockrSync(&m_mtxSync);

        // Look up the binding info for this path
        TNSRebindInfo* pnsrbiFound = pnsrbiFind(strNodePath);
        if (!pnsrbiFound)
        {
            // Doesn't seem to exist, so throw
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcRebind_NotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strNodePath
            );
        }

        if (c4Index == 1)
            pnsrbiFound->strExtra1(strNewVal);
        else if (c4Index == 2)
            pnsrbiFound->strExtra2(strNewVal);
        else if (c4Index == 3)
            pnsrbiFound->strExtra3(strNewVal);
        else if (c4Index == 4)
            pnsrbiFound->strExtra4(strNewVal);
        else
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_BadExtraValInd
                , strNodePath
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Index
                , TCardinal(c4Index)
            );
        }
    }

    //
    //  OK our info is updated, so let's try to get the name server updated
    //  immediately so that this change is visible. If not, then it'll be
    //  set the next time we can rebind, so we don't throw. Presumably if
    //  we can't contact the name server, then no one else can because it's
    //  down.
    //
    try
    {
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy(5000);
        orbcNS->UpdateExtraVal(strNodePath, c4Index, strNewVal);
    }

    catch(TError& errToCatch)
    {
        if (facCIDOrbUC().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}


// ---------------------------------------------------------------------------
//  TFacCIDOrbUC: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We assume that the caller has locked if required. In the case where we
//  need to do a full rebind, we can do that a lot more efficiently than
//  the normal case. We can just directly pass our binding list in to be
//  updated.
//
//  In this case, we get the cookie first, since we want to make sure we
//  are connected. We store that and know we are in sync before we done
//  the call.
//
tCIDLib::TBoolean TFacCIDOrbUC::bBindAll(tCIDOrbUC::TNSrvProxy& orbcNS)
{
    // Reset the last check time stamp
    m_enctLastNSCheck = TTime::enctNow();

    m_c8LastNSCookie = orbcNS->c8QueryCookie();
    facCIDOrb().CheckNSCookie(m_c8LastNSCookie);

    TFundVector<tCIDOrb::ERebindRes> fcolFlags(m_colList.c4ElemCount());
    if (!orbcNS->bRebindObjs(m_colList, fcolFlags, m_c8LastNSCookie))
    {
        //
        //  Pretty unlikely sine we just got the cookie, but theoretically
        //  possible.
        //
        facCIDOrb().CheckNSCookie(m_c8LastNSCookie);
        return kCIDLib::False;
    }

    // Our list and the return flags list should be the same size
    CIDAssert
    (
        m_colList.c4ElemCount() == fcolFlags.c4ElemCount()
        , L"The return binding flags list was not the right size"
    );

    //
    //  Go through the list and update them all for the next time. For those
    //  that worked, make sure the initial binding flag is set. For those
    //  that failed, we make sure we try a full one next time. For those
    //  that failed becasue they were non-terminals, remove them because
    //  they never will work.
    //
    tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        TNSRebindInfo& nsrbiCur = m_colList[c4Index];
        if (fcolFlags[c4Index] == tCIDOrb::ERebindRes::Success)
        {
            nsrbiCur.SetNextRenewal();
            c4Index++;
        }
         else if (fcolFlags[c4Index] == tCIDOrb::ERebindRes::Exception)
        {
            if (nsrbiCur.c4IncErrCount() > CIDOrbUC_ThisFacility::c4MaxBindFails)
            {
                // It's failed too many times. Remove it
                LogRemoved(nsrbiCur.strFullPath(), kOrbUCErrs::errcRebind_MaxFails);
                m_colList.RemoveAt(c4Index);
                c4Count--;
            }
             else
            {
                nsrbiCur.SetNotBound();
                c4Index++;
            }
        }
         else if (fcolFlags[c4Index] == tCIDOrb::ERebindRes::NotATerminal)
        {
            // It's not a terminal so clearly wrong. Remove it
            LogRemoved(nsrbiCur.strFullPath(), kOrbUCErrs::errcRebind_NotATerminal);
            m_colList.RemoveAt(c4Index);
            c4Count--;
        }
         else
        {
            CIDAssert2(L"Unknown bind result enum");
        }
    }
    return kCIDLib::True;
}



//
//  We assume that the caller has locked if required. This guy takes a list of
//  rebind objects that need to be rebound, and does them.
//
tCIDLib::TBoolean
TFacCIDOrbUC::bDoBindingPass(tCIDOrbUC::TNSrvProxy& orbcNS, tCIDLib::TBoolean& bChanges)
{
    // Assume no changes until proven otherwise
    bChanges = kCIDLib::False;

    //
    //  Search for any bindings that need a rebind. They have either not
    //  gotten their initial binding or they failed their last renewal.
    //  Remember their original indices for below so we can get back to
    //  the originals.
    //
    TBindList           colToBind;
    tCIDLib::TCardList  fcolOrgInd;

    tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TNSRebindInfo& nsrbiCur = m_colList[c4Index];
        if (!nsrbiCur.bIsBound())
        {
            TNSRebindInfo& nsrbiNew = colToBind.objAdd(nsrbiCur);
            fcolOrgInd.c4AddElement(c4Index);
        }
    }

    // If we didn't find any, then we are done
    if (colToBind.bIsEmpty())
        return kCIDLib::True;

    // Reset the last check time stamp
    m_enctLastNSCheck = TTime::enctNow();

    //
    //  Try to rebind the list we got. If we get back false, then our cookie
    //  is out of date, so we return false and a full binding pass will be
    //  done. None of these binding we requested will have been done.
    //
    TFundVector<tCIDOrb::ERebindRes> fcolFlags(colToBind.c4ElemCount());
    if (!orbcNS->bRebindObjs(colToBind, fcolFlags, m_c8LastNSCookie))
    {
        //
        //  We are out of sync, so return false. Store the new cookie on
        //  the ORB facility.
        //
        facCIDOrb().CheckNSCookie(m_c8LastNSCookie);
        return kCIDLib::False;
    }

    // Our list and the return flags list should be the same size
    CIDAssert
    (
        colToBind.c4ElemCount() == fcolFlags.c4ElemCount()
        , L"The return binding flags list was not the right size"
    );

    // Indicate we are making changes
    bChanges = kCIDLib::True;


    //
    //  Go through the list and update them all for the next time. For those
    //  that worked, make sure the initial binding flag is set. For those
    //  that failed, force another rebind next time. For those that were
    //  not terminals, they will never work so we have to remove them.
    //
    //  Since removal would invalidate the original indices we stored above,
    //  we do this loop in reverse index order. So any we remove are beyond
    //  the indices we have yet to process. We know we stored them in
    //  ascending order into the original indices list above.
    //
    tCIDLib::TInt4 i4Index = tCIDLib::TInt4(colToBind.c4ElemCount() - 1);
    while (i4Index >= 0)
    {
        const tCIDLib::TCard4 c4OrgInd = fcolOrgInd[i4Index];
        TNSRebindInfo& nsrbiCur = m_colList[c4OrgInd];

        // If we remove, we do that at the original index
        if (fcolFlags[i4Index] == tCIDOrb::ERebindRes::Success)
        {
            nsrbiCur.SetNextRenewal();
        }
         else if (fcolFlags[i4Index] == tCIDOrb::ERebindRes::Exception)
        {
            if (nsrbiCur.c4IncErrCount() > CIDOrbUC_ThisFacility::c4MaxBindFails)
            {
                // It's failed too many times. Remove it
                LogRemoved(nsrbiCur.strFullPath(), kOrbUCErrs::errcRebind_MaxFails);
                m_colList.RemoveAt(c4OrgInd);
            }
             else
            {
                nsrbiCur.SetNotBound();
            }
        }
         else if (fcolFlags[i4Index] == tCIDOrb::ERebindRes::NotATerminal)
        {
            // It's not a terminal so clearly wrong. Remove it
            LogRemoved(nsrbiCur.strFullPath(), kOrbUCErrs::errcRebind_NotATerminal);
            m_colList.RemoveAt(c4OrgInd);
        }
         else
        {
            CIDAssert2(L"Unknown bind result enum");
        }

        // Doesn't matter if we updated or removed, just move down
        i4Index--;
    }
    return kCIDLib::True;
}



//
//  We assume that caller has locked if required. We run through all the
//  bindings and look for any ready for renewal, but which have their
//  initial binding done, i.e. they are eligible and ready for a lease
//  renewal. It renews them if so.
//
tCIDLib::TBoolean
TFacCIDOrbUC::bDoLeaseRenewal(  tCIDOrbUC::TNSrvProxy&  orbcNS
                                , tCIDLib::TBoolean&    bChanges)
{
    // Assume no changes until proven otherwise
    bChanges = kCIDLib::False;

    tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();

    // Remember any paths that need a renewal
    tCIDLib::TStrList           colToDo;
    tCIDLib::TCardList          fcolOrgInd;
    const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TNSRebindInfo& nsrbiCur = m_colList[c4Index];

        //
        //  If we are ready for a refresh, add it. If it's not been bound,
        //  then it won't show up as ready for refresh. That's handled in
        //  a rebind pass.
        //
        if (nsrbiCur.bNeedsRefresh(enctNow))
        {
            fcolOrgInd.c4AddElement(c4Index);
            colToDo.objAdd(nsrbiCur.strFullPath());
        }
    }

    // If nothing to do, then we are done
    if (colToDo.bIsEmpty())
        return kCIDLib::True;

    // Reset the last check time stamp
    m_enctLastNSCheck = TTime::enctNow();

    //
    //  If he tells us the cookie has changed, we need to force
    //  all of them to rebind next time.
    //
    tCIDLib::TBoolList fcolFlags(colToDo.c4ElemCount());
    if (!orbcNS->bRenewLeases(colToDo, fcolFlags, m_c8LastNSCookie))
    {
        facCIDOrb().CheckNSCookie(m_c8LastNSCookie);
        return kCIDLib::False;
    }

    // Indicate we probably made changes
    bChanges = kCIDLib::True;

    //
    //  Go through the list and update them all for the next time. For those
    //  that worked, make sure the initial binding flag is set. For those
    //  that failed, force another rebind next time.
    //
    c4Count = colToDo.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TNSRebindInfo& nsrbiCur = m_colList[fcolOrgInd[c4Index]];
        if (fcolFlags[c4Index])
            nsrbiCur.SetNextRenewal();
        else
            nsrbiCur.SetNotBound();
    }
    return kCIDLib::True;
}


tCIDLib::EExitCodes
TFacCIDOrbUC::eBinderThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    //
    //  Start with our last name serve cookie zeroed, so that we'll do an
    //  initial full rebind.
    //
    m_c8LastNSCookie = 0;

    // Initialize the last NS contact time stamp to now
    m_enctLastNSCheck = TTime::enctNow();

    //
    //  And now loop till asked to stop.
    //
    tCIDLib::TCard4 c4SleepTime = 0;
    while (kCIDLib::True)
    {
        //
        //  Sleep for a bit. Break out if asked to shutdown while sleeping.
        //  It's a bit of a kludge but we don't want to sleep the first time
        //  through, but it's not convenient to have the sleep at the end
        //  of the loop. So we use a variable to hold the sleep time and
        //  skip it the first time through.
        //
        if (c4SleepTime)
        {
            if (!thrThis.bSleep(c4SleepTime))
                break;
        }
         else
        {
            c4SleepTime = 2500;
        }

        try
        {
            // Now we have to lock
            TLocker lockrSync(&m_mtxSync);

            tCIDOrbUC::TNSrvProxy orbcNS;

            //
            //  If our last cookie isn't the same as the one last set on
            //  the CIDOrb facility (various things will update it any
            //  time they have to go to the name server, and we do as well),
            //  the clearly we need to do a full sync, so just force our
            //  cookie to zero.
            //
            if (m_c8LastNSCookie != facCIDOrb().c8LastNSCookie())
                m_c8LastNSCookie = 0;

            //
            //  If we haven't tried to talk to the name server in a while,
            //  since there's been nothing to do, then actively ping it and
            //  make sure we are still in sync. If not, we'll reset our
            //  last cookie, to force a full rebind.
            //
            tCIDLib::TEncodedTime enctCur = TTime::enctNow();
            if ((enctCur - m_enctLastNSCheck) >= CIDOrbUC_ThisFacility::enctPingTime)
            {
                // Reset the time now
                m_enctLastNSCheck = enctCur;

                //
                //  Do a cookie check. If this fails, it'll throw and we'll
                //  go back into full rebind mode. If the new cookie doesn't
                //  equal ours, then we are out of sync and will force a
                //  full rebind below. If we are still in sync, then do
                //  nothing, just go back to the top.
                //
                orbcNS = facCIDOrbUC().orbcNameSrvProxy();
                const tCIDLib::TCard8 c8NewCookie = orbcNS->c8QueryCookie();
                facCIDOrb().CheckNSCookie(c8NewCookie);

                if (c8NewCookie == m_c8LastNSCookie)
                    continue;
                m_c8LastNSCookie = 0;
            }

            //
            //  If we aren't be forced to do anything, then see if there's
            //  any particular need to do anything this time around. If
            //  the list is empty, or there's nothing to do, then just go
            //  back to the top. The list is sorted, so if the 0th one
            //  doesn't need any work, none do.
            //
            if (m_c8LastNSCookie)
            {
                if (m_colList.bIsEmpty() || !m_colList[0].bNeedsWork())
                    continue;
            }

            //
            //  There's work to do, so now it's worth trying to create the
            //  name server proxy. We may have created it above in the
            //  cookie
            //
            if (!orbcNS.pobjData())
                orbcNS = facCIDOrbUC().orbcNameSrvProxy();

            tCIDLib::TBoolean bRenChanges = kCIDLib::False;
            tCIDLib::TBoolean bBindChanges = kCIDLib::False;
            if (m_c8LastNSCookie)
            {
                //
                //  Everything is normal, so we do a renewal pass and then a
                //  binding pass. Doing it this way insures that if any failures
                //  occur during the renweal, we'll pick them up in a
                //  subsequent binding pass. If no renewals are required, we'll
                //  just look for any initial bindings required.
                //
                //  If either of them idicate the cookie is out of date, zero it
                //  so that we have to do a full rebind next time.
                //
                if (!bDoLeaseRenewal(orbcNS, bRenChanges)
                ||  !bDoBindingPass(orbcNS, bBindChanges))
                {
                    m_c8LastNSCookie = 0;
                }
            }
             else
            {
                //
                //  Do a rebind of all of them. This always makes changes as
                //  long as the cookie isn't out of date.
                //
                if (bBindAll(orbcNS))
                    bBindChanges = kCIDLib::True;
                else
                    m_c8LastNSCookie = 0;
            }

            // Resort the list now if any changes were made
            if (bRenChanges || bBindChanges)
                m_colList.Sort(&TNSRebindInfo::eComp);
        }

        //
        //  If it fails, force a full update next time. And reset the
        //  last check time stamp since we are going to get back into
        //  sync.
        //
        catch(TError& errToCatch)
        {
            if (facCIDOrbUC().bLogWarnings() && !errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
            }

            m_enctLastNSCheck = TTime::enctNow();
            m_c8LastNSCookie = 0;
            continue;
        }

        catch(...)
        {
            m_enctLastNSCheck = TTime::enctNow();
            m_c8LastNSCookie = 0;
            continue;
        }
    }

    // Make sure the list is cleared
    TLocker lockrSync(&m_mtxSync);
    m_colList.RemoveAll();

    return tCIDLib::EExitCodes::Normal;
}


//
//  A helper to log a message if a rebind object is removed by us
//  because of failures. The passed error code has a 1 token that will
//  be replaced by the provided binding path.
//
tCIDLib::TVoid
TFacCIDOrbUC::LogRemoved(const  TString&            strPath
                        , const tCIDLib::TErrCode   errcToLog)
{
    facCIDOrbUC().LogMsg
    (
        CID_FILE
        , CID_LINE
        , errcToLog
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
        , strPath
    );
}



//
//  We look for the indicated path in our list and return a pointer to
//  the one found, or null if not.
//
//  We assume the caller has already locked if that is required
//
TNSRebindInfo* TFacCIDOrbUC::pnsrbiFind(const TString& strToFind)
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TNSRebindInfo& nsrbiCur = m_colList[c4Index];

        if (nsrbiCur.strFullPath().bCompareI(strToFind))
            return &nsrbiCur;
    }

    // Never found it
    return nullptr;
}


