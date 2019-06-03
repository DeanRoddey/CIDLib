//
// FILE NAME: CIDDocComp_HelpPage.Cpp
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
//  A derivative of the base page class, for general markup.
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
//   CLASS: THelpPage
//  PREFIX: pg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THelpPage: Constructors and Destructor
// ---------------------------------------------------------------------------
THelpPage::THelpPage(const  TString&            strExtTitle
                    , const TString&            strParSrcDir
                    , const TString&            strParTopic
                    , const TString&            strFileName
                    , const tCIDLib::TBoolean   bVirtual) :

    TBasePage
    (
        strExtTitle
        , strParSrcDir
        , strParTopic
        , strFileName
        , tCIDDocComp::EPageTypes::HelpPage
        , bVirtual
    )
{
}


// ---------------------------------------------------------------------------
//  THelpPage: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
THelpPage::Parse(       TTopic&             topicParent
                , const TXMLTreeElement&    xtnodeRoot
                ,       TParseCtx&          ctxToUse)
{
    // Get our inteneral title text
    QueryElemText(xtnodeRoot, kCIDDocComp::strXML_Title, m_strIntTitle);

    // The rest is all in a HelpText element, which our help node member will parse out
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement& xtnodeHelp = xtnodeRoot.xtnodeFindElement
    (
        kCIDDocComp::strXML_HelpText, 0, c4At
    );
    m_hnContent.Parse(xtnodeHelp, ctxToUse);
}


tCIDLib::TVoid THelpPage::OutputContent(TTextOutStream& strmTar) const
{
    strmTar << L"<p><span class='PageHdr'>"
            << m_strIntTitle
            << L"</span></p>";

    // The rest is all in our generic help node
    m_hnContent.OutputNodes(strmTar);
}
