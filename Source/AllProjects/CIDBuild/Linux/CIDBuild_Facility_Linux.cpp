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
tCIDLib::TVoid
TFacCIDBuild::BuildVersionString(       TBldStr&        strToFill
                                , const tCIDLib::TCard4 c4MajVer
                                , const tCIDLib::TCard4 c4MinVer)
{
    strToFill = L".";
    strToFill.Append(c4MajVer);
    strToFill.Append(L".");
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

    TBldStr strDest(strTargetDir);
    strDest.Append(L"bin/CIDMsgs");
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
    strDest.Append(L"bin/");
    tCIDLib::TCard4 c4Name;
    tCIDLib::TCard4 c4Ext;
    TUtils::FindPathParts(projiCur.strOutBin(), c4Name, c4Ext);
    strDest.Append(projiCur.strOutBin().pszBuffer() + c4Name);
    //strDest.Append(m_strVersionSuffix);
    //strDest.Append(L".Dll");
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
    strSrc.Append(L"_*.CIDMsg");
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
            strDest.Append(L"bin/");
            strDest.Append(fndiCur.strFileName());

            if (!TUtils::bCopyFile(strSrc, strDest))
            {
                stdOut  << L"Could not copy file: " << strSrc
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CopyFailed;
            }
        }   while (cursFiles.bNext());
    }
}

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
        strDest.Append(kCIDLib::chPathSep);
        strDest.Append(L"Source");
        strDest.Append(kCIDLib::chPathSep);
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

        // And now iterate the projects and copy the cpp files for each DLL project
        strDest = strTargetDir;
        strDest.Append(L"Source");
        strDest.Append(kCIDLib::chPathSep);
        cursProjs.bResetIter();
        do
        {
            //
            //  Get the current project out of the cursor and see if its a
            //  DLL. If so, lets process its Cpp files.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if (projiCur.eType() == tCIDBuild::EProjTypes::SharedLib)
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
}
