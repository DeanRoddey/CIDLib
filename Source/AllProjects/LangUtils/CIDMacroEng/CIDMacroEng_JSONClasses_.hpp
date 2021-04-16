//
// FILE NAME: CIDMacroEng_JSONClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file and its contents are the property of Charmed Quark Software. It
//  may not be redistributed in any modified form, or as part of any product
//  other than the original CIDLib C++ Frameworks distribution file. Its
//  contents may not be compiled into any other product, except when required
//  by use of the CIDLib C++ Frameworks product. Such use in any Sockercial
//  product is subject to any relevant licensing agreements.
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_JSONClass.cpp file, which
//  implements the info and value classes required to create new macro level
//  classes to wrap the C++ JSON parse stuff.
//
//  We use the underlying C++ classes via pointers so that we don't have to
//  impose the CIDNet headers on everyone who uses this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TJSONParser;
class TJSONNode;


// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONAnchorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJSONAnchorVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJSONAnchorVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngJSONAnchorVal(const TMEngJSONAnchorVal&) = delete;

        ~TMEngJSONAnchorVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngJSONAnchorVal& operator=(const TMEngJSONAnchorVal&) = delete;


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
        )   const final;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   final;


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
                    TMEngJSONAnchorVal&     mecvToUpdate
            , const tCIDLib::TBoolean       bNext
        );

        const TJSONValue& jprsnValue() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAnchorInfo
        (
            const   tCIDLib::TCard4         c4ParserId
            , const tCIDLib::TCard4         c4ParseId
            , const TJSONValue* const       pjprsnToSet
            , const tCIDLib::TCard4         c4Index
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index
        //      The child index (within the parent node) of the node that we
        //      are currently set for.
        //
        //  m_c4ParseId
        //  m_c4ParserId
        //      To insure that an anchor is only used with the parser and
        //      parsing pass that created it, we get two ids when we are
        //      created. The parser (through which all access through anchors
        //      is done) will insure that the anchor is still valid.
        //
        //  m_pjprsnValue
        //      The node that we are currently pointed at, null if we've not
        //      been set. We store them as value nodes, since everything that
        //      we care about is directly or indirectly derived from that.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Index;
        tCIDLib::TCard4     m_c4ParseId;
        tCIDLib::TCard4     m_c4ParserId;
        const TJSONValue*   m_pjprsnValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngJSONAnchorVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONAnchorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJSONAnchorInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJSONAnchorInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngJSONAnchorInfo(const TMEngJSONAnchorInfo&) = delete;

        ~TMEngJSONAnchorInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngJSONAnchorInfo& operator=(const TMEngJSONAnchorInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   final;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const final;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_DefCtor
        //      We just have a default ctor and that's it. All access to the
        //      info represented by this anchor is done via the parser class.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngJSONAnchorInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONParserVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJSONParserVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJSONParserVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngJSONParserVal(const TMEngJSONParserVal&);

        ~TMEngJSONParserVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngJSONParserVal& operator=(const TMEngJSONParserVal&) = delete;


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
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckAnchor
        (
            const   TMEngJSONAnchorVal&     mecvAnchor
        )   const;

        tCIDLib::TBoolean bGetChildAnchor
        (
            const   TMEngJSONAnchorVal&     mecvParent
            , const tCIDLib::TCard4         c4Index
            ,       TMEngJSONAnchorVal&     mecvToSet
        )   const;

        tCIDLib::TVoid GetRootAnchor
        (
                    TMEngJSONAnchorVal&     mecvToSet
        )   const;

        tCIDLib::TVoid InitIds
        (
                    TCIDMacroEngine&        meOwner
        );

        const TJSONParser& jprsValue() const;

        TJSONParser& jprsValue();

        tCIDLib::TVoid Parse
        (
                    TTextInStream&          strmSrc
        );

        tCIDLib::TVoid Print
        (
                    TTextOutStream&         strmTar
            , const TMEngJSONAnchorVal&     mecvStart
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAnchor
        (
            const   TString&                strPath
            ,       TMEngJSONAnchorVal&     mecvToSet
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
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
        //  m_pjprsValue
        //      The parser object that provides our value. We use a pointer
        //      so that we don't have to expose the underlying Net headers
        //      to our users.
        //
        //  m_pjprsnRoot
        //      The root node. We set an empty one initially just to have one.
        //      One each parse, we store a new one returned from the parser.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ParseId;
        tCIDLib::TCard4     m_c4ParserId;
        TJSONParser*        m_pjprsValue;
        TJSONValue*         m_pjprsnRoot;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngJSONParserVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONParserInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJSONParserInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJSONParserInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngJSONParserInfo(const TMEngJSONParserInfo&) = delete;

        ~TMEngJSONParserInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngJSONParserInfo& operator=(const TMEngJSONParserInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   final;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const final;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckAnchor
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngJSONParserVal&     mecvParser
            , const TMEngJSONAnchorVal&     mecvAnchor
        );

        const TJSONCont* pjprsnCheckIsCont
        (
                    TCIDMacroEngine&        meOwner
            , const TJSONValue&             jprsnSrc
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TJSONObject* pjprsnCheckIsObj
        (
                    TCIDMacroEngine&        meOwner
            , const TJSONValue&             jprsnSrc
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TJSONArray* pjprsnCheckIsArray
        (
                    TCIDMacroEngine&        meOwner
            , const TJSONValue&             jprsnSrc
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TJSONValue* pjprsnFind
        (
                    TCIDMacroEngine&        meOwner
            , const TJSONCont&              jprsnParent
            , const TString&                strToFind
            ,       tCIDLib::TCard4&        c4InOut
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TString& strCheckIsSimpleVal
        (
                    TCIDMacroEngine&        meOwner
            , const TJSONValue&             jprsnVal
            , const tCIDLib::TBoolean       bThrowIfNot
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
            , const MFormattable&           mfmtblToken1
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrId_XXX
        //      The ids for our errors that we throw, for efficiency we save
        //      them away for later throwing.
        //
        //  m_pmeciXXX
        //      The class info for any types we want to keep up with (our
        //      nested or external.)
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2EnumId_ValTypes;
        tCIDLib::TCard2 m_c2MethId_CheckValue;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_FindArray;
        tCIDLib::TCard2 m_c2MethId_FindBoolValue;
        tCIDLib::TCard2 m_c2MethId_FindCardValue;
        tCIDLib::TCard2 m_c2MethId_FindFloatValue;
        tCIDLib::TCard2 m_c2MethId_FindIntValue;
        tCIDLib::TCard2 m_c2MethId_FindChild;
        tCIDLib::TCard2 m_c2MethId_FindObject;
        tCIDLib::TCard2 m_c2MethId_FindValue;
        tCIDLib::TCard2 m_c2MethId_FindType;
        tCIDLib::TCard2 m_c2MethId_GetChildAt;
        tCIDLib::TCard2 m_c2MethId_GetChildCount;
        tCIDLib::TCard2 m_c2MethId_GetFirstChild;
        tCIDLib::TCard2 m_c2MethId_GetLastChild;
        tCIDLib::TCard2 m_c2MethId_GetName;
        tCIDLib::TCard2 m_c2MethId_GetNameAt;
        tCIDLib::TCard2 m_c2MethId_GetRootAnchor;
        tCIDLib::TCard2 m_c2MethId_GetType;
        tCIDLib::TCard2 m_c2MethId_GetTypeAt;
        tCIDLib::TCard2 m_c2MethId_GetValue;
        tCIDLib::TCard2 m_c2MethId_GetValueAt;
        tCIDLib::TCard2 m_c2MethId_NextPrevChild;
        tCIDLib::TCard2 m_c2MethId_ParseResponse;
        tCIDLib::TCard2 m_c2MethId_ParseResponse2;
        tCIDLib::TCard2 m_c2MethId_PrintToStream;
        tCIDLib::TCard2 m_c2TypeId_Anchor;
        tCIDLib::TCard2 m_c2TypeId_TextInStream;
        tCIDLib::TCard2 m_c2TypeId_TextOutStream;
        tCIDLib::TCard4 m_c4ErrBadAnchor;
        tCIDLib::TCard4 m_c4ErrBadIndex;
        tCIDLib::TCard4 m_c4ErrChildNotFound;
        tCIDLib::TCard4 m_c4ErrGetValAs;
        tCIDLib::TCard4 m_c4ErrNameNotFound;
        tCIDLib::TCard4 m_c4ErrNotACont;
        tCIDLib::TCard4 m_c4ErrNotAnArray;
        tCIDLib::TCard4 m_c4ErrNotAnObject;
        tCIDLib::TCard4 m_c4ErrNotASimpleVal;
        tCIDLib::TCard4 m_c4ErrParseFailed;
        tCIDLib::TCard4 m_c4ErrPrintErr;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciValTypes;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngJSONParserInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK

