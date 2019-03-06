//
// FILE NAME: TestOrb3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This is the main module for the test program. This program provides both
//  client and server sides of the program, and does one or the other according
//  to whether the /Client or /Server parameter is provided.
//
//  This is a simple test for a multi-threaded client, to insure that the
//  ORB transport system is thread-safe. The client side spins off a number
//  of threads, each of which does a different set of transactions for a
//  period of time.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestOrb3.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacTestOrb3,TFacility)



// ----------------------------------------------------------------------------
//  Global data
//
//  facTestOrb3
//      The definition of our global facility object, which is declared in
//      the main header.
// ----------------------------------------------------------------------------
TFacTestOrb3 facTestOrb3;



// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestCIDOrbMainThread"
        , TMemberFunc<TFacTestOrb3>(&facTestOrb3, &TFacTestOrb3::eMainThread)
    )
)



// ---------------------------------------------------------------------------
//   CLASS: TFacTestOrb3
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacTestOrb3: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacTestOrb3::TFacTestOrb3() :

    TFacility
    (
        L"TestOrb3"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacTestOrb3::~TFacTestOrb3()
{
}


// ---------------------------------------------------------------------------
//  TFacTestOrb3: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacTestOrb3::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
    {
        // Set our logging threshold down low for testing purposes
        TFacility::eGlobalLogMode(tCIDLib::ESeverities::Info);

        try
        {
            // Figure out the parameters
            if (TSysInfo::c4CmdLineParmCount() != 2)
            {
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }

            TString strParm = TSysInfo::strCmdLineParmAt(0);

            // The first parm must be the client or server flag
            tCIDLib::TBoolean bServer;
            if (strParm.bCompareI(L"/Client"))
                bServer = kCIDLib::False;
            else if (strParm.bCompareI(L"/Server"))
                bServer = kCIDLib::True;
            else
            {
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }

            // Next we have to have a port number
            tCIDLib::TIPPortNum ippnToUse = TSysInfo::strCmdLineParmAt(1).c4Val();

            if (bServer)
            {
                facCIDOrb().InitServer(ippnToUse, 4);
                if (!bOrbServerTest())
                    eRet = tCIDLib::EExitCodes::RuntimeError;
            }
             else
            {
                facCIDOrb().InitClient();
                if (!bOrbClientTest(ippnToUse))
                    eRet = tCIDLib::EExitCodes::RuntimeError;
            }

            if (eRet == tCIDLib::EExitCodes::Normal)
                m_conOut << L"Returned from tests normally" << kCIDLib::EndLn;
        }

        // Catch and report any error that occured
        catch(const TError& errToCatch)
        {
            m_conOut    << L"Unhandled exception occured!\n\n"
                        << L"Process: " << errToCatch.strProcess() << kCIDLib::NewLn
                        << L"   File: " << errToCatch.strFileName() << kCIDLib::NewLn
                        << L"   Line: " << errToCatch.c4LineNum() << kCIDLib::NewLn
                        << L"  Error: " << errToCatch.strErrText() << kCIDLib::NewLn
                        << L"Aux Txt: " << errToCatch.strAuxText() << kCIDLib::NewLn
                        << L"ErrCode: " << errToCatch.errcId() << kCIDLib::NewLn
                        << L" Thread: " << errToCatch.strThread() << kCIDLib::EndLn;
            eRet = tCIDLib::EExitCodes::RuntimeError;
        }

        catch(...)
        {
            m_conOut    << L"A system exception occured" << kCIDLib::EndLn;
            eRet = tCIDLib::EExitCodes::SystemException;
        }

        // And terminate our orb support
        try
        {
            facCIDOrb().Terminate();
        }

        catch(const TError& errToCatch)
        {
            m_conOut    << L"Exception during Orb cleanup!\n\n"
                        << L"   File: " << errToCatch.strFileName() << kCIDLib::NewLn
                        << L"   Line: " << errToCatch.c4LineNum() << kCIDLib::NewLn
                        << L"  Error: " << errToCatch.strErrText() << kCIDLib::NewLn
                        << L"Aux Txt: " << errToCatch.strAuxText() << kCIDLib::NewLn
                        << L"ErrCode: " << errToCatch.errcId() << kCIDLib::NewLn
                        << L" Thread: " << errToCatch.strThread() << kCIDLib::EndLn;
        }

        catch(...)
        {
            m_conOut    << L"System exception during Orb cleanup!" << kCIDLib::EndLn;
        }
    }

    return tCIDLib::EExitCodes::Normal;
}



// ---------------------------------------------------------------------------
//  TFacTestOrb3: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacTestOrb3::ShowUsage()
{
    m_conOut << L"TestOrb3 [/Client | /Server] portnum" << kCIDLib::EndLn;
}

