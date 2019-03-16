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
//                                               43
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  TFacCIDBuild: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFacCIDBuild::bBuildCallBack(const TBldStr& strTarget, const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // If not a code type project, nothing to do here
    if (projiCur.eType() > tCIDBuild::EProjTypes::MaxCodeType)
        return kCIDLib::True;

    //
    //  If this guy has any build level dependencies, i.e. not stuff that it
    //  links to, but stuff it needs to insure is compiled, then check those
    //  first and make sure they are built.
    //


    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Building project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    //
    //  Query the build tools driver object, which we get from the TUtils
    //  guy, because he contains all of the conditional code stuff. This
    //  is not optimal but its really important not to spread around
    //  conditional code all over the place.
    //
    //  He will lazily allocate the object upon the first call so as to
    //  not do bother if the build operation never gets invoked.
    //
    return TToolsDriver::tdrvBuilder().bBuildProject(projiCur);
}


tCIDLib::TBoolean
TFacCIDBuild::bCopyHeadersCallBack(const TBldStr& strTarget, const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // Nothing to do here for non-code type projects
    if (projiCur.eType() > tCIDBuild::EProjTypes::MaxCodeType)
        return kCIDLib::True;

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Updating Headers for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    //
    //  Lets loop through the list of headers for the target project
    //  and update each one that is out of date.
    //
    TList<TFindInfo>::TCursor cursHpps(&projiCur.listHpps());

    // Return if there aren't any Hpps for this project
    if (!cursHpps.bResetIter())
        return kCIDLib::True;

    // Change to this project's directory
    if (!TUtils::bChangeDir(projiCur.strProjectDir()))
    {
        stdOut  << L"Could not change to project directory: "
                << projiCur.strProjectDir() << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    do
    {
        // Get a short cut to the file info for this header
        const TFindInfo& fndiCur = cursHpps.tCurElement();

        //
        //  See if this is an internal header or a public header, and set
        //  up the correct target directory for that kind of header.
        //
        const TBldStr* pstrTargetDir;
        if (TUtils::bIsPrivateHeader(fndiCur.strFileName()))
            pstrTargetDir = &m_strPrivIncludeDir;
        else
            pstrTargetDir = &m_strIncludeDir;

        //
        //  Query the find info for the target file. If its not found or
        //  its earlier than our source file, we have to update. If the
        //  Force flag is on, then assume we have to update.
        //
        tCIDLib::TBoolean bUpdate = facCIDBuild.bForce();

        if (!bUpdate)
        {
            TFindInfo fndiTarget;
            if (!TFindInfo::bFindAFile
            (
                *pstrTargetDir, fndiCur.strFileName(), fndiTarget))
            {
                bUpdate = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Target not present: "
                            << fndiCur.strFileName() << kCIDBuild::EndLn;
                }
            }
             else
            {
                bUpdate = fndiCur > fndiTarget;

                if (bUpdate && facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Source is newer: "
                            << fndiCur.strFileName() << kCIDBuild::EndLn;
                }
            }
        }

        if (bUpdate)
        {
            //
            //  We have to copy the file to the target. The return only
            //  indicates a failure when the 'fail if exists' flag is on
            //  and the target already exists.
            //
            TUtils::bCopyFile
            (
                fndiCur.strFileName()
                , projiCur.strProjectDir()
                , fndiCur.strFileName()
                , *pstrTargetDir
            );
        }
    }   while (cursHpps.bNext());

    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDBuild::bCustomCmdsCallBack(  const   TBldStr&        strTarget
                                    , const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Doing Custom Commands for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    // Lets loop through the list of custom commands for this project
    TList<TBldStr>::TCursor cursCmds(&projiCur.listCustomCmds());

    // Return if there aren't any for this project
    if (!cursCmds.bResetIter())
        return kCIDLib::True;

    do
    {
        // Get the current command
        TBldStr& strCur = cursCmds.tCurElement();

        // Change to this project's directory
        if (!TUtils::bChangeDir(projiCur.strProjectDir()))
        {
            stdOut  << L"Could not change to project directory: "
                    << projiCur.strProjectDir() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        // Invoke this string as a command line
        tCIDLib::TCard4 c4Res;
        if (!TUtils::bRunCmdLine(strCur, c4Res, facCIDBuild.bLowPrio()))
        {
            stdOut  << L"Could not run custom command: "
                    << strCur << kCIDBuild::EndLn;
            return kCIDLib::False;
        }

    }   while (cursCmds.bNext());

    return kCIDLib::True;
}



tCIDLib::TBoolean
TFacCIDBuild::bFileCopiesCallBack(  const   TBldStr&        strTarget
                                    , const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Doing File Copies for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    // Lets loop through the list of file copy blocks for this project
    TList<TProjFileCopy>::TCursor cursCopies(&projiCur.listFileCopies());

    // Return if there aren't any file copies for this project
    if (!cursCopies.bResetIter())
        return kCIDLib::True;

    do
    {
        // Get the current file copy block
        TProjFileCopy& pfcCur = cursCopies.tCurElement();

        // Change to this project's directory
        if (!TUtils::bChangeDir(projiCur.strProjectDir()))
        {
            stdOut  << L"Could not change to project directory: "
                    << projiCur.strProjectDir() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        // Make the output directory if needed
        const tCIDLib::TBoolean bMadeDir = TUtils::bMakePath
        (
            facCIDBuild.strOutDir(), pfcCur.strOutPath()
        );

        // Get a cursor over the source file list
        TList<TBldStr>::TCursor cursSrcFiles(&pfcCur.listSrcFiles());
        if (!cursSrcFiles.bResetIter())
            continue;

        tCIDLib::TCard4 c4NameOfs;
        tCIDLib::TCard4 c4ExtOfs;
        TFindInfo       fndiSrc;
        TFindInfo       fndiTarget;
        TBldStr         strTargetFile;
        do
        {
            // Get a short cut to file name
            const TBldStr& strCur = cursSrcFiles.tCurElement();

            //
            //  If we are being forced, or we had to create the output dir above,
            //  just turn on the update flag right away since we know we have
            //  to copy.
            //
            tCIDLib::TBoolean bUpdate = facCIDBuild.bForce() | bMadeDir;

            // Make sure the source exists
            if (!TFindInfo::bFindAFile(strCur, fndiSrc, tCIDBuild::EPathModes::Full))
            {
                // This is bad, so give an error and return failure
                stdOut  << L"File '" << strCur << "' was not found" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::NotFound;
            }

            // Build up the full path to the target
            TUtils::FindPathParts(fndiSrc.strFileName(), c4NameOfs, c4ExtOfs);
            if (!c4NameOfs)
            {
                // This is bad, so give an error and return failure
                stdOut  << L"File '" << strCur
                        << "' was not a valid file name" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::BadParams;
            }
            strTargetFile = facCIDBuild.strOutDir();
            strTargetFile.Append(pfcCur.strOutPath());
            if (strTargetFile.chLast() != L'\\')
                strTargetFile.Append(L"\\");
            strTargetFile.AppendAt(fndiSrc.strFileName(), c4NameOfs);

            //
            //  If not already decided above, see if we need to copy. If the
            //  target doesn't exist, then we obviously do. Else if its out of
            //  date we do.
            //
            if (!bUpdate)
            {
                if (!TFindInfo::bFindAFile(strTargetFile, fndiTarget))
                {
                    if (facCIDBuild.bVerbose())
                    {
                        stdOut  << L"    Target not present: " << strTargetFile
                                << kCIDBuild::EndLn;
                    }
                    bUpdate = kCIDLib::True;
                }
                 else
                {
                    if (fndiSrc.tmLastWrite() > fndiTarget.tmLastWrite())
                    {
                        bUpdate = kCIDLib::True;

                        if (facCIDBuild.bVerbose())
                            stdOut  << L"    Source is newer: " << strCur << kCIDBuild::EndLn;
                    }
                }
            }

            // If we need to update, then lets try to copy
            if (bUpdate)
            {
                if (!TUtils::bCopyFile(fndiSrc.strFileName(), strTargetFile))
                {
                    stdOut  << L"Failed to copy file: " << fndiTarget.strFileName() << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }
            }
        }   while (cursSrcFiles.bNext());
    }   while (cursCopies.bNext());

    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDBuild::bDumpDependsCallBack( const   TBldStr&        strTarget
                                    , const tCIDLib::TCard4 c4Level)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // If this is a file copy project, then nothing to do here
    if (projiCur.eType() == tCIDBuild::EProjTypes::FileCopy)
        return kCIDLib::True;

    // Create the spacer for this level
    const tCIDLib::TCard4 c4Spaces = c4Level * 3;
    tCIDLib::TCh szSpacer[257];
    if (c4Spaces > 256)
    {
        stdOut  << L"Internal Error. Dependency level too deep to dump out"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    for (tCIDLib::TCard4 ulIndex = 0; ulIndex < c4Spaces; ulIndex++)
        szSpacer[ulIndex] = L' ';
    szSpacer[c4Spaces] = 0;

    // Separate the top level dumps
    if (c4Level == 1)
        stdOut << L"\n";

    //
    //  Output the name of the current element, indented to the indicated
    //  level.
    //
    stdOut << szSpacer << strTarget << kCIDBuild::EndLn;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDBuild::bIDLGenCallBack(  const   TBldStr&        strTarget
                                , const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // Nothing to do for non-code projects
    if (projiCur.eType() > tCIDBuild::EProjTypes::MaxCodeType)
        return kCIDLib::True;

    // If this project does not have any IDL files, there is nothing to do
    if (!projiCur.bHasIDLFiles())
        return kCIDLib::True;

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"IDL Generation for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    //
    //  Loop through the list of IDLInfo objects in this project. For each
    //  one we'll generate the indicated output files.
    //
    TList<TIDLInfo>::TCursor cursIDLs(&projiCur.listIDLFiles());
    if (cursIDLs.bResetIter())
    {
        do
        {
            // Tell this one to generate its output if required
            cursIDLs.tCurElement().GenerateCode(projiCur);
        }   while (cursIDLs.bNext());
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDBuild::bMakeDependsCallBack( const   TBldStr&        strTarget
                                    , const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // If this is a file copy or group project, then nothing to do here
    if ((projiCur.eType() == tCIDBuild::EProjTypes::FileCopy)
    ||  (projiCur.eType() == tCIDBuild::EProjTypes::Group))
    {
        return kCIDLib::True;
    }

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Updating Dependencies for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    //
    //  Create a dependency analyzer for the target project and tell it
    //  to make the dependency file.
    //
    TFileDepAnalyzer fdaTarget(projiCur);
    fdaTarget.MakeDeps();
    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDBuild::bMakeResCallBack(const TBldStr& strTarget, const tCIDLib::TCard4)
{
    // Get a quick ref to the project info for the target
    const TProjectInfo& projiCur = m_pplistProjs->projiByName(strTarget);

    // Nothing to do for a non-code type project
    if (projiCur.eType() > tCIDBuild::EProjTypes::MaxCodeType)
        return kCIDLib::True;

    //
    //  If this project does not have a resource or message text file, then
    //  there is nothing to do.
    //
    if (!projiCur.bHasResFile() && !projiCur.bHasMsgFile())
        return kCIDLib::True;

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Making resources for Project: " << strTarget << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    TResCompiler rcompCur(projiCur, facCIDBuild.bVerbose(), facCIDBuild.bForce());
    return rcompCur.bProcess();
}


tCIDLib::TVoid TFacCIDBuild::Build(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bBuildCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private project build method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterBld(this, &TFacCIDBuild::bBuildCallBack);
    if (!m_pplistProjs->bIterate(piterBld, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Build operation failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::CopyHeaders(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bCopyHeadersCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private header update method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterHdrs(this, &TFacCIDBuild::bCopyHeadersCallBack);
    if (!m_pplistProjs->bIterate(piterHdrs, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Project dependency iteration failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::DoFileCopies(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bFileCopiesCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private file copies method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterHdrs(this, &TFacCIDBuild::bFileCopiesCallBack);
    if (!m_pplistProjs->bIterate(piterHdrs, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Project dependency iteration failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::DumpDepends(const TBldStr& strTarget)
{
    stdOut  << L"\n----------------------------------------------\n"
            << L"Dependents for Project: " << strTarget << L"\n"
            << L"----------------------------------------------"
            << kCIDBuild::EndLn;

    //
    //  Create an iterator and have it call our private dep dump method
    //  for each dependent of the target. This one does a top down iteration
    //  and does not do minimal iteration.
    //
    TMethProjIterator<TFacCIDBuild> piterDump(this, &TFacCIDBuild::bDumpDependsCallBack);
    if (!m_pplistProjs->bIterate(piterDump, strTarget, tCIDBuild::EIterModes::FromTopMax))
        stdOut << L"Project dependency dump failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::GenIDLFiles(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bIDLGenCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private dependency method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterDep(this, &TFacCIDBuild::bIDLGenCallBack);
    if (!m_pplistProjs->bIterate(piterDep, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Project dependency iteration failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::MakeDepends(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bMakeDependsCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private dependency method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterDep(this, &TFacCIDBuild::bMakeDependsCallBack);
    if (!m_pplistProjs->bIterate(piterDep, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Project dependency iteration failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::MakeResources(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bMakeResCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private resource build method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterMsgs(this, &TFacCIDBuild::bMakeResCallBack);
    if (!m_pplistProjs->bIterate(piterMsgs, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Resource compile dependency iteration failed" << kCIDBuild::EndLn;
}


tCIDLib::TVoid TFacCIDBuild::RunCustomCmds(const TBldStr& strTarget)
{
    // If not recursing, then just call directly
    if (m_bNoRecurse)
    {
        bCustomCmdsCallBack(strTarget, 1);
        return;
    }

    //
    //  Create an iterator and have it call our private header update method
    //  for each dependent of the target.
    //
    TMethProjIterator<TFacCIDBuild> piterHdrs(this, &TFacCIDBuild::bCustomCmdsCallBack);
    if (!m_pplistProjs->bIterate(piterHdrs, strTarget, tCIDBuild::EIterModes::FromBottomMin))
        stdOut << L"Project dependency iteration failed" << kCIDBuild::EndLn;
}
