//
// FILE NAME: CIDXML_SimpleTree.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/02/2000
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
//  This file implements the classes used by the simple tree parser.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLTreeAttr,TObject)
RTTIDecls(TXMLTreeNode,TObject)
RTTIDecls(TXMLTreeComment,TXMLTreeNode)
RTTIDecls(TXMLTreeElement,TXMLTreeNode)
RTTIDecls(TXMLTreePI,TXMLTreeNode)
RTTIDecls(TXMLTreeText,TXMLTreeNode)
RTTIDecls(TXMLTreeDocument,TObject)
RTTIDecls(TTreeAttrCursor,TObject)
RTTIDecls(TTreeChildCursor,TObject)






// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeNode
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeNode: Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeNode::~TXMLTreeNode()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDXML::ENodeTypes TXMLTreeNode::eType() const
{
    return m_eType;
}


// ---------------------------------------------------------------------------
//  TXMLTreeNode: Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeNode::TXMLTreeNode(const tCIDXML::ENodeTypes eType) :

    m_eType(eType)
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeNode: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeNode::FormatTo(TTextOutStream& strmDest) const
{
    PrintTo(strmDest, 0, kCIDLib::True);
}



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeAttr
//  PREFIX: xtattr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeAttr: Destructor
// ---------------------------------------------------------------------------
TXMLTreeAttr::~TXMLTreeAttr()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeAttr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLTreeAttr::bExplicit() const
{
    return m_bExplicit;
}


tCIDLib::TBoolean TXMLTreeAttr::bValueAs() const
{
    tCIDLib::TBoolean bRet;
    if (m_strValue.bCompareI(L"true") || m_strValue.bCompareI(L"yes"))
    {
        bRet = kCIDLib::True;
    }
    else if (m_strValue.bCompareI(L"false") || m_strValue.bCompareI(L"no"))
    {
        bRet = kCIDLib::False;
    }
     else
    {
        // Make the compiler happy
        bRet = kCIDLib::False;

        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValAs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TBoolean")
        );
    }
    return bRet;
}


tCIDLib::TCard1 TXMLTreeAttr::c1ValueAs(const tCIDLib::ERadices eRadix) const
{
    // Call the Card4 version and just range check it
    tCIDLib::TCard4 c4Val = c4ValueAs(eRadix);
    if (c4Val > kCIDLib::c1MaxCard)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TCard1")
        );
    }
    return tCIDLib::TCard1(c4Val);
}

tCIDLib::TCard2 TXMLTreeAttr::c2ValueAs(const tCIDLib::ERadices eRadix) const
{
    // Call the Card4 version and just range check it
    tCIDLib::TCard4 c4Val = c4ValueAs(eRadix);
    if (c4Val > kCIDLib::c2MaxCard)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TCard2")
        );
    }
    return tCIDLib::TCard2(c4Val);
}

tCIDLib::TCard4 TXMLTreeAttr::c4ValueAs(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary
    (
        m_strValue.pszBuffer(), bValid, eRadix
    );

    if (!bValid)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValAs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TCard4")
        );
    }
    return c4Ret;
}


tCIDLib::TCard8 TXMLTreeAttr::c8ValueAs(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard8 c8Ret = TRawStr::c8AsBinary
    (
        m_strValue.pszBuffer(), bValid, eRadix
    );

    if (!bValid)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValAs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TCard8")
        );
    }
    return c8Ret;
}


tCIDXML::EAttrTypes TXMLTreeAttr::eType() const
{
    return m_eType;
}


tCIDLib::TFloat8 TXMLTreeAttr::f8ValueAs() const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TFloat8 f8Ret = TRawStr::f8AsBinary(m_strValue.pszBuffer(), bValid);
    if (!bValid)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValAs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TFloat8")
        );
    }
    return f8Ret;
}


tCIDLib::TInt1 TXMLTreeAttr::i1ValueAs() const
{
    // Call the Int4 version and just range check it
    tCIDLib::TInt4 i4Val = i4ValueAs();
    if ((i4Val < kCIDLib::i1MinInt) || (i4Val > kCIDLib::i1MaxInt))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TInt1")
        );
    }
    return tCIDLib::TInt1(i4Val);
}

tCIDLib::TInt2 TXMLTreeAttr::i2ValueAs() const
{
    // Call the Int4 version and just range check it
    tCIDLib::TInt4 i4Val = i4ValueAs();
    if ((i4Val < kCIDLib::i2MinInt) || (i4Val > kCIDLib::i2MaxInt))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TInt2")
        );
    }
    return tCIDLib::TInt2(i4Val);
}

tCIDLib::TInt4 TXMLTreeAttr::i4ValueAs() const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TInt4 i4Ret = TRawStr::i4AsBinary
    (
        m_strValue.pszBuffer(), bValid, tCIDLib::ERadices::Auto
    );

    if (!bValid)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_AttrValAs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strValue
            , m_strQName
            , TString(L"tCIDLib::TInt4")
        );
    }
    return i4Ret;
}


const TString& TXMLTreeAttr::strBaseName() const
{
    return m_strBaseName;
}


const TString& TXMLTreeAttr::strPrefix() const
{
    return m_strPrefix;
}


