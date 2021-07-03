//
// FILE NAME: CIDXML_SimpleTreeParser.hpp
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
//  This is the header for the CIDXML_SimpleTreeParser.Cpp file, which
//  implements a simple, tree based, interface to the core XML parser services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeParser
//  PREFIX: xtprs
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLTreeParser :

    public TObject
    , public MXMLDocEvents
    , public MXMLDTDEvents
    , public MXMLErrorEvents
    , public MXMLEntityResolver
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TIndexList = TFundVector<tCIDLib::TCard4>;
        using TNodeList = TRefVector<const TXMLTreeElement>;


        class TErrInfo
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and operators
                // -----------------------------------------------------------
                TErrInfo
                (
                    const   tCIDLib::TErrCode   errcId
                    , const tCIDXML::EErrTypes  eType
                    , const TString&            strText
                    , const tCIDLib::TCard4     c4Column
                    , const tCIDLib::TCard4     c4Line
                    , const TString&            strSystemId) :

                    m_errcId(errcId)
                    , m_eType(eType)
                    , m_c4Column(c4Column)
                    , m_c4Line(c4Line)
                    , m_strSystemId(strSystemId)
                    , m_strText(strText)
                {
                }

                TErrInfo(const TErrInfo&) = default;
                TErrInfo(TErrInfo&&) = default;
                ~TErrInfo() = default;


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TErrInfo& operator=(const TErrInfo&)  =default;
                TErrInfo& operator=(TErrInfo&&)  =default;


                // -----------------------------------------------------------
                //  Public data members
                // -----------------------------------------------------------
                tCIDLib::TErrCode errcId() const { return m_errcId; }

                tCIDXML::EErrTypes eType() const { return m_eType; }

                const TString& strText() const { return m_strText; }

                tCIDLib::TCard4 c4Column() const { return m_c4Column; }

                tCIDLib::TCard4 c4Line() const { return m_c4Line; }

                const TString& strSystemId() const { return m_strSystemId; }


            private :
                // -----------------------------------------------------------
                //  Private data members
                // -----------------------------------------------------------
                tCIDLib::TErrCode   m_errcId;
                tCIDXML::EErrTypes  m_eType;
                TString             m_strText;
                tCIDLib::TCard4     m_c4Column;
                tCIDLib::TCard4     m_c4Line;
                TString             m_strSystemId;
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLTreeParser();

        TXMLTreeParser(const TXMLTreeParser&) = delete;
        TXMLTreeParser(TXMLTreeParser&&) = delete;

        ~TXMLTreeParser();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLTreeParser& operator=(const TXMLTreeParser&) = delete;
        TXMLTreeParser& operator=(TXMLTreeParser&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMapping
        (
            const   TXMLEntitySrc* const    pxsrcToAdopt
        );

        tCIDLib::TVoid AddMapping
        (
                    tCIDXML::TEntitySrcRef& esrToAdd
        );

        tCIDLib::TVoid AdoptDefExtSS
        (
                    TXMLEntitySrc*  const   pxesDefExtSS
            , const TString&                strDefRootElem
        );

        tCIDLib::TBoolean bFindAttr
        (
            const   TString&                strPath
            , const TString&                strAttrName
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFindText
        (
            const   TString&                strPath
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bKeepIgnorableSpace() const;

        tCIDLib::TBoolean bKeepIgnorableSpace
        (
            const   tCIDLib::TBoolean       bIgnore
        );

        tCIDLib::TBoolean bLockValidator() const;

        tCIDLib::TBoolean bLockValidator
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bParseRootEntity
        (
                    tCIDXML::TEntitySrcRef& esrRoot
            , const tCIDXML::EParseOpts     eOpts  = tCIDXML::EParseOpts::None
            , const tCIDXML::EParseFlags    eFlags = tCIDXML::EParseFlags::None
        );

        tCIDLib::TBoolean bParseRootEntity
        (
            const   TString&                strRootEntity
            , const tCIDXML::EParseOpts     eOpts  = tCIDXML::EParseOpts::None
            , const tCIDXML::EParseFlags    eFlags = tCIDXML::EParseFlags::None
        );

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4FindPath
        (
            const   TString&                strPath
            ,       TIndexList&             fcolNodeIndices
            , const TXMLTreeElement*&       pxtnodeTar
        )   const;

        const TVector<TErrInfo>& colErrors() const;

        tCIDLib::TVoid ClearCatalog();

        const TErrInfo& erriFirst() const;

        tCIDLib::TVoid InstallCatalog
        (
                    TXMLCatalog* const      pxcatToSet
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        [[nodiscard]] TXMLTreeDocument* pxtdocOrphan();

        const TXMLTreeElement* pxtnodeFindPath
        (
            const   TString&                strPath
            ,       tCIDLib::TCard4&        c4Index
        )   const;

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TBoolean       bFlushPools = kCIDLib::False
        );

        const TXMLCatalog& xcatMappings() const;

        const TXMLTreeDecl& xtdeclThis() const;

        const TXMLTreeDocument& xtdocThis() const;

        const TXMLTreeElement& xtnodeFindPath
        (
            const   TString&                strPath
            ,       tCIDLib::TCard4&        c4Index
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods (Document events)
        // -------------------------------------------------------------------
        tCIDLib::TVoid DocCharacters
        (
            const   TString&                strChars
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDXML::ELocations     eLocation
            , const tCIDLib::TBoolean       bAllSpaces
        )   final;

        tCIDLib::TVoid DocComment
        (
            const   TString&                strCommentText
            , const tCIDXML::ELocations     eLocation
        )   final;

        tCIDLib::TVoid DocPI
        (
            const   TString&                strTarget
            , const TString&                strValue
            , const tCIDXML::ELocations     eLocation
        )   final;

        tCIDLib::TVoid EndDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   final;

        tCIDLib::TVoid EndTag
        (
            const   TXMLElemDecl&           xdeclElem
        )   final;

        tCIDLib::TVoid ResetDocument() final;

        tCIDLib::TVoid ResetErrors() final;

        tCIDLib::TVoid StartDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   final;

        tCIDLib::TVoid StartTag
        (
                    TXMLParserCore&         xprsSrc
            , const TXMLElemDecl&           xdeclElem
            , const tCIDLib::TBoolean       bEmpty
            , const TVector<TXMLAttr>&      colAttrList
            , const tCIDLib::TCard4         c4AttrListSize
        )   final;

        virtual tCIDLib::TVoid XMLDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
            , const TString&                strStandalone
        )   final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (Document type events)
        // -------------------------------------------------------------------
        tCIDLib::TVoid DocType
        (
            const   TDTDElemDecl&           xdeclRoot
            , const TString&                strPublicId
            , const TString&                strSystemId
        )   final;

        tCIDLib::TVoid ResetDocType() final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (error events)
        // -------------------------------------------------------------------
        tCIDLib::TVoid HandleXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
            , const tCIDXML::EErrTypes      eType
            , const TString&                strText
            , const tCIDLib::TCard4         c4CurColumn
            , const tCIDLib::TCard4         c4CurLine
            , const TString&                strSystemId
        )   final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (entity resolver)
        // -------------------------------------------------------------------
        tCIDXML::TEntitySrcRef esrResolve
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
            , const TString&                strEntityName
            , const TString&                strParentId
            , const tCIDXML::EResolveTypes  eResType
        )   override;

        tCIDLib::TVoid ResetResolver() final;



    private :
        // -------------------------------------------------------------------
        //  Some ref vectors we use as pools for common types of nodes so
        //  that we can avoid repetitive allocation and deallocation.
        // -------------------------------------------------------------------
        using TTextPool = TRefVector<TXMLTreeText>;
        using TElemPool = TRefVector<TXMLTreeElement>;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bGotErrors
        //      This is set when any non-warning errors are seen coming through the
        //      error handler, and returned as the result of the parse method.
        //
        //  m_bKeepIgnorable
        //      Indicates whether we should keep ignorable whitespace node or just
        //      drop them on the floor.
        //
        //  m_eAdoptCat
        //      Indicates whether we own the catalog installed on us or not. If the
        //      catalog pointer is zero, this is ignored.
        //
        //  m_colElemPoolInd
        //  m_colTextPoolInd
        //      As we have to create new elements of the type that we keep pools of,
        //      we use this index to hand them out, and to know when we are out of
        //      pool items and need to start adding more.
        //
        //  m_fcolIndices
        //      An index list we use internally, mostly to support some convenience
        //      versions of the path search that just return a single node.
        //
        //  m_pcolElemStack
        //      This is the stack that we used to push each current element onto as
        //      we hit a child element that we need to process (which becomes the new
        //      current element.)
        //
        //  m_pcolErrors
        //      A vector of XML exception objects, into which are formatted
        //      all the errors that we got during parsing.
        //
        //  m_pxcatMappings
        //      The optional installable catalog for doing entity resolution. Any
        //      derivative of TXMLCatalog can be set. It can be adopted by this
        //      parser or not. We fault a standard one if none has been set explcitly.
        //
        //  m_pxtdocThis
        //      This is a pointer to the current document. The client can have us
        //      orphan this to him, but if not we reuse it on each parse by cleaning
        //      it out and re-starting.
        //
        //  m_pxtnodeCur
        //      While working through the tree via the callbacks, we have to
        //      remember the current working element. We use a stack of these
        //      in order to support the nesting of elements. If this is non-
        //      zero, we are inside the root element.
        //
        //  m_xprsThis
        //      The source core XML parser that we use to do the parsing.
        //      We just install ourself on it as a handler.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bGotErrors;
        tCIDLib::TBoolean           m_bKeepIgnorable;
        tCIDLib::TCard4             m_c4ElemPoolInd;
        tCIDLib::TCard4             m_c4TextPoolInd;
        mutable tCIDLib::EAdoptOpts m_eAdoptCat;
        mutable TIndexList          m_fcolIndices;
        TRefStack<TXMLTreeElement>* m_pcolElemStack;
        TVector<TErrInfo>*          m_pcolErrors;
        mutable TXMLCatalog*        m_pxcatMappings;
        mutable TXMLTreeDocument*   m_pxtdocThis;
        TXMLTreeElement*            m_pxtnodeCur;
        TElemPool                   m_colElemPool;
        TTextPool                   m_colTextPool;
        TXMLParserCore              m_xprsThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLTreeParser,TObject)
};

#pragma CIDLIB_POPPACK

