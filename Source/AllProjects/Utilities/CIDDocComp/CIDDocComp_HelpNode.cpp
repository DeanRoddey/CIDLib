//
// FILE NAME: CIDDocComp_HelpNode.Cpp
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
//  The implementation the basic recursive markup containing node.
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
//   CLASS: THelpNode
//  PREFIX: hn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THelpNode: Constructors and Destructor
// ---------------------------------------------------------------------------

THelpNode::THelpNode() :

    m_c4Extra(0)
    , m_eType(tCIDDocComp::ETypes::None)
{
}

THelpNode::~THelpNode()
{
}


// ---------------------------------------------------------------------------
//  THelpNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean THelpNode::bIsEmpty() const
{
    //
    //  If this guy isn't actual content, and it holds no content. Or, it's a text node
    //  and there's no text, consider it empty.
    //
    return
    (
        m_colNodes.bIsEmpty()
        || ((m_eType == tCIDDocComp::ETypes::Text) && m_strText.bIsEmpty())
    );
}


//
//  Parse our content out from source XML. We have one that takes the XML element
//  itself and another that takes a parent node and the name of the XML element
//  to get, so we get that and call the first version.
//
tCIDLib::TBoolean
THelpNode::bParseFromParent(const   TXMLTreeElement&    xtnodePar
                            , const TString&            strName
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
    return bParse(*pxtnodeCur);
}

