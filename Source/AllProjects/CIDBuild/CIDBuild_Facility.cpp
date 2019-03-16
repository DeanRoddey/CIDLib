//
// FILE NAME: CIDBuild_Facility.Cpp
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
//  This file implements the TFacCIDBuild class, which provides all of the
//  central intelligence of the program. The program's main() just calls our
//  iMain() and we drive it from there.
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
//   CLASS: TFacCIDBuild
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDBuild: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDBuild::TFacCIDBuild() :

    m_bForce(kCIDLib::False)
    , m_bLowPrio(kCIDLib::False)
    , m_bMaxWarn(kCIDLib::False)
    , m_bNonPermissive(kCIDLib::False)
    , m_bNoRecurse(kCIDLib::False)
    , m_bSingle(kCIDLib::False)
    , m_bSupressLogo(kCIDLib::False)
    , m_bVerbose(kCIDLib::False)
    , m_eAction(tCIDBuild::EActions::Build)
    , m_eBldMode(tCIDBuild::EBldModes::Develop)
    , m_eHdrDumpMode(tCIDBuild::EHdrDmpModes::None)
    , m_c4CPUCount(1)
    , m_c4MajVer(0)
    , m_c4MinVer(0)
    , m_pplistProjs(nullptr)
    , m_ptdrvBuild(nullptr)
{
}

TFacCIDBuild::~TFacCIDBuild()
{
    //
    //  Delete the project list object, which is going to clean up a whole
    //  raft of stuff.
    //
    delete m_pplistProjs;

    // Delete our tools driver
    delete m_ptdrvBuild;
}


// ---------------------------------------------------------------------------
//  TFacCIDBuild: Public, non-static methods
// ---------------------------------------------------------------------------

//
//  To do a quick check to see if a given source project depends on another
//  project, directly or indirectly. We pass it on the project list and he uses
//  the depencency graph he has, to figure out the depenencey.
//
tCIDLib::TBoolean
TFacCIDBuild::bDependsOn(   const   TBldStr&            strSrcProj
                            , const TBldStr&            strToCheck
                            , const tCIDLib::TBoolean   bMustExist)
{
    return m_pplistProjs->bDependsOn(strSrcProj, strToCheck, bMustExist);
}


//
//  Get the value of the named macro, if it exists. The return indicates if it was
//  founds or not. it handles both some special ones that are implicitly defined,
//  and the explicitly defined ones.
//
tCIDLib::TBoolean
TFacCIDBuild::bGetMacroValue(const  TBldStr&    strMacroName
                            ,       TBldStr&    strMacroValue)
{
    //
    //  Look up the macro name in the macro list. We don't have any hash table
    //  at this level, so its just a brute force search. But we won't usually
    //  have more than a handful. And most of those used will be special case
    //  ones, which we check first.
    //
    tCIDLib::TBoolean bFound = kCIDLib::True;
    if (strMacroName == L"CIDSrcDir")
    {
        strMacroValue = m_strCIDLibSrcDir;
    }
     else if (strMacroName == L"Mode")
    {
        if (m_eBldMode == tCIDBuild::EBldModes::Develop)
            strMacroValue = L"Dev";
        else
            strMacroValue = L"Prod";
    }
     else if (strMacroName == L"PlatformDir")
    {
        strMacroValue = kCIDBuild::pszPlatformDir;
    }
     else if (strMacroName == L"SrcRoot")
    {
        strMacroValue = m_strRootDir;
    }
     else if (strMacroName == L"OutDir")
    {
        strMacroValue = m_strOutDir;
    }
     else
    {
        bFound = kCIDLib::False;
    }

    // If not a special case, then check the macro list
    if (!bFound)
    {
        TListCursor<TKeyValuePair> cursMacros(&m_listMacros);
        if (cursMacros.bResetIter())
        {
            do
            {
                const TKeyValuePair& kvalCur = cursMacros.tCurElement();
                if (kvalCur.strKey() == strMacroName)
                {
                    strMacroValue = kvalCur.strValue();
                    bFound = kCIDLib::True;

                    // Don't break out, later defs override earlier ones!
                }
            }   while (cursMacros.bNext());
        }
    }

    // If not in the macro list either, check the environment
    if (!bFound)
    {
        bFound = TUtils::bGetEnvVar(strMacroName, strMacroValue);
    }
    return bFound;
}


