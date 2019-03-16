//
// FILE NAME: TestOrb2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This one is purely for benchmarking the basic ability to do calls and
//  return. No parameters and no return value. It just does a huge number of
//  calls. It will also serve as a good test for leaks in the core ORB
//  transport system.
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
#include    "TestOrb2.hpp"



// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacTestOrb2,TFacility)


// ----------------------------------------------------------------------------
//  Global data
//
//  facTestOrb2
//      The definition of our global facility object, which is declared in
//      the main header.
// ----------------------------------------------------------------------------
TFacTestOrb2 facTestOrb2;



// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestCIDOrbMainThread"
        , TMemberFunc<TFacTestOrb2>(&facTestOrb2, &TFacTestOrb2::eMainThread)
    )
)



// ---------------------------------------------------------------------------
//   CLASS: TFacTestOrb2
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacTestOrb2: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacTestOrb2::TFacTestOrb2() :

    TFacility
    (
        L"TestOrb2"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacTestOrb2::~TFacTestOrb2()
{
}


// ---------------------------------------------------------------------------
//  TFacTestOrb2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacTestOrb2::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
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
//  TFacTestOrb2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacTestOrb2::ShowUsage()
{
    m_conOut << L"TestOrb2 [/Client | /Server] portnum" << kCIDLib::EndLn;
}

