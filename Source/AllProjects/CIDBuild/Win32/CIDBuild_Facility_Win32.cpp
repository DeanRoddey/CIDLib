//
// FILE NAME: CIDBuild_Facility_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/1999
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
//  This file provides the Win32 specific parts of the TFacCIDBuild class.
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

//
//  This method is called by the platform independent stuff to have us take
//  the user provided maj/min values and build a version string suffix that
//  is appropriate for the platform. For us, _ is a legal file name so we do
//  in the form M_n which is commonly done in Windows.
//
tCIDLib::TVoid
TFacCIDBuild::BuildVersionString(       TBldStr&        strToFill
                                , const tCIDLib::TCard4 c4MajVer
                                , const tCIDLib::TCard4 c4MinVer)
{
    strToFill = L"_";
    strToFill.Append(c4MajVer);
    strToFill.Append(L"_");
    strToFill.Append(c4MinVer);
}


//
//  This method is called to give us a chance to copy over the binary files
//  files for some utilities that are shipped prebuilt. For now its just
//  the CIDMsgs program.
//
tCIDLib::TVoid
TFacCIDBuild::CopyUtilReleaseFiles(const TBldStr& strTargetDir)
{
    TBldStr strSrc(m_strOutDir);
    strSrc.Append(L"CIDMsgs");
    strSrc.Append(kCIDBuild::pszExeExt);

    TBldStr strDest(strTargetDir);
    strDest.Append(L"Bin\\CIDMsgs.Exe");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << L"Could not copy the CIDMsgs utility" << kCIDBuild::EndLn;
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
tCIDLib::TVoid
TFacCIDBuild::CopyLibReleaseFiles(  const   TBldStr&            strTargetDir
                                    , const TProjectInfo&       projiCur
                                    , const tCIDLib::TBoolean   bCopyLibFile)
{
    TBldStr strSrc;
    TBldStr strDest;

    //
    //  Build up the source/dest paths for the DLL file and copy it over.
    //
    strSrc = projiCur.strOutBin();
    strDest = strTargetDir;
    strDest.Append(L"Bin\\");
    strDest.Append(projiCur.strProjectName());
    strDest.Append(m_strVersionSuffix);
    strDest.Append(L".Dll");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut << L"Could not copy file: " << strSrc << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    // Create a list that we'll use for directory searches
    TList<TFindInfo> listFiles;
    TListCursor<TFindInfo> cursFiles(&listFiles);

    //
    //  Copy over any .CIDMsg files that have this base file name. This will
    //  get us all of the language versions that are there.
    //
    strSrc = m_strOutDir;
    strSrc.Append(projiCur.strProjectName());
    strSrc.Append(m_strVersionSuffix);
    strSrc.Append(L"_??.CIDMsg");
    if (TFindInfo::c4FindFiles(strSrc, listFiles))
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
            strSrc = m_strOutDir;
            strSrc.Append(fndiCur.strFileName());
            strDest = strTargetDir;
            strDest.Append(L"Bin\\");
            strDest.Append(fndiCur.strFileName());

            if (!TUtils::bCopyFile(strSrc, strDest))
            {
                stdOut  << L"Could not copy file: " << strSrc
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CopyFailed;
            }
        }   while (cursFiles.bNext());
    }

    // Copy over the Lib file if asked to
    if (bCopyLibFile)
    {
        strSrc = m_strOutDir;
        strSrc.Append(projiCur.strProjectName());
        strSrc.Append(m_strVersionSuffix);
        strSrc.Append(kCIDBuild::pszLibExt);

        strDest = strTargetDir;
        strDest.Append(L"Lib\\");
        strDest.Append(projiCur.strProjectName());
        strDest.Append(m_strVersionSuffix);
        strDest.Append(kCIDBuild::pszLibExt);
        if (!TUtils::bCopyFile(strSrc, strDest))
        {
            stdOut << L"Could not copy file: " << strSrc << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CopyFailed;
        }
    }
}


