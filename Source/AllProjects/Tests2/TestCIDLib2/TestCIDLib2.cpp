//
// FILE NAME: TestCIDLib2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This is the main implementation file of the test program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "TestCIDLib2.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TCIDLibTestApp,TTestFWApp)




// ---------------------------------------------------------------------------
//  CLASS: TCIDLibTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TCIDLibTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TCIDLibTestApp::TCIDLibTestApp()
{
}

TCIDLibTestApp::~TCIDLibTestApp()
{
}


// ----------------------------------------------------------------------------
//  TCIDLibTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TCIDLibTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TCIDLibTestApp::LoadTests()
{
    //
    //  Load up our tests on our parent class, basically in the order that
    //  we'd want to test them, so that within a given test level they will
    //  be seen in this order.
    //

    // The most fundamental stuff
    AddTest(new TTest_Types);
    AddTest(new TTest_RawBits);
    AddTest(new TTest_SafeCnt1);
    AddTest(new TTest_CntPtr1);
    AddTest(new TTest_UniquePtr);
    AddTest(new TTest_FlagJanitor);
    AddTest(new TTest_LambdaJan);
    AddTest(new TTest_Enum1);
    AddTest(new TTest_EnumForEach);

    AddTest(new TTest_String1);
    AddTest(new TTest_String2);
    AddTest(new TTest_String3);
    AddTest(new TTest_StringMove);
    AddTest(new TTest_StringCat);

    // General tests of basic classes, non all that inter-related
    AddTest(new TTest_CoordCtor);
    AddTest(new TTest_CoordAcc);
    AddTest(new TTest_CoordOps);
    AddTest(new TTest_Area1);
    AddTest(new TTest_Area2);
    AddTest(new TTest_Area3);
    AddTest(new TTest_Color1);
    AddTest(new TTest_Color2);
    AddTest(new TTest_MemBufMove);
    AddTest(new TTest_MemBufExp);
    AddTest(new TTest_Numeric1);
    AddTest(new TTest_Numeric2);
    AddTest(new TTest_Time1);
    AddTest(new TTest_Time2);
    AddTest(new TTest_Time3);
    AddTest(new TTest_Time4);
    AddTest(new TTest_Base641);
    AddTest(new TTest_BitSeq1);
    AddTest(new TTest_BitSeq2);
    AddTest(new TTest_BitSeq3);
    AddTest(new TTest_Bitset1);

    // Fundamental collection tests
    AddTest(new TTest_FundVectorLambda);

    // Collection tests
    AddTest(new TTest_ObjArrayLambda);
    AddTest(new TTest_ObjArrayMoveSem);
    AddTest(new TTest_RefVectorLambda);
    AddTest(new TTest_RefVectorMoveItem);
    // AddTest(new TTest_RefVectorMoveSem);
    AddTest(new TTest_VectorLambda);
    AddTest(new TTest_VectorMoveSem);
    AddTest(new TTest_VectorPlace);
    AddTest(new TTest_DequeMoveSem);
    AddTest(new TTest_DequePlace);
    AddTest(new TTest_HashSetPlace);
    AddTest(new TTest_ColAlgo1);

    AddTest(new TTest_ColCursors);

    // General utilitie classes
    AddTest(new TTest_LogLimiter);

    // The pool and related classes
    AddTest(new TTest_SimplePool);
    AddTest(new TTest_SimplePoolPtr);
    AddTest(new TTest_FixedSizePool);

    // Higher level stuff
    AddTest(new TTest_GenCache1);
    AddTest(new TTest_GenCache2);
    AddTest(new TTest_GenCache3);

    // Test out streams first then input, which are linked to outputs
    AddTest(new TTest_ChunkedBinOutStream);
    AddTest(new TTest_ChunkedTextOutStream);
    AddTest(new TTest_ChunkedBinInStream);
    AddTest(new TTest_ChunkedTextInStream);

    // Cicular buffer
    AddTest(new TTest_CircularBuf);

    // High level tests that can gen up some threads
    AddTest(new TTest_CntPtr2);
    AddTest(new TTest_SafeCnt2);

    // Publish/subscribe
    AddTest(new TTest_PubSub1);
    AddTest(new TTest_PubSubVector);

    // Directory change monitoring
    AddTest(new TTest_DirChange1);
}

tCIDLib::TVoid TCIDLibTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCIDLibTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCIDLibTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TCIDLibTestApp   tfwappCIDLib;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TCIDLibTestApp>(&tfwappCIDLib, &TCIDLibTestApp::eTestThread)
    )
)

