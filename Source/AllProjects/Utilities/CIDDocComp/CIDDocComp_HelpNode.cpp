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
    , m_eType(tCIDDocComp::EMUTypes::None)
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
        || ((m_eType == tCIDDocComp::EMUTypes::Text) && m_strText.bIsEmpty())
    );
}


// We output a magic indenting wrapper node, then call the recursive private method
tCIDLib::TVoid
THelpNode::OutputHelpText(          TTextOutStream&     strmTar
                            , const tCIDLib::TBoolean   bIndented) const
{
    if (bIndented)
        strmTar << L"<div class=\"HelpTextCont\">";
    OutputNodes(strmTar);
    if (bIndented)
        strmTar << L"</div>";
}



//
//  Parse our content out from source XML. We have one that takes the XML element
//  itself and another that takes a parent node and the name of the XML element
//  to get, so we get that and call the first version.
//
tCIDLib::TVoid
THelpNode::ParseFromParent( const   TXMLTreeElement&    xtnodePar
                            , const TString&            strName
                            , const tCIDLib::TBoolean   bOptional)
{
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement* pxtnodeCur = xtnodePar.pxtnodeFindElement(strName, 0, c4At);
    if (!pxtnodeCur)
    {
        if (!bOptional)
        {
            facCIDDocComp.AddErrorMsg
            (
                L"Could not find element '%(1)' in parent node '%(2)'"
                , strName
                , xtnodePar.strQName()
            );
            return;
        }
        return;
    }

    // Now just call the other version and return its return
    Parse(*pxtnodeCur);
}

