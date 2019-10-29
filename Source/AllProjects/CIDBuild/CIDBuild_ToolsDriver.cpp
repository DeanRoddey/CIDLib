//
// FILE NAME: CIDBuild_ToolsDriver.Cpp
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
//  This file implements the TToolsDriver class.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because the outside world does not need to know about them, we include
//      directly here all of the headers for the concrete implementations of
//      the tools driver. Since the lazy eval factory method is here anyway,
//      that works out nicely.
//
//      This obvious means that there is a little conditional code here.
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
//  Include the required tools driver derivative for the target platform, by
//  using the token defined by each compiler.
// ---------------------------------------------------------------------------
#if defined(_MSC_VER)
#include    "CIDBuild_VCppDriver.hpp"
#elif defined(__KCC)
#include    "CIDBuild_KCCDriver.hpp"
#elif defined(__GNUC__)
#include    "CIDBuild_GCCDriver.hpp"
#else
#error("Unknown Tool Driver Error")
#endif



// ---------------------------------------------------------------------------
//   CLASS: TDepInfo
//  PREFIX: depi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDepInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TDepInfo::TDepInfo( const   TBldStr&        strFileName
                    , const TProjectInfo&   projiParent) :

    m_listHpps(tCIDLib::EAdoptOpts::NoAdopt)
{
    if (!TFindInfo::bFindAFile(strFileName, m_fndiCpp, tCIDBuild::EPathModes::Relative))
    {
        stdOut  << L"Could not find project Cpp file: "
                << strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  Build up the object file name. This is relative to the main output
    //  directory. We start with the output directory of the project. Then
    //  we get the base part of the file name, Then we tack on the object
    //  file extension for the platform.
    //
    m_strObjFile = projiParent.strOutDir();

    TBldStr strTmp(strFileName);
    tCIDLib::TCard4 c4Name, c4Ext;
    TUtils::FindPathParts(strTmp, c4Name, c4Ext);
    if (c4Ext != kCIDBuild::c4NotFound)
        strTmp.CapAt(c4Ext);
    if (c4Name != kCIDBuild::c4NotFound)
        strTmp.Cut(c4Name);
    m_strObjFile.Append(strTmp);
    m_strObjFile.Append(kCIDBuild::pszObjExt);
}

TDepInfo::~TDepInfo()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TToolsDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TToolsDriver: Public, static methods
// ---------------------------------------------------------------------------
TToolsDriver& TToolsDriver::tdrvBuilder()
{
    static TToolsDriver* ptdrvBuilder = 0;

    if (!ptdrvBuilder)
    {
        //
        //  The tokens used here for conditional compilation are those that
        //  are turned on by each environment's compiler, which will be used
        //  to compile this program first. So that insures that the right
        //  driver will be selected for the target platform.
        //
        #if defined(_MSC_VER)
        ptdrvBuilder = new TVCppDriver;
        #elif defined(__GNUC__)
        ptdrvBuilder = new TGCCDriver;
        #else
        !Unknown Tool Driver Error!
        #endif

        // Just in case, make sure it was created
        if (!ptdrvBuilder)
        {
            stdOut << L"The tools driver did not get created" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }
    }
    return *ptdrvBuilder;
}



// ---------------------------------------------------------------------------
//  TToolsDriver: Public Constructors and Destructor
// ---------------------------------------------------------------------------
TToolsDriver::~TToolsDriver()
{
}


// ---------------------------------------------------------------------------
//  TToolsDriver: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TToolsDriver::bBuildProject(const TProjectInfo& projiToBuild)
{
    // Store a temp pointer so our callbacks can see the project
    m_pprojiTar = &projiToBuild;

    //
    //  Flush any content from the list of libs and source files. We don't
    //  flush the header list since it is allowed to just build up any
    //  headers we've seen so far.
    //
    m_listCpps.RemoveAll();
    m_listLibs.RemoveAll();

    //
    //  Try to change to the target directory. We need to change here first
    //  while we build the list of libs and make sure that they exist. This
    //  is because the target will do the link step from the result directory
    //  and will want them to be just relative names.
    //
    if (!TUtils::bChangeDir(facCIDBuild.strOutDir()))
    {
        stdOut  << L"Could not change to output directory: "
                << facCIDBuild.strOutDir() << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  Build the Lib list. We use the standard dependency graph iteration
    //  mechanism to do it. So it calls us back on a method which will
    //  add entries to the libs list.
    //
    TMethProjIterator<TToolsDriver> piterLibs(this, &TToolsDriver::bLibListCallBack);
    if (!facCIDBuild.plistProjects().bIterate(piterLibs
                                              , projiToBuild.strProjectName()
                                              , tCIDBuild::EIterModes::FromBottomMinSkip))
    {
        stdOut  << L"Project dependency iteration failed building lib list"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    //
    //  Now change to the project directory. Use the full project directory
    //  here, not the relative project subdirectory name. This gets us ready
    //  to do the rest of the prep work, and we will stay here when we invoke
    //  the derived class' build method.
    //
    if (!TUtils::bChangeDir(projiToBuild.strProjectDir()))
    {
        stdOut  << L"Could not change to project directory: "
                << projiToBuild.strProjectDir() << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  Parse the .Depends file for this project. This will build back up
    //  the Cpps list and any new headers to the header list that we have
    //  not seen yet.
    //
    ParseDepFile(projiToBuild);

    //
    //  Call the derived class' building method, which will do all of the
    //  actual work, though it will call back into here to get help.
    //
    tCIDLib::TBoolean bRet = bBuild(projiToBuild);

    //
    //  Flush the Cpp and Lib lists, but not the Hpp list, which we just
    //  let build up through all of the projects since they won't change
    //  for a single run of the program, and it will save us bunchas of
    //  time.
    //
    m_listCpps.RemoveAll();
    m_listLibs.RemoveAll();

    // Clear the temp pointer now
    m_pprojiTar = nullptr;

    return kCIDLib::True;
}


tCIDLib::TBoolean TToolsDriver::bDebugProject(const TProjectInfo& projiTar)
{
    // It has to be an exe project
    if  (projiTar.eType() != tCIDBuild::EProjTypes::Executable)
    {
        stdOut  << L"project " << projiTar.strProjectName()
                << L" is not an executable project" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }

    // Try to change to the project directory. We want them all to run from there
    if (!TUtils::bChangeDir(projiTar.strProjectDir()))
    {
        stdOut  << L"Could not change to project directory: "
                << projiTar.strProjectDir() << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    // And let's call the virtual method
    return bInvokeDebugger(projiTar);
}


tCIDLib::TVoid TToolsDriver::DebugReset(const TProjectInfo& projiTar)
{
    // It has to be an exe project
    if  (projiTar.eType() != tCIDBuild::EProjTypes::Executable)
    {
        stdOut  << L"project " << projiTar.strProjectName()
                << L" is not an executable project" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }
    ResetDebugInfo(projiTar);
}


// ---------------------------------------------------------------------------
//  TToolsDriver: Hidden Constructors
// ---------------------------------------------------------------------------
TToolsDriver::TToolsDriver()
{
}


// ---------------------------------------------------------------------------
//  TToolsDriver: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TToolsDriver::bMustCompile( const   TProjectInfo&   projiTarget
                            , const TDepInfo&       depiToCheck)
{
    // If the Force flag is on, then just say it must be compiled
    if (facCIDBuild.bForce())
        return kCIDLib::True;

    //
    //  First of all, check the obvious one, which is that the Cpp file
    //  is later than the object file.
    //
    tCIDLib::TBoolean   bUpdate = kCIDLib::False;
    TFindInfo           fndiObj;
    if (!TFindInfo::bFindAFile(depiToCheck.strObjFileName(), fndiObj))
    {
        bUpdate = kCIDLib::True;
        if (facCIDBuild.bVerbose())
        {
            stdOut  << L"    Target does not exist: "
                    << depiToCheck.strObjFileName()
                    << kCIDBuild::EndLn;
        }
    }
     else
    {
        bUpdate = depiToCheck.fndiThis() > fndiObj;
        if (facCIDBuild.bVerbose() && bUpdate)
        {
            stdOut  << L"    Source is newer: "
                    << depiToCheck.strFileName()
                    << kCIDBuild::EndLn;
        }
    }

    // If we need to update, then we can return now
    if (bUpdate)
        return kCIDLib::True;

    //
    //  Oh well, we gotta do the long version. We have to compare against
    //  all of the headers that this Cpp file depends on.
    //
    //  NOTE: We compare the headers against the object file, not the Cpp
    //  file!
    //
    TList<TFindInfo>::TCursor cursHpps(&depiToCheck.listHpps());
    if (cursHpps.bResetIter())
    {
        do
        {
            if (cursHpps.tCurElement() > fndiObj)
            {
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Header is newer: "
                            << cursHpps.tCurElement().strFileName() << kCIDBuild::EndLn;
                }
                return kCIDLib::True;
            }
        }   while (cursHpps.bNext());
    }

    // Don't need to compile this guy
    return kCIDLib::False;
}


TDepInfo* TToolsDriver::pdepiFindCpp(const TBldStr&  strCppName)
{
    TList<TDepInfo>::TCursor cursCpps(&m_listCpps);
    if (cursCpps.bResetIter())
    {
        do
        {
            if (cursCpps.tCurElement().strFileName() == strCppName)
                return &cursCpps.tCurElement();
        }   while (cursCpps.bNext());
    }

    // Its not there
    return 0;
}



// ---------------------------------------------------------------------------
//  TToolsDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TToolsDriver::AddToCppsList(TDepInfo& depiTarget, const TBldStr& strHeader)
{
    //
    //  Try to find this header in the list first. If its there, then we
    //  just add its entry to the dependent list of the target. Otherwise
    //  we add a new entry and add that.
    //
    TList<TFindInfo>::TCursor cursHpps(&m_listHpps);
    if (cursHpps.bResetIter())
    {
        do
        {
            if (cursHpps.tCurElement().strFileName() == strHeader)
            {
                //
                //  Note that it does not adopt in this case, just
                //  references.
                //
                depiTarget.AddHppDependent(&cursHpps.tCurElement());
                return;
            }
        }   while (cursHpps.bNext());
    }

    // We have to add a new one
    TFindInfo* pfndiNew = new TFindInfo;
    if (!TFindInfo::bFindAFile(strHeader, *pfndiNew, tCIDBuild::EPathModes::Relative))
    {
        delete pfndiNew;
        stdOut << L"Could not find dependent header: " << strHeader << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    // Put it into the Hpp list
    m_listHpps.Add(pfndiNew);

    //
    //  And put a reference into the dependent list of the Cpp. Note that it
    //  does not adopt in this case, just references.
    //
    depiTarget.AddHppDependent(pfndiNew);
}


tCIDLib::TBoolean
TToolsDriver::bLibListCallBack( const  TBldStr&            strTarget
                                , const tCIDLib::TCard4    c4Level)
{
    //
    //  If the level is 1, then its the actual target project, so we want
    //  to skip that one and just do the underlying libraries that it
    //  depends on.
    //
    if (c4Level == 1)
        return kCIDLib::True;

    //
    //  See what kind of library this is. Not all dependent projects are
    //  code projects. Some can be file copies. We only want to add an item
    //  here for code type projects.
    //
    const TProjectInfo& projiTar = facCIDBuild.projiByName(strTarget);
    if (projiTar.eType() <= tCIDBuild::EProjTypes::MaxCodeType)
    {
        // Let the platform driver build the library name for us
        TBldStr strNew;
        BuildLibName(strTarget, strNew);

        TFindInfo* pfndiNew = new TFindInfo;
        if (!TFindInfo::bFindAFile(strNew, *pfndiNew))
        {
            delete pfndiNew;
            stdOut << L"Could not find dependent library: " << strNew << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        // Add this guy to the list
        m_listLibs.Add(pfndiNew);
    }
    return kCIDLib::True;
}


TDepInfo& TToolsDriver::depiAddCpp( const   TBldStr&        strCppName
                                    , const TProjectInfo&   projiTarget)
{
    // Just to make sure, check that its not already present
    if (pdepiFindCpp(strCppName))
    {
        stdOut  << L"File: " << strCppName
                << L" is already in the project list" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::AlreadyExists;
    }

    TDepInfo* pdepiNew = new TDepInfo(strCppName, projiTarget);
    m_listCpps.Add(pdepiNew);

    // And return the new entry
    return *pdepiNew;
}


tCIDLib::TVoid TToolsDriver::ParseDepFile(const TProjectInfo& projiTarget)
{
    //
    //  And build up a path to the dependency file for the project. Its
    //  in the temp output directory for this project.
    //
    TBldStr strDepFile(projiTarget.strOutDir());
    strDepFile.Append(projiTarget.strProjectName());
    strDepFile.Append(L".Depend");

    // Create a line spooler, and tell to do whitespace stripping for us
    TLineSpooler lsplDepends(strDepFile, kCIDLib::True);

    //
    //  And now lets loop through the file and check for each Cpp file's
    //  section of the file. All we expect to see at this level are those
    //  opening lines of each Cpp file.
    //
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If at the end of file, then we are done
        if (!lsplDepends.bReadLine(strReadBuf))
            break;

        //
        //  This should be the FILE= opening line of a Cpp's list of
        //  files.
        //
        if (strReadBuf.bStartsWith(L"FILE="))
        {
            //
            //  Get the Cpp file's name out of it and add it to the list
            //  of Cpp files.
            //
            TBldStr strCppName;
            strCppName.CopyAt(strReadBuf, 5);
            TDepInfo& depiCur = depiAddCpp(strCppName, projiTarget);

            //
            //  And now parse out the headers that this Cpp file depends
            //  on. It will add any headers it finds to the header list,
            //  if not already there, and then will update the entries in
            //  our current Cpp entry for all the dependent headers.
            //
            ParseDependents(depiCur, lsplDepends);
        }
         else
        {
            // Dunno what this is
            stdOut  << L"(Line " << lsplDepends.c4CurLine()
                    << L") Expected a comment or FILE=" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}


tCIDLib::TVoid
TToolsDriver::ParseDependents(TDepInfo& depiCpp, TLineSpooler& lsplSrc)
{
    //
    //  We read in dependent Hpp files until we hit the END FILE marker,
    //  which is the end of this file's dependents.
    //
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If at the end of file, that is an error
        if (!lsplSrc.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSrc.c4CurLine()
                    << L") Expected comment, dependent Hpp path, or END FILE"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END FILE")
            break;

        AddToCppsList(depiCpp, strReadBuf);
    }
}