const TString& TXMLTreeAttr::strQName() const
{
    // Fault it in if not already
    if (m_strQName.bIsEmpty())
    {
        m_strQName = m_strPrefix;
        if (!m_strQName.bIsEmpty())
            m_strQName.Append(L":");
        m_strQName.Append(m_strBaseName);
    }
    return m_strQName;
}


const TString& TXMLTreeAttr::strURI() const
{
    return m_strURI;
}


const TString& TXMLTreeAttr::strValue() const
{
    return m_strValue;
}



// ---------------------------------------------------------------------------
//  TXMLTreeAttr: Hidden Constructors
// ---------------------------------------------------------------------------
TXMLTreeAttr::TXMLTreeAttr(const TXMLAttr& xattrSrc) :

    m_bExplicit(xattrSrc.bExplicit())
    , m_eType(xattrSrc.eType())
    , m_strBaseName(xattrSrc.strBaseName())
    , m_strPrefix(xattrSrc.strPrefix())
    , m_strQName(xattrSrc.strQName())
    , m_strURI(xattrSrc.strURI())
    , m_strValue(xattrSrc.strValue())
{
}



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeComment
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeComment: Destructor
// ---------------------------------------------------------------------------
TXMLTreeComment::~TXMLTreeComment()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeComment: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TXMLTreeComment::strText() const
{
    return m_strText;
}


// ---------------------------------------------------------------------------
//  TXMLTreeComment: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeComment::PrintTo(       TTextOutStream&     strmTarget
                        , const tCIDLib::TCard4     c4Indent
                        , const tCIDLib::TBoolean   ) const
{
    strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                << L"<!--" << m_strText << L"-->\n";
}


// ---------------------------------------------------------------------------
//  TXMLTreeComment: Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeComment::TXMLTreeComment(const  tCIDLib::TCh* const pszText) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Comment)
    , m_strText(pszText)
{
}

TXMLTreeComment::TXMLTreeComment(const TString& strText) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Comment)
    , m_strText(strText)
{
}





// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDecl
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeDecl: Destructor
// ---------------------------------------------------------------------------
TXMLTreeDecl::~TXMLTreeDecl()
{
}

// ---------------------------------------------------------------------------
//  Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeDecl::PrintTo(       TTextOutStream&     strmTarget
                                    , const tCIDLib::TCard4     c4Indent
                                    , const tCIDLib::TBoolean   ) const
{
    strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                << L"<?xml version='" << m_strVersion << L"'";

    if (!m_strEncoding.bIsEmpty())
        strmTarget << L" encoding='" << m_strEncoding << L"'";

    if (!m_strStandalone.bIsEmpty())
        strmTarget << L" standalone='" << m_strStandalone << L"'";

    strmTarget << L" ?>\n";
}

// ---------------------------------------------------------------------------
//  Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeDecl::TXMLTreeDecl( const   TString&    strVersion
                            , const TString&    strEncoding
                            , const TString&    strStandalone) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Decl)
    , m_strVersion(strVersion)
    , m_strEncoding(strEncoding)
    , m_strStandalone(strStandalone)
{
}

TXMLTreeDecl::TXMLTreeDecl( const   tCIDLib::TCh* const pszVersion
                            , const tCIDLib::TCh* const pszEncoding
                            , const tCIDLib::TCh* const pszStandalone) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Decl)
    , m_strVersion(pszVersion)
    , m_strEncoding(pszEncoding)
    , m_strStandalone(pszStandalone)
{
}




// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDTD
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeDTD: Destructor
// ---------------------------------------------------------------------------
TXMLTreeDTD::~TXMLTreeDTD()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeDTD: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TXMLTreeDTD::strPublicId() const
{
    return m_strPublicId;
}

const TString& TXMLTreeDTD::strRootElem() const
{
    return m_strRootElem;
}

const TString& TXMLTreeDTD::strSystemId() const
{
    return m_strSystemId;
}


// ---------------------------------------------------------------------------
//  Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeDTD::PrintTo(        TTextOutStream&     strmTarget
                                    , const tCIDLib::TCard4     c4Indent
                                    , const tCIDLib::TBoolean   ) const
{
    // <TBD> We should be honoring the escape flag here?
    strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                << L"<!DOCTYPE " << m_strRootElem;

    const tCIDLib::TBoolean bPubId = !m_strPublicId.bIsEmpty();
    const tCIDLib::TBoolean bSysId = !m_strSystemId.bIsEmpty();
    if (bPubId || bSysId)
    {
        if (bPubId)
        {
            strmTarget << L" PUBLIC '" << m_strPublicId << L"'";
            if (bSysId)
                strmTarget << L" '" << m_strSystemId << L"'";
        }
         else
        {
            strmTarget << L" SYSTEM '" << m_strSystemId << L"'";
        }
    }
    strmTarget << L">\n";
}


// ---------------------------------------------------------------------------
//  Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeDTD::TXMLTreeDTD(const  TString&    strRootElem
                        , const TString&    strPublicId
                        , const TString&    strSystemId) :

    TXMLTreeNode(tCIDXML::ENodeTypes::DTD)
    , m_strRootElem(strRootElem)
    , m_strPublicId(strPublicId)
    , m_strSystemId(strSystemId)
{
}

