//
// FILE NAME: CIDDocComp_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This module implements the facility class for this program. This program's
//  facility class provides the centralized access to program functionality.
//  Everything goes through it, making it easy and convenient to coordinate
//  the program's work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacCIDDocComp, TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDDocComp
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDDocComp::TFacCIDDocComp() :

    TFacility
    (
        L"CIDDocComp"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_bVerbose(kCIDLib::False)
    , m_strmErr(tCIDLib::EStdFiles::StdErr)
    , m_pstrmOut(nullptr)
{
}

TFacCIDDocComp::~TFacCIDDocComp()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::EExitCodes
TFacCIDDocComp::eMainThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Check for a /Verbose command line parameter
    tCIDLib::TCard4 c4Args = TSysInfo::c4CmdLineParmCount();
    if (c4Args > 0)
        m_bVerbose = TSysInfo::strCmdLineParmAt(0).bCompareI(L"/verbose");

    // Figure out which output stream to use
    if (m_bVerbose)
        m_pstrmOut = new TTextFileOutStream(tCIDLib::EStdFiles::StdOut);
    else
        m_pstrmOut = new TTextStringOutStream(64 * 1024UL);

    strmOut() << L"CIDLib Documentation Compiler" << kCIDLib::NewEndLn;

    //
    //  Set an indent on the error stream and leave it set, so that all error
    //  msgs will be indented.
    //
    m_strmErr.c4Indent(4);

    // We output into a directory under the result directory
    TPathStr pathTmp;
    if (!TProcEnvironment::bFind(L"CID_RESDIR", pathTmp))
    {
        m_strmErr << L"   Could not find CIDLib result path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }

    // Create the target path if it doesn't exist
    pathTmp.AddLevel(L"CIDDocs.Out");
    TFileSys::MakePath(pathTmp);

    // Store it away now that it's good
    m_strTarPath = pathTmp;


    // Create the top level source directory
    if (!TProcEnvironment::bFind(L"CID_SRCTREE", pathTmp))
    {
        m_strmErr << L"   Could not find CIDLib Source path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }
    pathTmp.AddLevel(L"Source");
    pathTmp.AddLevel(L"Docs");
    pathTmp.AddLevel(L"CIDDocs");

    // Make sure it exists
    if (!TFileSys::bIsDirectory(pathTmp))
    {
        m_strmErr   << L"   Source path doesn't exists Path="
                    << pathTmp << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    // Store it away now
    m_strSrcPath = pathTmp;

    try
    {
        //
        //  We'll use a single parser. In order to avoid a lot of complexity with
        //  maintaining DTD paths in the XML files, just do a mapping of the DTD via
        //  the URN, so that the parser won't try to load the file. We have to load
        //  the DTD content to a string, which we pass in as the mapped entity content.
        //
        LoadDTD();

        //
        //  We have to create a special case root topic to start the process.
        //
        m_cptrRoot.SetPointer
        (
            new TTopic(L"Root", L"/", TString::strEmpty(), m_strSrcPath)
        );

        if (m_cptrRoot->bParse(m_xtprsToUse))
        {
            //
            //  Copy any files to the top level target directory that we just use
            //  as is. They will be in the top level source directory.
            //
            //  1.  The main HTML file which is hand done and includes the main
            //      header with menu and such. It includes a DIV into which all
            //      of the actual content is dynamically loaded.
            //  2.  The CSS file that contains the formatting info
            //  3.  Our own javascript file
            //  4.  The jquery javascript file
            //
            TPathStr pathCopySrc;
            TPathStr pathCopyTar;

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.html");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.html");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.css");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.css");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.js");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.js");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

/*
            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"jquery.js");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"jquery.js");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);
*/

            // Copy over all of the images
            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"Images");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"Images");
            if (!TFileSys::bIsDirectory(pathCopyTar))
                TFileSys::MakeSubDirectory(L"Images", m_strTarPath);
            CopyDir(pathCopySrc, pathCopyTar);


            //
            //  And now invoke the output generation on the root topic. For this one
            //  there is no parent, so pass an empty pointer. We also have to pass
            //  him our own pointer since it won't be available inside the method
            //  otherwise.
            //
            m_cptrRoot->GenerateOutput(m_strTarPath, TTopic::TTopicPtr(), m_cptrRoot);
        }

        // Now let's drop the root topic to clean everything up
        m_cptrRoot.DropRef();
    }

    catch(const TError& errToCatch)
    {
        m_strmErr   << L"   An exception occurred. Error=\n"
                    << errToCatch<< kCIDLib::NewEndLn;
        m_strmErr.Flush();
        return tCIDLib::EExitCodes::RuntimeError;
    }

    strmOut() << kCIDLib::FlushIt;
    return tCIDLib::EExitCodes::Normal;
}


