//
// FILE NAME: StressTests.hpp
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
//  This is the main header file of the CIDLib object store test app. This is
//  a standard CIDLib test framework test app.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "CIDMath.hpp"
#include    "CIDObjStore.hpp"
#include    "TestFWLib.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore1
// PREFIX: tfwt
//
//  This file implements a simple object store stress test, which just loads
//  enough large objects into the store to make it expand out and then checks
//  that they all expanded correctly and validates the store.
// ---------------------------------------------------------------------------
class TTest_ObjStore1: public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjStore1();

        ~TTest_ObjStore1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ObjStore1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore2
// PREFIX: tfwt
//
//  This file implements a single threaded stress test. It does some common
//  functions of adding, updating, reading, and deleting items, with a
//  reasonable number of items and over a minute period. This simulates the
//  standard functions, mixes up the store a lot and caues compactions and
//  expansions. Since it is single threaded, it will be far easier to catch
//  problems in the debugger, and the multi-threaded tests that follow should
//  be limited to catching synchronization issues.
// ---------------------------------------------------------------------------
class TTest_ObjStore2: public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjStore2();

        ~TTest_ObjStore2();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ObjStore2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore3
// PREFIX: tfwt
//
//  Similar to Stress2 above, but this one is multi-thread. It spins up 8
//  thread which all beat on the object store. In this case, all the threads
//  are using their own keys, so it's not testing contenting over keys, just
//  that use of the store by multiple threads doesn't hose it. At the end
//  we validate the store to insure that it's all still coherent.
// ---------------------------------------------------------------------------
class TTest_ObjStore3: public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjStore3();

        ~TTest_ObjStore3();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eTestThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4ThreadErrs;
        TEvent          m_evStart;
        TMutex          m_mtxOut;
        TCIDObjStore    m_oseTest;
        TTextOutStream* m_pstrmOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ObjStore3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore4
// PREFIX: tfwt
//
//  This is a multi-threaded test that pits a set of thread that add objects
//  to the store against a set that remove items. It just insures that multiple
//  threads can bang on the store and access the same keys without corrupting
//  it. At the end we validate the store to insure that it's all still
//  coherent. The adder thread adds keys and puts them on a queue. The remove
//  threads pull items off the queue and process them.
// ---------------------------------------------------------------------------
class TTest_ObjStore4: public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjStore4();

        ~TTest_ObjStore4();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eAddThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eDelThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4NextKeyId;
        tCIDLib::TCard4         m_c4ThreadErrs;
        tCIDLib::TSafeStringQ   m_colKeys;
        TEvent                  m_evStart;
        TMutex                  m_mtxOut;
        TCIDObjStore            m_oseTest;
        TTextOutStream*         m_pstrmOut;



        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ObjStore4,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TStressTests
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TStressTestsApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TStressTestsApp();

        TStressTestsApp(const TStressTestsApp&) = delete;

        ~TStressTestsApp();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        )   override;

        tCIDLib::TVoid LoadTests() override;

        tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        )   override;

        tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        )   override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStressTestsApp,TTestFWApp)
};


