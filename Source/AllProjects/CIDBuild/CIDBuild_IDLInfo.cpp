//
// FILE NAME: CIDBuild_IDLInfo.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/27/2001
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
//  This file implements the TIDLInfo class
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
//   CLASS: TIDLInfo
//  PREFIX: idli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIDLInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TIDLInfo::TIDLInfo() :

    m_bGenClient(kCIDLib::False)
    , m_bGenCSharp(kCIDLib::False)
    , m_bGenGlobals(kCIDLib::False)
    , m_bGenServer(kCIDLib::False)
    , m_bGenTypeScript(kCIDLib::False)
{
}

TIDLInfo::TIDLInfo(const TIDLInfo& idliSrc) :

    m_bGenClient(idliSrc.m_bGenClient)
    , m_bGenCSharp(idliSrc.m_bGenCSharp)
    , m_bGenGlobals(idliSrc.m_bGenGlobals)
    , m_bGenServer(idliSrc.m_bGenServer)
    , m_bGenTypeScript(idliSrc.m_bGenTypeScript)
    , m_strNameExt(idliSrc.m_strNameExt)
    , m_strSrcFile(idliSrc.m_strSrcFile)
    , m_strTSOutPath(idliSrc.m_strTSOutPath)
{
}

TIDLInfo::~TIDLInfo()
{
}


