//
// FILE NAME: CIDMacroEng_MiscClasses.cpp
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement a few miscellaneous fundamental intrinsic
//  types.
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
RTTIDecls(TMEngExceptInfo,TMEngClassInfo)
RTTIDecls(TMEngExceptVal,TMEngClassVal)
RTTIDecls(TMEngFormattableVal,TMEngClassVal)
RTTIDecls(TMEngFormattableInfo,TMEngClassInfo)
RTTIDecls(TMEngObjectInfo,TMEngClassInfo)
RTTIDecls(TMEngVoidVal,TMEngClassVal)
RTTIDecls(TMEngVoidInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_MiscClasses
{
    // -----------------------------------------------------------------------
    //  Make the FormatTo() method id available statically as an optimization,
    //  since lots of classes derive from the formattable class and override
    //  this. We'll set this when the formattable class is initialized.
    // -----------------------------------------------------------------------
    tCIDLib::TCard2 c2FormatToId;


    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strException(L"Exception");
    const TString   strExceptClassPath(L"MEng.Exception");
    const TString   strFormattable(L"Formattable");
    const TString   strFormattableClassPath(L"MEng.Formattable");
    const TString   strObject(L"Object");
    const TString   strObjectClassPath(L"MEng.Object");
    const TString   strVoid(L"Void");
    const TString   strVoidClassPath(L"MEng.Void");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngExceptVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngExceptVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngExceptVal::TMEngExceptVal( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Exception), eConst)
    , m_c2ErrClassId(kMacroEng::c2BadId)
    , m_c4ErrorNum(kCIDLib::c4MaxCard)
    , m_c4LineNum(0)
{
}

TMEngExceptVal::~TMEngExceptVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngExceptVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngExceptVal::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&     meciThis
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget  << L"Class: " << m_strSrcClassPath
                    << L"\nError: " << m_strErrorName
                    << L"\nError Text: " << m_strErrorText
                    << L"\nLine: " << m_c4LineNum;
    }
     else
    {
        strmTarget << m_strErrorText;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngExceptVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngExceptVal::bCheckError(const   tCIDLib::TCard2 c2ErrClassId
                            , const tCIDLib::TCard4 c4ErrToCheck) const
{
    return (c2ErrClassId == m_c2ErrClassId) && (c4ErrToCheck == m_c4ErrorNum);
}


tCIDLib::TBoolean
TMEngExceptVal::bCheckGreater(  const   tCIDLib::TCard2 c2ErrClassId
                                , const tCIDLib::TCard4 c4ErrToCheck) const
{
    return (c2ErrClassId == m_c2ErrClassId) && (m_c4ErrorNum > c4ErrToCheck);
}


tCIDLib::TCard4 TMEngExceptVal::c4ErrorNum() const
{
    return m_c4ErrorNum;
}


tCIDLib::TCard2 TMEngExceptVal::c2ErrClassId() const
{
    return m_c2ErrClassId;
}


tCIDLib::TCard4 TMEngExceptVal::c4LineNum() const
{
    return m_c4LineNum;
}


tCIDLib::TVoid TMEngExceptVal::Reset()
{
    m_c2ErrClassId  = kMacroEng::c2BadId;
    m_c4ErrorNum    = kCIDLib::c4MaxCard;
    m_c4LineNum     = 0;
    m_strSrcClassPath.Clear();
    m_strErrorName.Clear();
    m_strErrorText.Clear();
}


const TString& TMEngExceptVal::strErrorName() const
{
    return m_strErrorName;
}


const TString& TMEngExceptVal::strErrorText() const
{
    return m_strErrorText;
}


const TString& TMEngExceptVal::strSrcClassPath() const
{
    return m_strSrcClassPath;
}


tCIDLib::TVoid
TMEngExceptVal::Set(const   tCIDLib::TCard2 c2ErrClassId
                    , const TString&        strSrcClassPath
                    , const tCIDLib::TCard4 c4ErrorNum
                    , const TString&        strErrorName
                    , const TString&        strErrorText
                    , const tCIDLib::TCard4 c4LineNum)
{
    m_c2ErrClassId      = c2ErrClassId;
    m_c4ErrorNum        = c4ErrorNum;
    m_c4LineNum         = c4LineNum;
    m_strErrorName      = strErrorName;
    m_strErrorText      = strErrorText;
    m_strSrcClassPath   = strSrcClassPath;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngExceptInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngExceptInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngExceptInfo::strPath()
{
    return CIDMacroEng_MiscClasses::strExceptClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngExceptInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngExceptInfo::TMEngExceptInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MiscClasses::strException
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Check(kMacroEng::c2BadId)
    , m_c2MethId_CheckGreater(kMacroEng::c2BadId)
    , m_c2MethId_GetClass(kMacroEng::c2BadId)
    , m_c2MethId_GetErrorNum(kMacroEng::c2BadId)
    , m_c2MethId_GetErrorText(kMacroEng::c2BadId)
    , m_c2MethId_GetLine(kMacroEng::c2BadId)
{
}

TMEngExceptInfo::~TMEngExceptInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngExceptInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngExceptInfo::Init(TCIDMacroEngine& meOwner)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Exception"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the Check() method
    {
        TMEngMethodInfo methiNew
        (
            L"Check"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Error", tCIDMacroEng::EIntrinsics::Enum);
        m_c2MethId_Check = c2AddMethodInfo(methiNew);
    }

    // Add the CheckGreater() method
    {
        TMEngMethodInfo methiNew
        (
            L"CheckGreater"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Error", tCIDMacroEng::EIntrinsics::Enum);
        m_c2MethId_CheckGreater = c2AddMethodInfo(methiNew);
    }

    // Add the GetClass() method
    {
        TMEngMethodInfo methiNew
        (
            L"GetClass"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetClass = c2AddMethodInfo(methiNew);
    }

    // Get the error number, the ordinal of the enum
    {
        TMEngMethodInfo methiNew
        (
            L"GetErrorNum"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetErrorNum = c2AddMethodInfo(methiNew);
    }

    // Get the error text
    {
        TMEngMethodInfo methiNew
        (
            L"GetErrorText"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetErrorText = c2AddMethodInfo(methiNew);
    }

    // Add the GetLine() method
    {
        TMEngMethodInfo methiNew
        (
            L"GetLine"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLine = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngExceptInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngExceptVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngExceptInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngExceptInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInst)
{
    // Cast to our actual type
    TMEngExceptVal& mecvActual = static_cast<TMEngExceptVal&>(mecvInst);

    // Get the index of the first parameter
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        mecvActual.Reset();
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner)  << L"Class: " << mecvActual.strSrcClassPath()
                                        << L"\nName: " << mecvActual.strErrorName()
                                        << L"\nText: " << mecvActual.strErrorText()
                                        << L"\nLine: " << mecvActual.c4LineNum();
    }
     else if ((c2MethId == m_c2MethId_Check)
          ||  (c2MethId == m_c2MethId_CheckGreater))
    {
        // Get the class to check and the error to check
        const TMEngEnumVal& mecvError
        (
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd)
        );
        TMEngBooleanVal& mecvRet
        (
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
        );

        if (c2MethId == m_c2MethId_Check)
        {
            mecvRet.bValue
            (
                mecvActual.bCheckError(mecvError.c2ClassId(), mecvError.c4Ordinal())
            );
        }
         else
         {
            mecvRet.bValue
            (
                mecvActual.bCheckGreater(mecvError.c2ClassId(), mecvError.c4Ordinal())
            );
         }
    }
     else if (c2MethId == m_c2MethId_GetClass)
    {
        TMEngStringVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strSrcClassPath());
    }
     else if (c2MethId == m_c2MethId_GetErrorNum)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4ErrorNum());
    }
     else if (c2MethId == m_c2MethId_GetErrorText)
    {
        TMEngStringVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strErrorText());
    }
     else if (c2MethId == m_c2MethId_GetLine)
    {
        TMEngCard4Val& mecvRet
                = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4LineNum());
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFormattableVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFormattableVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFormattableVal::
TMEngFormattableVal(const   TString&                strName
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Formattable), eConst)
{
}

TMEngFormattableVal::~TMEngFormattableVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngFormattableVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngFormattableVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                                ,       TCIDMacroEngine&    meOwner)
{
    //
    //  We don't do anything. We can be called but we have no memebers and
    //  we know our parent (MEng.Object) doesn't.
    //
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFormattableInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFormattableInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngFormattableInfo::c2FormatToId()
{
    return CIDMacroEng_MiscClasses::c2FormatToId;
}


const TString& TMEngFormattableInfo::strPath()
{
    return CIDMacroEng_MiscClasses::strFormattableClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngFormattableInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFormattableInfo::TMEngFormattableInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MiscClasses::strFormattable
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::NonFinal
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
{
}

TMEngFormattableInfo::~TMEngFormattableInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngFormattableInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngFormattableInfo::Init(TCIDMacroEngine&)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Formattable"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    //
    //  Add the non-final, required method that takes an output text stream.
    //  The derived class must use this to format itself to the stream. The
    //  stream must be inout, since it will be modified by the output.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"FormatTo"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Required
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInOutParm(L"TargetStream", tCIDMacroEng::EIntrinsics::TextOutStream);
        CIDMacroEng_MiscClasses::c2FormatToId = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngFormattableInfo::pmecvMakeStorage( const   TString&
                                        ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes) const
{
    // Throw because this isn't legal
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcClass_ClassHasNoValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
        , CIDMacroEng_MiscClasses::strFormattableClassPath
    );

    // Make the compiler happy
    return 0;
}


// ---------------------------------------------------------------------------
//  TMEngFormattableInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFormattableInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    //
    //  For the default ctor, do nothing but say we handled it. Anything else
    //  is an error since we provide no other methods at this level.
    //
    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
        return kCIDLib::True;
    return kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngObjectInfoInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngObjectInfoInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngObjectInfo::strPath()
{
    return CIDMacroEng_MiscClasses::strObjectClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngObjectInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngObjectInfo::TMEngObjectInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MiscClasses::strObject
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::NonFinal
        , kCIDMacroEng_::pszNoParentClass
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
{
}

TMEngObjectInfo::~TMEngObjectInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngObjectInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngObjectInfo::Init(TCIDMacroEngine&)
{
    // Add our methods. We just have a default ctor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Object"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngObjectInfo::pmecvMakeStorage(  const   TString&
                                    ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes) const
{
    // Throw because this isn't legal
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcClass_ClassHasNoValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
        , CIDMacroEng_MiscClasses::strObjectClassPath
    );

    // Make the compiler happy
    return 0;
}


// ---------------------------------------------------------------------------
//  TMEngObjectInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngObjectInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    //
    //  For the default ctor, do nothing but say we handled it. Anything else
    //  is an error since we provide no other methods at this level.
    //
    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
        return kCIDLib::True;
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngVoidVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngVoidVal: Contsructors and Destructor
// ---------------------------------------------------------------------------
TMEngVoidVal::TMEngVoidVal( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void), eConst)
{
}

TMEngVoidVal::~TMEngVoidVal()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngVoidInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngVoidInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngVoidInfo::strPath()
{
    return CIDMacroEng_MiscClasses::strVoidClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngVoidInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngVoidInfo::TMEngVoidInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MiscClasses::strVoid
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
{
}

TMEngVoidInfo::~TMEngVoidInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngVoidInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngVoidInfo::Init(TCIDMacroEngine&)
{
}


TMEngClassVal*
TMEngVoidInfo::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngVoidVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngVoidInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngVoidInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    // We have no methods ourself, just a value
    return kCIDLib::False;
}


