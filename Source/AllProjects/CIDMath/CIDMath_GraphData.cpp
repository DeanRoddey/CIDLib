//
// FILE NAME: CIDMath_GraphData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/2011
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
//  This file implements the TGraphData class.
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
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGraphData,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TGraphData
//  PREFIX: grdat
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphData: Constructors and Destructor
// ---------------------------------------------------------------------------
TGraphData::TGraphData( const   tCIDLib::TCard4 c4MaxSamples
                        , const tCIDLib::TCard4 c4InitSerialNum) :

    m_c4MaxSamples(c4MaxSamples + 1)
    , m_c4SerialNum(c4InitSerialNum)
    , m_c4Head(0)
    , m_c4Tail(0)
    , m_pf4Data(0)
{
    CIDAssert(c4MaxSamples > 0, L"Graphs must have at least one element");

    // Allocate the data array
    m_pf4Data = new tCIDLib::TFloat4[m_c4MaxSamples];
}

TGraphData::TGraphData(const TGraphData& grdatToCopy) :

    m_c4MaxSamples(grdatToCopy.m_c4MaxSamples)
    , m_c4SerialNum(grdatToCopy.m_c4SerialNum)
    , m_c4Head(grdatToCopy.m_c4Head)
    , m_c4Tail(grdatToCopy.m_c4Tail)
    , m_pf4Data(0)
{
    // Allocate the data array and copy over the data
    m_pf4Data = new tCIDLib::TFloat4[m_c4MaxSamples];
    TRawMem::CopyMemBuf
    (
        m_pf4Data
        , grdatToCopy.m_pf4Data
        , sizeof(m_pf4Data[0]) * (m_c4MaxSamples)
    );
}

TGraphData::~TGraphData()
{
    // Clean up the sample data
    delete [] m_pf4Data;
}