// ---------------------------------------------------------------------------
//  TIDLInfo: Public operators
// ---------------------------------------------------------------------------
TIDLInfo& TIDLInfo::operator=(const TIDLInfo& idliSrc)
{
    if (this != &idliSrc)
    {
        m_bGenClient        = idliSrc.m_bGenClient;
        m_bGenCSharp        = idliSrc.m_bGenCSharp;
        m_bGenGlobals       = idliSrc.m_bGenGlobals;
        m_bGenServer        = idliSrc.m_bGenServer;
        m_bGenTypeScript    = idliSrc.m_bGenTypeScript;
        m_strNameExt        = idliSrc.m_strNameExt;
        m_strSrcFile        = idliSrc.m_strSrcFile;
        m_strTSOutPath      = idliSrc.m_strTSOutPath;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TIDLInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TIDLInfo::GenerateCode(const TProjectInfo& projiSrc)
{
    // Change to the project directory
    if (!TUtils::bChangeDir(projiSrc.strProjectDir()))
    {
        stdOut << L"Could not change to project directory" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    //
    //  First see if we can find the source file. It has to be relative to
    //  the project directory.
    //
    TBldStr strSrcPath = projiSrc.strProjectDir();
    strSrcPath.Append(m_strSrcFile);

    TFindInfo fndiSrc;
    if (!TFindInfo::bFindAFile(strSrcPath, fndiSrc, tCIDBuild::EPathModes::Relative))
    {
        stdOut  << L"Source file: " << m_strSrcFile << L" was not found"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  Ok, lets build the paths to the output files. Regardless of the
    //  build mode, we go ahead and build up all of the output file names.
    //  The Cpp files go into the project directory and the headers got to
    //  the output header directory, so we can start them off easily enough.
    //
    TBldStr strNameBase(projiSrc.strProjectDir());
    strNameBase.Append(projiSrc.strProjectName());
    strNameBase.Append(L"_");
    if (!m_strNameExt.bEmpty())
        strNameBase.Append(m_strNameExt);

    TBldStr strClientCpp(strNameBase);
    TBldStr strClientHpp(strNameBase);
    TBldStr strServerCpp(strNameBase);
    TBldStr strServerHpp(strNameBase);
    TBldStr strGlobalsHpp(strNameBase);

    // And then the rest is unique for each file
    strClientCpp.Append(L"ClientProxy.cpp");
    strClientHpp.Append(L"ClientProxy.hpp");
    strServerCpp.Append(L"ServerBase.cpp");
    strServerHpp.Append(L"ServerBase.hpp");
    strGlobalsHpp.Append(L".hpp");

    // This might be bogus if we aren't doing C# bindings for this file
    TBldStr strCSharpImpl(facCIDBuild.strIncludeDir());
    strCSharpImpl.Append(L"C#/");
    strCSharpImpl.Append(projiSrc.strProjectName());
    strCSharpImpl.Append(L"_");
    strCSharpImpl.Append(m_strNameExt);
    strCSharpImpl.Append(L".cs");

    // This might be bogus if we aren't doing .ts bindings for this file
    TBldStr strTypeScriptImpl(facCIDBuild.strIncludeDir());
    strTypeScriptImpl.Append(L"TS/");
    strTypeScriptImpl.Append(projiSrc.strProjectName());
    strTypeScriptImpl.Append(L"_");
    strTypeScriptImpl.Append(m_strNameExt);
    strTypeScriptImpl.Append(L".ts");

    //
    //  These flags are set as a combination of can we (configured) and should we (out of
    //  date source relative to target) each type of file.
    //
    TFindInfo fndiTmp;
    tCIDLib::TBoolean bDoClient     = kCIDLib::False;
    tCIDLib::TBoolean bDoCSharp     = kCIDLib::False;
    tCIDLib::TBoolean bDoGlobals    = kCIDLib::False;
    tCIDLib::TBoolean bDoServer     = kCIDLib::False;
    tCIDLib::TBoolean bDoTypeScript = kCIDLib::False;

    //
    //  If we are in force mode, then generate the modes that we are doing
    //  for this IDL file. Else, look and see if we need to compile, by
    //  checking for the presence and time stamps of the target files.
    //
    if (facCIDBuild.bForce())
    {
        bDoClient       = m_bGenClient;
        bDoCSharp       = m_bGenCSharp;
        bDoGlobals      = m_bGenGlobals;
        bDoServer       = m_bGenServer;
        bDoTypeScript   = m_bGenTypeScript;

        // Indicate why if we are in verbose mode
        if (facCIDBuild.bVerbose())
            stdOut << L"    Compiling IDL due to /Force flag" << kCIDBuild::EndLn;
    }
     else
    {
        if (m_bGenClient)
        {
            if (!TFindInfo::bFindAFile(strClientCpp, fndiTmp))
            {
                bDoClient = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the client Cpp file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoClient && (fndiSrc > fndiTmp))
            {
                bDoClient = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the client Cpp is out of date"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoClient && !TFindInfo::bFindAFile(strClientHpp, fndiTmp))
            {
                bDoClient = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the client Hpp file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoClient && (fndiSrc > fndiTmp))
            {
                bDoClient = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the client Hpp is out of date"
                            << kCIDBuild::EndLn;
                }
            }
        }

        if (m_bGenServer)
        {
            if (!TFindInfo::bFindAFile(strServerCpp, fndiTmp))
            {
                bDoServer = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the server Cpp file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoServer && (fndiSrc > fndiTmp))
            {
                bDoServer = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the server Cpp is out of date"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoServer && !TFindInfo::bFindAFile(strServerHpp, fndiTmp))
            {
                bDoServer = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the server Hpp file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoServer && (fndiSrc > fndiTmp))
            {
                bDoServer = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the Server Hpp is out of date"
                            << kCIDBuild::EndLn;
                }
            }
        }

        if (m_bGenGlobals)
        {
            if (!TFindInfo::bFindAFile(strGlobalsHpp, fndiTmp))
            {
                bDoGlobals = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the globals Hpp file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoGlobals && (fndiSrc > fndiTmp))
            {
                bDoGlobals = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the globals Hpp is out of date"
                            << kCIDBuild::EndLn;
                }
            }
        }

        if (m_bGenCSharp)
        {
            if (!TFindInfo::bFindAFile(strCSharpImpl, fndiTmp))
            {
                bDoCSharp = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the C# output file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoCSharp && (fndiSrc > fndiTmp))
            {
                bDoCSharp = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the C# output is out of date"
                            << kCIDBuild::EndLn;
                }
            }
        }

        if (m_bGenTypeScript)
        {
            if (!TFindInfo::bFindAFile(strTypeScriptImpl, fndiTmp))
            {
                bDoTypeScript = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the .ts output file doesn't exist"
                            << kCIDBuild::EndLn;
                }
            }

            if (!bDoTypeScript && (fndiSrc > fndiTmp))
            {
                bDoTypeScript = kCIDLib::True;
                if (facCIDBuild.bVerbose())
                {
                    stdOut  << L"    Compiling IDL " << m_strSrcFile
                            << L" because the .ts output is out of date"
                            << kCIDBuild::EndLn;
                }
            }
        }
    }

    // If we don't need to gen anything, then we are done
    if (!bDoClient && !bDoServer && !bDoGlobals && !bDoCSharp && !bDoTypeScript)
        return;

    //
    //  Otherwise, we have to invoke the IDL compiler. So lets build up
    //  the parameter list.
    //
    const tCIDLib::TCh* apszArgs[256];
    tCIDLib::TCard4 c4ArgCount = 0;
    apszArgs[c4ArgCount++] = L"CIDIDL";
    apszArgs[c4ArgCount++] = L"CodeGen";
    apszArgs[c4ArgCount++] = m_strSrcFile.pszBuffer();
    apszArgs[c4ArgCount++] = L"/NoLogo";

    //
    //  If a lib type project, then we want to have the generated code
    //  include the private header.
    //
    if ((projiSrc.eType() != tCIDBuild::EProjTypes::Executable)
    &&  (projiSrc.eType() != tCIDBuild::EProjTypes::Service))
    {
        apszArgs[c4ArgCount++] = L"/PrivHdr";
    }

    TBldStr strClientParm(L"/Client=");
    TBldStr strServerParm(L"/Server=");
    TBldStr strGlobalsParm(L"/Global=");
    TBldStr strCSharpParm(L"/CSharp=");
    TBldStr strTSParm(L"/TScrip=");

    TBldStr strClientExp(L"/CExport=");
    TBldStr strServerExp(L"/SExport=");
    TBldStr strGlobalsExp(L"/GExport=");

    if (bDoClient)
    {
        // Build up the client side string
        strClientParm.Append(projiSrc.strProjectName());
        strClientParm.Append(L";.\\");

        if (!m_strNameExt.bEmpty())
        {
            strClientParm.Append(L";");
            strClientParm.Append(m_strNameExt);
        }
        apszArgs[c4ArgCount++] = strClientParm.pszBuffer();

        if (!projiSrc.strExportKeyword().bEmpty())
        {
            strClientExp.Append(projiSrc.strExportKeyword());
            apszArgs[c4ArgCount++] = strClientExp.pszBuffer();
        }
    }

    if (bDoServer)
    {
        // Build up the server side string
        strServerParm.Append(projiSrc.strProjectName());
        strServerParm.Append(L";.\\");

        if (!m_strNameExt.bEmpty())
        {
            strServerParm.Append(L";");
            strServerParm.Append(m_strNameExt);
        }
        apszArgs[c4ArgCount++] = strServerParm.pszBuffer();

        if (!projiSrc.strExportKeyword().bEmpty())
        {
            strServerExp.Append(projiSrc.strExportKeyword());
            apszArgs[c4ArgCount++] = strServerExp.pszBuffer();
        }
    }

    if (bDoGlobals)
    {
        strGlobalsParm.Append(projiSrc.strProjectName());
        strGlobalsParm.Append(L";.\\;");
        strGlobalsParm.Append(m_strNameExt);
        apszArgs[c4ArgCount++] = strGlobalsParm.pszBuffer();

        if (!projiSrc.strExportKeyword().bEmpty())
        {
            strGlobalsExp.Append(projiSrc.strExportKeyword());
            apszArgs[c4ArgCount++] = strGlobalsExp.pszBuffer();
        }
    }

    if (bDoCSharp)
    {
        strCSharpParm.Append(projiSrc.strProjectName());
        strCSharpParm.Append(L";");
        strCSharpParm.Append(facCIDBuild.strIncludeDir());
        strCSharpParm.Append(L"C#\\");
        strCSharpParm.Append(L";");
        strCSharpParm.Append(m_strNameExt);
        apszArgs[c4ArgCount++] = strCSharpParm.pszBuffer();
    }

    if (bDoTypeScript)
    {
        strTSParm.Append(projiSrc.strProjectName());
        strTSParm.Append(L";");
        strTSParm.Append(m_strTSOutPath);
        strTSParm.Append(L";");
        strTSParm.Append(m_strNameExt);
        apszArgs[c4ArgCount++] = strTSParm.pszBuffer();
    }


    //
    //  If we have any mappings, add those parameters. We put in ours first
    //  then any global ones.
    //
    if (!m_listMappings.bEmpty())
    {
        TList<TBldStr>::TCursor cursMaps(&m_listMappings);
        if (cursMaps.bResetIter())
        {
            do
            {
                apszArgs[c4ArgCount++] = cursMaps.tCurElement().pszBuffer();
            }   while (cursMaps.bNext());
        }
    }

    TList<TBldStr>::TCursor cursMaps(&facCIDBuild.listIDLMappings());
    if (cursMaps.bResetIter())
    {
        do
        {
            apszArgs[c4ArgCount++] = cursMaps.tCurElement().pszBuffer();
        }   while (cursMaps.bNext());
    }

    if (facCIDBuild.bVerbose())
    {
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = 0; c4Ind < c4ArgCount; c4Ind++)
            stdOut << apszArgs[c4Ind] << L" ";
        stdOut << kCIDBuild::EndLn;
    }

    // Ok, lets try to invoke the IDL compiler
    tCIDLib::TCard4 c4Flags = kCIDBuild::c4ExecFlag_None;
    if (facCIDBuild.bLowPrio())
        c4Flags |= kCIDBuild::c4ExecFlag_LowPrio;
    tCIDLib::TCard4 c4Result;
    const tCIDLib::TBoolean bRes = TUtils::bExec(apszArgs, c4ArgCount, c4Result, c4Flags);

    // Space the output if verbose
    if (facCIDBuild.bVerbose())
        stdOut << kCIDBuild::EndLn;

    // If the exec failed, tell the user
    if (!bRes)
    {
        stdOut << L"Could not execute the IDL compiler" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }

    // If the program itself returned an error
    if (c4Result)
    {
        stdOut  << L"Failed compilation of IDL file: " << fndiSrc.strFileName()
                << L". Error Code: " << c4Result << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::BuildError;
    }
}


tCIDLib::TVoid TIDLInfo::Parse(TLineSpooler& lsplSource)
{
    // In case this get's reused, default values that aren't required
    m_bGenClient = kCIDLib::False;
    m_bGenCSharp = kCIDLib::False;
    m_bGenGlobals = kCIDLib::False;
    m_bGenServer = kCIDLib::False;
    m_bGenTypeScript = kCIDLib::False;
    m_listMappings.RemoveAll();
    m_strNameExt.Clear();
    m_strTSOutPath.Clear();

    TBldStr strName;
    TBldStr strReadBuf;
    TBldStr strValue;
    while (kCIDLib::True)
    {
        // Get the next line. If end of file, that's an error here
        if (!lsplSource.bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected SRCFILE=, GEN=, NAMEEXT=, TSPATH=, or END IDLFILE"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (strReadBuf == L"END IDLFILE")
            break;

        // Break out the name and value parts of the line
        if (!TUtils::bFindNVParts(strReadBuf, strName, strValue))
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected a 'key = value' statement" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Ok and now parse the rest according to the name field
        if (strName == L"SRCFILE")
        {
            m_strSrcFile = strValue;
            m_strSrcFile.Append(L".CIDIDL");
        }
         else if (strName == L"GEN")
        {
            //
            //  We can have a few different values here, indicating which of the types of
            //  output to generate. So break out the comma separated values. Copy the
            //  value so that we can tokenize it.
            //
            tCIDLib::TCh szTmp[2048];
            TRawStr::CopyStr(szTmp, strValue.pszBuffer());

            tCIDLib::TCh* pszTok = TRawStr::pszStrTok(szTmp, L", ");
            while (pszTok)
            {
                if (!TRawStr::iCompStr(pszTok, L"CLIENT"))
                {
                    m_bGenClient = kCIDLib::True;
                }
                 else if (!TRawStr::iCompStr(pszTok, L"CSHARP"))
                {
                    m_bGenCSharp = kCIDLib::True;
                }
                 else if (!TRawStr::iCompStr(pszTok, L"GLOBALS"))
                {
                    m_bGenGlobals = kCIDLib::True;
                }
                 else if (!TRawStr::iCompStr(pszTok, L"SERVER"))
                {
                    m_bGenServer = kCIDLib::True;
                }
                 else if (!TRawStr::iCompStr(pszTok, L"TYPESCRIPT"))
                {
                    m_bGenTypeScript = kCIDLib::True;
                }
                 else
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Unknown generation mode" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }
                pszTok = TRawStr::pszStrTok(0, L", ");
            }
        }
         else if (strName == L"NAMEEXT")
        {
            m_strNameExt = strValue;
        }
         else if (strName == L"MAPPING")
        {
            //
            //  Break out the two parts. We can overwrite the name and
            //  read buffer strings here, in order to avoid creating another
            //  temp. Although we store just the raw string, we want to do
            //  this in order to validate the format.
            //
            if (!TUtils::bFindNVParts(strValue, strName, strReadBuf))
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Badly formed mapping entry" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }

            // Make sure that neither is empty
            if (strName.bEmpty() || strReadBuf.bEmpty())
            {
                stdOut  << L"(Line " << lsplSource.c4CurLine()
                        << L") Badly formed mapping entry" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }

            // Lets add a mapping item
            m_listMappings.Add(new TBldStr(strValue));
        }
         else if (strName == L"TSPATH")
        {
            m_strTSOutPath = strValue;
        }
         else
        {
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected SRCFILE=, GEN=, or NAMEEXT="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }
    }

    // If generating globals, then the name extension must be provided
    if (m_bGenGlobals && m_strNameExt.bEmpty())
    {
        stdOut  << L"(Line " << lsplSource.c4CurLine()
                << L") If generating globals, then NAMEEXT must be provided"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
}


const TBldStr& TIDLInfo::strNameExt() const
{
    return m_strNameExt;
}


const TBldStr& TIDLInfo::strSrcFile() const
{
    return m_strSrcFile;
}


const TBldStr& TIDLInfo::strTSOutPath() const
{
    return m_strTSOutPath;
}
