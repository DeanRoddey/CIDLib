//
// FILE NAME: CIDBuild_FacilityActions.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This file contains some of the overflow from the CIDBuild_Facility.Cpp
//  file. It is kind of large, so the code associated with the performing of
//  actions is split out to here.
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
#include    "CIDBuild.hpp"


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static tCIDLib::TVoid MakeDirs( const   tCIDBuild::EActions     eAction
                                , const tCIDBuild::EBldModes    eMode
                                , const TBldStr&                strTargetDir)
{
    //
    //  Create all of the immediate subdirectories that we require to do this
    //  work. There is a different set according to the kind of release we
    //  are doing.
    //
    const tCIDLib::TCh** ppszSubDirs;
    tCIDLib::TCard4 c4SubDirCount;
    if (eAction == tCIDBuild::EActions::MakeDevRelease)
    {
        const tCIDLib::TCh* apszSubDirs[] =
        {
            L"Bin"
            , L"Lib"
            , L"Include"
            , L"PublicDocs"
            , L"Samples"
            , L"MiscFiles"
            , L"SceneFiles"
        };
        c4SubDirCount = sizeof(apszSubDirs) / sizeof(apszSubDirs[0]);
        ppszSubDirs = apszSubDirs;
    }
     else
    {
        static const tCIDLib::TCh* apszSubDirs[] =
        {
            L"Bin"
        };
        c4SubDirCount = sizeof(apszSubDirs) / sizeof(apszSubDirs[0]);
        ppszSubDirs = apszSubDirs;
    }

    tCIDLib::TCard4 c4Index;
    TBldStr strTmpDir;
    for (c4Index = 0; c4Index < c4SubDirCount; c4Index++)
    {
        strTmpDir = strTargetDir;
        strTmpDir.Append(ppszSubDirs[c4Index]);

        if (!TUtils::bExists(strTmpDir))
        {
            if (!TUtils::bMakeDir(strTmpDir))
            {
                stdOut  << L"Could not create output directory: "
                        << strTmpDir << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }
    }

    if (eAction == tCIDBuild::EActions::MakeDevRelease)
    {
        strTmpDir = strTargetDir;
        strTmpDir.Append(L"Include");
        strTmpDir.Append(kCIDBuild::pszPathSep);
        strTmpDir.Append(kCIDBuild::pszPlatformDir);
        if (!TUtils::bExists(strTmpDir))
        {
            if (!TUtils::bMakeDir(strTmpDir))
            {
                stdOut  << L"Could not create output directory: "
                        << strTmpDir << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }

        // Create the Source directory
        if (eMode == tCIDBuild::EBldModes::Develop)
        {
            strTmpDir = strTargetDir;
            strTmpDir.Append(L"Source");
            if (!TUtils::bExists(strTmpDir))
            {
                if (!TUtils::bMakeDir(strTmpDir))
                {
                    stdOut  << L"Could not create output directory: "
                            << strTmpDir << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CreateError;
                }
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  TFacCIDBuild: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFacCIDBuild::MakeRelease(  const   tCIDBuild::EActions eAction
                            , const TBldStr&            strTargetDir)
{
    TBldStr strTmpName;
    TBldStr strTmpDir;
    TBldStr strSrc;
    TBldStr strDest;

    //
    //  Make sure that there is at least one project defined in the project
    //  list.
    //
    if (!plistProjects().c4ProjectCount())
    {
        stdOut << L"No projects are defined\n" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    //
    //  Make sure that the main target directory exists. If not, then try to
    //  create it.
    //
    if (!TUtils::bExists(strTargetDir))
    {
        if (!TUtils::bMakeDir(strTargetDir))
        {
            stdOut  << L"Could not create output directory: "
                    << strTargetDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    // Create all of the output directories
    MakeDirs(eAction, m_eBldMode, strTargetDir);

    // Get a list of projects from the project list, and a cursor for the the list
    const TList<TProjectInfo>& listProjs = plistProjects().listProjects();
    TListCursor<TProjectInfo> cursProjs(&listProjs);

    //
    //  Ok, we have to copy over the SharedLib/SharedMsg files and their
    //  associated CIDMsg files to the ./Bin subdirectory of the output dir,
    //  and the Lib files to the ./Lib dir.
    //
    cursProjs.bResetIter();
    do
    {
        //
        //  Get a reference to the current project and see if its a DLL
        //  type. If not, then move forward and check out another.
        //
        const TProjectInfo& projiCur = cursProjs.tCurElement();
        if ((projiCur.eType() == tCIDBuild::EProjTypes::SharedObj)
        ||  (projiCur.eType() == tCIDBuild::EProjTypes::SharedLib))
        {
            //
            //  Ask the platform support to copy over the DLL/SharedLib and
            //  CIDMsg files to the target directory. If this is a development
            //  release, then copy over the Lib file as well.
            //
            CopyLibReleaseFiles
            (
                strTargetDir
                , projiCur
                , (eAction == tCIDBuild::EActions::MakeDevRelease)
            );
        }
    }   while (cursProjs.bNext());

    //
    //  The rest of this stuff is only for development releases until we get
    //  to the per-platform stuff.
    //
    if (eAction == tCIDBuild::EActions::MakeDevRelease)
    {
        //
        //  Lets copy over all of the Hpp files for all of the projects that
        //  are DLL types. Build up the destiation directory before we enter
        //  the loop since its always the same for this operation.
        //
        strDest = strTargetDir;
        strDest.Append(L"Include");
        strDest.Append(kCIDBuild::pszPathSep);
        cursProjs.bResetIter();
        do
        {
            //
            //  Get the current project out of the cursor and see if its any
            //  of the types that can export headers. If so, lets process its
            //  Hpp files.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if ((projiCur.eType() == tCIDBuild::EProjTypes::SharedObj)
            ||  (projiCur.eType() == tCIDBuild::EProjTypes::SharedLib)
            ||  (projiCur.eType() == tCIDBuild::EProjTypes::StaticLib))
            {
                //
                //  Iterate this project's Hpp files and copy over all of the
                //  ones that are not private.
                //
                TList<TFindInfo>::TCursor cursHpps(&projiCur.listHpps());
                if (cursHpps.bResetIter())
                {
                    do
                    {
                        // Get the current find info object
                        const TFindInfo& fndiCur = cursHpps.tCurElement();

                        // If its not a private header, then lets copy it over
                        if (!TUtils::bIsPrivateHeader(fndiCur.strFileName()))
                        {
                            if (!TUtils::bCopyFile( fndiCur.strFileName()
                                                    , projiCur.strProjectDir()
                                                    , fndiCur.strFileName()
                                                    , strDest))
                            {
                                strTmpName = projiCur.strProjectDir();
                                strTmpName.Append(fndiCur.strFileName());
                                stdOut  << L"Could not copy file: "
                                        << strTmpName << kCIDBuild::EndLn;
                                throw tCIDBuild::EErrors::CopyFailed;
                            }
                        }
                    }   while (cursHpps.bNext());
                }
            }
        }
        while (cursProjs.bNext());

        //
        //  For all of the projects that are samples, we have to copy over
        //  their source to the output directory under the samples directory.
        //
        cursProjs.bResetIter();
        do
        {
            //
            //  Get a reference to the current project and see if its a sample
            //  type. If so, then copy its files to the output directory.
            //
            const TProjectInfo& projiCur = cursProjs.tCurElement();
            if (projiCur.bIsSample())
            {
                // Build up the target directory for this project
                strDest = strTargetDir;
                strDest.Append(L"Samples");
                strDest.Append(kCIDBuild::pszPathSep);
                strDest.Append(projiCur.strProjectName());
                strDest.Append(kCIDBuild::pszPathSep);

                // Create the target directory
                if (!TUtils::bExists(strDest))
                {
                    if (!TUtils::bMakeDir(strDest))
                    {
                        stdOut  << L"Could not create output directory: "
                                << strDest << kCIDBuild::EndLn;
                        throw tCIDBuild::EErrors::CreateError;
                    }
                }

                // And call all the files to the target
                if (!TUtils::bCopyAll(projiCur.strProjectDir(), strDest))
                {
                    stdOut  << L"Could not copy files for sample: "
                            << projiCur.strProjectName() << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }

                //
                //  However, the .Depend file is not one that we want to keep,
                //  so lets delete it if it exists.
                //
                strTmpName = strDest;
                strTmpName.Append(projiCur.strProjectName());
                strTmpName.Append(L".Depend");
                TUtils::bDeleteFile(strTmpName);
            }
        }
        while (cursProjs.bNext());

        // We need to copy over all the ray tracer scene files
        strSrc = facCIDBuild.strRootDir();
        strSrc.Append(L"Source");
        strSrc.Append(kCIDBuild::chPathSep);
        strSrc.Append(L"SceneFiles");
        strSrc.Append(kCIDBuild::chPathSep);
        strDest = strTargetDir;
        strDest.Append(L"SceneFiles");
        strDest.Append(kCIDBuild::chPathSep);
        if (!TUtils::bCopyAll(strSrc, strDest))
        {
            stdOut  << L"Could not copy ray tracer scene files"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CopyFailed;
        }
    }

    // Ask the platform code to copy over some utility projects
    CopyUtilReleaseFiles(strTargetDir);

    //
    //  Now, lets give the per-platform code a chance to copy over any stuff
    //  that is specific to it.
    //
    MakePlatRelease(eAction, strTargetDir);
}