//
//  The program entry point.
//
tCIDLib::TSInt TFacCIDBuild::iMain(         tCIDLib::TCard4         c4Args
                                    , const tCIDLib::TCh* const*    apszArgs)
{
    try
    {
        stdOut << kCIDBuild::EndLn;

        //
        //  First get the values of any of the environment variables that
        //  we look for. Command line vars overwrite these, so we do these
        //  first and then parse the parms and just let any that we find
        //  there just overwrite what we find here, if any.
        //
        CheckEnv();

        // Parse out the command line parameters
        ParseParms(c4Args, apszArgs);

        //
        //  Allocate the project list object.
        //
        //  !!Note that this requires that the command line parameters alaready
        //  be allocated! So don't move this above the command line parsing
        //  above!!
        //
        m_pplistProjs = new TProjectList;

        // If the logo is not suppressed, then display it
        if (!m_bSupressLogo)
            ShowLogo();

        // If the verbose flag is on, then show our parms
        if (m_bVerbose)
            ShowParms();

        //
        //  Ask the project list to parse the project file and build all
        //  of the project dependency info. He gets any information about
        //  command line stuff from us.
        //
        m_pplistProjs->ParseProjectFile();

        // Store the CPU count
        m_c4CPUCount = TUtils::c4CPUCount();

        //
        //  If the action is the magic 'bootstrap' option, then we need
        //  to follow a special set of events. This is done when an initial
        //  build is done, usually after wasting the output directory.
        //
        if (m_eAction == tCIDBuild::EActions::Bootstrap)
        {
            stdOut << L"Doing a bootstrap build..." << kCIDBuild::EndLn;
            MakeTargetDirs();
            MakeResources(L"CIDIDL");

            //
            //  Load up all of the file lists now. Some of the ones that
            //  depend on IDL generation won't be technically correct,
            //  but that's ok for now.
            //
            m_pplistProjs->LoadFileLists();

            //
            //  And now we can copy out the headers, make dependencies,
            //  and build. All of this is still just for the IDL compiler.
            //
            CopyHeaders(L"CIDIDL");
            MakeDepends(L"CIDIDL");
            Build(L"CIDIDL");

            //
            //  Now we start ops against all projects. So set the target to
            //  all and flush and reparse the project file.
            //
            m_strTarget = kCIDBuild::pszAllProjects;
            m_pplistProjs->RemoveAll();
            m_pplistProjs->ParseProjectFile();

            // Now we can finally project IDL files for all projects
            GenIDLFiles(m_strTarget);

            //
            //  And go back and re-gen resources, and we can now reload the
            //  file list and it will be complete now.
            //
            MakeResources(m_strTarget);
            m_pplistProjs->LoadFileLists();

            // And go back and copy headers and make dependencies for all
            CopyHeaders(m_strTarget);
            MakeDepends(m_strTarget);

            // Now kick off a full build to do all the compile/link stuff
            Build(m_strTarget);

            // And finally do any extra file copies
            DoFileCopies(m_strTarget);

            stdOut << L"Bootstrap complete successfully\n" << kCIDBuild::EndLn;
            return 0;
        }

        //
        //  If the target is empty, then try to figure out if we are in a
        //  project directory. If so, make that project the target. Else,
        //  just fall back to the 'all' project.
        //
        if (m_strTarget.bEmpty())
        {
            //
            //  Get the current directory, which will be used below to check
            //  against the project directories of the defined projects.
            //
            TBldStr strCurDir;
            TUtils::QueryCurDir(strCurDir);

            //
            //  Iterate the projects, and see if one has the current directory
            //  as its project directory.
            //
            TListCursor<TProjectInfo>   cursProjs(&m_pplistProjs->listProjects());
            TBldStr                     strTmp;
            if (cursProjs.bResetIter())
            {
                do
                {
                    const TProjectInfo& projiCur = cursProjs.tCurElement();

                    //
                    //  Get the project directory, then delete the last char,
                    //  which is always a trailing separator. The current dir
                    //  doesn't have a trailing separator, so we have to toss
                    //  it off of the project dir.
                    //
                    strTmp = projiCur.strProjectDir();
                    strTmp.DeleteLast();

                    // If this equals the current directory, then this is our guy
                    if (strCurDir.bIEquals(strTmp))
                    {
                        m_strTarget = projiCur.strProjectName();
                        break;
                    }
                }   while (cursProjs.bNext());
            }

            // If still empty, then do all projects
            if (m_strTarget.bEmpty())
                m_strTarget = kCIDBuild::pszAllProjects;
        }

        //
        //  If the action indicates that we will need to output to the
        //  build target directories, make sure they all exist.
        //
        if ((m_eAction == tCIDBuild::EActions::Build)
        ||  (m_eAction == tCIDBuild::EActions::CopyHeaders)
        ||  (m_eAction == tCIDBuild::EActions::MakeDeps)
        ||  (m_eAction == tCIDBuild::EActions::MakeRes))
        {
            MakeTargetDirs();
        }

        //
        //  Build up a list of targets. Usually it's just one. But for the special
        //  case of a group target in no-recurse mode, we want to invoke each of
        //  the group's dependents.
        //
        TList<TBldStr> llstTargets;
        if (m_bNoRecurse
        &&  (m_eAction == tCIDBuild::EActions::Build)
        &&  !m_strTarget.bIEquals(kCIDBuild::pszAllProjects))
        {
            const TProjectInfo& projiTop = m_pplistProjs->projiByName(m_strTarget);
            if (projiTop.eType() == tCIDBuild::EProjTypes::Group)
            {
                TList<TBldStr>::TCursor cursDeps(&projiTop.listDeps());
                if (cursDeps.bResetIter())
                {
                    do
                    {
                        llstTargets.Add(new TBldStr(cursDeps.tCurElement()));
                    }   while (cursDeps.bNext());
                }
            }
             else
            {
                llstTargets.Add(new TBldStr(m_strTarget));
            }
        }
         else
        {
            llstTargets.Add(new TBldStr(m_strTarget));
        }

        //
        //  And now we can kick off the process. According to what action
        //  we are doing, we do one thing or another.
        //
        TList<TBldStr>::TCursor cursTars(&llstTargets);
        if (cursTars.bResetIter())
        {
            do
            {
                //
                //  If the action indicates that we need to build resources files
                //  first, then lets go ahead and do that.
                //
                if ((m_eAction == tCIDBuild::EActions::Build)
                ||  (m_eAction == tCIDBuild::EActions::MakeDeps))
                {
                    MakeResources(cursTars.tCurElement());
                }

                //
                //  If the action indicates that we need to do IDL generation, then
                //  lets do that next.
                //
                if ((m_eAction == tCIDBuild::EActions::Build)
                ||  (m_eAction == tCIDBuild::EActions::MakeDeps)
                ||  (m_eAction == tCIDBuild::EActions::IDLGen))
                {
                    GenIDLFiles(cursTars.tCurElement());
                }

                //
                //  Now ask the project list to load up its file lists. This will
                //  make each project get directory info on its Cpp/Hpp files.
                //
                m_pplistProjs->LoadFileLists();

                //
                //  If the action indicates that we need to update headers first,
                //  then lets do that.
                //
                if ((m_eAction == tCIDBuild::EActions::Build)
                ||  (m_eAction == tCIDBuild::EActions::MakeDeps))
                {
                    CopyHeaders(cursTars.tCurElement());
                }

                // Do any file copies indicated in this project
                DoFileCopies(cursTars.tCurElement());

                switch(m_eAction)
                {
                    case tCIDBuild::EActions::Build :
                    {
                        Build(cursTars.tCurElement());

                        // And do any trailing custom commands
                        RunCustomCmds(cursTars.tCurElement());
                        break;
                    }

                    case tCIDBuild::EActions::CopyHeaders :
                        //
                        //  Do any message file compilation required, then update
                        //  any headers that need to be.
                        //
                        CopyHeaders(cursTars.tCurElement());
                        break;

                    case tCIDBuild::EActions::MakeDeps :
                        //
                        //  Do any message file compilation required, update any
                        //  headers that need to be copied to the output directory,
                        //  then update the dependencies.
                        //
                        MakeDepends(cursTars.tCurElement());
                        break;

                    case tCIDBuild::EActions::MakeRes :
                        //
                        //  Just call our private resource building method. It will
                        //  invoke the operation as required.
                        //
                        MakeResources(cursTars.tCurElement());
                        break;

                    case tCIDBuild::EActions::ShowProjDeps :
                        // Just call our private method to dump the dependencies.
                        DumpDepends(cursTars.tCurElement());
                        break;

                    case tCIDBuild::EActions::ShowProjSettings :
                        //
                        //  Make sure that the target is a real target, and not
                        //  the all projects pseudo target.
                        //
                        if (cursTars.tCurElement() == kCIDBuild::pszAllProjects)
                        {
                            stdOut  << L"Showing project settings requires a target"
                                    << kCIDBuild::EndLn;
                            break;
                        }
                        m_pplistProjs->DumpProjectSettings(cursTars.tCurElement());
                        break;

                    case tCIDBuild::EActions::MakeBinRelease :
                    case tCIDBuild::EActions::MakeDevRelease :
                        //
                        //  This isn't implemented yet
                        //
                        // MakeRelease(m_eAction, cursTars.tCurElement());
                        //
                        break;

                    case tCIDBuild::EActions::IDLGen :
                        // This was already handled before we got here
                        break;

                    default :
                        stdOut << L"Unknown EActions value" << kCIDBuild::EndLn;
                        throw tCIDBuild::EErrors::Internal;
                }
            }   while (cursTars.bNext());
        }
    }

    catch(tCIDBuild::EErrors eErr)
    {
        if (eErr == tCIDBuild::EErrors::BadParams)
            ShowUsage();

        if (m_bVerbose)
        {
            stdOut  << L"An error occured during processing.\n   Error Type: "
                    << eErr << kCIDBuild::EndLn;
        }
        return 1;
    }

    catch(...)
    {
        stdOut << L"A system exception occured during processing" << kCIDBuild::EndLn;
        return 1;
    }

    stdOut << L"\nAction completed successfully" << kCIDBuild::EndLn;
    return 0;
}


