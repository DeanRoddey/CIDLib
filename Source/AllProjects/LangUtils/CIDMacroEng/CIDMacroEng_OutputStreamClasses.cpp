//
// FILE NAME: CIDMacroEng_OutputStreamClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/07/2003
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
//  This file implements a set of derivatives of the type info and type
//  value classes, which implement a base output text stream class, and some
//  derivatives for files and strings and console.
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
RTTIDecls(TMEngTextOutStreamVal,TMEngClassVal)
RTTIDecls(TMEngTextOutStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngConOutStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngFileOutStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngStringOutStreamInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_OuptutStreamClasses
{
    static const TString strTextOutPath(L"MEng.System.Runtime.TextOutStream");
    static const TString strStringOutPath(L"MEng.System.Runtime.StringOutStream");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextOutStreamVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextOutStreamVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngTextOutStreamVal::
TMEngTextOutStreamVal(  const   TString&                strName
                        , const tCIDLib::TCard2         c2ClassId
                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
    , m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
    , m_pstrmTarget(0)
    , m_strEncoding(L"USASCII")
{
}

TMEngTextOutStreamVal::~TMEngTextOutStreamVal()
{
    // If we have a stream and we own it, clean it up
    if (m_pstrmTarget && (m_eAdopt == tCIDLib::EAdoptOpts::Adopt))
        delete m_pstrmTarget;
}


// ---------------------------------------------------------------------------
//  TMEngTextOutStreamVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngTextOutStreamVal::bIsSet() const
{
    return (m_pstrmTarget != 0);
}


tCIDLib::TVoid TMEngTextOutStreamVal::Flush()
{
    if (m_pstrmTarget)
        m_pstrmTarget->Flush();
}


const TString& TMEngTextOutStreamVal::strEncoding() const
{
    return m_strEncoding;
}

const TString& TMEngTextOutStreamVal::strEncoding(const TString& strToSet)
{
    m_strEncoding = strToSet;
    return m_strEncoding;
}


const TTextOutStream&
TMEngTextOutStreamVal::strmTarget(TCIDMacroEngine& meOwner) const
{
    // The class object should check first, but check anyway
    if (!m_pstrmTarget)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_NotCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , meOwner.meciFind(c2ClassId()).strClassPath()
        );
    }
    return *m_pstrmTarget;
}

TTextOutStream& TMEngTextOutStreamVal::strmTarget(TCIDMacroEngine& meOwner)
{
    // The class object should check first, but check anyway
    if (!m_pstrmTarget)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_NotCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , meOwner.meciFind(c2ClassId()).strClassPath()
        );
    }
    return *m_pstrmTarget;
}


