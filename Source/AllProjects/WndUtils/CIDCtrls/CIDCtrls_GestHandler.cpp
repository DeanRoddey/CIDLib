//
// FILE NAME: CIDCtrls_GestureEng.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/26/2013
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
//  This file implements the gesture engine base class.
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
#include    "CIDCtrls_.hpp"
#include    "CIDCtrls_XPGestHandler_.hpp"
#include    "CIDCtrls_Win7GestHandler_.hpp"


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDCtrls_CIDGestureEng
{
    // -----------------------------------------------------------------------
    //  The length of time we'll track a gesture before we assume it's not
    //  really a gesture.
    // -----------------------------------------------------------------------
    #if CID_DEBUG_ON
//    const tCIDLib::TEncodedTime enctMaxTime(kCIDLib::enctOneSecond * 60);
    const tCIDLib::TEncodedTime enctMaxTime(kCIDLib::enctOnePtFiveSec);
    #else
    const tCIDLib::TEncodedTime enctMaxTime(kCIDLib::enctOnePtFiveSec);
    #endif


    // -----------------------------------------------------------------------
    //  Some global init stuff that we will check for once upon the first init
    //  of a gesture engine.
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean   bGlobalInitDone = kCIDLib::False;
    tCIDLib::TFloat4    f4InertiaScaler = 1.0F;
}


// ---------------------------------------------------------------------------
//  CLASS: MCIDGestTarget
// PREFIX: mgestt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MCIDGestTarget: Constructors and Destructor
// ---------------------------------------------------------------------------
MCIDGestTarget::MCIDGestTarget()
{
}

MCIDGestTarget::~MCIDGestTarget()
{
}


