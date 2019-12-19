//
// FILE NAME: TestServers.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/13/2019
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
//  This is the main header file of the test application that bangs hard on the
//  standard servers (name, log, config.) So these are longer running abuse
//  tests, which are looking to find chinks in the servers' armour by giving
//  them a high volume from multiple threads, to help find any issues with
//  thread safety (since they are quite multi-threaded.)
//
//  Since they all need to spin up multiple threads to be extra abusive, we
//  provide a base class that will do the grunt work of that, and just call
//  the derived class back on a virtual from each thread. He is responsible
//  for any synchronization of his data.
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
#include    "CIDOrbUC.hpp"
#include    "CIDCrypto.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TTestServersBase
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTestServersBase : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Some constants for the tests
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4TestMinutes = 2;


        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTestServersBase() = delete;
        TTestServersBase(const TTestServersBase&) = delete;
        TTestServersBase(TTestServersBase&) = delete;
        ~TTestServersBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestServersBase& operator=(const TTestServersBase&) = delete;
        TTestServersBase& operator=(TTestServersBase&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTestServersBase
        (
            const   TString&                strName
            , const TString&                strDescr
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid TestThreadCallback
        (
            const   tCIDLib::TCard4         c4Index
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eProcessThreadRes
        (
                    TTextStringOutStream&   strmOut
        );

        tCIDLib::TVoid InvokeThreads
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid SetThreadFailure
        (
            const   tCIDLib::TCard4         c4Index
            , const TString&                strErrMsg
        );

        tCIDLib::TVoid SetThreadFailure
        (
            const   tCIDLib::TCard4         c4Index
            , const TError&                 errToCatch
        );

        tCIDLib::TVoid SetThreadSuccess
        (
            const   tCIDLib::TCard4         c4Index
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eTestThreadStart
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colMsgs
        //      For any threads that report an error, the error text is put into
        //      this list at their index.
        //
        //  m_colThreads
        //      An adopting list of thread objects, which we start on the
        //      eTestThreadStart() method, which just turns around and calls
        //      the virtual method provided by the derived class.
        //
        //  m_fcolResults
        //      The pass/fail result for each thread is stored at their respective
        //      indices.
        // -------------------------------------------------------------------
        tCIDLib::TStrArray      m_colMsgs;
        TRefVector<TThread>     m_colThreads;
        tCIDLib::TBoolArray     m_fcolResults;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestServersBase,TTestFWTest)
};




// ---------------------------------------------------------------------------
//  CLASS: TTest_NameSrv
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_NameSrv : public TTestServersBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_NameSrv();

        ~TTest_NameSrv();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid TestThreadCallback
        (
            const   tCIDLib::TCard4         c4Index
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_NameSrv,TTestServersBase)
};



// ---------------------------------------------------------------------------
//  CLASS: TestServersApp
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TestServersApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TestServersApp();

        TestServersApp(const TestServersApp&) = delete;
        TestServersApp(TestServersApp&&) = delete;

        ~TestServersApp();


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
        RTTIDefs(TestServersApp,TTestFWApp)
};



