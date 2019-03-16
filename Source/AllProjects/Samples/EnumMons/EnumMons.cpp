//
// FILE NAME: EnumMons.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  This is the main module for a simple program that demonstrates how to
//  enumerate the available video display monitors and get some info about
//  them.
//
//  This program is so simple that it does not create a facility object.
//
// CAVEATS/GOTCHAS:
//

// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. The base UI header brings in everything we need
// ---------------------------------------------------------------------------
#include    "CIDCtrls.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"FileSys1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

// This is the the thread function for the main thread
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Get a shortcut to the standard output stream. We do this instead of a
    //  console, so we can be redirected.
    //
    TTextOutStream& strmOut = TSysInfo::strmOut();

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Get a list of available monikers, then iterate and display them
        TVector<TGUIMonInfo> colMons;
        facCIDCtrls().QueryMonitors(colMons);

        strmOut << L"\n\nAvailable Monitors\n----------------------------\n";
        colMons.ForEach
        (
            [&strmOut](const TGUIMonInfo& gmiCur)
            {
                strmOut << L"DevName=" << gmiCur.strDevName()
                        << L"\n    Phys=" << gmiCur.areaPhysical()
                        << L"\n    Avail=" << gmiCur.areaAvailable()
                        << kCIDLib::DNewLn;
                return kCIDLib::True;
            }
        );
        strmOut << kCIDLib::EndLn;
    }

    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        strmOut <<  L"A CIDLib runtime error occured during processing. "
                <<  L"\nError: " << errToCatch.strErrText()
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured during processing.\n  Error="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception, which also should not happen, but...
    catch(...)
    {
        strmOut << L"A general exception occured during processing\n"
                << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
