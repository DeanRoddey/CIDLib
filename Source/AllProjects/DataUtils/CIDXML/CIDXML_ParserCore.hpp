//
// FILE NAME: CIDXML_ParserCore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_ParserCore.Cpp file. This
//  file implements the the basic logic that knows how to internalize external
//  XML entities (the largest unit of XML, which is usually a file but does
//  not have to be) and confirm that the text in them meets basic XML 1.0
//  rules of well formedness. Each legal chunk of markup that is successfully
//  decoded is passed to a callback virtual method to be processed by a
//  derived class that wants to do specific things with this information.
//
//  In order to do this work, each instance of a core parser must manage a
//  set of basic data structures, then major ones being:
//
//  1)  An entity manager - This is where entities are stored in a stack
//      fashion (since they can be nested) as they are spooled out to be
//      parsed. It manages TXMLEntSpooler objects, each of which represents
//      a parseable XML unit (which can be external or internal.)
//
//  2)  A validator. All XML can be optionally validated against one or more
//      types of descriptions of the structure and content of the XML data.
//      The validator is managed via an abtract API, since there can be more
//      than one scheme for describing XML. The validator also contains all
//      of the decl information from the structural description (.e.g DTD or
//      Schema, so we have one even if not validating.)
//
//  3)  An element stack. XML is composed of a nested element structure,
//      and information must be stored at each level. So this stack is used
//      to store that information as elements are entered and exited.
//
//  4)  A buffer manager which manages a set of strings that can be used
//      for temporary text buffering. It allows temporary use of buffers
//      without worry of reuse in nested calls and without having to allocate
//      and free the constantly.
//
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
//  CLASS: TXMLParserCore
// PREFIX: xprs
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLParserCore : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLParserCore();

        ~TXMLParserCore();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        //
        //  Some of the grunt work methods here are implemented in the
        //  CIDXML_ParserCore3.Cpp file.
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptDefExtSS
        (
                    TXMLEntitySrc*  const   pxesDefExtSS
            , const TString&                strDefRootElem
        );

        tCIDLib::TBoolean bInfoWanted
        (
            const   tCIDXML::EParseFlags    eMask
        )   const;

        tCIDLib::TBoolean bLockValidator() const;

        tCIDLib::TBoolean bLockValidator
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bStandalone() const;

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4MaxErrors() const;

        tCIDLib::TCard4 c4MaxErrors
        (
            const   tCIDLib::TCard4         c4MaxToSet
        );

        tCIDXML::EParseFlags eFlags() const;

        tCIDXML::ELocations eLocation() const;

        tCIDXML::ELocations eLocation
        (
            const   tCIDXML::ELocations     eLocation
        );

        tCIDXML::EParseOpts eOptions() const;

        TXMLBufMgr& xbmThis();

        TXMLEntityMgr& xemThis();

        MXMLDocEvents* pmxevDocEvents() const;

        MXMLDocEvents* pmxevDocEvents
        (
                    MXMLDocEvents* const    pmxevDocEvents
        );

        MXMLEntityEvents* pmxevEntityEvents() const;

        MXMLEntityEvents* pmxevEntityEvents
        (
                    MXMLEntityEvents* const pmxevToSet
        );

        MXMLEntityResolver* pmxevEntityResolver() const;

        MXMLEntityResolver* pmxevEntityResolver
        (
                    MXMLEntityResolver* const pmxevToSet
        );

        MXMLErrorEvents* pmxevErrorEvents() const;

        MXMLErrorEvents* pmxevErrorEvents
        (
                    MXMLErrorEvents* const  pmxevErrorEvents
        );

        TXMLValidator* pxvalValidator() const;

        TXMLValidator* pxvalValidator
        (
                    TXMLValidator* const    pxvalToAdopt
        );

        tCIDLib::TVoid ParseRootEntity
        (
                    tCIDXML::TEntitySrcRef& esrcRoot
            , const tCIDXML::EParseOpts     eOpts  = tCIDXML::EParseOpts::None
            , const tCIDXML::EParseFlags    eFlags = tCIDXML::EParseFlags::None
        );

        tCIDLib::TVoid ParseRootEntity
        (
            const   TString&                strRootEntity
            , const tCIDXML::EParseOpts     eOpts  = tCIDXML::EParseOpts::None
            , const tCIDXML::EParseFlags    eFlags = tCIDXML::EParseFlags::None
        );

        tCIDLib::TVoid ParseRootEntity
        (
            const   TString&                strSrcName
            , const TMemBuf&                mbufRootEntity
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDXML::EParseOpts     eOpts  = tCIDXML::EParseOpts::None
            , const tCIDXML::EParseFlags    eFlags = tCIDXML::EParseFlags::None
        );

        tCIDLib::TVoid PostXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
        );

        tCIDLib::TVoid PostXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
            , const MFormattable&           mftblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid ThrowParseError
        (
            const   tCIDLib::TErrCode       errcToThrow
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  Some of the grunt work methods here are implemented in the
        //  CIDXML_ParserCore2.Cpp file.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseAttrValue
        (
            const   TXMLAttrDef&            xadCur
            ,       TString&                strValue
        );

        tCIDLib::TBoolean bParseEqualSign();

        tCIDLib::TBoolean bExpandCharRef
        (
                    tCIDLib::TCh&           chToFill
            ,       tCIDLib::TCh&           chSecond
        );

        tCIDXML::EEntityRes eExpandEntityRef
        (
            const   tCIDLib::TBoolean       bExtOk
            ,       tCIDLib::TCh&           chToFill
            ,       tCIDLib::TCh&           chSecond
            ,       tCIDLib::TBoolean&      bEscaped
            , const tCIDXML::ERefFrom       eFrom
        );

        tCIDLib::TBoolean bParseEndTag
        (
            const   tCIDLib::TCard4         c4SpoolerId
        );

        tCIDLib::TBoolean bParseStartTag
        (
            const   tCIDLib::TCard4         c4SpoolerId
        );

        tCIDXML::EMarkupTypes eNextMarkupType
        (
                    tCIDLib::TCard4&        c4OrgSpooler
        );

        tCIDLib::TVoid ParseChars();

        tCIDLib::TVoid ParseComment();

        tCIDLib::TVoid ParseContent
        (
            const   tCIDLib::TBoolean       bInExtEntity
        );

        tCIDLib::TVoid ParseCDATA();

        tCIDLib::TVoid ParseExtDecl
        (
            const   tCIDXML::EDeclTypes     eType
        );

        tCIDLib::TVoid ParsePI();

        tCIDLib::TVoid ParsePostContent();

        tCIDLib::TVoid ParsePreContent();

        tCIDLib::TVoid PassContentChars
        (
                    TString&                strChars
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bInException
        //      We use this flag to avoid circular problems between the
        //      PostXMLError() method and the catch blocks. Since they both
        //      can call each other, we have to stop them (in some cases)
        //      from doing so.
        //
        //  m_bStandalone
        //      This is driven by the standalone="" decl string. It defaults
        //      to false, which it stays if the decl does not set it.
        //
        //  m_bValidatorLocked
        //      In order to avoid reparsing the same DTD over and over, the
        //      calling code and do one parse, and then ask to lock the
        //      validate. At that point we won't reset it anymore and will
        //      not parse any DTD content. We'll just keep using the same
        //      validator over and over. This can be a big performance
        //      win in many programs. They can unlock it at any time which
        //      will start resetting it and parsing it again.
        //
        //  m_c4ErrorCount
        //  m_c4ErrorMax
        //      The parser is mostly designed to try to keep going in the face
        //      of errors. This flag indicates how many to try to recover from
        //      before giving up. It defaults to 1, until set to something
        //      else.
        //
        //  m_eFlags
        //      These are flags given in the ParseRootEntity() method. These
        //      flags control what info we will report via callbacks.
        //
        //  m_eLocation
        //      This is kept up to date to indicate where the parsing process
        //      currently is. This is used know whether certain information
        //      is to be reported and do other position sensitive stuff.
        //
        //  m_eOptions
        //      These are flags given in the ctor that tells us what optional
        //      modes of operation we should implement (.e.g. validate, do
        //      namespaces, etc...)
        //
        //  m_esrDefExtSS
        //      We can be give an entity source that will be used to provide
        //      a default 'external subset' (whatever that means for the type
        //      of validator) if the parser doesn't find one referenced in
        //      the document. They can call our AdoptExtSS() method to set
        //      one. This is ignored if the ignore DTD flag is set or the
        //      validator is locked.
        //
        //  m_pcolAttrList
        //      This is a pointer to a vector that is used to store the attrs
        //      of a start tag until they can be passed out in the start tag
        //      event. Its grown as required.
        //
        //  m_pmxevDocEvents
        //  m_pmxevEntityEvents
        //  m_pmxevEntityResolover
        //  m_pmxevErrEvents
        //      These are pointers to the error, document, and event event
        //      callback objects, if any. If they never installed one, then
        //      its zero. Some of these must also be passed on to member
        //      objects.
        //
        //  m_pxbmBuf
        //      This is the text buffer manager object, which controls a pool
        //      of text accumulation strings for temporary use during parsing.
        //
        //  m_pxvalValidator
        //      This is the current validator that we are using. We look at
        //      it via a core abstraction so we don't know what type of
        //      validator it really is.
        //
        //  m_strChars
        //  m_strName
        //  m_strbValue
        //      Though most string buffers are querying from the buffer pool
        //      in a dynamic manner, this small set are dedicated for the main
        //      work done during the primary content parsing area of the
        //      input. This will provide a nice boost in performance.
        //
        //  m_strDefRootElem
        //      If we are given a default external subset, we are also given
        //      a default root element to expect, since there is no real
        //      DOCTYPE to indicate this. It is just stored here to pass to
        //      the validator later if needed.
        //
        //  m_xcsElems
        //      This is the context stack in which state information is kept
        //      about the current path through the element hiearchy during
        //      content parsing.
        //
        //  m_xemThis
        //      This is the XML entity manager. We use it to keep track of
        //      the (possibly nested) entities that we have to parse.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bInException;
        tCIDLib::TBoolean       m_bStandalone;
        tCIDLib::TBoolean       m_bValidatorLocked;
        tCIDLib::TCard4         m_c4ErrorCount;
        tCIDLib::TCard4         m_c4ErrorMax;
        tCIDXML::EParseFlags    m_eFlags;
        tCIDXML::ELocations     m_eLocation;
        tCIDXML::EParseOpts     m_eOptions;
        TVector<TXMLAttr>*      m_pcolAttrList;
        tCIDXML::TEntitySrcRef  m_esrDefExtSS;
        MXMLDocEvents*          m_pmxevDocEvents;
        MXMLEntityEvents*       m_pmxevEntityEvents;
        MXMLEntityResolver*     m_pmxevEntityResolver;
        MXMLErrorEvents*        m_pmxevErrEvents;
        TXMLBufMgr*             m_pxbmBuf;
        TXMLValidator*          m_pxvalValidator;
        TString                 m_strChars;
        TString                 m_strDefRootElem;
        TString                 m_strName;
        TString                 m_strValue;
        TXMLContextStack        m_xcsElems;
        TXMLEntityMgr           m_xemThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLParserCore,TObject)
};