//
//  This method is called back on from the project list object. The project
//  list object starts parsing the project file. But, if it sees a ALLPROJECTS
//  block, it calls us back so that we can parse that block and store the
//  settings here for everyone to see.
//
tCIDLib::TVoid TFacCIDBuild::ParseAllProjects(TLineSpooler& lsplSource)
{
    TBldStr strKey;
    TBldStr strValue;
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected EXTLIBS=, EXTINCLUDES=, MACROS=, IDLMAPPINGS, or END ALLPROJECTS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END ALLPROJECTS")
            break;

        // Look for the blocks or settings that we support
        if (strReadBuf == L"EXTLIBS=")
        {
            //
            //  Loop until we hit the end of the block, storing away each
            //  new library name.
            //
            while (true)
            {
                if (!lsplSource.bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Expected external lib path, or END EXTLIBS"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == L"END EXTLIBS")
                    break;

                m_listExtLibs.Add(new TBldStr(strReadBuf));
            }
        }
         else if (strReadBuf == L"EXTLIBPATHS=")
        {
            //
            //  Loop until we hit the end of the block, storing away each
            //  new lib path.
            //
            while (true)
            {
                if (!lsplSource.bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Expected external lib path, or END EXTLIBPATHS"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == L"END EXTLIBPATHS")
                    break;

                m_listExtLibPaths.Add(new TBldStr(strReadBuf));
            }
        }
         else if (strReadBuf == L"EXTINCLUDEPATHS=")
        {
            //
            //  Loop until we hit the end of the block, storing away each
            //  new include path.
            //
            while (true)
            {
                if (!lsplSource.bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Expected external include path, or END EXTINCLUDES"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == L"END EXTINCLUDEPATHS")
                    break;

                m_listExtIncludePaths.Add(new TBldStr(strReadBuf));
            }
        }
         else if (strReadBuf == L"MACROS=")
        {
            //
            //  Loop until we hit the end of the block, storing away each
            //  new macro.
            //
            while (true)
            {
                if (!lsplSource.bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Expected key=value, or END MACROS"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == L"END MACROS")
                    break;

                // Have to assume its a define so pull it out
                if (!TUtils::bFindNVParts(strReadBuf, strKey, strValue))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Badly formed MACRO statement" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }

                m_listMacros.Add(new TKeyValuePair(strKey, strValue));
            }
        }
         else if (strReadBuf == L"IDLMAPPINGS=")
        {
            //
            //  Loop until we hit the end of the block, storing away each
            //  new macro.
            //
            while (true)
            {
                if (!lsplSource.bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Expected key=value, or END IDLMAPPINGS"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == L"END IDLMAPPINGS")
                    break;

                //
                //  Break the entry into its too parts. We don't actually
                //  store it as separate strings, but we need to check that
                //  its basically correctly formed.
                //
                if (!TUtils::bFindNVParts(strReadBuf, strKey, strValue))
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Badly formed mapping entry" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }

                // Make sure that neither is empty
                if (strKey.bEmpty() || strValue.bEmpty())
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Badly formed mapping entry" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }

                //
                //  Lets add a mapping item, adding the /Mapping= prefix that
                //  is needed when its spit out later.
                //
                strValue = L"/Mapping=";
                strValue.Append(strReadBuf);
                m_listIDLMappings.Add(new TBldStr(strValue));
            }
        }
         else
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected EXTLIBS=, EXTINCLUDES=, IDLMAPPINGS=, or END ALLPROJECTS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}



// ---------------------------------------------------------------------------
//  TFacCIDBuild: Private, non-virtual methods
//
//  NOTE: Some of these are implemented in CIDBuild_FacilityActions.Cpp!
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDBuild::MakeTargetDirs()
{
    //
    //  Create the main output directory if required. This one is a bit of a special
    //  case to get us started. We have no 'make path' that can do a whole path from
    //  the root, and this may have more than one trailing component that doesn't
    //  exist. But, we know it is under our source dir, so we can make that the
    //  parent, and use the relative path maker we do have. It also has a trailing
    //  slash so get rid of that.
    //
    TBldStr strTmpOutDir(m_strOutDir);
    strTmpOutDir.DeleteLast();
    if (!TUtils::bExists(strTmpOutDir))
    {
        if (!strTmpOutDir.bStartsWith(m_strRootDir))
        {
            stdOut  << L"Output directory is not beneath root directory: "
                    << m_strOutDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }

        // Cut out the root path part, leaving the trailing bits
        strTmpOutDir.Cut(m_strRootDir.c4Length());
        if (!TUtils::bMakePath(m_strRootDir, strTmpOutDir))
        {
            stdOut << L"Could not create output directory: "
                   << strTmpOutDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    //
    //  Create the pubilc include directory and the platform specific
    //  directory under it.
    //
    if (!TUtils::bExists(m_strIncludeDir))
    {
        if (!TUtils::bMakeDir(m_strIncludeDir))
        {
            stdOut  << L"Could not create output directory: "
                    << m_strIncludeDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    if (!TUtils::bExists(m_strPPIncludeDir))
    {
        if (!TUtils::bMakeDir(m_strPPIncludeDir))
        {
            stdOut  << L"Could not create output directory: "
                    << m_strPPIncludeDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    //
    //  Create the private include directory and the platform specific
    //  directory under it.
    //
    if (!TUtils::bExists(m_strPrivIncludeDir))
    {
        if (!TUtils::bMakeDir(m_strPrivIncludeDir))
        {
            stdOut  << L"Could not create output directory: "
                    << m_strPrivIncludeDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    if (!TUtils::bExists(m_strPPPrivIncludeDir))
    {
        if (!TUtils::bMakeDir(m_strPPPrivIncludeDir))
        {
            stdOut  << L"Could not create output directory: "
                    << m_strPPPrivIncludeDir << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::CreateError;
        }
    }

    // And insure that the output directories of the projects are created
    TListCursor<TProjectInfo> cursLists(&m_pplistProjs->listProjects());
    if (cursLists.bResetIter())
    {
        do
        {
            const TProjectInfo& projiCur = cursLists.tCurElement();
            if (!projiCur.bMakeOutDir())
            {
                stdOut  << L"Could not create output directory: "
                        << projiCur.strOutDir() << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }   while (cursLists.bNext());
    }
}


tCIDLib::TVoid TFacCIDBuild::CheckEnv()
{
    //
    //  Run through the environment and look for the following environment
    //  variables (and what they set):
    //
    //  CID_BUILDMODE       m_eBldMode
    //  CID_RESDIR          m_strOutDir
    //  CID_SRCTREE         m_strRootDir
    //  CID_VERSION         m_strVersionSuffix, m_c4MajVer, m_c4MinVer, m_c4Revision
    //
    //  We build up m_strFUllPlatform fro the base and version strings.
    //
    TBldStr strTmp;
    if (TUtils::bGetEnvVar(L"CID_BUILDMODE", strTmp))
    {
        if (strTmp.bIEquals(L"Dev"))
        {
            m_eBldMode = tCIDBuild::EBldModes::Develop;
        }
         else if (strTmp.bIEquals(L"Prod"))
        {
            m_eBldMode = tCIDBuild::EBldModes::Production;
        }
         else
        {
            stdOut  << L"/Mode= must be set to Dev or Prod"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BadParams;
        }
    }

    if (TUtils::bGetEnvVar(L"CID_VERSION", strTmp))
        ProcessVerParm(strTmp.pszBuffer());

    //
    //  CID_SRCTREE is the tree being built. CIDLIB_SRCDIR is the CIDLib
    //  tree itself. They are only the same when CIDLib itself is being
    //  built.
    //
    if (TUtils::bGetEnvVar(L"CID_SRCTREE", m_strRootDir))
    {
        // Make sure it ends with a slash
        if (m_strRootDir.chLast() != L'\\')
            m_strRootDir.Append(L"\\");
    }

    if (TUtils::bGetEnvVar(L"CIDLIB_SRCDIR", m_strCIDLibSrcDir))
    {
        // Make sure it ends with a slash
        if (m_strCIDLibSrcDir.chLast() != L'\\')
            m_strCIDLibSrcDir.Append(L"\\");
    }

    if (TUtils::bGetEnvVar(L"CID_RESDIR", m_strOutDir))
    {
        // Make sure it ends with a slash
        if (m_strOutDir.chLast() != L'\\')
            m_strOutDir.Append(L"\\");
    }

    // Create full string versions of the xxx_Platform.hpp file values
    m_strBasePlatform = kCIDBuild::pszBasePlatform;
    m_strFullPlatform = kCIDBuild::pszFullPlatform;
}


tCIDLib::TVoid
TFacCIDBuild::ParseParms(   const   tCIDLib::TCard4        c4Args
                            , const tCIDLib::TCh* const*  apszArgs)
{
    //
    //  Loop through the parameters and check for each one we understand.
    //  We have to have some but others are optional.
    //
    tCIDLib::TCard4 c4Ind;
    for (c4Ind = 1; c4Ind < c4Args; c4Ind++)
    {
        // We found one, so process it
        if (apszArgs[c4Ind][0] == L'/')
        {
            // Get a pointer past the slash
            const tCIDLib::TCh* const pszCurParm = &apszArgs[c4Ind][1];

            if (!TRawStr::c4StrLen(pszCurParm))
            {
                stdOut  << L"Invalid option, / must be followed by option info"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::BadParams;
            }

            if (!TRawStr::iCompIStr(pszCurParm, L"Force"))
            {
                m_bForce = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"HdrDump:Std"))
            {
                m_eHdrDumpMode = tCIDBuild::EHdrDmpModes::Std;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"HdrDump:Full"))
            {
                m_eHdrDumpMode = tCIDBuild::EHdrDmpModes::Full;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"LowPrio"))
            {
                m_bLowPrio = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"NoLogo"))
            {
                m_bSupressLogo = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"NoRecurse")
                  ||  !TRawStr::iCompIStr(pszCurParm, L"NR"))
            {
                m_bNoRecurse = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"NonPermissive"))
            {
                m_bNonPermissive = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"Verbose"))
            {
                m_bVerbose = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"MaxWarn"))
            {
                m_bMaxWarn = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"RootDir=", 8))
            {
                //
                //  Get a copy of the the provided root dir. Add some extra
                //  space for a trailing slash in case it does not have one.
                //
                m_strRootDir = &pszCurParm[8];
                if (m_strRootDir.chLast() != L'\\')
                    m_strRootDir.Append(L"\\");
            }
             else if (!TRawStr::iCompIStr(pszCurParm, L"Single"))
            {
                m_bSingle = kCIDLib::True;
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"Target=", 7))
            {
                // Save the part after the = as the targe tproject
                m_strTarget = &pszCurParm[7];
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"Version=", 8))
            {
                ProcessVerParm(&pszCurParm[8]);
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"Mode=", 5))
            {
                if (!TRawStr::iCompIStr(&pszCurParm[5], L"Dev"))
                {
                    m_eBldMode = tCIDBuild::EBldModes::Develop;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[5], L"Prod"))
                {
                    m_eBldMode = tCIDBuild::EBldModes::Production;
                }
                 else
                {
                    stdOut << L"/Mode= must be set to Develop or Production"
                         << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::BadParams;
                }
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"Action=", 7))
            {
                if (!TRawStr::iCompIStr(&pszCurParm[7], L"Build"))
                {
                    m_eAction = tCIDBuild::EActions::Build;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"MakeDeps"))
                {
                    m_eAction = tCIDBuild::EActions::MakeDeps;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"ShowProjDeps"))
                {
                    m_eAction = tCIDBuild::EActions::ShowProjDeps;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"ShowProjSettings"))
                {
                    m_eAction = tCIDBuild::EActions::ShowProjSettings;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"CopyHeaders"))
                {
                    m_eAction = tCIDBuild::EActions::CopyHeaders;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"MakeRes"))
                {
                    m_eAction = tCIDBuild::EActions::MakeRes;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"MakeBinRel"))
                {
                    m_eAction = tCIDBuild::EActions::MakeBinRelease;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"MakeDevRel"))
                {
                    m_eAction = tCIDBuild::EActions::MakeDevRelease;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"IDLGen"))
                {
                    m_eAction = tCIDBuild::EActions::IDLGen;
                }
                 else if (!TRawStr::iCompIStr(&pszCurParm[7], L"Bootstrap"))
                {
                    m_eAction = tCIDBuild::EActions::Bootstrap;
                }
                 else
                {
                    stdOut << L"/Action= Unknown action value" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::BadParams;
                }
            }
             else if (!TRawStr::iCompIStrN(pszCurParm, L"Lang=", 5))
            {
                //
                //  Make sure that the rest of the string is two characters
                //  since all suffixes are two.
                //
                if (TRawStr::c4StrLen(&pszCurParm[5]) != 2)
                {
                    stdOut << L"/Lang= Suffix must be 2 characters" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::BadParams;
                }

                //
                //  Look it up in the list of suffixes to make sure that
                //  its legal.
                //
                tCIDLib::TCard4 c4Tmp = 0;
                for (; c4Tmp < tCIDLib::TCard4(tCIDLib::ELanguages::Count); c4Tmp++)
                {
                    if (!TRawStr::iCompStr( kCIDLib::apszLangSuffixes[c4Tmp]
                                            , &pszCurParm[5]))
                    {
                        break;
                    }
                }

                if (c4Tmp == tCIDLib::TCard4(tCIDLib::ELanguages::Count))
                {
                    stdOut << L"/Lang= Unknown language suffix" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::BadParams;
                }
            }
             else
            {
                stdOut  << L"Option " << pszCurParm
                        << L" is not a known option"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::BadParams;
            }
        }
         else
        {
            stdOut  << L"Option " << apszArgs[c4Ind]
                    << L" is not a known option"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BadParams;
        }
    }

    //
    //  Check that we got any options that are required. If not, then
    //  display an error and throw an error.
    //
    if (m_strRootDir.bEmpty())
    {
        stdOut << L"The /RootDir= parameter must be provided" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BadParams;
    }

    if (!m_c4MajVer && !m_c4MinVer && !m_c4Revision)
    {
        stdOut << L"The /Version= parameter must be provided" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BadParams;
    }

    //
    //  Build up the version string postfix that is appended to the names
    //  of 'versioned' files. This is done by the platform since it might
    //  require different forms on each one.
    //
    BuildVersionString(m_strVersionSuffix, m_c4MajVer, m_c4MinVer);

    // Build up the standard include path
    m_strIncludeDir = m_strOutDir;
    m_strIncludeDir.Append(L"Include", L"\\");

    m_strPPIncludeDir = m_strIncludeDir;
    m_strPPIncludeDir.Append(kCIDBuild::pszPlatformDir);

    m_strPrivIncludeDir = m_strOutDir;
    m_strPrivIncludeDir.Append(L"PrivInclude", L"\\");

    m_strPPPrivIncludeDir = m_strPrivIncludeDir;
    m_strPPPrivIncludeDir.Append(kCIDBuild::pszPlatformDir);

    // And do a couple more checks now that we have the target
    if (m_eAction == tCIDBuild::EActions::ShowProjSettings)
    {
        if (m_strTarget.bEmpty())
        {
            stdOut << L"ShowProjSettings requires a target project" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BadParams;
        }
    }

    //
    //  If its one of the release building actions, then make sure that we
    //  have what we need for those. Otherwise, its some build related action
    //  so handle that stuff.
    //
    if ((m_eAction == tCIDBuild::EActions::MakeBinRelease)
    ||  (m_eAction == tCIDBuild::EActions::MakeDevRelease))
    {
        // Make sure we have a target
        if (m_strTarget.bEmpty())
        {
            stdOut << L"This action requires a target parameter" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BadParams;
        }

        // Make sure it ends in a path separator
        if (m_strTarget.chLast() != L'\\')
            m_strTarget.Append(L"\\");

        if (m_eBldMode == tCIDBuild::EBldModes::Develop)
            stdOut << L"WARNING: This action should use the Production build" << kCIDBuild::EndLn;

        if (!m_strLangSuffix.bEmpty())
        {
            stdOut << L"WARNING: Language suffix ignored for this action" << kCIDBuild::EndLn;
            m_strTarget.Clear();
        }
    }
     else
    {
        //
        //  If there was no language id given, then default it to "en" for
        //  the English version.
        //
        if (m_strLangSuffix.bEmpty())
            m_strLangSuffix = L"en";
    }
}


