//
// FILE NAME: Collect1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/24/1997
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
//  This is the main module for the first of the collection oriented demo
//  programs. This one just demonstrates the most basic collection, the
//  bag. It just does a directory iteration of the current directory and puts
//  the resulting find buffer objects into the bag. Then it iterates the bag
//  and dumps out the results to the standard out.
//
//  Note that there are much easier ways to do all of this stuff, via the
//  TFileSys class, but this is just for demonstration purposes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDLib, which is all we need.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our
//      standard output. Its a specialized text stream class. We don't
//      support redirection in this program, so we can just use a console
//      directly.
// ---------------------------------------------------------------------------
static TOutConsole  conOut;


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Collect1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
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
        //  Create a bag of find buffer objects. Find buffer objects are
        //  the results of directory searches.
        //
        TBag<TFindBuf>  colOfFiles;

        // Create a directory iterator for the search
        TDirIter  diterCur;

        //
        //  Create a temporary find buffer object for use in the loop. Set
        //  up a format string for it. This tells it how to format itself
        //  when dumped to a text stream. We use one of the prefab ones
        //  that is supplies.
        //
        //  Note that the find operations below won't override the format
        //  they just fill in the file find information.
        //
        TFindBuf fndbTmp(TFindBuf::strNameAndSize());

        //
        //  Query the current directory. We use a path string to make it
        //  easy to manipulate as a path.
        //
        TPathStr pathCurDir;
        TFileSys::QueryCurrentDir(pathCurDir);

        //
        //  Add on a search extension that will find all matches on the
        //  host platform.
        //
        pathCurDir.AddLevel(kCIDLib::pszAllFilesSpec);

        // Indicate what we are searching for
        conOut << kCIDLib::NewLn << L"Searching for files: "
                 << pathCurDir << kCIDLib::NewLn << kCIDLib::EndLn;

        //
        //  Now start a search on the directory iterator. We use the default
        //  file attributes, which are to just find everything. That's fine
        //  for us here.
        //
        if (!diterCur.bFindFirst(pathCurDir, fndbTmp))
        {
            // If no match, then we can't do very much
            conOut << L"There were no files found." << kCIDLib::NewLn << kCIDLib::EndLn;
        }

        // Continue while we are still finding matches
        do
        {
            // Add this find to the collection
            colOfFiles.objAdd(fndbTmp);
        }   while (diterCur.bFindNext(fndbTmp));

        //
        //  Ok, we have a bag of TFindBuf objects. So lets get a cursor for
        //  the collection, and iterate all the elements, dumping each one
        //  out to the console. We have to pass a pointer to the collection
        //  because the cursor keeps a pointer to the collection. This is a
        //  strict CIDLib convention.
        //
        TBag<TFindBuf>::TCursor cursFiles(&colOfFiles);

        //
        //  Reset the cursor to get it ready for iteration. Just for paranoia
        //  check that it indicates that there are elements.
        //
        if (!cursFiles.bIsValid())
        {
            conOut  << L"The cursor reset indicated no elements in bag!"
                    << kCIDLib::EndLn;
        }
         else
        {
            for (; cursFiles; ++cursFiles)
                conOut << *cursFiles << kCIDLib::NewLn;
            conOut << kCIDLib::EndLn;
        }
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // Log it if not already logged.
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  <<  L"A CIDLib runtime error occured during processing.\n"
                <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
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