// ---------------------------------------------------------------------------
//  TGraphData: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TGraphData::bIsFull() const
{
    //
    //  If the tail is at the element before the head, then the
    //  deque is full (taking wraparound into account.)
    //
    if (m_c4Tail == m_c4MaxSamples - 1)
    {
        //
        //  The tail is at the end, so if the head is at the start,
        //  then the tail is next and we are full.
        //
        if (m_c4Head == 0)
            return kCIDLib::True;
    }
     else if (m_c4Tail + 1 == m_c4Head)
    {
        // No special case, head is one past the tail, so full
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  We give back sample data to get the caller into sync. The caller should
//  set the serial number value to zero initially, to get all available samples,
//  then pass the old one back in each time. If new samples are available, he'll
//  get those and the new serial number.
//
//  We give the samples back oldest to newest, i.e. display order left to
//  right. In a scrolling graph, the newest ones are on the right and move left
//  over time.
//
tCIDLib::TBoolean
TGraphData::bQuerySamples(  tCIDLib::TCard4&                    c4SerialNum
                            , TFundVector<tCIDLib::TFloat4>&    fcolToFill
                            , tCIDLib::TCard4&                  c4NewSamples) const
{
    c4NewSamples = 0;
    fcolToFill.RemoveAll();

    // If his serial number is the same as ours, return no samples
    if (c4SerialNum == m_c4SerialNum)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Avail = c4SamplesAvail();
    if (c4SerialNum > m_c4SerialNum)
    {
        //
        //  If his serial number is somehow bigger than ours, that's bad, but we'll
        //  just reset him by returning him all the samples and giving him the new
        //  correct serial number.
        //
        c4NewSamples = c4Avail;
    }
     else
    {
        //
        //  The normal scenario. Calculate how far behind he is. If it's more
        //  than our available sample count, we give him that many back.
        //
        c4NewSamples = m_c4SerialNum - c4SerialNum;
        if (c4NewSamples > c4Avail)
            c4NewSamples = c4Avail;
    }

    CIDAssert(c4NewSamples <= c4Avail, L"Invalid new sample return count");

    //
    //  And now let's give him back samples. We have to calculate now far
    //  back from the tail we start, taking wrapping into account.
    //
    tCIDLib::TCard4 c4Index = m_c4Tail;
    if (c4NewSamples > c4Index)
        c4Index = m_c4MaxSamples - (c4NewSamples - m_c4Tail);
    else
        c4Index = m_c4Tail - c4NewSamples;

    while (c4Index != m_c4Tail)
    {
        fcolToFill.c4AddElement(m_pf4Data[c4Index]);

        c4Index++;
        if (c4Index == m_c4MaxSamples)
            c4Index = 0;
    }

    // Give the caller back our serial number and return that we gave back samples
    c4SerialNum = m_c4SerialNum;
    return kCIDLib::True;
}


//
//  This is similar to the one above, but it allows the new samples to be loaded
//  directly into another graph object, appending them to the target object. This
//  is very useful in a few places in the CQC layer.
//
tCIDLib::TBoolean
TGraphData::bQuerySamples(TGraphData& grdatTar, tCIDLib::TCard4& c4NewSamples) const
{
    // Get the caller's serial number out for our calculations below
    tCIDLib::TCard4 c4SerialNum = grdatTar.m_c4SerialNum;

    // If his is the same as ours, nothing to do
    if (c4SerialNum == m_c4SerialNum)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Avail = c4SamplesAvail();
    if (c4SerialNum > m_c4SerialNum)
    {
        //
        //  If his serial number is somehow bigger than ours, that's bad, but we'll
        //  just reset him by returning him all the samples and giving him the new
        //  correct serial number.
        //
        c4NewSamples = c4Avail;
    }
     else
    {
        //
        //  The normal scenario. Calculate how far behind he is. If it's more
        //  than our available sample count, we give him that many back.
        //
        c4NewSamples = m_c4SerialNum - c4SerialNum;
        if (c4NewSamples > c4Avail)
            c4NewSamples = c4Avail;
    }

    CIDAssert(c4NewSamples <= c4Avail, L"Invalid new sample return count");

    //
    //  If this is more than the caller can hold, then it will be a waste
    //  of time to push samples on him that are going to get pushed out
    //  before we event return, so clip it back.
    //
    if (c4NewSamples > grdatTar.c4MaxSamples())
        c4NewSamples = grdatTar.c4MaxSamples();

    //
    //  And now let's give him back samples. We have to calculate now far
    //  back from the tail we start, taking wrapping into account.
    //
    tCIDLib::TCard4 c4Index = m_c4Tail;
    if (c4NewSamples > c4Index)
        c4Index = m_c4MaxSamples - (c4NewSamples - m_c4Tail);
    else
        c4Index = m_c4Tail - c4NewSamples;

    while (c4Index != m_c4Tail)
    {
        grdatTar.PushNewSample(m_pf4Data[c4Index]);
        c4Index++;
        if (c4Index == m_c4MaxSamples)
            c4Index = 0;
    }

    //
    //  Put our serial number into the target. We are overriding the increments
    //  that happened when we called PushNewSample() on him in the loop above.
    //
    grdatTar.m_c4SerialNum = m_c4SerialNum;
    return kCIDLib::True;
}


//
//  Return the max samples we were told to hold. It's one less than the actual
//  max samples we have because we made room for the extra spacer.
//
tCIDLib::TCard4 TGraphData::c4MaxSamples() const
{
    return m_c4MaxSamples - 1;
}


// Return the count of samples available currently
tCIDLib::TCard4 TGraphData::c4SamplesAvail() const
{
    if (m_c4Head == m_c4Tail)
        return 0;

    if (m_c4Head < m_c4Tail)
    {
        // We are not wrapped around right now
        return m_c4Tail - m_c4Head;
    }
     else
    {
        // We are wrapped around
        return (m_c4MaxSamples - m_c4Head) + m_c4Tail;
    }
}


// Return our current serial number
tCIDLib::TCard4 TGraphData::c4SerialNum() const
{
    return m_c4SerialNum;
}


// Get a sample at a specific index from the 'left', i.e. the oldest samples
tCIDLib::TFloat4 TGraphData::f4SampleAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= c4SamplesAvail())
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcGraph_BadIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
        );
    }

    // Start at the head and add the index, wrap if need
    tCIDLib::TCard4 c4Index = m_c4Head + c4At;
    if (c4Index >= m_c4MaxSamples)
        c4Index -= m_c4MaxSamples;

    return m_pf4Data[c4Index];
}


