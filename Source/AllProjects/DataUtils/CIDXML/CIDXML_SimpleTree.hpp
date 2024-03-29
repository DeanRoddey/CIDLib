//
// FILE NAME: CIDXML_SimpleTree.hpp
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
//  This is the header for the CIDXML_SimpleTree.Cpp file, which provides the
//  classes used by the TXMLTreeParser class. This class provides to the
//  client code a very simple tree representation of the XML data.
//
// CAVEATS/GOTCHAS:
//
//  1)  All of the tree stuff supports the MFormattable interface, which
//      allows them to be formatted to a text stream. However, that interface
//      does not support any kind of flags, since its done via the << operator.
//      So a PrintTo() method is provided which supports the needed XML
//      specific formatting flags. MFormattable's FormatTo() method is just
//      implemented as a call to PrintTo() with default parameters.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTreeChildCursor;
class TTreeAttrCursor;
class TXMLTreeElement;
class TXMLTreePI;
class TXMLTreeText;


// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeAttr
//  PREFIX: xtattr
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeAttr : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeAttr() = delete;

        TXMLTreeAttr(const TXMLTreeAttr&) = default;
        TXMLTreeAttr(TXMLTreeAttr&&) = default;

        ~TXMLTreeAttr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeAttr& operator=(const TXMLTreeAttr&) = default;
        TXMLTreeAttr& operator=(TXMLTreeAttr&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bExplicit() const;

        tCIDLib::TBoolean bValueAs() const;

        tCIDLib::TCard1 c1ValueAs
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard2 c2ValueAs
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard4 c4ValueAs
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard8 c8ValueAs
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDXML::EAttrTypes eType() const;

        tCIDLib::TFloat8 f8ValueAs() const;

        tCIDLib::TInt1 i1ValueAs() const;

        tCIDLib::TInt2 i2ValueAs() const;

        tCIDLib::TInt4 i4ValueAs() const;

        const TString& strBaseName() const;

        const TString& strPrefix() const;

        const TString& strQName() const;

        const TString& strURI() const;

        const TString& strValue() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeElement;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeAttr
        (
            const   TXMLAttr&               xattrSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bExplicit
        //      Inicates whether this attribute was eplicitly provided for
        //      faulted in from a fixed or default value.
        //
        //  m_eType
        //      The type of the attribute.
        //
        //  m_strBaseName
        //      The base part of the name of the attribute.
        //
        //  m_strPrefix
        //      The prefix of the attribute name, if any.
        //
        //  m_strURI
        //      The URI that the prefix mapped to, if any. Won't be used if
        //      namespace processing is not on.
        //
        //  m_strValue
        //      The value text of the attribute.
        //
        //  This one is mutable, because its faulted in when needed.
        //
        //  m_strQName
        //      The full QName of the attribute name.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bExplicit;
        tCIDXML::EAttrTypes m_eType;
        TString             m_strBaseName;
        TString             m_strPrefix;
        TString             m_strURI;
        TString             m_strValue;

        mutable TString     m_strQName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeAttr, TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeNode
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeNode : public TObject, public MFormattable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeNode() = delete;

        TXMLTreeNode(const TXMLTreeNode&) = delete;
        TXMLTreeNode(TXMLTreeNode&&) = delete;

        ~TXMLTreeNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeNode& operator=(const TXMLTreeNode&) = delete;
        TXMLTreeNode& operator=(TXMLTreeNode&&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDXML::ENodeTypes eType() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeNode
        (
            const   tCIDXML::ENodeTypes     eType
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eType
        //      Indicates the type of node. This is often used to search
        //      through a list of nodes to find the ones of interest before
        //      you bother downcasting them.
        // -------------------------------------------------------------------
        tCIDXML::ENodeTypes     m_eType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeNode, TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeComment
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeComment : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TXMLTreeComment() = delete;

        TXMLTreeComment(const TXMLTreeComment&) = delete;
        TXMLTreeComment(TXMLTreeComment&&) = delete;

        ~TXMLTreeComment();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeComment& operator=(const TXMLTreeComment&) = delete;
        TXMLTreeComment& operator=(TXMLTreeComment&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strText() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeComment
        (
            const   tCIDLib::TCh* const     pszText
        );

        TXMLTreeComment
        (
            const   TString&                strText
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strText
        //      This is the text of the comment.
        // -------------------------------------------------------------------
        TString     m_strText;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeComment,TXMLTreeNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDecl
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeDecl : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeDecl() = delete;

        TXMLTreeDecl(const TXMLTreeDecl&) = delete;
        TXMLTreeDecl(TXMLTreeDecl&&) = delete;

        ~TXMLTreeDecl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeDecl& operator=(const TXMLTreeDecl&) = delete;
        TXMLTreeDecl& operator=(TXMLTreeDecl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TString strVersion() const
        {
            return m_strVersion;
        }

        TString strEncoding() const
        {
            return m_strEncoding;
        }

        TString strStandalone() const
        {
            return m_strStandalone;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
            , const TString&                strStandalone
        );


        TXMLTreeDecl
        (
            const   tCIDLib::TCh* const     pszVersion
            , const tCIDLib::TCh* const     pszEncoding
            , const tCIDLib::TCh* const     pszStandalone
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strVersion
        //      The version string found, if any.
        //
        //  m_strEncoding
        //      The encoding string found, if any.
        //
        //  m_strStandalone
        //      The standalone string found, if any.
        // -------------------------------------------------------------------
        TString     m_strVersion;
        TString     m_strEncoding;
        TString     m_strStandalone;
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDTD
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeDTD : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeDTD() = delete;

        TXMLTreeDTD(const TXMLTreeDTD&) = delete;
        TXMLTreeDTD(TXMLTreeDTD&&) = delete;

        ~TXMLTreeDTD();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeDTD& operator=(const TXMLTreeDTD&) = delete;
        TXMLTreeDTD& operator=(TXMLTreeDTD&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strPublicId() const;

        const TString& strRootElem() const;

        const TString& strSystemId() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeDTD
        (
            const   TString&                strRootElem
            , const TString&                strPublicId
            , const TString&                strSystemId
        );


        TXMLTreeDTD
        (
            const   tCIDLib::TCh* const     pszRootElem
            , const tCIDLib::TCh* const     pszPublicId
            , const tCIDLib::TCh* const     pszSystemId
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strRootElem
        //      The name of the root element used in the DOCTYPE line.
        //
        //  m_strPublicId
        //  m_strSystemId
        //      These public/system ids found in the DOCTYPE line.
        // -------------------------------------------------------------------
        TString     m_strRootElem;
        TString     m_strPublicId;
        TString     m_strSystemId;
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeElement
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeElement : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeElement() = delete;

        TXMLTreeElement(const TXMLTreeElement&) = delete;
        TXMLTreeElement(TXMLTreeElement&&) = delete;

        ~TXMLTreeElement();


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLTreeElement& operator=(const TXMLTreeElement&) = delete;
        TXMLTreeElement& operator=(TXMLTreeElement&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAttrExists
        (
            const   TString&                strName
            ,       tCIDLib::TCard4&        c4AtToFill
        )   const;

        tCIDLib::TBoolean bAttrExists
        (
            const   TString&                strName
            ,       TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4AttrCount() const;

        tCIDLib::TCard4 c4ChildCount() const;

        tCIDXML::ENodeTypes eChildTypeAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDXML::EElemTextTypes eTextType() const;

        //
        //  Call back each element node, breaking out on false return. Can be
        //  a function pointer or lambda. We return true if we went all the way
        //  to the end.
        //
        template <typename IterCB> tCIDLib::TBoolean bForEach(IterCB iterCB) const
        {
            // IF there were no children, this guy can be null
            if (m_pcolChildren)
            {
                const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    const TXMLTreeNode& xtnodeCur = *m_pcolChildren->pobjAt(c4Index);
                    if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Element)
                    {
                        const tCIDLib::TBoolean bRet = iterCB
                        (
                            static_cast<const TXMLTreeElement&>(xtnodeCur)
                        );
                        if (!bRet)
                            return bRet;
                    }
                }
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachI(IterCB iterCB) const
        {
            // If there were no children, this guy can be null
            if (m_pcolChildren)
            {
                const tCIDLib::TCard4 c4Count = m_pcolChildren->c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    const TXMLTreeNode& xtnodeCur = *m_pcolChildren->pobjAt(c4Index);
                    if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Element)
                    {
                        const tCIDLib::TBoolean bRet = iterCB
                        (
                            static_cast<const TXMLTreeElement&>(xtnodeCur), c4Index
                        );
                        if (!bRet)
                            return bRet;
                    }
                }
            }
            return kCIDLib::True;
        }

        const TXMLTreeElement* pxtnodeFindElement
        (
            const   TString&                strQName
            , const tCIDLib::TCard4         c4StartInd
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TCard4         c4Instance = 0
        )   const;

        TXMLTreeElement* pxtnodeFindElement
        (
            const   TString&                strQName
            , const tCIDLib::TCard4         c4StartInd
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TCard4         c4Instance = 0
        );

        TXMLTreeText* pxtnodeLastIsText();

        tCIDLib::TVoid Reset();

        const TString& strQName() const;

        const TXMLTreeAttr& xtattrAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreeAttr& xtattrNamed
        (
            const   TString&                strName
        )   const;

        const TXMLTreeNode& xtnodeChildAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreeElement& xtnodeChildAtAsElement
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreeComment& xtnodeChildAtAsComment
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreePI& xtnodeChildAtAsPI
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreeText& xtnodeChildAtAsText
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TXMLTreeElement& xtnodeFindElement
        (
            const   TString&                strQName
            , const tCIDLib::TCard4         c4StartInd
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TCard4         c4Instance = 0
        )   const;

        TXMLTreeElement& xtnodeFindElement
        (
            const   TString&                strQName
            , const tCIDLib::TCard4         c4StartInd
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TCard4         c4Instance = 0
        );

        const TXMLTreeElement& xtnodeNthElement
        (
            const   tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4FoundAt
        )   const;

        const TString& strAttr(const TString& strName) const
        {
           return xtattrNamed(strName).strValue();
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;
        friend class TTreeChildCursor;
        friend class TTreeAttrCursor;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeElement
        (
            const   TXMLElemDecl&       xdeclSrc
            , const TVector<TXMLAttr>&  colAttrList
            , const tCIDLib::TCard4     c4AttrListSize
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddChild
        (
                    TXMLTreeNode* const     pxtnodeNew
        );

        tCIDLib::TVoid Set
        (
            const   TXMLElemDecl&       xdeclSrc
            , const TVector<TXMLAttr>&  colAttrList
            , const tCIDLib::TCard4     c4AttrListSize
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReleaseChildren();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eTextTypes
        //      Indicatse how this type of element deals with text.
        //
        //  m_pcolAttrs
        //      A collection of the attributes of this element. Its faulted
        //      in when needed.
        //
        //  m_pcolChildren
        //      A collection of the child node of this element. Its faulted
        //      in when needed. It is non-adopting since some of our children
        //      are pooled and reused. So our ReleaseChildren() methods will
        //      handle cleaning them up appropriately.
        //
        //  m_strQName
        //      The QName of the element.
        // -------------------------------------------------------------------
        tCIDXML::EElemTextTypes             m_eTextType;
        mutable TRefVector<TXMLTreeAttr>*   m_pcolAttrs;
        mutable TRefVector<TXMLTreeNode>*   m_pcolChildren;
        TString                             m_strQName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeElement,TXMLTreeNode)
};


// ---------------------------------------------------------------------------
//   CLASS: TTreeAttrCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------
class CIDXMLEXP TTreeAttrCursor : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeAttrCursor() = delete;

        TTreeAttrCursor
        (
            const   TXMLTreeElement&        xtnodeSrc
        );

        TTreeAttrCursor(const TTreeAttrCursor&) = delete;
        TTreeAttrCursor(TTreeAttrCursor&&) = delete;

        ~TTreeAttrCursor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTreeAttrCursor& operator=(const TTreeAttrCursor&) = delete;
        TTreeAttrCursor& operator=(TTreeAttrCursor&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bReset();

        const TXMLTreeAttr& xtattrCur() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcursCol
        //      This is the underlying collection cursor that we are wrapping
        // -------------------------------------------------------------------
        TRefVector<TXMLTreeAttr>::TCursor*  m_pcursCol;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTreeAttrCursor,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TTreeChildCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------
class CIDXMLEXP TTreeChildCursor : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeChildCursor() = delete;

        TTreeChildCursor
        (
            const   TXMLTreeElement&        xtnodeSrc
        );

        TTreeChildCursor(const TTreeChildCursor&) = delete;
        TTreeChildCursor(TTreeChildCursor&&) = delete;

        ~TTreeChildCursor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTreeChildCursor& operator=(const TTreeChildCursor&) = delete;
        TTreeChildCursor& operator=(TTreeChildCursor&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bNextElementByName
        (
            const   tCIDLib::TCh* const     pszName
        );

        tCIDLib::TBoolean bNextElementByName
        (
            const   TString&                strName
        );

        tCIDLib::TBoolean bNextNonText();

        tCIDLib::TBoolean bNextOfType
        (
            const   tCIDXML::ENodeTypes     eType
        );

        tCIDLib::TBoolean bReset();

        const TXMLTreeNode& xtnodeCur() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cursCol
        //      This is the underlying collection cursor that we are wrapping
        // -------------------------------------------------------------------
        TRefVector<TXMLTreeNode>::TCursor*  m_pcursCol;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTreeChildCursor,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreePI
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreePI : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreePI() = delete;

        TXMLTreePI(const TXMLTreePI&) = delete;
        TXMLTreePI(TXMLTreePI&&) = delete;

        ~TXMLTreePI();


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLTreePI& operator=(const TXMLTreePI&) = delete;
        TXMLTreePI& operator=(TXMLTreePI&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strTarget() const;

        const TString& strValue() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreePI
        (
            const   tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCh* const     pszValue
        );

        TXMLTreePI
        (
            const   TString&                strTarget
            , const TString&                strValue
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strTarget
        //      This is the target of the PI
        //
        //  m_strValue
        //      This is the value of the PI
        // -------------------------------------------------------------------
        TString     m_strTarget;
        TString     m_strValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreePI,TXMLTreeNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeText
//  PREFIX: xtnode
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeText : public TXMLTreeNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TXMLTreeText() = delete;

        TXMLTreeText(const TXMLTreeText&) = delete;
        TXMLTreeText(TXMLTreeText&&) = delete;

        ~TXMLTreeText();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeText& operator=(const TXMLTreeText&) = delete;
        TXMLTreeText& operator=(TXMLTreeText&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendText
        (
            const   TString&                strToAppend
        );

        tCIDLib::TBoolean bAllSpace() const
        {
            return m_bAllSpaces;
        }

        tCIDLib::TBoolean bIsCDATA() const
        {
            return m_bIsCDATA;
        }

        tCIDLib::TBoolean bIsIgnorable() const
        {
            return m_bIsIgnorable;
        }

        const TString& strText() const
        {
            return m_strText;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLTreeText
        (
            const   TStringView&            strvText
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDLib::TBoolean       bAllSpaces
        );


        // -------------------------------------------------------------------
        //  Protected methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Set
        (
            const   TStringView&            strvText
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDLib::TBoolean       bAllSpaces
        );



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAllSpaces
        //      The parser has this info and provides it. We store it away since we
        //      may make use of it during printing.
        //
        //  m_bIsCDATA
        //  m_bIsIgnorable
        //      These indicate some attributes of the text node.
        //
        //  m_strText
        //      This is the text of the text node.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bAllSpaces;
        tCIDLib::TBoolean   m_bIsCDATA;
        tCIDLib::TBoolean   m_bIsIgnorable;
        TString             m_strText;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeText,TXMLTreeNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeDocument
//  PREFIX: xtdoc
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeDocument : public TObject, public MFormattable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLTreeDocument();

        TXMLTreeDocument(const TXMLTreeDocument&) = delete;
        TXMLTreeDocument(TXMLTreeDocument&&) = delete;

        ~TXMLTreeDocument();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeDocument& operator=(const TXMLTreeDocument&) = delete;
        TXMLTreeDocument& operator=(TXMLTreeDocument&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid PrintTo
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4IndentLevel
            , const tCIDXML::EPrintFmts     eFormat = tCIDXML::EPrintFmts::Pretty
            , const tCIDXML::EPrintOpts     eOpts = tCIDXML::EPrintOpts::None
        )   const;

        tCIDLib::TVoid Reset();

        const TXMLTreeDecl& xtnodeDecl() const;

        const TXMLTreeElement& xtnodeRoot() const;

        TXMLTreeElement& xtnodeRoot();


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLTreeParser;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddChild
        (
                    TXMLTreeNode* const     pxtnodeToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReleaseChildren();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolChildren
        //      The list of child nodes of the document. At least one (and
        //      only one) of these, in a well formed file, will be an element
        //      node and will be the root of the content. The others will be
        //      for space before the root, the DTD node, PIs and comments
        //      before the root, and so forth.
        //
        //      At this level we don't bother using any pooled nodes, so our
        //      list is adopting and we can just delete all children when we
        //      are reset. Any contained tree element nodes will do the right
        //      thing wrt their contained children.
        // -------------------------------------------------------------------
        TRefVector<TXMLTreeNode>*   m_pcolChildren;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeDocument,TObject)
};


#pragma CIDLIB_POPPACK

