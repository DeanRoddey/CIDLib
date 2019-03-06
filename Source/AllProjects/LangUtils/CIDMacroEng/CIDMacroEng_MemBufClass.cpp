//
// FILE NAME: CIDMacroEng_MemBufClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/18/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement a memory buffer object.
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
RTTIDecls(TMEngMemBufVal,TMEngClassVal)
RTTIDecls(TMEngMemBufInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_MemBufClass
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strMemBuf(L"MemBuf");
    const TString   strMemBufClassPath(L"MEng.System.Runtime.MemBuf");
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngMemBufVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMemBufVal: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Note that we don't know the real size until the macro level ctor is called,
//  so we allocate an 8 byte buffer as a place holder. Else, we'd have to use
//  a pointer and fault it in in the ctor. This is a little more overheap
//  perhaps, but safer.
//
//  We have a second version that lets the outside provide a buffer that we
//  just reference. This one would only be used in places where C++ code
//  calls directly into CML methods and wants to avoid having to copy data
//  into and out of a CML method parameter that takes a buffer.
//
TMEngMemBufVal::TMEngMemBufVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::Adopt)
    , m_pmbufValue(new THeapBuf(8))
{
}

TMEngMemBufVal::TMEngMemBufVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst
                                ,       THeapBuf* const         pmbufToUse) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
    , m_pmbufValue(pmbufToUse)
{
}

TMEngMemBufVal::~TMEngMemBufVal()
{
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
        delete m_pmbufValue;
}


// ---------------------------------------------------------------------------
//  TMEngMemBufVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMemBufVal::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&     meciThis
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    // Display the buffer size, then dump up to the first x number of bytes
    strmTarget  << L"Alloc Size: " << m_pmbufValue->c4Size();

    // If we have any bytes to display, then do it
    const tCIDLib::TCard4 c4Count = tCIDLib::MinVal
    (
        m_pmbufValue->c4Size()
        , (eFormat == tCIDMacroEng::EDbgFmts::Long) ? 128 : 8UL
    );

    if ((c4Count < m_pmbufValue->c4Size()) && (eFormat == tCIDMacroEng::EDbgFmts::Long))
        strmTarget << L"  (Showing first " << c4Count << L" bytes)\n";
    else
        strmTarget << kCIDLib::NewLn;

    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
        strmTarget << L", Bytes: ";

    // Save the stream state and set up for indicated byte format
    {
        TStreamJanitor janTarget(&strmTarget);
        if (eRadix == tCIDLib::ERadices::Hex)
        {
            strmTarget.chFill(kCIDLib::chDigit0);
            strmTarget.c4Width(2);
        }
         else
        {
            strmTarget.c4Width(3);
            strmTarget.chFill(kCIDLib::chSpace);
        }
        strmTarget.eJustification(tCIDLib::EHJustify::Right);

        // If short form, we only do 8 bytes, so we won't ever put out a new line
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (c4Index && !(c4Index % 8))
                strmTarget << kCIDLib::NewLn << TTextOutStream::Spaces(4);
            strmTarget  << TCardinal(m_pmbufValue->c1At(c4Index), eRadix)
                        << TTextOutStream::Spaces(1);
        }
    }
    return kCIDLib::True;

}


tCIDLib::TVoid
TMEngMemBufVal::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pmbufValue = *static_cast<const TMEngMemBufVal&>(mecvToCopy).m_pmbufValue;
}


// ---------------------------------------------------------------------------
//  TMEngMemBufVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
const THeapBuf& TMEngMemBufVal::mbufValue() const
{
    return *m_pmbufValue;
}


