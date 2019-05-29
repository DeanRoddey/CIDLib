//
// FILE NAME: CIDMacroEng_StringClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the fundamental MEng.String, MEng.Char,
//  nad MEng.StringList classes.
//
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngCharVal,TMEngClassVal)
RTTIDecls(TMEngCharInfo,TMEngClassInfo)
RTTIDecls(TMEngStringVal,TMEngClassVal)
RTTIDecls(TMEngStringInfo,TMEngClassInfo)
RTTIDecls(TMEngStrListVal,TMEngClassVal)
RTTIDecls(TMEngStrListInfo,TMEngClassInfo)
RTTIDecls(TMEngTextXCoderVal,TMEngClassVal)
RTTIDecls(TMEngTextXCoderInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_StringClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strChar(L"Char");
    const TString   strCharClassPath(L"MEng.Char");
    const TString   strString(L"String");
    const TString   strStringClassPath(L"MEng.String");
    const TString   strStrList(L"StringList");
    const TString   strStrListClassPath(L"MEng.StringList");
    const TString   strXCoder(L"TextXCoder");
    const TString   strXCoderClassPath(L"MEng.System.Runtime.TextXCoder");


    // -----------------------------------------------------------------------
    //  The max chars we'll let a string grow to
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MaxChars = 0x10000000;
}



// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------

//
//  A bogus key extractor so that we can use a keyed hash set (we don't have a
//  by ref regular has set yet) on a set of strings, so the object itself is
//  the key.
//
static const TString& strBogusKeyExtract(const TString& strElem)
{
    return strElem;
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngCharVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCharVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCharVal::TMEngCharVal( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCh            chInit) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Char), eConst)
    , m_chValue(chInit)
{
}

TMEngCharVal::~TMEngCharVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngCharVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCharVal::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices
                        , const TCIDMacroEngine&) const
{
    if ((eFormat == tCIDMacroEng::EDbgFmts::Short)
    &&  TRawStr::bIsAlphaNum(m_chValue) ||  TRawStr::bIsPunct(m_chValue))
    {
        // It's something printable and they want the short version so do that
        strmTarget << m_chValue;
    }
     else
    {
        // Else format the hex value
        strmTarget << L"0x" << TCardinal(m_chValue, tCIDLib::ERadices::Hex);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngCharVal::bParseFromText(const  TString&        strValue
                            , const TMEngClassInfo& mecvThis)
{
    if (strValue.c4Length() != 1)
        return kCIDLib::False;
    m_chValue = strValue.chFirst();
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngCharVal::CopyFrom( const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_chValue = static_cast<const TMEngCharVal&>(mecvToCopy).m_chValue;
}


// ---------------------------------------------------------------------------
//  TMEngCharVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCh TMEngCharVal::chValue() const
{
    return m_chValue;
}

tCIDLib::TCh TMEngCharVal::chValue(const tCIDLib::TCh chToSet)
{
    m_chValue = chToSet;
    return m_chValue;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCharInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCharInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCharInfo::strPath()
{
    return CIDMacroEng_StringClasses::strCharClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCharInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCharInfo::TMEngCharInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_StringClasses::strChar
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2EnumId_Types(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetOrdinal(kMacroEng::c2BadId)
    , m_c2MethId_GtThan(kMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kMacroEng::c2BadId)
    , m_c2MethId_LsThan(kMacroEng::c2BadId)
    , m_c2MethId_IsOfType(kMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kMacroEng::c2BadId)
    , m_c2MethId_OfsOrdinal(kMacroEng::c2BadId)
    , m_c2MethId_SetOrdinal(kMacroEng::c2BadId)
    , m_c2MethId_ToLower(kMacroEng::c2BadId)
    , m_c2MethId_ToUpper(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c4Type_ASCIIAlpha(kCIDLib::c4MaxCard)
    , m_c4Type_ASCIIAlphaNum(kCIDLib::c4MaxCard)
    , m_c4Type_Alpha(kCIDLib::c4MaxCard)
    , m_c4Type_AlphaNum(kCIDLib::c4MaxCard)
    , m_c4Type_Digit(kCIDLib::c4MaxCard)
    , m_c4Type_HexDigit(kCIDLib::c4MaxCard)
    , m_c4Type_Space(kCIDLib::c4MaxCard)
    , m_pmeciTypes(0)
{
}

TMEngCharInfo::~TMEngCharInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngCharInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCharInfo::Init(TCIDMacroEngine& meOwner)
{
    // Add some literals for common magic values
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kBOM"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kBOM", tCIDMacroEng::EConstTypes::Const, kCIDLib::chUniBOM)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kCR"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kCR", tCIDMacroEng::EConstTypes::Const, kCIDLib::chCR)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kEsc"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kEsc", tCIDMacroEng::EConstTypes::Const, kCIDLib::chEsc)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFF"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kFF", tCIDMacroEng::EConstTypes::Const, kCIDLib::chFF)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kLF"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kLF", tCIDMacroEng::EConstTypes::Const, kCIDLib::chLF)
        )
    );


    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kNull"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kNull", tCIDMacroEng::EConstTypes::Const, kCIDLib::chNull)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTab"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kHTab", tCIDMacroEng::EConstTypes::Const, kCIDLib::chTab)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kSwappedBOM"
            , tCIDMacroEng::EIntrinsics::Char
            , new TMEngCharVal(L"kSwappedBOM", tCIDMacroEng::EConstTypes::Const, kCIDLib::chSwappedUniBOM)
        )
    );

    // We need to generate an enum class for our char types
    {
        m_pmeciTypes = new TMEngEnumInfo
        (
            meOwner
            , L"CharTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 10
        );
        m_c4Type_ASCIIAlpha = m_pmeciTypes->c4AddEnumItem(L"ASCIIAlpha", L"ASCII alphabetical");
        m_c4Type_ASCIIAlphaNum = m_pmeciTypes->c4AddEnumItem(L"ASCIIAlphaNum", L"ASCII alphanumeric");
        m_c4Type_Alpha = m_pmeciTypes->c4AddEnumItem(L"Alpha", L"Unicode alphabetical");
        m_c4Type_AlphaNum = m_pmeciTypes->c4AddEnumItem(L"AlphaNum", L"Unicode alphanumeric");
        m_c4Type_Digit = m_pmeciTypes->c4AddEnumItem(L"Digit", L"Unicode decimal digit");
        m_c4Type_HexDigit = m_pmeciTypes->c4AddEnumItem(L"HexDigit", L"Unicode hex digit");
        m_c4Type_Space = m_pmeciTypes->c4AddEnumItem(L"Whitespace", L"Whitespace content");
        m_pmeciTypes->BaseClassInit(meOwner);
        m_c2EnumId_Types = meOwner.c2AddClass(m_pmeciTypes);
        bAddNestedType(m_pmeciTypes->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Char"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Check equality
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get the ordinal
    {
        TMEngMethodInfo methiNew
        (
            L"GetOrdinal"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetOrdinal = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_GtThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GtThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the char type
    {
        TMEngMethodInfo methiNew
        (
            L"IsOfType"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"TypeToCheck", m_c2EnumId_Types);
        m_c2MethId_IsOfType = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_LsThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"LsThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Offset the ordinal
    {
        TMEngMethodInfo methiNew
        (
            L"OfsOrdinal"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"OfsBy", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_OfsOrdinal = c2AddMethodInfo(methiNew);
    }

    // Set the ordinal
    {
        TMEngMethodInfo methiNew
        (
            L"SetOrdinal"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetOrdinal = c2AddMethodInfo(methiNew);
    }

    // Convert to lower or upper cass
    {
        TMEngMethodInfo methiNew
        (
            L"ToLower"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ToLower = c2AddMethodInfo(methiNew);
    }

    // Convert to lower or upper cass
    {
        TMEngMethodInfo methiNew
        (
            L"ToUpper"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ToUpper = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Char"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Char);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCharInfo::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCharVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCharInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCharInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngCharVal& mecvActual = static_cast<TMEngCharVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just sets it to space
        mecvActual.chValue(kCIDLib::chSpace);
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.chValue();
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThan)
          ||  (c2MethId == m_c2MethId_LsThanEq))
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        if (c2MethId == m_c2MethId_Equal)
            mecvRet.bValue(mecvActual.chValue() == meOwner.chStackValAt(c4FirstInd));
        else if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.chValue() > meOwner.chStackValAt(c4FirstInd));
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.chValue() >= meOwner.chStackValAt(c4FirstInd));
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.chValue() < meOwner.chStackValAt(c4FirstInd));
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.chValue() <= meOwner.chStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_GetOrdinal)
    {
        // Convert the character to a Card4
        TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.chValue());
    }
     else if (c2MethId == m_c2MethId_IsOfType)
    {
        // Get the type enum value
        const TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        const tCIDLib::TCard4 c4Type = mecvType.c4Ordinal();

        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        const tCIDLib::TCh chThis = mecvActual.chValue();
        if (c4Type == m_c4Type_ASCIIAlpha)
        {
            mecvRet.bValue
            (
                ((chThis >= kCIDLib::chLatin_A) && (chThis <= kCIDLib::chLatin_Z))
                || ((chThis >= kCIDLib::chLatin_a) && (chThis <= kCIDLib::chLatin_z))
            );
        }
         else if (c4Type == m_c4Type_ASCIIAlphaNum)
        {
            mecvRet.bValue
            (
                ((chThis >= kCIDLib::chLatin_A) && (chThis <= kCIDLib::chLatin_Z))
                || ((chThis >= kCIDLib::chLatin_a) && (chThis <= kCIDLib::chLatin_z))
                || ((chThis >= kCIDLib::chDigit0) && (chThis <= kCIDLib::chDigit9))
            );
        }
         else if (c4Type == m_c4Type_Alpha)
        {
            mecvRet.bValue(TRawStr::bIsAlpha(chThis));
        }
         else if (c4Type == m_c4Type_AlphaNum)
        {
            mecvRet.bValue(TRawStr::bIsAlphaNum(chThis));
        }
         else if (c4Type == m_c4Type_Digit)
        {
            mecvRet.bValue(TRawStr::bIsDigit(chThis));
        }
         else if (c4Type == m_c4Type_HexDigit)
        {
            mecvRet.bValue(TRawStr::bIsHexDigit(chThis));
        }
         else if (c4Type == m_c4Type_Space)
        {
            mecvRet.bValue(TRawStr::bIsSpace(chThis));
        }
         else
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcRT_UnknownCharType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(c4Type)
            );
        }
    }
     else if (c2MethId == m_c2MethId_OfsOrdinal)
    {
        const tCIDLib::TInt4 i4New = meOwner.i4StackValAt(c4FirstInd);
        mecvActual.chValue
        (
            tCIDLib::TCh(tCIDLib::TInt4(mecvActual.chValue()) + i4New)
        );
    }
     else if (c2MethId == m_c2MethId_SetOrdinal)
    {
        const tCIDLib::TCard4 c4New = meOwner.c4StackValAt(c4FirstInd);
        mecvActual.chValue(tCIDLib::TCh(c4New));
    }
     else if (c2MethId == m_c2MethId_ToLower)
    {
        mecvActual.chValue(TRawStr::chLower(mecvActual.chValue()));
    }
     else if (c2MethId == m_c2MethId_ToUpper)
    {
        mecvActual.chValue(TRawStr::chUpper(mecvActual.chValue()));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.chValue(meOwner.chStackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringVal::TMEngStringVal( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String), eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::Adopt)
    , m_ppathValue(new TPathStr(8UL))
{
}

TMEngStringVal::TMEngStringVal( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst
                                , const TString&                strInit) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String), eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::Adopt)
    , m_ppathValue(new TPathStr(strInit))
{
}

TMEngStringVal::TMEngStringVal( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst
                                ,       TPathStr* const         ppathToUse) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String), eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
    , m_ppathValue(ppathToUse)
{
}

TMEngStringVal::~TMEngStringVal()
{
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
        delete m_ppathValue;
}


