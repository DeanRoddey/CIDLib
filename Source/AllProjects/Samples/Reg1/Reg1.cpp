//
// FILE NAME: Reg1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2005
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
//  This is the main module for a simple program that will search the
//  registry for a particular binary file. Since registry stuff is Windows
//  specific, this one uses a namespace from the CIDKernel level that is
//  only built on the Windows platform.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDKernel_RegistryWin32.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Reg1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
const tCIDLib::TCard4 c4DataBufSz = 0x10000;
const tCIDLib::TCard4 c4NameBufSz = 1024;

tCIDLib::TCard1 ac1DataBuf[c4DataBufSz];
tCIDLib::TCh    achNameBuf[c4NameBufSz + 1];
tCIDLib::TCh    achTypeBuf[c4NameBufSz + 1];
TOutConsole     conOut;


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TVoid SearchTree2(  const   tCIDKernel::TWRegHandle hkeyPar
                                    ,       TPathStr&               pathCur)
{
    //
    //  Remember the end of the incoming path, so we can cap back at that
    //  point on the way out.
    //
    const tCIDLib::TCard4 c4PathLen = pathCur.c4Length();

    // Get info on this key
    tCIDLib::TBoolean   bOk;
    tCIDLib::TCard4     c4MaxDataLen;
    tCIDLib::TCard4     c4SubKeys;
    tCIDLib::TCard4     c4MaxSubKeyLen;
    tCIDLib::TCard4     c4MaxValueLen;
    tCIDLib::TCard4     c4Values;
    bOk = TKrnlWin32Registry::bQueryKeyInfo
    (
        hkeyPar
        , c4SubKeys
        , c4Values
        , c4MaxSubKeyLen
        , c4MaxValueLen
        , c4MaxDataLen
    );

    //
    //  First handle any values of the incoming key. We have the count of
    //  values from the info query above.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Values; c4Index++)
    {
        tCIDLib::TCard4         c4Bytes;
        tCIDKernel::ERegDTypes  eRegType;
        bOk = TKrnlWin32Registry::bQueryValueAt
        (
            hkeyPar
            , c4Index
            , achNameBuf
            , c4NameBufSz
            , eRegType
            , c4DataBufSz
            , ac1DataBuf
            , c4Bytes
        );
    }

    //
    //  Enumerate the sub-keys at this level and recurse on them. We have
    //  the count of sub-keys from the query call above.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SubKeys; c4Index++)
    {
        bOk = TKrnlWin32Registry::bQuerySubKeyAt(hkeyPar, c4Index, achNameBuf, c4NameBufSz);
        if (bOk)
        {
        }
         else
        {
            // Some sort of error
        }
    }

    // Remove any of the path we added
    pathCur.CapAt(c4PathLen);
}


static tCIDLib::TVoid SearchTree(const  tCIDKernel::ERootKeys   eRoot
                                , const tCIDLib::TCh* const     pszStart)
{
    tCIDKernel::TWRegHandle hkeyCur = TKrnlWin32Registry::hkeyGetRootSubKey
    (
        eRoot, 0, tCIDKernel::ERegAccFlags::ReadOnly
    );

    TPathStr pathCur(pszStart);
    SearchTree2(hkeyCur, pathCur);
    TKrnlWin32Registry::bCloseKey(hkeyCur);
}


//
//  Shows the parameter usage for the program.
//
static tCIDLib::TVoid ShowUsage()
{
    conOut << kCIDLib::NewLn
             << L"Usage: Reg1 tofind" << kCIDLib::DNewLn
             << L"       tofind is a quoted string of byte values to search\n"
             << L"       for. It should be little endian, e.g. to find 0x4321\n"
             << L"       search for '21 43'.\n"
             << kCIDLib::EndLn;
}


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Display the program blurb
    conOut << kCIDLib::NewLn
             << L"Reg1.Exe, Version: "
             << facCIDLib().strVersion() << kCIDLib::NewLn
             << L"CIDLib Registry Sample 1"
             << kCIDLib::NewLn << kCIDLib::EndLn;

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Invoke the operation on each major subkey
        SearchTree(tCIDKernel::ERootKeys::Classes, L"/Classes");
        SearchTree(tCIDKernel::ERootKeys::CurrentUser, L"/CurUser");
        SearchTree(tCIDKernel::ERootKeys::LocalMachine, L"/LocMachine");
        SearchTree(tCIDKernel::ERootKeys::Users, L"/Users");
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut <<  L"A CIDLib runtime error occured during processing. "
                 <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::NewLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing.\n  Error="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


