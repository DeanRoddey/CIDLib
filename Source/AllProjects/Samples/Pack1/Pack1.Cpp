//
// FILE NAME: Pack1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/22/2015
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
//  This is the main module for a program that demonstrates the basics of the
//  CIDPack facility, which creates packages of compressed files and allows them
//  to be later unpacked. It can pack all of the files under a given starting
//  directory and later pull them back out to recreate that directory structure.
//
//  This program supports both packaging and extracting.
//
//
//  Like most of these basic samples, this guy doesn't create a facility object,
//  it just starts up a thread on a local function.
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
#include    "CIDPack.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Pack1MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
tCIDLib::TBoolean   bPackMode;
tCIDLib::TBoolean   bVerbose = kCIDLib::False;
tCIDLib::TCard8     c8Version = 0;
TOutConsole         conOut;
TPathStr            pathSrc;
TPathStr            pathTar;
TString             strNotes;



// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------

// Shows the program usage if we get incorrect input
static tCIDLib::TVoid ShowUsage()
{
    conOut  << L"CIDLib Pack Demo 1\n\n"
            << L"   Usage:\n"
            << L"     Pack1 /pack srcdir tarpack [/Notes=text /Version=v.v.v /Verbose]\n\n"
            << L"   srcdir=Top directory to pack from\n"
            << L"   tarpack=Name of target package, without extension\n"
            << L"   Version must be a 3 part value in M.m.r format\n\n"
            << L"     Pack1 /extract srcpack tardir\n\n"
            << L"   srcpack=Name of source package, without extension\n"
            << L"   tardir=Top directory to extract to\n"
            << kCIDLib::NewEndLn;
}