//
//  This method is called after all of the regular release work has been
//  done on building a binary or development release. This gives the platform
//  a chance to copy over any miscellaneous platform specific stuff.
//
tCIDLib::TVoid
TFacCIDBuild::MakePlatRelease(  const   tCIDBuild::EActions eAction
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
    if ((m_eBldMode == tCIDBuild::EBldModes::Develop)
    &&  (eAction == tCIDBuild::EActions::MakeDevRelease))
    {
        //
        //  Lets first make our platform specific subdirectory under the Source
        //  directory, which was already created.
        //
        strDest = strTargetDir;
        strDest.Append(L"\\");
        strDest.Append(L"Source");
        strDest.Append(L"\\");
        strDest.Append(kCIDBuild::pszPlatformDir);
        if (!TUtils::bExists(strDest))
        {
            if (!TUtils::bMakeDir(strDest))
            {
                stdOut  << L"Could not create output directory: "
                        << strDest << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }

        // And now iterate the projects and copy the cpp files for each library project
        strDest = strTargetDir;
        strDest.Append(L"Source");
        strDest.Append(L"\\");
        cursProjs.bResetIter();
        do
        {
            //
            //  Get the current project out of the cursor and see if its a
            //  DLL. If so, lets process its Cpp files.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if ((projiCur.eType() == tCIDBuild::EProjTypes::SharedObj)
            ||  (projiCur.eType() == tCIDBuild::EProjTypes::SharedLib)
            ||  (projiCur.eType() == tCIDBuild::EProjTypes::StaticLib))
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
                            stdOut  << L"Could not copy file: "
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
    //  Lets replicate the tree of VC++ projects. The utilities provide a tree
    //  replication method that we can use. We just have to provide the source
    //  and destination directories.
    //
    strSrc = m_strRootDir;
    strSrc.Append(L"Source\\SampleProjs\\");
    strSrc.Append(kCIDBuild::pszPlatformDir);
    strSrc.Append(L"\\VC\\");
    strDest = strTargetDir;
    strDest.Append(L"Samples\\VCProjs\\");
    if (!TUtils::bReplicateTree(strSrc, strDest))
    {
        stdOut  << L"Could not replicate the sample projects directory"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    //
    //  Copy some selected Cmd files that will be of use in the development
    //  build.
    //
    strSrc = m_strRootDir;
    strSrc.Append(L"Source\\Cmd\\");
    strSrc.Append(kCIDBuild::pszPlatformDir);
    strSrc.Append(L"SetDevEnv.Cmd");
    strDest = strTargetDir;
    strDest.Append(L"SetDevEnv.Cmd");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << L"Could not copy the file: " << strSrc
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    //
    //  Copy all the the miscellaneous files over to the MiscFiles/ directory
    //  in the output.
    //
    strSrc = m_strRootDir;
    strSrc.Append(L"Source\\MiscFiles\\");
    strSrc.Append(kCIDBuild::pszPlatformDir);
    strSrc.Append(L"\\");
    strDest = strTargetDir;
    strDest.Append(L"MiscFiles\\");
    if (!TUtils::bCopyAll(strSrc, strDest))
    {
        stdOut  << L"Could not replicate the misc files directory"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    //
    //  Make another copy of the readme file into the root directory so that
    //  its easy to find.
    //
    strSrc = m_strRootDir;
    strSrc.Append(L"Source\\MiscFiles\\");
    strSrc.Append(kCIDBuild::pszPlatformDir);
    strSrc.Append(L"\\ReadMe.Txt");
    strDest = strTargetDir;
    strDest.Append(L"ReadMe.Txt");
    if (!TUtils::bCopyFile(strSrc, strDest))
    {
        stdOut  << L"Could not copy the readme file"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    //
    //  And copy over all of the Word format public docs to the PublicDocs
    //  directory.
    //
    strSrc = m_strRootDir;
    strSrc.Append(L"Source\\PublicDocs\\");
    strDest = strTargetDir;
    strDest.Append(L"PublicDocs\\");
    if (!TUtils::bCopyAll(strSrc, strDest))
    {
        stdOut  << L"Could not replicate the public docs directory"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }


    //
    //  Get a cursor for the project list. We'll use this to iterate through
    //  the projects and copy over some stuff for each DLL project.
    //
    if ((m_eBldMode == tCIDBuild::EBldModes::Develop)
    &&  (eAction == tCIDBuild::EActions::MakeDevRelease))
    {
        cursProjs.bResetIter();
        do
        {
            //
            //  Get the current project out of the cursor and see if its a
            //  library type. If so, lets copy over some of its stuff.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if ((projiCur.eType() == tCIDBuild::EProjTypes::SharedLib)
            ||  (projiCur.eType() == tCIDBuild::EProjTypes::SharedObj))
            {
                //
                //  Get the core part of the output file name, which we will
                //  use a number of places below.
                //
                TBldStr strBaseName = projiCur.strOutBin();
                tCIDLib::TCard4 c4Path, c4Ext;
                TUtils::FindPathParts(strBaseName, c4Path, c4Ext);
                strBaseName.CapAt(c4Ext);
                strBaseName.Cut(c4Path);

                // Copy over the pdb file
                strSrc = m_strOutDir;
                strSrc.Append(strBaseName);
                strSrc.Append(L".pdb");

                strDest = strTargetDir;
                strDest.Append(L"Bin\\");
                strDest.Append(strBaseName);
                strDest.Append(L".pdb");

                if (!TUtils::bCopyFile(strSrc, strDest))
                {
                    stdOut  << L"Could not copy pdb file: "
                            << strSrc << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }

                // Copy over the idb file
                strSrc = m_strOutDir;
                strSrc.Append(strBaseName);
                strSrc.Append(L".idb");

                strDest = strTargetDir;
                strDest.Append(L"Bin\\");
                strDest.Append(strBaseName);
                strDest.Append(L".idb");

                if (!TUtils::bCopyFile(strSrc, strDest))
                {
                    stdOut  << L"Could not copy idb file: "
                            << strSrc << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }

                // And copy over the map file
                strSrc = m_strOutDir;
                strSrc.Append(strBaseName);
                strSrc.Append(L".map");

                strDest = strTargetDir;
                strDest.Append(L"Bin\\");
                strDest.Append(strBaseName);
                strDest.Append(L".map");

                if (!TUtils::bCopyFile(strSrc, strDest))
                {
                    stdOut  << L"Could not copy map file: "
                            << strSrc << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }
            }
        }   while (cursProjs.bNext());
    }
}