// ---------------------------------------------------------------------------
//  TMEngStringVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringVal::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    tCIDLib::TCard4 c4OrgLen = m_ppathValue->c4Length();
    tCIDLib::TCard4 c4ShowLen = c4OrgLen;
    tCIDLib::TCard4 c4Line = 32;
    if (eRadix == tCIDLib::ERadices::Hex)
        c4Line = 8;

    // If long format, show the overview info
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Length = " << c4ShowLen << kCIDLib::NewLn
                   << L"Current Buffer Size = " << m_ppathValue->c4BufChars()
                   << kCIDLib::DNewLn;
    }

    //
    //  If the short format, only dump the first 32 or 16 chars, according
    //  to the radix.
    //
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        if (eRadix == tCIDLib::ERadices::Hex)
        {
            if (c4ShowLen > 16)
                c4ShowLen = 16;
        }
         else
        {
            if (c4ShowLen > 32)
                c4ShowLen = 32;
            c4Line = 32;
        }
    }

    // Dump it out in up to 32 character lines
    if (c4ShowLen)
    {
        tCIDLib::TCard4 c4Count;
        tCIDLib::TCard4 c4Lines = 0;
        const tCIDLib::TCh* pchSrc = m_ppathValue->pszBuffer();
        const tCIDLib::TCh* const pchEnd = pchSrc + c4ShowLen;
        if (eRadix == tCIDLib::ERadices::Hex)
        {
            TStreamJanitor janTarget(&strmTarget);
            strmTarget.chFill(kCIDLib::chDigit0);
            strmTarget.c4Width(4);
            strmTarget.eJustification(tCIDLib::EHJustify::Right);
            tCIDLib::TCard4 c4Val;
            while ((pchSrc < pchEnd) && (c4Lines < 128))
            {
                c4Count = 0;
                while ((c4Count < c4Line) && (pchSrc < pchEnd))
                {
                    c4Val = *pchSrc++;
                    if (c4Val > 0xFFFF)
                    {
                        strmTarget
                            << TCardinal(c4Val >> 16, tCIDLib::ERadices::Hex)
                            << TTextOutStream::Spaces(1);
                        c4Val &= 0xFFFF;
                    }
                    strmTarget  << TCardinal(c4Val, tCIDLib::ERadices::Hex)
                                << TTextOutStream::Spaces(1);
                    c4Count++;
                }
                if (eFormat == tCIDMacroEng::EDbgFmts::Long)
                    strmTarget << kCIDLib::NewLn;
                c4Lines++;
            }
        }
         else
        {
            while ((pchSrc < pchEnd) && (c4Lines < 128))
            {
                c4Count = 0;
                while ((c4Count < 32) && (pchSrc < pchEnd))
                {
                    const tCIDLib::TCh chCur = *pchSrc++;
                    switch(chCur)
                    {
                        case 0x7 :
                        {
                            strmTarget << L"\\a";
                            break;
                        }

                        case 0x8 :
                        {
                            strmTarget << L"\\b";
                            break;
                        }

                        case 0x9 :
                        {
                            strmTarget << L"\\t";
                            break;
                        }

                        case 0x0A :
                        {
                            strmTarget << L"\\n";
                            break;
                        }

                        case 0x0B :
                        {
                            strmTarget << L"\\v";
                            break;
                        }

                        case 0x0C :
                        {
                            strmTarget << L"\\f";
                            break;
                        }

                        case 0x0D :
                        {
                            strmTarget << L"\\r";
                            break;
                        }

                        default :
                        {
                            strmTarget << chCur;
                            break;
                        }
                    };
                    c4Count++;
                }
                if (eFormat == tCIDMacroEng::EDbgFmts::Long)
                    strmTarget << kCIDLib::NewLn;
                c4Lines++;
            }
        }
    }

    // If doing short format, and we clipped it, then do an ellipsis
    if ((c4OrgLen > c4ShowLen) && (eFormat == tCIDMacroEng::EDbgFmts::Short))
        strmTarget << L"...";

    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngStringVal::bParseFromText( const   TString&        strValue
                                , const TMEngClassInfo& mecvThis)
{
    *m_ppathValue = strValue;
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngStringVal::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_ppathValue = *static_cast<const TMEngStringVal&>(mecvToCopy).m_ppathValue;
}


// ---------------------------------------------------------------------------
//  TMEngStringVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TMEngStringVal::c4Length() const
{
    return m_ppathValue->c4Length();
}


const TPathStr& TMEngStringVal::pathValue() const
{
    return *m_ppathValue;
}

TPathStr& TMEngStringVal::pathValue()
{
    return *m_ppathValue;
}

const TPathStr& TMEngStringVal::pathValue(const TString& strToSet)
{
    *m_ppathValue = strToSet;
    return *m_ppathValue;
}


const TString& TMEngStringVal::strValue() const
{
    return *m_ppathValue;
}

TString& TMEngStringVal::strValue()
{
    return *m_ppathValue;
}

