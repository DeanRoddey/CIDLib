//
// FILE NAME: FileSys3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/1997
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
//  This is the main module for the third of the file system oriented demo
//  programs. This program is a file renamer. It will take all of the files
//  in a directory and rename them according to a pattern (with a continuously
//  incrementing values.)
//
//  For instance, if you wanted to name all of the files in a directory
//  with names like File0001.Txt, File0002.Txt, File0003.Txt, etc..., this
//  program will do that automatically. You give it a starting directory and a
//  file name pattern to use for renaming. The pattern uses a standard CIDLib
//  replacement token. For the above example, the pattern would be:
//
//      File%(1,4,0).Txt
//
//  This replacement token says that token #1 (which the program looks for)
//  should be right justified in a 4 character string, 0 filled.
//
//  If the pattern has spaces in it, then put it in double quotes. That's
//  legal as long as the target file system allows spaces.
//
//
//  This program is one of the first samples to use loadable text, and error
//  and message ids built from a .MsgText file.
//
//  It also creates a facility object so that it can load its text and log msgs
//  and throw errors that do that same.
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
#include    "FileSys3.hpp"


// ----------------------------------------------------------------------------
//  Typedef our collection type. We use a bag of find buffer objects. In
//  many cases we would use a hash map for this kind of thing, but we have
//  special case sensitive and insensitive name comparisons. Hash maps don't
//  do that well since the hashing scheme takes into account the case of the
//  text of the file name.
// ----------------------------------------------------------------------------
using TFileCol = TBag<TFindBuf>;


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
static tCIDLib::TVoid RenameFiles
(
    const   TFileCol&           colFiles
    , const TString&            strPattern
    , const TString&            strTargetDir
);



// ----------------------------------------------------------------------------
//  Local static data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
//
//  facFileSys3
//      This is our facility object, which we need in order to log errors and
//      load message resources. Since we have no particular needs, we just
//      create a regular TFacility object instead of creating our own derivative
// ----------------------------------------------------------------------------
static TOutConsole  conOut;
static TFacility    facFileSys3
(
    L"FileSys3"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgFile
);


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"FileSys3MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//  Shows the parameter usage for the program.
static tCIDLib::TVoid ShowUsage()
{
    conOut  << kCIDLib::NewLn << facFileSys3.strMsg(kFSMsgs::midUsage)
            << kCIDLib::NewLn << kCIDLib::EndLn;
}



// This is the the thread function for the main thread.
static tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Display the program blurb
        conOut << facFileSys3.strMsg(kFSMsgs::midBlurb1) << kCIDLib::EndLn;

        //
        //  Ok, lets first check out the command line parameters. We need
        //  to make sure that we got both of them, and that they directory
        //  exists and has entries and that the pattern is legal.
        //
        if (TSysInfo::c4CmdLineParmCount() < 2)
        {
            ShowUsage();
            TProcess::ExitProcess(tCIDLib::EExitCodes::BadParameters);
        }

        // Get the pattern parameter and make sure it has a %(1) token
        TString strPattern = TSysInfo::strCmdLineParmAt(1);
        if (!strPattern.bTokenExists(L'1'))
        {
            conOut << facFileSys3.strMsg(kFSErrs::errcInvalidPattern, strPattern)
                   << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  If the target directory is a partial directory, then we need to
        //  complete it.
        //
        TString strTargetDir;
        TString strTmp = TSysInfo::strCmdLineParmAt(0);
        TFileSys::QueryFullPath(strTmp, strTargetDir);

        // If it has a trailing separator, then toss it
        if (strTargetDir.chLast() == L'\\')
            strTargetDir.DeleteLast();

        // See if the target is a directory
        if (!TFileSys::bIsDirectory(strTargetDir))
        {
            conOut << facFileSys3.strMsg(kFSErrs::errcTargetDirNotFound, strTargetDir)
                   << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  See if there is anything in the target. If so, then this will
        //  return us a collection of TFindBuf objects for them. We need
        //  to create a search spec to find all files.
        //
        TPathStr pathSearch(strTargetDir);
        pathSearch.AddLevel(kCIDLib::pszAllFilesSpec);

        // Search the directory for all normal files
        TFileCol colFileList;
        if (!TFileSys::c4SearchDir(pathSearch, colFileList, tCIDLib::EDirSearchFlags::NormalFiles))
        {
            // No matches
            conOut  << facFileSys3.strMsg(kFSErrs::errcEmptyTarget)
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::NotFound;
        }

        // Say what we are about to do
        conOut << facFileSys3.strMsg
                  (
                    kFSMsgs::midRenaming
                    , strTargetDir
                    , strPattern
                  )
               << kCIDLib::NewEndLn;

        //
        //  Ok, it looks basically ok. So lets call the function that actually
        //  does the work.
        //
        RenameFiles(colFileList, strPattern, strTargetDir);
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  << facFileSys3.strMsg(kFSMsgs::midRuntimeErr)
                << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << facFileSys3.strMsg(kFSMsgs::midKrnlErr)
                << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << facFileSys3.strMsg(kFSMsgs::midSysExcept) << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  This function does the grunt work of the program. The parameters have
//  been validated and we are given a collect with the list of files in the
//  target directory, and the target directory and the pattern.
//
static tCIDLib::TVoid RenameFiles(  const   TFileCol&   colFiles
                                    , const TString&    strPattern
                                    , const TString&    strTargetDir)
{
    // Create a counter that will be used for the numbering
    tCIDLib::TCard4 c4Counter = 0;
    TPathStr        pathNewName;

    //
    //  Create a cursor to loop through the list of files. For each one,
    //  we'll try to rename it to the next available target name.
    //
    TFileCol::TCursor cursFiles(&colFiles);
    if (!cursFiles.bIsValid())
        return;

    //
    //  And create a hash set that we'll use to keep up with each file that
    //  we've used (or found to already exist) so far. This lets us check
    //  quickly whether a target name is in use. Tell the key ops not to be
    //  case sensitive, so it will catch names that just differ in case.
    //
    tCIDLib::TStrHashSet colUsed(29, new TStringKeyOps(kCIDLib::False));

    // And loop through the source files
    for (; cursFiles; ++cursFiles)
    {
        //
        //  Create the file name for this counter value. This is our new
        //  target name to rename to.
        //
        pathNewName = strTargetDir;
        pathNewName.AddLevel(strPattern);
        pathNewName.eReplaceToken(TCardinal(c4Counter), L'1');

        //
        //  First see if this guy is already in the collection. If so, then
        //  the file is already there and we don't want to try to rename.
        //
        const tCIDLib::TBoolean bFound = colUsed.bHasElement(pathNewName);

        //
        //  If the new file pattern did not already exist, then we need
        //  to rename the current file in the collection to the new pattern.
        //
        if (!bFound)
        {
            //
            //  Rename the curent file to the new pattern, and add this name
            //  to the used list.
            //
            TFileSys::Rename(cursFiles->pathFileName(), pathNewName);
            colUsed.objAdd(pathNewName);
        }

        // Move the counter up to the next value to use
        c4Counter++;
    }
}


