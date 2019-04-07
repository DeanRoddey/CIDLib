//
// FILE NAME: CIDBuild_ProjectInfo.Cpp
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
//  This file implements the TProjectInfo class, which represents the
//  platform independent settings for a single facility, plus some info
//  that is collected and cached away during processing.
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
//   CLASS: TProjFileCopy
//  PREFIX: pfc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProjFileCopy: Constructors and Destructor
// ---------------------------------------------------------------------------
TProjFileCopy::TProjFileCopy(const TBldStr& strOutPath) :

    m_strOutPath(strOutPath)
{
}

TProjFileCopy::~TProjFileCopy()
{
}


// ---------------------------------------------------------------------------
//  TProjFileCopy: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TProjFileCopy::AddSrcFile(const TBldStr& strToAdd)
{
    m_listSrcFiles.Add(new TBldStr(strToAdd));
}


const TList<TBldStr>& TProjFileCopy::listSrcFiles() const
{
    return m_listSrcFiles;
}


tCIDLib::TVoid TProjFileCopy::RemoveAll()
{
    m_listSrcFiles.RemoveAll();
}


const TBldStr& TProjFileCopy::strOutPath() const
{
    return m_strOutPath;
}

TBldStr& TProjFileCopy::strOutPath(const TBldStr& strToSet)
{
    m_strOutPath = strToSet;
    return m_strOutPath;
}




// ---------------------------------------------------------------------------
//   CLASS: TProjectInfo
//  PREFIX: proj
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProjectInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TProjectInfo::TProjectInfo(const TBldStr& strName) :

    m_bIsSample(kCIDLib::False)
    , m_bMsgFile(kCIDLib::False)
    , m_bNeedsAdminPrivs(kCIDLib::False)
    , m_bPlatformDir(kCIDLib::False)
    , m_bPlatformInclude(kCIDLib::False)
    , m_bResFile(kCIDLib::False)
    , m_bUseSysLibs(kCIDLib::False)
    , m_bVarArgs(kCIDLib::False)
    , m_bVersioned(kCIDLib::False)
    , m_eDisplayType(tCIDBuild::EDisplayTypes::Console)
    , m_eRTLMode(tCIDBuild::ERTLModes::MultiDynamic)
    , m_eType(tCIDBuild::EProjTypes::Executable)
    , m_strProjectName(strName)
    , m_c4Base(0)
    , m_c4DepIndex(0xFFFFFFFF)
{
    //
    //  !NOTE: We cannot initialize a lot of stuff yet because its not known
    //  until we parse out our content from the .Projects file.
    //
}

TProjectInfo::~TProjectInfo()
{
}


// ---------------------------------------------------------------------------
//  TProjectInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TProjectInfo::AddToDepGraph(TDependGraph& depgTarget)
{
    //
    //  Add ourself to the dependency graph object, and save away the
    //  index at which it added us.
    //
    m_c4DepIndex = depgTarget.c4AddNewElement(m_strProjectName);
}


