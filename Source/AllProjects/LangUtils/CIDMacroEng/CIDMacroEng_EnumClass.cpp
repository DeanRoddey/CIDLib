//
// FILE NAME: CIDMacroEng_EnumClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2003
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
//  value classes, which implement the fundamental MEng.Enum class.
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
RTTIDecls(TMEngEnumVal,TMEngClassVal)
RTTIDecls(TMEngEnumInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//   CLASS: TMEngClassInfo::Item
//  PREFIX: item
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngClassInfo::Item: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngEnumInfo::TItem::TItem() :

    m_c4MapVal(0)
{
}

TMEngEnumInfo::TItem::TItem(const   TString&        strName
                            , const TString&        strText
                            , const tCIDLib::TCard4 c4MapVal) :
    m_c4MapVal(c4MapVal)
    , m_strName(strName)
    , m_strText(strText)
{
}

TMEngEnumInfo::TItem::TItem(const TItem& itemSrc) :

    m_c4MapVal(itemSrc.m_c4MapVal)
    , m_strName(itemSrc.m_strName)
    , m_strText(itemSrc.m_strText)
{
}

TMEngEnumInfo::TItem::~TItem()
{
}


// ---------------------------------------------------------------------------
//  TMEngEnumInfo::TItem::: Public operators
// ---------------------------------------------------------------------------
TMEngEnumInfo::TItem& TMEngEnumInfo::TItem::operator=(const TItem& itemToAssign)
{
    if (this != &itemToAssign)
    {
        m_c4MapVal  = itemToAssign.m_c4MapVal;
        m_strName   = itemToAssign.m_strName;
        m_strText   = itemToAssign.m_strText;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngEnumVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngEnumVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngEnumVal::TMEngEnumVal( const   TString&                strName
                            , const tCIDLib::TCard2         c2ClassId
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCard4         c4MaxOrdinal
                            , const tCIDLib::TCard4         c4InitOrdinal) :

    TMEngClassVal(strName, c2ClassId, eConst)
    , m_c4MaxOrdValue(c4MaxOrdinal)
    , m_c4OrdValue(c4InitOrdinal)
{
    // If debugging, validate that the init is legal
    #if CID_DEBUG_ON
    if (c4InitOrdinal > c4MaxOrdinal)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadEnumInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
            , TCardinal(c4InitOrdinal)
            , strName
            , TCardinal(c4MaxOrdinal)
        );
    }
    #endif
}

TMEngEnumVal::~TMEngEnumVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngEnumVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngEnumVal::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&     meciThis
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices
                        , const TCIDMacroEngine&    ) const
{
    const TMEngEnumInfo& meciActual = static_cast<const TMEngEnumInfo&>(meciThis);

    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        strmTarget << meciActual.strItemName(*this);
    }
     else
    {
        strmTarget  << L"Name=" << meciActual.strItemName(*this)
                    << L"\nText = " << meciActual.strTextValue(*this);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngEnumVal::bParseFromText(const  TString&        strValue
                            , const TMEngClassInfo& mecvThis)
{
    // See if the enum we represent has a value with the given name
    const tCIDLib::TCard4 c4Ord
    (
        static_cast<const TMEngEnumInfo&>(mecvThis).c4FindOrdinal(strValue)
    );

    // If max card, we have no such value name, so fail
    if (c4Ord == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    // We found it, so store the new ordinal and return success
    m_c4OrdValue = c4Ord;
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngEnumVal::CopyFrom( const   TMEngClassVal&      mecvSrc
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvSrc);

    // We don't have to copy the max value, which is fixed for this class
    m_c4OrdValue = static_cast<const TMEngEnumVal&>(mecvSrc).m_c4OrdValue;
}


// ---------------------------------------------------------------------------
//  TMEngEnumVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngEnumVal::bAtMax() const
{
    return (m_c4OrdValue == m_c4MaxOrdValue);
}


tCIDLib::TBoolean TMEngEnumVal::bAtMin() const
{
    return (m_c4OrdValue == 0);
}


tCIDLib::TCard4 TMEngEnumVal::c4Ordinal() const
{
    return m_c4OrdValue;
}

tCIDLib::TCard4 TMEngEnumVal::c4Ordinal(const tCIDLib::TCard4 c4ToSet)
{
    if (c4ToSet > m_c4MaxOrdValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadEnumSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
            , strName()
            , TCardinal(c4ToSet)
            , TCardinal(m_c4MaxOrdValue)
        );
    }

    m_c4OrdValue = c4ToSet;
    return m_c4OrdValue;
}


tCIDLib::TCard4 TMEngEnumVal::c4MaxOrdinal() const
{
    return m_c4MaxOrdValue;
}


tCIDLib::TVoid TMEngEnumVal::Decrement()
{
    if (m_c4OrdValue)
        m_c4OrdValue--;
}


tCIDLib::TVoid TMEngEnumVal::Increment()
{
    if (m_c4OrdValue < m_c4MaxOrdValue)
        m_c4OrdValue++;
}


tCIDLib::TVoid TMEngEnumVal::SetToMax()
{
    m_c4OrdValue = m_c4MaxOrdValue;
}


tCIDLib::TVoid TMEngEnumVal::SetToMin()
{
    m_c4OrdValue = 0;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngEnumInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngEnumInfo::TMEngEnumInfo(       TCIDMacroEngine&    meOwner
                            , const TString&            strName
                            , const TString&            strBasePath
                            , const TString&            strParentClassPath
                            , const tCIDLib::TCard4     c4ExpectedItems) :
    TMEngClassInfo
    (
        strName
        , strBasePath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Abstract
        , strParentClassPath
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_AtMax(kMacroEng::c2BadId)
    , m_c2MethId_AtMin(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_FromName(kMacroEng::c2BadId)
    , m_c2MethId_FromText(kMacroEng::c2BadId)
    , m_c2MethId_FormatList(kMacroEng::c2BadId)
    , m_c2MethId_GetOrd(kMacroEng::c2BadId)
    , m_c2MethId_GetOrdCount(kMacroEng::c2BadId)
    , m_c2MethId_GetName(kMacroEng::c2BadId)
    , m_c2MethId_GetText(kMacroEng::c2BadId)
    , m_c2MethId_GreaterThan(kMacroEng::c2BadId)
    , m_c2MethId_GreaterThanEq(kMacroEng::c2BadId)
    , m_c2MethId_Inc(kMacroEng::c2BadId)
    , m_c2MethId_LessThan(kMacroEng::c2BadId)
    , m_c2MethId_MapFromText(kMacroEng::c2BadId)
    , m_c2MethId_LessThanEq(kMacroEng::c2BadId)
    , m_c2MethId_SetMin(kMacroEng::c2BadId)
    , m_c2MethId_SetMax(kMacroEng::c2BadId)
    , m_c2MethId_SetOrd(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_colItems(c4ExpectedItems ? c4ExpectedItems : 8)
    , m_c4ErrBadOrdinal(kCIDLib::c4MaxCard)
    , m_c4ErrNameNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrTextNotFound(kCIDLib::c4MaxCard)
    , m_pmeciErrors(nullptr)
{
    if (strClassPath() != L"MEng.Enum")
        eExtend(tCIDMacroEng::EClassExt::Final);
}

TMEngEnumInfo::~TMEngEnumInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngEnumInfo::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Int1 :
        case tCIDMacroEng::EIntrinsics::Int2 :
        case tCIDMacroEng::EIntrinsics::Int4 :
            break;

        default :
            return kCIDLib::False;
    };
    return kCIDLib::True;
}


tCIDMacroEng::ECastRes
TMEngEnumInfo::eCastFrom(           TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TCard4 c4New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Card1 :
            c4New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            c4New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            c4New = static_cast<const TMEngCard4Val&>(mecvSrc).c4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt1Val&>(mecvSrc).i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngEnumVal& mecvActual = static_cast<TMEngEnumVal&>(mecvTarget);

    // See what the max ordinal value is
    const tCIDLib::TCard4 c4MaxOrd = m_colItems.c4ElemCount() - 1;

    // If it won't fit, then return a range indicator
    if (c4New > c4MaxOrd)
        return tCIDMacroEng::ECastRes::Range;

    // Looks ok so set the ordinal on the target
    mecvActual.c4Ordinal(c4New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngEnumInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  If this is the base enum class, then store the id globally so that
    //  we can provide access to this id statically (since the parser will
    //  often want to see whether a class is an enum derivative.
    //
    //  We also add different methods according to the macro enum class being
    //  created. For the base class, we can provide the methods that don't
    //  care about the specific type of the enum. Otherwise, we generate those
    //  methods into the derived classes.
    //
    //  NOTE:   Only those methods that deal purely with the ordinal value
    //          can be handled by the base MEng.Enum instance of this class,
    //          since that's all it knows about. Though, at the C++ level,
    //          this case class holds the items, at the macro level, separate
    //          instances of this class hold each individual enum instance
    //          generated.
    //
    const tCIDLib::TCard2 c2OurId = c2Id();
    if (strClassPath() == L"MEng.Enum")
    {
        // Add the default constructor for the base class
        {
            TMEngMethodInfo methiNew
            (
                L"ctor1_MEng.Enum"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.bIsCtor(kCIDLib::True);
            m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
        }

        // Decrement the value
        {
            TMEngMethodInfo methiNew
            (
                L"Dec"
                , c2OurId
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
            );
            m_c2MethId_Dec = c2AddMethodInfo(methiNew);
        }

        // Check if it's at the max value
        {
            TMEngMethodInfo methiNew
            (
                L"IsAtMax"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_AtMax = c2AddMethodInfo(methiNew);
        }

        // Check if it's at the min value
        {
            TMEngMethodInfo methiNew
            (
                L"IsAtMin"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_AtMin = c2AddMethodInfo(methiNew);
        }

        // Get it via ordinal
        {
            TMEngMethodInfo methiNew
            (
                L"GetOrdinal"
                , tCIDMacroEng::EIntrinsics::Card4
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_GetOrd = c2AddMethodInfo(methiNew);
        }

        // Increment the value
        {
            TMEngMethodInfo methiNew
            (
                L"Inc"
                , c2OurId
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
            );
            m_c2MethId_Inc = c2AddMethodInfo(methiNew);
        }

        // Set to the maximum value
        {
            TMEngMethodInfo methiNew
            (
                L"SetToMax"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            m_c2MethId_SetMax = c2AddMethodInfo(methiNew);
        }

        // Set to the minimum value
        {
            TMEngMethodInfo methiNew
            (
                L"SetToMin"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            m_c2MethId_SetMin = c2AddMethodInfo(methiNew);
        }

        // We allow it to be set via ordinal value
        {
            TMEngMethodInfo methiNew
            (
                L"SetOrdinal"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_SetOrd = c2AddMethodInfo(methiNew);
        }


        //
        //  These are all required methods that speicifc enum types will
        //  derive from. This allows us to get/set the name and text stuff
        //  via the base class.
        //
        {
            TMEngMethodInfo methiNew
            (
                L"FromName"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
                , tCIDMacroEng::EConstTypes::NonConst
            );
            methiNew.c2AddInParm(L"NameToFind", tCIDMacroEng::EIntrinsics::String);
            m_c2MethId_FromName = c2AddMethodInfo(methiNew);
        }

        {
            TMEngMethodInfo methiNew
            (
                L"FromText"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
                , tCIDMacroEng::EConstTypes::NonConst
            );
            methiNew.c2AddInParm(L"TextToFind", tCIDMacroEng::EIntrinsics::String);
            m_c2MethId_FromText = c2AddMethodInfo(methiNew);
        }

        // Format out the names or values
        {
            TMEngMethodInfo methiNew
            (
                L"FormatList"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
            methiNew.c2AddInParm(L"Prefix", tCIDMacroEng::EIntrinsics::String);
            methiNew.c2AddInParm(L"SepChar", tCIDMacroEng::EIntrinsics::Char);
            methiNew.c2AddInParm(L"UseName", tCIDMacroEng::EIntrinsics::Boolean);
            m_c2MethId_FormatList = c2AddMethodInfo(methiNew);
        }

        // Return the name of the current value of this enum
        {
            TMEngMethodInfo methiNew
            (
                L"GetName"
                , tCIDMacroEng::EIntrinsics::String
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_GetName = c2AddMethodInfo(methiNew);
        }

        // Return the text of the current value of this enum
        {
            TMEngMethodInfo methiNew
            (
                L"GetText"
                , tCIDMacroEng::EIntrinsics::String
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Required
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_GetText = c2AddMethodInfo(methiNew);
        }

        //
        //  And now we have to play a trick, since exceptions are enums,
        //  and we need to throw some, we have to create an enumerated
        //  macro type as a nested type of ours.
        //
        {
            m_pmeciErrors = new TMEngEnumInfo
            (
                meOwner
                , L"EnumErrors"
                , strClassPath()
                , L"MEng.Enum"
                , 3
            );
            m_c4ErrBadOrdinal = m_pmeciErrors->c4AddEnumItem(L"BadOrdinal", L"%(1) is not a valid ordinal for enum %(2)", EBaseErrs::BadOrdinal);
            m_c4ErrNameNotFound = m_pmeciErrors->c4AddEnumItem(L"NameNotFound", L"%(1) is not a valid item name for enum %(2)", EBaseErrs::NameNotFound);
            m_c4ErrTextNotFound = m_pmeciErrors->c4AddEnumItem(L"TextNotFound", L"The passed text did not match any value for enum %(1)", EBaseErrs::TextNotFound);
            m_pmeciErrors->BaseClassInit(meOwner);
            m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
            bAddNestedType(m_pmeciErrors->strClassPath());
        }
    }
     else
    {
        //
        //  Get the class path, since we need it to create the constructor
        //  names in this case.
        //
        const TString& strCPath = strClassPath();

        // Add the default constructor for the derived class
        {
            TMEngMethodInfo methiNew
            (
                TString(L"ctor1_") + strCPath
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.bIsCtor(kCIDLib::True);
            m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
        }

        // Add the constructor that takes an initial value
        {
            TMEngMethodInfo methiNew
            (
                TString(L"ctor2_") + strCPath
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"InitVal", c2OurId);
            methiNew.bIsCtor(kCIDLib::True);
            m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
        }

        // Equality test
        {
            TMEngMethodInfo methiNew
            (
                L"Equal"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddInParm(L"ToCompare", c2OurId);
            m_c2MethId_Equal = c2AddMethodInfo(methiNew);
        }

        // Get the count of ordinal values
        {
            TMEngMethodInfo methiNew
            (
                L"GetOrdinalCount"
                , tCIDMacroEng::EIntrinsics::Card4
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_GetOrdCount = c2AddMethodInfo(methiNew);
        }

        // Greater than comparison
        {
            TMEngMethodInfo methiNew
            (
                L"GtThan"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddInParm(L"ToCompare", c2OurId);
            m_c2MethId_GreaterThan = c2AddMethodInfo(methiNew);
        }

        // Greater or equal than comparison
        {
            TMEngMethodInfo methiNew
            (
                L"GtThanEq"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddInParm(L"ToCompare", c2OurId);
            m_c2MethId_GreaterThanEq = c2AddMethodInfo(methiNew);
        }

        // Less than comparison
        {
            TMEngMethodInfo methiNew
            (
                L"LsThan"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddInParm(L"ToCompare", c2OurId);
            m_c2MethId_LessThan = c2AddMethodInfo(methiNew);
        }

        // Less than or equal comparison
        {
            TMEngMethodInfo methiNew
            (
                L"LsThanEq"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            methiNew.c2AddInParm(L"ToCompare", c2OurId);
            m_c2MethId_LessThanEq = c2AddMethodInfo(methiNew);
        }

        //
        //  Map from text, as either name or text, in a non-exception
        //  throwing way. If no match, it returns the default value.
        //
        {
            TMEngMethodInfo methiNew
            (
                L"MapFromText"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::NonConst
            );
            methiNew.c2AddInParm(L"TextVal", tCIDMacroEng::EIntrinsics::String);
            methiNew.c2AddInParm(L"IsName", tCIDMacroEng::EIntrinsics::Boolean);
            methiNew.c2AddInParm(L"DefVal", c2OurId);
            m_c2MethId_MapFromText = c2AddMethodInfo(methiNew);
        }


        //
        //  This next bunch are overrides, so we can't just add them,
        //  we have to look them up and update their method extension
        //  type to final for our copies of them.
        //
        TMEngMethodInfo* pmethiUpdate;

        pmethiUpdate = pmethiFind(L"FromName");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        m_c2MethId_FromName = pmethiUpdate->c2Id();

        pmethiUpdate = pmethiFind(L"FormatList");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        m_c2MethId_FormatList = pmethiUpdate->c2Id();

        pmethiUpdate = pmethiFind(L"FromText");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        m_c2MethId_FromText = pmethiUpdate->c2Id();

        pmethiUpdate = pmethiFind(L"GetName");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        m_c2MethId_GetName = pmethiUpdate->c2Id();

        pmethiUpdate = pmethiFind(L"GetText");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        m_c2MethId_GetText = pmethiUpdate->c2Id();

        // For these, we also have to update the return type
        pmethiUpdate = pmethiFind(L"Dec");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        pmethiUpdate->c2RetClassId(c2OurId);
        m_c2MethId_Dec = pmethiUpdate->c2Id();

        pmethiUpdate = pmethiFind(L"Inc");
        pmethiUpdate->eExtend(tCIDMacroEng::EMethExt::Final);
        pmethiUpdate->c2RetClassId(c2OurId);
        m_c2MethId_Inc = pmethiUpdate->c2Id();
    }
}


TMEngClassVal*
TMEngEnumInfo::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngEnumVal
    (
        strName, c2Id(), eConst, m_colItems.c4ElemCount() - 1, 0
    );
}


// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngEnumInfo::bItemExists(const TString& strName) const
{
    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colItems[c4Index].m_strName == strName)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TMEngEnumInfo::c4FindOrdinal(const TString& strName) const
{
    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colItems[c4Index].m_strName == strName)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


tCIDLib::TCard4 TMEngEnumInfo::c4MapValue(const TMEngEnumVal& mecvValue) const
{
    return m_colItems[mecvValue.c4Ordinal()].m_c4MapVal;
}


tCIDLib::TCard4 TMEngEnumInfo::c4OrdinalFromText(const TString& strText) const
{
    // Search for the item with this map value
    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        if (m_colItems[c4Index].m_strText == strText)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


tCIDLib::TCard4 TMEngEnumInfo::c4ValueCount() const
{
    return m_colItems.c4ElemCount();
}


TString TMEngEnumInfo::strFullName(const TMEngEnumVal& mecvValue) const
{
    return TString
    (
        TStrCat(strClassPath(), kCIDLib::chPeriod, m_colItems[mecvValue.c4Ordinal()].m_strName)
    );
}

TString TMEngEnumInfo::strFullName(const tCIDLib::TCard4 c4Ordinal) const
{
    return TString
    (
        TStrCat(strClassPath(), kCIDLib::chPeriod, m_colItems[c4Ordinal].m_strName)
    );
}


const TString& TMEngEnumInfo::strItemName(const TMEngEnumVal& mecvValue) const
{
    return m_colItems[mecvValue.c4Ordinal()].m_strName;
}

const TString& TMEngEnumInfo::strItemName(const tCIDLib::TCard4 c4Ordinal) const
{
    return m_colItems[c4Ordinal].m_strName;
}


TString TMEngEnumInfo::strPartialName(const TMEngEnumVal& mecvValue) const
{
    return TString
    (
        TStrCat(strName(), kCIDLib::chPeriod, m_colItems[mecvValue.c4Ordinal()].m_strName)
    );
}

TString TMEngEnumInfo::strPartialName(const tCIDLib::TCard4 c4Ordinal) const
{
    return TString(TStrCat(strName(), kCIDLib::chPeriod, m_colItems[c4Ordinal].m_strName));
}


const TString& TMEngEnumInfo::strTextValue(const TMEngEnumVal& mecvValue) const
{
    return m_colItems[mecvValue.c4Ordinal()].m_strText;
}

const TString& TMEngEnumInfo::strTextValue(const tCIDLib::TCard4 c4Ordinal) const
{
    return m_colItems[c4Ordinal].m_strText;
}


// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngEnumInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngEnumVal& mecvActual = static_cast<TMEngEnumVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if ((c2MethId == m_c2MethId_AtMax)
    ||  (c2MethId == m_c2MethId_AtMin))
    {
        // Get the return value
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        if (c2MethId == m_c2MethId_AtMax)
            mecvRet.bValue(mecvActual.bAtMax());
        else
            mecvRet.bValue(mecvActual.bAtMin());
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        // Set the return to the value before we increment
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(mecvActual.c4Ordinal());

        // This guy will only deccrement if not already at the beginning
        mecvActual.Decrement();
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        mecvActual.c4Ordinal(0);
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                    = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << m_colItems[mecvActual.c4Ordinal()].m_strText;
    }
     else if (c2MethId == m_c2MethId_FromName)
    {
        const TString& strToFind = meOwner.strStackValAt(c4FirstInd);

        // Try to find the item with the passed name
        const tCIDLib::TCard4 c4New = c4FindOrdinal(strToFind);
        if (c4New == kCIDLib::c4MaxCard)
        {
            // Its not our error, it's our parent class' error
            TMEngEnumInfo& meciPar = meOwner.meciFindAs<TMEngEnumInfo>(c2ParentClassId());
            meciPar.ThrowBaseErr(meOwner, EBaseErrs::NameNotFound, strToFind, strClassPath());
        }
        mecvActual.c4Ordinal(c4New);
    }
     else if (c2MethId == m_c2MethId_FromText)
    {
        const TString& strToFind = meOwner.strStackValAt(c4FirstInd);

        // Try to find the item with the passed text
        const tCIDLib::TCard4 c4New = c4OrdinalFromText(strToFind);
        if (c4New == kCIDLib::c4MaxCard)
        {
            // Its not our error, it's our parent class' error
            TMEngEnumInfo& meciPar = meOwner.meciFindAs<TMEngEnumInfo>(c2ParentClassId());
            meciPar.ThrowBaseErr(meOwner, EBaseErrs::TextNotFound, strClassPath());
        }
        mecvActual.c4Ordinal(c4New);
    }
     else if (c2MethId == m_c2MethId_FormatList)
    {
        //
        //  Get the target string to fill, the separator character, the
        //  prefix to use, and the name/value boolean indicator.
        //
        TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        const TString& strPref = meOwner.strStackValAt(c4FirstInd + 1);
        const tCIDLib::TCh chSep = meOwner.chStackValAt(c4FirstInd + 2);
        const tCIDLib::TBoolean bUseName = meOwner.bStackValAt(c4FirstInd + 3);

        // Put in the prefix
        TString& strTar = mecvToFill.strValue();
        strTar = strPref;
        strTar.Append(kCIDLib::chSpace);

        // And now iterate the values in the enumeration
        const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (bUseName)
                strTar.Append(m_colItems[c4Index].m_strName);
            else
                strTar.Append(m_colItems[c4Index].m_strText);

            // Add a separator if not on the last one
            if (c4Index + 1 != c4Count)
            {
                strTar.Append(chSep);
                strTar.Append(kCIDLib::chSpace);
            }
        }
    }
     else if (c2MethId == m_c2MethId_GetOrd)
    {
        // Get the return stack item and set it to our current value
        TMEngCard4Val& mecvToSet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvToSet.c4Value(mecvActual.c4Ordinal());
    }
     else if (c2MethId == m_c2MethId_GetOrdCount)
    {
        // Get the return stack item and set it to our count of ordinal values
        TMEngCard4Val& mecvToSet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvToSet.c4Value(m_colItems.c4ElemCount());
    }
     else if (c2MethId == m_c2MethId_GetName)
    {
        // Get the return stack item and set it to our current value
        TMEngStringVal& mecvToSet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvToSet.strValue(m_colItems[mecvActual.c4Ordinal()].m_strName);
    }
     else if (c2MethId == m_c2MethId_GetText)
    {
        // Get the return stack item and set it to our current text value
        TMEngStringVal& mecvToSet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvToSet.strValue(m_colItems[mecvActual.c4Ordinal()].m_strText);
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GreaterThan)
          ||  (c2MethId == m_c2MethId_GreaterThanEq)
          ||  (c2MethId == m_c2MethId_LessThan)
          ||  (c2MethId == m_c2MethId_LessThanEq))
    {
        // Get the return value and parm
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TMEngEnumVal& mecvComp = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);

        if (c2MethId == m_c2MethId_LessThan)
            mecvRet.bValue(mecvActual.c4Ordinal() < mecvComp.c4Ordinal());
        else if (c2MethId == m_c2MethId_LessThanEq)
            mecvRet.bValue(mecvActual.c4Ordinal() <= mecvComp.c4Ordinal());
        else if (c2MethId == m_c2MethId_GreaterThan)
            mecvRet.bValue(mecvActual.c4Ordinal() > mecvComp.c4Ordinal());
        else if (c2MethId == m_c2MethId_GreaterThanEq)
            mecvRet.bValue(mecvActual.c4Ordinal() >= mecvComp.c4Ordinal());
        else
            mecvRet.bValue(mecvActual.c4Ordinal() == mecvComp.c4Ordinal());
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        // Set the return to the value before we increment
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(mecvActual.c4Ordinal());

        // This guy will only increment if not already at the end
        mecvActual.Increment();
    }
     else if (c2MethId == m_c2MethId_MapFromText)
    {
        //
        //  Try to find the item with the passed text to either name or text
        //  depending on the second parm.
        //
        const TString& strToFind = meOwner.strStackValAt(c4FirstInd);
        tCIDLib::TCard4 c4New;
        if (meOwner.bStackValAt(c4FirstInd + 1))
            c4New = c4FindOrdinal(strToFind);
        else
            c4New = c4OrdinalFromText(strToFind);

        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        if (c4New == kCIDLib::c4MaxCard)
        {
            // We didn't find it, so use default value and return False
            TMEngEnumVal& mecvDef = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
            mecvActual.c4Ordinal(mecvDef.c4Ordinal());
            mecvRet.bValue(kCIDLib::False);
        }
         else
        {
            // We found it, so just update our ordinal and return True
            mecvActual.c4Ordinal(c4New);
            mecvRet.bValue(kCIDLib::True);
        }
    }
     else if (c2MethId == m_c2MethId_SetMax)
    {
        mecvActual.SetToMax();
    }
     else if (c2MethId == m_c2MethId_SetMin)
    {
        mecvActual.SetToMin();
    }
     else if (c2MethId == m_c2MethId_SetOrd)
    {
        const tCIDLib::TCard4 c4ToSet = meOwner.c4StackValAt(c4FirstInd);
        if (c4ToSet > mecvActual.c4MaxOrdinal())
        {
            ThrowAnErr
            (
                meOwner
                , m_c4ErrBadOrdinal
                , TCardinal(c4ToSet)
                , meOwner.strClassPathForId(mecvActual.c2ClassId())
            );
        }
        mecvActual.c4Ordinal(c4ToSet);
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        const TMEngEnumVal& mecvToSet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        mecvActual.c4Ordinal(mecvToSet.c4Ordinal());
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The errors only get set on the base MEng.Enum class. But derived types
//  need to be able to throw these errors. So they will call this method
//  to do that.
//
tCIDLib::TVoid
TMEngEnumInfo::ThrowBaseErr(        TCIDMacroEngine&    meOwner
                            , const EBaseErrs           eToThrow
                            , const MFormattable&       fmtblToken1)
{
    const tCIDLib::TCard4 c4Throw = m_pmeciErrors->c4FromMapValue(eToThrow);
    ThrowAnErr(meOwner, c4Throw, fmtblToken1);
}

tCIDLib::TVoid
TMEngEnumInfo::ThrowBaseErr(        TCIDMacroEngine&    meOwner
                            , const EBaseErrs           eToThrow
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2)
{
    const tCIDLib::TCard4 c4Throw = m_pmeciErrors->c4FromMapValue(eToThrow);
    ThrowAnErr(meOwner, c4Throw, fmtblToken1, fmtblToken2);
}


// ---------------------------------------------------------------------------
//  TMEngEnumInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

// A common worker used by some different public variations
tCIDLib::TCard4
TMEngEnumInfo::c4AddEnum(const  TString&        strName
                        , const TString&        strText
                        , const tCIDLib::TCard4 c4MapValue)
{
    facCIDMacroEng().CheckIdOverflow(m_colItems.c4ElemCount(), L"enums");

    const tCIDLib::TCard4 c4Ret = m_colItems.c4ElemCount();
    m_colItems.objAdd(TItem(strName, strText, c4MapValue));
    return c4Ret;
}


// An internal helper called from a public templatized method
tCIDLib::TCard4 TMEngEnumInfo::c4FromMap(const tCIDLib::TCard4 c4ToFind) const
{
    // Search for the item with this map value
    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        if (m_colItems[c4Index].m_c4MapVal == c4ToFind)
            break;
    }

    if (c4Index == c4Count)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_EnumMapValNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c4ToFind)
            , strClassPath()
        );
    }
    return c4Index;
}


tCIDLib::TVoid
TMEngEnumInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken1)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);

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
TMEngEnumInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
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