tCIDLib::TVoid TFacCIDBuild::ProcessVerParm(const tCIDLib::TCh* const pszVer)
{
    // Make a temp copy that we can tokenize
    tCIDLib::TCh* pszTmp = TRawStr::pszDupString(pszVer);
    TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);

    //
    //  We need to parse out a major and minor version out of this guy. It
    //  should be in the form M.m (where M is maj and m is minor versions.)
    //
    const tCIDLib::TCh* pszMaj = TRawStr::pszStrTok(pszTmp, L".");
    const tCIDLib::TCh* pszMin = TRawStr::pszStrTok(0, L".");
    const tCIDLib::TCh* pszRev = TRawStr::pszStrTok(0, L".");
    if (!pszMaj || !pszMin || !pszRev)
    {
        stdOut  << L"/Version= value must be in the form 'M.m.r'"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BadParams;
    }

    // Make sure that they both convert
    tCIDLib::TBoolean bMaj = TRawStr::bXlatCard4(pszMaj, m_c4MajVer);
    tCIDLib::TBoolean bMin = TRawStr::bXlatCard4(pszMin, m_c4MinVer);
    tCIDLib::TBoolean bRev = TRawStr::bXlatCard4(pszRev, m_c4Revision);
    if (!bMaj || !bMin || !bRev)
    {
        stdOut << L"/Version values are not legal numbers" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BadParams;
    }
}