// ---------------------------------------------------------------------------
//  MCIDGestTarget: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  This one is optional, so we provide a default. It just allows them to do something
//  in the idle time between the trailing inertial output events.
//
tCIDLib::TVoid
MCIDGestTarget::GestInertiaIdle(const   tCIDLib::TEncodedTime
                                , const tCIDLib::TCard4
                                , const tCIDLib::EDirs)
{
    // A no-op if not overridden
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDGestHandler
//  PREFIX: gest
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDGestHandler: Public, static methods
// ---------------------------------------------------------------------------

//
//  We get the maximum distance that is available to scroll in the direction
//  that the gesture indicated. We get the 'page size', i.e. how many pixels
//  are displayable in the direction of travel. And we get the speed factor
//  that was returned with the gesture info.
//
//  We return the actual distance to scroll. We return it as an int value,
//  though it's always possible, since the caller will often want to apply
//  it negatively as well.
//
//  We have one that works in terms of indices and another that works in
//  terms of pixels.
//
tCIDLib::TInt4
TCIDGestHandler::i4AdjustForSpeed(  const   tCIDLib::TInt4      i4MaxPels
                                    , const tCIDLib::TInt4      i4SpanPels
                                    , const tCIDLib::TFloat8    f8Speed)
{
    tCIDLib::TFloat8    f8AdjSpeed;
    tCIDLib::TInt4      i4Ret;

    if (i4MaxPels < i4SpanPels)
    {
        //
        //  We have less than a full page, so we the distance we adjust is
        //  is the max distance, and we have to adjust the speed adjustment
        //  based on how much of a full page we have.
        //
        f8AdjSpeed = f8Speed
                    + ((tCIDLib::TFloat8(i4SpanPels)
                       / tCIDLib::TFloat8(i4MaxPels)) * 0.25);
        if (f8AdjSpeed > 1.0)
            f8AdjSpeed = 1.0;

        i4Ret = i4MaxPels;
    }
     else
    {
        //
        //  At least a full page is available, so we can take the speed
        //  adjustment as is, and the value we adjust is a full span size
        //
        f8AdjSpeed = f8Speed;
        i4Ret = i4SpanPels;
    }

    //
    //  And now multiply the travel we can do by the adjusted speed factor.
    //  Generally they are passing pixels, but sometimes it's indices, which
    //  are fairly small number ranges, so we round to the nearest in order
    //  to be more accurate.
    //
    tCIDLib::TFloat8 f8Ret(i4Ret * f8AdjSpeed);
    TMathLib::Round(f8Ret);

    // And return cast to an int
    return tCIDLib::TInt4(f8Ret);
}


// ---------------------------------------------------------------------------
//  TCIDGestHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDGestHandler::TCIDGestHandler(MCIDGestTarget* const pmgesttToUse) :

    m_bTwoFingers(kCIDLib::False)
    , m_eCurState(EStates::Idle)
    , m_eDir(tCIDLib::EDirs::Count)
    , m_eGestOpts(tCIDCtrls::EGestOpts::Disable)
    , m_enctStart(0)
    , m_f4InertiaVelo(0)
    , m_f4PerGestVScale(0)
    , m_pmgesttToUse(pmgesttToUse)
{
}

TCIDGestHandler::~TCIDGestHandler()
{
}


// ---------------------------------------------------------------------------
//  TCIDGestHandler: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we have an outstanding gesture, we just need to clean up and stop the
//  gesture.
//
tCIDLib::TVoid TCIDGestHandler::CancelGesture()
{
    if ((m_eCurState == EStates::DoingPan)
    ||  (m_eCurState == EStates::DoingInertia))
    {
        // Report an end of gesture to the target window and then reset
        try
        {
            m_pntDelta.Set(0, 0);
            m_pmgesttToUse->bProcessGestEv
            (
                tCIDCtrls::EGestEvs::End
                , m_pntStartPos
                , m_pntLastPos
                , m_pntDelta
                , m_bTwoFingers
            );
            Reset(tCIDCtrls::EGestReset::Cancelled);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
            Reset(tCIDCtrls::EGestReset::Cancelled);
        }

        catch(...)
        {
            Reset(tCIDCtrls::EGestReset::Cancelled);
        }
    }
     else if (m_eCurState == EStates::DoingFlick)
    {
        // We don't have to report anything in this case
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }
}


//
//  The derived gesture engine should call here first then do its own initialization
//  processing. We will check for a scaler value that gets applied to inertia. We
//  grab the value and, if valid, we store it. This is only done once and stored.
//
tCIDLib::TVoid TCIDGestHandler::Initialize()
{
    if (!CIDCtrls_CIDGestureEng::bGlobalInitDone)
    {
        TBaseLock lockInit;
        if (!CIDCtrls_CIDGestureEng::bGlobalInitDone)
        {
            TString strVal;
            if (TProcEnvironment::bFind(L"CID_GESTSCALER", strVal))
            {
                // Convert to a floating point value
                tCIDLib::TFloat4 f4Val;
                if (strVal.bToFloat4(f4Val))
                {
                    // Clip to the legal range
                    if (f4Val < 0.5F)
                        f4Val = 0.5;
                    else if (f4Val > 1.5F)
                        f4Val = 1.5F;

                    CIDCtrls_CIDGestureEng::f4InertiaScaler = f4Val;
                }
            }

            // Set the flag last;
            CIDCtrls_CIDGestureEng::bGlobalInitDone = kCIDLib::False;
        }
    }
}


//
//  Return the original start point. Only meaningful if called between the
//  start and end event states.
//
const TPoint& TCIDGestHandler::pntStart() const
{
    return m_pntStartPos;
}


//
//  The derived gesture engine should call here first then do its own termination
//  processing.
//
tCIDLib::TVoid TCIDGestHandler::Terminate()
{
    // A no-op so far
}


// ---------------------------------------------------------------------------
//  TCIDGestHandler: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  This is called for starting, ending, and cancelled. It's virtual so it
//  goes to the derived class first, who calls us as well. We both needto
//  potentially do something different for each case.
//
tCIDLib::TVoid TCIDGestHandler::Reset(const tCIDCtrls::EGestReset eType)
{
    m_bTwoFingers = kCIDLib::False;
    m_enctStart = 0;
    m_eCurState = EStates::Idle;
    m_eDir = tCIDLib::EDirs::Count;
    m_eGestOpts = tCIDCtrls::EGestOpts::Disable;
    m_f4InertiaVelo = 0;
    m_pntLastPos.Set(0, 0);
    m_pntStartPos.Set(0, 0);

    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < kCIDCtrls::c4InertiaBufSz)
    {
        m_ac8InertiaBuf[c4Index] = 0;
        m_ai4InertiaBuf[c4Index] = 0;
        c4Index++;
    }

    // Let the gesture target know we are resetting
    m_pmgesttToUse->GestReset(eType);
}


