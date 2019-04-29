//
// FILE NAME: CIDDocComp_Pages.Cpp
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
//  The implementation of the basic page and topic classes.
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
//  Given the parent of a tree of help text, find the help text node and, if found, parse
//  it and fill in the passed help node. The caller tells us if it's optional or not. If
//  not, and we don't find it, we output an error and return false.
//
tCIDLib::TBoolean
TBasePage::bProcessHelpText(const   TXMLTreeElement&    xtnodePar
                            , const TString&            strName
                            ,       THelpNode&          hnToFill
                            , const tCIDLib::TBoolean   bOptional)
{
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement* pxtnodeCur = xtnodePar.pxtnodeFindElement(strName, 0, c4At);
    if (!pxtnodeCur)
    {
        if (!bOptional)
        {
            facCIDDocComp.strmErr() << L"Could not find element '" << strName
                                    << L"' in parent node '" << xtnodePar.strQName()
                                    << L"'"
                                    << kCIDLib::NewEndLn;
            return kCIDLib::False;
        }
        return kCIDLib::True;
    }

    // Now just call the other version and return its return
    return bProcessHelpText(*pxtnodeCur, hnToFill);
}


//
//  Given the top of a tree of help text markup, parse it and fill in the passed help
//  node.
//
tCIDLib::TBoolean
TBasePage::bProcessHelpText(const   TXMLTreeElement&    xtnodeText
                            ,       THelpNode&          hnToFill)
{
    const tCIDLib::TCard4 c4ChildCnt = xtnodeText.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCnt; c4Index++)
    {
        const TXMLTreeNode& xtnodeCur = xtnodeText.xtnodeChildAt(c4Index);

        // Add a new node to fill in
        THelpNode& hnNew = hnToFill.m_colNodes.objAdd(THelpNode());

        tCIDLib::TBoolean bGetChildren = kCIDLib::True;
        if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Text)
        {
            // It's got to just be a regular text node
            hnNew.m_eType = tCIDDocComp::EType_Text;
            hnNew.m_strText = static_cast<const TXMLTreeText&>(xtnodeCur).strText();
        }
         else if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Element)
        {
            //
            //  It's one of the special cases, so check the QName and based on
            //  that we store the info.
            //
            const TXMLTreeElement& xtnodeElem = static_cast<const TXMLTreeElement&>(xtnodeCur);
            const TString& strQName = xtnodeElem.strQName();

            tCIDLib::TBoolean bGetRef = kCIDLib::False;
            tCIDLib::TBoolean bGetID = kCIDLib::False;
            tCIDLib::TBoolean bGetType = kCIDLib::False;
            if (strQName.bCompare(L"Bold"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Bold;
            }
             else if (strQName.bCompare(L"Br"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Break;
            }
             else if (strQName.bCompare(L"Code"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Code;
            }
             else if (strQName.bCompare(L"DIV"))
            {
                hnNew.m_eType = tCIDDocComp::EType_DIV;
                bGetID = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Image"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Image;

                //
                //  We use the ref to hold the image path, and type to hold the
                //  wrapping style.
                //
                bGetRef = kCIDLib::True;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Indent"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Indent;
            }
             else if (strQName.bCompare(L"InlineNote"))
            {
                hnNew.m_eType = tCIDDocComp::EType_InlineNote;
            }
             else if (strQName.bCompare(L"Link"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Link;

                //
                //  We get all of the available values. For quick links or external links, we
                //  only use the ref. For page links we use the ref for the directory (topic)
                //  and the id for the actual page within that directory (starting with a slash)
                //  So we actually need to get the type here ourself, so that we know whether
                //  the id is needed or not.
                //
                hnNew.m_strType = xtnodeElem.xtattrNamed(L"Type").strValue();
                if (hnNew.m_strType == L"Page")
                    bGetID = kCIDLib::True;
                bGetRef = kCIDLib::True;
            }
             else if (strQName.bCompare(L"LItem"))
            {
                hnNew.m_eType = tCIDDocComp::EType_ListItem;

                //
                //  We use the Ref= attribute to hold prefix text. This will
                //  be bolded and before the rest of the list item text. We use
                //  Type= to hold the separate character if there's a ref value.
                //
                bGetRef = kCIDLib::True;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"List"))
            {
                hnNew.m_eType = tCIDDocComp::EType_List;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Note"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Note;
            }
             else if (strQName.bCompare(L"P"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Paragraph;
            }
             else if (strQName.bCompare(L"SecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::EType_SecTitle;
            }
             else if (strQName.bCompare(L"SubSecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::EType_SubSecTitle;
            }
             else if (strQName.bCompare(L"Superscript"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Superscript;
            }
             else if (strQName.bCompare(L"Table"))
            {
                hnNew.m_eType = tCIDDocComp::EType_Table;

                // Call a helper to process the class
                if (!bProcessTable(xtnodeElem, hnNew))
                    return kCIDLib::False;

                // We have eaten the children, so don't do them below
                bGetChildren = kCIDLib::False;
            }
             else
            {
                facCIDDocComp.strmOut()
                    << L"'" << strQName << L"' is not a known markup type"
                    << kCIDLib::NewEndLn;
                return kCIDLib::False;
            }

            // If indicated, get some well known attributes
            if (bGetID)
                hnNew.m_strID = xtnodeElem.xtattrNamed(TFacCIDDocComp::s_strId).strValue();

            if (bGetRef)
                hnNew.m_strRef = xtnodeElem.xtattrNamed(TFacCIDDocComp::s_strRef).strValue();

            if (bGetType)
                hnNew.m_strType = xtnodeElem.xtattrNamed(TFacCIDDocComp::s_strType).strValue();

            // If the current node has children, then recurse
            if (bGetChildren && xtnodeElem.c4ChildCount())
                bProcessHelpText(xtnodeElem, hnNew);
        }
    }
    return kCIDLib::True;
}


//
//  Called from then general node processor above to handle a table, since it includes
//  a fair bit of stuff. We allow markup inside row columns, so we recurse to get the
//  column content.
//
tCIDLib::TBoolean
TBasePage::bProcessTableRow(const TXMLTreeElement& xtnodeRow, THelpNode& hnToFill)
{
    const tCIDLib::TCard4 c4Count = xtnodeRow.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCol = xtnodeRow.xtnodeChildAtAsElement(c4Index);

        // Add a new node to fill in and set its type to table column
        THelpNode& hnCol = hnToFill.m_colNodes.objAdd(THelpNode());
        hnCol.m_eType = tCIDDocComp::EType_TableCol;

        // Store the column span
        hnCol.m_c4Extra = xtnodeCol.xtattrNamed(TFacCIDDocComp::s_strColSpan).c4ValueAs();

        // And recurse to get that content
        if (!bProcessHelpText(xtnodeCol, hnCol))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TBasePage::bProcessTable(const TXMLTreeElement& xtnodeTbl, THelpNode& hnToFill)
{
    // Get the class if it is present, and store it as the type of the table node
    xtnodeTbl.bAttrExists(L"Class", hnToFill.m_strType);

    // Now we process any defined row nodes
    const tCIDLib::TCard4 c4Count = xtnodeTbl.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeRow = xtnodeTbl.xtnodeChildAtAsElement(c4Index);

        // Add a new node to fill in and set its type to table row
        THelpNode& hnRow = hnToFill.m_colNodes.objAdd(THelpNode());
        hnRow.m_eType = tCIDDocComp::EType_TableRow;

        // And now lt's process the columns of this row
        if (!bProcessTableRow(xtnodeRow, hnRow))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  For each output file, this is called to recursively iterate the help text
//  nodes and spit out the appropriate stuff.
//
tCIDLib::TVoid
TBasePage::OutputNodes(         TTextOutStream&     strmTar
                        , const THelpNode&          hnPar
                        , const TString&            strKey)
{
    const tCIDLib::TCard4 c4Count = hnPar.m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnCur = hnPar.m_colNodes[c4Index];

        switch(hnCur.m_eType)
        {
            case tCIDDocComp::EType_Bold :
            {
                strmTar << L"<B>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</B>";
                break;
            }

            case tCIDDocComp::EType_Break :
            {
                strmTar << L"<br/>";
                break;
            }

            case tCIDDocComp::EType_Code :
            {
                strmTar << L"\n<PRE>\n";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</PRE>\n";
                break;
            }

            case tCIDDocComp::EType_DIV :
            {
                strmTar << L"\n<DIV id='"
                        << hnCur.m_strID
                        << L"'>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</DIV>\n";
                break;
            }

            case tCIDDocComp::EType_Image :
            {
                strmTar << L"<p></p><div style='width: 100%; text-align: center;'>"
                        << L"<img ";

                // Do any wrapping style, which we stored in the type value
                if (hnCur.m_strType == L"FloatLeft")
                    strmTar << L"style='float: left; margin-right: 12px;'";
                else if (hnCur.m_strType == L"FloatRight")
                    strmTar << L"style='float: right; margin-left: 12px'";

                // Now do the actual image ref and close it off
                strmTar << L" src='/Web2/CQCDocs/Images/"
                        << hnCur.m_strRef
                        << L"'/></div></p>";
                break;
            }

            case tCIDDocComp::EType_Indent :
            {
                strmTar << L"<blockquote>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</blockquote>";
                break;
            }

            case tCIDDocComp::EType_Link :
            {
                // Depending on type, add other attributes
                if (hnCur.m_strType == L"QLink")
                {
                    //
                    //  It's a quick link. We just have a javascript function
                    //  that watches for this class and does the right thing.
                    //
                    //
                    //  For now we disable these since they aren't implemented.
                    //
                    #if 0
                    strmTar << L"<a href='"
                            << hnCur.m_strRef
                            << L".html'"
                            << L" class='QLink'";
                    #endif
                }
                 else if (hnCur.m_strType == L"Page")
                {
                    strmTar << L"<a onclick=\"javascript:loadDynDiv('"
                            << hnCur.m_strRef
                            << L"', '"
                            << hnCur.m_strID
                            << L"');\" href='javascript:void(0)'"
                            << L">";
                }
                 else if (hnCur.m_strType == L"Extern")
                {
                    // Force it to a new tab/window
                    strmTar << L"<a href='"
                            << hnCur.m_strRef
                            << L"'"
                            << L" target='_blank'"
                            << L">";
                }

                // Output any nested content
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</a>";
                break;
            }

            case tCIDDocComp::EType_List :
            {
                if (hnCur.m_strType == L"Ordered")
                    strmTar << L"<ol>";
                else
                    strmTar << L"<ul>";
                OutputNodes(strmTar, hnCur, strKey);
                if (hnCur.m_strType == L"Ordered")
                    strmTar << L"</ol>";
                else
                    strmTar << L"</ul>";
                break;
            }

            case tCIDDocComp::EType_ListItem :
            {
                strmTar << L"<li>";
                // If there's prefix text, do that
                if (!hnCur.m_strRef.bIsEmpty())
                {
                    strmTar << L"<span class=\"ListItemPref\">" << hnCur.m_strRef;
                    if (hnCur.m_strType == L"Colon")
                        strmTar << L" : ";
                    else if (hnCur.m_strType == L"Dash")
                        strmTar << L" - ";
                    else if (hnCur.m_strType == L"Equals")
                        strmTar << L" = ";
                    else if (hnCur.m_strType == L"Period")
                        strmTar << L". ";
                    strmTar << L"&nbsp;</span>";
                }
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</li>";
                break;
            }

            case tCIDDocComp::EType_Note :
            {
                strmTar << L"<div class=\"BarredNote\">";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</div><P/>";
                break;
            }

            case tCIDDocComp::EType_Paragraph :
            {
                strmTar << L"<p>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</p>";
                break;
            }

            case tCIDDocComp::EType_SecTitle :
            {
                strmTar << L"<p><span class='SecHdr'>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::EType_SubSecTitle :
            {
                strmTar << L"<p><span class='SubSecHdr'>";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::EType_Superscript :
            {
                strmTar << L"<span class=\"Superscript\">";
                OutputNodes(strmTar, hnCur, strKey);
                strmTar << L"</span>";
                break;
            }

            case tCIDDocComp::EType_Text :
            {
                strmTar << hnCur.m_strText;
                break;
            }

            case tCIDDocComp::EType_Table :
            {
                // Call a helper for this
                OutputTable(strmTar, hnCur, strKey);
                strmTar << L"<br/>";
                break;
            }

            default :
                facCIDDocComp.strmErr()
                        << L"Unknown help text node type in file "
                        << strKey
                        << kCIDLib::EndLn;
                break;
        };
    }
}


//
//  This is called from the general node output method when it sees a table. We output the
//  table contents, recursing on the columns since they can have markup.
//
tCIDLib::TVoid
TBasePage::OutputTableRow(          TTextOutStream&     strmTar
                            , const THelpNode&          hnRow
                            , const TString&            strKey)
{
    // Go through the columns
    const tCIDLib::TCard4 c4Count = hnRow.m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnCol = hnRow.m_colNodes[c4Index];
        CIDAssert(hnCol.m_eType == tCIDDocComp::EType_TableCol, L"Expected a table column");

        // Recurse to output the info
        if (hnCol.m_c4Extra > 1)
            strmTar << L"<td colspan=\"" << hnCol.m_c4Extra << L"\">";
        else
            strmTar << L"<td>";
        OutputNodes(strmTar, hnCol, strKey);
        strmTar << L"</td>";
    }
}


tCIDLib::TVoid
TBasePage::OutputTable(         TTextOutStream&     strmTar
                        , const THelpNode&          hnTbl
                        , const TString&            strKey)
{
    strmTar << L"<table";
    if (!hnTbl.m_strType.bIsEmpty())
        strmTar << L" class='" << hnTbl.m_strType << L"'";
    strmTar << L">";

    // Go through the rows
    const tCIDLib::TCard4 c4Count = hnTbl.m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnRow = hnTbl.m_colNodes[c4Index];
        CIDAssert(hnRow.m_eType == tCIDDocComp::EType_TableRow, L"Expected a table row");

        strmTar << L"<tr>";
        OutputTableRow(strmTar, hnRow, strKey);
        strmTar << L"</tr>";
    }

    strmTar << L"</table>";
}


//
//  Helpers to get the text content of a child of the passed parent node. This
//  is something we need to do a good bit, so it avoids a lot of grunt work.
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


// To allow for use in keyed collections
const TString& TBasePage::strKey(const TBasePage& pgSrc)
{
    return pgSrc.m_strPath;
}


// ---------------------------------------------------------------------------
//  TBasePage: Constructors and Destructor
// ---------------------------------------------------------------------------
TBasePage::TBasePage()
{
}


TBasePage::~TBasePage()
{
}


// ---------------------------------------------------------------------------
//  TBasePage: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TBasePage::bParseXML(const  TXMLTreeElement&    xtnodePar
                    , const TString&            strName
                    , const TTopic&             topicPar)
{
    // Query the topic title. which goes at the top of the page
    QueryElemText(xtnodePar, TFacCIDDocComp::s_strTitle, m_strTitle);

    // Build up our key based on our parent topic's key and our file name
    m_strPath = topicPar.m_strTopicPath;
    if (m_strPath.chLast() != kCIDLib::chForwardSlash)
        m_strPath.Append(kCIDLib::chForwardSlash);
    m_strPath.Append(strName);

    m_strFileName = strName;

    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//   CLASS: THelpPage
//  PREFIX: pg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THelpPage: Constructors and Destructor
// ---------------------------------------------------------------------------
THelpPage::THelpPage()
{
}

THelpPage::~THelpPage()
{
}


// ---------------------------------------------------------------------------
//  THelpPage: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
THelpPage::bParseXML(const  TXMLTreeElement&    xtnodePar
                    , const TString&            strName
                    , const TTopic&             topicPar)
{
    // Call our parent first
    if (!TBasePage::bParseXML(xtnodePar, strName, topicPar))
        return kCIDLib::False;

    // This is a standard page, so find the help text node and process it
    return bProcessHelpText(xtnodePar, L"HelpText", m_hnText, kCIDLib::False);
}


tCIDLib::TVoid THelpPage::GenerateOutput(TTextOutStream& strmTar) const
{
    // Put out the doctype and main page title
    strmTar << L"<!DOCTYPE html>\n"
               L"<span class='PageHdr'>";
    strmTar << m_strTitle;
    strmTar << L"</span><p></p>";

    //
    //  For this type of page, we just have to do the node output of our text
    //  member.
    //
    OutputNodes(strmTar, m_hnText, m_strPath);
}



// ---------------------------------------------------------------------------
//   CLASS: TPageInfo
//  PREFIX: tpage
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPageInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TPageInfo::TPageInfo() :

    m_bVirtual(kCIDLib::False)
    , m_ppgThis(nullptr)
{
}

TPageInfo::~TPageInfo()
{
}


// ---------------------------------------------------------------------------
//  TPageInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPageInfo::Complete(const TString& strParPath)
{
    m_strTopicPath = strParPath;
    m_strPath = strParPath;

    // It could be the root, in which case it ends with a slash so don't add one
    if (strParPath.chLast() != kCIDLib::chForwardSlash)
        m_strPath.Append(kCIDLib::chForwardSlash);

    m_strPath.Append(m_strFile);
}


//
//  We don't reset everything. Some stuff has to be set every time, and we'd fail if
//  it weren't. So just reset optional bits, and the page pointer for safety.
//
tCIDLib::TVoid TPageInfo::Reset()
{
    m_bVirtual = kCIDLib::False;
    m_ppgThis = nullptr;
    m_strExtra1.Clear();
    m_strExtra2.Clear();
}



// ---------------------------------------------------------------------------
//   CLASS: TSubTopicInfo
//  PREFIX: tpage
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSubTopicInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TSubTopicInfo::TSubTopicInfo()
{
}

TSubTopicInfo::~TSubTopicInfo()
{
}


// ---------------------------------------------------------------------------
//  TPageInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSubTopicInfo::Complete(const TString& strParPath)
{
    m_strPath = strParPath;

    // It could be the root, in which case it ends with a slash so don't add one
    if (strParPath.chLast() != kCIDLib::chForwardSlash)
        m_strPath.Append(kCIDLib::chForwardSlash);

    m_strPath.Append(m_strSubDir);
}




// ---------------------------------------------------------------------------
//   CLASS: TTopic
//  PREFIX: topic
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTopic: Constructors and Destructor
// ---------------------------------------------------------------------------
TTopic::TTopic() :

    m_colPageMap(tCIDLib::EAdoptOpts::NoAdopt)
    , m_colParsedPages
      (
        tCIDLib::EAdoptOpts::Adopt
        , 109
        , new TStringKeyOps(kCIDLib::False)
        , &TBasePage::strKey
      )
    , m_colChildTopicMap(tCIDLib::EAdoptOpts::Adopt)
{
}

TTopic::~TTopic()
{
}


// ---------------------------------------------------------------------------
//  TTopic: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Find the main topic page for this topic. If there isn't a specific one, then we
//  take the first page.
//
const TPageInfo& TTopic::tpageTopicPage(tCIDLib::TCard4& c4At) const
{
    c4At = 0;

    // If we had no set title page, then return the 0th page
    if (m_strTopicPage.bIsEmpty())
        return *m_colPageMap[0];

    // Else look for the one whose title matches our set topic page title.
    const tCIDLib::TCard4 c4PgCnt = m_colPageMap.c4ElemCount();
    while (c4At < c4PgCnt)
    {
        const TPageInfo& tpageCur = *m_colPageMap[c4At];
        if (tpageCur.m_strFile == m_strTopicPage)
            break;

        c4At++;
    }

    if (c4At == c4PgCnt)
    {
        // This is an error
        facCIDDocComp.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDDocsErrs::errcSrc_TopicPgNotFound
            , m_strTopicPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Return the page we found
    return *m_colPageMap[c4At];
}



//
//  Th is called to generate topic oriented output at this topic level, to the
//  indicated target directory, which the caller has made sure exists. We first
//  do pages at our own level, and generate the topic index for this topic. Then
//  we recurse on sub-topic directories.
//
tCIDLib::TVoid
TTopic::GenerateOutput( const   TTopic&     topicPar
                        , const TString&    strTarDir) const
{
    TPathStr pathTar;

    // Get our counts of contained content. If none, do nothing
    const tCIDLib::TCard4 c4PgCount = m_colParsedPages.c4ElemCount();
    const tCIDLib::TCard4 c4SubCount = m_colChildTopicMap.c4ElemCount();
    if (!c4PgCount && !c4SubCount)
        return;

    // Generate the topic index for this topic
    {
        pathTar = strTarDir;
        pathTar.AddLevel(L"CQCTopicIndex");
        pathTar.AppendExt(L"html");

        TTextFileOutStream strmOutFl
        (
            pathTar
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Write
            , new TUTF8Converter
        );

        GenerateTopicIndex(topicPar, strmOutFl);
        strmOutFl   << kCIDLib::EndLn;
    }


    //
    //  And now let's generate any files at this level. IN this case we
    //
    TParsedPageList::TCursor cursPages(&m_colParsedPages);
    if (cursPages.bReset())
    {
        do
        {
           // Get this page and spit out the output
            const TBasePage& pgCur = cursPages.objRCur();
            {
                pathTar = strTarDir;
                pathTar.AddLevel(pgCur.m_strFileName);
                pathTar.AppendExt(L"html");

                TTextFileOutStream strmOutFl
                (
                    pathTar
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Excl_Write
                    , new TUTF8Converter
                );

                pgCur.GenerateOutput(strmOutFl);
                strmOutFl   << kCIDLib::EndLn;
            }
        }   while (cursPages.bNext());
    }


    // Now let's recurse on any sub-topics
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SubCount; c4Index++)
    {
        const TTopic& topicCur = *m_colChildTopicMap[c4Index];

        //
        //  Make sure the output subdirectory exists
        //
        if (!TFileSys::bExists(strTarDir, topicCur.m_strDirName, tCIDLib::EDirSearchFlags::NormalDirs))
            TFileSys::MakeSubDirectory(topicCur.m_strDirName, strTarDir);

        // And now recurse
        pathTar = strTarDir;
        pathTar.AddLevel(topicCur.m_strDirName);
        topicCur.GenerateOutput(*this, pathTar);
    }

}


//
//  The GenerateOutput() method above calls us here to generate the topic index file for
//  this topic. We spit out links for going back to the previous topic, for any topic
//  files at this current level (to load to the right side div) and any sub-topics.
//
tCIDLib::TVoid
TTopic::GenerateTopicIndex( const   TTopic&         topicPar
                            ,       TTextOutStream& strmOutFl) const
{
    // Do the doctype
    strmOutFl << L"<!DOCTYPE html>\n";

    // If we have a main topic page, then let's do it first
    if (!m_strTopicPage.bIsEmpty())
    {
        tCIDLib::TCard4 c4At;
        const TPageInfo& tpageMain = tpageTopicPage(c4At);
        GenPageLink(*this, tpageMain, strmOutFl);

        // We want a break after this one if there's more than one
        if (m_colPageMap.c4ElemCount() > 1)
            strmOutFl << L"<br/>";
    }

    //
    //  Process the page map, outputting a link for each entry, skipping the main
    //  topic page this time.
    //
    const tCIDLib::TCard4 c4MapCnt = m_colPageMap.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MapCnt; c4Index++)
    {
        const TPageInfo& tpageCur = *m_colPageMap[c4Index];

        // If not the topic page, then generate a link
        if (!tpageCur.m_strFile.bCompareI(m_strTopicPage))
            GenPageLink(*this, tpageCur, strmOutFl);
    }


    //
    //  Now go through all of the sub-topics and do links that load the topic index for the
    //  topic into the left side, and the main topic page for the topic on the right.
    //
    //
    const tCIDLib::TCard4 c4SubCnt = m_colChildTopicMap.c4ElemCount();
    if (c4SubCnt)
    {
        // Space it down from the file list
        strmOutFl << L"<br/><br/>";

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SubCnt; c4Index++)
        {
            const TTopic& topicCur = *m_colChildTopicMap[c4Index];

            // Skip the quick links
            if (topicCur.m_strTopicPath == L"/QL")
                continue;

            GenTopicLink(topicCur, strmOutFl);
        }
    }

    //
    //  If this is not the top level topic, or a quick link, then put out an up level link
    //  to go back to the previous topic. This avoids them having to do multiple back cmds to
    //  go back through all the stuff at this level before they can go back up to a previous
    //  level.
    //
    if ((m_strTopicPath != L"/") && (m_strTopicPath != L"/QL"))
    {
        // Get the topic page for the parent topic
        tCIDLib::TCard4 c4At;
        const TPageInfo& tpageTopic = topicPar.tpageTopicPage(c4At);

        //
        //  And now we can spit out the link. It invokes a javacript function
        //  that knows it is getting a topic level link.
        //
        strmOutFl   << L"<br/><a onclick=\"javascript:loadDynDiv('"
                    << topicPar.m_strTopicPath
                    << L"', '"
                    << tpageTopic.m_strFile
                    << L"');\" href='javascript:void(0)'>Up</a><p/>";
    }
}



// ---------------------------------------------------------------------------
//  TTopic: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper to generate a current topic page link, which just loads a new page
//  to the right side.
//
tCIDLib::TVoid
TTopic::GenPageLink(const   TTopic&         topicPar
                    , const TPageInfo&      tpageSrc
                    ,       TTextOutStream& strmTar) const
{
    // If we got alias text, we use that as the link text, else we use the page's title.
    strmTar << L"<a onclick=\"javascript:loadRightSide('"
            << topicPar.m_strTopicPath
            << L"', '/" << tpageSrc.m_strFile
            << L"', '');\" href='javascript:void(0)'>"
            << tpageSrc.m_strTitle
            << L"</a><br/>\n";
}



//
//  A helper to generate a topic link. We get the topic to generate the link
//  to. We have to figure out the correct initial page.
//
tCIDLib::TVoid
TTopic::GenTopicLink(const  TTopic&         topicTar
                    ,       TTextOutStream& strmTar) const
{
    TString strPageFile;

    // Find the main topic page for the target topic
    tCIDLib::TCard4 c4At;
    const TPageInfo& tpageTopic = topicTar.tpageTopicPage(c4At);

    strmTar << L"<a onclick=\"javascript:loadDynDiv('"
            << topicTar.m_strTopicPath
            << L"', '/"
            << tpageTopic.m_strFile
            << L"');\" href='javascript:void(0)'>"
            << topicTar.m_strTitle
            << L"</a><br/>\n";
}

