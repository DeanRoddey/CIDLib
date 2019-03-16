//
// FILE NAME: CIDBuild_VCppDriver.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This file implements the TVCppDriver class.
//
// CAVEATS/GOTCHAS:
//
//  1)  Each driver includes its own header directly, so that it does not
//      show up to other code. Only the TToolsDriver.cpp file includes all
//      of the concrete driver instantiation headers, and uses conditional
//      compilation to include only the one that is required.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"
#include    "CIDBuild_VCppDriver.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TVCppDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TVCppDriver: Constructors and Destructor
// ---------------------------------------------------------------------------
TVCppDriver::TVCppDriver() :

    m_bDebug(kCIDLib::False)
{
    //
    //  Get a short cut indicator for whether we are doing a debug or prod
    //  build, since we use it a lot below.
    //
    m_bDebug = (facCIDBuild.eBldMode() == tCIDBuild::EBldModes::Develop);
}

TVCppDriver::~TVCppDriver()
{
}


// ---------------------------------------------------------------------------
//  TVCppDriver: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TVCppDriver::bBuild(const TProjectInfo& projiTarget)
{
    // Store away the target project for internal use while we are here
    m_pprojiTarget = &projiTarget;

    //
    //  Build up the output file. If its versioned, then add the version
    //  postfix.
    //
    m_strTargetFile = projiTarget.strProjectName();
    if (projiTarget.bVersioned())
        m_strTargetFile.Append(facCIDBuild.strVersionSuffix());

    if ((projiTarget.eType() == tCIDBuild::EProjTypes::SharedLib)
    ||  (projiTarget.eType() == tCIDBuild::EProjTypes::SharedObj))
    {
        m_strTargetFile.Append(kCIDBuild::pszDllExt);
    }
     else if (projiTarget.eType() == tCIDBuild::EProjTypes::StaticLib)
    {
        m_strTargetFile.Append(kCIDBuild::pszLibExt);
    }
     else if ((projiTarget.eType() == tCIDBuild::EProjTypes::Executable)
          ||  (projiTarget.eType() == tCIDBuild::EProjTypes::Service))
    {
        m_strTargetFile.Append(kCIDBuild::pszExeExt);
    }
     else
    {
        stdOut  << L"Unknown project type, can't set output prefix"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    //
    //  Lets do our Cpp compilations. We just run through each of the Cpp
    //  files, which our parent class has has already set up, and ask if
    //  each one needs to be compiled. If so, we build it.
    //
    tCIDLib::TBoolean bObjBuilt = bCompileCpps();

    //
    //  NOTE: We change to the main output directory before we link, so all
    //  of the file names are relative to that. This makes the link line
    //  a LOT smaller.
    //
    if (!TUtils::bChangeDir(facCIDBuild.strOutDir()))
    {
        stdOut  << L"Could not change to output directory for link step"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    if ((bObjBuilt) && facCIDBuild.bVerbose())
        stdOut << L"    Linking because an Obj was rebuilt" << kCIDBuild::EndLn;

    //
    //  Now lets see if the target file is out of date with any of its Obj,
    //  Lib, Res, etc... files. If so, then we have to build it. If any
    //  Obj files were built, then don't bother checking since we know that
    //  we have to link.
    //
    //  If the Force option is on, then of course we have to rebuild and
    //  there is no reason to check anything.
    //
    tCIDLib::TBoolean bUpdate = (bObjBuilt || facCIDBuild.bForce());
    if (!bUpdate)
    {
        // See if the target exists, if not obviously we have to build
        TFindInfo fndiTarget;
        if (!TFindInfo::bFindAFile(m_strTargetFile, fndiTarget))
        {
            bUpdate = kCIDLib::True;

            if (facCIDBuild.bVerbose())
            {
                stdOut  << L"    Target file not present: "
                        << m_strTargetFile << kCIDBuild::EndLn;
            }
        }

        //
        //  If nothing to do yet, then see if any of the Lib or Obj files
        //  are newer than the target.
        //
        if (!bUpdate)
        {
            if (bObjsNewer(fndiTarget) || bLibsNewer(fndiTarget))
                bUpdate = kCIDLib::True;
        }
    }

    if (bUpdate)
    {
        if ((projiTarget.eType() == tCIDBuild::EProjTypes::SharedLib)
        ||  (projiTarget.eType() == tCIDBuild::EProjTypes::SharedObj)
        ||  (projiTarget.eType() == tCIDBuild::EProjTypes::Executable)
        ||  (projiTarget.eType() == tCIDBuild::EProjTypes::Service))
        {
            Link(kCIDLib::False);
        }
         else if (projiTarget.eType() == tCIDBuild::EProjTypes::StaticLib)
        {
            LinkStatic();
        }
         else
        {
            // This is bad, we don't know what it is
            stdOut  << L"Unknown project type, can't link"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        // Call the post link step, to allow for per-platform twiddling
        PostLink(projiTarget, m_strTargetFile);
    }

    // It went ok, so return success
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TVCppDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TVCppDriver::bCompileCpps()
{
    TBldStr strTmp;

    //
    //  First we want to build up the list of command line parameters that
    //  will be used to compile a Cpp file. For our Visual C++ compiler, the
    //  order of stuff is not important, so we can just build up the stuff
    //  that will stay the same.
    //
    //  Some of the strings have to be allocated, so we also have a flag
    //  array to remember which ones.
    //
    const tCIDLib::TCard4 c4MaxParms = 256;
    tCIDLib::TBoolean abDynAlloc[c4MaxParms];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MaxParms; c4Index++)
        abDynAlloc[c4Index] = kCIDLib::False;

    const tCIDLib::TCh* apszArgs[c4MaxParms];
    tCIDLib::TCard4 c4CurArg = 0;
    apszArgs[c4CurArg++] = L"CL.Exe";
    apszArgs[c4CurArg++] = L"/c";
    apszArgs[c4CurArg++] = L"/EHa";     // Needed for /GR+
    apszArgs[c4CurArg++] = L"/GR";      // Enable RTTI
    apszArgs[c4CurArg++] = L"/FD";
    apszArgs[c4CurArg++] = L"/DSTRICT";
    apszArgs[c4CurArg++] = L"/nologo";
    apszArgs[c4CurArg++] = L"/DWIN32";
    apszArgs[c4CurArg++] = L"/Zp8";     // Pack on 8 bytes unless forced otherwise

    //
    //  Multi-core support during compiles if not low priority or in single
    //  mode. We set the count  to the count minus one.
    //
    TBldStr strMP = L"/MP";
    if (!facCIDBuild.bLowPrio() && !facCIDBuild.bSingle())
    {
        tCIDLib::TCard4 c4Count = facCIDBuild.c4CPUCount();
        if (c4Count > 1)
            c4Count--;
        strMP.Append(c4Count);
        apszArgs[c4CurArg++] = strMP.pszBuffer();
    }

    //
    //  Enable SSE2 support which should be available on anything that CIDLib based apps
    //  would run on these days. We cannot enable /fp:fast because we enable strict
    //  ANSI for all facilities that are not explicitly marked as not being compliant,
    //  and fast mode is not ANSI compliant.
    //
    apszArgs[c4CurArg++] = L"/arch:SSE2";

    // We always want wchar_t to be an intrinsic type
    apszArgs[c4CurArg++] = L"/Zc:wchar_t";

    //
    //  We always build for UNICODE mode. These are for the platform headers,
    //  and have no affect on the CIDLib code itself.
    //
    apszArgs[c4CurArg++] = L"/DUNICODE";
    apszArgs[c4CurArg++] = L"/D_UNICODE";


    //
    //  Set some stuff according to the platforms we support. These are used
    //  by the system headers, and a little bit of code in our Win32 platform
    //  specific driver code, where we have to call slightly different APIs
    //  according to targeted platform.
    //
    if (facCIDBuild.strFullPlatform().bIEquals(L"WIN32_WIN7"))
    {
        apszArgs[c4CurArg++] = L"/DNTDDI_VERSION=NTDDI_WIN7";
    }
     else
    {
        stdOut  << facCIDBuild.strFullPlatform()
                << L" is not a platform understood by the VC++ driver" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    // If a service, support conditional compilation for that
    if (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Service)
        apszArgs[c4CurArg++] = L"/DCIDSERVICE";

    // Set either normal or maximum warning level
    if (facCIDBuild.bMaxWarn())
        apszArgs[c4CurArg++] = L"/W4";
    else
        apszArgs[c4CurArg++] = L"/W3";

    // For libraries we do some extra bits
    if ((m_pprojiTarget->eType() == tCIDBuild::EProjTypes::SharedLib)
    ||  (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::SharedObj))
    {
        apszArgs[c4CurArg++] = L"/D_WINDOWS";
        apszArgs[c4CurArg++] = L"/LD";
    }

    // Set up the debug/prod flags
    if (m_bDebug)
    {
        apszArgs[c4CurArg++] = L"/Od";
        apszArgs[c4CurArg++] = L"/D_DEBUG";
        apszArgs[c4CurArg++] = L"/Zi";
        apszArgs[c4CurArg++] = L"/RTC1";
    }
     else
    {
        //
        //  Enable plenty of optimization
        ///
        apszArgs[c4CurArg++] = L"/O2";
        apszArgs[c4CurArg++] = L"/DNDEBUG";
    }

    // If a pure ANSI project, then disable extension
    if (m_pprojiTarget->bPureANSI())
        apszArgs[c4CurArg++] = L"/Za";

    apszArgs[c4CurArg++] = L"/std:c++17";

    // Disable permissive mode if asked
    if (facCIDBuild.bNonPermissive())
    {
        apszArgs[c4CurArg++] = L"/permissive-";

        // For now we have to keep two phase old style behavior
        apszArgs[c4CurArg++] = L"/Zc:twoPhase-";
    }


    //
    //  Set the version defines. This passes the release version info from the
    //  build environment on to the stuff compiled, so that they can use them
    //  if they want (and most do.)
    //
    TBldStr strMajVer(L"/DCID_MAJVER=");
    strTmp.Format(facCIDBuild.c4MajVer());
    strMajVer.Append(strTmp);
    apszArgs[c4CurArg++] = strMajVer.pszBuffer();

    TBldStr strMinVer(L"/DCID_MINVER=");
    strTmp.Format(facCIDBuild.c4MinVer());
    strMinVer.Append(strTmp);
    apszArgs[c4CurArg++] = strMinVer.pszBuffer();

    TBldStr strRevision(L"/DCID_REVISION=");
    strTmp.Format(facCIDBuild.c4Revision());
    strRevision.Append(strTmp);
    apszArgs[c4CurArg++] = strRevision.pszBuffer();

    TBldStr strVerString(L"/DCID_VERSTRING=");
    strTmp.Format(facCIDBuild.c4MajVer());
    strVerString.Append(strTmp);
    strVerString.Append(L".");
    strTmp.Format(facCIDBuild.c4MinVer());
    strVerString.Append(strTmp);
    strVerString.Append(L".");
    strTmp.Format(facCIDBuild.c4Revision());
    strVerString.Append(strTmp);
    apszArgs[c4CurArg++] = strVerString.pszBuffer();

    TBldStr strVerSuff(L"/DCID_VERSUFF=");
    strVerSuff.Append(facCIDBuild.strVersionSuffix());
    apszArgs[c4CurArg++] = strVerSuff.pszBuffer();

    //
    //  Set up our standard include directories, for public and private
    //  includes and their respective per-platform subdirectories
    //
    TBldStr strInclude(L"/I");
    strInclude.Append(facCIDBuild.strIncludeDir());
    apszArgs[c4CurArg++] = strInclude.pszBuffer();

    TBldStr strPPInclude(L"/I");
    strPPInclude.Append(facCIDBuild.strPPIncludeDir());
    apszArgs[c4CurArg++] = strPPInclude.pszBuffer();

    TBldStr strPrivInclude(L"/I");
    strPrivInclude.Append(facCIDBuild.strPrivIncludeDir());
    apszArgs[c4CurArg++] = strPrivInclude.pszBuffer();

    TBldStr strPPPrivInclude(L"/I");
    strPPPrivInclude.Append(facCIDBuild.strPPPrivIncludeDir());
    apszArgs[c4CurArg++] = strPPPrivInclude.pszBuffer();

    // If there are any per-project include paths, then add them in as well.
    if (m_pprojiTarget->listIncludePaths().c4ElemCount())
    {
        TList<TBldStr>::TCursor cursIncludePaths(&m_pprojiTarget->listIncludePaths());
        cursIncludePaths.bResetIter();
        do
        {
            tCIDLib::TCh* pszNew = new tCIDLib::TCh
            [
                TRawStr::c4StrLen(cursIncludePaths.tCurElement().pszBuffer())
                + 16
            ];
            TRawStr::CopyStr(pszNew, L"/I");
            TRawStr::CatStr(pszNew, cursIncludePaths.tCurElement().pszBuffer());
            abDynAlloc[c4CurArg] = kCIDLib::True;
            apszArgs[c4CurArg++] = pszNew;
        }   while (cursIncludePaths.bNext());
    }

    // And do the same for any system wide includes
    if (facCIDBuild.listExtIncludePaths().c4ElemCount())
    {
        TList<TBldStr>::TCursor cursIncludePaths(&facCIDBuild.listExtIncludePaths());
        cursIncludePaths.bResetIter();
        do
        {
            tCIDLib::TCh* pszNew = new tCIDLib::TCh
            [
                TRawStr::c4StrLen(cursIncludePaths.tCurElement().pszBuffer())
                + 8
            ];
            TRawStr::CopyStr(pszNew, L"/I");
            TRawStr::CatStr(pszNew, cursIncludePaths.tCurElement().pszBuffer());
            abDynAlloc[c4CurArg] = kCIDLib::True;
            apszArgs[c4CurArg++] = pszNew;
        }   while (cursIncludePaths.bNext());
    }

    //
    //  Set up the flags for the runtime library mode. This is driven by
    //  the RTL mode and current build mode, at least it is on VC++.
    //
    switch(m_pprojiTarget->eRTLMode())
    {
        case tCIDBuild::ERTLModes::SingleStatic :
            if (m_bDebug)
                apszArgs[c4CurArg++] = L"/MLd";
            else
                apszArgs[c4CurArg++] = L"/ML";
            break;

        case tCIDBuild::ERTLModes::SingleDynamic :
            // This is the default, so not options are required
            break;

        case tCIDBuild::ERTLModes::MultiStatic :
            if (m_bDebug)
                apszArgs[c4CurArg++] = L"/MTd";
            else
                apszArgs[c4CurArg++] = L"/MT";
            break;

        case tCIDBuild::ERTLModes::MultiDynamic :
            if (m_bDebug)
                apszArgs[c4CurArg++] = L"/MDd";
            else
                apszArgs[c4CurArg++] = L"/MD";
            break;

        default :
            stdOut << L"Unknown RTL mode" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
            break;
    }

    //
    //  Set the parameters for the display type. Under Visual C++ we just
    //  need to indicate if it is a console, otherwise its assumed to be
    //  a GUI display.
    //
    if (m_pprojiTarget->eDisplayType() == tCIDBuild::EDisplayTypes::Console)
        apszArgs[c4CurArg++] = L"/D_CONSOLE";

    //
    //  Every project has a define which is in the form PROJ_XXX, where XXX
    //  is the name of the project uppercased.
    //
    TBldStr strProjDef(L"/DPROJ_");
    strProjDef.Append(m_pprojiTarget->strProjectName());
    strProjDef.UpperCase();
    apszArgs[c4CurArg++] = strProjDef.pszBuffer();

    // Set up the debug database
    TBldStr strPDBName(L"/Fd");
    strPDBName.Append(facCIDBuild.strOutDir());
    strPDBName.Append(m_pprojiTarget->strProjectName());
    if (m_pprojiTarget->bVersioned())
        strPDBName.Append(facCIDBuild.strVersionSuffix());
    apszArgs[c4CurArg++] = strPDBName.pszBuffer();

    // Set up the Obj output directory param
    TBldStr strOutDir(L"/Fo");
    strOutDir.Append(m_pprojiTarget->strOutDir());
    if (strOutDir.chLast() == L'\\')
    apszArgs[c4CurArg++] = strOutDir.pszBuffer();

    //
    //  And now check each Cpp file and see if we have to build it. If so,
    //  the do it. Keep track of whether we build anything and return that
    //  status.
    //
    TList<TDepInfo>::TCursor cursCpps(&listCpps());
    if (!cursCpps.bResetIter())
        return kCIDLib::False;

    tCIDLib::TBoolean bBuiltSomething = kCIDLib::False;
    TBldStr strTargets;
    do
    {
        // Get a ref to the current element
        const TDepInfo& depiCur = cursCpps.tCurElement();

        // If this guy does not need to be compiled, check the next one
        if (!bMustCompile(*m_pprojiTarget, depiCur))
            continue;

        // Indicate that we built at least one Obj file
        bBuiltSomething = kCIDLib::True;

        apszArgs[c4CurArg++] = depiCur.strFileName().pszBuffer();
    }   while (cursCpps.bNext());


    // If any targets need updating, then let's do it
    if (bBuiltSomething)
    {

        if (facCIDBuild.bVerbose())
        {
            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4CurArg; c4Ind++)
                stdOut << apszArgs[c4Ind] << L" ";
            stdOut << kCIDBuild::EndLn;
        }

        // Ok, lets do the compilation
        tCIDLib::TCard4 c4Result;
        if (!TUtils::bExec(apszArgs, c4CurArg, c4Result, facCIDBuild.bLowPrio()))
        {
            stdOut << L"Could not execute the compiler" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }

        if (c4Result)
        {
            stdOut  << L"Failed compilation of targets: "
                    << strTargets << L". Error Code: "
                    << c4Result << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::BuildError;
        }

        // Make sure we have a line after the output if in verbose mode
        if (facCIDBuild.bVerbose())
            stdOut << kCIDBuild::EndLn;
    }

    // Delete any dynamically allocated parameters
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MaxParms; c4Index++)
    {
        if (abDynAlloc[c4Index])
            delete [] apszArgs[c4Index];
    }
    return bBuiltSomething;
}


tCIDLib::TBoolean TVCppDriver::bLibsNewer(const TFindInfo& fndiTarget) const
{
    // Get a cursor for the libs list
    TList<TFindInfo>::TCursor cursLibs(&listLibs());

    // If no libs listed, then couldn't be newer
    if (!cursLibs.bResetIter())
        return kCIDLib::False;

    do
    {
        //
        //  See if this Lib file is newer than the target. If so, then
        //  return kCIDLib::True, since we only have to find one to know that we
        //  have to relink.
        //
        if (cursLibs.tCurElement() > fndiTarget)
        {
            if (facCIDBuild.bVerbose())
            {
                stdOut  << L"    Dependent library is newer: "
                        << cursLibs.tCurElement().strFileName() << kCIDBuild::EndLn;
            }
            return kCIDLib::True;
        }

    }   while (cursLibs.bNext());
    return kCIDLib::False;
}


tCIDLib::TBoolean TVCppDriver::bObjsNewer(const TFindInfo& fndiTarget) const
{
    // Get a cursor for the Cpp list
    TList<TDepInfo>::TCursor cursCpps(&listCpps());

    // If no obect files listed, then couldn't be newer
    if (!cursCpps.bResetIter())
        return kCIDLib::False;

    TFindInfo fndiTmp;
    do
    {
        // Get a ref to the current element
        const TDepInfo& depiCur = cursCpps.tCurElement();

        // Try to find the current obj
        if (!TFindInfo::bFindAFile(depiCur.strObjFileName(), fndiTmp))
        {
            stdOut  << L"Could not find object file: "
                    << depiCur.strObjFileName() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        //
        //  See if the Obj file is newer than the target. If so, then
        //  return kCIDLib::True, since we only have to find one to know that we
        //  have to relink.
        //
        if (fndiTmp > fndiTarget)
        {
            if (facCIDBuild.bVerbose())
            {
                stdOut  << L"    Object file is newer: "
                        << depiCur.strObjFileName() << kCIDBuild::EndLn;
            }
            return kCIDLib::True;
        }
    }   while (cursCpps.bNext());
    return kCIDLib::False;
}


tCIDLib::TVoid TVCppDriver::Link(const tCIDLib::TBoolean bHaveResFile)
{
    const tCIDLib::TCard4   c4MaxParms(128);
    const tCIDLib::TCh*     apszArgs[c4MaxParms];
    tCIDLib::TCard4         c4CurArg = 0;
    TBldStr                 strTmp;

    // Set up the standard stuff
    apszArgs[c4CurArg++] = L"Link.Exe";
    apszArgs[c4CurArg++] = L"/nologo";
    apszArgs[c4CurArg++] = L"/machine:X86";
    apszArgs[c4CurArg++] = L"/NODEFAULTLIB:LIBCMT";

    //
    //  Set up the map file output. If this is a versioned project, then
    //  add the version postfix.
    //
    TBldStr strMap(L"/map:");
    strMap.Append(m_pprojiTarget->strProjectName());
    if (m_pprojiTarget->bVersioned())
        strMap.Append(facCIDBuild.strVersionSuffix());
    strMap.Append(L".map");
    apszArgs[c4CurArg++] = strMap.pszBuffer();

    // Set up the output file.
    TBldStr strOut(L"/out:");
    strOut.Append(m_strTargetFile);
    apszArgs[c4CurArg++] = strOut.pszBuffer();

    // Set flags based on the type of the program
    if ((m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Executable)
    ||  (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::Service))
    {
        if (m_pprojiTarget->eDisplayType() == tCIDBuild::EDisplayTypes::Console)
            apszArgs[c4CurArg++] = L"/subsystem:console";
        else
            apszArgs[c4CurArg++] = L"/subsystem:windows";
    }
     else if ((m_pprojiTarget->eType() == tCIDBuild::EProjTypes::SharedLib)
          ||  (m_pprojiTarget->eType() == tCIDBuild::EProjTypes::SharedObj))
    {
        apszArgs[c4CurArg++] = L"/dll";

        //
        //  In libraries, force retension of unreferenced functions because
        //  they may be needed in the field later, if not now.
        //
        apszArgs[c4CurArg++] = L"/OPT:NOREF";
    }

    // Don't generate a manifest
    apszArgs[c4CurArg++] = L"/MANIFEST:NO";

    // And the debug vs. production flags
    if (m_bDebug)
        apszArgs[c4CurArg++] = L"/DEBUG";

    //
    //  If we are to do incremental linking, then turn it on. But only do
    //  so if this is a debug build.
    //
    if (!m_pprojiTarget->bIncrementalLink() && m_bDebug)
        apszArgs[c4CurArg++] = L"/incremental:no";

    //
    //  See if this one needs the underlying standard system libraries. If
    //  so, then add them in as a single parameter. Anything beyond these basic
    //  libraries will be done by pragmas in the system API wrapper classes.
    //
    if (m_pprojiTarget->bUseSysLibs())
    {
        apszArgs[c4CurArg++] = L"kernel32.lib";
        apszArgs[c4CurArg++] = L"user32.lib";
        apszArgs[c4CurArg++] = L"advapi32.lib";
        apszArgs[c4CurArg++] = L"ole32.lib";
        apszArgs[c4CurArg++] = L"oleaut32.lib";
    }

    //
    //  If the 'VARARGS' settings is on, then we have to link the SetArgV.obj
    //  file into the target.
    //
    if (m_pprojiTarget->bVarArgs())
        apszArgs[c4CurArg++] = L"SetArgV.obj";

    // If this project has a resource file, then add it to the list
    TBldStr strResFile;
    if (bHaveResFile)
    {
        strResFile = m_pprojiTarget->strProjectName();
        strResFile.Append(L".res");
        apszArgs[c4CurArg++] = strResFile.pszBuffer();
    }

    //
    //  If this is a based project, then set up the base address of the
    //  project. We space them out in 2MB spots. Its formatted as a hex
    //  value.
    //
    TBldStr strBase(L"/base:0x");
    if (m_pprojiTarget->c4Base())
    {
//        // We start at 258MB and work up in 1MB chunks
//        const tCIDLib::TCard4 c4Chunk = (1024*1024);
//        const tCIDLib::TCard4 c4BaseBase = 0x12000000 + c4Chunk;
//        const tCIDLib::TCard4 c4BaseOfs = c4BaseBase
//                                        + ((m_pprojiTarget->c4Base() - 1) * c4Chunk);
//        strBase.Append(c4BaseOfs, 16);
//        apszArgs[c4CurArg++] = strBase.pszBuffer();
    }

    // Add in all of the library files that are from dependent projects.
    TList<TFindInfo>::TCursor cursLibs(&listLibs());
    if (cursLibs.bResetIter())
    {
        do
        {
            apszArgs[c4CurArg++] = cursLibs.tCurElement().strFileName().pszBuffer();
        }   while (cursLibs.bNext());
    }

    // Add in all of the per-project external library files
    TList<TBldStr>::TCursor cursExtLibs(&m_pprojiTarget->listExtLibs());
    if (cursExtLibs.bResetIter())
    {
        do
        {
            apszArgs[c4CurArg++] = cursExtLibs.tCurElement().pszDupBuffer();
        }   while (cursExtLibs.bNext());
    }

    // And do the same for any system wide libs
    TList<TBldStr>::TCursor cursSysExtLibs(&facCIDBuild.listExtLibs());
    if (cursSysExtLibs.bResetIter())
    {
        do
        {
            apszArgs[c4CurArg++] = cursSysExtLibs.tCurElement().pszDupBuffer();
        }   while (cursSysExtLibs.bNext());
    }

    //  And do system wide lib paths
    TList<TBldStr>::TCursor cursSysExtLibPaths(&facCIDBuild.listExtLibPaths());
    if (cursSysExtLibPaths.bResetIter())
    {
        do
        {
            strTmp = L"/LIBPATH:";
            strTmp.Append(cursSysExtLibPaths.tCurElement());
            apszArgs[c4CurArg++] = strTmp.pszDupBuffer();
        }   while (cursSysExtLibPaths.bNext());
    }

    //
    //  All all of the Obj files that we've already built. Remember this
    //  argument index, in case of verbose logging below.
    //
    const tCIDLib::TCard4 c4ObjStartInd = c4CurArg;
    TList<TDepInfo>::TCursor cursCpps(&listCpps());
    if (cursCpps.bResetIter())
    {
        do
        {
            // Get a ref to the current element
            const TDepInfo& depiCur = cursCpps.tCurElement();

            // And add its object file name to the list of args
            apszArgs[c4CurArg++] = depiCur.strObjFileName().pszBuffer();
        }   while (cursCpps.bNext());
    }

    if (facCIDBuild.bVerbose())
    {
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = 0; c4Ind < c4ObjStartInd; c4Ind++)
            stdOut << apszArgs[c4Ind] << L" ";

        stdOut << L"\n";
        for (; c4Ind < c4CurArg; c4Ind++)
            stdOut << L"   " << apszArgs[c4Ind] << L"\n";
        stdOut << kCIDBuild::EndLn;
    }

    // And invoke the link
    tCIDLib::TCard4 c4Result;
    if (!TUtils::bExec(apszArgs, c4CurArg, c4Result, facCIDBuild.bLowPrio()))
    {
        stdOut << L"Could not execute the linker" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    if (c4Result)
    {
        stdOut << L"Link step failed. Error Code:" << c4Result << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }
}


tCIDLib::TVoid TVCppDriver::LinkStatic()
{
    //
    //  In this case, we want to run the Lib utility on all of the obj files
    //  and output them to the correct output file.
    //
    const tCIDLib::TCh* apszArgs[128];
    tCIDLib::TCard4     c4CurArg = 0;

    // Set up the standard stuff
    apszArgs[c4CurArg++] = L"Lib.Exe";
    apszArgs[c4CurArg++] = L"/nologo";

    TBldStr strOut(L"/Out:");
    strOut.Append(m_strTargetFile);
    apszArgs[c4CurArg++] = strOut.pszBuffer();

    //
    //  All all of the Obj files that we've already built. Remember this
    //  argument index, in case of verbose logging below.
    //
    const tCIDLib::TCard4 c4ObjStartInd = c4CurArg;
    TList<TDepInfo>::TCursor cursCpps(&listCpps());
    if (cursCpps.bResetIter())
    {
        do
        {
            // Get a ref to the current element
            const TDepInfo& depiCur = cursCpps.tCurElement();

            // And add its object file name to the list of args
            apszArgs[c4CurArg++] = depiCur.strObjFileName().pszBuffer();
        }   while (cursCpps.bNext());
    }

    if (facCIDBuild.bVerbose())
    {
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = 0; c4Ind < c4ObjStartInd; c4Ind++)
            stdOut << apszArgs[c4Ind] << L" ";

        stdOut << L"\n";
        for (; c4Ind < c4CurArg; c4Ind++)
            stdOut << L"   " << apszArgs[c4Ind] << L"\n";
        stdOut << kCIDBuild::EndLn;
    }

    // And invoke the link
    tCIDLib::TCard4 c4Result;
    if (!TUtils::bExec(apszArgs, c4CurArg, c4Result, facCIDBuild.bLowPrio()))
    {
        stdOut << L"Could not execute the librarian" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    if (c4Result)
    {
        stdOut << L"Librarian step failed. Error Code:" << c4Result << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }
}

