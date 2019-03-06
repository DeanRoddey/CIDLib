//
// FILE NAME: CIDAI_BTParser_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/01/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the behavior tree parser. The trees can be fairly large, so we
//  don't want to use the XML tree parser. Instead we just use the core parser directly and
//  handle events from the parser. When we have collected the bits we need for a node, we
//  create it and insert it, then move on. We return the root node of the tree.
//
//  This is for internal use, so we don't have to expose the XML facility to our users.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TCIDAIBTParser
//  PREFIX: aibtp
// ---------------------------------------------------------------------------
class TCIDAIBTParser :  public MXMLDocEvents
                        , public MXMLDTDEvents
                        , public MXMLErrorEvents
                        , public MXMLEntityResolver
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAIBTParser();

        TCIDAIBTParser(const TCIDAIBTParser&) = delete;

        ~TCIDAIBTParser();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAIBTParser& operator=(const TCIDAIBTParser&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TAIBTRootNode* pbtnodeParse
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4Bytes
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DocCharacters
        (
            const   TString&                strChars
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDXML::ELocations     eLocation
        )   override;

        tCIDXML::TEntitySrcRef esrResolve
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
            , const TString&                strEntityName
            , const TString&                strParentId
            , const tCIDXML::EResolveTypes  eResType
        )   override;

        tCIDLib::TVoid EndDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   override;

        tCIDLib::TVoid EndTag
        (
            const   TXMLElemDecl&           xdeclElem
        )   override;

        tCIDLib::TVoid HandleXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
            , const tCIDXML::EErrTypes      eType
            , const TString&                strText
            , const tCIDLib::TCard4         c4CurColumn
            , const tCIDLib::TCard4         c4CurLine
            , const TString&                strSystemId
        )   override;

        tCIDLib::TVoid ResetDocType() override;

        tCIDLib::TVoid ResetDocument() override;

        tCIDLib::TVoid ResetResolver() override;

        tCIDLib::TVoid StartDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   override;

        tCIDLib::TVoid StartTag
        (
                    TXMLParserCore&         xprsSrc
            , const TXMLElemDecl&           xdeclElem
            , const tCIDLib::TBoolean       bEmpty
            , const TVector<TXMLAttr>&      colAttrList
            , const tCIDLib::TCard4         c4AttrListSize
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckNodeName
        (
            const   TXMLElemDecl&           xdeclElem
            , const TString&                strExpected
        );

        const TString& strFindAttr
        (
            const   TVector<TXMLAttr>&      colAttrList
            , const TString&                strToFind
            , const tCIDLib::TBoolean       bRequired
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colNodeStack
        //      We need to keep up with the stack of nodes as we go, since we are
        //      callback based and can't track it on the call stack. So the one on the
        //      top of the stack is the node whose children we are processing. When we
        //      get the end element we pop the stack. It's non-adopting. The tree
        //      itself owns the nodes.
        //
        //  m_colParams
        //      We have to accumulate parameter elements here until we get to the end
        //      of the params element, and can set them on the node at the top of the
        //      stack.
        //
        //  m_esrDTD
        //      An entity reference that we fault in upon construction. We handle the
        //      entity resolver interface of the XML parser, and we just give him this
        //      guy when we see him ask for our internal DTD's public id.
        //
        //  m_pbtnodeRoot
        //      A pointer to the root node we initially create. If all goes well, we
        //      return it. Else we destroy it, which cleans up the whole tree. It has
        //      to be a root type node, so it's the one special case.
        //
        //  m_strCurXMLPath
        //      In order to know where we are in the XML hierarchy during the callbacks,
        //      we add/remove element names as the start/end element callbacks are made.
        //      So at any time we can check to see, are we in the /X/Y/Z element right
        //      now. This is needed because the same element name might be used in
        //      multiple places (else we could just check the current element name.)
        //
        //  m_xtprsThis
        //      Our XML parser that we use to parse the files. We keep one as a member
        //      so that we don't have to creat eone for each new file parsed.
        // -------------------------------------------------------------------
        tCIDAI::TNodeStack      m_colNodeStack;
        tCIDLib::TKVPList       m_colParams;
        tCIDXML::TEntitySrcRef  m_esrDTD;
        TAIBTRootNode*          m_pbtnodeRoot;
        TString                 m_strCurXMLPath;
        TXMLParserCore          m_xtprsThis;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_strAttr_xxx
        //      The names of some attributes we look for
        //
        //  s_strDTD
        //      We use a fixed, internal DTD which we just set on the parser.
        //
        //  s_strElem_BTree
        //      The names of some elements we look for.
        // -------------------------------------------------------------------
        static const TString            s_strAttr_Flag;
        static const TString            s_strAttr_Key;
        static const TString            s_strAttr_NodeName;
        static const TString            s_strAttr_NodeType;
        static const TString            s_strAttr_TreeName;
        static const TString            s_strAttr_Value;
        static const TString            s_strAttr_Version;
        static const TString            s_strDTD;
        static const TString            s_strDTDPublicId;
        static const TString            s_strDTDSystemId;
        static const TString            s_strElem_BTNode;
        static const TString            s_strElem_BTree;
        static const TString            s_strElem_NodeParam;
        static const TString            s_strElem_NodeParams;
};
