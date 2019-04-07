//
// FILE NAME: CIDBuild_Facility_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 04/08/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Linux specific parts of the TFacCIDBuild class.
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
#include    "../CIDBuild.hpp"


// ---------------------------------------------------------------------------
//  TFacCIDBuild: Private platform methods
// ---------------------------------------------------------------------------
tCIDBuild::TVoid
TFacCIDBuild::__BuildVersionString(         TBldStr&            strToFill
                                    , const tCIDBuild::TUInt    uiMajVer
                                    , const tCIDBuild::TUInt    uiMinVer)
{
    strToFill = L".";
    strToFill.Append(uiMajVer);
    strToFill.Append(L".");
    strToFill.Append(uiMinVer);
}

//
//  This method is called to give us a chance to copy over the binary files
//  files for some utilities that are shipped prebuilt. For now its just
//  the CIDMsgs program.
//
tCIDBuild::TVoid
TFacCIDBuild::__CopyUtilReleaseFiles(const TBldStr& strTargetDir)
{
    TBldStr strSrc(__strOutDir);
    strSrc.Append(L"CIDMsgs");

    TBldStr strDest(strTargetDir);
    strDest.Append(L"bin/CIDMsgs");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << NStr("Could not copy the CIDMsgs utility") << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
}


//
//  This method is called by the platform independent code for each project
//  that is a DLL/Shared lib style project. This method copies over the
//  binary release files for that project, which consists of whatever makes
//  up the DLL/SharedLib file on that platform, plus the CIDMsg files that
//  are associated with that project, and optionally whatever makes up the
//  'lib' file on that platform.
//
tCIDBuild::TVoid
TFacCIDBuild::__CopyLibReleaseFiles(const   TBldStr&            strTargetDir
                                    , const TProjectInfo&       projiCur
                                    , const tCIDBuild::TBoolean bCopyLibFile)
{
    TBldStr strSrc;
    TBldStr strDest;

    //
    //  Build up the source/dest paths for the DLL file and copy it over.
    //
    strSrc = projiCur.strOutBin();
    strDest = strTargetDir;
    strDest.Append(L"bin/");
    tCIDBuild::TUInt uiName;
    tCIDBuild::TUInt uiExt;
    TUtils::FindPathParts(projiCur.strOutBin(), uiName, uiExt);
    strDest.Append(projiCur.strOutBin().pszBuffer() + uiName);
    //strDest.Append(__strVersionSuffix);
    //strDest.Append(L".Dll");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut << NStr("Could not copy file: ") << strSrc << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    // Create a list that we'll use for directory searches
    TList<TFindInfo> listFiles;
    TListCursor<TFindInfo> cursFiles(&listFiles);

    //
    //  Copy over any .CIDMsg files that have this base file name. This will
    //  get us all of the language versions that are there.
    //
    strSrc = __strOutDir;
    strSrc.Append(projiCur.strProjectName());
    strSrc.Append(__strVersionSuffix);
    strSrc.Append(L"_*.CIDMsg");
    if (TFindInfo::uiFindFiles(strSrc, listFiles))
    {
        //
        //  Cursor through the files we found and copy them over
        //  to the output directory.
        //
        cursFiles.bResetIter();
        do
        {
            // Get the current find info object
            const TFindInfo& fndiCur = cursFiles.tCurElement();

            // Build up the source/dest file names
            strSrc = __strOutDir;
            strSrc.Append(fndiCur.strFileName());
            strDest = strTargetDir;
            strDest.Append(L"bin/");
            strDest.Append(fndiCur.strFileName());

            if (!TUtils::bCopyFile(strSrc, strDest))
            {
                stdOut  << NStr("Could not copy file: ") << strSrc
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CopyFailed;
            }
        }   while (cursFiles.bNext());
    }
}