#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//  TXMLParserCore: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TXMLParserCore::bLockValidator() const
{
    return m_bValidatorLocked;
}

inline tCIDLib::TBoolean
TXMLParserCore::bLockValidator(const tCIDLib::TBoolean bToSet)
{
    m_bValidatorLocked = bToSet;
    return m_bValidatorLocked;
}

inline tCIDLib::TBoolean TXMLParserCore::bStandalone() const
{
    return m_bStandalone;
}

inline tCIDLib::TCard4 TXMLParserCore::c4MaxErrors() const
{
    return m_c4ErrorMax;
}

inline tCIDLib::TCard4
TXMLParserCore::c4MaxErrors(const tCIDLib::TCard4 c4MaxToSet)
{
    m_c4ErrorMax = c4MaxToSet;
    return m_c4ErrorMax;
}

inline tCIDXML::EParseFlags TXMLParserCore::eFlags() const
{
    return m_eFlags;
}

inline tCIDXML::ELocations TXMLParserCore::eLocation() const
{
    return m_eLocation;
}

inline tCIDXML::ELocations
TXMLParserCore::eLocation(const tCIDXML::ELocations eLocation)
{
    m_eLocation = eLocation;
    return m_eLocation;
}

inline tCIDXML::EParseOpts TXMLParserCore::eOptions() const
{
    return m_eOptions;
}

inline TXMLBufMgr& TXMLParserCore::xbmThis()
{
    return *m_pxbmBuf;
}

inline TXMLEntityMgr& TXMLParserCore::xemThis()
{
    return m_xemThis;
}