tCIDLib::TVoid
THelpNode::Parse(const TXMLTreeElement& xtnodeText)
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
            hnNew.m_eType = tCIDDocComp::EMUTypes::Text;
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
                hnNew.m_eType = tCIDDocComp::EMUTypes::Bold;
            }
             else if (strQName.bCompare(L"Br"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Break;
            }
             else if (strQName.bCompare(L"ClassRef"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::ClassRef;
                bGetRef = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Code"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Code;
            }
             else if (strQName.bCompare(L"DIV"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::DIV;
                bGetID = kCIDLib::True;
            }
             else if (strQName.bCompare(L"FacRef"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::FacRef;
                bGetRef = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Image"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Image;

                //
                //  We use the ref to hold the image path, and type to hold the
                //  wrapping style.
                //
                bGetRef = kCIDLib::True;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Indent"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Indent;
            }
             else if (strQName.bCompare(L"InlineNote"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::InlineNote;
            }
             else if (strQName.bCompare(L"Italic"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Italic;
            }
             else if (strQName.bCompare(L"Link"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Link;

                //
                //  We get all of the available values. For quick links or external links, we
                //  only use the ref. For page links we use the ref for the directory (topic)
                //  and the id for the actual page within that directory (starting with a slash)
                //  So we actually need to get the type here ourself, so that we know whether
                //  the id is needed or not.
                //
                hnNew.m_strType = xtnodeElem.strAttr(L"Type");
                if (hnNew.m_strType == L"Page")
                    bGetID = kCIDLib::True;
                bGetRef = kCIDLib::True;
            }
             else if (strQName.bCompare(L"LItem"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::ListItem;

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
                hnNew.m_eType = tCIDDocComp::EMUTypes::List;
                bGetType = kCIDLib::True;
            }
             else if (strQName.bCompare(L"Note"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Note;
            }
             else if (strQName.bCompare(L"P"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Paragraph;
            }
             else if (strQName.bCompare(L"SecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::SecTitle;
            }
             else if (strQName.bCompare(L"SubSecTitle"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::SubSecTitle;
            }
             else if (strQName.bCompare(L"Superscript"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Superscript;
            }
             else if (strQName.bCompare(L"Table"))
            {
                hnNew.m_eType = tCIDDocComp::EMUTypes::Table;

                // Call a helper to process the class
                hnNew.ProcessTable(xtnodeElem);

                // We have eaten the children, so don't do them below
                bGetChildren = kCIDLib::False;
            }
             else
            {
                facCIDDocComp.AddErrorMsg(L"'%(1)' is not a known markup type", strQName);
            }

            // If indicated, get some well known attributes
            if (bGetID)
                hnNew.m_strID = xtnodeElem.strAttr(kCIDDocComp::strXML_Id);

            if (bGetRef)
                hnNew.m_strRef = xtnodeElem.strAttr(kCIDDocComp::strXML_Ref);

            if (bGetType)
                hnNew.m_strType = xtnodeElem.strAttr(kCIDDocComp::strXML_Type);

            // If the current node has children, then recurse
            if (bGetChildren && xtnodeElem.c4ChildCount())
                hnNew.Parse(xtnodeElem);
        }
    }
}



// Just add create a single text node
tCIDLib::TVoid THelpNode::SetToText(const  tCIDLib::TCh* const     pszToSet)
{
    m_colNodes.RemoveAll();
    THelpNode& hnNew = m_colNodes.objAdd(THelpNode());
    hnNew.m_eType = tCIDDocComp::EMUTypes::Text;
    hnNew.m_strText = pszToSet;
}



// ---------------------------------------------------------------------------
//  THelpNode: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called from then general node processor above to handle a table, since it includes
//  a fair bit of stuff. We allow markup inside row columns, so we recurse to get the
//  column content.
//
tCIDLib::TVoid THelpNode::ProcessTableRow(const TXMLTreeElement& xtnodeRow)
{
    const tCIDLib::TCard4 c4Count = xtnodeRow.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCol = xtnodeRow.xtnodeChildAtAsElement(c4Index);

        // Add a new node to fill in and set its type to table column
        THelpNode& hnCol = m_colNodes.objAdd(THelpNode());
        hnCol.m_eType = tCIDDocComp::EMUTypes::TableCol;

        // Store the column span
        hnCol.m_c4Extra = xtnodeCol.xtattrNamed(kCIDDocComp::strXML_ColSpan).c4ValueAs();

        // And recurse to get that content
        hnCol.Parse(xtnodeCol);
    }
}


tCIDLib::TVoid THelpNode::ProcessTable(const TXMLTreeElement& xtnodeTbl)
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
        hnRow.m_eType = tCIDDocComp::EMUTypes::TableRow;

        // And now lt's process the columns of this row
        hnRow.ProcessTableRow(xtnodeRow);
    }
}


//
//  The public OutputHelpText() method puts out a top level indenting wrapper div
//  then calls this to recursively output the help nodes.
//
tCIDLib::TVoid THelpNode::OutputNodes(TTextOutStream& strmTar) const
{
    const tCIDLib::TCard4 c4Count = m_colNodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const THelpNode& hnCur = m_colNodes[c4Index];

        switch(hnCur.m_eType)
        {
            case tCIDDocComp::EMUTypes::Bold :
            {
                strmTar << L"<B>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</B>";
                break;
            }

            case tCIDDocComp::EMUTypes::Break :
            {
                strmTar << L"<br/>";
                break;
            }

            case tCIDDocComp::EMUTypes::ClassRef :
            {
                //
                //  This is not for formatting but to allow us to automatically
                //  generate links to classes that are referred to in general text.
                //  This will save a vast amount of grunt work. The facility class
                //  has a helper to build a class link from the class name.
                //
                facCIDDocComp.GenerateClassLink(strmTar, hnCur.m_strRef);
                break;
            }

            case tCIDDocComp::EMUTypes::Code :
            {
                strmTar << L"\n<PRE>\n";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</PRE>\n";
                break;
            }

            case tCIDDocComp::EMUTypes::DIV :
            {
                strmTar << L"\n<DIV id='"
                        << hnCur.m_strID
                        << L"'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</DIV>\n";
                break;
            }

            case tCIDDocComp::EMUTypes::FacRef :
            {
                //
                //  This is not for formatting but to allow us to automatically
                //  generate links to facilities that are referred to in general text.
                //  This will save a vast amount of grunt work. The facility class
                //  has a helper to build a link based on the facility name that was
                //  stored in the ref member.
                //
                facCIDDocComp.GenerateFacLink(strmTar, hnCur.m_strRef);
                break;
            }

            case tCIDDocComp::EMUTypes::Image :
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

            case tCIDDocComp::EMUTypes::Indent :
            {
                strmTar << L"<blockquote>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</blockquote>";
                break;
            }

            case tCIDDocComp::EMUTypes::Italic :
            {
                strmTar << L"<I>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</I>";
                break;
            }

            case tCIDDocComp::EMUTypes::Link :
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
                    strmTar << L"<a onclick=\"javascript:loadTopic('"
                            << hnCur.m_strRef
                            << L"', '"
                            << hnCur.m_strID
                            << L"', true);\" href='javascript:void(0)'"
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

            case tCIDDocComp::EMUTypes::List :
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

            case tCIDDocComp::EMUTypes::ListItem :
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

            case tCIDDocComp::EMUTypes::Note :
            {
                strmTar << L"<div class=\"BarredNote\">";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</div><P/>";
                break;
            }

            case tCIDDocComp::EMUTypes::Paragraph :
            {
                strmTar << L"<p>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</p>";
                break;
            }

            case tCIDDocComp::EMUTypes::SecTitle :
            {
                strmTar << L"<p><span class='SecHdr'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::EMUTypes::SubSecTitle :
            {
                strmTar << L"<p><span class='SubSecHdr'>";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span></p>";
                break;
            }

            case tCIDDocComp::EMUTypes::Superscript :
            {
                strmTar << L"<span class=\"Superscript\">";
                hnCur.OutputNodes(strmTar);
                strmTar << L"</span>";
                break;
            }

            case tCIDDocComp::EMUTypes::Text :
            {
                strmTar << hnCur.m_strText;
                break;
            }

            case tCIDDocComp::EMUTypes::Table :
            {
                // Call a helper for this
                hnCur.OutputTable(strmTar);
                strmTar << L"<br/>";
                break;
            }

            default :
                facCIDDocComp.strmOut()
                        << L"        Unknown help text node type" << kCIDLib::EndLn;
                break;
        };
    }
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
        CIDAssert(hnCol.m_eType == tCIDDocComp::EMUTypes::TableCol, L"Expected a table column");

        // Recurse to output the info
        if (hnCol.m_c4Extra > 1)
            strmTar << L"<td colspan=\"" << hnCol.m_c4Extra << L"\">";
        else
            strmTar << L"<td>";
        hnCol.OutputNodes(strmTar);
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
        CIDAssert(hnRow.m_eType == tCIDDocComp::EMUTypes::TableRow, L"Expected a table row");

        strmTar << L"<tr>";
        hnRow.OutputTableRow(strmTar);
        strmTar << L"</tr>";
    }

    strmTar << L"</table>";
}

