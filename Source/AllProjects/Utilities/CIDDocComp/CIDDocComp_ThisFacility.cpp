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



// -----------------------------------------------------------------------------
//   CLASS: TCtxStackJan
//  PREFIX: jan
// -----------------------------------------------------------------------------
TCtxStackJan::TCtxStackJan(const TTopic& topicToPush)
{
    facCIDDocComp.m_colSrcStack.objPush
    (
        TFacCIDDocComp::TSrcStackItem
        {
            tCIDDocComp::ESrcTypes::Topic
            , TString::strEmpty()
            , topicToPush.strTopicPath()
            , TString::strEmpty()
        }
    );
}

TCtxStackJan::TCtxStackJan(const THelpPage& pgToPush)
{
    facCIDDocComp.m_colSrcStack.objPush
    (
        TFacCIDDocComp::TSrcStackItem
        {
            tCIDDocComp::ESrcTypes::HelpPage
            , pgToPush.strPageName()
            , pgToPush.strPagePath()
            , TString::strEmpty()
        }
    );
}

TCtxStackJan::TCtxStackJan(const TCppClassPage& pgToPush)
{
    facCIDDocComp.m_colSrcStack.objPush
    (
        TFacCIDDocComp::TSrcStackItem
        {
            tCIDDocComp::ESrcTypes::CppClass
            , pgToPush.strClass()
            , pgToPush.strPagePath()
            , pgToPush.strPrefix()
        }
    );
}

TCtxStackJan::TCtxStackJan(const TNamespacePage& pgToPush)
{
    facCIDDocComp.m_colSrcStack.objPush
    (
        TFacCIDDocComp::TSrcStackItem
        {
            tCIDDocComp::ESrcTypes::Namespace
            , pgToPush.strName()
            , pgToPush.strPagePath()
            , TString::strEmpty()
        }
    );
}

TCtxStackJan::~TCtxStackJan()
{
    facCIDDocComp.m_colSrcStack.TrashTop();
}



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
    , m_bPostParse(kCIDLib::False)
    , m_bVerbose(TSysInfo::bFindCmdLineParm(L"/verbose"))
    , m_colClassByName(tCIDLib::EAdoptOpts::NoAdopt)
{
}

TFacCIDDocComp::~TFacCIDDocComp()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Every time a topic pages a Cpp class, it will call here so we can add it to
//  our list that we will sort for use during output.
//
tCIDLib::TVoid TFacCIDDocComp::AddClass(TCppClassPage* const ppgToAdd)
{
    CIDAssert(!m_bPostParse, L"Only valid during the parsing phase");
    m_colClassByName.Add(ppgToAdd);
}



//
//  Called by parsing code to add an error to our error list. The error list is
//  mutable since this has to be called from some otherwise const methods as
//  well.
//
tCIDLib::TVoid
TFacCIDDocComp::AddErrorMsg(const   TString&        strMsg
                            , const MFormattable&   mfmtblToken1
                            , const MFormattable&   mfmtblToken2) const
{
    TString strErr = strMsg;
    if (!MFormattable::bIsNullObject(mfmtblToken1))
        strErr.eReplaceToken(mfmtblToken1, kCIDLib::chDigit1);
    if (!MFormattable::bIsNullObject(mfmtblToken2))
        strErr.eReplaceToken(mfmtblToken1, kCIDLib::chDigit2);

    m_colErrs.objAdd
    (
        TKeyValuePair(m_colSrcStack.objPeek().m_strPath, strErr)
    );
}


//
//  For each new file seen, this is called. We use the same parser for all of them,
//  which means that they have to first parse the files, then deal with any nested
//  references after they have fully processed the current one, because this is
//  going to whack any previous info.
//
//  This is really only an issue for topic files, which hve lists of other files to
//  parse (and sub-dirs to process.) They load up those two lists first, then they
//  continue.
//
tCIDLib::TBoolean TFacCIDDocComp::bParseXML(const TString& strFile)
{
    const tCIDLib::TBoolean bRes = m_xtprsToUse.bParseRootEntity
    (
        strFile
        , tCIDXML::EParseOpts::Validate
        , tCIDXML::EParseFlags::TagsNText
    );
    if (!bRes)
    {
        TTextStringOutStream strmFmt(1024UL);
        const TXMLTreeParser::TErrInfo& errCur = m_xtprsToUse.erriFirst();
        strmFmt << L"XML Error: "
                << errCur.strText() << kCIDLib::NewLn
                << L"    - " << errCur.c4Line()
                << kCIDLib::chPeriod << errCur.c4Column()
                << L" [" << errCur.strSystemId() << L"]";

        strmFmt.Flush();
        AddErrorMsg(strmFmt.strData());
    }
    return bRes;
}



