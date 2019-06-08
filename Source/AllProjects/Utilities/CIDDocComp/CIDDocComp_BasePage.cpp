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
TBasePage::TBasePage(const  TString&                strExtTitle
                    , const TString&                strParSrcDir
                    , const TString&                strParTopic
                    , const TString&                strFileName
                    , const tCIDDocComp::EPageTypes eType
                    , const tCIDLib::TBoolean       bVirtual) :

    m_bVirtual(bVirtual)
    , m_eType(eType)
    , m_strExtTitle(strExtTitle)
    , m_strFileName(strFileName)
    , m_strParSrcDir(strParSrcDir)
    , m_strParTopic(strParTopic)
{
    //
    //  Build up our page path. We have to special case the root, so we don't end
    //  up with double slashes.
    //
    m_strPagePath = strParTopic;
    if (m_strParTopic.chLast() != kCIDLib::chForwardSlash)
        m_strPagePath += kCIDLib::chForwardSlash;
    m_strPagePath += strFileName;
}


// ---------------------------------------------------------------------------
//  TBasePage: Public, virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TBasePage::ParseFile(TTopic& topicParent)
{
    if (facCIDDocComp.bVerbose())
        facCIDDocComp.strmOut() << L"      " << m_strPagePath << kCIDLib::EndLn;

    // Build up our source path
    TPathStr pathSrc = m_strParSrcDir;
    pathSrc.AddLevel(m_strFileName);

    // Use Load here, which gets us the text that holds the file extension
    pathSrc.AppendExt(tCIDDocComp::strLoadEPageTypes(m_eType));

    // If the parse fails, we are done for this guy, else get the root and process
    if (!facCIDDocComp.bParseXML(pathSrc))
        return;
    const TXMLTreeElement& xtnodeRoot = facCIDDocComp.xtnodeRoot();

    //
    //  Do any stuff we handled at this level. All pages can optionally have a
    //  keywords section.
    //
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement* pxtnodeKWs = xtnodeRoot.pxtnodeFindElement(L"Keywords", 0, c4At);
    if (pxtnodeKWs)
    {
        // Get the body text of this element, which is a comma separated list of phrases
        QueryElemText(*pxtnodeKWs, kCIDDocComp::strXML_Keywords, facCIDDocComp.m_strTmp1);

        if (!TStringTokenizer::bParseCSVLine(facCIDDocComp.m_strTmp1, m_colKeywords, c4At))
        {
            facCIDDocComp.AddErrorMsg
            (
                L"Invalid keyword content at offset %(1)", TCardinal(c4At)
            );
        }
    }

    // And then invoke the virtual for the derived class to parse his stuff
    Parse(topicParent, xtnodeRoot);
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
            << L"<html>\n<body>\n";

    // Now invoke the virtual for the derived class to do his thing
    OutputContent(strmTar);

    // And finish it off now
    strmTar << L"</body>\n</html>\n" << kCIDLib::EndLn;
}


tCIDLib::TVoid TBasePage::GenerateLink(TTextOutStream& strmTar) const
{
    // Generate a link to load our page content into the right hand side
    strmTar  << L"<a onclick=\"javascript:loadRightSide('"
            << m_strParTopic
            << L"', '/" << m_strFileName
            << L"', '');\" href='javascript:void(0)'>"
            << m_strExtTitle
            << L"</a><br/>\n";
}