THeapBuf& TMEngMemBufVal::mbufValue()
{
    return *m_pmbufValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngMemBufInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMemBufInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMemBufInfo::strPath()
{
    return CIDMacroEng_MemBufClass::strMemBufClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngMemBufInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMemBufInfo::TMEngMemBufInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MemBufClass::strMemBuf
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_CalcSum(kMacroEng::c2BadId)
    , m_c2MethId_CheckIndRange(kMacroEng::c2BadId)
    , m_c2MethId_CompBytes(kMacroEng::c2BadId)
    , m_c2MethId_CopyIn(kMacroEng::c2BadId)
    , m_c2MethId_CopyInAt(kMacroEng::c2BadId)
    , m_c2MethId_CopyOut(kMacroEng::c2BadId)
    , m_c2MethId_ExportString(kMacroEng::c2BadId)
    , m_c2MethId_ExportStringAt(kMacroEng::c2BadId)
    , m_c2MethId_ExportVarString(kMacroEng::c2BadId)
    , m_c2MethId_ExtractDecDigAt(kMacroEng::c2BadId)
    , m_c2MethId_ExtractDecValAt(kMacroEng::c2BadId)
    , m_c2MethId_ExtractHexDigAt(kMacroEng::c2BadId)
    , m_c2MethId_ExtractHexValAt(kMacroEng::c2BadId)
    , m_c2MethId_GetCard1At(kMacroEng::c2BadId)
    , m_c2MethId_GetCard2At(kMacroEng::c2BadId)
    , m_c2MethId_GetCard4At(kMacroEng::c2BadId)
    , m_c2MethId_GetInt1At(kMacroEng::c2BadId)
    , m_c2MethId_GetInt2At(kMacroEng::c2BadId)
    , m_c2MethId_GetInt4At(kMacroEng::c2BadId)
    , m_c2MethId_GetAlloc(kMacroEng::c2BadId)
    , m_c2MethId_GetMaxSize(kMacroEng::c2BadId)
    , m_c2MethId_ImportString(kMacroEng::c2BadId)
    , m_c2MethId_ImportStringAt(kMacroEng::c2BadId)
    , m_c2MethId_InsertASCIIHexPair(kMacroEng::c2BadId)
    , m_c2MethId_MakeSpace(kMacroEng::c2BadId)
    , m_c2MethId_MoveToStart(kMacroEng::c2BadId)
    , m_c2MethId_PutCard1At(kMacroEng::c2BadId)
    , m_c2MethId_PutCard2At(kMacroEng::c2BadId)
    , m_c2MethId_PutCard4At(kMacroEng::c2BadId)
    , m_c2MethId_PutInt1At(kMacroEng::c2BadId)
    , m_c2MethId_PutInt2At(kMacroEng::c2BadId)
    , m_c2MethId_PutInt4At(kMacroEng::c2BadId)
    , m_c2MethId_Reallocate(kMacroEng::c2BadId)
    , m_c2MethId_SetAll(kMacroEng::c2BadId)
    , m_c2MethId_SzCtor(kMacroEng::c2BadId)
    , m_c4Err_BadChar(kCIDLib::c4MaxCard)
    , m_c4Err_BadDigitRange(kCIDLib::c4MaxCard)
    , m_c4Err_BadIndex(kCIDLib::c4MaxCard)
    , m_c4Err_BadInitSizes(kCIDLib::c4MaxCard)
    , m_c4Err_BadReallocSize(kCIDLib::c4MaxCard)
    , m_c4Err_BadValue(kCIDLib::c4MaxCard)
    , m_c4Err_ByteRange(kCIDLib::c4MaxCard)
    , m_c4Err_NotDecDig(kCIDLib::c4MaxCard)
    , m_c4Err_NotHexDig(kCIDLib::c4MaxCard)
    , m_c4Err_Overflow(kCIDLib::c4MaxCard)
    , m_c4Err_BlockMove(kCIDLib::c4MaxCard)
    , m_c4Err_SelfTarget(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngMemBufInfo::~TMEngMemBufInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngMemBufInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngMemBufInfo::Init(TCIDMacroEngine& meOwner)
{
    // We use our own class id a lot below, so get a copy
    const tCIDLib::TCard2 c2OurId = c2Id();

    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"MemBufErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 12
        );
        m_c4Err_BadDigitRange = m_pmeciErrors->c4AddEnumItem(L"BadDigRange", L"%(1) is too large a range for digit extraction");
        m_c4Err_BadChar = m_pmeciErrors->c4AddEnumItem(L"BadChar", L"Zero byte cannot be put into a string");
        m_c4Err_BadIndex = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"Index %(1) is beyond the buffer size of %(2)");
        m_c4Err_BadInitSizes = m_pmeciErrors->c4AddEnumItem(L"BadInitSizes", L"The initial (%(1)) or max (%(2)) size was invalid");
        m_c4Err_BadReallocSize = m_pmeciErrors->c4AddEnumItem(L"BadReallocSize", L"The realloc size (%(1)) is larger than the max size (%(2))");
        m_c4Err_BadValue = m_pmeciErrors->c4AddEnumItem(L"BadValue", L"Could not convert the %(1) bytes at index %(2) into a numeric value");
        m_c4Err_ByteRange = m_pmeciErrors->c4AddEnumItem(L"ByteRange", L"The source value is > 255");
        m_c4Err_NotDecDig = m_pmeciErrors->c4AddEnumItem(L"NotDecDig", L"The value at offset %(1) is not an ASCII decimal digit");
        m_c4Err_NotHexDig = m_pmeciErrors->c4AddEnumItem(L"NotHexDig", L"The value at offset %(1) is not an ASCII hex digit");
        m_c4Err_Overflow = m_pmeciErrors->c4AddEnumItem(L"Overflow", L"The start index (%(1)) plus the size (%(2)) would overflow the max buffer size (%(3))");
        m_c4Err_BlockMove = m_pmeciErrors->c4AddEnumItem(L"BlockMove", L"");
        m_c4Err_SelfTarget = m_pmeciErrors->c4AddEnumItem(L"SelfTarget", L"The source and target buffers cannot be the same");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }


    // Do a sum of a range of bytes in the buffer
    {
        TMEngMethodInfo methiNew
        (
            L"CalcSum"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CalcSum = c2AddMethodInfo(methiNew);
    }

    //
    //  Check a start/count range to see if it's valid. If bad, it
    //  will throw an exception.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"CheckIndRange"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"UseMaxSz", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_CheckIndRange = c2AddMethodInfo(methiNew);
    }

    // Comp a range of bytes in this buffer to the same range in another
    {
        TMEngMethodInfo methiNew
        (
            L"CompByteRange"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcBuf", c2OurId);
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CompBytes = c2AddMethodInfo(methiNew);
    }

    // Copy bytes into our out of another memory buffer
    {
        TMEngMethodInfo methiNew
        (
            L"CopyIn"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcBuf", c2OurId);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CopyIn = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"CopyInAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcBuf", c2OurId);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"AtInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CopyInAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"CopyOut"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"TargetBuf", c2OurId);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"StartInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_CopyOut = c2AddMethodInfo(methiNew);
    }

    // Export out to a string (i.e. create a Unicode char from each byte)
    {
        TMEngMethodInfo methiNew
        (
            L"ExportString"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExportString = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExportStringAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"SrcInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExportStringAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExportVarString"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"MaxCnt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"SrcInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExportVarString = c2AddMethodInfo(methiNew);
    }

    // Extract ASCII hex or decimal digits at a given offset
    {
        TMEngMethodInfo methiNew
        (
            L"ExtractDecDigAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractDecDigAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractDecValAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractDecValAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractHexDigAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractHexDigAt = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ExtractHexValAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ExtractHexValAt = c2AddMethodInfo(methiNew);
    }

    // Find the next byte with the given value
    {
        TMEngMethodInfo methiNew
        (
            L"FindByte"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"StartOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_FindByte = c2AddMethodInfo(methiNew);
    }

    // Get all the card/int types from the buffer at a given offset
    {
        TMEngMethodInfo methiNew
        (
            L"GetCard1At"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard1At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard2At"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard2At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard4At"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard4At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetFloat4At"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetFloat4At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetFloat8At"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetFloat8At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt1At"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt1At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt2At"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt2At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt4At"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt4At = c2AddMethodInfo(methiNew);
    }

    // Get the allocation size and max size of the buffer
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

    {
        TMEngMethodInfo methiNew
        (
            L"GetMaxSize"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetMaxSize = c2AddMethodInfo(methiNew);
    }

    // Import a string into the buffer (i.e. take each low byte of chars)
    {
        TMEngMethodInfo methiNew
        (
            L"ImportString"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Src", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ImportString = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ImportStringAt"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Src", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"TarInd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ImportStringAt = c2AddMethodInfo(methiNew);
    }

    // Format a Card1 value to a byte ASCII pair and insert the two bytes
    {
        TMEngMethodInfo methiNew
        (
            L"InsertASCIIHexPair"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToInsert", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"At", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_InsertASCIIHexPair = c2AddMethodInfo(methiNew);
    }

    // Copy up bytes in a buffer to make space
    {
        TMEngMethodInfo methiNew
        (
            L"MakeSpace"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"SpaceSz", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"FullCnt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MakeSpace = c2AddMethodInfo(methiNew);
    }

    // Copy bytes down in a memory buffer to the start of the buffer
    {
        TMEngMethodInfo methiNew
        (
            L"MoveToStart"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MoveToStart = c2AddMethodInfo(methiNew);
    }

    // Set all the card/int types from the buffer at a given offset
    {
        TMEngMethodInfo methiNew
        (
            L"PutCard1At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_PutCard1At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutCard2At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_PutCard2At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutCard4At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_PutCard4At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutFloat4At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_PutFloat4At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutFloat8At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_PutFloat8At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutInt1At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_PutInt1At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutInt2At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_PutInt2At = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PutInt4At"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"AtOfs", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToPut", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_PutInt4At = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Size", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Reallocate = c2AddMethodInfo(methiNew);
    }

    // Copy down bytes in a buffer to compact it
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveSpace"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"RemoveCnt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"FullCnt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_RemoveSpace = c2AddMethodInfo(methiNew);
    }

    // Set all bytes of the buffer to a specific value
    {
        TMEngMethodInfo methiNew
        (
            L"SetAll"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FillChar", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_SetAll = c2AddMethodInfo(methiNew);
    }

    // Add the constructor, which takes a buffer size
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.MemBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitSize", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"MaxSize", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SzCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngMemBufInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngMemBufVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMacroEngMemBufInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is a convenience so that other CML wrapper classes can do a check
//  on the start/count range of access to a memory buffer object to see if
//  it's valid, and it not to have a CML level exception thrown.
//
tCIDLib::TVoid
TMEngMemBufInfo::IndexCheck(        TCIDMacroEngine&    meOwner
                            , const TMEngMemBufVal&     mecvCheck
                            , const tCIDLib::TCard4     c4Count
                            , const tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bMaxSize)
{
    const THeapBuf& mbufCheck = mecvCheck.mbufValue();
    CheckIndex(meOwner, mbufCheck, c4Count, c4Index, bMaxSize);
}


// ---------------------------------------------------------------------------
//  TMacroEngMemBufInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMemBufInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngMemBufVal& mecvActual = static_cast<TMEngMemBufVal&>(mecvInstance);
    THeapBuf& mbufInst = mecvActual.mbufValue();

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_CalcSum)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const tCIDLib::TCard4 c4Start = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckIndex(meOwner, mbufInst, c4Count, c4Start);
        mecvRet.c4Value(mbufInst.c4CheckSum(c4Start, c4Count));
    }
     else if (c2MethId == m_c2MethId_CheckIndRange)
    {
        const tCIDLib::TCard4 c4Start = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TBoolean bUseMax = meOwner.bStackValAt(c4FirstInd + 2);
        CheckIndex(meOwner, mbufInst, c4Count, c4Start, bUseMax);
    }
     else if (c2MethId == m_c2MethId_CompBytes)
    {
        TMEngMemBufVal& mecvSrc = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 2);

        // Check both buffers for the count and index against current size (not max)
        CheckIndex(meOwner, mbufInst, c4Count, c4Index, kCIDLib::False);
        CheckIndex(meOwner, mecvSrc.mbufValue(), c4Count, c4Index, kCIDLib::False);

        // They are both legal, so compare the bytes
        tCIDLib::TBoolean bRes = kCIDLib::True;

        const tCIDLib::TCard1* pc1This = mbufInst.pc1DataAt(c4Index);
        const tCIDLib::TCard1* pc1Src = mecvSrc.mbufValue().pc1DataAt(c4Index);
        const tCIDLib::TCard1* const pc1End = pc1This + c4Count;

        while (pc1This < pc1End)
        {
            if (*pc1This != *pc1Src)
            {
                bRes = kCIDLib::False;
                break;
            }
            pc1This++;
            pc1Src++;
        }

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(bRes);
    }
     else if (c2MethId == m_c2MethId_CopyIn)
    {
        TMEngMemBufVal& mecvSrc = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 2);

        // The two buffers cannot be the same
        if (&mbufInst == &mecvSrc.mbufValue())
            ThrowAnErr(meOwner, m_c4Err_SelfTarget);

        CheckIndex(meOwner, mbufInst, c4Count, 0);
        CheckIndex(meOwner, mecvSrc.mbufValue(), c4Count, c4Index, kCIDLib::False);
        mecvSrc.mbufValue().CopyOut(mbufInst, c4Count, c4Index);
    }
     else if (c2MethId == m_c2MethId_CopyInAt)
    {
        TMEngMemBufVal& mecvSrc = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4AtIndex = meOwner.c4StackValAt(c4FirstInd + 2);

        // The two buffers cannot be the same
        if (&mbufInst == &mecvSrc.mbufValue())
            ThrowAnErr(meOwner, m_c4Err_SelfTarget);

        CheckIndex(meOwner, mbufInst, c4Count, c4AtIndex);
        CheckIndex(meOwner, mecvSrc.mbufValue(), c4Count, 0, kCIDLib::False);
        mbufInst.CopyIn(mecvSrc.mbufValue(), c4Count, c4AtIndex);
    }
     else if (c2MethId == m_c2MethId_CopyOut)
    {
        TMEngMemBufVal& mecvTarget = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 2);

        // The two buffers cannot be the same
        if (&mbufInst == &mecvTarget.mbufValue())
            ThrowAnErr(meOwner, m_c4Err_SelfTarget);

        CheckIndex(meOwner, mbufInst, c4Count, c4Index, kCIDLib::False);
        CheckIndex(meOwner, mecvTarget.mbufValue(), c4Count, 0);
        mbufInst.CopyOut(mecvTarget.mbufValue(), c4Count, c4Index);
    }
     else if ((c2MethId == m_c2MethId_ExportString)
          ||  (c2MethId == m_c2MethId_ExportStringAt)
          ||  (c2MethId == m_c2MethId_ExportVarString))
    {
        // Get the target string and count value
        TMEngStringVal& mecvTarget = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        TString& strTar = mecvTarget.strValue();
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);

        //
        //  If the 'at' version get that and make sure the index and count
        //  are valid for our buffer. If not the 'at' version, then we just
        //  start the beginning.
        //
        tCIDLib::TCard4 c4At = 0;
        if ((c2MethId == m_c2MethId_ExportStringAt)
        ||  (c2MethId == m_c2MethId_ExportVarString))
        {
            c4At = meOwner.c4StackValAt(c4FirstInd + 2);
        }
        CheckIndex(meOwner, mbufInst, c4Count, c4At, kCIDLib::False);

        strTar.Clear();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const tCIDLib::TCard1 c1Cur = mbufInst[c4At++];
            if (!c1Cur)
            {
                //
                //  We hit a null. If it's export at, then this is an
                //  error, otherwise, it's a var string export so we just
                //  treat this as the end point.
                //
                if (c2MethId == m_c2MethId_ExportVarString)
                    break;
                ThrowAnErr(meOwner, m_c4Err_BadChar);
            }
            strTar.Append(tCIDLib::TCh(c1Cur));
        }
    }
     else if (c2MethId == m_c2MethId_ExtractDecDigAt)
    {
        // Call a helper for this
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value
        (
            c4ExtractASCII
            (
                meOwner
                , mbufInst
                , meOwner.c4StackValAt(c4FirstInd)
                , 1
                , tCIDLib::ERadices::Dec
            )
        );
    }
     else if (c2MethId == m_c2MethId_ExtractDecValAt)
    {
        // Call a helper for this
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value
        (
            c4ExtractASCII
            (
                meOwner
                , mbufInst
                , meOwner.c4StackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , tCIDLib::ERadices::Dec
            )
        );
    }
     else if (c2MethId == m_c2MethId_ExtractHexDigAt)
    {
        // Call a helper for this
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value
        (
            c4ExtractASCII
            (
                meOwner
                , mbufInst
                , meOwner.c4StackValAt(c4FirstInd)
                , 1
                , tCIDLib::ERadices::Hex
            )
        );
    }
     else if (c2MethId == m_c2MethId_ExtractHexValAt)
    {
        // Call a helper for this
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value
        (
            c4ExtractASCII
            (
                meOwner
                , mbufInst
                , meOwner.c4StackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , tCIDLib::ERadices::Hex
            )
        );
    }
     else if (c2MethId == m_c2MethId_FindByte)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard1 c1Find = meOwner.c1StackValAt(c4FirstInd + 1);
        CheckIndex(meOwner, mbufInst, 1, c4At, kCIDLib::False);

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mbufInst.c4FindByte(c4At, c1Find));
    }
     else if (c2MethId == m_c2MethId_GetCard1At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 1, c4At, kCIDLib::False);
        TMEngCard1Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mbufInst.c1At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetCard2At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 2, c4At, kCIDLib::False);
        TMEngCard2Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mbufInst.c2At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetCard4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At, kCIDLib::False);
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mbufInst.c4At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetFloat4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At, kCIDLib::False);
        TMEngFloat4Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(mbufInst.f4At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetFloat8At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 8, c4At, kCIDLib::False);
        TMEngFloat8Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(mbufInst.f8At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetInt1At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 1, c4At, kCIDLib::False);
        TMEngInt1Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mbufInst.i1At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetInt2At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 2, c4At, kCIDLib::False);
        TMEngInt2Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mbufInst.i2At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetInt4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At, kCIDLib::False);
        TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mbufInst.i4At(c4At));
    }
     else if (c2MethId == m_c2MethId_GetAlloc)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mbufInst.c4Size());
    }
     else if (c2MethId == m_c2MethId_GetMaxSize)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mbufInst.c4MaxSize());
    }
     else if ((c2MethId == m_c2MethId_ImportString)
          ||  (c2MethId == m_c2MethId_ImportStringAt))
    {
        // Get the source string and count value
        const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);

        //
        //  If the 'at' version, then get that, else we start at the
        //  beginning.
        //
        tCIDLib::TCard4 c4At = 0;
        if (c2MethId == m_c2MethId_ImportStringAt)
            c4At = meOwner.c4StackValAt(c4FirstInd + 2);

        //
        //  If the count is beyond the length of the src string, then
        //  set it back to the string length.
        //
        const tCIDLib::TCard4 c4SrcLen = strSrc.c4Length();
        if (c4Count > c4SrcLen)
            c4Count = c4SrcLen;

        // Make sure these are valid for our buffer
        CheckIndex(meOwner, mbufInst, c4Count, c4At);

        //
        //  Increase our allocation if needed. We checked above that the
        //  at+count is not beyond our max allocation.
        //
        mbufInst.Reallocate(c4At + c4Count, kCIDLib::True);

        //
        //  Run through the source chars and pull each char into the
        //  byte at the target index in our buffer. If the char is not
        //  in the first 256 codes, then throw.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const tCIDLib::TCh chCur = strSrc[c4Index];
            if (chCur > 255)
                ThrowAnErr(meOwner, m_c4Err_ByteRange);
            mbufInst[c4At++] = tCIDLib::TCard1(chCur);
        }

        // Give back the count we actually did
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Count);
    }
     else if (c2MethId == m_c2MethId_InsertASCIIHexPair)
    {
        const tCIDLib::TCard1 c1Val = meOwner.c1StackValAt(c4FirstInd);
        tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckIndex(meOwner, mbufInst, 2, c4At);

        tCIDLib::TCh szTmp[3];
        TRawStr::bFormatVal(c1Val, szTmp, 2, tCIDLib::ERadices::Hex);
        if (c1Val < 0x10)
        {
            mbufInst[c4At] = 0x20;
            mbufInst[c4At + 1] = tCIDLib::TCard1(szTmp[0]);
        }
         else
        {
            mbufInst[c4At] = tCIDLib::TCard1(szTmp[0]);
            mbufInst[c4At + 1] = tCIDLib::TCard1(szTmp[1]);
        }
    }
     else if (c2MethId == m_c2MethId_MakeSpace)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4SpaceSz = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4TopCnt = meOwner.c4StackValAt(c4FirstInd + 2);

        try
        {
            mbufInst.MakeSpace(c4At, c4SpaceSz, c4TopCnt);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_BlockMove, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_MoveToStart)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd + 1);
        CheckIndex(meOwner, mbufInst, c4Count, c4At);

        try
        {
            mbufInst.MoveToStart(c4At, c4Count);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_BlockMove, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_PutCard1At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 1, c4At);
        mbufInst.PutCard1(meOwner.c1StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutCard2At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 2, c4At);
        mbufInst.PutCard2(meOwner.c2StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutCard4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At);
        mbufInst.PutCard4(meOwner.c4StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutFloat4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At);
        mbufInst.PutFloat4(meOwner.f4StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutFloat8At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 8, c4At);
        mbufInst.PutFloat8(meOwner.f8StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutInt1At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 1, c4At);
        mbufInst.PutInt1(meOwner.i1StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutInt2At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 2, c4At);
        mbufInst.PutInt2(meOwner.i2StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_PutInt4At)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        CheckIndex(meOwner, mbufInst, 4, c4At);
        mbufInst.PutInt4(meOwner.i4StackValAt(c4FirstInd + 1), c4At);
    }
     else if (c2MethId == m_c2MethId_Reallocate)
    {
        const tCIDLib::TCard4 c4NewSize = meOwner.c4StackValAt(c4FirstInd);
        if (c4NewSize > mbufInst.c4MaxSize())
            ThrowAnErr(meOwner, m_c4Err_BadReallocSize, TCardinal(c4NewSize), TCardinal(mbufInst.c4MaxSize()));
        mbufInst.Reallocate(c4NewSize, kCIDLib::False);
    }
     else if (c2MethId == m_c2MethId_RemoveSpace)
    {
        const tCIDLib::TCard4 c4At = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4SpaceSz = meOwner.c4StackValAt(c4FirstInd + 1);
        const tCIDLib::TCard4 c4TopCnt = meOwner.c4StackValAt(c4FirstInd + 2);

        try
        {
            mbufInst.RemoveSpace(c4At, c4SpaceSz, c4TopCnt);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_BlockMove, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_SetAll)
    {
        mbufInst.Set(meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SzCtor)
    {
        const tCIDLib::TCard4 c4InitSz = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4MaxSz = meOwner.c4StackValAt(c4FirstInd + 1);

        if (!c4MaxSz || (c4MaxSz < c4InitSz))
            ThrowAnErr(meOwner, m_c4Err_BadInitSizes, TCardinal(c4InitSz), TCardinal(c4MaxSz));

        // The parms are the new init size and max size
        mbufInst.Reset(c4InitSz, c4MaxSz);
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMacroEngMemBufInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngMemBufInfo::CheckIndex(        TCIDMacroEngine&    meOwner
                            , const TMemBuf&            mbufCheck
                            , const tCIDLib::TCard4     c4Count
                            , const tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bMaxSize)
{
    // We either check against the buffer's max or current size
    const tCIDLib::TCard4 c4MaxSize = bMaxSize ? mbufCheck.c4MaxSize()
                                               : mbufCheck.c4Size();
    if (c4Index >= c4MaxSize)
    {
        ThrowAnErr
        (
            meOwner
            , m_c4Err_BadIndex
            , TCardinal(c4Index)
            , TCardinal(c4MaxSize)
        );
    }

    // Cast to Card8 to avoid a wrap around problem
    if (tCIDLib::TCard8(c4Index) + tCIDLib::TCard8(c4Count) > c4MaxSize)
    {
        ThrowAnErr
        (
            meOwner
            , m_c4Err_Overflow
            , TCardinal(c4Index)
            , TCardinal(c4Count)
            , TCardinal(c4MaxSize)
        );
    }
}


tCIDLib::TCard4
TMEngMemBufInfo::c4ExtractASCII(        TCIDMacroEngine&    meOwner
                                , const TMemBuf&            mbufSrc
                                , const tCIDLib::TCard4     c4At
                                , const tCIDLib::TCard4     c4Count
                                , const tCIDLib::ERadices   eRadix)
{
    // Check the range first
    CheckIndex(meOwner, mbufSrc, c4Count, c4At);

    //
    //  And even if it fits in the buffer, check for stupid ranges that
    //  are obviously too large to convert in decimal radix. Hex will
    //  use even fewer. This is mainly to allow us to use a local buffer
    //  for a cheap transcode.
    //
    const tCIDLib::TCard4 c4MaxChars = 10;
    if (!c4Count || (c4Count > c4MaxChars))
        ThrowAnErr(meOwner, m_c4Err_BadDigitRange, TCardinal(c4Count));

    //
    //  We can do a quick and dirty transcode here into a temp buffer,
    //  because the stuff is supposed to all be ASCII decimal or hex
    //  digits. So convert them to Unicode format in a local by just
    //  doing a zero extension. Check each one to make sure it fits the
    //  radix.
    //
    tCIDLib::TCh achBuf[c4MaxChars + 1];
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        tCIDLib::TCard1 c1Cur = mbufSrc.c1At(c4At + c4Index);

        if (eRadix == tCIDLib::ERadices::Dec)
        {
            if ((c1Cur < kCIDLib::chDigit0) || (c1Cur > kCIDLib::chDigit9))
                ThrowAnErr(meOwner, m_c4Err_NotDecDig, TCardinal(c4At + c4Index));
        }
         else
        {
            if ((c1Cur < kCIDLib::chDigit0) && (c1Cur > kCIDLib::chDigit9)
            &&  (c1Cur < kCIDLib::chLatin_A) && (c1Cur > kCIDLib::chLatin_F)
            &&  (c1Cur < kCIDLib::chLatin_a) && (c1Cur > kCIDLib::chLatin_f))
            {
                ThrowAnErr(meOwner, m_c4Err_NotHexDig, TCardinal(c4At + c4Index));
            }
        }

        // It is ok, so store it
        achBuf[c4Index] = tCIDLib::TCh(c1Cur);
    }

    // Zero terminate the string and try to convert it
    achBuf[c4Index] = kCIDLib::chNull;
    tCIDLib::TBoolean bOk;
    const tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary(achBuf, bOk, eRadix);
    if (!bOk)
        ThrowAnErr(meOwner, m_c4Err_BadValue, TCardinal(c4At), TCardinal(c4Count));

    return c4Ret;
}


tCIDLib::TVoid
TMEngMemBufInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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
TMEngMemBufInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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
TMEngMemBufInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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


tCIDLib::TVoid
TMEngMemBufInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2
                            , const MFormattable&       fmtblToken3)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);
    strText.eReplaceToken(fmtblToken3, kCIDLib::chDigit3);

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