tCIDLib::TBoolean TProjectInfo::bDefineExists(const TBldStr& strToFind) const
{
    TList<TKeyValuePair>::TCursor cursDefs(&m_listDefs);
    if (cursDefs.bResetIter())
    {
        do
        {
            if (cursDefs.tCurElement().strKey() == strToFind)
                return kCIDLib::True;
        }   while (cursDefs.bNext());
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TProjectInfo::bHasIDLFiles() const
{
    return !m_listIDLFiles.bEmpty();
}

tCIDLib::TBoolean TProjectInfo::bHasMsgFile() const
{
    return m_bMsgFile;
}


tCIDLib::TBoolean TProjectInfo::bHasResFile() const
{
    return m_bResFile;
}

tCIDLib::TBoolean TProjectInfo::bIsSample() const
{
    return m_bIsSample;
}


tCIDLib::TBoolean TProjectInfo::bMakeOutDir() const
{
    // If a group type, nothing to do
    if (m_eType == tCIDBuild::EProjTypes::Group)
        return kCIDLib::True;

    // If not exists, try to create it and return that result
    if (!TUtils::bExists(m_strOutDir))
        return TUtils::bMakeDir(m_strOutDir);

    // Else nothing to do so return true
    return kCIDLib::True;
}

tCIDLib::TBoolean TProjectInfo::bNeedsAdminPrivs() const
{
    return m_bNeedsAdminPrivs;
}

tCIDLib::TBoolean TProjectInfo::bPlatformDir() const
{
    return m_bPlatformDir;
}


tCIDLib::TBoolean
TProjectInfo::bSupportsPlatform(const TBldStr& strToCheck) const
{
    //
    //  If m_bPlatformInclude is true, we see if this project is in our list.
    //  Else we see if it not in our list.
    //
    tCIDLib::TBoolean bInList = kCIDLib::False;

    TList<TBldStr>::TCursor cursPlatforms(&m_listPlatforms);
    if (cursPlatforms.bResetIter())
    {
        do
        {
            if (cursPlatforms.tCurElement().bIEquals(strToCheck))
            {
                bInList = kCIDLib::True;
                break;
            }
        }   while (cursPlatforms.bNext());
    }

    //
    //  If the flags are equal, taht means that either in the list and we are in
    //  include mode, or not in the list and we are in ignore mode, so either is
    //  what we are looking for.
    //
    return (bInList == m_bPlatformInclude);
}


tCIDLib::TBoolean TProjectInfo::bUseSysLibs() const
{
    return m_bUseSysLibs;
}


tCIDLib::TBoolean
TProjectInfo::bUsesExtLib(const TBldStr& strToCheck) const
{
    // Search the external libs list for this one.
    TList<TBldStr>::TCursor cursLibs(&m_listExtLibs);
    if (!cursLibs.bResetIter())
        return kCIDLib::True;

    do
    {
        if (cursLibs.tCurElement() == strToCheck)
            return kCIDLib::True;

    }   while (cursLibs.bNext());

    return kCIDLib::False;
}



tCIDLib::TBoolean TProjectInfo::bVarArgs() const
{
    return m_bVarArgs;
}

tCIDLib::TBoolean TProjectInfo::bVersioned() const
{
    return m_bVersioned;
}

tCIDLib::TCard4 TProjectInfo::c4Base() const
{
    return m_c4Base;
}

tCIDLib::TCard4 TProjectInfo::c4CppCount() const
{
    return m_listCpps.c4ElemCount();
}

tCIDLib::TCard4 TProjectInfo::c4DefCount() const
{
    return m_listDefs.c4ElemCount();
}

tCIDLib::TCard4 TProjectInfo::c4DepCount() const
{
    return m_listDeps.c4ElemCount();
}

tCIDLib::TCard4 TProjectInfo::c4DepIndex() const
{
    return m_c4DepIndex;
}

tCIDLib::TCard4 TProjectInfo::c4HppCount() const
{
    return m_listDefs.c4ElemCount();
}


tCIDLib::TVoid TProjectInfo::DumpSettings() const
{
    stdOut  << L"\n----------------------------------------------\n"
            << L"Settings for project: " << m_strProjectName << L"\n"
            << L"----------------------------------------------\n"
            << L"            Name: " << m_strProjectName << L"\n"
            << L"    Project Type: " << m_eType << L"\n"
            << L"       Directory: " << m_strDirectory << L"\n"
            << L"            Base: " << m_c4Base << L"\n"
            << L"    Message File: " << (m_bMsgFile ? L"Yes\n" : L"No\n")
            << L"     Admin Privs: " << (m_bNeedsAdminPrivs ? L"Yes\n" : L"No\n")
            << L"   Resource File: " << (m_bResFile ? L"Yes\n" : L"No\n")
            << L"    Platform Dir: " << (m_bPlatformDir ? L"Yes\n" : L"No\n")
            << L"    Use Sys Libs: " << (m_bUseSysLibs ? L"Yes\n" : L"No\n")
            << L"        Var Args: " << (m_bVarArgs ? L"Yes\n" : L"No\n")
            << L"        RTL Mode: " << m_eRTLMode << L"\n"
            << L"          Sample: " << (m_bIsSample ? L"Yes\n" : L"No\n");


    stdOut << L"       Platforms: ";
    TList<TBldStr>::TCursor cursPlatforms(&m_listPlatforms);
    if (!cursPlatforms.bResetIter())
    {
        stdOut << L"All";
    }
     else
    {
        do
        {
            stdOut << cursPlatforms.tCurElement() << L" ";
        }   while (cursPlatforms.bNext());
    }

    stdOut << kCIDBuild::EndLn;
}


tCIDBuild::EDisplayTypes TProjectInfo::eDisplayType() const
{
    return m_eDisplayType;
}

tCIDBuild::ERTLModes TProjectInfo::eRTLMode() const
{
    return m_eRTLMode;
}

tCIDBuild::EProjTypes TProjectInfo::eType() const
{
    return m_eType;
}

const TList<TFindInfo>& TProjectInfo::listCpps() const
{
    return m_listCpps;
}


const TList<TBldStr>& TProjectInfo::listCustomCmds() const
{
    return m_listCustomCmds;
}


const TList<TBldStr>& TProjectInfo::listDeps() const
{
    return m_listDeps;
}

const TList<TBldStr>& TProjectInfo::listExtLibs() const
{
    return m_listExtLibs;
}

const TList<TProjFileCopy>& TProjectInfo::listFileCopies() const
{
    return m_listFileCopies;
}

const TList<TFindInfo>& TProjectInfo::listHpps() const
{
    return m_listHpps;
}

const TList<TIDLInfo>& TProjectInfo::listIDLFiles() const
{
    return m_listIDLFiles;
}

const TList<TBldStr>& TProjectInfo::listIncludePaths() const
{
    return m_listIncludePaths;
}


tCIDLib::TVoid TProjectInfo::LoadFileLists()
{
    // If a group type, nothing to do
    if (m_eType == tCIDBuild::EProjTypes::Group)
        return;

    // Change to this project's directory
    if (!TUtils::bChangeDir(m_strProjectDir))
    {
        stdOut  << L"Could not change to project directory: "
                << m_strProjectDir << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    // Load up all of the H
    TBldStr strSearch(m_strProjectDir);
    strSearch.Append(kCIDBuild::pszAllHFiles);
    TFindInfo::c4FindFiles(strSearch, m_listHpps);

    // And Hpp files
    strSearch = m_strProjectDir;
    strSearch.Append(kCIDBuild::pszAllHppFiles);
    TFindInfo::c4FindFiles(strSearch, m_listHpps);

    // Load up all of the C files
    strSearch = m_strProjectDir;
    strSearch.Append(kCIDBuild::pszAllCFiles);
    TFindInfo::c4FindFiles(strSearch, m_listCpps);

    // And the Cpp files
    strSearch = m_strProjectDir;
    strSearch.Append(kCIDBuild::pszAllCppFiles);
    TFindInfo::c4FindFiles(strSearch, m_listCpps);

    //
    //  But, if this project has a per-platform directory, then we have to
    //  also do those files, which means we have to append onto the stuff we
    //  already got.
    //
    //  For the Cpp files we tell it to return the relative path component.
    //  Since we give it only "platdir\*.XXX", we will get back just what we
    //  want, which is a path relative to the project directory.
    //
    if (m_bPlatformDir)
    {
        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(L"\\", kCIDBuild::pszAllHFiles);
        TFindInfo::c4FindFiles(strSearch, m_listHpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(L"\\", kCIDBuild::pszAllHppFiles);
        TFindInfo::c4FindFiles(strSearch, m_listHpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(L"\\", kCIDBuild::pszAllCFiles);
        TFindInfo::c4FindFiles(strSearch, m_listCpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(L"\\", kCIDBuild::pszAllCppFiles);
        TFindInfo::c4FindFiles(strSearch, m_listCpps, tCIDBuild::EPathModes::Relative);
    }
}


// Used by per-platform tools drivers to check for
const TKeyValuePair*
TProjectInfo::pkvpFindPlatOpt(const TBldStr& strPlatform, const TBldStr& strOption) const
{
    //
    //  Loop through the list of lists. The key of the first entry in each one is
    //  platform name.
    //
    TPlatOptList::TCursor cursPlatOpts(&m_listPlatOpts);
    if (!cursPlatOpts.bResetIter())
        return nullptr;

    do
    {
        TKVPList::TCursor cursPlat(&cursPlatOpts.tCurElement());
        if (cursPlat.bResetIter())
        {
            if (cursPlat->strKey().bIEquals(strPlatform))
            {
                //
                // This is the right platform, so check it's entries, move past the first one
                //  before we start.
                //
                if (!cursPlat.bNext())
                    break;

                do
                {
                    if (cursPlat->strKey().bIEquals(strOption))
                        return &cursPlat.tCurElement();
                }   while (cursPlat.bNext());
            }
        }
    }   while (cursPlatOpts.bNext());

    return nullptr;
}


// This is called to let us parse our contents out of the project definition file
tCIDLib::TVoid TProjectInfo::ParseContent(TLineSpooler& lsplSource)
{
    //
    //  Ok, lets go into a line reading loop and pull out or information
    //  we find the major sections here and then pass them off to private
    //  methods to deal with the details.
    //
    tCIDLib::TBoolean bSeenPlats = kCIDLib::False;
    tCIDLib::TBoolean bDone = kCIDLib::False;
    TBldStr             strReadBuf;
    while (!bDone)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected SETTINGS, DEPENDENTS, DEFINES, or END PROJECT"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"SETTINGS")
        {
            ParseSettings(lsplSource);
        }
         else if (strReadBuf == L"DEPENDENTS")
        {
            ParseDependents(lsplSource);
        }
         else if (strReadBuf == L"DEFINES")
        {
            ParseDefines(lsplSource);
        }
         else if (strReadBuf == L"EXCLUDEPLATS")
        {
            if (bSeenPlats)
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Only one of include/exclude platforms can be used"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::UnexpectedEOF;
            }

            bSeenPlats = kCIDLib::True;
            m_bPlatformInclude = kCIDLib::False;
            ParsePlatforms(lsplSource);
        }
         else if (strReadBuf == L"EXTLIBS")
        {
            ParseExtLibs(lsplSource);
        }
         else if (strReadBuf == L"IDLFILES")
        {
            ParseIDLFiles(lsplSource);
        }
         else if (strReadBuf == L"INCLUDEPATHS")
        {
            ParseIncludePaths(lsplSource);
        }
         else if (strReadBuf == L"INCLUDEPLATS")
        {
            if (bSeenPlats)
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Only one of include/exclude platforms can be used"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::UnexpectedEOF;
            }

            m_bPlatformInclude = kCIDLib::True;
            ParsePlatforms(lsplSource);
        }
         else if (strReadBuf == L"END PROJECT")
        {
            bDone = kCIDLib::True;
        }
         else if (strReadBuf.bStartsWith(L"CUSTOMCMDS"))
        {
            ParseCustCmds(lsplSource);
        }
         else if (strReadBuf.bStartsWith(L"FILECOPIES"))
        {
            //
            //  It's a file copy block. Is can be followed by the target
            //  path for this block of copies.
            //
            strReadBuf.Cut(10);
            strReadBuf.StripWhitespace();
            if (strReadBuf[0] == L'=')
            {
                strReadBuf.Cut(1);
                strReadBuf.StripWhitespace();
            }
            ParseFileCopies(lsplSource, strReadBuf);
        }
         else if (strReadBuf == L"PLATFORMOPTS")
        {
            ParsePlatOpts(lsplSource);
        }
         else
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected SETTINGS, DEPENDENTS, EXTLIBS, DEFINES, or END PROJECT"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }

    // Only do this stuff if not a group
    if (m_eType != tCIDBuild::EProjTypes::Group)
    {
        // Build up a path the project's output
        m_strOutDir = facCIDBuild.strOutDir();
        m_strOutDir.Append(m_strProjectName);
        m_strOutDir.Append(L".Out");
        m_strOutDir.Append(L"\\");

        // Build the name to the error ids header, which goes to the include dir
        m_strOutErrHpp = m_strProjectDir; // facCIDBuild.strIncludeDir();
        m_strOutErrHpp.Append(m_strProjectName);
        m_strOutErrHpp.Append(L"_ErrorIds.hpp");

        // Build the name to the message ids header, which goes to the include dir
        m_strOutMsgHpp = m_strProjectDir; // facCIDBuild.strIncludeDir();
        m_strOutMsgHpp.Append(m_strProjectName);
        m_strOutMsgHpp.Append(L"_MessageIds.hpp");

        // Bulld the name to the message text source, if any
        m_strMsgSrc = m_strProjectDir;
        m_strMsgSrc.Append(m_strProjectName);
        m_strMsgSrc.Append(L"_");
        m_strMsgSrc.Append(facCIDBuild.strLangSuffix());
        m_strMsgSrc.Append(L".MsgText");

        // Bulld the name to the GUI resource source, if any
        m_strResSrc = m_strProjectDir;
        m_strResSrc.Append(m_strProjectName);
        m_strResSrc.Append(L".CIDRC");

        //
        //  Build up the output binary message file name. If this project is
        //  versioned, then append the version postfix. And its language specific
        //  so add the language suffix.
        //
        m_strOutMsgs = facCIDBuild.strOutDir();
        m_strOutMsgs.Append(m_strProjectName);
        if (m_bVersioned)
            m_strOutMsgs.Append(facCIDBuild.strVersionSuffix());
        m_strOutMsgs.Append(L"_");
        m_strOutMsgs.Append(facCIDBuild.strLangSuffix());
        m_strOutMsgs.Append(L".CIDMsg");

        //
        //  And do the same for the resource file. Its version specific, but not
        //  language specific.
        //
        m_strOutRes = facCIDBuild.strOutDir();
        m_strOutRes.Append(m_strProjectName);
        if (m_bVersioned)
            m_strOutRes.Append(facCIDBuild.strVersionSuffix());
        m_strOutRes.Append(L".CIDRes");

        // And the resource header file
        m_strOutResHpp = m_strProjectDir; // facCIDBuild.strIncludeDir();
        m_strOutResHpp.Append(m_strProjectName);
        m_strOutResHpp.Append(L"_ResourceIds.hpp");

        //
        //  Allow the per-platform code to build up the paths to the primary
        //  output file.
        //
        BuildOutputFileName();
    }
}


const TBldStr& TProjectInfo::strDirectory() const
{
    return m_strDirectory;
}

const TBldStr& TProjectInfo::strExportKeyword() const
{
    return m_strExportKeyword;
}

const TBldStr& TProjectInfo::strMsgSrc() const
{
    return m_strMsgSrc;
}

const TBldStr& TProjectInfo::strResSrc() const
{
    return m_strResSrc;
}

const TBldStr& TProjectInfo::strOutDir() const
{
    return m_strOutDir;
}

const TBldStr& TProjectInfo::strOutErrHpp() const
{
    return m_strOutErrHpp;
}

const TBldStr& TProjectInfo::strOutMsgHpp() const
{
    return m_strOutMsgHpp;
}

const TBldStr& TProjectInfo::strOutMsgs() const
{
    return m_strOutMsgs;
}

const TBldStr& TProjectInfo::strOutBin() const
{
    return m_strOutBin;
}

const TBldStr& TProjectInfo::strOutRes() const
{
    return m_strOutRes;
}

const TBldStr& TProjectInfo::strOutResHpp() const
{
    return m_strOutResHpp;
}

const TBldStr& TProjectInfo::strProjectName() const
{
    return m_strProjectName;
}

const TBldStr& TProjectInfo::strProjectDir() const
{
    return m_strProjectDir;
}



// ---------------------------------------------------------------------------
//  TProjectInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TProjectInfo::bSetSetting(const TBldStr& strName, const TBldStr& strValue)
{
    if (strName == L"BASE")
    {
        //
        //  This is a number so it must convert correctly. Its just a simple
        //  decimal value.
        //
        if (!TRawStr::bXlatCard4(strValue.pszBuffer(), m_c4Base, 10))
            return kCIDLib::False;
    }
     else if (strName == L"DIRECTORY")
    {
        // Store the raw directory
        m_strDirectory = strValue;

        // Build up the path to the directory and store it.
        m_strProjectDir = facCIDBuild.strRootDir();
        m_strProjectDir.Append(L"Source", L"\\");
        m_strProjectDir.Append(L"AllProjects", L"\\");

        // If it's not empty (i.e. top level), then add it, and end with a slash
        if (!m_strDirectory.bEmpty())
        {
            m_strProjectDir.Append(m_strDirectory);
            m_strProjectDir.Append(L"\\");
        }
    }
     else if (strName == L"DISPLAY")
    {
        if (strValue == L"N/A")
            m_eDisplayType = tCIDBuild::EDisplayTypes::NotApplicable;
        else if (strValue == L"Console")
            m_eDisplayType = tCIDBuild::EDisplayTypes::Console;
        else if (strValue == L"GUI")
            m_eDisplayType = tCIDBuild::EDisplayTypes::GUI;
        else
            return kCIDLib::False;
    }
     else if (strName == L"EXPORT")
    {
        m_strExportKeyword = strValue;
    }
     else if (strName == L"MSGFILE")
    {
        if (strValue == L"No")
        {
            m_bMsgFile = kCIDLib::False;
        }
         else if (strValue == L"Yes")
        {
            m_bMsgFile = kCIDLib::True;
        }
         else
        {
            // It's not valid
            return kCIDLib::False;
        }
    }
     else if (strName == L"RESFILE")
    {
        if (strValue == L"No")
        {
            m_bResFile = kCIDLib::False;
        }
         else if (strValue == L"Yes")
        {
            m_bResFile = kCIDLib::True;
        }
         else
        {
            // It's not valid
            return kCIDLib::False;
        }
    }
     else if (strName == L"ADMINPRIVS")
    {
        if (!TRawStr::bXlatBoolean(strValue.pszBuffer(), m_bNeedsAdminPrivs))
            return kCIDLib::False;
    }
     else if (strName == L"PLATFORMDIR")
    {
        if (!TRawStr::bXlatBoolean(strValue.pszBuffer(), m_bPlatformDir))
            return kCIDLib::False;
    }
     else if (strName == L"RTL")
    {
        if (strValue == L"Single/Static")
            m_eRTLMode = tCIDBuild::ERTLModes::SingleStatic;
        else if (strValue == L"Single/Dynamic")
            m_eRTLMode = tCIDBuild::ERTLModes::SingleStatic;
        else if (strValue == L"Multi/Static")
            m_eRTLMode = tCIDBuild::ERTLModes::MultiStatic;
        else if (strValue == L"Multi/Dynamic")
            m_eRTLMode = tCIDBuild::ERTLModes::MultiDynamic;
        else
            return kCIDLib::False;
    }
     else if (strName == L"TYPE")
    {
        if (strValue == L"SharedObj")
            m_eType = tCIDBuild::EProjTypes::SharedObj;
        else if (strValue == L"SharedLib")
            m_eType = tCIDBuild::EProjTypes::SharedLib;
        else if (strValue == L"StaticLib")
            m_eType = tCIDBuild::EProjTypes::StaticLib;
        else if (strValue == L"Executable")
            m_eType = tCIDBuild::EProjTypes::Executable;
        else if (strValue == L"Service")
            m_eType = tCIDBuild::EProjTypes::Service;
        else if (strValue == L"FileCopy")
            m_eType = tCIDBuild::EProjTypes::FileCopy;
        else if (strValue == L"Group")
            m_eType = tCIDBuild::EProjTypes::Group;
        else
            return kCIDLib::False;
    }
     else if (strName == L"VARARGS")
    {
        if (!TUtils::bXlatBool(strValue, m_bVarArgs))
            return kCIDLib::False;
    }
     else if (strName == L"VERSIONED")
    {
        if (!TUtils::bXlatBool(strValue, m_bVersioned))
            return kCIDLib::False;
    }
     else if (strName == L"USESYSLIBS")
    {
        if (!TUtils::bXlatBool(strValue, m_bUseSysLibs))
            return kCIDLib::False;
    }
     else if (strName == L"SAMPLE")
    {
        if (!TRawStr::bXlatBoolean(strValue.pszBuffer(), m_bIsSample))
            return kCIDLib::False;
    }
     else
    {
        // Don't know what this setting is
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Called to parse the contents of the custom commands block
tCIDLib::TVoid TProjectInfo::ParseCustCmds(TLineSpooler& lsplSource)
{
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected 'custom command line' or END CUSTOMCMDS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END CUSTOMCMDS")
            break;

        // Just add it to our custom commands list
        m_listCustomCmds.Add(new TBldStr(strReadBuf));
    }
}


tCIDLib::TVoid TProjectInfo::ParseDefines(TLineSpooler& lsplSource)
{
    TBldStr strName;
    TBldStr strReadBuf;
    TBldStr strValue;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected 'name=value' or END DEFINES"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END DEFINES")
            break;

        // Have to assume its a define so pull it out
        if (!TUtils::bFindNVParts(strReadBuf, strName, strValue))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Badly formed DEFINE statement" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Add a new define, if it does not already exist
        if (bDefineExists(strName))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Define '" << strName
                    << L"' already exists" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::AlreadyExists;
        }

        // Ok we can add it
        m_listDefs.Add(new TKeyValuePair(strName, strValue));
    }
}


tCIDLib::TVoid TProjectInfo::ParseDependents(TLineSpooler& lsplSource)
{
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected 'dependent name' or END DEPENDENTS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END DEPENDENTS")
            break;

        // Have to assume its the name of a dependent
        m_listDeps.Add(new TBldStr(strReadBuf));
    }
}


tCIDLib::TVoid TProjectInfo::ParseExtLibs(TLineSpooler& lsplSource)
{
    // Only valid for code type projects
    if (m_eType > tCIDBuild::EProjTypes::MaxCodeType)
    {
        stdOut  << L"(Line " << lsplSource.c4CurLine()
                << L") Not valid for this type of project"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }

    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected include path or END EXTLIBS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END EXTLIBS")
            break;

        //
        //  Have to assume its a lib path. It can start with the project dir
        //  macro, which we have to expand, else we take it as is.
        //
        if (strReadBuf.bStartsWith(L"$(ProjDir)\\"))
        {
            TBldStr strTmp(m_strProjectDir);
            strTmp.AppendAt(strReadBuf, 11);
            m_listExtLibs.Add(new TBldStr(strTmp));
        }
         else
        {
            m_listExtLibs.Add(new TBldStr(strReadBuf));
        }
    }
}


