//
// FILE NAME: CIDBuild_KCCDriver.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/24/99
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
//  This file implements the TKCCDriver class.
//
// CAVEATS/GOTCHAS:
//
//  1)  Each driver includes its own header directly, so that it does not
//      show up to other code. Only the TToolsDriver.Cpp file includes all
//      of the concrete driver instantiation headers, and uses conditional
//      compilation to include only the one that is required.
//
// LOG:
//
//  $_CIDLib_Log_$
//
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"
#include    "CIDBuild_KCCDriver.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TKCCDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKCCDriver: Constructors and Destructor
// ---------------------------------------------------------------------------
TKCCDriver::TKCCDriver() :

    m_bDebug(false)
{
    //
    //  Get a short cut indicator for whether we are doing a debug or prod
    //  build, since we use it a lot below.
    //
    m_bDebug = (facCIDBuild.eBldMode() == tCIDBuild::EBldModes::Develop);
}

TKCCDriver::~TKCCDriver()
{
}


// ---------------------------------------------------------------------------
//  TKCCDriver: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDBuild::TBoolean TKCCDriver::_bBuild(const TProjectInfo& projiTarget)
{
    // Store away the target project for internal use while we are here
    m_pprojiTarget = &projiTarget;

    // Build up the target file name. We take the already computed one
    // (minus the path) to avoid duplicating the procedure.
    tCIDBuild::TUInt uiName;
    tCIDBuild::TUInt uiExt;
    TUtils::FindPathParts(m_pprojiTarget->strOutBin(), uiName, uiExt);
    m_strTargetFile.CopyAt(m_pprojiTarget->strOutBin(), uiName);

    // If it's a shared library then we've got to strip the version
    // part for compilation. We'll add it back later.
    if (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Dll)
    {
        m_strTargetFile.CapAt(m_strTargetFile.uiLength()
                              - facCIDBuild.strVersionSuffix().uiLength());
    }

    //
    //  Lets do our Cpp compilations. We just run through each of the Cpp
    //  files, which our parent class has has already set up, and ask if
    //  each one needs to be compiled. If so, we build it.
    //
    bool bObjBuilt = bCompileCpps();

    //
    //  NOTE: We change to the main output directory before we link, so all
    //  of the file names are relative to that. This makes the link line
    //  a LOT smaller.
    //
    if (!TUtils::bChangeDir(facCIDBuild.strOutDir()))
    {
        stdOut << NStr("Could not change to output directory for link step") << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    if (bObjBuilt && facCIDBuild.bVerbose())
        stdOut << NStr("    Linking because an Obj was rebuilt") << kCIDBuild::EndLn;

    //
    //  Now lets see if the target file is out of date with any of its Obj,
    //  Lib, Res, etc... files. If so, then we have to build it. If any
    //  Obj files were built, then don't bother checking since we know that
    //  we have to link.
    //
    //  If the Force option is on, then of course we have to rebuild and
    //  there is no reason to check anything.
    //
    bool bUpdate = (bObjBuilt || facCIDBuild.bForce());
    if (!bUpdate)
    {
        // See if the target exists, if not obviously we have to build
        TFindInfo fndiTarget;
        if (!TFindInfo::bFindAFile(m_strTargetFile, fndiTarget))
        {
            bUpdate = true;

            if (facCIDBuild.bVerbose())
                stdOut << NStr("    Target file not present: ") << m_strTargetFile << kCIDBuild::EndLn;
        }

        //
        //  If nothing to do yet, then see if any of the Lib or Obj files
        //  are newer than the target.
        //
        if (!bUpdate)
        {
            if (bObjsNewer(fndiTarget) || bLibsNewer(fndiTarget))
                bUpdate = true;
        }
    }

    if (bUpdate)
        Link();

    // It went ok, so return success
    return true;
}



// ---------------------------------------------------------------------------
//  TKCCDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDBuild::TBoolean TKCCDriver::bCompileCpps()
{
    //
    //  First we want to build up the list of command line parameters that
    //  will be used to compile a Cpp file. For our Visual C++ compiler, the
    //  order of stuff is not important, so we can just build up the stuff
    //  that will stay the same.
    //
    const tCIDBuild::TCh* apszArgs[128];
    tCIDBuild::TUInt uiCurArg = 0;
    apszArgs[uiCurArg++] = L"KCC";
    apszArgs[uiCurArg++] = L"-c";

    // Set up the debug/prod flags
    if (m_bDebug)
    {
        apszArgs[uiCurArg++] = L"+K0";
        apszArgs[uiCurArg++] = L"-DCID_DEGUG_ON=1";
    }
    else
    {
        apszArgs[uiCurArg++] = L"+K3";
        apszArgs[uiCurArg++] = L"-DCID_DEGUG_ON=0";
    }

    //
    //  Set up our CIDLib platform define that gets passed to all code
    //  to drive conditional compilation.
    //
    TBldStr strPlatform(L"-D");
    strPlatform.Append(kCIDBuild::pszTargetPlatform);
    apszArgs[uiCurArg++] = strPlatform.pszBuffer();

    //
    //  Set up our CIDLib build mode define that gets passed to all code
    //  to drive debug/production compilation.
    //
    TBldStr strBuildMode(L"-D");
    strBuildMode.Append(kCIDBuild::pszBuildMode);
    apszArgs[uiCurArg++] = strBuildMode.pszBuffer();

    // Set the flags for the project type
    if (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Dll)
    {
        apszArgs[uiCurArg++] = L"-fPIC";
        apszArgs[uiCurArg++] = L"--one_per";
    }

    //
    //  Set up our standard include directories, for public and private
    //  includes and their respective per-platform subdirectories
    //
    TBldStr strInclude(L"-I");
    strInclude.Append(facCIDBuild.strIncludeDir());
    apszArgs[uiCurArg++] = strInclude.pszBuffer();

    TBldStr strPPInclude(L"-I");
    strPPInclude.Append(facCIDBuild.strPPIncludeDir());
    apszArgs[uiCurArg++] = strPPInclude.pszBuffer();

    TBldStr strPrivInclude(L"-I");
    strPrivInclude.Append(facCIDBuild.strPrivIncludeDir());
    apszArgs[uiCurArg++] = strPrivInclude.pszBuffer();

    TBldStr strPPPrivInclude(L"-I");
    strPPPrivInclude.Append(facCIDBuild.strPPPrivIncludeDir());
    apszArgs[uiCurArg++] = strPPPrivInclude.pszBuffer();

    //
    //  If there are any per-project includes, then add them in as well.
    //
    //  NOTE:
    //  In order to avoid a lot of crap, we just allow these to leak. This
    //  is ok since this program is always just a one shot anyway and this
    //  is not a repetitious operation.
    //
    if (m_pprojiTarget->listIncludes().uiElemCount())
    {
        TBldStr strTmp;

        TList<TBldStr>::TCursor cursIncludes(&m_pprojiTarget->listIncludes());
        cursIncludes.bResetIter();
        do
        {
            strTmp = L"-I";
            strTmp.Append(cursIncludes.tCurElement());
            apszArgs[uiCurArg++] = strTmp.pszOrphanBuffer();
        }   while (cursIncludes.bNext());
    }

    //
    //  Set up the flags for the runtime library mode. This is driven by
    //  the RTL mode and current build mode, at least it is on VC++.
    //
    if (m_pprojiTarget->eRTLMode() == tCIDBuild::ERTLModes::MultiStatic
        || m_pprojiTarget->eRTLMode() == tCIDBuild::ERTLModes::MultiDynamic)
    {
        apszArgs[uiCurArg++] = L"--thread_safe";
    }

    // Set up the stuff driven by the 'PUREANSI' setting.
    if (m_pprojiTarget->bPureANSI())
    {
        apszArgs[uiCurArg++] = L"--strict";
        apszArgs[uiCurArg++] = L"-Dm_KAI_STRICT=1";
    }

    //
    //  Deal with the character mode. The mode to build target code in is
    //  set by the user. If Unicode mode, we also have to set some VC++
    //  flags.
    //
    apszArgs[uiCurArg++] = L"-DCHARMODE_ASCII";

    //
    //  Every project has a define which is in the form PROJ_XXX, where XXX
    //  is the name of the project uppercased.
    //
    TBldStr strProjDef(L"-DPROJ_");
    strProjDef.Append(m_pprojiTarget->strProjectName());
    strProjDef.UpperCase();
    apszArgs[uiCurArg++] = strProjDef.pszBuffer();

    //
    //  Set up the precompiled header for this project. There is no project
    //  setting for this since its just done if the environment supports it.
    //
    apszArgs[uiCurArg++] = L"--pch";
    apszArgs[uiCurArg++] = L"--no_pch_messages";
    apszArgs[uiCurArg++] = L"--pch_dir";
    apszArgs[uiCurArg++] = m_pprojiTarget->strOutDir().pszBuffer();

    // This one is in case we need to suppress the warning
    // about the packing pragmas being in the wrong place
    #if defined(NO_PACK_WARNING)
    apszArgs[uiCurArg++] = L"--diag_suppress";
    if (m_pprojiTarget->strProjectName() == L"CIDKernel")
        apszArgs[uiCurArg++] = L"608,280,47";
    else
        apszArgs[uiCurArg++] = L"608";
    #else
    if (m_pprojiTarget->strProjectName() == L"CIDKernel")
    {
        apszArgs[uiCurArg++] = L"--diag_suppress";
        apszArgs[uiCurArg++] = L"280,47";
    }
    #endif

    //
    //  Remember the index at which the file independent parameters stopped
    //  so that we can just add onto the list what we need below.
    //
    const tCIDBuild::TUInt uiFirstPerFileArg = uiCurArg;

    //
    //  And now check each Cpp file and see if we have to build it. If so,
    //  the do it. Keep track of whether we build anything and return that
    //  status.
    //
    TList<TDepInfo>::TCursor cursCpps(&_listCpps());
    if (!cursCpps.bResetIter())
        return false;

    tCIDBuild::TBoolean bBuiltSomething = false;
    do
    {
        // Get a ref to the current element
        const TDepInfo& depiCur = cursCpps.tCurElement();

        // If this guy does not need to be compiled, check the next one
        if (!_bMustCompile(*m_pprojiTarget, depiCur))
            continue;

        // Keep up with the per-file args we add
        tCIDBuild::TUInt uiExtraArgs = uiFirstPerFileArg;

        // Setup output
        apszArgs[uiExtraArgs++] = L"-o";
        TBldStr strJustName(depiCur.strObjFileName());
        tCIDBuild::TUInt uiName, uiExtension;
        TUtils::FindPathParts(strJustName, uiName, uiExtension);
        strJustName.Cut(uiName);
        TBldStr strOutput(m_pprojiTarget->strOutDir(), strJustName);
        apszArgs[uiExtraArgs++] = strOutput.pszBuffer();

        // Add the current file as the source file
        apszArgs[uiExtraArgs++] = depiCur.strFileName().pszBuffer();

        if (facCIDBuild.bVerbose())
        {
            for (tCIDBuild::TUInt uiInd = 0; uiInd < uiExtraArgs; uiInd++)
                stdOut << apszArgs[uiInd] << NStr(" ");
            stdOut << kCIDBuild::EndLn;
        }

        // Ok, lets do the compilation
        tCIDBuild::TUInt uiResult;
        if (!TUtils::bExec(apszArgs, uiExtraArgs, uiResult))
        {
            stdOut << NStr("Could not execute the compiler") << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }

        if (uiResult)
        {
            stdOut  << NStr("Failed compilation of file: ")
                    << depiCur.strFileName() << NStr(". Error Code: ")
                    << uiResult << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }

        // Make sure we have a line after the output if in verbose mode
        if (facCIDBuild.bVerbose())
            stdOut << kCIDBuild::EndLn;

        // Indicate that we built at least one Obj file
        bBuiltSomething = true;
    }   while (cursCpps.bNext());

    return bBuiltSomething;
}


tCIDBuild::TBoolean TKCCDriver::bLibsNewer(const TFindInfo& fndiTarget) const
{
    // Get a cursor for the libs list
    TList<TFindInfo>::TCursor cursLibs(&_listLibs());

    // If no libs listed, then couldn't be newer
    if (!cursLibs.bResetIter())
        return false;

    do
    {
        //
        //  See if this Lib file is newer than the target. If so, then
        //  return true, since we only have to find one to know that we
        //  have to relink.
        //
        if (cursLibs.tCurElement() > fndiTarget)
        {
            if (facCIDBuild.bVerbose())
            {
                stdOut  << NStr("    Dependent library is newer: ")
                        << cursLibs.tCurElement().strFileName() << kCIDBuild::EndLn;
            }
            return true;
        }

    }   while (cursLibs.bNext());
    return false;
}


tCIDBuild::TBoolean TKCCDriver::bMakeRes()
{
    return true;
}


tCIDBuild::TBoolean TKCCDriver::bObjsNewer(const TFindInfo& fndiTarget) const
{
    // Get a cursor for the Cpp list
    TList<TDepInfo>::TCursor cursCpps(&_listCpps());

    // If no obect files listed, then couldn't be newer
    if (!cursCpps.bResetIter())
        return false;

    TFindInfo fndiTmp;
    do
    {
        // Get a ref to the current element
        const TDepInfo& depiCur = cursCpps.tCurElement();

        // Try to find the current obj
        if (!TFindInfo::bFindAFile(depiCur.strObjFileName(), fndiTmp))
        {
            stdOut  << NStr("Could not find object file: ")
                    << depiCur.strObjFileName() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        //
        //  See if the Obj file is newer than the target. If so, then
        //  return true, since we only have to find one to know that we
        //  have to relink.
        //
        if (fndiTmp > fndiTarget)
        {
            if (facCIDBuild.bVerbose())
            {
                stdOut  << NStr("    Object file is newer: ")
                        << depiCur.strObjFileName() << kCIDBuild::EndLn;
            }
            return true;
        }
    }   while (cursCpps.bNext());
    return false;
}


tCIDBuild::TVoid TKCCDriver::Link()
{
    const tCIDBuild::TCh* apszArgs[128];
    tCIDBuild::TUInt uiCurArg = 0;

    // Set up the standard stuff
    apszArgs[uiCurArg++] = L"KCC";

    if (m_pprojiTarget->eRTLMode() == tCIDBuild::ERTLModes::MultiStatic
        || m_pprojiTarget->eRTLMode() == tCIDBuild::ERTLModes::MultiDynamic)
    {
        apszArgs[uiCurArg++] = L"--thread_safe";
    }

    // Set up the output file. It is a versioned file and the name is prebuilt
    apszArgs[uiCurArg++] = L"-o";
    apszArgs[uiCurArg++] = m_strTargetFile.pszBuffer();

    // And the debug vs. production flags
    if (m_bDebug)
        apszArgs[uiCurArg++] = L"+K0";
    else
        apszArgs[uiCurArg++] = L"+K3";

    //
    //  And now add in all of the Obj and Lib files. We built them up above
    //  when we needed to check all of the source for staleness of the target.
    //  So we just reference those existing names with our argument list.
    //
    TList<TFindInfo>::TCursor cursLibs(&_listLibs());
    if (cursLibs.bResetIter())
    {
        do
        {
            apszArgs[uiCurArg++] = cursLibs.tCurElement().strFileName().pszBuffer();
        }   while (cursLibs.bNext());
    }

    TList<TDepInfo>::TCursor cursCpps(&_listCpps());
    if (cursCpps.bResetIter())
    {
        do
        {
            // Get a ref to the current element
            const TDepInfo& depiCur = cursCpps.tCurElement();

            // And add its object file name to the list of args
            apszArgs[uiCurArg++] = depiCur.strObjFileName().pszBuffer();
        }   while (cursCpps.bNext());
    }

    if (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Exe)
    {
        apszArgs[uiCurArg++] = L"-lncurses";
        apszArgs[uiCurArg++] = L"-ldl";
        apszArgs[uiCurArg++] = L"-lm";
    }
    else if (m_pprojiTarget->strProjectName() == TBldStr("CIDKernel"))
    {
        apszArgs[uiCurArg++] = L"-lreadline";
        apszArgs[uiCurArg++] = L"-lhistory";
    }

    if (facCIDBuild.bVerbose())
    {
        for (tCIDBuild::TUInt uiInd = 0; uiInd < uiCurArg; uiInd++)
            stdOut << apszArgs[uiInd] << NStr(" ");
        stdOut << kCIDBuild::EndLn;
    }

    // And invoke the link
    tCIDBuild::TUInt uiResult;
    if (!TUtils::bExec(apszArgs, uiCurArg, uiResult))
    {
        stdOut << NStr("Could not execute the linker") << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    if (uiResult)
    {
        stdOut << NStr("Link step failed. Error Code:") << uiResult << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    if (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Dll)
    {
        TBldStr strRealName(m_strTargetFile);
        strRealName.Append(facCIDBuild.strVersionSuffix());

        tCIDBuild::TNatCh* pszRealName = TRawStr::pszTranscode(strRealName.pszBuffer());
        TArrayJanitor<tCIDBuild::TNatCh> janReal(pszRealName);
        tCIDBuild::TNatCh* pszTarget = TRawStr::pszTranscode(m_strTargetFile.pszBuffer());
        TArrayJanitor<tCIDBuild::TNatCh> janTarget(pszTarget);

        if (::rename(pszTarget, pszRealName))
        {
            stdOut << NStr("Unable to rename target file") << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }

        if (::symlink(pszRealName, pszTarget))
        {
            stdOut << NStr("Unable to create symbolic link") << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }
    }
}
