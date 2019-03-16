//
// FILE NAME: CIDMacroEng_Parser.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_Parser.cpp file, which
//  implements the TMacroEngParser class. This class provides the parsing of
//  macro text and the creation of the appropriate parsed representation of
//  the resulting macro into the passed macro engine.
//
//  You'll have to create a derivative of this class in order to handle
//  resolving references to classes, since this parser doesn't have a clue
//  where they will be. The class path is used to load them, so they will
//  generally be stored under some directory in a structure that derives
//  directly from the classpath hierarchy.
//
//  The derived class just needs to return an allocated input text stream
//  of some sort that provides access to the class data. The macro engine
//  will use it to parse the class, and then delete it.
//
//  Since parsing is recursive, this class works in terms of an internal
//  class, TParserSrc, which represents a single class source to be parsed.
//  This class can create them as required to parse imported classes (if they
//  are not already.) Each parser source object provides the core methods to
//  parse tokens and chars from the text, keep up with the current line/col
//  info, unget tokens and chars, stores the class path of the thing being
//  parsed, etc... This class works in terms of those objects and provides
//  the higher level parsing helper classes that use a passed parser source
//  object to do their work.
//
//  We also use a string pool for temp strings, in order to maintain sanity
//  in this recursive environment. Like the parser source we forward ref it
//  and keep it's impl internal.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TParserSrc;
class TMEngStringPool;
class TMEngFlowCtrlItem;


