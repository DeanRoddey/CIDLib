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
THelpPage::THelpPage(const  TString&                strTitle
                    , const TString&                strParSrcDir
                    , const TString&                strParTopic
                    , const TString&                strFileName
                    , const TString&                strFileExt
                    , const tCIDLib::TBoolean       bVirtual) :

    TBasePage(strTitle, strParSrcDir, strParTopic, strFileName, strFileExt, bVirtual)
{
}

THelpPage::~THelpPage()
{
}


// ---------------------------------------------------------------------------
//  THelpPage: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
THelpPage::bParse(TTopic& topicParent, const TXMLTreeElement& xtnodeRoot)
{
    // For us it's all in a HelpText element, which our help node member will parse out
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement& xtnodeHelp = xtnodeRoot.xtnodeFindElement
    (
        kCIDDocComp::strXML_HelpText, 0, c4At
    );
    return m_hnContent.bParse(xtnodeHelp);
}


tCIDLib::TVoid THelpPage::OutputContent(TTextOutStream& strmTar) const
{
    // WE just delegate to our help text node
    m_hnContent.OutputNodes(strmTar);
}