//
//  Called at startup to parse input parms, which we leave in globals for later
//  use.
//
static tCIDLib::TBoolean bParseParms()
{
    const tCIDLib::TCard4 c4Count = TSysInfo::c4CmdLineParmCount();

    // Have to have at least 3 parameters
    if (c4Count < 3)
    {
        ShowUsage();
        return kCIDLib::False;
    }

    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    TString strPVal;

    // Get the pack/extract mode
    strPVal = *cursParms++;
    if (strPVal.bCompareI(L"/pack"))
        bPackMode = kCIDLib::True;
    else if (strPVal.bCompareI(L"/extract"))
        bPackMode = kCIDLib::False;
    else
    {
        ShowUsage();
        return kCIDLib::False;
    }

    // Get the source dir and target file
    pathSrc = *cursParms++;
    pathTar = *cursParms++;

    // Check for any optional parms
    for (; cursParms; ++cursParms)
    {
        strPVal = *cursParms;

        if (strPVal.bCompareI(L"/Verbose"))
        {
            bVerbose = kCIDLib::True;
        }
         else if (strPVal.bStartsWithI(L"/Notes="))
        {
            strPVal.Cut(0, 7);
            strNotes = strPVal;
        }
         else if (strPVal.bStartsWithI(L"/Version="))
        {
            strPVal.Cut(0, 9);

            //
            //  It has to be in the form M.m.r. We parse it out and put it into
            //  a 64 bit value in the form:
            //
            //      1 2 3 4 5 6 7 8
            //      M M m m r r r r
            //
            tCIDLib::TCard4 c4Maj, c4Min, c4Rev;
            TString strMaj, strMin, strRev;
            TStringTokenizer stokVer(&strPVal, L".");
            if (!stokVer.bGetNextToken(strMaj)
            ||  !stokVer.bGetNextToken(strMin)
            ||  !stokVer.bGetNextToken(strRev)
            ||  !strMaj.bToCard4(c4Maj, tCIDLib::ERadices::Dec)
            ||  !strMin.bToCard4(c4Min, tCIDLib::ERadices::Dec)
            ||  !strRev.bToCard4(c4Rev, tCIDLib::ERadices::Dec))
            {
                conOut  << L"The Version parameter must be in the form M.m.r"
                        << kCIDLib::NewEndLn;
                return kCIDLib::False;
            }

            // Build up the 64 bit value
            c8Version = c4Maj;
            c8Version <<= 16;
            c8Version |= c4Min;
            c8Version <<= 32;
            c8Version |= c4Rev;
        }
         else
        {
            conOut  << L"One or more optional parameters were unknown"
                    << kCIDLib::NewEndLn;
            return kCIDLib::False;
        }
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  Parse the input parms. This will just store the parsed values in
        //  globals.
        //
        if (!bParseParms())
            return tCIDLib::EExitCodes::BadParameters;

        if (bPackMode)
        {
            //
            //  In order to build the directory tree we use as the source material,
            //  we have to do a find on the top level directory, which also lets us
            //  check that it exists and bail out if not.
            //
            TFindBuf fndbStart;
            if (!TFileSys::bExists(pathSrc, fndbStart, tCIDLib::EDirSearchFlags::NormalDirs))
            {
                conOut  << L"Source directory was not found" << kCIDLib::NewEndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }

            //
            //  Make the call to create the package. We pass our console stream
            //  as a status output stream, so he'll output status info for us.
            //
            conOut << L"\nCreating Package "
                    << L"\n     to: " << pathTar
                    << L"\n   from: " << pathSrc
                    << L"\n-------------------------------------------"
                    << kCIDLib::EndLn;

            if (TFileSys::c4BuildFileTree(kCIDLib::pszAllFilesSpec, fndbStart))
            {
                // The True parm says target file overwrite OK
                tCIDLib::TCard4 c4SoFar = 0;
                facCIDPack().CreatePackage
                (
                    pathTar
                    , pathSrc
                    , kCIDLib::True
                    , fndbStart
                    , c8Version
                    , TTime::enctNow()
                    , strNotes
                    , &conOut
                    , c4SoFar
                    , bVerbose
                );
            }
             else
            {
                conOut << L"\n  No files were found" << kCIDLib::EndLn;
            }
        }
         else
        {
            tCIDLib::TCard8         c8Version;
            tCIDLib::TEncodedTime   enctStamp;
            tCIDLib::TCard4         c4TotalFiles;
            TString                 strNotes;

            //
            //  Extract the header info. We'll get it again below, but we want to
            //  display it at the start.
            //
            facCIDPack().ExtractDetails
            (
                pathSrc, c8Version, enctStamp, strNotes, c4TotalFiles
            );

            // Format out the version
            TString strVer;
            strVer.AppendFormatted(c8Version >> 48);
            strVer.Append(kCIDLib::chPeriod);
            strVer.AppendFormatted((c8Version >> 32) & kCIDLib::c2MaxCard);
            strVer.Append(kCIDLib::chPeriod);
            strVer.AppendFormatted(c8Version & kCIDLib::c4MaxCard);

            // Set up a time object with the time stamp and desired formatting
            TTime tmFmt(enctStamp);
            tmFmt.strDefaultFormat(TTime::strCTime());

            conOut << L"\nExtracting Package "
                    << L"\n      From: " << pathSrc
                    << L"\n        To: " << pathTar
                    << L"\n     Notes: " << strNotes
                    << L"\n     Count: " << c4TotalFiles
                    << L"\n   Version: " << strVer
                    << L"\n     Stamp: " << tmFmt
                    << L"\n-------------------------------------------"
                    << kCIDLib::EndLn;


            tCIDLib::TCard4 c4SoFar = 0;
            facCIDPack().ExtractPackage
            (
                pathSrc
                , pathTar
                , kCIDLib::True
                , c8Version
                , enctStamp
                , strNotes
                , c4TotalFiles
                , &conOut
                , c4SoFar
                , kCIDLib::True
            );
        }
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        conOut << L"\n\nCIDLib Exception occurred\n"
                << errToCatch << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"\n\nCIDKernel exception occurred ("
                << kerrToCatch.errcId()
                << L"/" << kerrToCatch.errcHostId()
                << L")"
                << kCIDLib::NewEndLn;

        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"\n\nUnknown exception occurred" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