TXMLTreeDTD::TXMLTreeDTD(const  tCIDLib::TCh* const pszRootElem
                        , const tCIDLib::TCh* const pszPublicId
                        , const tCIDLib::TCh* const pszSystemId) :

    TXMLTreeNode(tCIDXML::ENodeTypes::DTD)
    , m_strRootElem(pszRootElem)
    , m_strPublicId(pszPublicId)
    , m_strSystemId(pszSystemId)
{
}



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeElement
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeElement: Destructor
// ---------------------------------------------------------------------------
TXMLTreeElement::~TXMLTreeElement()
{
    delete m_pcolAttrs;

    // Release childen first appropriately
    ReleaseChildren();
    delete m_pcolChildren;
}


// ---------------------------------------------------------------------------
//  TXMLTreeElement: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeElement::PrintTo(       TTextOutStream&     strmTarget
                        , const tCIDLib::TCard4     c4Indent
                        , const tCIDLib::TBoolean   bEscape) const
{
    strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                << L"<" << m_strQName;

    tCIDLib::TCard4 c4Count = c4AttrCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeAttr* pxtattrCur = m_pcolAttrs->pobjAt(c4Index);
            strmTarget << L" " << pxtattrCur->strQName() << L"=\"";

            if (bEscape)
            {
                facCIDXML().EscapeFor
                (
                    pxtattrCur->strValue(), strmTarget, tCIDXML::EEscTypes::Attribute
                );
            }
             else
            {
                strmTarget << pxtattrCur->strValue();
            }

            strmTarget << L"\"";
        }
    }

    c4Count = c4ChildCount();
    if (c4Count)
    {
        strmTarget << L">\n";

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            m_pcolChildren->pobjAt(c4Index)->PrintTo(strmTarget, c4Indent + 1, bEscape);

        strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                    << L"</" << m_strQName << L">\n";
    }
     else
    {
        strmTarget << L"/>\n";
    }
}


// ---------------------------------------------------------------------------
//  TXMLTreeElement: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TXMLTreeElement::bAttrExists(const TString& strName, tCIDLib::TCard4& c4At) const
{
    if (!m_pcolAttrs)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Count = c4AttrCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeAttr* pxtattrCur = m_pcolAttrs->pobjAt(c4Index);
            if (pxtattrCur->strQName() == strName)
            {
                c4At = c4Index;
                return kCIDLib::True;
            }
        }
    }
    c4At = kCIDLib::c4MaxCard;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TXMLTreeElement::bAttrExists(const TString& strName, TString& strToFill) const
{
    if (!m_pcolAttrs)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Count = c4AttrCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeAttr* pxtattrCur = m_pcolAttrs->pobjAt(c4Index);
            if (pxtattrCur->strQName() == strName)
            {
                strToFill = pxtattrCur->strValue();
                return kCIDLib::True;
            }
        }
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TXMLTreeElement::c4AttrCount() const
{
    if (!m_pcolAttrs)
        return 0;
    return m_pcolAttrs->c4ElemCount();
}


tCIDLib::TCard4 TXMLTreeElement::c4ChildCount() const
{
    if (!m_pcolChildren)
        return 0;
    return m_pcolChildren->c4ElemCount();
}


tCIDXML::ENodeTypes TXMLTreeElement::eChildTypeAt(const tCIDLib::TCard4 c4At) const
{
    if (!m_pcolChildren)
        return tCIDXML::ENodeTypes::Count;

    return m_pcolChildren->pobjAt(c4At)->eType();
}


tCIDXML::EElemTextTypes TXMLTreeElement::eTextType() const
{
    return m_eTextType;
}


//
//  Finds the next child element of this node with the passed qualified name,
//  if any. StartIndex is the index to start looking. So this supports
//  interation through child elements of the same name and whatnot.
//
//  If none is found, it returns a null pointer. They can indicate an instance
//  number (0 based) and we'll find the nth instance going forward. It defaults
//  to zero, which just returns the first one. Generally, when doing a search
//  it's set to 0. It's usually only used when called from the regular find
//  element methods, which start at the beginning.
//
const TXMLTreeElement*
TXMLTreeElement::pxtnodeFindElement(const   TString&         strQName
                                    , const tCIDLib::TCard4  c4StartInd
                                    ,       tCIDLib::TCard4& c4FoundAt
                                    , const tCIDLib::TCard4  c4Instance) const
{
    tCIDLib::TCard4 c4InstCnt = 0;
    const TXMLTreeElement* pxtnodeRet = 0;
    if (m_pcolChildren)
    {
        const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
        c4FoundAt = c4StartInd;
        for (; c4FoundAt < c4Count; c4FoundAt++)
        {
            const TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4FoundAt);
            if (pxtnodeCur->eType() != tCIDXML::ENodeTypes::Element)
                continue;

            // Cast to the real type and check the name
            const TXMLTreeElement* pxtnodeElem
                    = reinterpret_cast<const TXMLTreeElement*>(pxtnodeCur);

            // If it's our guy, then store it in the return pointer and break
            if (strQName == pxtnodeElem->strQName())
            {
                if (c4InstCnt == c4Instance)
                {
                    pxtnodeRet = pxtnodeElem;
                    break;
                }

                // Not enough of them yet, so bump the count and keep going
                c4InstCnt++;
            }
        }
    }
    return pxtnodeRet;
}

TXMLTreeElement*
TXMLTreeElement::pxtnodeFindElement(const   TString&            strQName
                                    , const tCIDLib::TCard4     c4StartInd
                                    ,       tCIDLib::TCard4&    c4FoundAt
                                    , const tCIDLib::TCard4     c4Instance)
{
    tCIDLib::TCard4 c4InstCnt = 0;
    TXMLTreeElement* pxtnodeRet = 0;
    if (m_pcolChildren)
    {
        const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
        c4FoundAt = c4StartInd;
        for (; c4FoundAt < c4Count; c4FoundAt++)
        {
            TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4FoundAt);
            if (pxtnodeCur->eType() != tCIDXML::ENodeTypes::Element)
                continue;

            // Cast to the real type and check the name
            TXMLTreeElement* pxtnodeElem
                    = static_cast<TXMLTreeElement*>(pxtnodeCur);

            // If it's our guy, then store it in the return pointer and break
            if (strQName == pxtnodeElem->strQName())
            {
                if (c4InstCnt == c4Instance)
                {
                    pxtnodeRet = pxtnodeElem;
                    break;
                }

                // Not enough of them yet, so bump the count and keep tgoing
                c4InstCnt++;
            }
        }
    }
    return pxtnodeRet;
}


//
//  This guy works backwards and finds the last text node, if any. When the
//  parser gets callbacks with text, it must append all contiguous chunks into
//  the same text node.
//
TXMLTreeText* TXMLTreeElement::pxtnodeLastIsText()
{
    TXMLTreeText* pxtnodeRet = nullptr;

    // If we have a child array
    if (m_pcolChildren)
    {
        // And if there is at least one child node
        const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
        if (c4Count)
        {
            // Get the last node and see if it is a text node
            TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4Count - 1);

            // If it's a text node, cast it to the real type
            if (pxtnodeCur->eType() == tCIDXML::ENodeTypes::Text)
                pxtnodeRet = static_cast<TXMLTreeText*>(pxtnodeCur);
        }
    }
    return pxtnodeRet;
}


tCIDLib::TVoid TXMLTreeElement::Reset()
{
    if (m_pcolAttrs)
        m_pcolAttrs->RemoveAll();

    //
    //  Go through and clean up the child nodes. We have to deal with the
    //  fact that some are pooled ones that we don't delete and others are
    //  allocated ones that we do.
    //
    if (m_pcolChildren)
        ReleaseChildren();

    m_strQName.Clear();
}


const TString& TXMLTreeElement::strQName() const
{
    return m_strQName;
}


const TXMLTreeAttr& TXMLTreeElement::xtattrAt(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolAttrs || (c4Index >= m_pcolAttrs->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadAttrIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , m_pcolAttrs ? TCardinal(m_pcolAttrs->c4ElemCount()) : TString(L"0")
        );
    }
    return *m_pcolAttrs->pobjAt(c4Index);
}


const TXMLTreeAttr& TXMLTreeElement::xtattrNamed(const TString& strName) const
{
    if (m_pcolAttrs)
    {
        const tCIDLib::TCard4 c4Count = c4AttrCount();
        if (c4Count)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                const TXMLTreeAttr* pxtattrCur = m_pcolAttrs->pobjAt(c4Index);
                if (pxtattrCur->strQName() == strName)
                    return *pxtattrCur;
            }
        }
    }

    // We didn't find it
    facCIDXML().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kXMLErrs::errcTree_AttrNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , m_strQName
        , strName
    );

    // Make the compiler happy, won't ever get called
    return *m_pcolAttrs->pobjAt(0);
}


const TXMLTreeNode&
TXMLTreeElement::xtnodeChildAt(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolChildren || (c4Index >= m_pcolChildren->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadChildIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , TCardinal(m_pcolChildren ? m_pcolChildren->c4ElemCount() : 0UL)
            , strQName()
        );
    }
    return *m_pcolChildren->pobjAt(c4Index);
}


const TXMLTreeElement&
TXMLTreeElement::xtnodeChildAtAsElement(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolChildren || (c4Index >= m_pcolChildren->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadChildIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , TCardinal(m_pcolChildren ? TCardinal(m_pcolChildren->c4ElemCount()) : 0UL)
            , strQName()
        );
    }

    // Try to get the indicated child and see if its of the right type
    const TXMLTreeNode* pxtnodeRet = m_pcolChildren->pobjAt(c4Index);
    if (pxtnodeRet->eType() != tCIDXML::ENodeTypes::Element)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4Index)
            , m_strQName
            , TXMLTreeElement::clsThis()
        );
    }

    // Its ok, so do the cast and return
    return *reinterpret_cast<const TXMLTreeElement*>(pxtnodeRet);
}

const TXMLTreeComment&
TXMLTreeElement::xtnodeChildAtAsComment(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolChildren || (c4Index >= m_pcolChildren->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadChildIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , m_pcolChildren ? TCardinal(m_pcolChildren->c4ElemCount()) : TString(L"0")
            , strQName()
        );
    }

    // Try to get the indicated child and see if its of the right type
    const TXMLTreeNode* pxtnodeRet = m_pcolChildren->pobjAt(c4Index);
    if (pxtnodeRet->eType() != tCIDXML::ENodeTypes::Comment)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4Index)
            , m_strQName
            , TXMLTreeComment::clsThis()
        );
    }

    // Its ok, so do the cast and return
    return *reinterpret_cast<const TXMLTreeComment*>(pxtnodeRet);
}

