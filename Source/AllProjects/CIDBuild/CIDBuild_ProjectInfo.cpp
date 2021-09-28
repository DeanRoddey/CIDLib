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


const tCIDBuild::TStrList& TProjFileCopy::listSrcFiles() const
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
TProjectInfo::TProjectInfo( const   TBldStr&                strName
                            , const tCIDBuild::TStrList&    listPlatIncl
                            , const tCIDBuild::TStrList&    listPlatExcl) :

    m_bIsSample(kCIDLib::False)
    , m_bMsgFile(kCIDLib::False)
    , m_bNeedsAdminPrivs(kCIDLib::False)
    , m_bPlatformDir(kCIDLib::False)
    , m_bPureCpp(kCIDLib::True)
    , m_bResFile(kCIDLib::False)
    , m_bUseSysLibs(kCIDLib::False)
    , m_bVarArgs(kCIDLib::False)
    , m_bVersioned(kCIDLib::False)
    , m_eDisplayType(tCIDBuild::EDisplayTypes::Console)
    , m_eType(tCIDBuild::EProjTypes::Executable)
    , m_c4Base(0)
    , m_c4DepIndex(0xFFFFFFFF)
    , m_strProjectName(strName)
{
    // Copy over the include/exclude lists
    {
        tCIDBuild::TStrList::TCursor cursIncl(&listPlatIncl);
        if (cursIncl.bResetIter())
        {
            do
            {
                m_listPlatformsIncl.Add(new TBldStr(cursIncl.tCurElement()));
            }   while (cursIncl.bNext());
        }
    }

    {
        tCIDBuild::TStrList::TCursor cursExcl(&listPlatExcl);
        if (cursExcl.bResetIter())
        {
            do
            {
                m_listPlatformsExcl.Add(new TBldStr(cursExcl.tCurElement()));
            }   while (cursExcl.bNext());
        }
    }
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

tCIDLib::TBoolean TProjectInfo::bPureCpp() const
{
    return m_bPureCpp;
}


//
//  Returns true if this project is supported on the platform currently being
//  built.
//
tCIDLib::TBoolean TProjectInfo::bSupportsThisPlatform() const
{
    return TUtils::bSupportsPlatform
    (
        kCIDBuild::pszFullPlatform, m_listPlatformsIncl, m_listPlatformsExcl
    );
}


//
//  Returns true if this project is supported on the passed platform.
//
tCIDLib::TBoolean TProjectInfo::bSupportsPlatform(const TBldStr& strToCheck) const
{
    return TUtils::bSupportsPlatform(strToCheck, m_listPlatformsIncl, m_listPlatformsExcl);
}


tCIDLib::TBoolean TProjectInfo::bUseSysLibs() const
{
    return m_bUseSysLibs;
}


tCIDLib::TBoolean
TProjectInfo::bUsesExtLib(const TBldStr& strToCheck) const
{
    // Search the external libs list for this one.
    tCIDBuild::TStrList::TCursor cursLibs(&m_listExtLibs);
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
            << L"        Pure Cpp: " << (m_bPureCpp ? L"Yes\n" : L"No\n")
            << L"    Use Sys Libs: " << (m_bUseSysLibs ? L"Yes\n" : L"No\n")
            << L"        Var Args: " << (m_bVarArgs ? L"Yes\n" : L"No\n")
            << L"          Sample: " << (m_bIsSample ? L"Yes\n" : L"No\n");

    stdOut << L"  Incl  Platforms: ";
    {
        tCIDBuild::TStrList::TCursor cursPlatforms(&m_listPlatformsIncl);
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

    stdOut << L"  Excl  Platforms: ";
    {
        tCIDBuild::TStrList::TCursor cursPlatforms(&m_listPlatformsExcl);
        if (!cursPlatforms.bResetIter())
        {
            stdOut << L"None";
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

    // If any options show those
    {
        tCIDBuild::TKVPList::TCursor cursOpts(&m_listOptions);
        if (cursOpts.bResetIter())
        {
            stdOut << L"\n    Options:\n    ----------------\n";
            do
            {
                const TKeyValuePair& kvpCur = cursOpts.tCurElement();
                stdOut  << L"        " << kvpCur.strKey()
                        << kCIDLib::chEquals << kvpCur.strValue()
                        << kCIDBuild ::EndLn;

            }   while (cursOpts.bNext());
        }
    }

    stdOut << kCIDBuild::EndLn;
}


tCIDBuild::EDisplayTypes TProjectInfo::eDisplayType() const
{
    return m_eDisplayType;
}

tCIDBuild::EProjTypes TProjectInfo::eType() const
{
    return m_eType;
}

const TList<TFindInfo>& TProjectInfo::listCpps() const
{
    return m_listCpps;
}


const tCIDBuild::TStrList& TProjectInfo::listCustomCmds() const
{
    return m_listCustomCmds;
}


const  tCIDBuild::TStrList& TProjectInfo::listDeps() const
{
    return m_listDeps;
}

const  tCIDBuild::TStrList& TProjectInfo::listExtLibs() const
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

const  tCIDBuild::TStrList& TProjectInfo::listIncludePaths() const
{
    return m_listIncludePaths;
}

const  tCIDBuild::TStrList& TProjectInfo::listShaders() const
{
    return m_listShaders;
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
        strSearch.Append(kCIDBuild::pszPathSep, kCIDBuild::pszAllHFiles);
        TFindInfo::c4FindFiles(strSearch, m_listHpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(kCIDBuild::pszPathSep, kCIDBuild::pszAllHppFiles);
        TFindInfo::c4FindFiles(strSearch, m_listHpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(kCIDBuild::pszPathSep, kCIDBuild::pszAllCFiles);
        TFindInfo::c4FindFiles(strSearch, m_listCpps, tCIDBuild::EPathModes::Relative);

        strSearch = kCIDBuild::pszPlatformDir;
        strSearch.Append(kCIDBuild::pszPathSep, kCIDBuild::pszAllCppFiles);
        TFindInfo::c4FindFiles(strSearch, m_listCpps, tCIDBuild::EPathModes::Relative);
    }
}


// Find an option in the optio list
const TKeyValuePair* TProjectInfo::pkvpFindOption(const TBldStr& strOption) const
{
    tCIDBuild::TKVPList::TCursor cursOpts(&m_listOptions);
	if (cursOpts.bResetIter())
	{
		do
		{
			if (cursOpts->strKey().bIEquals(strOption))
				return &cursOpts.tCurElement();

		} while (cursOpts.bNext());
	}

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
    tCIDLib::TBoolean bDone = kCIDLib::False;
    TBldStr           strReadBuf;
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
         else if (strReadBuf.bStartsWith(L"DEPENDENTS"))
        {
            strReadBuf.Cut(10);
            strReadBuf.StripWhitespace();
            ParseDependents(strReadBuf, lsplSource);
        }
         else if (strReadBuf == L"DEFINES")
        {
            ParseDefines(lsplSource);
        }
         else if (strReadBuf == L"EXTLIBS")
        {
            ParseExtLibs(lsplSource);
        }
         else if (strReadBuf == L"IDLFILES")
        {
            ParseIDLFiles(lsplSource);
        }
         else if (strReadBuf.bStartsWith("INCLUDEPATHS"))
        {
            strReadBuf.Cut(12);
            strReadBuf.StripWhitespace();
            ParseIncludePaths(strReadBuf, lsplSource);
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
         else if (strReadBuf.bStartsWith(L"OPTIONS"))
        {
            strReadBuf.Cut(7);
            strReadBuf.StripWhitespace();
            ParseOptions(strReadBuf, lsplSource);
        }
         else if (strReadBuf == L"SHADERS")
        {
            ParseShaders(lsplSource);
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
        m_strOutDir.Append(kCIDBuild::pszPathSep);

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
TProjectInfo::bBlockForThisPlatform(        TLineSpooler&       lsplSource
                                    , const TBldStr&            strPlatforms
                                    , const TBldStr&            strEndBlock
                                    , const tCIDLib::TCh* const pszExpected)
{
    if (!strPlatforms.bEmpty())
    {
        TBldStr strReadBuf;

        m_listTmp1.RemoveAll();
        m_listTmp2.RemoveAll();
        if (!TUtils::bParseInclExclLists(strPlatforms, m_listTmp1, m_listTmp2, strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") " << strReadBuf << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // If this block is not for the current platform, then skip it
        if (!TUtils::bSupportsThisPlatform(m_listTmp1, m_listTmp2))
        {
            // Tell the spooler to disable macro expansion for this
            tCIDLib::TBoolean bGotEnd = kCIDLib::False;
            while (lsplSource.bReadLine(strReadBuf, kCIDLib::True))
            {
                if (strReadBuf == strEndBlock)
                {
                    bGotEnd = kCIDLib::True;
                    break;
                }
            }

            if (!bGotEnd)
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Expected " << strEndBlock << L" or "
                        << pszExpected << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::UnexpectedEOF;
            }
            return kCIDLib::False;
        }
    }

    // Either empty list or it explicitly supports the platform
    return kCIDLib::True;
}


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
        // Store the raw directory, updating for local path sep type
        m_strDirectory = strValue;
        if (kCIDBuild::chProjectPathSep != kCIDBuild::chPathSep)
            m_strDirectory.ReplaceChar(kCIDBuild::chProjectPathSep, kCIDBuild::chPathSep);

        // Build up the path to the directory and store it.
        m_strProjectDir = facCIDBuild.strRootDir();

        // Indicate we want a trailing slash
        m_strProjectDir.AppendPathComps(L"Source", L"AllProjects", kCIDLib::True);

        // If it's not empty (i.e. top level), then add it, and end with a slash
        if (!m_strDirectory.bEmpty())
        {
            m_strProjectDir.Append(m_strDirectory);
            m_strProjectDir.Append(kCIDBuild::chPathSep);
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
            m_bMsgFile = kCIDLib::False;
        else if (strValue == L"Yes")
            m_bMsgFile = kCIDLib::True;
        else
            return kCIDLib::False;
    }
     else if (strName == L"RESFILE")
    {
        if (!TRawStr::bXlatBoolean(strValue.pszBuffer(), m_bResFile))
            return kCIDLib::False;
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
     else if (strName == L"PURECPP")
    {
        if (!TRawStr::bXlatBoolean(strValue.pszBuffer(), m_bPureCpp))
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


tCIDLib::TVoid
TProjectInfo::ParseDependents(const TBldStr& strOptions, TLineSpooler& lsplSource)
{
    static const TBldStr strEndBlock(L"END DEPENDENTS");

    // If not for this platform, then we skip this block
    if (!bBlockForThisPlatform(lsplSource, strOptions, strEndBlock, L"dependents"))
        return;

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


tCIDLib::TVoid
TProjectInfo::ParseIncludePaths(const TBldStr& strOptions, TLineSpooler& lsplSource)
{
    static const TBldStr strEndBlock(L"END INCLUDEPATHS");

    // Only valid for code type projects
    if (m_eType > tCIDBuild::EProjTypes::MaxCodeType)
    {
        stdOut  << L"(Line " << lsplSource.c4CurLine()
                << L") Not valid for a 'filecopy' project"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotSupported;
    }


    // If not for this platform, the we skip this block
    if (!bBlockForThisPlatform(lsplSource, strOptions, strEndBlock, L"include path"))
        return;

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



tCIDLib::TVoid TProjectInfo::ParseOptions(const TBldStr& strPlatforms, TLineSpooler& lsplSource)
{
    static const TBldStr strEndBlock(L"END OPTIONS");

    // If not for this platform, the we skipp it
    if (!bBlockForThisPlatform(lsplSource, strPlatforms, strEndBlock, L"option key/value pair"))
        return;

    //
    //  Now we loop till we see the end of this block, pulling out lines
    //  each of which is either a value or a key=value pair.
    //
    TBldStr strOptName;
    TBldStr strOptValue;
    TBldStr strReadBuf;

    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected END OPTIONS or a key=value option"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == strEndBlock)
            break;

        // It has to be in our standard key=value format
        if (!TUtils::bFindNVParts(strReadBuf, strOptName, strOptValue))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Badly formed platform option statement" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Add a new pair for this guy
        m_listOptions.Add(new TKeyValuePair(strOptName, strOptValue));
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


// Parses any shade files from the SHADERS= block, which has already been see
tCIDLib::TVoid TProjectInfo::ParseShaders(TLineSpooler& lsplSource)
{
    static const TBldStr strEndBlock(L"END SHADERS");

    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected shader file name or END SHADERS"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == strEndBlock)
            break;

        // Store it as a shader file
        m_listShaders.Add(new TBldStr(strReadBuf));
    }
}
