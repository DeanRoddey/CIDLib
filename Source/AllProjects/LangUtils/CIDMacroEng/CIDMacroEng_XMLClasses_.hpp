//
// FILE NAME: CIDMacroEng_XMLClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2005
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
//  This is the header file for the CIDMacroEng_XMLClass.cpp file, which
//  implements the info and value classes required wrap the CIDXML facility's
//  tree parser.
//
//  We also implement an 'anchor' class, which is used to allow the client
//  code to keep up with where they are in the tree with reasonable efficiency
//  and to support hierarchical traversal and such. We provide methods to get
//  anchors to specific paths through thetree, parent anchors, and child anchors.
//
//  We have to implement a very simple derivative of the tree parser class
//  so that we can override it's entity resolution method. XML based file
//  access is relative to a specific directory, so the files use a syntax
//  that starts with a slash and that represents the top of the CML file
//  structure. We have to remap them to the full paths.
//
//  In order to keep the system id in the CML file space, we also have to
//  create our own file entity source classes that we will return for any
//  file or file based URL queries. We can set the real file name on it
//  separately and leave the URL or file path alone, since that will show
//  up in any errors and is used for any relative entity references.
//
//  For any non-file based URLs, we just return a regular URL entity source.
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
//  CLASS: TMEngFileEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------
class TMEngFileEntitySrc : public TXMLEntitySrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFileEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strActualPath
        );

        TMEngFileEntitySrc(const TMEngFileEntitySrc&) = default;

        ~TMEngFileEntitySrc();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TBinInStream* pstrmMakeNew() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strActualPath
        //      The actual path that we open, as apposed to the system id
        //      which will be in CML path terms, and we want to leave it that
        //      way since it'll show up in any errors from the parser.
        // -------------------------------------------------------------------
        TString m_strActualPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFileEntitySrc,TXMLEntitySrc)
        DefPolyDup(TMEngFileEntitySrc)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParser
