//
// FILE NAME: CIDMacroEng_StringClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_StringClass.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data object for that class) and another that provides the value storage
//  for instances of that class.
//
//  We provide these derivatives here for the MEng.String class, which is the
//  fundamental string class that is widely used in almost any macro. THis
//  class derives from the MEng.Formattable class, which allows it to be
//  polymorphically formatted to a stream. We give this guy his own file since
//  it has a good number of methods.
//
//  Note that we actually use a TPathStr for our storage, so that we can use
//  the MEng.String class for what both TString and TPathStr would be used for
//  in the C++ world.
//
//  We also define the single character class here, since it is very closely
//  related. And we define a 'string list' class, which is just a vector of
//  strings, because it's so commonly used, particularly in CQC.
//
//  And we also define a trancoder class. It just supports transcoding text
//  to/from a particular encoding.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngTextOutStreamVal;
class TTextConverter;


// ---------------------------------------------------------------------------
//  CLASS: TMEngCharVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCharVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCharVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCh            chInit = kCIDLib::chSpace
        );

        TMEngCharVal(const TMEngCharVal&) = delete;

        ~TMEngCharVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCharVal& operator=(const TMEngCharVal&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCh chValue() const;

        tCIDLib::TCh chValue
        (
            const   tCIDLib::TCh            chToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_chValue
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCh    m_chValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCharVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCharInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCharInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCharInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCharInfo(const TMEngCharInfo&) = delete;

        ~TMEngCharInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCharInfo& operator=(const TMEngCharInfo&) = delete;


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
        tCIDLib::TCard2 m_c2EnumId_Types;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetOrdinal;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_IsOfType;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_OfsOrdinal;
        tCIDLib::TCard2 m_c2MethId_SetOrdinal;
        tCIDLib::TCard2 m_c2MethId_ToLower;
        tCIDLib::TCard2 m_c2MethId_ToUpper;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard4 m_c4Type_ASCIIAlpha;
        tCIDLib::TCard4 m_c4Type_ASCIIAlphaNum;
        tCIDLib::TCard4 m_c4Type_Alpha;
        tCIDLib::TCard4 m_c4Type_AlphaNum;
        tCIDLib::TCard4 m_c4Type_Digit;
        tCIDLib::TCard4 m_c4Type_HexDigit;
        tCIDLib::TCard4 m_c4Type_Space;
        TMEngEnumInfo*  m_pmeciTypes;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCharInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStringVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngStringVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const TString&                strInit
        );

        TMEngStringVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            ,       TPathStr* const         ppathToUse
        );

        TMEngStringVal(const TMEngStringVal&) = delete;

        ~TMEngStringVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringVal& operator=(const TMEngStringVal&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Length() const;

        const TPathStr& pathValue() const;

        TPathStr& pathValue();

        const TPathStr& pathValue
        (
            const   TString&                strToSet
        );

        const TString& strValue() const;

        TString& strValue();

        const TString& strValue
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_eAdopt
        //      Indicates whether we adopted the passed string. We want to
        //      be able to use passed in strings to push on the stack and
        //      get results back to the C++ world, that's why we use a pointer
        //      and allow adoption or just usage of passed strings.
        //
        //  m_pstrValue
        //      This is the storage for values of this type. Note that we use
        //      this value class for both string and path string classes.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        TPathStr*           m_ppathValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStringInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStringInfo(const TMEngStringInfo&) = delete;

        ~TMEngStringInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringInfo& operator=(const TMEngStringInfo&) = delete;


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
        tCIDLib::TVoid CheckRange
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4Length
        );

        tCIDLib::TVoid CheckSrcTarSame
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngStringVal&         mecvThis
            , const TMEngStringVal&         mecvOther
        );

        tCIDLib::EFindRes eAppendFmt
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TBoolean       bAppend
            , const TMEngClassInfo&         meciTarget
            , const tCIDLib::TCh            chTok
            ,       TString&                strTar
            ,       TMEngClassVal&          mecvFmt
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
            ,       TError&                 errThrown
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken
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
        //      We have to create some nested classes, so we cache the class
        //      ids for performance.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrXXX
        //      We have a nested enum for the exceptions we throw, so we store
        //      the ordinals as we register the values, which are used later
        //      to throw them.
        //
        //  m_pmeciErrors
        //      We hang onto a copy of the error enum class that we create
        //      since we need it when we throw a macro level exception. But
        //      we DONT own it.
        //
        //  m_pmeciRadices
        //      We pre-look this type up during init so that we don't have to
        //      go back and get it each time we need it.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_AddLevel;
        tCIDLib::TCard2 m_c2MethId_Append;
        tCIDLib::TCard2 m_c2MethId_AppendChar;
        tCIDLib::TCard2 m_c2MethId_AppendCard1;
        tCIDLib::TCard2 m_c2MethId_AppendCard2;
        tCIDLib::TCard2 m_c2MethId_AppendCard4;
        tCIDLib::TCard2 m_c2MethId_AppendCard8;
        tCIDLib::TCard2 m_c2MethId_AppendEnum;
        tCIDLib::TCard2 m_c2MethId_AppendFloat4;
        tCIDLib::TCard2 m_c2MethId_AppendFloat8;
        tCIDLib::TCard2 m_c2MethId_AppendFmt;
        tCIDLib::TCard2 m_c2MethId_AppendInt1;
        tCIDLib::TCard2 m_c2MethId_AppendInt2;
        tCIDLib::TCard2 m_c2MethId_AppendInt4;
        tCIDLib::TCard2 m_c2MethId_CapAt;
        tCIDLib::TCard2 m_c2MethId_Clear;
        tCIDLib::TCard2 m_c2MethId_CompNC;
        tCIDLib::TCard2 m_c2MethId_Cut;
        tCIDLib::TCard2 m_c2MethId_CvtDecDigitAt;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_DelLast;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_ExtractCard4;
        tCIDLib::TCard2 m_c2MethId_ExtractCard4R;
        tCIDLib::TCard2 m_c2MethId_ExtractCard8;
        tCIDLib::TCard2 m_c2MethId_ExtractCard8R;
        tCIDLib::TCard2 m_c2MethId_ExtractExt;
        tCIDLib::TCard2 m_c2MethId_ExtractFloat8;
        tCIDLib::TCard2 m_c2MethId_ExtractInt4;
        tCIDLib::TCard2 m_c2MethId_ExtractInt4R;
        tCIDLib::TCard2 m_c2MethId_ExtractNameExt;
        tCIDLib::TCard2 m_c2MethId_ExtractPath;
        tCIDLib::TCard2 m_c2MethId_ExtractSubStr;
        tCIDLib::TCard2 m_c2MethId_ExtractUpTo;
        tCIDLib::TCard2 m_c2MethId_FindFirstChar;
        tCIDLib::TCard2 m_c2MethId_FindLastChar;
        tCIDLib::TCard2 m_c2MethId_FindNextChar;
        tCIDLib::TCard2 m_c2MethId_FindNextSubStr;
        tCIDLib::TCard2 m_c2MethId_FindSubStr;
        tCIDLib::TCard2 m_c2MethId_FindTokenList;
        tCIDLib::TCard2 m_c2MethId_FmtToField;
        tCIDLib::TCard2 m_c2MethId_GetAlloc;
        tCIDLib::TCard2 m_c2MethId_GetAt;
        tCIDLib::TCard2 m_c2MethId_GetExt;
        tCIDLib::TCard2 m_c2MethId_GetLast;
        tCIDLib::TCard2 m_c2MethId_GetLength;
        tCIDLib::TCard2 m_c2MethId_GetNameExt;
        tCIDLib::TCard2 m_c2MethId_GetPath;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_InsertCharAt;
        tCIDLib::TCard2 m_c2MethId_InsertStrAt;
        tCIDLib::TCard2 m_c2MethId_IsEmpty;
        tCIDLib::TCard2 m_c2MethId_HasExt;
        tCIDLib::TCard2 m_c2MethId_HasName;
        tCIDLib::TCard2 m_c2MethId_HasPath;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_ParseVersion;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Prepend;
        tCIDLib::TCard2 m_c2MethId_PrependChar;
        tCIDLib::TCard2 m_c2MethId_PutAt;
        tCIDLib::TCard2 m_c2MethId_Reallocate;
        tCIDLib::TCard2 m_c2MethId_RemoveLevel;
        tCIDLib::TCard2 m_c2MethId_RemoveTrailSep;
        tCIDLib::TCard2 m_c2MethId_ReplaceChar;
        tCIDLib::TCard2 m_c2MethId_ReplaceSubStr;
        tCIDLib::TCard2 m_c2MethId_ReplaceToken;
        tCIDLib::TCard2 m_c2MethId_SizeCtor;
        tCIDLib::TCard2 m_c2MethId_Split;
        tCIDLib::TCard2 m_c2MethId_StartsWith;
        tCIDLib::TCard2 m_c2MethId_StripChars;
        tCIDLib::TCard2 m_c2MethId_StripChars2;
        tCIDLib::TCard2 m_c2MethId_StripWhitespace;
        tCIDLib::TCard2 m_c2MethId_SumChars;
        tCIDLib::TCard2 m_c2MethId_ToCard4;
        tCIDLib::TCard2 m_c2MethId_ToCard4R;
        tCIDLib::TCard2 m_c2MethId_ToCard4Ex;
        tCIDLib::TCard2 m_c2MethId_ToCard8;
        tCIDLib::TCard2 m_c2MethId_ToCard8R;
        tCIDLib::TCard2 m_c2MethId_ToCard8Ex;
        tCIDLib::TCard2 m_c2MethId_ToFloat4;
        tCIDLib::TCard2 m_c2MethId_ToFloat4Ex;
        tCIDLib::TCard2 m_c2MethId_ToFloat8;
        tCIDLib::TCard2 m_c2MethId_ToFloat8Ex;
        tCIDLib::TCard2 m_c2MethId_ToInt4;
        tCIDLib::TCard2 m_c2MethId_ToInt4R;
        tCIDLib::TCard2 m_c2MethId_ToInt4Ex;
        tCIDLib::TCard2 m_c2MethId_ToLower;
        tCIDLib::TCard2 m_c2MethId_ToUpper;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard4 m_c4ErrBadInd;
        tCIDLib::TCard4 m_c4ErrBadRange;
        tCIDLib::TCard4 m_c4ErrCantConvert;
        tCIDLib::TCard4 m_c4ErrFmtFailed;
        tCIDLib::TCard4 m_c4ErrNotDecDigit;
        tCIDLib::TCard4 m_c4ErrTooLarge;
        tCIDLib::TCard4 m_c4ErrSrcTarSame;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciFindRes;
        TMEngEnumInfo*  m_pmeciHJustify;
        TMEngEnumInfo*  m_pmeciRadices;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrListVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStrListVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        enum class EFromTypes
        {
            Append
            , Copy
            , Steal
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStrListVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngStrListVal(const TMEngStrListVal&) = delete;

        ~TMEngStrListVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStrListVal& operator=(const TMEngStrListVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAppend
        (
            const   TString&                strToAdd
        );

        tCIDLib::TBoolean bFromSource
        (
                    TMEngStrListVal&        mecvSrc
            , const EFromTypes              eType
        );

        tCIDLib::TBoolean bInsertAt
        (
            const   tCIDLib::TCard4         c4At
            , const TString&                strToInsert
        );

        tCIDLib::TBoolean bSetAt
        (
            const   tCIDLib::TCard4         c4At
            , const TString&                strNewValue
        );

        tCIDLib::TBoolean bUnique() const;

        tCIDLib::TBoolean bUnique
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4ElemCount() const;

        tCIDLib::TCard4 c4FindString
        (
            const   TString&                strToFind
        );

        tCIDLib::TStrList& colValue();

        const tCIDLib::TStrList& colValue() const;

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveAt
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TBoolean       bUnique
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TStrList&      colToSet
            , const tCIDLib::TBoolean       bUnique
        );

        const TString& strAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid Set
        (
            const   TMEngStrListVal&        mecvToSet
        );


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


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckUnique
        (
            const   TString&                strToCheck
        );


        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_bUnique
        //      Indicates whether the list must be unique. This is the default
        //      since it is most often used to hold a set of unique values.
        //
        //  m_colValue
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bUnique;
        tCIDLib::TStrList   m_colValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStrListVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrListInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStrListInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStrListInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStrListInfo(const TMEngStrListInfo&) = delete;

        ~TMEngStrListInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStrListInfo& operator=(const TMEngStrListInfo&) = delete;


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
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
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
        //  m_c2EnumId_Errors
        //      We have to create some nested classes, so we cache the class
        //      ids for performance.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrXXX
        //      We have a nested enum for the exceptions we throw, so we store
        //      the ordinals as we register the values, which are used later
        //      to throw them.
        //
        //  m_pmeciErrors
        //      We hang onto a copy of the error enum class that we create
        //      since we need it when we throw a macro level exception. But
        //      we DONT own it.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_Append;
        tCIDLib::TCard2 m_c2MethId_AppendFrom;
        tCIDLib::TCard2 m_c2MethId_CopyFrom;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_FindString;
        tCIDLib::TCard2 m_c2MethId_GetAt;
        tCIDLib::TCard2 m_c2MethId_GetElemCount;
        tCIDLib::TCard2 m_c2MethId_GetUnique;
        tCIDLib::TCard2 m_c2MethId_InsertAt;
        tCIDLib::TCard2 m_c2MethId_IsEmpty;
        tCIDLib::TCard2 m_c2MethId_RemoveAll;
        tCIDLib::TCard2 m_c2MethId_RemoveAt;
        tCIDLib::TCard2 m_c2MethId_SetAt;
        tCIDLib::TCard2 m_c2MethId_StealFrom;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard4 m_c4ErrBadInd;
        tCIDLib::TCard4 m_c4ErrAlreadyUsed;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStrListInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextXCoderVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTextXCoderVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextXCoderVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngTextXCoderVal(const TMEngTextXCoderVal&) = delete;

        ~TMEngTextXCoderVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextXCoderVal& operator=(const TMEngTextXCoderVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetEncoding
        (
            const   TString&                strToSet
        );

        const TTextConverter& tcvtValue() const;

        TTextConverter& tcvtValue();


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


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_ptcvtValue
        //      This is the text converter for this value object. It is not
        //      set until the macro level constructor is called.
        // -------------------------------------------------------------------
        TTextConverter* m_ptcvtValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextXCoderVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextXCoderInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTextXCoderInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextXCoderInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngTextXCoderInfo(const TMEngTextXCoderInfo&) = delete;

        ~TMEngTextXCoderInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextXCoderInfo& operator=(const TMEngTextXCoderInfo&) = delete;


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
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken
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
            , const MFormattable&           fmtblToken2
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c2TypeId_XXX
        //      We have some nested types. We store the ids for later use,
        //      to avoid having to look them up again.
        //
        //  m_c4ErrXXX
        //      We store the ordinals for the values of the error enum that
        //      we create, so that we can later use them to throw exceptions
        //      without having to look them back up.
        //
        //  m_pmeciXXX
        //      Pointers to the class info objects of any nested classes that
        //      we create. We don't own them, we just keep them around for
        //      later use so we don't have to go look it up every time.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_ConvertFrom;
        tCIDLib::TCard2 m_c2MethId_ConvertFromRange;
        tCIDLib::TCard2 m_c2MethId_ConvertTo;
        tCIDLib::TCard2 m_c2MethId_ConvertNTo;
        tCIDLib::TCard2 m_c2MethId_GetEncoding;
        tCIDLib::TCard2 m_c2MethId_SetEncoding;
        tCIDLib::TCard2 m_c2MethId_SetErrAction;
        tCIDLib::TCard2 m_c2MethId_SetRepChar;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2TypeId_CvtErrActs;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard4 m_c4ErrBadCharCount;
        tCIDLib::TCard4 m_c4ErrBadEncoding;
        tCIDLib::TCard4 m_c4ErrBadSrc;
        tCIDLib::TCard4 m_c4ErrBadSrcRange;
        tCIDLib::TCard4 m_c4ErrBufSz;
        tCIDLib::TCard4 m_c4ErrConvertFrom;
        tCIDLib::TCard4 m_c4ErrConvertTo;
        tCIDLib::TCard4 m_c4ErrUnRep;
        TMEngEnumInfo*  m_pmeciCvtErrActs;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextXCoderInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