// ---------------------------------------------------------------------------
//  CLASS: TMacroEngParser
// PREFIX: meprs
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMacroEngParser : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDMacroEng::ENumTypes eCheckNumericLiteral
        (
                    TString&                strLit
            ,       tCIDLib::ERadices&      eRadix
            ,       tCIDLib::TBoolean&      bExplicit
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMacroEngParser
        (
            const   tCIDMacroEng::EOptLevels   eOpt = tCIDMacroEng::EOptLevels::Minimal
        );

        TMacroEngParser(const TMacroEngParser&) = delete;

        ~TMacroEngParser();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMacroEngParser& operator=(const TMacroEngParser&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParse
        (
            const   TString&                strMainClass
            ,       TMEngClassInfo*&        pmeciMainClass
            ,       TCIDMacroEngine* const  pmeTarget
            ,       MMEngPrsErrHandler* const pmeehToUse
            ,       MMEngClassMgr* const    pmecmToUse
        );

        tCIDLib::TBoolean bParse
        (
            const   TString&                strMainClass
            ,       TMemBuf&                mbufContent
            , const tCIDLib::TCard4         c4Bytes
            , const TString&                strEncoding
            ,       TMEngClassInfo*&        pmeciMainClass
            ,       TCIDMacroEngine* const  pmeTarget
            ,       MMEngPrsErrHandler* const pmeehToUse
            ,       MMEngClassMgr* const    pmecmToUse
        );


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TMatchList    = TRefVector<TMEngClassInfo>;
        using TFlowStack    = TStack<TMEngFlowCtrlItem>;
        using TClassStack   = TStack<TString>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckBegin
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::Yes
        );

        tCIDLib::TBoolean bCheckCloseParen
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckCloseBracket
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckColon
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckComma
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::Yes
        );

        tCIDLib::TBoolean bCheckEquals
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::Yes
        );

        tCIDLib::TBoolean bCheckFlowStack
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ETokens   eTokenSeen
            ,       TMEngFlowCtrlItem&      mefciToFill
            , const tCIDLib::TBoolean       bPop
        );

        tCIDLib::TBoolean bCheckNextToken
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ETokens   eToCheck
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckOpenBracket
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckOpenParen
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckPeriod
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bCheckSemiColon
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::Yes
        );

        tCIDLib::TBoolean bGetBoolToken
        (
                    TParserSrc&             psrcClass
            ,       tCIDLib::TBoolean&      bResult
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bGetClassPath
        (
                    TParserSrc&             psrcClass
            ,       TString&                strToFill
            ,       tCIDLib::TCard4&        c4LastSep
            , const tCIDLib::TBoolean       bAllowName = kCIDLib::False
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bGetClassPath
        (
                    TParserSrc&             psrcClass
            ,       TString&                strToFill
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bGetNameToken
        (
                    TParserSrc&             psrcClass
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bKeywordOk
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::Yes
        );

        tCIDLib::TBoolean bGetQuotedString
        (
                    TParserSrc&             psrcClass
            ,       TString&                strToFill
            , const tCIDMacroEng::ERecovers eRecover = tCIDMacroEng::ERecovers::No
        );

        tCIDLib::TBoolean bIsOnNestingStack
        (
            const   TString&                strClassPath
        )   const;

        tCIDLib::TBoolean bLastFlowItem
        (
            const   tCIDMacroEng::EFlowTypes eType
            ,       TMEngFlowCtrlItem*&     pmefciToFill
        );

        tCIDLib::TBoolean bLastLoopedFlowItem
        (
                    TMEngFlowCtrlItem*&     pmefciToFill
        );

        tCIDLib::TBoolean bPushNumericLiteral
        (
                    TParserSrc&             psrcClass
            ,       TString&                strLiteral
            ,       TMEngOpMethodImpl&      methToFill
            ,       tCIDLib::TCard2&        c2ClassId
        );

        tCIDLib::TCard2 c2FindCorrectCtor
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciTarget
            ,       TMEngClassInfo&         meciToFill
            ,       TMEngMethodInfo&        methiTarget
            ,       TMEngOpMethodImpl&      methToFill
            ,       tCIDLib::TCard4&        c4ParmCnt
        );

        tCIDLib::TCard2 c2ParseCallSequence
        (
                    TParserSrc&             psrcClass
            , const TMEngClassInfo&         meciTarget
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngOpMethodImpl&      methToFill
            ,       tCIDMacroEng::EConstTypes& eConst
            , const tCIDMacroEng::ERefTypes eRefType
            , const TMEngClassInfo&         meciRef
            , const tCIDLib::TCard2         c2RefId
            ,       tCIDMacroEng::EExprTypes& eTypeParsed
        );

        tCIDLib::TCard2 c2ParseExpr
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::EExprSides eSide
            , const TMEngClassInfo&         meciTarget
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngOpMethodImpl&      methToFill
            ,       tCIDMacroEng::EExprTypes& eTypeParsed
            ,       tCIDLib::TBoolean&      bDirectRef
        );

        tCIDLib::TCard2 c2ParseFactor
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::EExprSides eSide
            , const TMEngClassInfo&         meciTarget
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngOpMethodImpl&      methToFill
            ,       tCIDMacroEng::EExprTypes& eTypeParsed
            ,       tCIDLib::TBoolean&      bDirectRef
        );

        tCIDLib::TVoid CheckLHS
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::EExprSides eSide
        );

        tCIDLib::TVoid CheckRHS
        (
                    TParserSrc&             psrcClass
            , const tCIDMacroEng::EExprSides eSide
        );

        tCIDMacroEng::EOpResults eGetOpInfo
        (
            const   TMEngClassInfo&         meciTarget
            , const tCIDMacroEng::ETokens   eToken
            ,       tCIDLib::TCard2&        c2MethId
        );

        tCIDLib::TVoid GenDefMemberCtor
        (
                    TParserSrc&             psrcClass
            ,       TMEngMemberInfo&        memiTarget
            ,       TMEngOpMethodImpl&      methToFill
        );

        tCIDLib::TVoid GenDefParentCtor
        (
                    TParserSrc&             psrcClass
            , const TMEngClassInfo&         meciTarget
            ,       TMEngOpMethodImpl&      methToFill
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const TString&                strText
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const tCIDLib::TErrCode       errcToIssue
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const tCIDLib::TErrCode       errcToIssue
            , const MFormattable&           mfmtblToken1
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const tCIDLib::TErrCode       errcToIssue
            , const MFormattable&           mfmtblToken1
            , const MFormattable&           mfmtblToken2
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const tCIDLib::TErrCode       errcToIssue
            , const MFormattable&           mfmtblToken1
            , const MFormattable&           mfmtblToken2
            , const MFormattable&           mfmtblToken3
        );

        tCIDLib::TVoid IssueErr
        (
            const   TParserSrc&             psrcClass
            , const tCIDLib::TErrCode       errcToIssue
            , const MFormattable&           mfmtblToken1
            , const MFormattable&           mfmtblToken2
            , const MFormattable&           mfmtblToken3
            , const MFormattable&           mfmtblToken4
        );

        tCIDLib::TVoid IssueExcept
        (
            const   TParserSrc&             psrcClass
            , const TError&                 errCaught
        );

        tCIDLib::TVoid IssueExcept
        (
            const   TParserSrc&             psrcClass
        );

        TMEngClassInfo& meciResolvePath
        (
                    TParserSrc&             psrcClass
            , const TString&                strPath
        );

        TMEngClassInfo* pmeciCheckClassLoad
        (
                    TParserSrc&             psrcClass
            , const TString&                strClassPath
        );

        TMEngClassInfo* pmeciParseClass
        (
                    TParserSrc&             psrcClass
        );

        TMEngClassInfo* pmeciResolveRef
        (
                    TParserSrc&             psrcClass
            , const TString&                strToFind
            , const TMEngClassInfo&         meciSrc
            , const TMEngMethodInfo&        methiSrc
            , const TMEngMethodImpl&        methSrc
            , const TMEngLiteralVal*&       pmelvFound
            ,       tCIDMacroEng::EConstTypes& eConst
            ,       tCIDLib::TCard2&        c2Id
            ,       tCIDMacroEng::ERefTypes& eRefType
        );

        tCIDLib::TVoid ParseDirectives
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
        );

        tCIDLib::TVoid ParseImports
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
        );

        tCIDLib::TVoid ParseInitializers
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
            ,       TMEngMethodInfo&        methiTarget
            ,       TMEngOpMethodImpl&      methToFilll
        );

        tCIDLib::TVoid ParseLiterals
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
        );

        tCIDLib::TVoid ParseLocals
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
            ,       TMEngMethodInfo&        methiToFill
            ,       TMEngOpMethodImpl&      methToFill
        );

        tCIDLib::TVoid ParseMembers
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
        );

        tCIDLib::TVoid ParseMethods
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciToFill
        );

        tCIDLib::TVoid ParseMethodBody
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciWorking
            ,       TMEngMethodInfo&        methiWorking
            ,       TMEngOpMethodImpl&      methToFill
        );

        tCIDLib::TVoid ParseMethodParms
        (
                    TParserSrc&             psrcClass
            , const TMEngClassInfo&         meciTarget
            ,       TMEngMethodInfo&        methiToFill
        );

        tCIDLib::TVoid ParseTypes
        (
                    TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciOwner
        );

        tCIDLib::TVoid PopFlowStack
        (
            const   TParserSrc&             psrcClass
            ,       TMEngFlowCtrlItem&      mefciToFill
        );

        tCIDLib::TVoid ParseSwitchCase
        (
            const   tCIDMacroEng::ETokens   eOrgToken
            ,       TParserSrc&             psrcClass
            ,       TMEngClassInfo&         meciWorking
            ,       TMEngMethodInfo&        methiWorking
            ,       TMEngOpMethodImpl&      methToFill
        );

        TMEngClassVal* pmecvMakeNumericLiteral
        (
                    TParserSrc&             psrcClass
            ,       TString&                strLit
            ,       tCIDLib::TCard2&        c2ClassId
            , const TString&                strName
            , const tCIDMacroEng::ENumTypes eExpected = tCIDMacroEng::ENumTypes::None
        );

        tCIDLib::TVoid RecursiveExport
        (
            const   TMEngClassInfo&         meciStart
            ,       TMEngClassInfo&         meciTarget
        );

        tCIDLib::TVoid ThrowUnrecoverable();

        tCIDLib::TVoid ValidateClass
        (
                    TParserSrc&             psrcClass
            , const TMEngClassInfo&         meciToVal
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bInCatch
        //      Some special stuff is available if we are inside a Catch
        //      block, so this flag is set while in that block so that we
        //      know when they are legal.
        //
        //  m_c4ErrCount
        //      We'll try to keep parsing where possible, and keep a count
        //      of errors. At the end, we check this to see if it failed
        //      or not.
        //
        //  m_colClassStack
        //      We have to avoid circular imports, or we'll freak out and
        //      eat up all the memory. So we keep a stack that tracks our
        //      current nesting of parsed classes and check it on each new
        //      class to make sure it's not already on the stack. If so,
        //      then there's a circular dependency.
        //
        //  m_colFlowStack
        //      A stack of flow control items, which is used to handle the
        //      resolution of nested flow control structures, to make sure
        //      that htey are nested correctly, and to handle generating the
        //      back/forward jumps they generally require.
        //
        //  m_colMatches
        //      A non-adopting ref vector used by the meciResolvePath()
        //      method. It calls a path resolution method on the target
        //      engine, which will return multiple matches if the type is
        //      ambiguous, so our method must provide a collection to return
        //      the matches in. To avoid creating and destroying it on the
        //      fly, we use this temp one. Parsing is single threaded per
        //      parser instance, so this is fine.
        //
        //  m_meopToUse
        //      A temp opcode object to use for all opcode generation. We
        //      never need more than one at a time, so it's better to use
        //      a member rather than pass it all over or create and destroy
        //      them all over the place.
        //
        //  m_pitResData
        //      We use this resolution data to figure out which overloaded
        //      method is being targeted by a particular set of parameters.
        //      The class info class provides a method to fill this table with
        //      the parms of all of the available ctors.
        //
        //  m_pmecmToUse
        //      A pointer to the class manager to use while parsing. We don't
        //      own it, we just use it.
        //
        //  m_pmeehToUse
        //      A pointer to an error handler to pass parser errors to. We
        //      don't own it, we just use it.
        //
        //  m_pmeTarget
        //      A pointer to the macro engine that we were give to fill in
        //      with the results of the parse.
        //
        //  m_strThisName
        //      This is set to the special name of the current object. to
        //      speed up comparisons more so than using a literal.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bInCatch;
        tCIDLib::TCard4             m_c4ErrCount;
        TClassStack                 m_colClassStack;
        TFlowStack                  m_colFlowStack;
        TMatchList                  m_colMatches;
        tCIDMacroEng::EOptLevels    m_eOptLevel;
        TMEngOpCode                 m_meopToUse;
        tCIDMacroEng::TParmIdTable  m_pitResData;
        MMEngClassMgr*              m_pmecmToUse;
        MMEngPrsErrHandler*         m_pmeehToUse;
        TCIDMacroEngine*            m_pmeTarget;
        TMEngStringPool*            m_pmsplParse;
        TString                     m_strThisName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMacroEngParser,TObject)
};

#pragma CIDLIB_POPPACK

inline tCIDLib::TBoolean
TMacroEngParser::bGetClassPath(         TParserSrc&             psrcClass
                                ,       TString&                strToFill
                                , const tCIDMacroEng::ERecovers eRecover)
{
    tCIDLib::TCard4 c4Dummy;
    return bGetClassPath(psrcClass, strToFill, c4Dummy, kCIDLib::False, eRecover);
}

