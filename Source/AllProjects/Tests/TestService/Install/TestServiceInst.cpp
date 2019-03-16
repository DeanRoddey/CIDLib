//
// FILE NAME: TestServiceInst.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/2002
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
#include    "CIDLib.hpp"
#include    "CIDKernel_ServiceWin32.hpp"


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
CIDLib_MainModule(TThread(L"TestService", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Functions for local use
// ----------------------------------------------------------------------------
tCIDLib::TVoid ShowUsage(TOutConsole& conOut)
{
    conOut << L"\nUsage:\n"
              L"    TestServiceInst /Remove svcname\n"
              L"    TestServiceInst /Install svcname display descr path [/Depends dep1 ...]\n"
           << kCIDLib::FlushIt;
}


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // For user output
    TOutConsole conOut;

    // We have to have at least two parms, no matter what
    if (TSysInfo::c4CmdLineParmCount() < 2)
    {
        ShowUsage(conOut);
        return tCIDLib::EExitCodes::BadParameters;
    }


    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    const TString strCmd = *cursParms++;

    // We have to have a next parameter which is the service name
    TString strName = *cursParms++;

    if (strCmd.bCompareI(L"/Remove"))
    {
        // Remove any previous service
        tCIDKernel::TWSvcHandle hsvcRemove = TKrnlWin32Service::hsvcOpen
        (
            strName.pszBuffer()
        );

        if (!hsvcRemove)
        {
            TKrnlError kerrCreate = TKrnlError::kerrLast();
            conOut  << L"Could not open the service. Error: "
                    << kmodCIDKernel.pszLoadCIDFacMsg(kerrCreate.errcId())
                    << L". Host err=" << kerrCreate.errcHostId()
                    << kCIDLib::FlushIt;
        }
         else
        {
            if (!TKrnlWin32Service::bRemove(hsvcRemove))
            {
                TKrnlError kerrCreate = TKrnlError::kerrLast();
                conOut  << L"Could not remove the service. Error: "
                        << kmodCIDKernel.pszLoadCIDFacMsg(kerrCreate.errcId())
                        << L". Host err=" << kerrCreate.errcHostId()
                        << kCIDLib::FlushIt;
            }
             else
            {
                conOut  << L"The service is scheduled for removal. Please log off"
                        << kCIDLib::FlushIt;
            }
        }
    }
     else if (strCmd.bCompareI(L"/Install"))
    {
        // We have to have at least 3 more parms
        if (TSysInfo::c4CmdLineParmCount() < 4)
        {
            ShowUsage(conOut);
            return tCIDLib::EExitCodes::BadParameters;
        }

        TString strDisplay = *cursParms++;
        TString strDescription = *cursParms++;
        TString strPath = *cursParms++;

        //
        //  The last parm is optional and is /Depends, and any parameters after
        //  that represent dependent services.
        //
        tCIDLib::TCard4 c4DepCount = 0;
        const tCIDLib::TCh* apszDeps[64];
        if (cursParms)
        {
            TString strCurDep = *cursParms++;
            if (strCurDep != L"/Depends")
            {
                conOut  << L"Expected /Depends following the path" << kCIDLib::FlushIt;
                return tCIDLib::EExitCodes::BadParameters;
            }

            for (; cursParms; ++cursParms)
                apszDeps[c4DepCount++] = cursParms->pszDupBuffer();
        }

        const tCIDLib::TCh* apszParms[3];
        apszParms[0] = L"Parm1";
        apszParms[1] = L"Parm2";
        apszParms[2] = L"Parm3";

        // Create our test service
        tCIDKernel::TWSvcHandle hsvcNew = TKrnlWin32Service::hsvcCreate
        (
            strPath.pszBuffer()
            , strName.pszBuffer()
            , strDisplay.pszBuffer()
            , strDescription.pszBuffer()
            , tCIDKernel::EWSrvStarts::Demand
            , apszDeps
            , c4DepCount
            , apszParms
            , 3
        );

        if (!hsvcNew)
        {
            TKrnlError kerrCreate = TKrnlError::kerrLast();
            conOut  << L"Could not create the service. Error: "
                    << kmodCIDKernel.pszLoadCIDFacMsg(kerrCreate.errcId())
                    << L". Host Err=" << kerrCreate.errcHostId()
                    << kCIDLib::FlushIt;
        }
         else
        {
            if (!TKrnlWin32Service::bStart(hsvcNew, 0, 0, 10000))
            {
                TKrnlError kerrCreate = TKrnlError::kerrLast();
                conOut  << L"Could not start the service. Error: "
                        << kmodCIDKernel.pszLoadCIDFacMsg(kerrCreate.errcId())
                        << L". Host Err=" << kerrCreate.errcHostId()
                        << kCIDLib::FlushIt;
            }
        }
    }
     else
    {
        ShowUsage(conOut);
        return tCIDLib::EExitCodes::BadParameters;
    }
    return tCIDLib::EExitCodes::Normal;
}