// ---------------------------------------------------------------------------
//  TCIDGestHandler: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  When the derived class sees a click starting, he'll ask us if it's ok to let it be
//  dealt with. If we are currently in inertial mode, we'll cancel it and say no. If we
//  are currently in a gesture otherwise, we'll just say no.
//
//  Though it's a little kludgey, we cancel inertial by stting the current inertia value
//  to 0, which will cause the inertial loop to get a zero delta and finish up.
//
//  Note that we cannot allow Clicked() to just return a boolean value, since it doesn't
//  get called until a release is done in place, so that wouldn't help for cancelling
//  inertial scrolling.
//
//  And, since in non-multi-touch mode, a regular mouse click starts a gesture, this lets
//  us cancel there before even starting a faux gesture, though StartGesture() below also
//  does this same sort of thing.
//
tCIDLib::TBoolean TCIDGestHandler::bAllowClickStart()
{
    if (m_eCurState == EStates::DoingInertia)
    {
        m_f4InertiaVelo = 0;
        return kCIDLib::False;
    }
    return (m_eCurState == EStates::Idle);
}


//
//  The derived class calls us here upon start of a gesture. He gives us the
//  point where the gesture started. We reset ourself back to default state,
//  then set our state to indicate we are waiting for the first move and store
//  teh starting point.
//
//  If we are currently sending inertial messages, we'll start that cancelling
//  and return false to not start a new gesture.
//
tCIDLib::TBoolean TCIDGestHandler::bStartGesture(const TPoint& pntAt)
{
    //
    //  If we are doing inertia, then cancel that gesture so that we can
    //  start another one.
    //
    if (m_eCurState == EStates::DoingInertia)
        CancelGesture();

    // Reset and start waiting for the first move
    Reset(tCIDCtrls::EGestReset::Starting);

    m_eCurState = EStates::WaitFirst;

    // Remember the start pos, and save it as the last pos as well
    m_pntStartPos = pntAt;
    m_pntLastPos = pntAt;

    return kCIDLib::True;
}