//
//  We provide an XML parser that we just pass along to be used in all of the
//  parsing. If any XML parsing errors occur, this is called and we can do the
//  grunt work of getting the info out for the first error and displaying it
//  on the error stream.
//
tCIDLib::TVoid TFacCIDDocComp::ShowXMLParseErr(const TString& strSrcPath)
{
    const TXMLTreeParser::TErrInfo& errCur = m_xtprsToUse.colErrors()[0];
    m_strmErr   << L"XML Error: "
                << errCur.strText() << kCIDLib::NewLn
                << L"    - " << errCur.c4Line()
                << kCIDLib::chPeriod << errCur.c4Column()
                << L" - " << strSrcPath << kCIDLib::EndLn;
}



// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TFacCIDDocComp::CopyDir(const TString& strSrc, const TString& strTar)
{
    TDirIter    diterSrc;
    TFindBuf    fndbCur;

    // Copy any files first
    TPathStr pathTar;
    TString  strNameExt;
    if (diterSrc.bFindFirst(strSrc, L"*.*", fndbCur, tCIDLib::EDirSearchFlags::NormalFiles))
    {
        do
        {
            // Build up the target path for this file
            fndbCur.pathFileName().bQueryNameExt(strNameExt);
            pathTar = strTar;
            pathTar.AddLevel(strNameExt);

            TFileSys::CopyFile(fndbCur.pathFileName(), pathTar);
        }   while (diterSrc.bFindNext(fndbCur));
    }

    // And now do any child directories
    if (diterSrc.bFindFirst(strSrc, L"*", fndbCur, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        do
        {
            //
            //  Build up the target path for this file. And make sure it
            //  exists.
            //
            fndbCur.pathFileName().bQueryNameExt(strNameExt);
            pathTar = strTar;
            pathTar.AddLevel(strNameExt);

            if (!TFileSys::bIsDirectory(pathTar))
                TFileSys::MakeSubDirectory(strNameExt, strTar);

            CopyDir(fndbCur.pathFileName(), pathTar);

        }   while (diterSrc.bFindNext(fndbCur));
    }
}


//
//  Called to load our DTD text and add it to our parser as a preloaded entity,
//  so that the individual XML files don't have to deal with pointing themselves
//  at an actual DTD file (with all of the nesting going on it's a pain.) And it
//  will speed things up as well.
//
//  But it keeps the DTD external so we can quickly make changes and test them.
//
tCIDLib::TVoid TFacCIDDocComp::LoadDTD()
{
    // Build up the full path name
    TPathStr pathDTD(m_strSrcPath);
    pathDTD.AddLevel(L"CIDDocs.DTD");

    TTextFileInStream strmDTD
    (
        pathDTD
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
        , facCIDEncode().ptcvtMakeNew(L"Latin1")
    );

    //
    //  Read in the lines and add them to a string. The file doesn't have the
    //  entity header line, so we add that, indicating our internal string
    //  format as the encoding.
    //
    TString strDTDText(32 * 1024UL);
    strDTDText = L"<?xml encoding='$NativeWideChar$'?>\n";

    TString strLine(128UL);
    while (!strmDTD.bEndOfStream())
    {
        strmDTD >> strLine;
        strDTDText.Append(strLine);
        strDTDText.Append(L"\n");
    }

    m_xtprsToUse.AddMapping
    (
        new TMemBufEntitySrc
        (
            L"http://www.charmedquark.com/CIDLib/CIDHelpDocs.DTD"
            , L"urn:charmedquark.com:CIDLib-Documentation.DTD"
            , strDTDText
        )
    );
}