tCIDLib::TVoid
TMEngTextOutStreamVal::SetStream(       TTextOutStream* const   pstrmToUse
                                , const tCIDLib::EAdoptOpts     eAdopt)
{
    if (m_pstrmTarget)
    {
        // If the current adopt flag is set, we delete this one
        if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
            delete m_pstrmTarget;
        m_pstrmTarget = 0;
    }

    m_eAdopt = eAdopt;
    m_pstrmTarget = pstrmToUse;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngTextOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextOutStreamInfo: Public, static data
// ---------------------------------------------------------------------------

const TString&  TMEngTextOutStreamInfo::strPath()
{
    return CIDMacroEng_OuptutStreamClasses::strTextOutPath;
}


// ---------------------------------------------------------------------------
//  TMEngTextOutStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTextOutStreamInfo::TMEngTextOutStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"TextOutStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Abstract
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2EnumId_Justify(kCIDMacroEng::c2BadId)
    , m_c2EnumId_Radix(kCIDMacroEng::c2BadId)
    , m_c2MethId_DNewLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_Flush(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtBool(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard1(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard1R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard2(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard2R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard4(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard4R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard8(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCard8R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtChar(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtCurLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtFloat4(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtFloat8(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtFmtable(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt1(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt1R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt2(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt2R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt4(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtInt4R(kCIDMacroEng::c2BadId)
    , m_c2MethId_FmtString(kCIDMacroEng::c2BadId)
    , m_c2MethId_NewLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_Reset(kCIDMacroEng::c2BadId)
    , m_c2MethId_SeekToEnd(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetJust(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetWidth(kCIDMacroEng::c2BadId)
    , m_c4ErrId_FmtErr(kCIDLib::c4MaxCard)
    , m_pmeciErrs(0)
    , m_pmeciJustify(0)
{
}

TMEngTextOutStreamInfo::~TMEngTextOutStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngTextOutStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngTextOutStreamInfo::Init(TCIDMacroEngine& meOwner)
{
    // Create an enum for our errors.
    {
        m_pmeciErrs = new TMEngEnumInfo
        (
            meOwner
            , L"TextOutStreamErrs"
            , strClassPath()
            , L"MEng.Enum"
            , 1
        );
        m_c4ErrId_FmtErr = m_pmeciErrs->c4AddEnumItem(L"FmtErr", TString::strEmpty());
        m_pmeciErrs->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrs);
        bAddNestedType(m_pmeciErrs->strClassPath());
    }

    // Generate our nested justification class
    {
        m_pmeciJustify = new TMEngEnumInfo
        (
            meOwner
            , L"StreamJustify"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );

        m_pmeciJustify->c4AddEnumItem(L"Left", L"Left", tCIDLib::EHJustify::Left);
        m_pmeciJustify->c4AddEnumItem(L"Right", L"Right", tCIDLib::EHJustify::Right);
        m_pmeciJustify->c4AddEnumItem(L"Center", L"Center", tCIDLib::EHJustify::Center);
        m_pmeciJustify->BaseClassInit(meOwner);
        m_c2EnumId_Justify = meOwner.c2AddClass(m_pmeciJustify);
        bAddNestedType(m_pmeciJustify->strClassPath());
    }

    // Look up the radix enum class's id and keep it around
    m_c2EnumId_Radix = meOwner.c2FindClassId(TMEngBaseInfoInfo::strRadixPath());

    // Put out two new lines
    {
        TMEngMethodInfo methiNew
        (
            L"DNewLn"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_DNewLine = c2AddMethodInfo(methiNew);
    }

    // Flush the stream
    {
        TMEngMethodInfo methiNew
        (
            L"Flush"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Flush = c2AddMethodInfo(methiNew);
    }

    // Format a Boolean
    {
        TMEngMethodInfo methiNew
        (
            L"FmtBool"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FmtBool = c2AddMethodInfo(methiNew);
    }

    // Format a Card1 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_FmtCard1 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard1R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtCard1R = c2AddMethodInfo(methiNew);
    }

    // Format a Card2 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_FmtCard2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard2R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtCard2R = c2AddMethodInfo(methiNew);
    }

    // Format a Card4 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_FmtCard4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard4R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtCard4R = c2AddMethodInfo(methiNew);
    }

    // Format a Card8 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_FmtCard8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtCard8R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtCard8R = c2AddMethodInfo(methiNew);
    }

    // Format a Char
    {
        TMEngMethodInfo methiNew
        (
            L"FmtChar"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_FmtChar = c2AddMethodInfo(methiNew);
    }

    // Format the current line within the current class
    {
        TMEngMethodInfo methiNew
        (
            L"FmtCurLine"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_FmtCurLine = c2AddMethodInfo(methiNew);
    }

    // Format a Float4
    {
        TMEngMethodInfo methiNew
        (
            L"FmtFloat4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_FmtFloat4 = c2AddMethodInfo(methiNew);
    }

    // Format a Float8
    {
        TMEngMethodInfo methiNew
        (
            L"FmtFloat8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_FmtFloat8 = c2AddMethodInfo(methiNew);
    }

    //
    //  This is a special one. It will take any class that derives from
    //  the formattable class.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"Format"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Formattable);
        m_c2MethId_FmtFmtable = c2AddMethodInfo(methiNew);
    }

    // Format an Int1 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_FmtInt1 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt1R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int1);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtInt1R = c2AddMethodInfo(methiNew);
    }

    // Format an Int2 amd with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_FmtInt2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt2R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int2);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtInt2R = c2AddMethodInfo(methiNew);
    }

    // Format an Int4 and with radix
    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_FmtInt4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FmtInt4R"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::Int4);
        methiNew.c2AddInParm(L"Radix", m_c2EnumId_Radix);
        m_c2MethId_FmtInt4R = c2AddMethodInfo(methiNew);
    }

    // Format a string
    {
        TMEngMethodInfo methiNew
        (
            L"FmtStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFmt", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FmtString = c2AddMethodInfo(methiNew);
    }

    // Put out a new line
    {
        TMEngMethodInfo methiNew
        (
            L"NewLn"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_NewLine = c2AddMethodInfo(methiNew);
    }

    // Reset the stream, which will truncate it back to empty state
    {
        TMEngMethodInfo methiNew
        (
            L"Reset"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Reset = c2AddMethodInfo(methiNew);
    }

    // Seek to the end of input
    {
        TMEngMethodInfo methiNew
        (
            L"SeekToEnd"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SeekToEnd = c2AddMethodInfo(methiNew);
    }

    // Set the justification
    {
        TMEngMethodInfo methiNew
        (
            L"SetJustify"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2EnumId_Justify);
        m_c2MethId_SetJust = c2AddMethodInfo(methiNew);
    }

    // Set the field width
    {
        TMEngMethodInfo methiNew
        (
            L"SetWidth"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetWidth = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngTextOutStreamInfo::pmecvMakeStorage(const  TString&
                                        ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes) const
{
    // These cannot be created directly, only derived classes
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcClass_ClassHasNoValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
        , strClassPath()
    );

    // Make the compiler happy
    return 0;
}



// ---------------------------------------------------------------------------
//  TMEngTextOutStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTextOutStreamInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextOutStreamVal& mecvActual
                    = static_cast<TMEngTextOutStreamVal&>(mecvInstance);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    // All methods at this level require the stream to be ready
    if (!mecvActual.bIsSet())
    {
        // We throw an exception from BaseInfo
        TMEngBaseInfoInfo* pmeciBaseInfo= meOwner.pmeciFindAs<TMEngBaseInfoInfo>
        (
            TMEngBaseInfoInfo::strPath()
        );
        pmeciBaseInfo->ThrowErr1
        (
            pmeciBaseInfo->c4ErrIdNotOpened
            , strClassPath()
            , meOwner
            , meOwner.meciFind(mecvActual.c2ClassId()).strClassPath()
        );
    }

    //
    //  In our case, we have all external 'methods', so we just handle them
    //  all right here.
    //
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    if (c2MethId == m_c2MethId_DNewLine)
    {
        mecvActual.strmTarget(meOwner) << kCIDLib::DNewLn;
    }
     else if (c2MethId == m_c2MethId_Flush)
    {
        mecvActual.strmTarget(meOwner).Flush();
    }
     else if (c2MethId == m_c2MethId_FmtBool)
    {
        mecvActual.strmTarget(meOwner) << meOwner.bStackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtCard1)
    {
        mecvActual.strmTarget(meOwner) << meOwner.c1StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtCard2)
    {
        mecvActual.strmTarget(meOwner) << meOwner.c2StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtCard4)
    {
        mecvActual.strmTarget(meOwner) << meOwner.c4StackValAt(c4FirstInd);
    }
     else if ((c2MethId == m_c2MethId_FmtCard1R)
          ||  (c2MethId == m_c2MethId_FmtCard2R)
          ||  (c2MethId == m_c2MethId_FmtCard4R))
    {
        // Get the value out into a Card4
        tCIDLib::TCard4 c4Val;
        if (c2MethId == m_c2MethId_FmtCard1R)
            c4Val = meOwner.c1StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_FmtCard2R)
            c4Val = meOwner.c2StackValAt(c4FirstInd);
        else
            c4Val = meOwner.c4StackValAt(c4FirstInd);

        // Get the radix enum value out and into the C++ form
        const TMEngEnumVal& mecvRadix
        (
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1)
        );
        const TMEngEnumInfo& meciRadix = meOwner.meciFindAs<TMEngEnumInfo>
        (
            m_c2EnumId_Radix
        );
        const tCIDLib::ERadices eRadix
        (
            tCIDLib::ERadices(meciRadix.c4MapValue(mecvRadix))
        );

        // And now we can do the format
        mecvActual.strmTarget(meOwner) << TCardinal(c4Val, eRadix);
    }
     else if (c2MethId == m_c2MethId_FmtCard8R)
    {
        // Get the radix enum value out and into the C++ form
        const TMEngEnumVal& mecvRadix
        (
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1)
        );
        const TMEngEnumInfo& meciRadix = meOwner.meciFindAs<TMEngEnumInfo>
        (
            m_c2EnumId_Radix
        );
        const tCIDLib::ERadices eRadix
        (
            tCIDLib::ERadices(meciRadix.c4MapValue(mecvRadix))
        );
        mecvActual.strmTarget(meOwner)
            << TCardinal64(meOwner.c8StackValAt(c4FirstInd), eRadix);
    }
     else if (c2MethId == m_c2MethId_FmtCard8)
    {
        mecvActual.strmTarget(meOwner) << meOwner.c8StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtChar)
    {
        mecvActual.strmTarget(meOwner) << meOwner.chStackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtCurLine)
    {
        mecvActual.strmTarget(meOwner) << meOwner.c4CurLine();
    }
     else if (c2MethId == m_c2MethId_FmtFloat4)
    {
        mecvActual.strmTarget(meOwner) << meOwner.f4StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtFloat8)
    {
        mecvActual.strmTarget(meOwner) << meOwner.f8StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtFmtable)
    {
        //
        //  This is a special case one. It takes any object derived from the
        //  formattable class. It just turns around and calls the object's
        //  FormatTo() method, passing ourself as the target stream.
        //
        TMEngClassVal& mecvFmt = meOwner.mecvStackAt(c4FirstInd);

        // Get the real class type of the target object
        TMEngClassInfo& meciTarget = meOwner.meciFind(mecvFmt.c2ClassId());

        // Generate a call to the object's FormatTo() method on the fly
        meOwner.PushPoolValue(tCIDMacroEng::EIntrinsics::Void, tCIDMacroEng::EConstTypes::Const);
        meOwner.PushValue(&mecvInstance, tCIDMacroEng::EStackItems::Parm, kCIDLib::True);
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
        meOwner.MultiPop(3);
    }
     else if (c2MethId == m_c2MethId_FmtInt1)
    {
        mecvActual.strmTarget(meOwner) << meOwner.i1StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtInt2)
    {
        mecvActual.strmTarget(meOwner) << meOwner.i2StackValAt(c4FirstInd);
    }
     else if (c2MethId == m_c2MethId_FmtInt4)
    {
        mecvActual.strmTarget(meOwner) << meOwner.i4StackValAt(c4FirstInd);
    }
     else if ((c2MethId == m_c2MethId_FmtInt1R)
          ||  (c2MethId == m_c2MethId_FmtInt2R)
          ||  (c2MethId == m_c2MethId_FmtInt4R))
    {
        // Get the value out into a Int4
        tCIDLib::TInt4 i4Val;
        if (c2MethId == m_c2MethId_FmtInt1R)
            i4Val = meOwner.i1StackValAt(c4FirstInd);
        else if (c2MethId == m_c2MethId_FmtInt2R)
            i4Val = meOwner.i2StackValAt(c4FirstInd);
        else
            i4Val = meOwner.i4StackValAt(c4FirstInd);

        // Now get the radix out
        const TMEngEnumVal& mecvRadix
        (
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1)
        );
        const TMEngEnumInfo& meciRadix = meOwner.meciFindAs<TMEngEnumInfo>
        (
            m_c2EnumId_Radix
        );
        const tCIDLib::ERadices eRadix
        (
            tCIDLib::ERadices(meciRadix.c4MapValue(mecvRadix))
        );

        // And now we can do the format
        mecvActual.strmTarget(meOwner) << TInteger(i4Val, eRadix);
    }
     else if (c2MethId == m_c2MethId_FmtString)
    {
        //
        //  This one could fail because the string contains character not
        //  representable in the target.
        //
        try
        {
            mecvActual.strmTarget(meOwner) << meOwner.strStackValAt(c4FirstInd);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_FmtErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_NewLine)
    {
        mecvActual.strmTarget(meOwner) << kCIDLib::NewLn;
    }
     else if (c2MethId == m_c2MethId_Reset)
    {
        mecvActual.strmTarget(meOwner).Reset();
    }
     else if (c2MethId == m_c2MethId_SeekToEnd)
    {
        mecvActual.strmTarget(meOwner).SeekToEnd();
    }
     else if (c2MethId == m_c2MethId_SetJust)
    {
        //
        //  Set the justification to the passed value. We use the mapping
        //  capability to map it back to the underlying C++ enum value
        //  which we can cast to that type.
        //
        const TMEngEnumVal& mecvJust
        (
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd)
        );
        mecvActual.strmTarget(meOwner).eJustification
        (
            tCIDLib::EHJustify(m_pmeciJustify->c4MapValue(mecvJust))
        );
    }
     else if (c2MethId == m_c2MethId_SetWidth)
    {
        // Set the width to the passed value
        mecvActual.strmTarget(meOwner).c4Width(meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TMEngTextOutStreamInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngTextOutStreamInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    ,       TError&             errCaught)
{
    // If in verbose mode we'll log the C++ error
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
        , m_pmeciErrs->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngConOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngConOutStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngConOutStreamInfo::TMEngConOutStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"ConsoleOutStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextOutStream"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
{
}

TMEngConOutStreamInfo::~TMEngConOutStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngConOutStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngConOutStreamInfo::Init(TCIDMacroEngine&)
{
    // Add the default ctor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.ConsoleOutStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngConOutStreamInfo::pmecvMakeStorage(const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Create one of the text out stream value object, and put our kind of
    //  of streams on it, and set it to our class id.
    //
    return new TMEngTextOutStreamVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngConOutStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngConOutStreamInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextOutStreamVal& mecvActual
                    = static_cast<TMEngTextOutStreamVal&>(mecvInstance);

    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        //
        //  Store the console stream. This is a special case one, in that
        //  the 'console' means different things according to where the macro
        //  is run, so this is provided by the engine, and we don't own it.
        //
        mecvActual.SetStream(meOwner.pstrmConsole(), tCIDLib::EAdoptOpts::NoAdopt);
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngFileOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFileOutStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFileOutStreamInfo::TMEngFileOutStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"FileOutStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextOutStream"
    )
    , m_c2MethId_Close(kCIDMacroEng::c2BadId)
    , m_c2EnumId_CreateAct(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_EncodingCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Open(kCIDMacroEng::c2BadId)
    , m_strDotDot(L"..")
{
    // Add imports for any non-intrinsic classes we use in our signatures
    bAddClassImport(TMEngFileSysInfo::strPath());
}

TMEngFileOutStreamInfo::~TMEngFileOutStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngFileOutStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngFileOutStreamInfo::Init(TCIDMacroEngine& meOwner)
{
    // Look up the creation action class' id and keep it around
    m_c2EnumId_CreateAct = meOwner.c2FindClassId(TMEngBaseInfoInfo::strCreateActsPath());

    // Close the file
    {
        TMEngMethodInfo methiNew
        (
            L"Close"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Close = c2AddMethodInfo(methiNew);
    }

    // Add the default ctor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.FileOutStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the ctor that takes an encoding
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.FileOutStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Encoding", tCIDMacroEng::EIntrinsics::String);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_EncodingCtor = c2AddMethodInfo(methiNew);
    }

    // Open the file
    {
        TMEngMethodInfo methiNew
        (
            L"Open"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FilePath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"CreateAction", m_c2EnumId_CreateAct);
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngFileOutStreamInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Create one of the text out stream value object, and put our kind of
    //  of streams on it, and set it to our class id.
    //
    return new TMEngTextOutStreamVal(strName, c2Id(), eConst);
}



// ---------------------------------------------------------------------------
//  TMEngFileOutStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFileOutStreamInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextOutStreamVal& mecvActual
                    = static_cast<TMEngTextOutStreamVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    //
    //  All these methods don't require the stream to be set, since they are
    //  just involved in setting it or cleaning it up. Our base class will
    //  insure the stream is set for any of his methods.
    //
    if (c2MethId == m_c2MethId_Close)
    {
        if (mecvActual.bIsSet())
            static_cast<TTextFileOutStream&>(mecvActual.strmTarget(meOwner)).Close();
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Set a default USASCII encoding
        mecvActual.strEncoding(L"USASCII");
    }
     else if (c2MethId == m_c2MethId_EncodingCtor)
    {
        // Store the passed encoding to use
        mecvActual.strEncoding(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Open)
    {
        // We throw file system errore here, so look up that class
        TMEngFileSysInfo* pmeciFileSys = meOwner.pmeciFindAs<TMEngFileSysInfo>
        (
            TMEngFileSysInfo::strPath()
            , kCIDLib::True
        );

        //
        //  The first parm is the path. It's a CML file path, so it's relative
        //  to a magic root directory that has to be mapped to a local path.
        //  The engine provides a method for this that will expand it, based
        //  on the root file path set one the engine.
        //
        //
        const TString& strPath = meOwner.strStackValAt(c4FirstInd);

        //
        //  The path must start with a slash and cannot have any .. items in
        //  it, i.e. it must be a fully qualified path. This prevents any
        //  attempts to .. themselves up above the controlled root imposed
        //  by the macro engine.
        //
        tCIDLib::TCard4 c4Pos;
        if (strPath.bIsEmpty()
        ||  (strPath.chFirst() != L'\\')
        ||  strPath.bFirstOccurrence(m_strDotDot, c4Pos))
        {
            pmeciFileSys->ThrowPathNotFQ(meOwner, strClassPath());
        }

        try
        {
            // Expand it to a real local path
            TPathStr pathActual;
            meOwner.ExpandFilePath(strPath, pathActual);

            //
            //  The second parm is the create action. We get the enum param
            //  value from the staci and look up the class info so that we can
            //  do the mapping from CML value to C++ value.
            //
            const TMEngEnumVal& mecvAct
            (
                meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1)
            );
            const TMEngEnumInfo& meciAct = meOwner.meciFindAs<TMEngEnumInfo>(m_c2EnumId_CreateAct);

            //
            //  Create and store the output stream. Create a text encoder based
            //  on the encoding set on this object.
            //
            mecvActual.SetStream
            (
                new TTextFileOutStream
                (
                    pathActual
                    , tCIDLib::ECreateActs(meciAct.c4MapValue(mecvAct))
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Write
                    , facCIDEncode().ptcvtMake(mecvActual.strEncoding())
                )
                , tCIDLib::EAdoptOpts::Adopt
            );
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            pmeciFileSys->ThrowOpenFailed(meOwner, strClassPath(), strPath);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngStringOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringOutStreamInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStringOutStreamInfo::strPath()
{
    return CIDMacroEng_OuptutStreamClasses::strStringOutPath;
}


// ---------------------------------------------------------------------------
//  TMEngStringOutStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringOutStreamInfo::TMEngStringOutStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"StringOutStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextOutStream"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetText(kCIDMacroEng::c2BadId)
    , m_c2MethId_SizeCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_XcodeTo(kCIDMacroEng::c2BadId)
    , m_c4ErrId_Xcode(kCIDLib::c4MaxCard)
{
}

TMEngStringOutStreamInfo::~TMEngStringOutStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringOutStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStringOutStreamInfo::Init(TCIDMacroEngine& meOwner)
{
    // Create an enum for our errors.
    {
        m_pmeciErrs = new TMEngEnumInfo
        (
            meOwner
            , L"StrOutStreamErrs"
            , strClassPath()
            , L"MEng.Enum"
            , 1
        );
        m_c4ErrId_Xcode = m_pmeciErrs->c4AddEnumItem(L"XCodeErr", TString::strEmpty());
        m_pmeciErrs->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrs);
        bAddNestedType(m_pmeciErrs->strClassPath());
    }

    // Add the default ctor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.StringOutStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the text getter method
    {
        TMEngMethodInfo methiNew
        (
            L"GetText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetText = c2AddMethodInfo(methiNew);
    }

    // Add the ctor that takes an initial size
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.StringOutStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitSize", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SizeCtor = c2AddMethodInfo(methiNew);
    }

    //
    //  Transcode the current stream contents to the target memory buffer,
    //  using the indicated encoding.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"XcodeTo"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"Encoding", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_XcodeTo = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngStringOutStreamInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Create one of the text out stream value object, and put our kind of
    //  of streams on it, and set it to our class id.
    //
    return new TMEngTextOutStreamVal(strName, c2Id(), eConst);
}



// ---------------------------------------------------------------------------
//  TMEngStringOutStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringOutStreamInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextOutStreamVal& mecvActual
                    = static_cast<TMEngTextOutStreamVal&>(mecvInstance);

    // Get the first parm index stack index and the method id
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Create and store the output stream
        mecvActual.SetStream
        (
            new TTextStringOutStream(4192)
            , tCIDLib::EAdoptOpts::Adopt
        );
    }
     else if (c2MethId == m_c2MethId_GetText)
    {
        // Flush it to make sure the data is all in the string
        mecvActual.Flush();

        TMEngStringVal& mecvFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvFill.strValue
        (
            static_cast<TTextStringOutStream&>(mecvActual.strmTarget(meOwner)).strData()
        );
    }
     else if (c2MethId == m_c2MethId_SizeCtor)
    {
        mecvActual.SetStream
        (
            new TTextStringOutStream(meOwner.c4StackValAt(c4FirstInd))
            , tCIDLib::EAdoptOpts::Adopt
        );
    }
     else if (c2MethId == m_c2MethId_XcodeTo)
    {
        try
        {
            // Get the buffer to fill
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);

            // Get the encoding and create a text converter
            TTextConverter* ptcvtToUse = facCIDEncode().ptcvtMake
            (
                meOwner.strStackValAt(c4FirstInd + 1)
            );
            TJanitor<TTextConverter> janCvt(ptcvtToUse);

            //
            //  Now flush the stream to make sure all the data is out there
            //  then do the conversion and store the bytes we output.
            //
            TMEngCard4Val& mecvCount = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvActual.Flush();

            tCIDLib::TCard4 c4OutBytes;
            ptcvtToUse->c4ConvertTo
            (
                static_cast<TTextStringOutStream&>(mecvActual.strmTarget(meOwner)).strData()
                , mecvToFill.mbufValue()
                , c4OutBytes
            );
            mecvCount.c4Value(c4OutBytes);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Xcode, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngStringOutStreamInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStringOutStreamInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    , const TString&            strText)
{
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrs->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