//
//  The derived class calls this when a click occurs. Generally this is a quick
//  press/release, without moving, which differentiates it from a gesture.
//
tCIDLib::TVoid TCIDGestHandler::Clicked(const TPoint& pntAt)
{
    try
    {
        m_pmgesttToUse->GestClick(pntAt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


//
//  The derived class calls this when the current gesture ends. If we are in idle
//  mode, then this is a gesture we are ignoring so we do nothing.
//
//  If we are processing a flick, we finish off the flick processing and tell the
//  target window about the flick that we got.
//
//  If processing a pan, we send the end of gesture to the target. If we are doing an
//  inertial pan, we set up to start generating inertia msgs.
//
tCIDLib::TVoid TCIDGestHandler::EndGesture(const TPoint& pntAt)
{
    try
    {
        if (m_eCurState == EStates::Idle)
        {
            // Ignore this one
        }
         else if (m_eCurState == EStates::WaitFirst)
        {
            //
            //  Could happen if they pressed and released and never moved at
            //  all. The derived class will likely generate a click.
            //
            m_eCurState = EStates::Idle;
        }
         else if (m_eCurState == EStates::DoingFlick)
        {
            const tCIDLib::TBoolean bTimedout
            (
                TTime::enctNow()
                    >= (m_enctStart + CIDCtrls_CIDGestureEng::enctMaxTime)
            );

            // Save the direction and start pos, and reset
            const tCIDLib::EDirs eDir = m_eDir;
            const TPoint pntStart(m_pntStartPos);
            Reset(tCIDCtrls::EGestReset::Ended);

            //
            //  If we got to this point, then it's a valid flick, else we
            //  would have already cancelled the gesture processing. So we
            //  can tell the target window about the flick if we didn't
            //  timeout.
            //
            //  We tell it where the gesture started, so that it can know
            //  what to apply it to.
            //
            if (!bTimedout)
                m_pmgesttToUse->GestFlick(eDir, pntStart);
        }
         else if (m_eCurState == EStates::DoingPan)
        {
            //
            //  If this is an inertial pan, we don't send the end yet, we
            //  just enable inertia and move to that state. otherwise, we
            //  can send the end and reset.
            //
            if (m_eGestOpts == tCIDCtrls::EGestOpts::PanInertia)
            {
                // Store a last detla if any
                if (bCalcDelta(pntAt, m_pntDelta, m_pntNewPos))
                    StoreInertiaPnt(m_pntDelta);

                // And now we are doing inertia
                m_eCurState = EStates::DoingInertia;

                //
                //  Calculate the pixels per second starting point for our inerita
                //  calculation.
                //
                CalcInertialDelta();

                //
                //  Now we run a loop that generates the inertial output.
                //  We call the target back regularly to let him update.
                //
                GenerateInertia();
            }
             else
            {
                //
                //  Calc the final delta, and the new point to report, and pass
                //  it on to the target window. We do this even if there's no
                //  delta in this case, since we need to send the end no matter
                //  what.
                //
                //  There's generally no delta on this last one, but we also
                //  need to get the new position to report.
                //
                bCalcDelta(pntAt, m_pntDelta, m_pntNewPos);
                m_pmgesttToUse->bProcessGestEv
                (
                    tCIDCtrls::EGestEvs::End
                    , m_pntStartPos
                    , m_pntNewPos
                    , m_pntDelta
                    , m_bTwoFingers
                );

                // We can end this gesture now
                Reset(tCIDCtrls::EGestReset::Ended);
            }
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDCtrls().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // Be safe and cancel
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }

    catch(...)
    {
        // Be safe and cancel
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }
}


//
//  The derived class calls us here if we get gesture move. He tells us
//  the new point, and whether there are two fingers down or not.
//
//  If we are waiting for the first move, then we determine the direction
//  of the movement and ask the target window for options. If the direction
//  isn't supported, we go back to idle state and will ignore this gesture.
//  If the direction is supported, then we set up for a new gesture of the
//  type allowed, and tell the derived class that a gesture is starting.
//
//  If we are in idle mode, then this is a gesture we are ignoring so we
//  do nothing.
//
//  Else we process the move, either passing it on to the target window if
//  doing a gesture, or just processing the movement if doing a flick.
//
//  If doing a flick and the movement goes contrary to the target direction,
//  then the gesture processing is cancelled and we go back to idle mode and
//  ignore the rest. The derived class was never told anything, so we don't
//  have to call it to do any sort of cancel of the gesture.
//
tCIDLib::TVoid
TCIDGestHandler::Move(const TPoint& pntAt, const tCIDLib::TBoolean bTwoFingers)
{
    try
    {
        if (m_eCurState == EStates::Idle)
        {
            // Ignore this one
        }
         else if (m_eCurState == EStates::WaitFirst)
        {
            //
            //  Figure out the angle of movement from the start point to
            //  this first movement.
            //
            const tCIDLib::EDirs eDir = eCalcDirection(m_pntStartPos, pntAt);

            //
            //  Let the target window set options for this direction. Set it
            //  to disabled in case they don't set it.
            //
            m_eGestOpts = tCIDCtrls::EGestOpts::Disable;
            m_f4PerGestVScale = 1.0F;
            m_pmgesttToUse->PerGestOpts
            (
                pntAt, eDir, bTwoFingers, m_eGestOpts, m_f4PerGestVScale
            );

            // If no gesture accepted, then cancel
            if (m_eGestOpts == tCIDCtrls::EGestOpts::Disable)
            {
                // Not enabled for the direction we are going, so ignore
                Reset(tCIDCtrls::EGestReset::Cancelled);
                return;
            }

            // Looks like we are on, so start storing info
            m_eDir = eDir;
            m_bTwoFingers = bTwoFingers;

            // So we now are either going to do a flick, or a pan
            if (m_eGestOpts == tCIDCtrls::EGestOpts::Flick)
            {
                //
                //  Just set the state. We don't tell the window anything
                //  until the gesture ends. Store the new position as the
                //  last.
                //
                m_eCurState = EStates::DoingFlick;

                //
                //  In this case just store the new point as the last
                //  point.
                //
                m_pntLastPos = pntAt;

                // Set the start time stamp
                m_enctStart = TTime::enctNow();
            }
             else
            {
                m_eCurState = EStates::DoingPan;
                m_pntDelta.Zero();

                //
                //  Store an initial inertial delta. Doesn't matter what the delta
                //  is in this case, since it's to provide an initial time stamp.
                //
                if (m_eGestOpts == tCIDCtrls::EGestOpts::PanInertia)
                    StoreInertiaPnt(m_pntDelta);

                // Send a start event with zero delta
                m_pmgesttToUse->bProcessGestEv
                (
                    tCIDCtrls::EGestEvs::Start
                    , m_pntStartPos
                    , m_pntStartPos
                    , m_pntDelta
                    , bTwoFingers
                );

                //
                //  Now calculate a delta and send a first drag if any
                //  delta exists.
                //
                if (bCalcDelta(pntAt, m_pntDelta, m_pntNewPos))
                {
                    m_pmgesttToUse->bProcessGestEv
                    (
                        tCIDCtrls::EGestEvs::Drag
                        , m_pntStartPos
                        , m_pntNewPos
                        , m_pntDelta
                        , bTwoFingers
                    );
                    StoreInertiaPnt(m_pntDelta);
                }
            }
        }
         else if (m_eCurState == EStates::DoingFlick)
        {
            //
            //  As long as we are still moving in the target direction, and
            //  we are within the time constraint for a flick, then keep
            //  going, else cancel.
            //
            tCIDLib::TEncodedTime enctCur = TTime::enctNow();
            if (enctCur >= (m_enctStart + CIDCtrls_CIDGestureEng::enctMaxTime))
            {
                // We timed out, so cancel
                Reset(tCIDCtrls::EGestReset::Cancelled);
            }
             else
            {
                //
                //  See if we are still moving in the target direction.
                //
                const tCIDLib::TInt4 i4Delta = i4CalcDelta(pntAt);
                if ((m_eDir == tCIDLib::EDirs::Left) || (m_eDir == tCIDLib::EDirs::Up))
                {
                    //
                    //  Make sure it's either negative or if not only slightly
                    //  not so.
                    //
                    if (i4Delta > 0)
                        Reset(tCIDCtrls::EGestReset::Cancelled);
                }
                 else
                {
                    // The delta has to be positive
                    if (i4Delta < 0)
                        Reset(tCIDCtrls::EGestReset::Cancelled);
                }
            }
        }
         else if (m_eCurState == EStates::DoingPan)
        {
            //
            //  Calculate the delta and store the new point as the last point.
            //  If there was any delta, then report it.
            //
            if (bCalcDelta(pntAt, m_pntDelta, m_pntNewPos))
            {
                // Store a delta for later inertial purposes if needed
                if (m_eGestOpts == tCIDCtrls::EGestOpts::PanInertia)
                    StoreInertiaPnt(m_pntDelta);

                m_pmgesttToUse->bProcessGestEv
                (
                    tCIDCtrls::EGestEvs::Drag
                    , m_pntStartPos
                    , m_pntNewPos
                    , m_pntDelta
                    , bTwoFingers
                );
            }
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDCtrls().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        Reset(tCIDCtrls::EGestReset::Cancelled);
    }

    catch(...)
    {
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }
}


MCIDGestTarget& TCIDGestHandler::mgesttCur()
{
    CIDAssert(m_pmgesttToUse!= nullptr, L"The gesture handler mixin is not set");
    return *m_pmgesttToUse;
}


// ---------------------------------------------------------------------------
//  TCIDGestHandler: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given a new position, calculate the delta from the previous point and
//  give that back, and calculate a new position, which keeps the transverse
//  axis coordinate the same as the start position and store that as the
//  last position seen.
//
tCIDLib::TBoolean
TCIDGestHandler::bCalcDelta(const   TPoint& pntAt
                            ,       TPoint& pntDelta
                            ,       TPoint& pntNew)
{
    tCIDLib::TInt4 i4Delta;
    if (bHorizontal())
    {
        i4Delta = pntAt.i4X() - m_pntLastPos.i4X();
        pntDelta.Set(i4Delta, 0);

        pntNew.Set(pntAt.i4X(), m_pntStartPos.i4Y());
    }
     else
    {
        i4Delta = pntAt.i4Y() - m_pntLastPos.i4Y();
        pntDelta.Set(0, i4Delta);

        pntNew.Set(m_pntStartPos.i4X(), pntAt.i4Y());
    }

    // Rmember new point as the last pos
    m_pntLastPos = pntNew;

    // Return true if there was any delta
    return (i4Delta != 0);
}


// Based on the direction stored, return true if it's a horizontal direction
tCIDLib::TBoolean TCIDGestHandler::bHorizontal() const
{
    return ((m_eDir == tCIDLib::EDirs::Left) || (m_eDir == tCIDLib::EDirs::Right));
}


//
//  At the end this is called to calculate the initial inertial delta that
//  we will use to generate the remaining inertial, if inertia is enabled.
//
tCIDLib::TVoid TCIDGestHandler::CalcInertialDelta()
{
    //
    //  We had to have at least gotten two samples, since the first one is the
    //  initial one. If not, assume no movement.
    //
    if (!m_ac8InertiaBuf[0] || !m_ac8InertiaBuf[1])
    {
        m_f4InertiaVelo = 0;
        return;
    }

    //
    //  Go through all of the samples that we collected and turn the abs time
    //  stamps into times relative to the one before it. And see how many it
    //  takes to reach our cutoff. That's how many we'll consider.
    //
    const tCIDLib::TEncodedTime enctCutoff(kCIDLib::enctOneMilliSec * 150);
    tCIDLib::TCard4 c4DeltaCnt = 0;
    tCIDLib::TEncodedTime enctAccum = 0;
    while ((c4DeltaCnt < kCIDCtrls::c4InertiaBufSz - 1)
    &&     (enctAccum < enctCutoff))
    {
        // Break out if the next one is unused
        if (!m_ac8InertiaBuf[c4DeltaCnt + 1])
            break;

        // Accumulate this time
        enctAccum += m_ac8InertiaBuf[c4DeltaCnt]
                     - m_ac8InertiaBuf[c4DeltaCnt + 1];
        c4DeltaCnt++;
    }

    // Sort the ones we are keeping
    if (c4DeltaCnt > 1)
    {
        TArrayOps::TSort<tCIDLib::TInt4>
        (
            m_ai4InertiaBuf, c4DeltaCnt, tCIDLib::eComp<tCIDLib::TInt4>
        );
    }

    //
    //  Throw out the low 2 if we have more than 6. They always tend to be slow
    //  as the gesture ramps up.
    //
    tCIDLib::TCard4 c4AccumInd = 0;
    if (c4DeltaCnt > 6)
    {
        c4AccumInd += 2;
        c4DeltaCnt -= 2;
    }

    // Accumulate the distance of those we are keeping
    tCIDLib::TInt4 i4Accum = 0;
    for (; c4AccumInd < c4DeltaCnt; c4AccumInd++)
        i4Accum += m_ai4InertiaBuf[c4AccumInd];


    // Make it negative if going left or up.
    if ((m_eDir == tCIDLib::EDirs::Left) || (m_eDir == tCIDLib::EDirs::Up))
        i4Accum *= -1;

    //
    //  And divide by the count for an average, and multiply that by the scaler
    //  value.
    //
    m_f4InertiaVelo =
    (
        (tCIDLib::TFloat4(i4Accum) / tCIDLib::TFloat4(c4DeltaCnt))
        * CIDCtrls_CIDGestureEng::f4InertiaScaler
    );
}


//
//  Given the start position and the first move position, we calculate a
//  polar coordinate from the starting point. This gives us a degree slope,
//  and we use that to determine the direction of movement.
//
tCIDLib::EDirs
TCIDGestHandler::eCalcDirection(const   TPoint& pntStartPos
                                , const TPoint& pntNew)
{
    //
    //  We need to get the new point relative to the origin. We subtract
    //  the original from the new, but because our coordinate system gets
    //  larger going downwards, we flip the Y component sign, to make the
    //  angle come out in the standard way.
    //
    TPoint pntDir(pntNew - pntStartPos);
    pntDir.NegateY();

    // And now convert to polar coordinates (in degrees)
    tCIDLib::TCard4 c4Theta;
    tCIDLib::TCard4 c4Radius;
    pntDir.ToPolarDegrees(c4Theta, c4Radius);

    //
    //  And now, based on the range of the degrees, decide which direction
    //  we are going.
    //
    //  Note on the Right one, we are crossing the zero boundary, so we
    //  do an OR, not and AND. This is OK since anything else would have
    //  been caught already.
    //
    tCIDLib::EDirs eRet = tCIDLib::EDirs::Count;
    if ((c4Theta > 45) && (c4Theta < 135))
        eRet = tCIDLib::EDirs::Up;
    else if ((c4Theta >= 135) && (c4Theta < 225))
        eRet = tCIDLib::EDirs::Left;
    else if ((c4Theta >= 225) && (c4Theta < 315))
        eRet = tCIDLib::EDirs::Down;
    else if ((c4Theta >= 315) || (c4Theta <= 45))
        eRet = tCIDLib::EDirs::Right;

    return eRet;
}


//
//  When a pan gesture ends, and inertia is enabled, then this is called and we do
//  an inertia generating loop. We call the gest target with with deltas until he
//  says stop or we run out of inertia.
//
//  We also call it back between events, with a time interval it can spend doing
//  other things it needs to do. If we come back before that time has expired, we
//  wait the rest of the time.
//
tCIDLib::TVoid TCIDGestHandler::GenerateInertia()
{
    const tCIDLib::TCard4       c4IntervalMS = 16;
    const tCIDLib::TEncodedTime enctIdleDiff = kCIDLib::enctOneMilliSec * 4;

    //
    //  Set up a power value that we'll use to ramp down the effect over time.
    //
    tCIDLib::TFloat4 f4Power = 1.1F;

    //
    //  And set up the per-round value that we'll use the power value above to
    //  scale down each round. We'll then multiply this by the velocity to generate
    //  the per-round delta. Since this is less than 1, the power operation will
    //  scale it downwards each time.
    //
    tCIDLib::TFloat4 f4Round = 0.99F;

    //
    //  Use the per-gesture velocity scaling factor to adjust the round value.
    //  It's a percent value from 0 to 1.
    //
    f4Round *= m_f4PerGestVScale;

    //
    //  Figure out the direction of the inertia. We pass this to the idle callback,
    //  so that the target handler can know which direction to adjust for new
    //  content to be displayed.
    //
    if ((m_eDir == tCIDLib::EDirs::Left) || (m_eDir == tCIDLib::EDirs::Right))
    {
        // It was horizontal, so negative velo means left, else right
        if (m_f4InertiaVelo < 0)
            m_eDir = tCIDLib::EDirs::Left;
        else
            m_eDir = tCIDLib::EDirs::Right;
    }
     else
    {
        // It was vertical, so negative velo means up, else down
        if (m_f4InertiaVelo < 0)
            m_eDir = tCIDLib::EDirs::Up;
        else
            m_eDir = tCIDLib::EDirs::Down;
    }

    // Elevate our thread priority during this, because it is time sensitive
    TThreadPrioJan janPrio(tCIDLib::EPrioLevels::Highest);

    // Make sure that the unused size of the delta point is zero
    m_pntDelta.Zero();
    try
    {
        while(kCIDLib::True)
        {
            //
            //  Wait for our inter-loop interval. We give the target a chance to spend
            //  that time doing something, but will eat any remainder if needed. They
            //  will typically use it to continue to draw ahead and keep their
            //  scrollable bitmap content updated to cover the next movements.
            //
            tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(c4IntervalMS);
            m_pmgesttToUse->GestInertiaIdle(enctEnd - enctIdleDiff, c4IntervalMS - 4, m_eDir);

            // Eat any remaining time
            {
                tCIDLib::TEncodedTime enctLeft = TTime::enctNow();
                if (enctLeft < enctEnd)
                {
                    tCIDLib::TCard4 c4MSs = tCIDLib::TCard4
                    (
                        (enctEnd - enctLeft) / kCIDLib::enctOneMilliSec
                    );

                    if (c4MSs > 4)
                        facCIDCtrls().MsgYield(c4MSs - 2);
                }
            }

            //
            //  If they cancelled us during the callback, then we can stop now
            //  and send the end of gesture.
            //
            if (m_f4InertiaVelo == 0)
            {
                Reset(tCIDCtrls::EGestReset::Cancelled);
                break;
            }

            // Apply our power to the inerital velocity
            f4Round = TMathLib::f4Power(f4Round, f4Power);

            //
            //  Multiply the new scaled velocity by the calculated MSs. This will
            //  tell us now many pixels we should move for the time expired.
            //
            tCIDLib::TInt4 i4Delta = tCIDLib::TInt4(m_f4InertiaVelo * f4Round);

            // Set up the delta point with this change
            if (bHorizontal())
                m_pntDelta.i4X(i4Delta);
            else
                m_pntDelta.i4Y(i4Delta);

            // Adjust the last pos by the delta
            m_pntLastPos += m_pntDelta;

            //
            //  When the delta hits two or less, we stop, else we send the next
            //  point and delta. We have to deal with both directions here.
            //
            if ((i4Delta > -3) && (i4Delta < 3))
            {
                m_pmgesttToUse->bProcessGestEv
                (
                    tCIDCtrls::EGestEvs::End
                    , m_pntStartPos
                    , m_pntLastPos
                    , m_pntDelta
                    , m_bTwoFingers
                );

                //
                //  Reset with end an ended status and break out, as long
                //  as we haven't already been killed. Otherwise, we might
                //  cancel a new one they started, which would have cancelled
                //  the current one.
                //
                if (m_eCurState == EStates::DoingInertia)
                    Reset(tCIDCtrls::EGestReset::Ended);
                break;
            }
             else
            {
                //
                //  Send the current new info. Since it is inertial, they
                //  can tell us to stop or continue.
                //
                const tCIDLib::TBoolean bContinue = m_pmgesttToUse->bProcessGestEv
                (
                    tCIDCtrls::EGestEvs::Inertia
                    , m_pntStartPos
                    , m_pntLastPos
                    , m_pntDelta
                    , m_bTwoFingers
                );

                //
                //  If not continuing, then cancel the remaining inertial
                //  and break out. We first send an end gesture to finish it
                //  off, then we do a cancel.
                //
                if (!bContinue)
                {
                    // SEnd an end with no delta
                    m_pntDelta.Set(0, 0);
                    m_pmgesttToUse->bProcessGestEv
                    (
                        tCIDCtrls::EGestEvs::End
                        , m_pntStartPos
                        , m_pntLastPos
                        , m_pntDelta
                        , m_bTwoFingers
                    );
                    Reset(tCIDCtrls::EGestReset::Cancelled);
                    break;
                }
            }
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDCtrls().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // Be safe and cancel
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }

    catch(...)
    {
        // Be safe and cancel
        Reset(tCIDCtrls::EGestReset::Cancelled);
    }

}


//
//  For our internal use when doing flicks. Figure out the delta in the
//  direction of travel. Then store the passed point as the new last point.
//
tCIDLib::TInt4 TCIDGestHandler::i4CalcDelta(const TPoint& pntAt)
{
    tCIDLib::TInt4 i4Ret;
    if (bHorizontal())
        i4Ret = pntAt.i4X() - m_pntLastPos.i4X();
    else
        i4Ret = pntAt.i4Y() - m_pntLastPos.i4Y();

    m_pntLastPos = pntAt;
    return i4Ret;
}


//
//  For each non-inertial position reported to the window, if we are doing an
//  ineertial pan, this is called. We just store the new values away for
//  later use in calculating a speed. The position info we get is a delta from
//  the previous one.
//
tCIDLib::TVoid TCIDGestHandler::StoreInertiaPnt(const TPoint& pntNewPos)
{
    tCIDLib::TInt4 i4New;
    if (bHorizontal())
        i4New = pntNewPos.i4X();
    else
        i4New = pntNewPos.i4Y();

    if (i4New < 0)
        i4New *= -1;

    // Move the buffer items upwards. We want the newest ones at the start.
    tCIDLib::TCard4 c4Index = kCIDCtrls::c4InertiaBufSz;
    while (c4Index)
    {
        c4Index--;
        m_ac8InertiaBuf[c4Index] = m_ac8InertiaBuf[c4Index - 1];
        m_ai4InertiaBuf[c4Index] = m_ai4InertiaBuf[c4Index - 1];
    }

    // And store the new one
    m_ac8InertiaBuf[0] = TTime::enctNow();
    m_ai4InertiaBuf[0] = i4New;
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDWndGestHandler
// PREFIX: gesth
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TCIDWndGestHandler: Public, static methods
// -------------------------------------------------------------------

//
//  Gen up a gesture handler based on local capabilities, and return it. We pass it the
//  caller's desired gesture target, which he will mix into some class that he wants to
//  get gesture callbacks on. The handler doesn't own it, it just references it.
//
//  This is for the common, window based gesture application, so we return the object
//  via the window-based gesture base class. Any non-window based gesture oriented
//  applications will have to create their own gesture handler derivative.
//
TCIDWndGestHandler*
TCIDWndGestHandler::pgesthMakeNew(          MCIDGestTarget*         pmgesttToUse
                                    , const tCIDCtrls::TWndHandle   hwndTar)
{
    // See if they want to force multi-touch off
    TString strTmp;
    const tCIDLib::TBoolean bMultiOK = !TProcEnvironment::bFind(L"CID_GESTNOMULTI", strTmp);

    //
    //  If multi-ok and it appears to be available, then take that. Else load the
    //  mouse only stuff.
    //
    TCIDWndGestHandler* pgesthRet = nullptr;
    const int iWantedFlags(NID_READY | NID_MULTI_INPUT);
    if (bMultiOK
    &&  ((::GetSystemMetrics(SM_DIGITIZER) & iWantedFlags) == iWantedFlags))
    {
        pgesthRet = new TWin7GestHandler(pmgesttToUse, hwndTar);
    }
     else
    {
        pgesthRet = new TXPGestHandler(pmgesttToUse, hwndTar);
    }

    // Initialize it and return it
    pgesthRet->Initialize();
    return pgesthRet;
}


// -------------------------------------------------------------------
//  TCIDWndGestHandler: Destructor
// -------------------------------------------------------------------
TCIDWndGestHandler::~TCIDWndGestHandler()
{
}

// -------------------------------------------------------------------
//  TCIDWndGestHandler: Hidden constructors
// -------------------------------------------------------------------
TCIDWndGestHandler::TCIDWndGestHandler(MCIDGestTarget* const pmgesttToUse) :

    TCIDGestHandler(pmgesttToUse)
{
}
