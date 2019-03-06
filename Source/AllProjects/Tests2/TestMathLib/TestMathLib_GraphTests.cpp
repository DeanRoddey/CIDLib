//
// FILE NAME: TestMathLib_GraphTests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements tests for the TGraphData class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestMathLib.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_GraphData,TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_GraphData
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_GraphData: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_GraphData::TTest_GraphData() :

    TTestFWTest(L"Graph Data", L"Tests the TGraphData class", 3)
{
}

TTest_GraphData::~TTest_GraphData()
{
}


// ---------------------------------------------------------------------------
//  TTest_GraphData: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_GraphData::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Some of the methods to get samples out of and into a graph object use
    //  a fund vector of sample values.
    //
    tCIDLib::TBoolean           bRes;
    tCIDLib::TCard4             c4NewCnt;
    tCIDLib::TCard4             c4SerialNum;
    tCIDLib::TFloat4            f4Sample;
    TFundVector<tCIDLib::TFloat4> fcolSamples(64);

    // Do some simple tests on a small sample count object
    {
        f4Sample = 0;

        const tCIDLib::TCard4 c4SampleCnt = 8;
        TGraphData grdatTest(c4SampleCnt);

        // We should have zero available samples and not show up full
        if (grdatTest.c4SamplesAvail() != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Initial sample count was not zero\n\n";
        }

        if (grdatTest.bIsFull())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Graph should not be full after ctor\n\n";
        }

        // Check the reported max samples
        if (grdatTest.c4MaxSamples() != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Max samples was not set correctly\n\n";
        }

        // Push it all but one full
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SampleCnt - 1; c4Index++)
        {
            grdatTest.PushNewSample(f4Sample);
            f4Sample += 1.0;
        }

        // Check that there are that many samples
        if (grdatTest.c4SamplesAvail() != c4SampleCnt - 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Wrong number of samples in graph\n\n";
        }

        // Still should not be full
        if (grdatTest.bIsFull())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Graph should not be full after ctor\n\n";
        }

        // Push one more, which should make it full
        grdatTest.PushNewSample(f4Sample);
        f4Sample += 1.0;

        if (!grdatTest.bIsFull())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Graph should be full now\n\n";
        }

        // The serial number should be the count plus one now
        if (grdatTest.c4SerialNum() != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Invalid serial number in graph\n\n";
        }

        // Push another one
        grdatTest.PushNewSample(f4Sample);
        f4Sample += 1.0;

        // There should still be only the sample count samples available
        if (grdatTest.c4SamplesAvail() != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Sample count overflow error\n\n";
        }

        // The serial number should be the sample count plus two now
        if (grdatTest.c4SerialNum() != c4SampleCnt + 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Invalid serial number in graph\n\n";
        }

        //
        //  Query with a serial number of 0, which should get us all of the
        //  available samples.
        //
        c4SerialNum = 0;
        bRes = grdatTest.bQuerySamples(c4SerialNum, fcolSamples, c4NewCnt);
        if (!bRes)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Expected to get new samples\n\n";
        }

        if (c4NewCnt != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong number of new samples\n\n";
        }

        //
        //  Check the values, they should be in oldest to newest order. They
        //  should start at 1, since we pushed the first sample out of the
        //  graph by now.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SampleCnt; c4Index++)
        {
            if (fcolSamples[c4Index] != tCIDLib::TFloat4(c4Index + 1))
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"New samples are not in the right order\n\n";
                break;
            }
        }

        // Do another query with the same serial number, which should get nothing
        bRes = grdatTest.bQuerySamples(c4SerialNum, fcolSamples, c4NewCnt);
        if (bRes)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should not have gotten new samples\n\n";
        }

        //
        //  Push multiple samples in. The zero final parm indicates we don't
        //  want to force a new serial number, it should just increment it's own.
        //
        const tCIDLib::TFloat4 f4SaveCur = f4Sample;
        fcolSamples.RemoveAll();
        fcolSamples.c4AddElement(f4Sample);
        f4Sample += 1.0;
        fcolSamples.c4AddElement(f4Sample);
        f4Sample += 1.0;
        fcolSamples.c4AddElement(f4Sample);
        f4Sample += 1.0;
        grdatTest.PushNewSamples(fcolSamples, 3, 0);

        // And now we should get these new samples
        bRes = grdatTest.bQuerySamples(c4SerialNum, fcolSamples, c4NewCnt);
        if (!bRes)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Did not get newly added samples\n\n";
        }

        if (c4NewCnt != 3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got wrong number of new samples\n\n";
        }

        // Test the values
        if ((fcolSamples[0] != f4SaveCur)
        ||  (fcolSamples[1] != f4SaveCur + 1)
        ||  (fcolSamples[2] != f4SaveCur + 2))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"New sample values were wrong\n\n";
        }


        // And just in case, we should still have the same number of samples
        if (grdatTest.c4SamplesAvail() != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Sample count overflow error\n\n";
        }

        // And his and our serial numbers should be the same
        if (grdatTest.c4SerialNum() != c4SerialNum)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Serial numbers were not equal\n\n";
        }


        //
        //  Now test querying into another graph object. In this one, we will
        //  set it to two fewer samples in size. Reset the serial number to get
        //  all new data.
        //
        const tCIDLib::TCard4 c4CopyCnt = c4SampleCnt - 2;
        TGraphData grdatCopy(c4CopyCnt);
        c4SerialNum = 0;
        bRes = grdatTest.bQuerySamples(grdatCopy, c4NewCnt);

        if (!bRes)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Did not get any samples\n\n";
        }

        //
        //  We should have only gotten the max it can hold since the source
        //  has more than it could hold without pushing some back out.
        //
        if (c4NewCnt != c4CopyCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Did not get correct number of samples\n\n";
        }

        //
        //  And make sure that they are the right values, or at least just check
        //  the first value to make sure it is the starting value we'd expect,
        //  which should be c4CopyCnt down from the last sample we put into the
        //  original graph.
        //
        if (grdatCopy.f4SampleAt(0) != f4Sample - tCIDLib::TFloat4(c4CopyCnt))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"New samples had wrong values\n\n";
        }

        //
        //  Reset the graph and test the state. First without resetting the
        //  serial number.
        //
        const tCIDLib::TCard4 c4SaveSerNum = grdatTest.c4SerialNum();
        grdatTest.Reset();
        if (grdatTest.c4SerialNum() != c4SaveSerNum)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Serial number should not have been reset\n\n";
        }

        if (grdatTest.c4SamplesAvail() != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Samples still available after reset\n\n";
        }

        if (grdatTest.bIsFull())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Graph still full after reset\n\n";
        }

        // Do the reset again with a new serial number this time
        grdatTest.Reset(1UL);

        if (grdatTest.c4SerialNum() != 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The serial number was not reset\n\n";
        }


        //
        //  Do one more test where we push samples in via another graph. Tell it
        //  to take the source's serial number. This should leave them with the
        //  same number of samples and the same serial number.
        //
        grdatTest.PushNewSamples(grdatCopy, kCIDLib::True);
        if (grdatTest.c4SamplesAvail() != grdatCopy.c4SamplesAvail())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The samples available were not the same\n\n";
        }

        if (grdatTest.c4SerialNum() != grdatCopy.c4SerialNum())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The serial numbers were not the same\n\n";
        }

        // Push them again and say don't take the serial number
        grdatTest.PushNewSamples(grdatCopy, kCIDLib::False);

        //
        //  Now the test one should be equal to the max of the original object
        //  since we've exceeded it's capacity.
        //
        if (grdatTest.c4SamplesAvail() != c4SampleCnt)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The samples available was not adjusted correctly\n\n";
        }

        if (grdatTest.c4SerialNum() != grdatCopy.c4SerialNum() + grdatCopy.c4SamplesAvail())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The serial number was not adjusted correctly\n\n";
        }
    }
    return eRes;
}


