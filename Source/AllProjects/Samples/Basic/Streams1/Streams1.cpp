//
// FILE NAME: Streams1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  This is the main module for the first of the streaming oriented demo
//  programs. This one just demonstrates the basics of text streaming.
//
//  This guy opens a test file, SourceTestFile.Text, which consists of a
//  list of words and their definitions. They are in this form:
//
//      Word - Definition text
//
//  The file is read a line at a time and then the word part and the
//  definition text part are pulled out into separate strings. Then this
//  information is output in a simple columnar report format.
//
//  The output is sent to the standard output, via the console object that
//  is also used for all the other output.
//
//
//  Like most of the basic samples, this one doesn't create a facility object, it
//  just starts a main thread on a local function.
//
// CAVEATS/GOTCHAS:
//
//  1)  This simple program does not attempt to be language independent.
//
//  2)  This program also does not create a facility object of its own because
//      its so simple that it has no need for one.
//
//  3)  It is assumed that this program is run from the project directory,
//      where the test source file is. If not, it displays an error that it
//      cannot find the file.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Local constant data
//
//  strTestFile
//      This is the name of the test file that we read in our test text
//      from.
// ----------------------------------------------------------------------------
static const TString    strTestFile(L"SourceTestFile.Text");


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Streams1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This method is called to do the work, from the main thread function
//  below.
//
static tCIDLib::TVoid DoWork()
{
    // See if the file exists
    if (!TFileSys::bExists(strTestFile))
    {
        conOut  << L"The test source file " << strTestFile
                << L" was not found" << kCIDLib::NewEndLn;
        return;
    }

    //
    //  First of all lets see if we can find and open our test file. We
    //  use the TTextFileStream class which is a convenience class. It
    //  is a text stream which understand how to use a file as its data
    //  sink/source. We assume ascii format first, but we auto sense the
    //  actual format after its open.
    //
    TTextFileInStream strmSource
    (
        strTestFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
    );

    //
    //  Create two stream format objects. One is for the descriptive
    //  word column and one is for the descriptive text column.
    //
    TStreamFmt  strmfWord(24, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
    TStreamFmt  strmfDescription(0, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    //
    //  Now output the header lines. We put out a title for each column and
    //  then underline them.
    //
    //  The RepChars() mechanism does not honor the formatting system. It
    //  just puts out the requested number of the passed character. The
    //  ESpaces() scheme, which is built on the other mechanism, is the
    //  same. If we just dumped "  ", it would be formatted and justified
    //  according to which of the formatting schemes was on the stream at
    //  the time.
    //
    conOut  << kCIDLib::DNewLn
            << strmfWord << L"Term"
            << TTextOutStream::Spaces(4)
            << strmfDescription << L"Description of Term"
            << kCIDLib::NewLn
            << TTextOutStream::RepChars(L'-', 24)
            << TTextOutStream::Spaces(4)
            << TTextOutStream::RepChars(L'-', 32)
            << kCIDLib::EndLn;

    //
    //  Ok, so lets read in each line. If its not empty, then we parse it
    //  to get out the term and its description. We then output the term
    //  and its text in the same way that we just did the titles.
    //
    TString strInput;
    TString strTerm;
    TString strText;
    TStringTokenizer stokInput(&strInput, L"-");

    //
    //  Just read until the end of input, processing each line. Let any
    //  exceptions just propogate back to the caller.
    //
    while (!strmSource.bEndOfStream())
    {
        // Get a line of text. Skip it if its empty
        strmSource >> strInput;
        strInput.StripWhitespace();
        if (!strInput.bIsEmpty())
        {
            // Now break out the two parts using the tokenizer
            stokInput.Reset();
            if (!stokInput.bGetNextToken(strTerm)
            ||  !stokInput.bGetNextToken(strText))
            {
                conOut  << L"Got an invalid line from the test file"
                        << kCIDLib::EndLn;
                continue;
            }

            // Strip any whitespace off them
            strTerm.StripWhitespace();
            strText.StripWhitespace();

            // And output them
            conOut  << strmfWord << strTerm
                    << TTextOutStream::Spaces(4)
                    << strmfDescription << strText
                    << kCIDLib::EndLn;
        }
    }
}


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        DoWork();
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

        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\n  Error="
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