// PREFIX: xtprs
// ---------------------------------------------------------------------------
class TMEngXMLTreeParser : public TXMLTreeParser
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngXMLTreeParser
        (
                    TCIDMacroEngine* const  pmeOwner
        );

        TMEngXMLTreeParser(const TMEngXMLTreeParser&) = delete;

        ~TMEngXMLTreeParser();


    protected :
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


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pmeOwner
        //      The macro engine that created the parser value object that
        //      created this object. WE need it to expand any CML file paths
        //      to full file paths.
        // -------------------------------------------------------------------
        TCIDMacroEngine*    m_pmeOwner;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngXMLTreeParser, TXMLTreeParser)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeAnchorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngXMLTreeAnchorVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngXMLTreeAnchorVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngXMLTreeAnchorVal(const TMEngXMLTreeAnchorVal&) = delete;

        ~TMEngXMLTreeAnchorVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngXMLTreeAnchorVal& operator=(const TMEngXMLTreeAnchorVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid
        (
            const   tCIDLib::TCard4         c4ParserId
            , const tCIDLib::TCard4         c4ParseId
        )   const;

        tCIDLib::TBoolean bNextPrevChild
        (
                    TMEngXMLTreeAnchorVal&  mecvToUpdate
            , const tCIDLib::TBoolean       bNext
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAnchorInfo
        (
            const   tCIDLib::TCard4         c4ParserId
            , const tCIDLib::TCard4         c4ParseId
            , const TXMLTreeElement* const  pxtnodeToSet
            , const tCIDLib::TCard4         c4Index
        );

        const TXMLTreeElement& xtnodeValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c4Index
        //      Node index within the parent element, mostly for the next/prev
        //      child method, so that we can easily know where we are within
        //      the parent. For the root node it's set to max card.
        //
        //  m_c4ParseId
        //  m_c4ParserId
        //      We have two ids that insure that this anchor is only used with
        //      the parser object that it was gotten from and only with the
        //      specific parsed tree that it was gotten from. The parser object
        //      will set these on us when this anchor is set and will throw
        //      an exception if it's used when it's out of date.
        //
        //  m_pxtnodeValue
        //      The element node that this anchor represents.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Index;
        tCIDLib::TCard4         m_c4ParseId;
        tCIDLib::TCard4         m_c4ParserId;
        const TXMLTreeElement*  m_pxtnodeValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngXMLTreeAnchorVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeAnchorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngXMLTreeAnchorInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngXMLTreeAnchorInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngXMLTreeAnchorInfo(const TMEngXMLTreeAnchorInfo&) = delete;

        ~TMEngXMLTreeAnchorInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngXMLTreeAnchorInfo& operator=(const TMEngXMLTreeAnchorInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngXMLTreeAnchorInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParserVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngXMLTreeParserVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngXMLTreeParserVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes eConst
            ,       TCIDMacroEngine* const  pmeOwner
        );

        TMEngXMLTreeParserVal(const TMEngXMLTreeParserVal&) = delete;

        ~TMEngXMLTreeParserVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngXMLTreeParserVal& operator=(const TMEngXMLTreeParserVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckAnchor
        (
            const   TMEngXMLTreeAnchorVal&  mecvAnchor
        )   const;

        tCIDLib::TBoolean bGetChildAnchor
        (
            const   TMEngXMLTreeAnchorVal&  mecvParent
            , const tCIDLib::TCard4         c4Index
            ,       TMEngXMLTreeAnchorVal&  mecvToSet
        )   const;

        tCIDLib::TBoolean bGetRootAnchor
        (
                    TMEngXMLTreeAnchorVal&  mecvToSet
        )   const;

        tCIDLib::TCard4 c4Parse
        (
                    tCIDXML::TEntitySrcRef& esrToParse
        );

        tCIDLib::TVoid InitIds
        (
                    TCIDMacroEngine&        meOwner
        );

        const TXMLTreeElement* pxtnodeFindPath
        (
            const   TString&                strPath
            ,       tCIDLib::TCard4&        c4Index
        )   const;

        tCIDLib::TVoid SetAnchor
        (
            const   TString&                strPath
            ,       TMEngXMLTreeAnchorVal&  mecvToSet
        )   const;

        tCIDLib::TVoid SetAnchorInfo
        (
            const   TXMLTreeElement* const  pxtnodeToSet
            , const tCIDLib::TCard4         c4Index
            ,       TMEngXMLTreeAnchorVal&  mecvToSet
        )   const;

        tCIDLib::TVoid SetDefOptions();

        tCIDLib::TVoid SetOption
        (
            const   tCIDXML::EParseOpts     eOption
            , const tCIDLib::TBoolean       bToSet
        );

        const TXMLTreeDocument& xtdocThis() const;

        const TXMLTreeParser& xtprsValue() const;

        TXMLTreeParser& xtprsValue();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c4ParseId
        //      Each time we do a parse, this is bumped. Any achors created
        //      from this parser get this parse id. This insures that we
        //      can catch if one is out of date.
        //
        //  m_c4ParserId
        //      Each new parser instance gets an id from a running counter,
        //      so that between this and the parse id above, we have a way
        //      to mark anchors such that we know that they are not out of
        //      date before we use them.
        //
        //  m_eOpts
        //  m_eFlags
        //      The options and flags that have been set.
        //
        //  m_xtprsValue
        //      The XML tree parser object that provides the underlying
        //      parsing and tree storage.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ParseId;
        tCIDLib::TCard4         m_c4ParserId;
        tCIDXML::EParseOpts     m_eOpts;
        tCIDXML::EParseFlags    m_eFlags;
        TMEngXMLTreeParser      m_xtprsValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngXMLTreeParserVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParserInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngXMLTreeParserInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngXMLTreeParserInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngXMLTreeParserInfo(const TMEngXMLTreeParserInfo&) = delete;

        ~TMEngXMLTreeParserInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngXMLTreeParserInfo& operator=(const TMEngXMLTreeParserInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckAnchor
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngXMLTreeParserVal&  mecvParser
            , const TMEngXMLTreeAnchorVal&  mecvAnchor
        );

        tCIDLib::TVoid CheckPath
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strToCheck
            ,       TPathStr&               pathToFill
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We create some enum nested types, so we store the ids here
        //      for quick access later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c2TypeId_Anchor
        //      We use the anchor class in our interface, so we need to look
        //      up its class id.
        //
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciXXX
        //      Some class info objects for some classes we want to interact with
        //      particularly much.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2EnumId_Errors;
        tCIDLib::TCard2         m_c2EnumId_NodeTypes;
        tCIDLib::TCard2         m_c2MethId_ChildExists;
        tCIDLib::TCard2         m_c2MethId_DefCtor;
        tCIDLib::TCard2         m_c2MethId_FindChildByName;
        tCIDLib::TCard2         m_c2MethId_FormatTo;
        tCIDLib::TCard2         m_c2MethId_GetAnchor;
        tCIDLib::TCard2         m_c2MethId_GetAttribute;
        tCIDLib::TCard2         m_c2MethId_GetAttribute2;
        tCIDLib::TCard2         m_c2MethId_GetChildAnchor;
        tCIDLib::TCard2         m_c2MethId_GetChildText;
        tCIDLib::TCard2         m_c2MethId_GetElemName;
        tCIDLib::TCard2         m_c2MethId_GetErrorAt;
        tCIDLib::TCard2         m_c2MethId_GetRootAnchor;
        tCIDLib::TCard2         m_c2MethId_NextPrevChild;
        tCIDLib::TCard2         m_c2MethId_ParseBuf;
        tCIDLib::TCard2         m_c2MethId_ParseFile;
        tCIDLib::TCard2         m_c2MethId_ParseString;
        tCIDLib::TCard2         m_c2MethId_SetDefOptions;
        tCIDLib::TCard2         m_c2MethId_SetOption;
        tCIDLib::TCard2         m_c2TypeId_Anchor;
        tCIDLib::TCard4         m_c4ErrAttrNotFound;
        tCIDLib::TCard4         m_c4ErrBadAnchor;
        tCIDLib::TCard4         m_c4ErrBadEntityPath;
        tCIDLib::TCard4         m_c4ErrBadErrIndex;
        tCIDLib::TCard4         m_c4ErrChildNotFound;
        tCIDLib::TCard4         m_c4ErrElemNotFound;
        tCIDLib::TCard4         m_c4ErrNoDocument;
        tCIDLib::TCard4         m_c4ErrParseFailed;
        tCIDLib::TCard4         m_c4ErrPathLookup;
        tCIDLib::TCard4         m_c4ErrPrint;
        TMEngEnumInfo*          m_pmeciErrors;
        TMEngEnumInfo*          m_pmeciNodeTypes;
        TMEngEnumInfo*          m_pmeciParseOpts;
        TMEngTextOutStreamInfo* m_pmeciStream;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngXMLTreeParserInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


