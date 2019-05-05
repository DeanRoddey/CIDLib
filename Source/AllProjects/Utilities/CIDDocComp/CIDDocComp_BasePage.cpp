//
// FILE NAME: CIDDocComp_BasePage.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/03/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  The implementation of the abstract base page class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TBasePage
//  PREFIX: pg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBasePage: Public, static methods
// ---------------------------------------------------------------------------

//
//  Helpers to get the text content of a child of the passed parent node. This
//  is something we need to do a good bit, so it avoids a lot of grunt work in
//  the various page derivatives.
//
tCIDLib::TVoid
TBasePage::QueryElemText(const  TXMLTreeElement&    xtnodePar
                        ,       TString&            strToFill)
{
    // Get the first child of it, which should be its text content
    const TXMLTreeText& xtnodeText = xtnodePar.xtnodeChildAtAsText(0);

    // And get it's text content
    strToFill = xtnodeText.strText();
}

tCIDLib::TVoid
TBasePage::QueryElemText(const  TXMLTreeElement&    xtnodePar
                        , const TString&            strChildName
                        ,       TString&            strToFill)
{
    // Find the first child with the passed name, as a text element
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement& xtnodeChild = xtnodePar.xtnodeFindElement
    (
        strChildName, 0, c4At
    );

    // Get the first child of it, which should be its text content
    const TXMLTreeText& xtnodeText = xtnodeChild.xtnodeChildAtAsText(0);

    // And get it's text content
    strToFill = xtnodeText.strText();
}


tCIDLib::TVoid
TBasePage::QueryElemTextAt( const   TXMLTreeElement&    xtnodePar
                            , const tCIDLib::TCard4     c4Index
                            ,       TString&            strToFill)
{
    // Find the first child with the passed name, as a text element
    const TXMLTreeElement& xtnodeChild = xtnodePar.xtnodeChildAtAsElement(c4Index);

    // Get the first child of it, which should be its text content
    const TXMLTreeText& xtnodeText = xtnodeChild.xtnodeChildAtAsText(0);

    // And get it's text content
    strToFill = xtnodeText.strText();
}


// ---------------------------------------------------------------------------
//  TBasePage: Constructors and Destructor
// ---------------------------------------------------------------------------
TBasePage::~TBasePage()
{
}


// ---------------------------------------------------------------------------
//  TBasePage: Hidden constructors
// ---------------------------------------------------------------------------
TBasePage::TBasePage(const  TString&            strExtTitle
                    , const TString&            strParSrcDir
                    , const TString&            strParTopic
                    , const TString&            strFileName
                    , const TString&            strFileExt
                    , const tCIDLib::TBoolean   bVirtual) :

    m_bVirtual(bVirtual)
    , m_strExtTitle(strExtTitle)
    , m_strFileExt(strFileExt)
    , m_strFileName(strFileName)
    , m_strParSrcDir(strParSrcDir)
    , m_strParTopic(strParTopic)
{
    // Build up our page path
    m_strPagePath = strParTopic;
    m_strPagePath += kCIDLib::chForwardSlash;
    m_strPagePath += strFileName;
}


// ---------------------------------------------------------------------------
//  TBasePage: Public, virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TBasePage::bParseFile(TTopic& topicParent, TXMLTreeParser& xtprsToUse)
{
    facCIDDocComp.strmOut()
            << L"\n   Parsing File: "
            << m_strPagePath << kCIDLib::EndLn;

    // Build up our source path
    TPathStr pathSrc = m_strParSrcDir;
    pathSrc.AddLevel(m_strFileName);
    pathSrc.AppendExt(m_strFileExt);

    const tCIDLib::TBoolean bRes = xtprsToUse.bParseRootEntity
    (
        pathSrc
        , tCIDXML::EParseOpts::Validate
        , tCIDXML::EParseFlags::TagsNText
    );
    if (!bRes)
    {
        facCIDDocComp.ShowXMLParseErr(pathSrc);
        return kCIDLib::False;
    }

    const TXMLTreeElement& xtnodeRoot = xtprsToUse.xtdocThis().xtnodeRoot();

    // Get any stuff we do at this level
    QueryElemText(xtnodeRoot, kCIDDocComp::strXML_Title, m_strIntTitle);

    // If the external title was empty, then use our internal for that as well
    if (m_strExtTitle.bIsEmpty())
        m_strExtTitle = m_strIntTitle;

    // And then invoke the virtual for the derived class to parse his stuff
    return bParse(topicParent, xtnodeRoot);
}


tCIDLib::TVoid TBasePage::GenerateOutput(const TString& strParPath) const
{
    // Create our output file
    TPathStr pathTar = strParPath;
    pathTar.AddLevel(m_strFileName);
    pathTar.AppendExt(L"html");

    TTextFileOutStream strmTar
    (
        pathTar
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
        , new TUTF8Converter
    );

    // Spit out any common stuff
    strmTar << L"<!DOCTYPE html>\n"
            << L"<html>\n<body>\n"
            << L"<p><span class='PageHdr'>"
            << m_strIntTitle
            << L"</span></p>";

    // Now invoke the virtual for the derived class to do his thing
    OutputContent(strmTar);

    // And finish it off now
    strmTar << L"</body>\n</html>\n" << kCIDLib::EndLn;
}


tCIDLib::TVoid
TBasePage::GenerateTopicLink(TTextOutStream& strmTar) const
{
    // If we got alias text, we use that as the link text, else we use the page's title.
    strmTar  << L"<a onclick=\"javascript:loadRightSide('"
            << m_strParTopic
            << L"', '/" << m_strFileName
            << L"', '');\" href='javascript:void(0)'>"
            << m_strExtTitle
            << L"</a><br/>\n";
}
