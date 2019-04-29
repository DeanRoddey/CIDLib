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
//  TFacCIDDocComp: Public, static data members
// ---------------------------------------------------------------------------
const TString   TFacCIDDocComp::s_strExt_HelpPage(L"cqchpage");

const TString   TFacCIDDocComp::s_strColSpan(L"ColSpan");
const TString   TFacCIDDocComp::s_strExtra(L"Extra");
const TString   TFacCIDDocComp::s_strExtra1(L"Extra1");
const TString   TFacCIDDocComp::s_strExtra2(L"Extra2");
const TString   TFacCIDDocComp::s_strFileExt(L"FileExt");
const TString   TFacCIDDocComp::s_strFileName(L"FileName");
const TString   TFacCIDDocComp::s_strId(L"Id");
const TString   TFacCIDDocComp::s_strPageLink(L"PageLink");
const TString   TFacCIDDocComp::s_strPageMap(L"PageMap");
const TString   TFacCIDDocComp::s_strRef(L"Ref");
const TString   TFacCIDDocComp::s_strRoot(L"/");
const TString   TFacCIDDocComp::s_strSubDir(L"SubDir");
const TString   TFacCIDDocComp::s_strSubTopicMap(L"SubTopicMap");
const TString   TFacCIDDocComp::s_strTitle(L"Title");
const TString   TFacCIDDocComp::s_strTopicPage(L"TopicPage");
const TString   TFacCIDDocComp::s_strType(L"Type");
const TString   TFacCIDDocComp::s_strV2Compat(L"  <Superscript>[V2]</Superscript>");
const TString   TFacCIDDocComp::s_strVirtual(L"Virtual");



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

    // Keep a path to the Src directory under that for later use
    pathTmp.AddLevel(L"Src");
    m_strSrcSrcPath = pathTmp;

    try
    {
        //
        //  Create the starting directory for the tree of source XML files that
        //  we process.
        //
        TPathStr pathXMLSrc(m_strSrcPath);
        pathXMLSrc.AddLevel(L"Src");

        //
        //  We'll use a single parser. In order to avoid a lot of complexity with
        //  maintaining DTD paths in the XML files, just do a mapping of the DTD
        //  via the URN, so that the parser won't try to load the file. We have
        //  to load the DTD content to a string, which we pass in as the mapped
        //  entity content.
        //
        LoadDTD();

        //
        //  We start with a root topic, and a root path of /. We'll then start parsing
        //  at the XML source directory. So the index file there will fill in the root
        //  and we'll go from there.
        //
        m_topicRoot.m_strTitle = L"CIDLib Documentation";
        if (bIterateTopics(pathXMLSrc, TString::strEmpty(), s_strRoot, m_topicRoot))
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

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"jquery.js");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"jquery.js");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

        }
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


// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to iterate the topics. It is recursive and we are started off
//  on the root topic member, and it goes fro there.
//
//  We get the directory name separately for convenience, so we don't have to parse
//  it back out of the source path. We want to have it later when we start doing the
//  output, and need to build up the output directories.
//
//  For the top level when we start, the source and help path are / and the directory
//  name is empty.
//
//  Since we reuse the parser for pages and we recurse, we get out all of the topic
//  file info and store it in TPageInfo and TSubTopicInfo objects before we start
//  parsing page files. Then we can go back and work through those stored lists.
//
tCIDLib::TBoolean
TFacCIDDocComp::bIterateTopics( const   TString&    strSrcPath
                                , const TString&    strDirName
                                , const TString&    strHelpPath
                                ,       TTopic&     topicToFill)
{
    TDirIter    diterSrc;
    TFindBuf    fndbCur;
    TPathStr    pathTmp;

    // Output the category we are doing
    strmOut() << L"\n   Key: " << strHelpPath << kCIDLib::EndLn;

    // Look for the topic file at this level
    pathTmp = strSrcPath;
    pathTmp.AddLevel(L"CIDTopic");
    pathTmp.AppendExt(L"xml");
    if (!TFileSys::bExists(pathTmp))
    {
        facCIDDocComp.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDDocsErrs::errcSrc_NoTopicFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , pathTmp
        );
    }

    // Let's parse the topic file
    const tCIDLib::TBoolean bRes = m_xtprsToUse.bParseRootEntity
    (
        pathTmp
        , tCIDXML::EParseOpts::Validate
        , tCIDXML::EParseFlags::TagsNText
    );

    if (!bRes)
    {
        const TXMLTreeParser::TErrInfo& errCur = m_xtprsToUse.colErrors()[0];
        m_strmErr   << L"The CIDCTopic parse failed\n"
                    << errCur.strText()
                    << kCIDLib::NewLn << L"(" << errCur.c4Line()
                    << kCIDLib::chPeriod << errCur.c4Column()
                    << L") " << pathTmp << kCIDLib::NewEndLn;
        return kCIDLib::False;
    }

    const TXMLTreeElement& xtnodeRoot = m_xtprsToUse.xtdocThis().xtnodeRoot();

    // Store our path info
    topicToFill.m_strTopicPath = strHelpPath;
    topicToFill.m_strSrcPath = strSrcPath;
    topicToFill.m_strDirName = strDirName;

    // Get the topic page attribute
    topicToFill.m_strTopicPage = xtnodeRoot.xtattrNamed(L"TopicPage").strValue();

    //
    //  We now go through and get out all of the page and sub-top info from the
    //  XML, then we can start reusing the XML parser.
    //
    tCIDLib::TCard4 c4Count;
    TTopicList colSubTopics;
    TPageInfo pinfoCur;
    {
        tCIDLib::TCard4 c4At = 0;
        const TXMLTreeElement& xtnodePageMap = xtnodeRoot.xtnodeFindElement
        (
            TFacCIDDocComp::s_strPageMap, 0, c4At
        );
        c4Count = xtnodePageMap.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeNode& xtnodeCur = xtnodePageMap.xtnodeChildAt(c4Index);

            if (xtnodeCur.eType() != tCIDXML::ENodeTypes::Element)
                continue;

            // Reset our page info object and let's load up the stuff for this round
            pinfoCur.Reset();
            const TXMLTreeElement& xtnodeElem = xtnodePageMap.xtnodeChildAtAsElement(c4Index);
            if (!xtnodeElem.bAttrExists(s_strFileName, pinfoCur.m_strFile)
            ||  !xtnodeElem.bAttrExists(s_strFileExt, pinfoCur.m_strExt)
            ||  !xtnodeElem.bAttrExists(s_strTitle, pinfoCur.m_strTitle))
            {
                // We are missing required info
                facCIDDocComp.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDDocsErrs::errcSrc_PageMapInfo
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            // See if it's a virtual one
            pinfoCur.m_bVirtual = xtnodeElem.xtattrNamed(TFacCIDDocComp::s_strVirtual).bValueAs();

            // Store the extra values if present
            xtnodeElem.bAttrExists(s_strExtra1, pinfoCur.m_strExtra1);
            xtnodeElem.bAttrExists(s_strExtra2, pinfoCur.m_strExtra2);

            //
            //  If the link text is empty, set it to the same as the file name. In a good
            //  number of cases it is, and that saves work in the XML.
            //
            if (pinfoCur.m_strTitle.bIsEmpty())
                pinfoCur.m_strTitle = pinfoCur.m_strFile;

            pinfoCur.Complete(strHelpPath);

            // And add this page info to the main page list and this topic's page map
            topicToFill.m_colPageMap.Add(&m_colPages.objAdd(pinfoCur));
        }

        const TXMLTreeElement& xtnodeSubTopicMap = xtnodeRoot.xtnodeFindElement
        (
            TFacCIDDocComp::s_strSubTopicMap, 0, c4At
        );
        c4Count = xtnodeSubTopicMap.c4ChildCount();
        TSubTopicInfo stinfoCur;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeNode& xtnodeCur = xtnodeSubTopicMap.xtnodeChildAt(c4Index);

            if (xtnodeCur.eType() != tCIDXML::ENodeTypes::Element)
                continue;

            const TXMLTreeElement& xtnodeElem = xtnodeSubTopicMap.xtnodeChildAtAsElement(c4Index);

            if (!xtnodeElem.bAttrExists(TFacCIDDocComp::s_strSubDir, stinfoCur.m_strSubDir)
            ||  !xtnodeElem.bAttrExists(TFacCIDDocComp::s_strTitle, stinfoCur.m_strTitle))
            {
                // We are missing required info
                facCIDDocComp.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDDocsErrs::errcSrc_TopicMapInfo
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            //
            //  If the link text is empty, set it to the same as the dir name.
            //
            if (stinfoCur.m_strTitle.bIsEmpty())
                stinfoCur.m_strTitle = stinfoCur.m_strSubDir;

            stinfoCur.Complete(strHelpPath);

            // Add this subtopic info to our local list and the overall list
            colSubTopics.objAdd(stinfoCur);
            m_colTopics.objAdd(stinfoCur);
        }
    }


    // OK, now we can go back and start processing the page files
    c4Count = topicToFill.m_colPageMap.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TPageInfo& pinfoCur = *topicToFill.m_colPageMap[c4Index];

        // If this one is virtual, skip it. It will be generated later
        if (pinfoCur.m_bVirtual)
            continue;

        // See if this one has been compiled yet
        TBasePage* ppgNew = topicToFill.m_colParsedPages.pobjFindByKey
        (
            pinfoCur.m_strPath, kCIDLib::False
        );

        // If not, let's let it parse its content
        if (!ppgNew)
        {
            if (pinfoCur.m_strExt.bCompareI(s_strExt_HelpPage))
                ppgNew = new THelpPage();

            CIDAssert(ppgNew != nullptr, L"Unknown page extension")

            pathTmp = strSrcPath;
            pathTmp.AddLevel(pinfoCur.m_strFile);
            pathTmp.AppendExt(pinfoCur.m_strExt);

            const tCIDLib::TBoolean bRes = bParsePageFile
            (
                pathTmp, pinfoCur, *ppgNew, topicToFill, kCIDLib::True
            );

            if (!bRes)
                return kCIDLib::False;

            // And add this one to the list of parsed pages for this topic
            topicToFill.m_colParsedPages.Add(ppgNew);
        }

        // Store the page in the page info object
        pinfoCur.m_ppgThis = ppgNew;
    }

    //
    //  And go through the child topics map and recurse on them.
    //
    c4Count = colSubTopics.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TSubTopicInfo& stinfoCur = colSubTopics[c4Index];

        // Add a new topic object for this topic and fill in it
        TTopic* ptopicSub = new TTopic();
        topicToFill.m_colChildTopicMap.Add(ptopicSub);

        pathTmp = strSrcPath;
        pathTmp.AddLevel(stinfoCur.m_strSubDir);

        // The title we set at this level since it is gotten from the map items
        ptopicSub->m_strTitle = stinfoCur.m_strTitle;
        if (!bIterateTopics(pathTmp, stinfoCur.m_strSubDir, stinfoCur.m_strPath, *ptopicSub))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  We have to parse a help page from more than one place, so we break it out here