tCIDLib::TBoolean THelpNode::bParse(const TXMLTreeElement& xtnodeText)
{
    m_colNodes.RemoveAll();

    const tCIDLib::TCard4 c4ChildCnt = xtnodeText.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCnt; c4Index++)
    {
        const TXMLTreeNode& xtnodeCur = xtnodeText.xtnodeChildAt(c4Index);

        // Add a new node to fill in
        THelpNode& hnNew = m_colNodes.objAdd(THelpNode());

        tCIDLib::TBoolean bGetChildren = kCIDLib::True;
        if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Text)
        {
            // It's got to just be a regular text node
            hnNew.m_eType = tCIDDocComp::ETypes::Text;
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
                hnNew.m_eType = tCIDDocComp::ETypes::Bold;
            }
             else if (strQName.bCompare(L"Br"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Break;
            }
             else if (strQName.bCompare(L"Code"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Code;
            }
             else if (strQName.bCompare(L"DIV"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::DIV;
                bGetID = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Image"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Image;

                //
                //  We use the ref to hold the image path, and type to hold the
                //  wrapping style.
                //
                bGetRef = kCIDLib::True;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Indent"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Indent;
            }
             else if (strQName.bCompare(L"InlineNote"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::InlineNote;
            }
             else if (strQName.bCompare(L"Link"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Link;

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
                hnNew.m_eType = tCIDDocComp::ETypes::ListItem;

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
                hnNew.m_eType = tCIDDocComp::ETypes::List;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Note"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Note;
            }
             else if (strQName.bCompare(L"P"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Paragraph;
            }
             else if (strQName.bCompare(L"SecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::SecTitle;
            }
             else if (strQName.bCompare(L"SubSecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::SubSecTitle;
            }
             else if (strQName.bCompare(L"Superscript"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Superscript;
            }
             else if (strQName.bCompare(L"Table"))
            {
                hnNew.m_eType = tCIDDocComp::ETypes::Table;

                // Call a helper to process the class
                if (!hnNew.bProcessTable(xtnodeElem))
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
                hnNew.m_strID = xtnodeElem.xtattrNamed(kCIDDocComp::strXML_Id).strValue();

            if (bGetRef)
                hnNew.m_strRef = xtnodeElem.xtattrNamed(kCIDDocComp::strXML_Ref).strValue();

            if (bGetType)
                hnNew.m_strType = xtnodeElem.xtattrNamed(kCIDDocComp::strXML_Type).strValue();

            // If the current node has children, then recurse
            if (bGetChildren && xtnodeElem.c4ChildCount())
                hnNew.bParse(xtnodeElem);
        }
    }
    return kCIDLib::True;
}



tCIDLib::TVoid THelpNode::OutputNodes(TTextOutStream& strmTar) const
{
    const tCIDLib::TCard4 c4Count = m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnCur = m_colNodes[c4Index];

        switch(hnCur.m_eType)
        {
            case tCIDDocComp::ETypes::Bold :
            {
                strmTar << L"<B>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</B>";
                break;
            }

            case tCIDDocComp::ETypes::Break :
            {
                strmTar << L"<br/>";
                break;
            }

            case tCIDDocComp::ETypes::Code :
            {
                strmTar << L"\n<PRE>\n";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</PRE>\n";
                break;
            }

            case tCIDDocComp::ETypes::DIV :
            {
                strmTar << L"\n<DIV id='"
                        << hnCur.m_strID
                        << L"'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</DIV>\n";
                break;
            }

            case tCIDDocComp::ETypes::Image :
            {
                strmTar << L"<p></p><div style='width: 100%; text-align: center;'>"
                        << L"<img ";

                // Do any wrapping style, which we stored in the type value
                if (hnCur.m_strType == L"FloatLeft")
                    strmTar << L"style='float: left; margin-right: 12px;'";
                else if (hnCur.m_strType == L"FloatRight")
                    strmTar << L"style='float: right; margin-left: 12px'";

                // Now do the actual image ref and close it off
                strmTar << L" src='/Images/"
                        << hnCur.m_strRef
                        << L"'/></div></p>";
                break;
            }

            case tCIDDocComp::ETypes::Indent :
            {
                strmTar << L"<blockquote>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</blockquote>";
                break;
            }

            case tCIDDocComp::ETypes::Link :
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
                hnCur.OutputNodes(strmTar);
                strmTar << L"</a>";
                break;
            }

            case tCIDDocComp::ETypes::List :
            {
                if (hnCur.m_strType == L"Ordered")
                    strmTar << L"<ol>";
                else
                    strmTar << L"<ul>";
                hnCur.OutputNodes(strmTar);
                if (hnCur.m_strType == L"Ordered")
                    strmTar << L"</ol>";
                else
                    strmTar << L"</ul>";
                break;
            }

            case tCIDDocComp::ETypes::ListItem :
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
                hnCur.OutputNodes(strmTar);
                strmTar << L"</li>";
                break;
            }

            case tCIDDocComp::ETypes::Note :
            {
                strmTar << L"<div class=\"BarredNote\">";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</div><P/>";
                break;
            }

            case tCIDDocComp::ETypes::Paragraph :
            {
                strmTar << L"<p>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</p>";
                break;
            }

            case tCIDDocComp::ETypes::SecTitle :
            {
                strmTar << L"<p><span class='SecHdr'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::ETypes::SubSecTitle :
            {
                strmTar << L"<p><span class='SubSecHdr'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::ETypes::Superscript :
            {
                strmTar << L"<span class=\"Superscript\">";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span>";
                break;
            }

            case tCIDDocComp::ETypes::Text :
            {
                strmTar << hnCur.m_strText;
                break;
            }

            case tCIDDocComp::ETypes::Table :
            {
                // Call a helper for this
                hnCur.OutputTable(strmTar);
                strmTar << L"<br/>";
                break;
            }

            default :
                facCIDDocComp.strmErr()
                        << L"Unknown help text node type" << kCIDLib::EndLn;
                break;
        };
    }
}



// ---------------------------------------------------------------------------
//  THelpNode: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called from then general node processor above to handle a table, since it includes
//  a fair bit of stuff. We allow markup inside row columns, so we recurse to get the
//  column content.
//
tCIDLib::TBoolean THelpNode::bProcessTableRow(const TXMLTreeElement& xtnodeRow)
{
    const tCIDLib::TCard4 c4Count = xtnodeRow.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCol = xtnodeRow.xtnodeChildAtAsElement(c4Index);

        // Add a new node to fill in and set its type to table column
        THelpNode& hnCol = m_colNodes.objAdd(THelpNode());
        hnCol.m_eType = tCIDDocComp::ETypes::TableCol;

        // Store the column span
        hnCol.m_c4Extra = xtnodeCol.xtattrNamed(kCIDDocComp::strXML_ColSpan).c4ValueAs();

        // And recurse to get that content
        if (!hnCol.bParse(xtnodeCol))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean THelpNode::bProcessTable(const TXMLTreeElement& xtnodeTbl)
{
    // Get the class if it is present, and store it as the type of the table node
    xtnodeTbl.bAttrExists(L"Class", m_strType);

    // Now we process any defined row nodes
    const tCIDLib::TCard4 c4Count = xtnodeTbl.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeRow = xtnodeTbl.xtnodeChildAtAsElement(c4Index);

        // Add a new node to fill in and set its type to table row
        THelpNode& hnRow = m_colNodes.objAdd(THelpNode());
        hnRow.m_eType = tCIDDocComp::ETypes::TableRow;

        // And now lt's process the columns of this row
        if (!hnRow.bProcessTableRow(xtnodeRow))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  This is called from the general node output method when it sees a table. We output
//  the table contents, recursing on the columns since they can have markup.
//
tCIDLib::TVoid THelpNode::OutputTableRow(TTextOutStream& strmTar) const
{
    // Go through the columns
    const tCIDLib::TCard4 c4Count = m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnCol = m_colNodes[c4Index];
        CIDAssert(hnCol.m_eType == tCIDDocComp::ETypes::TableCol, L"Expected a table column");

        // Recurse to output the info
        if (hnCol.m_c4Extra > 1)
            strmTar << L"<td colspan=\"" << hnCol.m_c4Extra << L"\">";
        else
            strmTar << L"<td>";
        OutputNodes(strmTar);
        strmTar << L"</td>";
    }
}


tCIDLib::TVoid THelpNode::OutputTable(TTextOutStream& strmTar) const
{
    strmTar << L"<table";
    if (!m_strType.bIsEmpty())
        strmTar << L" class='" << m_strType << L"'";
    strmTar << L">";

    // Go through the rows
    const tCIDLib::TCard4 c4Count = m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnRow = m_colNodes[c4Index];
        CIDAssert(hnRow.m_eType == tCIDDocComp::ETypes::TableRow, L"Expected a table row");

        strmTar << L"<tr>";
        OutputTableRow(strmTar);
        strmTar << L"</tr>";
    }

    strmTar << L"</table>";
}

