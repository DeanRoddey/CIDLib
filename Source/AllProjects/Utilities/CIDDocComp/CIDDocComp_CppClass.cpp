//
// FILE NAME: CIDDocComp_cppClass.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/30/2019
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  A derivative of the base page class, for C++ classes
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
//   CLASS: TCppClassPage
//  PREFIX: pg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCppClassPage: Constructors and Destructor
// ---------------------------------------------------------------------------
TCppClassPage::TCppClassPage(   const   TString&                strExtTitle
                                , const TString&                strParSrcDir
                                , const TString&                strParTopic
                                , const TString&                strFileName) :

    TBasePage
    (
        strExtTitle
        , strParSrcDir
        , strParTopic
        , strFileName
        , kCIDDocComp::strExt_CppCPage
    )
    , m_bDuplicable(kCIDLib::False)
    , m_bFormattable(kCIDLib::False)
    , m_bStreamable(kCIDLib::False)
{
}


// ---------------------------------------------------------------------------
//  TCppClassPage: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCppClassPage::bParse(TTopic& topicParent, const TXMLTreeElement& xtnodeClass)
{
    // First we need to get out our class name and parent class name
    m_strClass = xtnodeClass.xtattrNamed(L"Class").strValue();
    m_strParClass = xtnodeClass.xtattrNamed(L"Parent").strValue();

    // Get the boolean flag attributes
    m_bDuplicable = xtnodeClass.xtattrNamed(L"Duplicable").bValueAs();
    m_bFormattable = xtnodeClass.xtattrNamed(L"Formattable").bValueAs();
    m_bStreamable = xtnodeClass.xtattrNamed(L"Streamable").bValueAs();

    return kCIDLib::True;
}


tCIDLib::TVoid TCppClassPage::OutputContent(TTextOutStream& strmTar) const
{

}