tCIDLib::TVoid
TProjectInfo::ParseFileCopies(TLineSpooler& lsplSource, const TBldStr& strTarPath)
{
    // Add a new file copy object to the list, setting the passed target path
    TProjFileCopy* pfcNew = new TProjFileCopy(strTarPath);
    m_listFileCopies.Add(pfcNew);

    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected include path or END FILECOPIES"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END FILECOPIES")
            break;

        // Have to assume its a file name to copy
        pfcNew->AddSrcFile(strReadBuf);
    }

    //
    //  If there's one entry and it is in the form *, then we get the source
    //  directory and add all the files in that directory to the list.
    //
    if (pfcNew->listSrcFiles().c4ElemCount() == 1)
    {
        TListCursor<TBldStr> cursOrgList(&pfcNew->listSrcFiles());
        if (cursOrgList.bResetIter())
        {
            if (cursOrgList.tCurElement() == L"*")
            {
                strReadBuf = strProjectDir();
                strReadBuf.Append(L"\\*");

                pfcNew->RemoveAll();

                TList<TFindInfo> listNewList;
                TFindInfo::c4FindFiles(strReadBuf, listNewList);
                TListCursor<TFindInfo> cursNewList(&listNewList);
                if (cursNewList.bResetIter())
                {
                    do
                    {
                        if (!cursNewList.tCurElement().bIsDirectory())
                            pfcNew->AddSrcFile(cursNewList.tCurElement().strFileName());
                    }   while (cursNewList.bNext());
                }
            }
        }
    }
}