//  separately. It returns true if successful. Sometimes we are parsing a file and
//  sometimes we are parsing actual XML content generated out into a string, so we
//  have a parameter to tell us. If it's XML content, then the strSysId is a system
//  id we can pass to the parse for error reporting. Else it's just the base name to
//  show as what we are parsing.
//
tCIDLib::TBoolean
TFacCIDDocComp::bParsePageFile( const   TString&            strSrc
                                , const TPageInfo&          pinfoSrc
                                ,       TBasePage&          pgTar
                                ,       TTopic&             topicPar
                                , const tCIDLib::TBoolean   bFileSrc)
{
    // Try to parse the passed file as XML
    tCIDLib::TBoolean bRes;

    if (bFileSrc)
    {
        // It's a file, so we have a method for that directly
        bRes =  m_xtprsToUse.bParseRootEntity
        (
            strSrc, tCIDXML::EParseOpts::Validate, tCIDXML::EParseFlags::TagsNText
        );
    }
     else
    {
        // We need to set up a memory buffer entity source for this one
        tCIDXML::TEntitySrcRef esrSrc(new TMemBufEntitySrc(pinfoSrc.m_strPath, strSrc));
        bRes = m_xtprsToUse.bParseRootEntity
        (
            esrSrc, tCIDXML::EParseOpts::Validate, tCIDXML::EParseFlags::TagsNText
        );
    }

    // If an XML level errors, display the first one and return
    if (!bRes)
    {
        const TXMLTreeParser::TErrInfo& errCur = m_xtprsToUse.colErrors()[0];
        m_strmErr   << L"The help page parse failed\n"
                    << errCur.strText()
                    << kCIDLib::NewLn << L"(" << errCur.c4Line()
                    << kCIDLib::chPeriod << errCur.c4Column()
                    << L") "
                    << pinfoSrc.m_strPath << kCIDLib::NewEndLn;
        return kCIDLib::False;
    }

    // Output the name of the content we are going to parse
    strmOut() << L"        -> " << pinfoSrc.m_strPath << kCIDLib::EndLn;

    //
    //  Get the root now out and kick off and let the target page object try to pull
    //  his data from it.
    //
    const TXMLTreeElement& xtnodeRoot = m_xtprsToUse.xtdocThis().xtnodeRoot();
    if (!pgTar.bParseXML(xtnodeRoot, pinfoSrc.m_strFile, topicPar))
    {
        m_strmErr << L"Page could not internalize parsed XML content\n"
                  << pinfoSrc.m_strPath << kCIDLib::NewEndLn;
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Copy the contents of a source directory to a target one, recursively.
//
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


//
//  Spits out the standard header content for a regular help page, with title, to avoid
//  any duplication since we may do this in a number of places where we generate dynamic
//  content. We output whatever the encoding name is that the target stream says that he
//  is creating, so that this will work for files or string based streams or whatever.
//
tCIDLib::TVoid
TFacCIDDocComp::OutputStdHPHeader(TTextOutStream& strmTar, const TString& strTitle)
{
    strmTar << L"<?xml version=\"1.0\" encoding=\""
            << strmTar.strEncodingName()
            << L"\"?>\n"
            << L"<!DOCTYPE HelpPage PUBLIC "
            << L"\"urn:charmedquark.com:CIDLib-Documentation.DTD\" \"CIDDocs.DTD\">\n\n"
            << L"<HelpPage>\n\n"
            << L"<Title>" << strTitle << L"</Title>\n"
            << L"<HelpText>\n";
}


//
//  This will locate a topic by its path, e.g. /Reference/Drivers. We pull out each
//  component of the path at a time, and look for a sub-topic with that name, and so on.
//
TTopic& TFacCIDDocComp::topicFindByPath(const TString& strToFind)
{
    // Start at the top root topic
    TTopic* ptopicCur = &m_topicRoot;
    TStringTokenizer stokCur(&strToFind, L"/");

    TString strCurName;
    TString strCurPath(strToFind.c4Length());
    while (stokCur.bGetNextToken(strCurName))
    {
        strCurPath.Append(L"/");
        strCurPath.Append(strCurName);

        const tCIDLib::TCard4 c4Count = ptopicCur->m_colChildTopicMap.c4ElemCount();
        tCIDLib::TCard4 c4Index = 0;
        while (c4Index < c4Count)
        {
            // If we find this one, make it the new current topic and go again
            if (ptopicCur->m_colChildTopicMap[c4Index]->m_strTopicPath.bCompareI(strCurPath))
            {
                ptopicCur = ptopicCur->m_colChildTopicMap[c4Index];
                break;
            }
            c4Index++;
        }

        // Never found this one
        if (c4Index == c4Count)
        {
            ptopicCur = nullptr;
            break;
        }
    }

    // If not found, then throw
    if (!ptopicCur)
    {
        facCIDDocComp.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDDocsErrs::errcSrc_TopicNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
        );
    }

    return *ptopicCur;
}