const TXMLTreePI&
TXMLTreeElement::xtnodeChildAtAsPI(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolChildren || (c4Index >= m_pcolChildren->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadChildIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , m_pcolChildren ? TCardinal(m_pcolChildren->c4ElemCount()) : TString(L"0")
            , strQName()
        );
    }

    // Try to get the indicated child and see if its of the right type
    const TXMLTreeNode* pxtnodeRet = m_pcolChildren->pobjAt(c4Index);
    if (pxtnodeRet->eType() != tCIDXML::ENodeTypes::PI)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4Index)
            , m_strQName
            , TXMLTreePI::clsThis()
        );
    }

    // Its ok, so do the cast and return
    return *reinterpret_cast<const TXMLTreePI*>(pxtnodeRet);
}

const TXMLTreeText&
TXMLTreeElement::xtnodeChildAtAsText(const tCIDLib::TCard4 c4Index) const
{
    if (!m_pcolChildren || (c4Index >= m_pcolChildren->c4ElemCount()))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_BadChildIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , m_pcolChildren ? TCardinal(m_pcolChildren->c4ElemCount()) : TString(L"0")
            , strQName()
        );
    }

    // Try to get the indicated child and see if its of the right type
    const TXMLTreeNode* pxtnodeRet = m_pcolChildren->pobjAt(c4Index);
    if (pxtnodeRet->eType() != tCIDXML::ENodeTypes::Text)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4Index)
            , m_strQName
            , TXMLTreeText::clsThis()
        );
    }

    // Its ok, so do the cast and return
    return *reinterpret_cast<const TXMLTreeText*>(pxtnodeRet);
}


//
//  These guys just call the versions above that return a pointer and check
//  to see if the pointer is null, throwing if so. So they are just
//  convenience methods to keep the caller from having to make such sanity
//  checks when the child node should always be there in any normal
//  circumstance. So see the other versions above for details.
//
const TXMLTreeElement&
TXMLTreeElement::xtnodeFindElement( const   TString&            strQName
                                    , const tCIDLib::TCard4     c4StartInd
                                    ,       tCIDLib::TCard4&    c4FoundAt
                                    , const tCIDLib::TCard4     c4Instance) const
{
    const TXMLTreeElement* pxtnodeRet = pxtnodeFindElement
    (
        strQName, c4StartInd, c4FoundAt, c4Instance
    );

    if (!pxtnodeRet)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_ElemNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strQName
        );
    }
    return *pxtnodeRet;
}

TXMLTreeElement&
TXMLTreeElement::xtnodeFindElement( const   TString&            strQName
                                    , const tCIDLib::TCard4     c4StartInd
                                    ,       tCIDLib::TCard4&    c4FoundAt
                                    , const tCIDLib::TCard4     c4Instance)
{
    TXMLTreeElement* pxtnodeRet = pxtnodeFindElement
    (
        strQName, c4StartInd, c4FoundAt, c4Instance
    );

    if (!pxtnodeRet)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_ElemNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strQName
        );
    }
    return *pxtnodeRet;
}


//
//  Returns the Nth child element, ignoring other types. We return the actual
//  index it was found at as well.
//
const TXMLTreeElement&
TXMLTreeElement::xtnodeNthElement(  const   tCIDLib::TCard4     c4Index
                                    ,       tCIDLib::TCard4&    c4At) const
{
    c4At = 0;
    tCIDLib::TCard4 c4InstCnt = 0;
    TXMLTreeElement* pxtnodeRet = 0;
    if (m_pcolChildren)
    {
        const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
        for (; c4At < c4Count; c4At++)
        {
            TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4At);

            // If not ane element, skip it
            if (pxtnodeCur->eType() != tCIDXML::ENodeTypes::Element)
                continue;

            // Cast to the real type and check the name
            TXMLTreeElement* pxtnodeElem
                    = static_cast<TXMLTreeElement*>(pxtnodeCur);

            if (c4InstCnt == c4Index)
            {
                pxtnodeRet = pxtnodeElem;
                break;
            }

            // Not enough of them yet, so bump the count and keep tgoing
            c4InstCnt++;
        }
    }

    if (!pxtnodeRet)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_NoSuchElementIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c4Index)
        );
    }
    return *pxtnodeRet;
}


// ---------------------------------------------------------------------------
//  TXMLTreeElement: Hidden Constructors
// ---------------------------------------------------------------------------
TXMLTreeElement::TXMLTreeElement(const  TXMLElemDecl&       xdeclSrc
                                , const TVector<TXMLAttr>&  colAttrList
                                , const tCIDLib::TCard4     c4AttrListSize) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Element)
    , m_eTextType(xdeclSrc.eTextType())
    , m_pcolAttrs(0)
    , m_pcolChildren(0)
    , m_strQName
      (
        (xdeclSrc.strFullName().c4Length() < 64)
         ? 64UL : xdeclSrc.strFullName().c4Length()
      )
{
    m_strQName = xdeclSrc.strFullName();
    if (c4AttrListSize)
    {
        m_pcolAttrs = new TRefVector<TXMLTreeAttr>(tCIDLib::EAdoptOpts::Adopt);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4AttrListSize; c4Index++)
            m_pcolAttrs->Add(new TXMLTreeAttr(colAttrList[c4Index]));
    }
}