tCIDLib::TVoid TProjectInfo::ParseIDLFiles(TLineSpooler& lsplSource)
{
    // Only valid for code type projects
    if (m_eType > tCIDBuild::EProjTypes::MaxCodeType)
    {
        stdOut  << L"(Line " << lsplSource.c4CurLine()
                << L") Not valid for a 'filecopy' project"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }

    TIDLInfo    idliTmp;
    TBldStr     strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected IDLFILE or END IDLFILES"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END IDLFILES")
            break;

        // It must be IDLFILE
        if (strReadBuf != L"IDLFILE")
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected IDLFILE" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        //
        //  Ok, parse out the fields for this entry. It just has a few
        //  fields, the source IDL file path, the gen mode, and an optional
        //  name extension. We just create a new TIDLInfo object and let
        //  it parse itself out.
        //
        idliTmp.Parse(lsplSource);

        // It worked, so add it to the list
        m_listIDLFiles.Add(new TIDLInfo(idliTmp));
    }
}


tCIDLib::TVoid TProjectInfo::ParseIncludePaths(TLineSpooler& lsplSource)
{
    // Only valid for code type projects
    if (m_eType > tCIDBuild::EProjTypes::MaxCodeType)
    {
        stdOut  << L"(Line " << lsplSource.c4CurLine()
                << L") Not valid for a 'filecopy' project"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }

    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected include path or END INCLUDEPATHS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END INCLUDEPATHS")
            break;

        //
        //  Have to assume its an include path. If it isn't fully qualified,
        //  then assume it is relative to the project source path. If if
        //  starts with $(ProjDir), replace that with the project directory.
        //
        if (TUtils::bIsFQPath(strReadBuf))
        {
            m_listIncludePaths.Add(new TBldStr(strReadBuf));
        }
         else if (strReadBuf.bStartsWith(L"$(ProjDir)\\"))
        {
            TBldStr strTmp(m_strProjectDir);
            strTmp.AppendAt(strReadBuf, 11);
            m_listIncludePaths.Add(new TBldStr(strTmp));
        }
         else
        {
            TBldStr strTmp(m_strProjectDir);
            strTmp.Append(strReadBuf);
            m_listIncludePaths.Add(new TBldStr(strTmp));
        }
    }
}