tCIDLib::TVoid TFacCIDBuild::ShowLogo()
{
    stdOut  << L"CIDBuild.Exe, CIDLib Portable Build Utility\n"
            << L"  Copyright (c) 1998..2018, Charmed Quark Systems\n"
            << L"  Compiled On: " << __DATE__ << "\n" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::ShowParms()
{
    if ((m_eAction == tCIDBuild::EActions::MakeBinRelease)
    ||  (m_eAction == tCIDBuild::EActions::MakeDevRelease))
    {
        stdOut  << L"     Action: " << m_eAction << L"\n"
                << L"   Root Dir: " << m_strRootDir << L"\n"
                << L" Target Dir: " << m_strTarget << L"\n"
                << L"    Verbose: " << (m_bVerbose ? L"Yes" : L"No") << L"\n"
                << L"    Version: " << m_strVersionSuffix
                << kCIDBuild::EndLn;
    }
     else
    {
        stdOut  << L"   Root Dir: " << m_strRootDir << L"\n"
                << L"    Out Dir: " << m_strOutDir << L"\n"
                << L"    Version: " << m_c4MajVer
                                         << L"." << m_c4MinVer
                                         << L"." << m_c4Revision << L"\n"
                << L"    Verbose: " << (m_bVerbose ? L"Yes" : L"No") << L"\n"
                << L"   Low Prio: " << (m_bLowPrio ? L"Yes" : L"No") << L"\n"
                << L"   Max Warn: " << (m_bMaxWarn ? L"Yes" : L"No") << L"\n"
                << L"    No Logo: " << (m_bSupressLogo ? L"Yes" : L"No") << L"\n";

        stdOut << L"     Target: ";
        if (!m_strTarget.bEmpty())
            stdOut << m_strTarget << L"\n";
        else
            stdOut << L"ALL\n";

        stdOut  << L"     Action: " << m_eAction << L"\n"
                << L"       Mode: " << m_eBldMode << L"\n"
                << L"   Platform: " << kCIDBuild::pszFullPlatform << L"\n"
                << L" LangSuffix: " << m_strLangSuffix << L"\n"
                << L"    Version: " << m_strVersionSuffix
                << kCIDBuild::EndLn;
    }
}


tCIDLib::TVoid TFacCIDBuild::ShowUsage()
{
    stdOut  << L"\nUsage:\n"
            << L"    CIDBuild [options]\n"
            << L"      Options:\n"
            << L"        /NoLogo        - Supress logo display\n"
            << L"        /Verbose       - Display more info\n"
            << L"        /Action=XX     - Set action to perform (Build)\n"
            << L"        /Mode=XX       - Set build mode to Dev or Prod (Dev)\n"
            << L"        /Version=M.m.r - Set the release version *\n"
            << L"        /RootDir=XX    - Set root directory of build tree *\n"
            << L"        /Lang=xx       - Set language suffix (en)\n"
            << L"        /Target=xx     - Set the target project or directory (all)\n"
            << L"        /LowPrio       - Invoke compiler with lower priority\n"
            << L"        /Single        - Invoke compiler in non-parallel mode\n"
            << L"        /NonPermissive - Invoke compiler in strict C++ mode\n"
            << L"\n"
            << L"    Target:\n"
            << L"        Project to operate on. If not provided, means build\n"
            << L"        all facilities. All underlying dependent facilities\n"
            << L"        are operated on as well\n"
            << L"        For MakeDevRel, MakeBinRel, and MakeSrcRel, this is the\n"
            << L"        target directory to build to. Required in this case\n"
            << L"\n"
            << L"    Actions:\n"
            << L"        Build, MakeDeps, ShowProjDeps, CopyHeaders, MakeRes\n"
            << L"        ShowProjSettings, MakeBinRel, MakeDevRel, IDLGen\n"
            << L"\n"
            << L"      *  Means a required option\n"
            << L"      () Means the default if not provided\n\n"
            << kCIDBuild::EndLn;
}

