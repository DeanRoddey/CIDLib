//
// FILE NAME: TestAppShell.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/01/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
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
#if         !defined(CIDAPPSHELL_HPP)
#include    "CIDAppSh.hpp"
#endif


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestAppShell", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
TInConsole  conIn;
TOutConsole conOut;


//
//  Create our test derivative of the app shell engine
//
class TTestShEngine : public TAppShEngine
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestShEngine()
        {
        }

        ~TTestShEngine()
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean
        bCheckShutdownComplete( TAppShRecord&           asrTarget
                                , tCIDLib::EExitCodes&  eToFill)
        {
            //
            //  See if it's done yet. We can wait a bit for it to die, as
            //  long as we don't go over a second or so.
            //
            return asrTarget.bWaitForDeath(eToFill, 500);
        }

        tCIDLib::TVoid Exited(          TAppShRecord&       asrExited
                                , const tCIDLib::EExitCodes eCode)
        {
            conOut << L"App '" << asrExited.strTitle() << L"' exited\n\n"
                   << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid IdleCallback()
        {
            TAudio::Beep(880, 25);
        }

        tCIDLib::TVoid Restarted(TAppShRecord& asrRestarted)
        {
            conOut << L"App '" << asrRestarted.strTitle() << L"' re-started\n\n"
                   << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid
        ShellException( const   TError&         errThatOccured
                        ,       TAppShRecord&   asrInvolved)
        {
            conOut  << L"Except in: '" << asrInvolved.strTitle() << L"'\n"
                    << errThatOccured << kCIDLib::DNewLn << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid ShellMessage(const TString& strMsg)
        {
            conOut  << strMsg << kCIDLib::NewLn << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid ShellUnknownException(TAppShRecord& asrInvolved)
        {
            conOut  << L"Unknown except in: '" << asrInvolved.strTitle()
                    << L"'\n" << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid Started(TAppShRecord& asrStarted)
        {
            conOut << L"App '" << asrStarted.strTitle() << L"' started\n"
                   << kCIDLib::FlushIt;
        }

        tCIDLib::TVoid StartShutdown(TAppShRecord& asrToStart)
        {
            // Create the event and trigger it
            try
            {
                TEvent evWait
                (
                    TResourceName(L"CharmedQuark", L"AppShellTester", asrToStart.strExtra())
                    , tCIDLib::EEventStates::Reset
                    , tCIDLib::ECreateActs::OpenIfExists
                    , kCIDLib::False
                );
                evWait.Trigger();
            }

            catch(...)
            {
                // It must already be dead
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Unimplimented
        // -------------------------------------------------------------------
        TTestShEngine(const TTestShEngine&);
        tCIDLib::TVoid operator=(const TTestShEngine&);


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestShEngine,TAppShEngine)
};

RTTIDecls(TTestShEngine,TAppShEngine)




// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {

        TTestShEngine aseTest;

        //
        //  Add some instances of the test program, with different levels and
        //  shutdown times.
        //
        TAppShRecord* pasrNew;
        pasrNew = new TAppShRecord
        (
            L"Test1_1", L"ShellTester", L"1000 Test1_1", tCIDAppSh::EAppLevels::L1
        );
        pasrNew->strExtra(L"Test1_1");
        aseTest.AddApp(pasrNew);

        pasrNew = new TAppShRecord
        (
            L"Test1_2", L"ShellTester", L"2000 Test1_2", tCIDAppSh::EAppLevels::L1
        );
        pasrNew->strExtra(L"Test1_2");
        aseTest.AddApp(pasrNew);

        pasrNew = new TAppShRecord
        (
            L"Test2_1", L"ShellTester", L"3500 Test2_1", tCIDAppSh::EAppLevels::L2
        );
        pasrNew->strExtra(L"Test2_1");
        aseTest.AddApp(pasrNew);

        pasrNew = new TAppShRecord
        (
            L"Test2_2", L"ShellTester", L"2500 Test2_2", tCIDAppSh::EAppLevels::L2
        );
        pasrNew->strExtra(L"Test2_2");
        aseTest.AddApp(pasrNew);

        pasrNew = new TAppShRecord
        (
            L"Test3_1", L"ShellTester", L"3000 Test3_1", tCIDAppSh::EAppLevels::L3
        );
        pasrNew->strExtra(L"Test3_1");
        aseTest.AddApp(pasrNew);

        tCIDLib::EConKeys eKeyType;
        while (kCIDLib::True)
        {
            const tCIDLib::TCh chGot = conIn.chReadChar(eKeyType, kCIDLib::c4MaxWait);
            if (chGot == kCIDLib::chLatin_p)
            {
                // Stop all the apps
                aseTest.StopAll(10000);
            }
             else if (chGot == kCIDLib::chLatin_s)
            {
                aseTest.StartAll();
            }
             else if (chGot == kCIDLib::chLatin_x)
            {
                // Stop all and exit
                aseTest.StopAll(10000);
                break;
            }
        }


        conOut << L"Completed sucessfully" << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