const TString& TMEngStringVal::strValue(const TString& strToSet)
{
    *m_ppathValue = strToSet;
    return *m_ppathValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngStringInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStringInfo::strPath()
{
    return CIDMacroEng_StringClasses::strStringClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngStringInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringInfo::TMEngStringInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_StringClasses::strString
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::NonFinal
        , L"MEng.Formattable"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_AddLevel(kMacroEng::c2BadId)
    , m_c2MethId_Append(kMacroEng::c2BadId)
    , m_c2MethId_AppendCard1(kMacroEng::c2BadId)
    , m_c2MethId_AppendCard2(kMacroEng::c2BadId)
    , m_c2MethId_AppendCard4(kMacroEng::c2BadId)
    , m_c2MethId_AppendCard8(kMacroEng::c2BadId)
    , m_c2MethId_AppendChar(kMacroEng::c2BadId)
    , m_c2MethId_AppendEnum(kMacroEng::c2BadId)
    , m_c2MethId_AppendFloat4(kMacroEng::c2BadId)
    , m_c2MethId_AppendFloat8(kMacroEng::c2BadId)
    , m_c2MethId_AppendFmt(kMacroEng::c2BadId)
    , m_c2MethId_AppendInt1(kMacroEng::c2BadId)
    , m_c2MethId_AppendInt2(kMacroEng::c2BadId)
    , m_c2MethId_AppendInt4(kMacroEng::c2BadId)
    , m_c2MethId_CapAt(kMacroEng::c2BadId)
    , m_c2MethId_Clear(kMacroEng::c2BadId)
    , m_c2MethId_Cut(kMacroEng::c2BadId)
    , m_c2MethId_CvtDecDigitAt(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_DelLast(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_ExtractCard4(kMacroEng::c2BadId)
    , m_c2MethId_ExtractCard4R(kMacroEng::c2BadId)
    , m_c2MethId_ExtractExt(kMacroEng::c2BadId)
    , m_c2MethId_ExtractFloat8(kMacroEng::c2BadId)
    , m_c2MethId_ExtractInt4(kMacroEng::c2BadId)
    , m_c2MethId_ExtractInt4R(kMacroEng::c2BadId)
    , m_c2MethId_ExtractNameExt(kMacroEng::c2BadId)
    , m_c2MethId_ExtractPath(kMacroEng::c2BadId)
    , m_c2MethId_ExtractSubStr(kMacroEng::c2BadId)
    , m_c2MethId_FindFirstChar(kMacroEng::c2BadId)
    , m_c2MethId_FindLastChar(kMacroEng::c2BadId)
    , m_c2MethId_FindNextChar(kMacroEng::c2BadId)
    , m_c2MethId_FindNextSubStr(kMacroEng::c2BadId)
    , m_c2MethId_FindSubStr(kMacroEng::c2BadId)
    , m_c2MethId_FindTokenList(kMacroEng::c2BadId)
    , m_c2MethId_FmtToField(kMacroEng::c2BadId)
    , m_c2MethId_GetAlloc(kMacroEng::c2BadId)
    , m_c2MethId_GetAt(kMacroEng::c2BadId)
    , m_c2MethId_GetExt(kMacroEng::c2BadId)
    , m_c2MethId_GetLast(kMacroEng::c2BadId)
    , m_c2MethId_GetLength(kMacroEng::c2BadId)
    , m_c2MethId_GetNameExt(kMacroEng::c2BadId)
    , m_c2MethId_GetPath(kMacroEng::c2BadId)
    , m_c2MethId_GtThan(kMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kMacroEng::c2BadId)
    , m_c2MethId_HasExt(kMacroEng::c2BadId)
    , m_c2MethId_HasName(kMacroEng::c2BadId)
    , m_c2MethId_HasPath(kMacroEng::c2BadId)
    , m_c2MethId_InsertCharAt(kMacroEng::c2BadId)
    , m_c2MethId_InsertStrAt(kMacroEng::c2BadId)
    , m_c2MethId_IsEmpty(kMacroEng::c2BadId)
    , m_c2MethId_LsThan(kMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kMacroEng::c2BadId)
    , m_c2MethId_ParseVersion(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_Prepend(kMacroEng::c2BadId)
    , m_c2MethId_PrependChar(kMacroEng::c2BadId)
    , m_c2MethId_PutAt(kMacroEng::c2BadId)
    , m_c2MethId_Reallocate(kMacroEng::c2BadId)
    , m_c2MethId_RemoveLevel(kMacroEng::c2BadId)
    , m_c2MethId_RemoveTrailSep(kMacroEng::c2BadId)
    , m_c2MethId_ReplaceChar(kMacroEng::c2BadId)
    , m_c2MethId_ReplaceSubStr(kMacroEng::c2BadId)
    , m_c2MethId_ReplaceToken(kMacroEng::c2BadId)
    , m_c2MethId_SizeCtor(kMacroEng::c2BadId)
    , m_c2MethId_Split(kMacroEng::c2BadId)
    , m_c2MethId_StartsWith(kMacroEng::c2BadId)
    , m_c2MethId_StripChars(kMacroEng::c2BadId)
    , m_c2MethId_StripChars2(kMacroEng::c2BadId)
    , m_c2MethId_StripWhitespace(kMacroEng::c2BadId)
    , m_c2MethId_SumChars(kMacroEng::c2BadId)
    , m_c2MethId_ToCard4(kMacroEng::c2BadId)
    , m_c2MethId_ToCard4R(kMacroEng::c2BadId)
    , m_c2MethId_ToCard4Ex(kMacroEng::c2BadId)
    , m_c2MethId_ToCard8(kMacroEng::c2BadId)
    , m_c2MethId_ToCard8R(kMacroEng::c2BadId)
    , m_c2MethId_ToCard8Ex(kMacroEng::c2BadId)
    , m_c2MethId_ToFloat4(kMacroEng::c2BadId)
    , m_c2MethId_ToFloat4Ex(kMacroEng::c2BadId)
    , m_c2MethId_ToFloat8(kMacroEng::c2BadId)
    , m_c2MethId_ToFloat8Ex(kMacroEng::c2BadId)
    , m_c2MethId_ToInt4(kMacroEng::c2BadId)
    , m_c2MethId_ToInt4R(kMacroEng::c2BadId)
    , m_c2MethId_ToInt4Ex(kMacroEng::c2BadId)
    , m_c2MethId_ToLower(kMacroEng::c2BadId)
    , m_c2MethId_ToUpper(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c4ErrBadInd(kCIDLib::c4MaxCard)
    , m_c4ErrBadRange(kCIDLib::c4MaxCard)
    , m_c4ErrCantConvert(kCIDLib::c4MaxCard)
    , m_c4ErrFmtFailed(kCIDLib::c4MaxCard)
    , m_c4ErrNotDecDigit(kCIDLib::c4MaxCard)
    , m_c4ErrTooLarge(kCIDLib::c4MaxCard)
    , m_c4ErrSrcTarSame(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciFindRes(0)
    , m_pmeciHJustify(0)
    , m_pmeciRadices(0)
{
}

TMEngStringInfo::~TMEngStringInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStringInfo::Init(TCIDMacroEngine& meOwner)
{
    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"StringErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrBadInd = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"The index %(1) is invalid for this string");
        m_c4ErrBadRange = m_pmeciErrors->c4AddEnumItem(L"BadRange", L"The index and count %(1)/%(2) is invalid for this string");
        m_c4ErrCantConvert = m_pmeciErrors->c4AddEnumItem(L"CantConvert", L"Can't convert '%(1)' to a %(2) binary form");
        m_c4ErrNotDecDigit = m_pmeciErrors->c4AddEnumItem(L"NotDecDigit", L"%(1) is not a decimal digit");
        m_c4ErrTooLarge = m_pmeciErrors->c4AddEnumItem(L"TooLarge", L"%(1) is too large a string allocation size");
        m_c4ErrSrcTarSame = m_pmeciErrors->c4AddEnumItem(L"SrcTarSame", L"The source and target string cannot be the same object");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    //
    //  Look up any non-intrinsic classes we use. We'll need the id below
    //  for parameters, but we'll keep it around for later use when we
    //  are evaluating methods.
    //
    m_pmeciFindRes = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngBaseInfoInfo::strFindResPath(), kCIDLib::True
    );

    m_pmeciHJustify = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngBaseInfoInfo::strHorzJustifyPath(), kCIDLib::True
    );

    m_pmeciRadices = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngBaseInfoInfo::strRadixPath(), kCIDLib::True
    );

    // Some constants for the flags to StripChars2
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kLeading"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kLeading", tCIDMacroEng::EConstTypes::Const, 0x0001)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kTrailing"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kTrailing", tCIDMacroEng::EConstTypes::Const, 0x0002)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMiddle"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kMiddle", tCIDMacroEng::EConstTypes::Const, 0x0004)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kTotal"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kTotal", tCIDMacroEng::EConstTypes::Const, 0x0008)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kLeadTrail"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kLeadTrail", tCIDMacroEng::EConstTypes::Const, 0x0003)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFull"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kFull", tCIDMacroEng::EConstTypes::Const, 0x0007)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kComplete"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kComplete", tCIDMacroEng::EConstTypes::Const, 0x000F)
        )
    );


    // Binary addition (which is an append for this class)
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Add a level to the path
    {
        TMEngMethodInfo methiNew
        (
            L"AddLevel"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_AddLevel = c2AddMethodInfo(methiNew);
    }

    // Append another string onto this one
    {
        TMEngMethodInfo methiNew
        (
            L"Append"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Append = c2AddMethodInfo(methiNew);
    }

    // Append each type of cardinal with desired radix
    {
        TMEngMethodInfo methiNew
        (
            L"AppendCard1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendCard1= c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendCard2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendCard2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendCard4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendCard4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendCard8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendCard8 = c2AddMethodInfo(methiNew);
    }

    // Append a character onto the string
    {
        TMEngMethodInfo methiNew
        (
            L"AppendChar"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_AppendChar = c2AddMethodInfo(methiNew);
    }

    // Append an enum value
    {
        TMEngMethodInfo methiNew
        (
            L"AppendEnum"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Enum);
        methiNew.c2AddInParm(L"DoName", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_AppendEnum = c2AddMethodInfo(methiNew);
    }

    // Append the float variants with desired radix
    {
        TMEngMethodInfo methiNew
        (
            L"AppendFloat4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Float4);
        methiNew.c2AddInParm(L"DecDigits", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_AppendFloat4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendFloat8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.c2AddInParm(L"DecDigits", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_AppendFloat8 = c2AddMethodInfo(methiNew);
    }

    // Append a formatted formattable object onto this string
    {
        TMEngMethodInfo methiNew
        (
            L"AppendFmt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Formattable);
        m_c2MethId_AppendFmt = c2AddMethodInfo(methiNew);
    }

    // Append the int variants with desired radix
    {
        TMEngMethodInfo methiNew
        (
            L"AppendInt1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Int1);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendInt1 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendInt2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Int2);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendInt2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AppendInt4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::Int4);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_AppendInt4 = c2AddMethodInfo(methiNew);
    }

    // Cap the string at the given index
    {
        TMEngMethodInfo methiNew
        (
            L"CapAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CapAt = c2AddMethodInfo(methiNew);
    }

    // Clear the string contents
    {
        TMEngMethodInfo methiNew
        (
            L"Clear"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Clear = c2AddMethodInfo(methiNew);
    }

    // Compare strings, case insensitive
    {
        TMEngMethodInfo methiNew
        (
            L"CompNC"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Src", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CompNC = c2AddMethodInfo(methiNew);
    }

    // Cut out a range
    {
        TMEngMethodInfo methiNew
        (
            L"Cut"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Cut = c2AddMethodInfo(methiNew);
    }

    // Convert a decimal digit at index to it's binary form
    {
        TMEngMethodInfo methiNew
        (
            L"CvtDecDigitAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CvtDecDigitAt = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.String"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Delete the last character, if any
    {
        TMEngMethodInfo methiNew
        (
            L"DelLast"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_DelLast = c2AddMethodInfo(methiNew);
    }

    // Check equality
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Extract the extension from this path to the passed string
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractExt"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ExtractExt = c2AddMethodInfo(methiNew);
    }

    // Convert a substring to a numeric value
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractCard4"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractCard4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractCard8"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractCard8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractCard4R"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ExtractCard4R = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractCard8R"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ExtractCard8R = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractFloat8"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractFloat8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractInt4"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractInt4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractInt4R"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ExtractInt4R = c2AddMethodInfo(methiNew);
    }

    // Extract the name.ext from this path to the passed string
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractNameExt"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ExtractNameExt = c2AddMethodInfo(methiNew);
    }

    // Extract the path parth from this path to the passed string
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractPath"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ExtractPath = c2AddMethodInfo(methiNew);
    }

    // Extract a substring
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractSubStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ExtractSubStr = c2AddMethodInfo(methiNew);
    }

    // Extract from the start index, up to (but not including) the terminator
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractUpTo"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"TermChar", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ExtractUpTo = c2AddMethodInfo(methiNew);
    }

    // Find the first/last/next matching char in this string
    {
        TMEngMethodInfo methiNew
        (
            L"FindFirstChar"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindFirstChar = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FindLastChar"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindLastChar = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FindNextChar"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindNextChar = c2AddMethodInfo(methiNew);
    }

    // Find the next substring from a starting point
    {
        TMEngMethodInfo methiNew
        (
            L"FindNextSubStr"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindNextSubStr = c2AddMethodInfo(methiNew);
    }

    // Search this string for the passed substring
    {
        TMEngMethodInfo methiNew
        (
            L"FindSubStr"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindSubStr = c2AddMethodInfo(methiNew);
    }

    // Search this string any replacement tokens
    {
        TMEngMethodInfo methiNew
        (
            L"FindTokenList"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FindTokenList = c2AddMethodInfo(methiNew);
    }

    // Format a string object into ourself, in a field
    {
        TMEngMethodInfo methiNew
        (
            L"FmtToField"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Formattable);
        methiNew.c2AddInParm(L"Width", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Justify", m_pmeciHJustify->c2Id());
        methiNew.c2AddInParm(L"FillChar", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_FmtToField = c2AddMethodInfo(methiNew);
    }

    // Get the allocation size
    {
        TMEngMethodInfo methiNew
        (
            L"GetAlloc"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetAlloc = c2AddMethodInfo(methiNew);
    }

    // Get a char at index
    {
        TMEngMethodInfo methiNew
        (
            L"GetAt"
            , tCIDMacroEng::EIntrinsics::Char
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetAt = c2AddMethodInfo(methiNew);
    }

    // Get the extension
    {
        TMEngMethodInfo methiNew
        (
            L"GetExt"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetExt = c2AddMethodInfo(methiNew);
    }

    // Get the last character
    {
        TMEngMethodInfo methiNew
        (
            L"GetLast"
            , tCIDMacroEng::EIntrinsics::Char
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLast = c2AddMethodInfo(methiNew);
    }

    // Get the length
    {
        TMEngMethodInfo methiNew
        (
            L"GetLength"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLength = c2AddMethodInfo(methiNew);
    }

    // Get the name.ext
    {
        TMEngMethodInfo methiNew
        (
            L"GetNameExt"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetNameExt = c2AddMethodInfo(methiNew);
    }

    // Get the path part
    {
        TMEngMethodInfo methiNew
        (
            L"GetPath"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetPath = c2AddMethodInfo(methiNew);
    }

    // Check greater than and >=
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_GtThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GtThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Check if we have an extension
    {
        TMEngMethodInfo methiNew
        (
            L"HasExt"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_HasExt = c2AddMethodInfo(methiNew);
    }

    // Check if we have an name
    {
        TMEngMethodInfo methiNew
        (
            L"HasName"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_HasName = c2AddMethodInfo(methiNew);
    }

    // Check if we have a path
    {
        TMEngMethodInfo methiNew
        (
            L"HasPath"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_HasPath = c2AddMethodInfo(methiNew);
    }

    // Insert a char or string at a point
    {
        TMEngMethodInfo methiNew
        (
            L"InsertCharAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToInsert", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"InsertAt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_InsertCharAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"InsertStrAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToInsert", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"InsertAt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_InsertStrAt = c2AddMethodInfo(methiNew);
    }

    // Check if the string is empty
    {
        TMEngMethodInfo methiNew
        (
            L"IsEmpty"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_IsEmpty = c2AddMethodInfo(methiNew);
    }

    // Check greater than and >=
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_LsThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"LsThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Parse a x.x or x.x.x version string into a Card8 value
    {
        TMEngMethodInfo methiNew
        (
            L"ParseVersion"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ThreeVals", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_ParseVersion = c2AddMethodInfo(methiNew);
    }

    // Unary addition (which is an append in this class)
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Prepend another string or char into this one
    {
        TMEngMethodInfo methiNew
        (
            L"Prepend"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToPrepend", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Prepend = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PrependChar"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToPrepend", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_PrependChar = c2AddMethodInfo(methiNew);
    }

    // Set the character at the given index
    {
        TMEngMethodInfo methiNew
        (
            L"PutAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_PutAt = c2AddMethodInfo(methiNew);
    }

    // Reallocate the buffer
    {
        TMEngMethodInfo methiNew
        (
            L"Reallocate"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"NewSize", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"RetainOld", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Reallocate = c2AddMethodInfo(methiNew);
    }

    // Remove a level from the end of the path
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveLevel"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_RemoveLevel = c2AddMethodInfo(methiNew);
    }

    // Remove a trailing separator from the end of the path
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveTrailingSep"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_RemoveTrailSep = c2AddMethodInfo(methiNew);
    }

    // Replace all instances of one character with another one
    {
        TMEngMethodInfo methiNew
        (
            L"ReplaceChar"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToReplace", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"ReplaceWith", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_ReplaceChar = c2AddMethodInfo(methiNew);
    }

    // Replace or or all instances of one substring with another
    {
        TMEngMethodInfo methiNew
        (
            L"ReplaceSubStr"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToReplace", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ReplaceWith", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ReplaceAll", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_ReplaceSubStr = c2AddMethodInfo(methiNew);
    }

    // Replace a token in this string
    {
        TMEngMethodInfo methiNew
        (
            L"ReplaceToken"
            , m_pmeciFindRes->c2Id()
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToReplace", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"RepData", tCIDMacroEng::EIntrinsics::Formattable);
        m_c2MethId_ReplaceToken = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that let's them indicate an initial size
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.String"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitSize", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SizeCtor = c2AddMethodInfo(methiNew);
    }

    // Split into two parts based on a divider char
    {
        TMEngMethodInfo methiNew
        (
            L"Split"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"SecondHalf", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Divider", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Split = c2AddMethodInfo(methiNew);
    }

    // See if this string starts with a substring
    {
        TMEngMethodInfo methiNew
        (
            L"StartsWith"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"CaseSensitive", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_StartsWith = c2AddMethodInfo(methiNew);
    }

    // Strip off leading/trailing whitespace or chars
    {
        TMEngMethodInfo methiNew
        (
            L"StripChars"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToStrip", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_StripChars = c2AddMethodInfo(methiNew);
    }

    // A more advanced stripper
    {
        TMEngMethodInfo methiNew
        (
            L"StripChars2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToStrip", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Flags", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_StripChars2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"StripWhitespace"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_StripWhitespace = c2AddMethodInfo(methiNew);
    }

    // Sum the values of a range of chars
    {
        TMEngMethodInfo methiNew
        (
            L"SumChars"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SumChars = c2AddMethodInfo(methiNew);
    }

    // Convert to the numeric types
    {
        TMEngMethodInfo methiNew
        (
            L"ToCard4"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ToCard4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToCard4R"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ToCard4R = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToCard4Ex"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ToCard4Ex = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToCard8"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ToCard8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToCard8R"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ToCard8R = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToCard8Ex"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_ToCard8Ex = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToFloat4"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ToFloat4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToFloat4Ex"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_ToFloat4Ex = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToFloat8"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ToFloat8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToFloat8Ex"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_ToFloat8Ex = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToInt4"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ToInt4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToInt4R"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        m_c2MethId_ToInt4R = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToInt4Ex"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Radix", m_pmeciRadices->c2Id());
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_ToInt4Ex = c2AddMethodInfo(methiNew);
    }


    // Convert to lower or upper cass
    {
        TMEngMethodInfo methiNew
        (
            L"ToLower"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ToLower = c2AddMethodInfo(methiNew);
    }

    // Convert to lower or upper cass
    {
        TMEngMethodInfo methiNew
        (
            L"ToUpper"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ToUpper = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor3_MEng.String"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::String);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngStringInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngStringVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngStringInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngStringVal& mecvActual = static_cast<TMEngStringVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Add)
    {
        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strValue());
        mecvRet.strValue().Append(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_AddLevel)
    {
        mecvActual.pathValue().AddLevel(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Append)
    {
        mecvActual.strValue().Append(meOwner.strStackValAt(c4FirstInd));
    }
     else if ((c2MethId == m_c2MethId_AppendCard1)
          ||  (c2MethId == m_c2MethId_AppendCard2)
          ||  (c2MethId == m_c2MethId_AppendCard4)
          ||  (c2MethId == m_c2MethId_AppendCard8))
    {
        //
        //  Get the cardinal value and radix values. We can just use a Card8
        //  which can hold any of them. The formatted result won't be any
        //  different.
        //
        tCIDLib::TCard8 c8Val;
        if (c2MethId == m_c2MethId_AppendCard1)
            c8Val = meOwner.c1StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendCard2)
            c8Val = meOwner.c2StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendCard4)
            c8Val = meOwner.c4StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendCard8)
            c8Val = meOwner.c8StackValAt(c4FirstInd);

        const TMEngEnumVal& mecvRadix
            = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
        const tCIDLib::ERadices eRadix = tCIDLib::ERadices
        (
            m_pmeciRadices->c4MapValue(mecvRadix)
        );

        // Format into a local temp and then append to us
        const tCIDLib::TCard4 c4BufSz = 128;
        tCIDLib::TCh achBuf[c4BufSz + 1];
        TRawStr::bFormatVal(c8Val, achBuf, c4BufSz, eRadix);
        mecvActual.strValue().Append(achBuf);
    }
     else if (c2MethId == m_c2MethId_AppendChar)
    {
        mecvActual.strValue().Append(meOwner.chStackValAt(c4FirstInd));
    }
     else if ((c2MethId == m_c2MethId_AppendFloat4)
          ||  (c2MethId == m_c2MethId_AppendFloat8))
    {
        // Get the float value and decimal digits value
        tCIDLib::TFloat8 f8Val;
        if (c2MethId == m_c2MethId_AppendFloat4)
            f8Val = meOwner.f4StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendFloat8)
            f8Val = meOwner.f8StackValAt(c4FirstInd);

        const tCIDLib::TCard4 c4DecDigs = meOwner.c4StackValAt(c4FirstInd + 1);

        // Format into a local temp and then append to us
        const tCIDLib::TCard4 c4BufSz = 128;
        tCIDLib::TCh achBuf[c4BufSz + 1];
        TRawStr::bFormatVal(f8Val, achBuf, c4DecDigs, c4BufSz, tCIDLib::ETrailFmts::Zeroes);
        mecvActual.strValue().Append(achBuf);
    }
     else if (c2MethId == m_c2MethId_AppendEnum)
    {
        //
        //  Get the enum value and look up it's class type so that we can
        //  get the text value.
        //
        const TMEngEnumVal& mecvAppend
            = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        const TMEngEnumInfo& meciEnum = meOwner.meciFindAs<TMEngEnumInfo>
        (
            mecvAppend.c2ClassId()
        );

        // Get the flag that indicates to format name or text
        const tCIDLib::TBoolean bDoName = meOwner.bStackValAt(c4FirstInd + 1);

        if (bDoName)
            mecvActual.strValue().Append(meciEnum.strItemName(mecvAppend));
        else
            mecvActual.strValue().Append(meciEnum.strTextValue(mecvAppend));
    }
     else if ((c2MethId == m_c2MethId_AppendFmt)
          ||  (c2MethId == m_c2MethId_ReplaceToken))
    {
        //
        //  These have similar needs and there is too much code to want to
        //  do it twice, so despite the extra complexity, they are put
        //  together here. Both have to handle any formattable, and want to
        //  optimize for the intrisnic types.
        //
        const tCIDLib::TBoolean bDoAppend = (c2MethId == m_c2MethId_AppendFmt);
        const tCIDLib::TCard4 c4FmtIndex = bDoAppend ? c4FirstInd : c4FirstInd + 1;

        // Get the param to format and it's type
        TMEngClassVal& mecvFmt = meOwner.mecvStackAt(c4FmtIndex);
        TMEngClassInfo& meciTarget = meOwner.meciFind(mecvFmt.c2ClassId());

        // Get the token character out if doing the token replacement
        tCIDLib::TCh chTok = 0;
        if (!bDoAppend)
            chTok = meOwner.chStackValAt(c4FirstInd);

        // Call a helper since the amount of code is pretty significant
        TMEngClassInfo& meciThis = meOwner.meciFind(mecvActual.c2ClassId());
        tCIDLib::EFindRes eRes = eAppendFmt
        (
            meOwner, bDoAppend, meciTarget, chTok, mecvActual.strValue(), mecvFmt
        );

        // If a replace token, return the enum result
        if (!bDoAppend)
        {
            TMEngEnumVal& mecvRes = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
            mecvRes.c4Ordinal(tCIDLib::c4EnumOrd(eRes));
        }
    }
     else if ((c2MethId == m_c2MethId_AppendInt1)
          ||  (c2MethId == m_c2MethId_AppendInt2)
          ||  (c2MethId == m_c2MethId_AppendInt4))
    {
        //
        //  Get the integral value and radix values. We can get them all
        //  into an Int8. It won't affect the formatted result.
        //
        tCIDLib::TInt8 i8Val;

        if (c2MethId == m_c2MethId_AppendInt1)
            i8Val = meOwner.i1StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendInt2)
            i8Val = meOwner.i2StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_AppendInt4)
            i8Val = meOwner.i4StackValAt(c4FirstInd);

        const TMEngEnumVal& mecvRadix
            = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
        const tCIDLib::ERadices eRadix = tCIDLib::ERadices
        (
            m_pmeciRadices->c4MapValue(mecvRadix)
        );

        // Format into a local temp and then append to us
        const tCIDLib::TCard4 c4BufSz = 128;
        tCIDLib::TCh achBuf[c4BufSz + 1];
        TRawStr::bFormatVal(i8Val, achBuf, c4BufSz, eRadix);
        mecvActual.strValue().Append(achBuf);
    }
     else if (c2MethId == m_c2MethId_CapAt)
    {
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);

        // Note that we allow a cap at the current end, so > not >=
        if (c4Index > mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));
        mecvActual.strValue().CapAt(c4Index);
    }
     else if (c2MethId == m_c2MethId_Clear)
    {
        mecvActual.strValue().Clear();
    }
     else if (c2MethId == m_c2MethId_CompNC)
    {
        TMEngBooleanVal& mecvRes = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
        mecvRes.bValue
        (
            mecvActual.strValue().eCompareI(strSrc) == tCIDLib::ESortComps::Equal
        );
    }
     else if (c2MethId == m_c2MethId_Cut)
    {
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckRange(meOwner, c4Index, c4Count, mecvActual.c4Length());
        mecvActual.strValue().Cut(c4Index, c4Count);
    }
     else if (c2MethId == m_c2MethId_CvtDecDigitAt)
    {
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        if (c4Index >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));

        const tCIDLib::TCh chAt = mecvActual.strValue().chAt(c4Index);
        if (!TRawStr::bIsDigit(chAt))
        {
            TString strTmp(chAt);
            ThrowAnErr(meOwner, m_c4ErrNotDecDigit, strTmp);
        }

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(chAt - kCIDLib::chDigit0);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just empties the string
        mecvActual.strValue().Clear();
    }
     else if (c2MethId == m_c2MethId_DelLast)
    {
        mecvActual.strValue().DeleteLast();
    }
     else if ((c2MethId == m_c2MethId_ExtractCard4)
          ||  (c2MethId == m_c2MethId_ExtractCard4R)
          ||  (c2MethId == m_c2MethId_ExtractCard8)
          ||  (c2MethId == m_c2MethId_ExtractCard8R)
          ||  (c2MethId == m_c2MethId_ExtractFloat8)
          ||  (c2MethId == m_c2MethId_ExtractInt4)
          ||  (c2MethId == m_c2MethId_ExtractInt4R))
    {
        // Get the substring and validate it
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckRange(meOwner, c4Index, c4Count, mecvActual.c4Length());

        // Pull out the substring we will convert
        TString strToCvt;
        mecvActual.strValue().CopyOutSubStr(strToCvt, c4Index, c4Count);

        //
        //  And, according to the type, convert it and put the return value
        //  out, if successful.
        //
        tCIDLib::TBoolean bOk;
        if ((c2MethId == m_c2MethId_ExtractCard4)
        ||  (c2MethId == m_c2MethId_ExtractCard4R))
        {
            // Set up the radix, which may not be present
            tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
            if (c2MethId == m_c2MethId_ExtractCard4R)
            {
                const TMEngEnumVal& mecvRadix
                    = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
                eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
            }

            const tCIDLib::TCard4 c4Tmp = TRawStr::c4AsBinary
            (
                strToCvt.pszBuffer(), bOk, eRadix
            );

            if (bOk)
                meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1).c4Value(c4Tmp);
            else
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strToCvt, TMEngCard4Info::strPath());
        }
         else if ((c2MethId == m_c2MethId_ExtractCard8)
              ||  (c2MethId == m_c2MethId_ExtractCard8R))
        {
            // Set up the radix, which may not be present
            tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
            if (c2MethId == m_c2MethId_ExtractCard8R)
            {
                const TMEngEnumVal& mecvRadix
                    = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
                eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
            }

            const tCIDLib::TCard8 c8Tmp = TRawStr::c8AsBinary
            (
                strToCvt.pszBuffer(), bOk, eRadix
            );

            if (bOk)
                meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1).c8Value(c8Tmp);
            else
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strToCvt, TMEngCard8Info::strPath());
        }
         else if (c2MethId == m_c2MethId_ExtractFloat8)
        {
            const tCIDLib::TFloat8 f8Tmp = TRawStr::f8AsBinary
            (
                strToCvt.pszBuffer(), bOk
            );

            if (bOk)
                meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1).f8Value(f8Tmp);
            else
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strToCvt, TMEngFloat8Info::strPath());
        }
         else if ((c2MethId == m_c2MethId_ExtractInt4)
              ||  (c2MethId == m_c2MethId_ExtractInt4R))
        {
            // Set up the radix, which may not be present
            tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
            if (c2MethId == m_c2MethId_ExtractInt4R)
            {
                const TMEngEnumVal& mecvRadix
                    = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
                eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
            }

            const tCIDLib::TInt4 i4Tmp = TRawStr::i4AsBinary
            (
                strToCvt.pszBuffer(), bOk, eRadix
            );

            if (bOk)
                meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1).i4Value(i4Tmp);
            else
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strToCvt, TMEngInt4Info::strPath());
        }
    }
     else if (c2MethId == m_c2MethId_ExtractExt)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bExtractExt(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_ExtractNameExt)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bExtractNameExt(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_ExtractPath)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bExtractPath(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_ExtractSubStr)
    {
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckRange(meOwner, c4Index, c4Count, mecvActual.c4Length());
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvActual.strValue().CopyOutSubStr(mecvToFill.strValue(), c4Index, c4Count);
    }
     else if (c2MethId == m_c2MethId_ExtractUpTo)
    {
        // Get the parms out
        tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCh chTerm = meOwner.chStackValAt(c4FirstInd + 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);

        // Validate the range info
        if (c4Index >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));

        // Get refs to the source (us) and target string
        const TString& strSrc = mecvActual.strValue();
        TString& strTar = mecvToFill.strValue();

        //
        //  Empty the target and then starting at the index, copy over any
        //  chars from the source until we hit the end or the terminator.
        //
        const tCIDLib::TCard4 c4Len = strSrc.c4Length();
        strTar.Clear();
        while (c4Index < c4Len)
        {
            const tCIDLib::TCh chCur = strSrc[c4Index++];
            if (chCur == chTerm)
                break;
            strTar.Append(chCur);
        }

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Index);
    }
     else if ((c2MethId == m_c2MethId_FindFirstChar)
          ||  (c2MethId == m_c2MethId_FindLastChar))
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TCh chFind = meOwner.chStackValAt(c4FirstInd);
        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
        const tCIDLib::TBoolean bCase = meOwner.bStackValAt(c4FirstInd + 2);
        tCIDLib::TBoolean   bRes;
        tCIDLib::TCard4     c4At;
        if (c2MethId == m_c2MethId_FindFirstChar)
            bRes = mecvActual.strValue().bFirstOccurrence(chFind, c4At, bCase);
        else
            bRes = mecvActual.strValue().bLastOccurrence(chFind, c4At, bCase);
        mecvRet.bValue(bRes);
        mecvAt.c4Value(c4At);
    }
     else if (c2MethId == m_c2MethId_FindNextChar)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TCh chFind = meOwner.chStackValAt(c4FirstInd);
        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
        const tCIDLib::TBoolean bCase = meOwner.bStackValAt(c4FirstInd + 2);

        // In this case, the index is an in/out, so check the incoming
        tCIDLib::TCard4 c4At = mecvAt.c4Value();
        if (c4At >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4At));

        if (mecvActual.strValue().bNextOccurrence(chFind, c4At, bCase))
        {
            mecvRet.bValue(kCIDLib::True);
            mecvAt.c4Value(c4At);
        }
         else
        {
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_FindNextSubStr)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strFind = meOwner.strStackValAt(c4FirstInd);
        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
        const tCIDLib::TBoolean bCase = meOwner.bStackValAt(c4FirstInd + 2);

        // At is an in/out in this case, so get the incoming value to pass in
        tCIDLib::TCard4 c4At = mecvAt.c4Value();
        if (mecvActual.strValue().bNextOccurrence(strFind, c4At, kCIDLib::False, bCase))
        {
            mecvRet.bValue(kCIDLib::True);
            mecvAt.c4Value(c4At);
        }
         else
        {
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_FindSubStr)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strFind = meOwner.strStackValAt(c4FirstInd);
        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
        const tCIDLib::TBoolean bCase = meOwner.bStackValAt(c4FirstInd + 2);

        tCIDLib::TCard4 c4At;
        if (mecvActual.strValue().bFirstOccurrence(strFind, c4At, kCIDLib::False, bCase))
        {
            mecvRet.bValue(kCIDLib::True);
            mecvAt.c4Value(c4At);
        }
         else
        {
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_FindTokenList)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvRet.bValue(mecvActual.strValue().bFindTokenList(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_FmtToField)
    {
        try
        {
            //
            //  First we want to format the foramttable object into a
            //  local string. Call a helper since the amount of code is
            //  pretty significant.
            //
            TString strFmt;
            TMEngClassVal& mecvFmt = meOwner.mecvStackAt(c4FirstInd);
            eAppendFmt
            (
                meOwner
                , kCIDLib::True
                , meOwner.meciFind(mecvFmt.c2ClassId())
                , kCIDLib::chNull
                , strFmt
                , mecvFmt
            );

            // And format that string into the field in this string
            TMEngEnumVal& mecvJust = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
            mecvActual.strValue().FormatToFld
            (
                strFmt
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , tCIDLib::EHJustify(m_pmeciHJustify->c4MapValue(mecvJust))
                , meOwner.chStackValAt(c4FirstInd + 3)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrFmtFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetAlloc)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.strValue().c4BufChars());
    }
     else if (c2MethId == m_c2MethId_GetAt)
    {
        // Get the parm, which is the index and check it. Throw if bad
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        if (c4Index >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));

        // Get the return value and set it with the indicated char
        TMEngCharVal& mecvRet = meOwner.mecvStackAtAs<TMEngCharVal>(c4FirstInd - 1);
        mecvRet.chValue(mecvActual.strValue().chAt(c4Index));
    }
     else if (c2MethId == m_c2MethId_GetExt)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bQueryExt(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_GetLast)
    {
        TMEngCharVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngCharVal>(c4FirstInd - 1);
        mecvRet.chValue(mecvActual.strValue().chLast());
    }
     else if (c2MethId == m_c2MethId_GetLength)
    {
        // Get the return value and set it with our length
        TMEngCard4Val& mecvRet
                = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Length());
    }
     else if (c2MethId == m_c2MethId_GetNameExt)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bQueryNameExt(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_GetPath)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        // Make sure the target isn't us
        CheckSrcTarSame(meOwner, mecvActual, mecvToFill);
        mecvRet.bValue(mecvActual.pathValue().bQueryPath(mecvToFill.strValue()));
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThan)
          ||  (c2MethId == m_c2MethId_LsThanEq))
    {
        // Get the boolean return and the string parm
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strParm = meOwner.strStackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.strValue() > strParm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.strValue() >= strParm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.strValue() < strParm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.strValue() <= strParm);
        else
            mecvRet.bValue(mecvActual.strValue() == strParm);
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.strValue();
    }
     else if (c2MethId == m_c2MethId_HasExt)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.pathValue().bHasExt());
    }
     else if (c2MethId == m_c2MethId_HasName)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.pathValue().bHasName());
    }
     else if (c2MethId == m_c2MethId_HasPath)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.pathValue().bHasPath());
    }
     else if (c2MethId == m_c2MethId_InsertCharAt)
    {
        // Make sure the start index is valid
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4At >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4At));

        tCIDLib::TCh achInsert[2];
        achInsert[0] = meOwner.chStackValAt(c4FirstInd);
        achInsert[1] = kCIDLib::chNull;
        mecvActual.strValue().Insert(achInsert, c4At);
    }
     else if (c2MethId == m_c2MethId_InsertStrAt)
    {
        // Make sure the start index is valid
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4At >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4At));
        mecvActual.strValue().Insert(meOwner.strStackValAt(c4FirstInd), c4At);
    }
     else if (c2MethId == m_c2MethId_IsEmpty)
    {
        meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1).bValue
        (
            mecvActual.strValue().bIsEmpty()
        );
    }
     else if (c2MethId == m_c2MethId_ParseVersion)
    {
        tCIDLib::TCard8 c8Ret = 0;
        try
        {
            c8Ret = TModule::c8ParseVersionStr
            (
                mecvActual.strValue(), meOwner.bStackValAt(c4FirstInd)
            );
        }

        catch(...)
        {
            // Just fall through with the zero
        }

        TMEngCard8Val& mecvRet
                = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(c8Ret);
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        mecvActual.strValue().Append(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PutAt)
    {
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        if (c4Index >= mecvActual.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));
        mecvActual.strValue().PutAt(c4Index, meOwner.chStackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_Prepend)
    {
        mecvActual.strValue().Prepend(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PrependChar)
    {
        mecvActual.strValue().Prepend(meOwner.chStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Reallocate)
    {
        const tCIDLib::TCard4 c4NewSize = meOwner.c4StackValAt(c4FirstInd);
        if (c4NewSize > CIDMacroEng_StringClasses::c4MaxChars)
            ThrowAnErr(meOwner, m_c4ErrTooLarge, TCardinal(c4NewSize));
        mecvActual.strValue().Reallocate
        (
            c4NewSize, meOwner.bStackValAt(c4FirstInd + 1)
        );
    }
     else if (c2MethId == m_c2MethId_RemoveLevel)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.pathValue().bRemoveLevel());
    }
     else if (c2MethId == m_c2MethId_RemoveTrailSep)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.pathValue().bRemoveTrailingSeparator());
    }
     else if (c2MethId == m_c2MethId_ReplaceChar)
    {
        // Get the two chars, first is the one to replace, second is replace value
        const tCIDLib::TCh chToRep = meOwner.chStackValAt(c4FirstInd);
        const tCIDLib::TCh chRepWith = meOwner.chStackValAt(c4FirstInd + 1);
        mecvActual.strValue().bReplaceChar(chToRep, chRepWith);
    }
     else if (c2MethId == m_c2MethId_ReplaceSubStr)
    {
        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2);
        tCIDLib::TCard4 c4Index = mecvAt.c4Value();

        // The start at can't be at or beyond the end of the string
        if (c4Index > mecvActual.strValue().c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));

        const tCIDLib::TBoolean bFound = mecvActual.strValue().bReplaceSubStr
        (
            meOwner.strStackValAt(c4FirstInd)
            , meOwner.strStackValAt(c4FirstInd + 1)
            , c4Index
            , meOwner.bStackValAt(c4FirstInd + 3)
            , meOwner.bStackValAt(c4FirstInd + 4)
        );

        // Give back the found at index (max card if no matches found)
        mecvAt.c4Value(c4Index);

        // Return whether we replaced any instances
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(bFound);
    }
     else if (c2MethId == m_c2MethId_SizeCtor)
    {
        // Clip it back to the max size if greater
        tCIDLib::TCard4 c4NewSize = meOwner.c4StackValAt(c4FirstInd);
        if (c4NewSize > CIDMacroEng_StringClasses::c4MaxChars)
            c4NewSize = CIDMacroEng_StringClasses::c4MaxChars;

        // Just reallocate the string to the indicated size
        mecvActual.strValue().Reallocate(c4NewSize, kCIDLib::False);
    }
     else if (c2MethId == m_c2MethId_Split)
    {
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvOut = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);

        mecvRet.bValue
        (
            mecvActual.strValue().bSplit
            (
                mecvOut.strValue()
                , meOwner.chStackValAt(c4FirstInd + 1)
                , meOwner.bStackValAt(c4FirstInd + 2)
            )
        );
    }
     else if (c2MethId == m_c2MethId_StartsWith)
    {
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strPref = meOwner.strStackValAt(c4FirstInd);
        if (meOwner.bStackValAt(c4FirstInd + 1))
            mecvRet.bValue(mecvActual.strValue().bStartsWith(strPref));
        else
            mecvRet.bValue(mecvActual.strValue().bStartsWithI(strPref));
    }
     else if (c2MethId == m_c2MethId_StripChars)
    {
        const TString& strToStrip = meOwner.strStackValAt(c4FirstInd);
        mecvActual.strValue().Strip(strToStrip);
    }
     else if (c2MethId == m_c2MethId_StripChars2)
    {
        const TString& strToStrip = meOwner.strStackValAt(c4FirstInd);
        const tCIDLib::EStripModes eMode
        (
            tCIDLib::EStripModes(meOwner.c4StackValAt(c4FirstInd + 1))
        );
        mecvActual.strValue().Strip(strToStrip.pszBuffer(), eMode);
    }
     else if (c2MethId == m_c2MethId_StripWhitespace)
    {
        mecvActual.strValue().StripWhitespace();
    }
     else if (c2MethId == m_c2MethId_SumChars)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const tCIDLib::TCard4 c4Start = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckRange(meOwner, c4Start, c4Count, mecvActual.c4Length());

        const tCIDLib::TCh* pszSrc = mecvActual.strValue().pszBuffer() + c4Start;
        const tCIDLib::TCh* pszEnd = pszSrc + c4Count;
        tCIDLib::TCard4 c4Sum = 0;
        while (pszSrc < pszEnd)
            c4Sum += tCIDLib::TCard4(*pszSrc++);
        mecvRet.c4Value(c4Sum);
    }
     else if ((c2MethId == m_c2MethId_ToCard4)
          ||  (c2MethId == m_c2MethId_ToCard4R)
          ||  (c2MethId == m_c2MethId_ToCard4Ex))
    {
        tCIDLib::TBoolean bOk;
        const TString& strVal = mecvActual.strValue();

        tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
        if (c2MethId != m_c2MethId_ToCard4)
        {
            const TMEngEnumVal& mecvRadix
                = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
        }

        const tCIDLib::TCard4 c4Tmp = TRawStr::c4AsBinary
        (
            strVal.pszBuffer(), bOk, eRadix
        );

        if (c2MethId == m_c2MethId_ToCard4Ex)
        {
            // Here the return is the bookean result and the value an out parm
            TMEngCard4Val& mecvVal
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
            mecvVal.c4Value(c4Tmp);

            TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bOk);
        }
         else
        {
            // Here we throw if bad, else return the value
            if (!bOk)
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strVal, TMEngCard4Info::strPath());
            TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Tmp);
        }
    }
     else if ((c2MethId == m_c2MethId_ToCard8)
          ||  (c2MethId == m_c2MethId_ToCard8R)
          ||  (c2MethId == m_c2MethId_ToCard8Ex))
    {
        tCIDLib::TBoolean bOk;
        const TString& strVal = mecvActual.strValue();

        tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
        if (c2MethId != m_c2MethId_ToCard8)
        {
            const TMEngEnumVal& mecvRadix
                = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
        }

        const tCIDLib::TCard8 c8Tmp = TRawStr::c8AsBinary
        (
            strVal.pszBuffer(), bOk, eRadix
        );

        if (c2MethId == m_c2MethId_ToCard8Ex)
        {
            // Here the return is the bookean result and the value an out parm
            TMEngCard8Val& mecvVal = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd + 1);
            mecvVal.c8Value(c8Tmp);

            TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bOk);
        }
         else
        {
            if (!bOk)
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strVal, TMEngCard8Info::strPath());
            TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
            mecvRet.c8Value(c8Tmp);
        }
    }
     else if ((c2MethId == m_c2MethId_ToFloat4)
          ||  (c2MethId == m_c2MethId_ToFloat4Ex))
    {
        tCIDLib::TBoolean bOk;
        const TString& strVal = mecvActual.strValue();
        const tCIDLib::TFloat8 f8Tmp = TRawStr::f8AsBinary(strVal.pszBuffer(), bOk);
        if ((f8Tmp < kCIDLib::f4MinFloat) || (f8Tmp > kCIDLib::f4MaxFloat))
            bOk = kCIDLib::False;

        if (c2MethId == m_c2MethId_ToFloat4Ex)
        {
            // Here the return is the bookean result and the value an out parm
            TMEngFloat4Val& mecvVal
                = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd);
            mecvVal.f4Value(tCIDLib::TFloat4(f8Tmp));

            TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bOk);
        }
         else
        {
            // Here we throw if bad, else return the value
            if (!bOk)
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strVal, TMEngFloat4Info::strPath());
            TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
            mecvRet.f4Value(tCIDLib::TFloat4(f8Tmp));
        }
    }
     else if ((c2MethId == m_c2MethId_ToFloat8)
          ||  (c2MethId == m_c2MethId_ToFloat8Ex))
    {
        tCIDLib::TBoolean bOk;
        const TString& strVal = mecvActual.strValue();
        const tCIDLib::TFloat8 f8Tmp = TRawStr::f8AsBinary(strVal.pszBuffer(), bOk);

        if (c2MethId == m_c2MethId_ToFloat8Ex)
        {
            // Here the return is the bookean result and the value an out parm
            TMEngFloat8Val& mecvVal
                = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd);
            mecvVal.f8Value(f8Tmp);

            TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bOk);
        }
         else
        {
            // Here we throw if bad, else return the value
            if (!bOk)
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strVal, TMEngFloat8Info::strPath());
            TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
            mecvRet.f8Value(f8Tmp);
        }
    }
     else if ((c2MethId == m_c2MethId_ToInt4)
          ||  (c2MethId == m_c2MethId_ToInt4R)
          ||  (c2MethId == m_c2MethId_ToInt4Ex))
    {
        tCIDLib::TBoolean bOk;
        const TString& strVal = mecvActual.strValue();

        // Get the radix if present
        tCIDLib::ERadices eRadix = tCIDLib::ERadices::Auto;
        if (c2MethId != m_c2MethId_ToInt4)
        {
            const TMEngEnumVal& mecvRadix
                = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            eRadix = tCIDLib::ERadices(m_pmeciRadices->c4MapValue(mecvRadix));
        }

        const tCIDLib::TInt4 i4Tmp = TRawStr::i4AsBinary
        (
            strVal.pszBuffer(), bOk, eRadix
        );


        if (c2MethId == m_c2MethId_ToInt4Ex)
        {
            // Here the return is the bookean result and the value an out parm
            TMEngInt4Val& mecvVal
                        = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd + 1);
            mecvVal.i4Value(i4Tmp);

            TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bOk);
        }
         else
        {
            // Here we throw if bad, else return the value
            if (!bOk)
                ThrowAnErr(meOwner, m_c4ErrCantConvert, strVal, TMEngInt4Info::strPath());

            TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
            mecvRet.i4Value(i4Tmp);
        }
    }
     else if (c2MethId == m_c2MethId_ToLower)
    {
        mecvActual.strValue().ToLower();
    }
     else if (c2MethId == m_c2MethId_ToUpper)
    {
        mecvActual.strValue().ToUpper();
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.strValue(meOwner.strStackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngStringInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EFindRes
TMEngStringInfo::eAppendFmt(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TBoolean   bDoAppend
                            , const TMEngClassInfo&     meciTarget
                            , const tCIDLib::TCh        chTok
                            ,       TString&            strTar
                            ,       TMEngClassVal&      mecvFmt)
{
    //
    //  Check for some special case ones that we can do with far less
    //  overhead. If not one of those, we have to do it the heavy way.
    //  Basically, any of the fundamental types, we can do directly by
    //  just looking straight at the value. Otherwise, we have ot create
    //  a macro level string stream and stream it out as a formattable,
    //  then use that as the replacement text.
    //
    tCIDLib::EFindRes eRes = tCIDLib::EFindRes::NotFound;
    switch(tCIDMacroEng::EIntrinsics(meciTarget.c2Id()))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        {
            tCIDLib::TBoolean bVal
                = static_cast<TMEngBooleanVal&>(mecvFmt).bValue();
            if (bDoAppend)
            {
                if (bVal)
                    strTar.Append(L"True");
                else
                    strTar.Append(L"False");
            }
             else
            {
                eRes = strTar.eReplaceToken(bVal, chTok);
            }
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card1 :
        {
            tCIDLib::TCard1 c1Val = static_cast<TMEngCard1Val&>(mecvFmt).c1Value();
            if (bDoAppend)
                strTar.AppendFormatted(c1Val);
            else
                eRes = strTar.eReplaceToken(c1Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card2 :
        {
            tCIDLib::TCard2 c2Val = static_cast<TMEngCard2Val&>(mecvFmt).c2Value();
            if (bDoAppend)
                strTar.AppendFormatted(c2Val);
            else
                eRes = strTar.eReplaceToken(c2Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card4 :
        {
            tCIDLib::TCard4 c4Val = static_cast<TMEngCard4Val&>(mecvFmt).c4Value();
            if (bDoAppend)
                strTar.AppendFormatted(c4Val);
            else
                eRes = strTar.eReplaceToken(c4Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Char :
        {
            tCIDLib::TCh chVal = static_cast<TMEngCharVal&>(mecvFmt).chValue();
            if (bDoAppend)
                strTar.Append(chVal);
            else
                eRes = strTar.eReplaceToken(chVal, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float4 :
        {
            tCIDLib::TFloat4 f4Val = static_cast<TMEngFloat4Val&>(mecvFmt).f4Value();
            if (bDoAppend)
                strTar.AppendFormatted(f4Val);
            else
                eRes = strTar.eReplaceToken(f4Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float8 :
        {
            tCIDLib::TFloat8 f8Val = static_cast<TMEngFloat8Val&>(mecvFmt).f8Value();
            if (bDoAppend)
                strTar.AppendFormatted(f8Val);
            else
                eRes = strTar.eReplaceToken(f8Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int1 :
        {
            tCIDLib::TInt1 i1Val = static_cast<TMEngInt1Val&>(mecvFmt).i1Value();
            if (bDoAppend)
                strTar.AppendFormatted(i1Val);
            else
                eRes = strTar.eReplaceToken(i1Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int2 :
        {
            tCIDLib::TInt2 i2Val = static_cast<TMEngInt2Val&>(mecvFmt).i2Value();
            if (bDoAppend)
                strTar.AppendFormatted(i2Val);
            else
                eRes = strTar.eReplaceToken(i2Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int4 :
        {
            tCIDLib::TInt4 i4Val = static_cast<TMEngInt4Val&>(mecvFmt).i4Value();
            if (bDoAppend)
                strTar.AppendFormatted(i4Val);
            else
                eRes = strTar.eReplaceToken(i4Val, chTok);
            break;
        }

        case tCIDMacroEng::EIntrinsics::String :
        {
            const TString& strVal = static_cast<TMEngStringVal&>(mecvFmt).strValue();
            if (bDoAppend)
                strTar.Append(strVal);
            else
                eRes = strTar.eReplaceToken(strVal, chTok);
            break;
        }

        default :
        {
            // Create a temp string out stream
            TMEngClassInfo& meciStrm = meOwner.meciFind
            (
                tCIDMacroEng::EIntrinsics::StringOutStream
            );
            TMEngTextOutStreamVal* pmecvStrm = static_cast<TMEngTextOutStreamVal*>
            (
                meciStrm.pmecvMakeStorage
                (
                    L"TempStream"
                    , meOwner
                    , tCIDMacroEng::EConstTypes::NonConst
                )
            );
            TJanitor<TMEngTextOutStreamVal> janStream(pmecvStrm);

            //
            //  Give it a stream, since we won't be constructing it at the
            //  macro language level, which normally is how it gets one.
            //
            TTextStringOutStream* pstrmTarget = new TTextStringOutStream(1024UL);
            pmecvStrm->SetStream(pstrmTarget, tCIDLib::EAdoptOpts::Adopt);

            //
            //  Generate a call to the object's FormatTo() method on the fly.
            //  This one needs to be a polymorphic dispatch. Lie and say that
            //  the stream is a 'repush', though its not, so that the engine
            //  won't delete it when it's popped.
            //
            meOwner.PushPoolValue(tCIDMacroEng::EIntrinsics::Void, tCIDMacroEng::EConstTypes::Const);
            meOwner.PushValue(pmecvStrm, tCIDMacroEng::EStackItems::Parm, kCIDLib::True);
            meOwner.meciPushMethodCall
            (
                meciTarget.c2Id()
                , TMEngFormattableInfo::c2FormatToId()
            );
            meciTarget.Invoke
            (
                meOwner
                , mecvFmt
                , TMEngFormattableInfo::c2FormatToId()
                , tCIDMacroEng::EDispatch::Poly
            );

            //
            //  We cheated this one, so we have to pop the method item as well
            //  the parm and return!
            //
            meOwner.MultiPop(3);

            // And finally do the token replacement or append
            pmecvStrm->Flush();
            if (bDoAppend)
                strTar.Append(pstrmTarget->strData());
            else
                eRes = strTar.eReplaceToken(pstrmTarget->strData(), chTok);
        }
    }
    return eRes;
}


//
//  Check the ranges used in many methods to insure that they are valid and
//  pre-emptively throw a CML exception rather than risking throwing a C++
//  exception.
//
tCIDLib::TVoid
TMEngStringInfo::CheckRange(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4Index
                            , const tCIDLib::TCard4     c4Count
                            , const tCIDLib::TCard4     c4Length)
{
    // Check the index first
    if (c4Index >= c4Length)
        ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index));

    //
    //  That's ok, so check the index plus count. If the count is max card,
    //  then they want to do the whole string.
    //
    tCIDLib::TCard4 c4ActualCount = c4Count;
    if ((c4ActualCount != kCIDLib::c4MaxCard)
    &&  ((c4Index + c4ActualCount) > c4Length))
    {
        ThrowAnErr(meOwner, m_c4ErrBadRange, TCardinal(c4Index), TCardinal(c4Count));
    }
}


//
//  Most methods that take a source (us) and target cannot allow the two to be
//  the same, so this is called to check for that. We throw if they are the
//  same object.
//
tCIDLib::TVoid
TMEngStringInfo::CheckSrcTarSame(       TCIDMacroEngine&    meOwner
                                , const TMEngStringVal&     mecvThis
                                , const TMEngStringVal&     mecvOther)
{
    if (&mecvThis == &mecvOther)
        ThrowAnErr(meOwner, m_c4ErrSrcTarSame);
}


// Helpers to throw our CML level exceptions in various ways
tCIDLib::TVoid
TMEngStringInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngStringInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngStringInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngStringInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngStrListVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStrListVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStrListVal::TMEngStrListVal(const  TString&                    strName
                                , const tCIDMacroEng::EConstTypes   eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::StringList), eConst)
    , m_bUnique(kCIDLib::True)
    , m_colValue(8)
{
}

TMEngStrListVal::~TMEngStrListVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngStrListVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngStrListVal::bAppend(const TString& strToAdd)
{
    if (m_bUnique && !bCheckUnique(strToAdd))
        return kCIDLib::False;
    m_colValue.objAdd(strToAdd);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngStrListVal::bFromSource(TMEngStrListVal& mecvSrc, const EFromTypes eType)
{
    // If copying or stealing, then we need to remove all our strings
    if ((eType == TMEngStrListVal::EFromTypes::Copy)
    ||  (eType == TMEngStrListVal::EFromTypes::Steal))
    {
        m_colValue.RemoveAll();
    }

    //
    //  If the source is not unique but we are, then we have to verify
    //  that we can take the src.
    //
    const tCIDLib::TCard4 c4Count = mecvSrc.m_colValue.c4ElemCount();
    if (m_bUnique && !mecvSrc.m_bUnique)
    {
        //
        //  Create a non-adopting ref hash table. If we can add all of the
        //  src strings to it, then we are safe. This avoids having to copy
        //  the strings or do a laborious n*n comparison.
        //
        //  We don't actually have a regular hash set in by ref form, so
        //  have to use the keyed one and give it a key extractor that just
        //  returns the source element.
        //
        TRefKeyedHashSet<TString, TString, TStringKeyOps> colTest
        (
            tCIDLib::EAdoptOpts::NoAdopt
            , 67
            , TStringKeyOps()
            , &strBogusKeyExtract
        );

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            // If we can't add it because it's not new, then return failure
            if (!colTest.bAddIfNew(&mecvSrc.m_colValue[c4Index]))
                return kCIDLib::False;
        }
    }

    if (eType == TMEngStrListVal::EFromTypes::Steal)
    {
        // Just steal the src collection's stuff
        m_colValue.StealAllFrom(mecvSrc.m_colValue);
    }
     else
    {
        // Copy over the values
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            m_colValue.objAdd(mecvSrc.m_colValue[c4Index]);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TMEngStrListVal::bInsertAt(const  tCIDLib::TCard4 c4At
                                            , const TString&        strToInsert)
{
    if (m_bUnique && !bCheckUnique(strToInsert))
        return kCIDLib::False;
    m_colValue.InsertAt(strToInsert, c4At);
    return kCIDLib::True;
}


tCIDLib::TBoolean TMEngStrListVal::bSetAt(  const   tCIDLib::TCard4 c4At
                                            , const TString&        strNewValue)
{
    //
    //  Do a uniqueness check. But, if it fails, see if it's the target string
    //  that caused it. If so, then do nothing but return success. Else we
    //  return failure.
    //
    if (m_bUnique && !bCheckUnique(strNewValue))
    {
        if (m_colValue[c4At] == strNewValue)
            return kCIDLib::True;
        return kCIDLib::False;
    }

    m_colValue[c4At] = strNewValue;
    return kCIDLib::True;
}


tCIDLib::TBoolean TMEngStrListVal::bUnique() const
{
    return m_bUnique;
}

tCIDLib::TBoolean TMEngStrListVal::bUnique(const tCIDLib::TBoolean bToSet)
{
    m_bUnique = bToSet;
    return m_bUnique;
}


tCIDLib::TCard4 TMEngStrListVal::c4ElemCount() const
{
    return m_colValue.c4ElemCount();
}


tCIDLib::TCard4 TMEngStrListVal::c4FindString(const TString& strToFind)
{
    const tCIDLib::TCard4 c4Count = m_colValue.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colValue[c4Index] == strToFind)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


const TVector<TString>& TMEngStrListVal::colValue() const
{
    return m_colValue;
}

TVector<TString>& TMEngStrListVal::colValue()
{
    return m_colValue;
}


tCIDLib::TVoid TMEngStrListVal::RemoveAll()
{
    m_colValue.RemoveAll();
}


tCIDLib::TVoid TMEngStrListVal::RemoveAt(const tCIDLib::TCard4 c4At)
{
    m_colValue.RemoveAt(c4At);
}


tCIDLib::TVoid TMEngStrListVal::Reset(const tCIDLib::TBoolean bUnique)
{
    m_colValue.RemoveAll();
    m_bUnique = bUnique;
}

tCIDLib::TVoid
TMEngStrListVal::Reset( const   TVector<TString>&   colToSet
                        , const tCIDLib::TBoolean   bUnique)
{
    m_colValue = colToSet;
    m_bUnique  = bUnique;
}


const TString& TMEngStrListVal::strAt(const tCIDLib::TCard4 c4At) const
{
    return m_colValue[c4At];
}


tCIDLib::TVoid TMEngStrListVal::Set(const TMEngStrListVal& mecvToSet)
{
    m_colValue = mecvToSet.m_colValue;
}



// ---------------------------------------------------------------------------
//  TMEngStrListVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStrListVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&     meciThis
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        // Display the string values.
        tCIDLib::TCard4 c4Count = m_colValue.c4ElemCount();
        strmTarget << L"Count: " << c4Count;
        if (c4Count > 256)
        {
            strmTarget << L" (Showing first 256 entries...)";
            c4Count = 256;
        }

        TStreamFmt strmfIndex(3, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfString(0, 0, tCIDLib::EHJustify::Left);
        strmTarget << L"\nStrings:\n";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmTarget  << strmfIndex << (c4Index + 1)
                        << strmfString << L". " << m_colValue[c4Index] << kCIDLib::NewLn;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngStrListVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);

    const TMEngStrListVal& mecvSrc = static_cast<const TMEngStrListVal&>(mecvToCopy);
    m_bUnique = mecvSrc.m_bUnique;
    m_colValue = mecvSrc.m_colValue;
}


// ---------------------------------------------------------------------------
//  TMEngStrListVal: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngStrListVal::bCheckUnique(const TString& strToCheck)
{
    const tCIDLib::TCard4 c4Count = m_colValue.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colValue[c4Index] == strToCheck)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrListInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStrListInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStrListInfo::strPath()
{
    return CIDMacroEng_StringClasses::strStrListClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngStrListInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStrListInfo::TMEngStrListInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_StringClasses::strStrList
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_Append(kMacroEng::c2BadId)
    , m_c2MethId_AppendFrom(kMacroEng::c2BadId)
    , m_c2MethId_CopyFrom(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_FindString(kMacroEng::c2BadId)
    , m_c2MethId_GetAt(kMacroEng::c2BadId)
    , m_c2MethId_GetElemCount(kMacroEng::c2BadId)
    , m_c2MethId_GetUnique(kMacroEng::c2BadId)
    , m_c2MethId_InsertAt(kMacroEng::c2BadId)
    , m_c2MethId_IsEmpty(kMacroEng::c2BadId)
    , m_c2MethId_RemoveAll(kMacroEng::c2BadId)
    , m_c2MethId_RemoveAt(kMacroEng::c2BadId)
    , m_c2MethId_SetAt(kMacroEng::c2BadId)
    , m_c2MethId_StealFrom(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
{
}

TMEngStrListInfo::~TMEngStrListInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStrListInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStrListInfo::Init(TCIDMacroEngine& meOwner)
{
    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"StrListErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 2
        );
        m_c4ErrBadInd = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"Index %(1) is invalid for this string list. Count=%(2)");
        m_c4ErrAlreadyUsed = m_pmeciErrors->c4AddEnumItem(L"AlreadyUsed", L"A duplicate string was added to the list");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Append a string to this list
    {
        TMEngMethodInfo methiNew
        (
            L"Append"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Append = c2AddMethodInfo(methiNew);
    }

    // Append all the strings from the src list to the end of ours
    {
        TMEngMethodInfo methiNew
        (
            L"AppendFrom"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcList", c2Id());
        m_c2MethId_AppendFrom = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.StringList"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Replace all our contents with copies of the strings from the src
    {
        TMEngMethodInfo methiNew
        (
            L"CopyFrom"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcList", c2Id());
        m_c2MethId_CopyFrom = c2AddMethodInfo(methiNew);
    }

    // Find a particular string and give back the index
    {
        TMEngMethodInfo methiNew
        (
            L"FindString"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_FindString = c2AddMethodInfo(methiNew);
    }

    // Get a string out at the given index
    {
        TMEngMethodInfo methiNew
        (
            L"GetAt"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetAt = c2AddMethodInfo(methiNew);
    }

    // Get the count of strings in the list
    {
        TMEngMethodInfo methiNew
        (
            L"GetElemCount"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetElemCount = c2AddMethodInfo(methiNew);
    }

    // Get the uniqueness attribute
    {
        TMEngMethodInfo methiNew
        (
            L"GetUnique"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetUnique = c2AddMethodInfo(methiNew);
    }

    // Insert a new string at the given index
    {
        TMEngMethodInfo methiNew
        (
            L"InsertAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToInsert", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_InsertAt = c2AddMethodInfo(methiNew);
    }

    // Return if we are empty or not
    {
        TMEngMethodInfo methiNew
        (
            L"IsEmpty"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_IsEmpty = c2AddMethodInfo(methiNew);
    }

    // Remove all strings from the list
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveAll"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_RemoveAll = c2AddMethodInfo(methiNew);
    }

    // Remove string at given index
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_RemoveAt = c2AddMethodInfo(methiNew);
    }

    // Set a a new value for the string at the given index
    {
        TMEngMethodInfo methiNew
        (
            L"SetAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NewValue", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetAt = c2AddMethodInfo(methiNew);
    }

    // Steal all the strings from the src, leaving it empty
    {
        TMEngMethodInfo methiNew
        (
            L"StealFrom"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcList", c2Id());
        m_c2MethId_StealFrom = c2AddMethodInfo(methiNew);
    }

    // Add a ctor that takes a unique flag
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.StringList"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"UniqueOnly", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngStrListInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngStrListVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngStrListInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStrListInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngStrListVal& mecvActual = static_cast<TMEngStrListVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Append)
    {
        //
        //  Add the passed string. If it shows up as rejected due to
        //  uniqueness constraints, throw an exception.
        //
        if (!mecvActual.bAppend(meOwner.strStackValAt(c4FirstInd)))
            ThrowAnErr(meOwner, m_c4ErrAlreadyUsed);
    }
     else if ((c2MethId == m_c2MethId_AppendFrom)
          ||  (c2MethId == m_c2MethId_CopyFrom)
          ||  (c2MethId == m_c2MethId_StealFrom))
    {
        TMEngStrListVal::EFromTypes eType;
        if (c2MethId == m_c2MethId_AppendFrom)
            eType = TMEngStrListVal::EFromTypes::Append;
        else if (c2MethId == m_c2MethId_CopyFrom)
            eType = TMEngStrListVal::EFromTypes::Copy;
        else
            eType = TMEngStrListVal::EFromTypes::Steal;

        // These are all so simple that we handle them together
        TMEngStrListVal& mecvSrc(meOwner.mecvStackAtAs<TMEngStrListVal>(c4FirstInd));
        if (!mecvActual.bFromSource(mecvSrc, eType))
            ThrowAnErr(meOwner, m_c4ErrAlreadyUsed);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Reset it wtih the unique flag set
        mecvActual.Reset(kCIDLib::True);
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);

        // Display the strings
        const tCIDLib::TCard4 c4Count = mecvActual.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            mecvTarget.strmTarget(meOwner)  << mecvActual.strAt(c4Index)
                                            << kCIDLib::NewLn;
        }
    }
     else if (c2MethId == m_c2MethId_FindString)
    {
        //
        //  Get the string parameter that we are to find and the boolean
        //  return value.
        //
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TString& strToFind = meOwner.strStackValAt(c4FirstInd);
        TMEngCard4Val& mecvInd = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);

        // Try to find the match
        tCIDLib::TCard4 c4Index = mecvActual.c4FindString(strToFind);;

        if (c4Index != kCIDLib::c4MaxCard)
        {
            mecvInd.c4Value(c4Index);
            mecvRet.bValue(kCIDLib::True);
        }
         else
        {
            mecvInd.c4Value(kCIDLib::c4MaxCard);
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_GetAt)
    {
        // Get the index parm and check it. If bad, throw
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = mecvActual.c4ElemCount();
        if (c4Index >= c4Count)
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index), TCardinal(c4Count));

        // Get the string return value and fill it in
        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strAt(c4Index));
    }
     else if (c2MethId == m_c2MethId_GetElemCount)
    {
        // Get the card return value and fill it in
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4ElemCount());
    }
     else if (c2MethId == m_c2MethId_GetUnique)
    {
        // Get the boolean return value and fill it in
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bUnique());
    }
     else if (c2MethId == m_c2MethId_InsertAt)
    {
        // Get the index parm and check it. If bad, throw
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = mecvActual.c4ElemCount();
        if (c4Index > c4Count)
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index), TCardinal(c4Count));

        // Insert the second parm. If it fails due to uniqueness issues, throw
        if (!mecvActual.bInsertAt(c4Index, meOwner.strStackValAt(c4FirstInd + 1)))
            ThrowAnErr(meOwner, m_c4ErrAlreadyUsed);
    }
     else if (c2MethId == m_c2MethId_IsEmpty)
    {
        // Just set the return boolean to true if we have no values
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.c4ElemCount() == 0);
    }
     else if (c2MethId == m_c2MethId_RemoveAll)
    {
        mecvActual.RemoveAll();
    }
     else if (c2MethId == m_c2MethId_RemoveAt)
    {
        // Get the index parm and check it. If bad, throw
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = mecvActual.c4ElemCount();
        if (c4Index >= c4Count)
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index), TCardinal(c4Count));

        // Looks ok so remove it
        mecvActual.RemoveAt(c4Index);
    }
     else if (c2MethId == m_c2MethId_SetAt)
    {
        // Get the index parm and check it. If bad, throw
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = mecvActual.c4ElemCount();
        if (c4Index >= c4Count)
            ThrowAnErr(meOwner, m_c4ErrBadInd, TCardinal(c4Index), TCardinal(c4Count));

        // Set with the second parm. If it fails due to uniqueness issues, throw
        if (!mecvActual.bSetAt(c4Index, meOwner.strStackValAt(c4FirstInd + 1)))
            ThrowAnErr(meOwner, m_c4ErrAlreadyUsed);
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        // Reset it wtih passed unique flag
        mecvActual.Reset(meOwner.bStackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngStrListInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStrListInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngStrListInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngTextXCoderVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextXCoderVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTextXCoderVal::TMEngTextXCoderVal( const   TString&                strName
                                        , const tCIDLib::TCard2         c2Id
                                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_ptcvtValue(new TUSASCIIConverter)
{
}

TMEngTextXCoderVal::~TMEngTextXCoderVal()
{
    delete m_ptcvtValue;
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngTextXCoderVal::bSetEncoding(const TString& strToSet)
{
    // See if we can create an encoder for this new guy
    TTextConverter* ptcvtNew = facCIDEncode().ptcvtMakeNew(strToSet);
    if (!ptcvtNew)
        return kCIDLib::False;
    TJanitor<TTextConverter> janConverter(ptcvtNew);

    // Looks ok, so delete the old one and set the new one
    delete m_ptcvtValue;
    m_ptcvtValue = janConverter.pobjOrphan();
    return kCIDLib::True;
}


const TTextConverter& TMEngTextXCoderVal::tcvtValue() const
{
    return *m_ptcvtValue;
}

TTextConverter& TMEngTextXCoderVal::tcvtValue()
{
    return *m_ptcvtValue;
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTextXCoderVal::bDbgFormat(         TTextOutStream&     strmTarget
                                , const TMEngClassInfo&
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices
                                , const TCIDMacroEngine&) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Encoding: " << m_ptcvtValue->strEncodingName();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngTextXCoderInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextXCoderInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngTextXCoderInfo::strPath()
{
    return CIDMacroEng_StringClasses::strXCoderClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTextXCoderInfo::TMEngTextXCoderInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_StringClasses::strXCoder
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_ConvertFrom(kMacroEng::c2BadId)
    , m_c2MethId_ConvertFromRange(kMacroEng::c2BadId)
    , m_c2MethId_ConvertTo(kMacroEng::c2BadId)
    , m_c2MethId_GetEncoding(kMacroEng::c2BadId)
    , m_c2MethId_SetEncoding(kMacroEng::c2BadId)
    , m_c2MethId_SetErrAction(kMacroEng::c2BadId)
    , m_c2MethId_SetRepChar(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2TypeId_CvtErrActs(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_pmeciCvtErrActs(0)
    , m_pmeciErrors(0)
{
}

TMEngTextXCoderInfo::~TMEngTextXCoderInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngTextXCoderInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the input conversion error actions
    {
        m_pmeciCvtErrActs = new TMEngEnumInfo
        (
            meOwner
            , L"CvtErrActs"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_pmeciCvtErrActs->c4AddEnumItem(L"Throw", L"Throw on error", tCIDLib::ETCvtActions::Throw);
        m_pmeciCvtErrActs->c4AddEnumItem(L"StopThrow", L"Stop first, then throw", tCIDLib::ETCvtActions::StopThenThrow);
        m_pmeciCvtErrActs->c4AddEnumItem(L"RepChar", L"Use replacement character", tCIDLib::ETCvtActions::Replace);
        m_pmeciCvtErrActs->BaseClassInit(meOwner);
        m_c2TypeId_CvtErrActs = meOwner.c2AddClass(m_pmeciCvtErrActs);
        bAddNestedType(m_pmeciCvtErrActs->strClassPath());
    }

    //
    //  We need to throw some exceptions, so we need to create a nested
    //  enum for our errors.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"XCoderErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        m_c4ErrBadEncoding = m_pmeciErrors->c4AddEnumItem(L"BadEncoding", L"Text encoding '%(1)' is not supported");
        m_c4ErrBadSrc = m_pmeciErrors->c4AddEnumItem(L"BadSrcData", L"The source data was badly formed for this encoding");
        m_c4ErrBadSrcRange = m_pmeciErrors->c4AddEnumItem(L"BadSrcRange", L"The source index/count %(1)/%(2) is beyond the buffer allocation");
        m_c4ErrBadCharCount = m_pmeciErrors->c4AddEnumItem(L"BadCharCount", L"The requested source count is %(1) but the string length is %(2)");
        m_c4ErrBufSz = m_pmeciErrors->c4AddEnumItem(L"BufferSz", L"The target buffer is too small (Max=%(1))to hold the resulting %(2) bytes");
        m_c4ErrConvertFrom = m_pmeciErrors->c4AddEnumItem(L"ConvertFromErr", TString::strEmpty());
        m_c4ErrConvertTo = m_pmeciErrors->c4AddEnumItem(L"ConvertToErr", TString::strEmpty());
        m_c4ErrUnRep = m_pmeciErrors->c4AddEnumItem(L"Unrep", L"The source contains unrepresentable chars");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.TextXCoder"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an encoding name
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.TextXCoder"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"EncodingName", tCIDMacroEng::EIntrinsics::String);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }

    // Convert from the encoding to internal format
    {
        TMEngMethodInfo methiNew
        (
            L"ConvertFrom"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Buffer", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"SrcBytes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ConvertFrom = c2AddMethodInfo(methiNew);
    }

    // Same, but from a range within the source buffer
    {
        TMEngMethodInfo methiNew
        (
            L"ConvertFromRange"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Buffer", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ConvertFromRange = c2AddMethodInfo(methiNew);
    }

    // Convert from internal format to encoding
    {
        TMEngMethodInfo methiNew
        (
            L"ConvertTo"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToConvert", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Buffer", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"BytesWritten", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ConvertTo = c2AddMethodInfo(methiNew);
    }

    // Same but not all src chars
    {
        TMEngMethodInfo methiNew
        (
            L"ConvertNTo"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToConvert", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Buffer", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"BytesWritten", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ConvertNTo = c2AddMethodInfo(methiNew);
    }

    // Get the encoding after contruction
    {
        TMEngMethodInfo methiNew
        (
            L"GetEncoding"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetEncoding = c2AddMethodInfo(methiNew);
    }

    // Set the encoding after contruction
    {
        TMEngMethodInfo methiNew
        (
            L"SetEncoding"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"EncodingName", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetEncoding = c2AddMethodInfo(methiNew);
    }

    // Set the error action
    {
        TMEngMethodInfo methiNew
        (
            L"SetErrorAction"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ActToSet", m_c2TypeId_CvtErrActs);
        m_c2MethId_SetErrAction = c2AddMethodInfo(methiNew);
    }

    // Set the replacement character
    {
        TMEngMethodInfo methiNew
        (
            L"SetRepChar"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_SetRepChar = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngTextXCoderInfo::pmecvMakeStorage(  const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngTextXCoderVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTextXCoderInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngTextXCoderVal& mecvActual = static_cast<TMEngTextXCoderVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        mecvActual.bSetEncoding(L"USASCII");
    }
     else if (c2MethId == m_c2MethId_ConvertFrom)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const TMemBuf& mbufSrc = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue();
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);

        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4Count > mbufSrc.c4Size())
            ThrowAnErr(meOwner, m_c4ErrBadSrcRange, TInteger(0), TCardinal(c4Count));

        tCIDLib::TCard4 c4BytesEaten = 0;
        try
        {
            c4BytesEaten = mecvActual.tcvtValue().c4ConvertFrom
            (
                mbufSrc.pc1Data()
                , c4Count
                , mecvToFill.strValue()
            );
        }

        catch(const TError& errToCatch)
        {
            if (errToCatch.bCheckEvent( facCIDLib().strName()
                                        , kCIDErrs::errcTCvt_BadSource))
            {
                ThrowAnErr(meOwner, m_c4ErrBadSrc);
            }
             else
            {
                ThrowAnErr(meOwner, m_c4ErrConvertFrom, errToCatch.strErrText());
            }
        }

        // Return the bytes we ate
        mecvRet.c4Value(c4BytesEaten);
    }
     else if (c2MethId == m_c2MethId_ConvertFromRange)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const TMemBuf& mbufSrc = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue();
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3);

        const tCIDLib::TCard4 c4StartInd = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 2);
        if (c4StartInd + c4Count >= mbufSrc.c4Size())
            ThrowAnErr(meOwner, m_c4ErrBadSrcRange, TCardinal(c4StartInd), TCardinal(c4Count));

        tCIDLib::TCard4 c4BytesEaten = kCIDLib::c4MaxCard;
        try
        {
            c4BytesEaten = mecvActual.tcvtValue().c4ConvertFrom
            (
                mbufSrc.pc1Data() + c4StartInd
                , c4Count
                , mecvToFill.strValue()
            );
        }

        catch(const TError& errToCatch)
        {
            if (errToCatch.bCheckEvent( facCIDLib().strName()
                                        , kCIDErrs::errcTCvt_BadSource))
            {
                ThrowAnErr(meOwner, m_c4ErrBadSrc);
            }
             else
            {
                ThrowAnErr(meOwner, m_c4ErrConvertFrom, errToCatch.strErrText());
            }
        }

        // Return the bytes we ate
        mecvRet.c4Value(c4BytesEaten);
    }
     else if (c2MethId == m_c2MethId_ConvertTo)
    {
        const TString& strSrc = meOwner.strStackValAt(c4FirstInd);

        const tCIDLib::TCard4 c4MaxBytes = strSrc.c4Length() * 6;
        TExpByteBuf expbToFill(c4MaxBytes);
        tCIDLib::TCard4 c4Chars = 0;
        try
        {
            c4Chars = mecvActual.tcvtValue().c4ConvertTo
            (
                strSrc.pszBuffer(), strSrc.c4Length(), expbToFill
            );
        }

        catch(const TError& errToCatch)
        {
            if (errToCatch.bCheckEvent( facCIDLib().strName()
                                        , kCIDErrs::errcTCvt_Unrepresentable))
            {
                ThrowAnErr(meOwner, m_c4ErrUnRep);
            }
             else
            {
                ThrowAnErr(meOwner, m_c4ErrConvertTo, errToCatch.strErrText());
            }
        }

        // It worked so fill in the output parm and return value
        TMEngCard4Val& mecvBytesEaten = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2);
        mecvBytesEaten.c4Value(expbToFill.c4Bytes());

        TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);
        if (expbToFill.c4Bytes() > mecvToFill.mbufValue().c4MaxSize())
            ThrowAnErr(meOwner, m_c4ErrBufSz, TCardinal(mecvToFill.mbufValue().c4MaxSize()), TCardinal(expbToFill.c4Bytes()));
        mecvToFill.mbufValue().CopyIn(expbToFill.pc1Buffer(), expbToFill.c4Bytes());

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Chars);
    }
     else if (c2MethId == m_c2MethId_ConvertNTo)
    {
        const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4Count > strSrc.c4Length())
            ThrowAnErr(meOwner, m_c4ErrBadCharCount, TCardinal(c4Count), TCardinal(strSrc.c4Length()));

        const tCIDLib::TCard4 c4MaxBytes = c4Count * 6;
        TExpByteBuf expbToFill(c4MaxBytes);
        tCIDLib::TCard4 c4Chars = 0;
        try
        {
            c4Chars = mecvActual.tcvtValue().c4ConvertTo
            (
                strSrc.pszBuffer(), c4Count, expbToFill
            );
        }

        catch(const TError& errToCatch)
        {
            if (errToCatch.bCheckEvent( facCIDLib().strName()
                                        , kCIDErrs::errcTCvt_Unrepresentable))
            {
                ThrowAnErr(meOwner, m_c4ErrUnRep);
            }
             else
            {
                ThrowAnErr(meOwner, m_c4ErrConvertTo, errToCatch.strErrText());
            }
        }

        // It worked so fill in the output parm and return value
        TMEngCard4Val& mecvBytesEaten = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 3);
        mecvBytesEaten.c4Value(expbToFill.c4Bytes());

        TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 2);
        if (expbToFill.c4Bytes() > mecvToFill.mbufValue().c4MaxSize())
            ThrowAnErr(meOwner, m_c4ErrBufSz, TCardinal(mecvToFill.mbufValue().c4MaxSize()), TCardinal(expbToFill.c4Bytes()));
        mecvToFill.mbufValue().CopyIn(expbToFill.pc1Buffer(), expbToFill.c4Bytes());

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Chars);
    }
     else if (c2MethId == m_c2MethId_GetEncoding)
    {
        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.tcvtValue().strEncodingName());
    }
     else if (c2MethId == m_c2MethId_SetEncoding)
    {
        const TString& strToSet = meOwner.strStackValAt(c4FirstInd);
        if (!mecvActual.bSetEncoding(strToSet))
            ThrowAnErr(meOwner, m_c4ErrBadEncoding, strToSet);
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetErrAction)
    {
        TMEngEnumVal& mecvAct = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        mecvActual.tcvtValue().eErrorAction
        (
            tCIDLib::ETCvtActions(m_pmeciCvtErrActs->c4MapValue(mecvAct))
        );
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetRepChar)
    {
        mecvActual.tcvtValue().chRepChar(meOwner.chStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        const TString& strToSet = meOwner.strStackValAt(c4FirstInd);
        if (!mecvActual.bSetEncoding(strToSet))
            ThrowAnErr(meOwner, m_c4ErrBadEncoding, strToSet);
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngTextXCoderInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngTextXCoderInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngTextXCoderInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngTextXCoderInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngTextXCoderInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken1
                                , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