tCIDLib::TVoid TProjectInfo::ParsePlatforms(TLineSpooler& lsplSource)
{
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected 'platform name' or end of platforms"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (((strReadBuf == L"END INCLUDEPLATS") && m_bPlatformInclude)
        ||  ((strReadBuf == L"END EXPLUDEPLATS") && !m_bPlatformInclude))
        {
            break;
        }

        // Have to assume its the name of a support platform
        m_listPlatforms.Add(new TBldStr(strReadBuf));
    }
}


tCIDLib::TVoid TProjectInfo::ParsePlatOpts(TLineSpooler& lsplSource)
{
    TBldStr strOptName;
    TBldStr strOptValue;
    TBldStr strPlatName;
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected END PLATFORMOPTS or PLATFORM="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END PLATFORMOPTS")
        {
            break;
        }

        // It should start with PLATFORM= and the platform name
        if (!strReadBuf.bStartsWith(L"PLATFORM="))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected END PLATFORMOPTS or PLATFORM="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Next should be the platform name so get that out
        strPlatName = strReadBuf;
        strPlatName.Cut(9);

        //
        //  Add a new list for this platform and a first entry with the key being
        //  the platform name.
        //
        TKVPList* plistNew = new TKVPList();
        plistNew->Add(new TKeyValuePair(strPlatName, L""));
        m_listPlatOpts.Add(plistNew);

        //
        //  Now we loop till we see the end of this block, pulling out lines
        //  each of which is either a value or a key=value pair.
        //
        while (kCIDLib::True)
        {
            // Get the next line. If end of file, that's an error here
            if (!lsplSource.bReadLine(strReadBuf))
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Expected END PLATFORM or a platform option="
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::UnexpectedEOF;
            }

            if (strReadBuf == L"END PLATFORM")
                break;

            // It has to be in our standard key=value format
            if (!TUtils::bFindNVParts(strReadBuf, strOptName, strOptValue))
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Badly formed platform option statement" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }

            // Add a new pair for this guy
            plistNew->Add(new TKeyValuePair(strOptName, strOptValue));
        }
    }
}


tCIDLib::TVoid TProjectInfo::ParseSettings(TLineSpooler& lsplSource)
{
    TBldStr strName;
    TBldStr strReadBuf;
    TBldStr strValue;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected 'setting=value' or END SETTINGS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END SETTINGS")
            break;

        //
        //  Have to assume its a setting value, so lets see if we can
        //  recognize it. We use a utility function that will find the
        //  name/value parts of a string divided by an '=' sign.
        //
        if (!TUtils::bFindNVParts(strReadBuf, strName, strValue))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Badly formed SETTING statement" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Find out which one it is and check the value
        if (!bSetSetting(strName, strValue))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Unknown SETTING name or bad value. Name="
                    << strName << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }

    //
    //  Insure that we got any settings that must have been provided. If not, that's
    //  an error. The directory can be empty if it's a group
    //
    if (m_strProjectName.bEmpty())
    {
        stdOut  << L"Project settings at line " << lsplSource.c4CurLine()
                << L" failed to define a project name"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    if (m_strDirectory.bEmpty() && (m_eType != tCIDBuild::EProjTypes::Group))
    {
        stdOut  << L"Project settings at line " << lsplSource.c4CurLine()
                << L" failed to define a project name"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
}