tCIDLib::EExitCodes
TFacCIDDocComp::eMainThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    if (m_bVerbose)
        strmOut() << L"CIDLib Documentation Compiler" << kCIDLib::NewEndLn;

    // We output into a directory under the result directory
    TPathStr pathTmp;
    if (!TProcEnvironment::bFind(L"CID_RESDIR", pathTmp))
    {
        strmOut() << L"   Could not find CIDLib result path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }

    // Check any parameters
    if (!bParseParams())
        return tCIDLib::EExitCodes::BadParameters;

    // Create the target path if it doesn't exist
    pathTmp.AddLevel(L"CIDDocs.Out");
    TFileSys::MakePath(pathTmp);

    // Store it away now that it's good
    m_strTarPath = pathTmp;


    // Create the top level source directory
    if (!TProcEnvironment::bFind(L"CID_SRCTREE", pathTmp))
    {
        strmOut() << L"   Could not find CIDLib Source path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }
    pathTmp.AddLevel(L"Source");
    pathTmp.AddLevel(L"Docs");
    pathTmp.AddLevel(L"CIDDocs");

    // Make sure it exists
    if (!TFileSys::bIsDirectory(pathTmp))
    {
        strmOut()   << L"   Source path doesn't exists Path="
                    << pathTmp << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    // Store it away now
    m_strSrcPath = pathTmp;

    try
    {
        // Tell the context object to load the DTD into his XML parser
        LoadDTD(m_strSrcPath);

        // We have to create a special case root topic to start the process.
        m_cptrRoot.SetPointer
        (
            new TTopic(L"Root", L"/", TString::strEmpty(), m_strSrcPath)
        );

        // And kick off the parse of the whole hierarchy
        m_cptrRoot->Parse();

        // If no errors so far, try the output
        if (!bGotErrors())
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

            // Copy over all of the images
            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"Images");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"Images");
            if (!TFileSys::bIsDirectory(pathCopyTar))
                TFileSys::MakeSubDirectory(L"Images", m_strTarPath);
            CopyDir(pathCopySrc, pathCopyTar);

            // Do post parse processing on the contex object
            PostParseProc();

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

        // If we got any errors show them.
        if (!m_colErrs.bIsEmpty())
        {
            strmOut() << L"\nErrors occurred:\n\n";
            ShowErrors(strmOut());
			strmOut().Flush();
            return tCIDLib::EExitCodes::RuntimeError;
        }
    }

    catch(const TError& errToCatch)
    {
        strmOut()   << L"        An exception occurred. Error=\n"
                    << errToCatch<< kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        strmOut()   << L"        A system exception occurred." <<  kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    strmOut() << kCIDLib::FlushIt;
    return tCIDLib::EExitCodes::Normal;
}


// We have to do these fairly often manually, so this saves a lot of trouble
tCIDLib::TVoid
TFacCIDDocComp::FormatDeemphText(TTextOutStream& strmTar, const TString& strText)
{
    strmTar << L"<span class=\"DeemphCode\">" << strText << L"</span>";
}

tCIDLib::TVoid
TFacCIDDocComp::FormatDeemphText(       TTextOutStream& strmTar
                                , const TString&        strText1
                                , const TString&        strText2)
{
    strmTar << L"<span class=\"DeemphCode\">" << strText1 << strText2 << L"</span>";
}

tCIDLib::TVoid
TFacCIDDocComp::FormatDeemphText(       TTextOutStream&     strmTar
                                , const TString&            strText1
                                , const TString&            strText2
                                , const tCIDLib::TCh* const pszText3)
{
    strmTar << L"<span class=\"DeemphCode\">" << strText1
            << strText2 << pszText3 << L"</span>";
}


tCIDLib::TVoid
TFacCIDDocComp::FormatEmphText(TTextOutStream& strmTar, const TString& strText)
{
    strmTar << L"<span class=\"EmphCode\">" << strText << L"</span>";
}

tCIDLib::TVoid
TFacCIDDocComp::FormatEmphText(         TTextOutStream& strmTar
                                , const TString&        strText1
                                , const TString&        strText2)
{
    strmTar << L"<span class=\"EmphCode\">" << strText1 << strText2 << L"</span>";
}

tCIDLib::TVoid
TFacCIDDocComp::FormatEmphText(         TTextOutStream&     strmTar
                                , const TString&            strText1
                                , const TString&            strText2
                                , const tCIDLib::TCh* const pszText3)
{
    strmTar << L"<span class=\"EmphCode\">" << strText1 << strText2
            << pszText3 << L"</span>";
}



//
//  This is called during output any time a ClassRef node is seen, and we generate
//  a page link for that class. We can't call the page's own link generator method,
//  since that is just for generating left hand side links to load just the right
//  side. This needs to load the page but also the topic for that page.
//
tCIDLib::TVoid
TFacCIDDocComp::GenerateClassLink(TTextOutStream& strmTar, const TString& strClass) const
{
    CIDAssert(m_bPostParse, L"Online valid in post-parse phase");

    const TCppClassPage* ppgTar = ppgFindByClass(strClass);
    if (ppgTar)
    {
        strmTar << L"<a onclick=\"javascript:loadTopic('"
                << ppgTar->strParTopic()
                << L"', '/" << ppgTar->strPageName()
                << L"', true);\" href='javascript:void(0)'>"
                << strClass
                << L"</a>\n";
    }
     else
    {
        strmTar << strClass;
    }
}


//
//  This is called during output any time a FcRef node is seen, and we generate
//  a page link for that facility. This is a full load of both sides, loading the
//  overview page into the right. In this case w know the paths since they are all
//  in a single topic.
//
tCIDLib::TVoid
TFacCIDDocComp::GenerateFacLink(TTextOutStream& strmTar, const TString& strFacName) const
{
    CIDAssert(m_bPostParse, L"Online valid in post-parse phase");

    strmTar << L"<a onclick=\"javascript:loadTopic('"
                << L"/Reference/APIDocs/" << strFacName
                << L"', '/Overview"
                << L"', true);\" href='javascript:void(0)'>"
                << strFacName
                << L"</a>\n";
}


//
//  Called to load our DTD text and add it to our parser as a preloaded entity,
//  so that the individual XML files don't have to deal with pointing themselves
//  at an actual DTD file (with all of the nesting going on it's a pain.) And it
//  will speed things up as well.
//
//  But it keeps the DTD external so we can quickly make changes and test them.
//
tCIDLib::TVoid TFacCIDDocComp::LoadDTD(const TString& strSrcPath)
{
    // Build up the full path name
    TPathStr pathDTD(strSrcPath);
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
//  Find the passed class in our list. It is sorted by now so we do a binary search.
//  This is only valid for post-parse after we have sorted the list.
//
const TCppClassPage* TFacCIDDocComp::ppgFindByClass(const TString& strToFind) const
{
    CIDAssert(m_bPostParse, L"Online valid in post-parse phase");

    // Let's do a binary search for this class
    tCIDLib::TCard4 c4At;
    const TCppClassPage* ppgTar = m_colClassByName.pobjKeyedBinarySearch
    (
        strToFind
        , [](const TString& strKey, const TCppClassPage& pg2) -> tCIDLib::ESortComps
          {
              return strKey.eCompare(pg2.strClass());
          }
        , c4At
    );

    if (!ppgTar)
    {
        if (!m_bNoClassRefWarn)
            AddErrorMsg(L"The class %(1) was not found in the class list", strToFind);

        return nullptr;
    }
    return ppgTar;
}


//
//  After parsing, but before output generation, this is called to let us do
//  any required swizzling of content.
//
tCIDLib::TVoid TFacCIDDocComp::PostParseProc()
{
    m_bPostParse = kCIDLib::True;

    //
    //  Make sure the source stack is empty, since the output phase will start
    //  using it again to maintain output context.
    //
    m_colSrcStack.RemoveAll();

    // Let's sort our by class class list now that we know it's complete
    m_colClassByName.Sort
    (
        [](const TCppClassPage& pg1, const TCppClassPage& pg2)
        {
            return pg1.strClass().eCompare(pg2.strClass());
        }
    );
}


//
//  This is really ONLY for the cpp class page to call so that it can get access
//  to the current class info without having to pass it down through all of the
//  layers of the class data structures. All topics and pages push themselves,
//  well only Cpp pages during the output process, so when a cpp class file is
//  being parsed, it's own info is going to be on the top of the source stack.
//  So this is a convenient way to get to his own information.
//
const TString& TFacCIDDocComp::strCurClass() const
{
    CIDAssert(!m_colSrcStack.bIsEmpty(), L"The source stack is empty");
    const TSrcStackItem& siCur = m_colSrcStack.objPeek();
    CIDAssert
    (
        siCur.m_eType == tCIDDocComp::ESrcTypes::CppClass
        , L"The top of the stack is not a class page"
    );
    return siCur.m_strName;
}

const TString& TFacCIDDocComp::strCurClassPref() const
{
    CIDAssert(!m_colSrcStack.bIsEmpty(), L"The source stack is empty");
    const TSrcStackItem& siCur = m_colSrcStack.objPeek();
    CIDAssert
    (
        siCur.m_eType == tCIDDocComp::ESrcTypes::CppClass
        , L"The top of the stack is not a class page"
    );
    return siCur.m_strExtra;
}


// Format any errors we collected to the passed output stream
tCIDLib::TVoid TFacCIDDocComp::ShowErrors(TTextOutStream& strmOut)
{
    TString strLastSrc;
    m_colErrs.bForEach
    (
        [&strLastSrc, &strmOut](const TKeyValuePair& kvalCur)
        {
            if (kvalCur.strKey() != strLastSrc)
            {
                strmOut << L"  " << kvalCur.strKey() << kCIDLib::NewLn;
                strLastSrc = kvalCur.strKey();
            }
            strmOut << L"    " << kvalCur.strValue() << kCIDLib::NewLn;
            return kCIDLib::True;
        }
    );
    strmOut.Flush();
}



// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Parses our parameters and stores the results away
tCIDLib::TBoolean TFacCIDDocComp::bParseParams()
{
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        if (cursParms->bCompareI(L"/NoClassRefWarn"))
        {
            m_bNoClassRefWarn = kCIDLib::True;
        }
         else
        {
            strmOut()   << *cursParms << L" is not a valid command line parameter"
                        << kCIDLib::NewEndLn;
        }
    }
    return kCIDLib::True;
}


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


