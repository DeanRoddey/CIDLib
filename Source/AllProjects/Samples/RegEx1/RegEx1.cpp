//
// FILE NAME: Regex1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/1998
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
//  This is the main module for the first of the regular expression demo
//  programs. This program implements a simple 'grep' program, where the
//  user gives a file name and a regular expression. The file is read one
//  line at a time and each line that matches the regular expression is
//  displayed.
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
#include    "CIDRegX.hpp"


// ---------------------------------------------------------------------------
//  Local, static data
//
//  bCaseSensitive
//      Indicates whether we should do case sensitive searches. Defaults to
//      true, and is overriden by /NoCase command line option.
//
//  bFullMatch
//      Indicates whether only full line matches are counted. Defaults to false
//      and is overridden by the /Full command line option.
//
//  c4Matches
//      The number of lines that matched the pattern. This is bumped up as each
//      match is made.
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
//
//  strFile
//      Filled in with the name of the file to search.
//
//  strRegEx
//      Fill in with the regular expression provided by the user.
// ---------------------------------------------------------------------------
tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True;
tCIDLib::TBoolean       bFullMatch = kCIDLib::False;
tCIDLib::TCard4         c4Matches = 0;
TOutConsole             conOut;
TString                 strFile;
TString                 strRegEx;


// ---------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"RegEx1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method does the meat of the program. it gets the name of the file
//  (which is known to exist) and the compiled regular expression to search.
//
static tCIDLib::TVoid SearchFile(TRegEx& regxToFind)
{
    // Get a text stream for the file.
    TTextFileInStream strmSrc
    (
        strFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
    );

    //
    //  Set up a couple stream format objects. One is for the line number
    //  column, and the other is for the other stuff.
    //
    TStreamFmt strmfOther;
    TStreamFmt strmfLine(5, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);

    //
    //  Ok, lets go into a line reading loop. On each round, we just read
    //  a line and test it against the regular expression. We keep up with
    //  the line number which is output also.
    //
    //  Instead of testing for end of file, we just catch the 'read at end'
    //  error.
    //
    tCIDLib::TCard4 c4LineNum = 0;
    try
    {
        TString strLine;
        while (!strmSrc.bEndOfStream())
        {
            // Get a line and bump the counter
            strmSrc.c4GetLine(strLine);
            c4LineNum++;

            // Test the line against the regular expression
            tCIDLib::TBoolean   bFind;
            tCIDLib::TCard4     c4Ofs = 0;
            tCIDLib::TCard4     c4Len = 0;

            if (bFullMatch)
            {
                bFind = regxToFind.bFullyMatches(strLine, bCaseSensitive);
            }
             else
            {
                bFind = regxToFind.bFindMatch
                (
                    strLine
                    , c4Ofs
                    , c4Len
                    , kCIDLib::False
                    , bCaseSensitive
                );
            }

            if (bFind)
            {
                c4Matches++;

                // Strip off leading whitespace of the line
                strLine.StripWhitespace();

                //
                //  If its longer than 52 characters, then clip it and append
                //  an ellipsis.
                //
                if (strLine.c4Length() > 52)
                {
                    strLine.CapAt(52);
                    strLine.Append(L"...");
                }

                conOut  << strmfOther << L"  Match at line: "
                        << strmfLine << TCardinal(c4LineNum)
                        << strmfOther << L" - "
                        << strLine << kCIDLib::EndLn;
            }
        }
    }

    catch(TError& errToCatch)
    {
        conOut << L"Error during process!\n   Error: "
               << errToCatch << kCIDLib::NewEndLn;
    }
}


//
//  Shows the parameter usage for the program.
//
static tCIDLib::TVoid ShowUsage()
{
    conOut << kCIDLib::NewLn
             << L"Usage: RegEx1 [options] regex filename" << kCIDLib::DNewLn
             << L"       Put RegEx in quotes if it has any" << kCIDLib::NewLn
             << L"       spaces or illegal cmd line characters" << kCIDLib::DNewLn
             << L"       Options:" << kCIDLib::NewLn
             << L"          /NoCase     - Do case insensitive match" << kCIDLib::NewLn
             << L"          /Full       - Only matches full lines" << kCIDLib::NewLn
             << kCIDLib::NewEndLn;
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
             << L"RegEx1.Exe, Version: "
             << facCIDLib().strVersion() << kCIDLib::NewLn
             << L"CIDLib Regular Expression Demo"
             << kCIDLib::NewEndLn;

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Check out the command line parms for minimal valid count
        const tCIDLib::TCard4 c4ArgCount = TSysInfo::c4CmdLineParmCount();
        if (c4ArgCount < 2)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Start getting parameters out
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        TString strParm;
        for (; cursParms; ++cursParms)
        {
            // Break out if not an option
            if (cursParms->chFirst() != kCIDLib::chForwardSlash)
                break;

            strParm = *cursParms;
            if (strParm.bCompareI(L"/NoCase"))
            {
                bCaseSensitive = kCIDLib::False;
            }
             else if (strParm.bCompareI(L"/Full"))
            {
                bFullMatch = kCIDLib::True;
            }
             else
            {
                // Not a valid optional switch
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        if (!cursParms)
        {
            // Missing the non-optional parameters
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // The parm we broke out on is the pattern
        strRegEx = *cursParms++;

        // And the next one must be the file
        strFile = *cursParms++;

        // If that was not the last parameter, then bad params
        if (cursParms)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  See if the file exists. If not, then we can just bail out now
        //  of coures.
        //
        if (!TFileSys::bExists(strFile))
        {
            conOut  << L"The input file '" << strFile
                    << L"' does not exist" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::NotFound;
        }

        //
        //  Ok, it exists, so lets try to compile the regular expression
        //  to see if its valid.
        //
        TRegEx regxToFind;
        try
        {
            regxToFind.SetExpression(strRegEx);
        }

        catch(const TError& errToCatch)
        {
            conOut  << L"The regular expression is invalid" << kCIDLib::NewLn
                    << L"  Reason: " << errToCatch.strErrText()
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // We survived, so display the search info
        conOut << L"        File: " << strFile << kCIDLib::NewLn
                 << L"  Expression: " << strRegEx
                 << kCIDLib::NewEndLn;

        //
        //  Do the search of the file for all matches of the regular
        //  expression.
        //
        SearchFile(regxToFind);

        if (!c4Matches)
            conOut << L"  No matches found" << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  <<  L"A CIDLib runtime error occured during processing. "
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
                 << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}
