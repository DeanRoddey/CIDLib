//
// FILE NAME: CIDMacroEng_ASCII.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/29/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the class info class for the MEng.ASCII macro level
//  class.
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
RTTIDecls(TMEngASCIIInfo,TMEngClassInfo)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_ASCIIClass
{
    const TString   strName(L"ASCII");
    const TString   strClassPath(L"MEng.System.Runtime.ASCII");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngASCIIInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngASCIIInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngASCIIInfo::strPath()
{
    return CIDMacroEng_ASCIIClass::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngASCIIInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngASCIIInfo::TMEngASCIIInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_ASCIIClass::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_FromDecDigit(kMacroEng::c2BadId)
    , m_c2MethId_FromHexDigit(kMacroEng::c2BadId)
    , m_c2MethId_IsDecDigit(kMacroEng::c2BadId)
    , m_c2MethId_IsHexDigit(kMacroEng::c2BadId)
    , m_c2MethId_ToDecDigit(kMacroEng::c2BadId)
    , m_c2MethId_ToHexDigit(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
{
}

TMEngASCIIInfo::~TMEngASCIIInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngASCIIInfo: Public, inherited methods
// ---------------------------------------------------------------------------

// To cut down on verbiage below, since we define a good number of literals
#define MEngDefASCIILit(nam,val) \
AddLiteral \
( \
    new TMEngLiteralVal \
    ( \
        L#nam \
        , tCIDMacroEng::EIntrinsics::Card1 \
        , new TMEngCard1Val(L#nam, tCIDMacroEng::EConstTypes::Const, val) \
    ) \
);


tCIDLib::TVoid TMEngASCIIInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  Add literals for common ASCII values.
    //

    // Control characters
    MEngDefASCIILit(kNUL , 0);
    MEngDefASCIILit(kSOH , 1);
    MEngDefASCIILit(kSTX , 2);
    MEngDefASCIILit(kETX , 3);
    MEngDefASCIILit(kACK , 6);
    MEngDefASCIILit(kBEL , 7);
    MEngDefASCIILit(kBS  , 8);
    MEngDefASCIILit(kTAB , 9);
    MEngDefASCIILit(kLF  , 10);
    MEngDefASCIILit(kVT  , 11);
    MEngDefASCIILit(kFF  , 12);
    MEngDefASCIILit(kCR  , 13);
    MEngDefASCIILit(kSO  , 14);
    MEngDefASCIILit(kSI  , 15);
    MEngDefASCIILit(kDLE , 16);
    MEngDefASCIILit(kDC1 , 17);
    MEngDefASCIILit(kDC2 , 18);
    MEngDefASCIILit(kDC3 , 19);
    MEngDefASCIILit(kDC4 , 20);
    MEngDefASCIILit(kNAK , 21);
    MEngDefASCIILit(kSYN , 22);
    MEngDefASCIILit(kETB , 23);
    MEngDefASCIILit(kCAN , 24);
    MEngDefASCIILit(kEM  , 25);
    MEngDefASCIILit(kSUB , 26);
    MEngDefASCIILit(kESC , 27);

    // Decimal digits
    MEngDefASCIILit(kDig_0, 0x30);
    MEngDefASCIILit(kDig_1, 0x31);
    MEngDefASCIILit(kDig_2, 0x32);
    MEngDefASCIILit(kDig_3, 0x33);
    MEngDefASCIILit(kDig_4, 0x34);
    MEngDefASCIILit(kDig_5, 0x35);
    MEngDefASCIILit(kDig_6, 0x36);
    MEngDefASCIILit(kDig_7, 0x37);
    MEngDefASCIILit(kDig_8, 0x38);
    MEngDefASCIILit(kDig_9, 0x39);

    // Upper case chars
    MEngDefASCIILit(kChar_A, 0x41);
    MEngDefASCIILit(kChar_B, 0x42);
    MEngDefASCIILit(kChar_C, 0x43);
    MEngDefASCIILit(kChar_D, 0x44);
    MEngDefASCIILit(kChar_E, 0x45);
    MEngDefASCIILit(kChar_F, 0x46);
    MEngDefASCIILit(kChar_G, 0x47);
    MEngDefASCIILit(kChar_H, 0x48);
    MEngDefASCIILit(kChar_I, 0x49);
    MEngDefASCIILit(kChar_J, 0x4A);
    MEngDefASCIILit(kChar_K, 0x4B);
    MEngDefASCIILit(kChar_L, 0x4C);
    MEngDefASCIILit(kChar_M, 0x4D);
    MEngDefASCIILit(kChar_N, 0x4E);
    MEngDefASCIILit(kChar_O, 0x4F);
    MEngDefASCIILit(kChar_P, 0x50);
    MEngDefASCIILit(kChar_Q, 0x51);
    MEngDefASCIILit(kChar_R, 0x52);
    MEngDefASCIILit(kChar_S, 0x53);
    MEngDefASCIILit(kChar_T, 0x54);
    MEngDefASCIILit(kChar_U, 0x55);
    MEngDefASCIILit(kChar_V, 0x56);
    MEngDefASCIILit(kChar_W, 0x57);
    MEngDefASCIILit(kChar_X, 0x58);
    MEngDefASCIILit(kChar_Y, 0x59);
    MEngDefASCIILit(kChar_Z, 0x5A);

    // Lower case chars
    MEngDefASCIILit(kChar_a, 0x61);
    MEngDefASCIILit(kChar_b, 0x62);
    MEngDefASCIILit(kChar_c, 0x63);
    MEngDefASCIILit(kChar_d, 0x64);
    MEngDefASCIILit(kChar_e, 0x65);
    MEngDefASCIILit(kChar_f, 0x66);
    MEngDefASCIILit(kChar_g, 0x67);
    MEngDefASCIILit(kChar_h, 0x68);
    MEngDefASCIILit(kChar_i, 0x69);
    MEngDefASCIILit(kChar_j, 0x6A);
    MEngDefASCIILit(kChar_k, 0x6B);
    MEngDefASCIILit(kChar_l, 0x6C);
    MEngDefASCIILit(kChar_m, 0x6D);
    MEngDefASCIILit(kChar_n, 0x6E);
    MEngDefASCIILit(kChar_o, 0x6F);
    MEngDefASCIILit(kChar_p, 0x70);
    MEngDefASCIILit(kChar_q, 0x71);
    MEngDefASCIILit(kChar_r, 0x72);
    MEngDefASCIILit(kChar_s, 0x73);
    MEngDefASCIILit(kChar_t, 0x74);
    MEngDefASCIILit(kChar_u, 0x75);
    MEngDefASCIILit(kChar_v, 0x76);
    MEngDefASCIILit(kChar_w, 0x77);
    MEngDefASCIILit(kChar_x, 0x78);
    MEngDefASCIILit(kChar_y, 0x79);
    MEngDefASCIILit(kChar_z, 0x7A);

    // Punctuation
    MEngDefASCIILit(kExclaim    , 0x21);
    MEngDefASCIILit(kQuote      , 0x22);
    MEngDefASCIILit(kPound      , 0x23);
    MEngDefASCIILit(kDollar     , 0x24);
    MEngDefASCIILit(kPercent    , 0x25);
    MEngDefASCIILit(kAmpersand  , 0x26);
    MEngDefASCIILit(kApostrophe , 0x27);
    MEngDefASCIILit(kOpenParen  , 0x28);
    MEngDefASCIILit(kCloseParen , 0x29);
    MEngDefASCIILit(kAsterisk   , 0x2A);
    MEngDefASCIILit(kPlus       , 0x2B);
    MEngDefASCIILit(kComma      , 0x2C);
    MEngDefASCIILit(kMinus      , 0x2D);
    MEngDefASCIILit(kPeriod     , 0x2E);
    MEngDefASCIILit(kSlash      , 0x2F);
    MEngDefASCIILit(kColon      , 0x3A);
    MEngDefASCIILit(kSemiColon  , 0x3B);
    MEngDefASCIILit(kLessThan   , 0x3C);
    MEngDefASCIILit(kEqual      , 0x3D);
    MEngDefASCIILit(kGreaterThan, 0x3E);
    MEngDefASCIILit(kQuestion   , 0x3F);
    MEngDefASCIILit(kAt         , 0x40);
    MEngDefASCIILit(kOpenBracket, 0x5B);
    MEngDefASCIILit(kBackSlash  , 0x5C);
    MEngDefASCIILit(kCloseBracket,0x5D);
    MEngDefASCIILit(kCaret      , 0x5E);
    MEngDefASCIILit(kUnderscore , 0x5F);
    MEngDefASCIILit(kGrave      , 0x60);

    MEngDefASCIILit(kOpenCurly  , 0x7B);
    MEngDefASCIILit(kVertBar    , 0x7C);
    MEngDefASCIILit(kCloseCurly , 0x7D);
    MEngDefASCIILit(kTilde      , 0x7E);
    MEngDefASCIILit(kDEL        , 0x7F);


    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"ASCIIErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_c4Err_NotDecDig = m_pmeciErrors->c4AddEnumItem(L"NotDecDig", L"%(1) is not an ASCII decimal digit");
        m_c4Err_NotHexDig = m_pmeciErrors->c4AddEnumItem(L"NotHexDig", L"%(1) is not an ASCII hex digit");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.ASCII"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Convert to decimal/hex digit digits
    {
        TMEngMethodInfo methiNew
        (
            L"FromDecDigit"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCvt", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_FromDecDigit = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FromHexDigit"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCvt", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_FromHexDigit = c2AddMethodInfo(methiNew);
    }

    // Test for decimal/hex digit digits
    {
        TMEngMethodInfo methiNew
        (
            L"IsDecDigit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_IsDecDigit = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"IsHexDigit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_IsHexDigit = c2AddMethodInfo(methiNew);
    }

    // Convert from decimal/hex digits
    {
        TMEngMethodInfo methiNew
        (
            L"ToDecDigit"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCvt", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_ToDecDigit = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ToHexDigit"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCvt", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_ToHexDigit = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngASCIIInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngStdClassVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngASCIIInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngASCIIInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      )
{
    //
    //  In our case, we have all external 'methods', so we just handle them
    //  all either right here, or call a private to do it.
    //
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if ((c2MethId == m_c2MethId_FromDecDigit)
    ||  (c2MethId == m_c2MethId_FromHexDigit))
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        tCIDLib::TCard4 c4Val = meOwner.c1StackValAt(c4FirstInd);

        tCIDLib::TCard4 c4Err = 0;
        if (c2MethId == m_c2MethId_FromDecDigit)
        {
            if ((c4Val < kCIDLib::chDigit0) || (c4Val > kCIDLib::chDigit9))
                c4Err = m_c4Err_NotDecDig;
            else
                c4Val -= kCIDLib::chDigit0;
        }
         else
        {
            if ((c4Val >= kCIDLib::chDigit0) && (c4Val <= kCIDLib::chDigit9))
                c4Val -= kCIDLib::chDigit0;
            else if ((c4Val >= kCIDLib::chLatin_A) && (c4Val <= kCIDLib::chLatin_F))
                c4Val -= 0x37;
            else if ((c4Val >= kCIDLib::chLatin_a) && (c4Val <= kCIDLib::chLatin_f))
                c4Val -= 0x57;
            else
                c4Err = m_c4Err_NotHexDig;
        }

        if (c4Err)
            ThrowAnErr(meOwner, c4Err, TCardinal(c4Val));

        // Looks ok, so give back the converted value, casting back to type
        mecvRet.c1Value(tCIDLib::TCard1(c4Val));
    }
     else if (c2MethId == m_c2MethId_IsDecDigit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        tCIDLib::TCard1 c1Val = meOwner.c1StackValAt(c4FirstInd);
        mecvRet.bValue
        (
            (c1Val >= kCIDLib::chDigit0) && (c1Val <= kCIDLib::chDigit9)
        );
    }
     else if (c2MethId == m_c2MethId_IsHexDigit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        tCIDLib::TCard1 c1Val = meOwner.c1StackValAt(c4FirstInd);

        if ((c1Val >= kCIDLib::chDigit0) && (c1Val <= kCIDLib::chDigit9)
        ||  (c1Val >= kCIDLib::chLatin_A) && (c1Val <= kCIDLib::chLatin_F)
        ||  (c1Val >= kCIDLib::chLatin_a) && (c1Val <= kCIDLib::chLatin_f))
        {
            mecvRet.bValue(kCIDLib::True);
        }
         else
        {
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if ((c2MethId == m_c2MethId_ToDecDigit)
          ||  (c2MethId == m_c2MethId_ToHexDigit))
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        tCIDLib::TCard1 c1Val = meOwner.c1StackValAt(c4FirstInd);

        tCIDLib::TCard4 c4Err = 0;
        if (c2MethId == m_c2MethId_ToDecDigit)
        {
            if (c1Val > 9)
                c4Err = m_c4Err_NotDecDig;
            else
                c1Val += 0x30;
        }
         else
        {
            if (c1Val < 0xA)
                c1Val += 0x30;
            else if (c1Val < 0x10)
                c1Val += 0x37;
            else
                c4Err = m_c4Err_NotHexDig;
        }

        if (c4Err)
            ThrowAnErr(meOwner, c4Err, TCardinal(c1Val));

        // Looks ok, so give back the converted value
        mecvRet.c1Value(c1Val);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngASCIIInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngASCIIInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
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