// ---------------------------------------------------------------------------
//  TXMLTreeElement: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeElement::AddChild(TXMLTreeNode* const pxtnodeNew)
{
    // Our list is non-adopting due to the use of pooled nodes!
    if (!m_pcolChildren)
        m_pcolChildren = new TRefVector<TXMLTreeNode>(tCIDLib::EAdoptOpts::NoAdopt);

    m_pcolChildren->Add(pxtnodeNew);
}


tCIDLib::TVoid
TXMLTreeElement::Set(const  TXMLElemDecl&       xdeclSrc
                    , const TVector<TXMLAttr>&  colAttrList
                    , const tCIDLib::TCard4     c4AttrListSize)
{
    m_eTextType = xdeclSrc.eTextType();
    m_strQName = xdeclSrc.strFullName();

    // If we have an attribute list currently, empty it
    if (m_pcolAttrs)
        m_pcolAttrs->RemoveAll();

    // If any incoming ones, then add those
    if (c4AttrListSize)
    {
        // We may not have allocated the list yet
        if (!m_pcolAttrs)
            m_pcolAttrs = new TRefVector<TXMLTreeAttr>(tCIDLib::EAdoptOpts::Adopt);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4AttrListSize; c4Index++)
            m_pcolAttrs->Add(new TXMLTreeAttr(colAttrList[c4Index]));
    }
}