tCIDBuild::TVoid
TFacCIDBuild::__MakePlatRelease(const   tCIDBuild::EActions eAction
                                , const TBldStr&            strTargetDir)
{
    // Nothing to do for the binary release
    if (eAction == tCIDBuild::EActions::MakeBinRelease)
        return;

    TBldStr strSrc;
    TBldStr strDest;
    TBldStr strTmp;
    TListCursor<TProjectInfo> cursProjs(&plistProjects().listProjects());

    //
    //  If we are in Dev mode and its a Dev release, then we want to copy
    //  the Cpp files of all of the DLL projects to the Source/xx output
    //  directory.
    //
    if ((__eBldMode == tCIDBuild::EBldModes::Develop)
    &&  (eAction == tCIDBuild::EActions::MakeDevRelease))
    {
        //
        //  Lets first make our platform specific subdirectory under the Source
        //  directory, which was already created.
        //
        strDest = strTargetDir;
        strDest.Append(kCIDBuild::pszPathSep);
        strDest.Append(L"Source");
        strDest.Append(kCIDBuild::pszPathSep);
        strDest.Append(kCIDBuild::pszPlatformDir);
        if (!TUtils::bExists(strDest))
        {
            if (!TUtils::bMakeDir(strDest))
            {
                stdOut  << NStr("Could not create output directory: ")
                        << strDest << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }

        // And now iterate the projects and copy the cpp files for each DLL project
        strDest = strTargetDir;
        strDest.Append(L"Source");
        strDest.Append(kCIDBuild::pszPathSep);
        cursProjs.bResetIter();
        do
        {
            //
            //  Get the current project out of the cursor and see if its a
            //  DLL. If so, lets process its Cpp files.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if (projiCur.eType() == tCIDBuild::EProjTypes::Dll)
            {
                TList<TFindInfo>::TCursor cursCpps(&projiCur.listCpps());
                if (cursCpps.bResetIter())
                {
                    do
                    {
                        // Get the current find info object
                        const TFindInfo& fndiCur = cursCpps.tCurElement();

                        if (!TUtils::bCopyFile( fndiCur.strFileName()
                                                , projiCur.strProjectDir()
                                                , fndiCur.strFileName()
                                                , strDest))
                        {
                            TBldStr strTmpName = projiCur.strProjectDir();
                            strTmpName.Append(fndiCur.strFileName());
                            stdOut  << NStr("Could not copy file: ")
                                    << strTmpName << kCIDBuild::EndLn;
                            throw tCIDBuild::EErrors::CopyFailed;
                        }
                    }   while (cursCpps.bNext());
                }

            }
        }
        while (cursProjs.bNext());
    }

    //
    //  Copy some selected Cmd files that will be of use in the development
    //  build.
    //
    /*
    strSrc = __strRootDir;
    strSrc.Append(NStr("Source\\Cmd\\Win32\\SetDevEnv.Cmd"));
    strDest = strTargetDir;
    strDest.Append(NStr("SetDevEnv.Cmd"));
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << NStr("Could not copy the file: ") << strSrc
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
    */

    //
    //  Copy all the the miscellaneous files over to the MiscFiles/ directory
    //  in the output.
    //
    /*
    strSrc = __strRootDir;
    strSrc.Append(NStr("Source\\MiscFiles\\Win32\\"));
    strDest = strTargetDir;
    strDest.Append(NStr("MiscFiles\\"));
    if (!TUtils::bCopyAll(strSrc, strDest))
    {
        stdOut  << NStr("Could not replicate the misc files directory")
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
    */

    //
    //  Make another copy of the readme file into the root directory so that
    //  its easy to find.
    //
    /*
    strSrc = __strRootDir;
    strSrc.Append(NStr("Source\\MiscFiles\\Win32\\ReadMe.Txt"));
    strDest = strTargetDir;
    strDest.Append(NStr("ReadMe.Txt"));
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << NStr("Could not copy the readme file")
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
    */

    //
    //  And copy over all of the Word format public docs to the PublicDocs
    //  directory.
    //
    strSrc = __strRootDir;
    strSrc.Append(NStr("Source/PublicDocs/"));
    strDest = strTargetDir;
    strDest.Append(NStr("PublicDocs/"));
    if (!TUtils::bCopyAll(strSrc, strDest))
    {
        stdOut  << NStr("Could not replicate the public docs directory")
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
}