//
//  Add a single new sample to the list. We push them in at the tail. If we are
//  full we first toss one from the head.
//
tCIDLib::TVoid TGraphData::PushNewSample(const tCIDLib::TFloat4 f4ToPush)
{
    if (bIsFull())
    {
        m_c4Head++;
        if (m_c4Head == m_c4MaxSamples)
            m_c4Head = 0;
    }

    // Add a new one at the tail and push it up, wrapping if needed
    m_pf4Data[m_c4Tail] = f4ToPush;
    m_c4Tail++;
    if (m_c4Tail == m_c4MaxSamples)
        m_c4Tail = 0;

    // Bump the serial number
    m_c4SerialNum++;
}


//
//  Push multiple new samples in. If the passed serial number is non-zero, then
//  we take it. Otherwise we bump our own serial number for each new sample.
//
tCIDLib::TVoid
TGraphData::PushNewSamples( const   TFundVector<tCIDLib::TFloat4>&  fcolNew
                            , const tCIDLib::TCard4                 c4Count
                            , const tCIDLib::TCard4                 c4SerialNum)
{
    //
    //  For this one, we just do the same as the single sample version above,
    //  but in a loop.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // If full, push the head forward to toss an old sample
        if (bIsFull())
        {
            m_c4Head++;
            if (m_c4Head == m_c4MaxSamples)
                m_c4Head = 0;
        }

        // Add a new one at the tail and push it up, wrapping if needed
        m_pf4Data[m_c4Tail] = fcolNew[c4Index];
        m_c4Tail++;
        if (m_c4Tail == m_c4MaxSamples)
            m_c4Tail = 0;
    }

    // Either take the new (non-zero) serial number, or bump ours by the count
    if (c4SerialNum)
        m_c4SerialNum = c4SerialNum;
    else
        m_c4SerialNum += c4Count;
}


//
//  Similar to above, but we push them from another graph data object. We can
//  either take the caller's serial number or just bump ours.
//
tCIDLib::TVoid
TGraphData::PushNewSamples( const   TGraphData&         grdatNew
                            , const tCIDLib::TBoolean   bTakeSerialNum)
{
    //
    //  For this one, we just do the same as the single sample version above,
    //  but in a loop.
    //
    const tCIDLib::TCard4 c4Count = grdatNew.c4SamplesAvail();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // If full, push the head forward to toss an old sample
        if (bIsFull())
        {
            m_c4Head++;
            if (m_c4Head == m_c4MaxSamples)
                m_c4Head = 0;
        }

        // Add a new one at the tail and push it up, wrapping if needed
        m_pf4Data[m_c4Tail] = grdatNew.f4SampleAt(c4Index);
        m_c4Tail++;
        if (m_c4Tail == m_c4MaxSamples)
            m_c4Tail = 0;
    }

    // Take the caller's serial number or bump ours by the count
    if (bTakeSerialNum)
        m_c4SerialNum = grdatNew.m_c4SerialNum;
    else
        m_c4SerialNum += c4Count;
}


//
//  Reset us, optionally resetting the serial number as well. Otherwise, we
//  just empty the list, or fill it with a default value.
//
tCIDLib::TVoid TGraphData::Reset(const tCIDLib::TCard4 c4NewSerialNum)
{
    m_c4Head = 0;
    m_c4Tail = 0;
    if (c4NewSerialNum != kCIDLib::c4MaxCard)
        m_c4SerialNum = c4NewSerialNum;

    // Put some dummy data in for testing, just a sine wave
    #if 0
    tCIDLib::TFloat4 f4Inc = ((2 * kCIDLib::f4PI) * 2) / m_c4MaxSamples;
    tCIDLib::TFloat4 f4Angle = -(2 * kCIDLib::f4PI);
    while (m_c4Tail < m_c4MaxSamples - 1)
    {
        m_pf4Data[m_c4Tail++] =
            (TMathLib::f4Sine(f4Angle) * 128) + 128;
        f4Angle += f4Inc;
    }
    #endif
}

tCIDLib::TVoid
TGraphData::Reset(  const   tCIDLib::TFloat4    f4FillWith
                    , const tCIDLib::TCard4     c4NewSerialNum)
{
    m_c4Head = 0;
    m_c4Tail = 0;
    if (c4NewSerialNum != kCIDLib::c4MaxCard)
        m_c4SerialNum = 0;

    while (m_c4Tail < m_c4MaxSamples - 1)
        m_pf4Data[m_c4Tail++] = f4FillWith;
}