// ---------------------------------------------------------------------------
//  TXMLTreeElement: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to release our children. Some are pooled ones that we do
//  not own and some are allocated ones that we do. So we have to do the
//  appropriate thing. For the pooled ones we just drop them.
//
tCIDLib::TVoid TXMLTreeElement::ReleaseChildren()
{
    //
    //  Go through our list in reverse to avoid the need to compact the
    //  list repeatedly. For each one, depending on its type we either
    //  delete it or just ignore it (because it's pooled.)
    //
    if (m_pcolChildren && !m_pcolChildren->bIsEmpty())
    {
        tCIDLib::TCard4 c4Index = m_pcolChildren->c4ElemCount();
        do
        {
            c4Index--;
            TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4Index);
            switch(pxtnodeCur->eType())
            {
                case tCIDXML::ENodeTypes::Text :
                    // These are pooled so we don't have to do anything
                    break;

                case tCIDXML::ENodeTypes::Element :
                    // Recurse on this one, otherwise it's pooled itself
                    static_cast<TXMLTreeElement*>(pxtnodeCur)->ReleaseChildren();
                    break;

                default :
                    // These are allocated so delete them
                    delete pxtnodeCur;
                    break;
            };
        }   while (c4Index != 0);

        // And now flush the collection
        m_pcolChildren->RemoveAll();
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TTreeAttrCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTreeAttrCursor: Constructors and Destructor
// ---------------------------------------------------------------------------
TTreeAttrCursor::TTreeAttrCursor(const TXMLTreeElement& xtnodeSrc) :

    m_pcursCol(nullptr)
{
    m_pcursCol = new TRefVector<TXMLTreeAttr>::TCursor(xtnodeSrc.m_pcolAttrs);
    m_pcursCol->bReset();
}

TTreeAttrCursor::~TTreeAttrCursor()
{
    delete m_pcursCol;
}


// ---------------------------------------------------------------------------
//  TTreeAttrCursor: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTreeAttrCursor::bNext()
{
    return m_pcursCol->bNext();
}

tCIDLib::TBoolean TTreeAttrCursor::bReset()
{
    return m_pcursCol->bReset();
}

const TXMLTreeAttr& TTreeAttrCursor::xtattrCur() const
{
    return m_pcursCol->objRCur();
}



// ---------------------------------------------------------------------------
//   CLASS: TTreeChildCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTreeChildCursor: Constructors and Destructor
// ---------------------------------------------------------------------------
TTreeChildCursor::TTreeChildCursor(const TXMLTreeElement& xtnodeSrc) :

    m_pcursCol(nullptr)
{
    if (xtnodeSrc.m_pcolChildren)
        m_pcursCol = new TRefVector<TXMLTreeNode>::TCursor(xtnodeSrc.m_pcolChildren);
}

TTreeChildCursor::~TTreeChildCursor()
{
    delete m_pcursCol;
}


// ---------------------------------------------------------------------------
//  TTreeChildCursor: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTreeChildCursor::bNext()
{
    if (!m_pcursCol)
        return kCIDLib::False;
    return m_pcursCol->bNext();
}


tCIDLib::TBoolean
TTreeChildCursor::bNextElementByName(const TString& strName)
{
    if (!m_pcursCol)
        return kCIDLib::False;

    //
    //  We have to move forward first, since we are currently on the last
    //  node found.
    //
    while (m_pcursCol->bNext())
    {
        if (m_pcursCol->objRCur().eType() == tCIDXML::ENodeTypes::Element)
        {
            // Do the needed downcast
            const TXMLTreeElement& xtnodeCur
                = reinterpret_cast<const TXMLTreeElement&>(m_pcursCol->objRCur());

            if (xtnodeCur.strQName() == strName)
                return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TTreeChildCursor::bNextNonText()
{
    if (!m_pcursCol)
        return kCIDLib::False;

    //
    //  We have to move forward first, since we are currently on the last
    //  node found.
    //
    while (m_pcursCol->bNext())
    {
        if (m_pcursCol->objRCur().eType() != tCIDXML::ENodeTypes::Text)
            return kCIDLib::True;
    }

    return kCIDLib::False;
}


tCIDLib::TBoolean
TTreeChildCursor::bNextOfType(const tCIDXML::ENodeTypes eType)
{
    if (!m_pcursCol)
        return kCIDLib::False;

    //
    //  We have to move forward first, since we are currently on the last
    //  node found.
    //
    while (m_pcursCol->bNext())
    {
        if (m_pcursCol->objRCur().eType() == eType)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TTreeChildCursor::bReset()
{
    if (!m_pcursCol)
        return kCIDLib::False;
    return m_pcursCol->bReset();
}


const TXMLTreeNode& TTreeChildCursor::xtnodeCur() const
{
    return m_pcursCol->objRCur();
}



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreePI
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreePI: Destructor
// ---------------------------------------------------------------------------
TXMLTreePI::~TXMLTreePI()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreePI: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreePI::PrintTo(        TTextOutStream&     strmTarget
                    , const tCIDLib::TCard4     c4Indent
                    , const tCIDLib::TBoolean) const
{
    // <TBD> We should be honoring the escape flag here?

    strmTarget  << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4)
                << L"<?" << m_strTarget;
    if (!m_strValue.bIsEmpty())
        strmTarget << L" " << m_strValue;
    strmTarget << L"?>\n";
}


// ---------------------------------------------------------------------------
//  TXMLTreePI: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TXMLTreePI::strTarget() const
{
    return m_strTarget;
}

const TString& TXMLTreePI::strValue() const
{
    return m_strValue;
}


// ---------------------------------------------------------------------------
//  TXMLTreePI: Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreePI::TXMLTreePI( const   tCIDLib::TCh* const pszTarget
                        , const tCIDLib::TCh* const pszValue) :

    TXMLTreeNode(tCIDXML::ENodeTypes::PI)
    , m_strTarget(pszTarget)
    , m_strValue(pszValue)
{
}

TXMLTreePI::TXMLTreePI(const TString& strTarget, const TString& strValue) :

    TXMLTreeNode(tCIDXML::ENodeTypes::PI)
    , m_strTarget(strTarget)
    , m_strValue(strValue)
{
}



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeText
//  PREFIX: xtnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeText: Destructor
// ---------------------------------------------------------------------------
TXMLTreeText::~TXMLTreeText()
{
}


// ---------------------------------------------------------------------------
//  TXMLTreeText: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeText::PrintTo(          TTextOutStream&     strmTarget
                        , const tCIDLib::TCard4     c4Indent
                        , const tCIDLib::TBoolean   bEscape) const
{
    strmTarget << TTextOutStream::RepChars(kCIDLib::chSpace, c4Indent * 4);

    // If ignorable, or all whitespace, ignore it
    if (!m_bIsIgnorable)
    {
        tCIDLib::TBoolean bShow = kCIDLib::False;
        const tCIDLib::TCard4 c4Count = m_strText.c4Length();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (!TRawStr::bIsSpace(m_strText[c4Index]))
            {
                bShow = kCIDLib::True;
                break;
            }
        }

        // Escape the text if asked to, else just show it as is
        if (bShow)
        {
            if (bEscape)
                facCIDXML().EscapeFor(m_strText, strmTarget, tCIDXML::EEscTypes::ElemText);
            else
                strmTarget << m_strText;
        }
    }
}


// ---------------------------------------------------------------------------
//  TXMLTreeText: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeText::AppendText(const TString& strToAppend)
{
    //
    //  If the new text would exceed the string's buffer, then we will force
    //  a more efficient (though more wasteful) expansion of the string than
    //  it would do itself. We'll expand it to 1.25 times what it's new size
    //  would be, or by 4K, whichever is larger.
    //
    tCIDLib::TCard4 c4New = m_strText.c4Length() + strToAppend.c4Length();
    if (c4New > m_strText.c4BufChars())
    {
        tCIDLib::TCard4 c4ExpSz = tCIDLib::TCard4(c4New * 1.25);
        if (c4ExpSz < c4New + 4096)
            c4ExpSz = c4New + 4096;
        m_strText.Reallocate(c4ExpSz);
    }
    m_strText.Append(strToAppend);
}


tCIDLib::TBoolean TXMLTreeText::bIsCDATA() const
{
    return m_bIsCDATA;
}

tCIDLib::TBoolean TXMLTreeText::bIsIgnorable() const
{
    return m_bIsIgnorable;
}

const TString& TXMLTreeText::strText() const
{
    return m_strText;
}


// ---------------------------------------------------------------------------
//  TXMLTreeText: Hidden constructors
// ---------------------------------------------------------------------------
TXMLTreeText::TXMLTreeText( const   tCIDLib::TCh* const pszText
                            , const tCIDLib::TBoolean   bIsCDATA
                            , const tCIDLib::TBoolean   bIsIgnorable) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Text)
    , m_bIsCDATA(bIsCDATA)
    , m_bIsIgnorable(bIsIgnorable)
    , m_strText(1024UL)
{
    m_strText = pszText;
}


TXMLTreeText::TXMLTreeText( const   TString&            strText
                            , const tCIDLib::TBoolean   bIsCDATA
                            , const tCIDLib::TBoolean   bIsIgnorable) :

    TXMLTreeNode(tCIDXML::ENodeTypes::Text)
    , m_bIsCDATA(bIsCDATA)
    , m_bIsIgnorable(bIsIgnorable)
    , m_strText
      (
        (strText.c4Length() < 1024) ? 1024UL : strText.c4Length()
      )
{
    m_strText = strText;
}


// ---------------------------------------------------------------------------
//  TXMLTreeText: Protected methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeText::Set(  const   TString&            strText
                    , const tCIDLib::TBoolean   bIsCDATA
                    , const tCIDLib::TBoolean   bIsIgnorable)
{
    m_bIsCDATA = bIsCDATA;
    m_bIsIgnorable = bIsIgnorable;
    m_strText = strText;
}




// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDocument
//  PREFIX: xtdoc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeDocument: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLTreeDocument::TXMLTreeDocument() :

    m_pcolChildren(new TRefVector<TXMLTreeNode>(tCIDLib::EAdoptOpts::Adopt))
{
}

TXMLTreeDocument::~TXMLTreeDocument()
{
    delete m_pcolChildren;
}


// ---------------------------------------------------------------------------
//  TXMLTreeDocument: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TXMLTreeElement& TXMLTreeDocument::xtnodeRoot() const
{
    //
    //  Search the list of children for the one that is an element type. If
    //  we don't find one, then its not set yet.
    //
    TXMLTreeNode* pxtnodeRoot = 0;
    const tCIDLib::TCard4 c4ChildCount = m_pcolChildren->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCount; c4Index++)
    {
        TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4Index);
        if (pxtnodeCur->eType() == tCIDXML::ENodeTypes::Element)
        {
            pxtnodeRoot = pxtnodeCur;
            break;
        }
    }

    if (!pxtnodeRoot)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_NoRoot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *(static_cast<TXMLTreeElement*>(pxtnodeRoot));
}


TXMLTreeElement& TXMLTreeDocument::xtnodeRoot()
{
    //
    //  Search the list of children for the one that is an element type. If
    //  we don't find one, then its not set yet.
    //
    TXMLTreeNode* pxtnodeRoot = 0;
    const tCIDLib::TCard4 c4ChildCount = m_pcolChildren->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCount; c4Index++)
    {
        TXMLTreeNode* pxtnodeCur = m_pcolChildren->pobjAt(c4Index);
        if (pxtnodeCur->eType() == tCIDXML::ENodeTypes::Element)
        {
            pxtnodeRoot = pxtnodeCur;
            break;
        }
    }

    if (!pxtnodeRoot)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_NoRoot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *(static_cast<TXMLTreeElement*>(pxtnodeRoot));
}


tCIDLib::TVoid
TXMLTreeDocument::PrintTo(          TTextOutStream&     strmTarget
                            , const tCIDLib::TCard4     c4Indent
                            , const tCIDLib::TBoolean   bEscape) const
{
    //
    //  Iterate our children and print each one of them. To make it nicer we
    //  put out the decl and root at zero indent, then 1 indent until we hit the
    //  last one.
    //
    enum EStates
    {
        EState_Decl
        , EState_Root
        , EState_Content
    }   eState = EState_Decl;

    const tCIDLib::TCard4 c4ChildCount = m_pcolChildren->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCount; c4Index++)
    {
        const TXMLTreeNode& xtnodeCur = *m_pcolChildren->pobjAt(c4Index);

        tCIDLib::TCard4 c4Indent = 0;
        switch(eState)
        {
            case EState_Decl :
                //
                //  If the current node is a decl, then keep zero indent, else
                //  fall through to root.
                //
                if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Decl)
                {
                    eState = EState_Root;
                    break;
                }
                 else
                {
                    eState = EState_Root;
                }

            case EState_Root :
                // Stay at zero, go to content
                eState = EState_Content;
            break;

            case EState_Content :
                // If the last one, go back to zero, else 1
                if (c4Index != c4ChildCount - 1)
                    c4Indent = 1;
            break;

            default :
                break;
        };

        xtnodeCur.PrintTo(strmTarget, c4Indent, bEscape);
    }
}


tCIDLib::TVoid TXMLTreeDocument::Reset()
{
    //
    //  Trash the tree of children. At this level, no pooled nodes are
    //  used so we can just delete them all. The parser will give us an
    //  allocated root element node, and will pool any other nested
    //  element nodes.
    //
    m_pcolChildren->RemoveAll();
}


// ---------------------------------------------------------------------------
//  TXMLTreeDocument: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeDocument::AddChild(TXMLTreeNode* const pxtnodeToAdopt)
{
    m_pcolChildren->Add(pxtnodeToAdopt);
}


tCIDLib::TVoid TXMLTreeDocument::FormatTo(TTextOutStream& strmDest) const
{
    PrintTo(strmDest, 0, kCIDLib::True);
}


