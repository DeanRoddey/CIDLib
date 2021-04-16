//
// FILE NAME: CIDMacroEng_InputStreamClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/10/2003
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
//  value classes, which implement a base input text stream class, and some
//  derivatives for files, console, and so on.
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
RTTIDecls(TMEngTextInStreamVal,TMEngClassVal)
RTTIDecls(TMEngTextInStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngConInStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngFileInStreamInfo,TMEngClassInfo)
RTTIDecls(TMEngStringInStreamInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMacroEng_InputStreamClasses
{
    namespace
    {
        const TString   strTextInStreamClassPath(L"MEng.System.Runtime.TextInStream");
        const TString   strConsoleInStreamClassPath(L"MEng.System.Runtime.ConsoleInStream");
        const TString   strFileInStreamClassPath(L"MEng.System.Runtime.FileInStream");
        const TString   strStringInStreamClassPath(L"MEng.System.Runtime.StringInStream");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextInStreamVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextInStreamVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngTextInStreamVal::
TMEngTextInStreamVal(const  TString&                strName
                    , const tCIDLib::TCard2         c2ClassId
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
    , m_pstrmSrc(0)
    , m_strEncoding(L"USASCII")
{
}

TMEngTextInStreamVal::~TMEngTextInStreamVal()
{
    delete m_pstrmSrc;
}


// ---------------------------------------------------------------------------
//  TMEngTextInStreamVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngTextInStreamVal::bIsSet() const
{
    return (m_pstrmSrc != 0);
}


//
//  Get or set the encoding to use. It makes no sense to set it after the
//  stream is created.
//
const TString& TMEngTextInStreamVal::strEncoding() const
{
    return m_strEncoding;
}

const TString& TMEngTextInStreamVal::strEncoding(const TString& strToSet)
{
    m_strEncoding = strToSet;
    return m_strEncoding;
}


const TString& TMEngTextInStreamVal::strExtra() const
{
    return m_strExtra;
}

const TString& TMEngTextInStreamVal::strExtra(const TString& strToSet)
{
    m_strExtra = strToSet;
    return m_strExtra;
}


// Provide access to the source stream
const TTextInStream&
TMEngTextInStreamVal::strmSrc(const TCIDMacroEngine& meOwner) const
{
    // If not set up yet, then that's an error
    CheckStreamSet(meOwner);
    return *m_pstrmSrc;
}

TTextInStream& TMEngTextInStreamVal::strmSrc(const TCIDMacroEngine& meOwner)
{
    // If not set up yet, then that's an error
    CheckStreamSet(meOwner);
    return *m_pstrmSrc;
}


// Set a new underlying stream object
tCIDLib::TVoid
TMEngTextInStreamVal::SetStream(TTextInStream* const pstrmToAdopt)
{
    if (m_pstrmSrc)
    {
        try
        {
            delete m_pstrmSrc;
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // We are responsible but never got it set, so clean it up
            delete pstrmToAdopt;
            throw;
        }
    }
    m_pstrmSrc = pstrmToAdopt;

    //
    //  Make sure it's in throw mode. The normal default is Stop/Throw, but
    //  that is not a useful mode for streams, so our CML level enum does
    //  not expose it, so we it would fail if they tried to read the mode
    //  at any point.
    //
    m_pstrmSrc->tcvtThis().eErrorAction(tCIDLib::ETCvtActs::Throw);
}


// ---------------------------------------------------------------------------
//  TMEngTextInStreamVal: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngTextInStreamVal::CheckStreamSet(const TCIDMacroEngine& meOwner) const
{
    if (!m_pstrmSrc)
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
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngTextInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTextInStreamInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngTextInStreamInfo::strPath()
{
    return CIDMacroEng_InputStreamClasses::strTextInStreamClassPath;
}



// ---------------------------------------------------------------------------
//  TMEngTextInStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTextInStreamInfo::TMEngTextInStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"TextInStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Abstract
        , L"MEng.Object"

    )
    , m_c2MethId_EndOfStream(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_Reset(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetErrAction(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetRepChar(kCIDMacroEng::c2BadId)
    , m_c2TypeId_Errors(kCIDMacroEng::c2BadId)
    , m_c2TypeId_InErrActs(kCIDMacroEng::c2BadId)
    , m_c4ErrId_Configure(kCIDMacroEng::c2BadId)
    , m_c4ErrId_Internalize(kCIDLib::c4MaxCard)
    , m_c4ErrId_Reset(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciInErrActs(0)
{
}

TMEngTextInStreamInfo::~TMEngTextInStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngTextInStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngTextInStreamInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"InpStrmErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_c4ErrId_Internalize = m_pmeciErrors->c4AddEnumItem(L"Internalize", TString::strEmpty());
        m_c4ErrId_Reset = m_pmeciErrors->c4AddEnumItem(L"Reset", TString::strEmpty());
        m_c4ErrId_Configure = m_pmeciErrors->c4AddEnumItem(L"Configure", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum to represent the input conversion error actions
    {
        m_pmeciInErrActs = new TMEngEnumInfo
        (
            meOwner
            , L"InErrActs"
            , strClassPath()
            , L"MEng.Enum"
            , 2
        );
        m_pmeciInErrActs->c4AddEnumItem(L"Throw", L"Throw on Error", tCIDLib::ETCvtActs::Throw);
        m_pmeciInErrActs->c4AddEnumItem(L"RepChar", L"Use replacement character", tCIDLib::ETCvtActs::Replace);
        m_pmeciInErrActs->BaseClassInit(meOwner);
        m_c2TypeId_InErrActs = meOwner.c2AddClass(m_pmeciInErrActs);
        bAddNestedType(m_pmeciInErrActs->strClassPath());
    }

    // Check for end of stream
    {
        TMEngMethodInfo methiNew
        (
            L"EndOfStream"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_EndOfStream = c2AddMethodInfo(methiNew);
    }

    // Format a Boolean
    {
        TMEngMethodInfo methiNew
        (
            L"GetLine"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetLine = c2AddMethodInfo(methiNew);
    }

    // Reset the stream back to the start
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

    // Set the error actions
    {
        TMEngMethodInfo methiNew
        (
            L"SetErrAction"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_InErrActs);
        m_c2MethId_SetErrAction = c2AddMethodInfo(methiNew);
    }

    // Set the replace character on the text converter
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
TMEngTextInStreamInfo::pmecvMakeStorage(const  TString&
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
//  TMEngTextInStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTextInStreamInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextInStreamVal& mecvActual
                    = static_cast<TMEngTextInStreamVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    // These methods all require that the stream be set
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

    if (methiTarget.c2Id() == m_c2MethId_EndOfStream)
    {
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            mecvRet.bValue(mecvActual.strmSrc(meOwner).bEndOfStream());
        }

        catch(const TError& errToCatch)
        {
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
            ThrowAnErr(meOwner, m_c4ErrId_Internalize, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetLine)
    {
        // Get the string parm and load it with the next line
        TMEngStringVal& mecvToFill
                = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        try
        {
            mecvActual.strmSrc(meOwner).c4GetLine(mecvToFill.strValue());
        }

        catch(const TError& errToCatch)
        {
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
            ThrowAnErr(meOwner, m_c4ErrId_Internalize, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_Reset)
    {
        try
        {
            mecvActual.strmSrc(meOwner).Reset();
        }

        catch(const TError& errToCatch)
        {
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
            ThrowAnErr(meOwner, m_c4ErrId_Reset, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetErrAction)
    {
        try
        {
            TMEngEnumVal& mecvAct = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            mecvActual.strmSrc(meOwner).tcvtThis().eErrorAction
            (
                tCIDLib::ETCvtActs(m_pmeciInErrActs->c4MapValue(mecvAct))
            );
        }

        catch(const TError& errToCatch)
        {
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
            ThrowAnErr(meOwner, m_c4ErrId_Configure, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetRepChar)
    {
        try
        {
            mecvActual.strmSrc(meOwner).tcvtThis().chRepChar(meOwner.chStackValAt(c4FirstInd));
        }

        catch(const TError& errToCatch)
        {
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
            ThrowAnErr(meOwner, m_c4ErrId_Configure, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngTextInStreamInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngTextInStreamInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
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




// ---------------------------------------------------------------------------
//  CLASS: TMEngConInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngConInStreamInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngConInStreamInfo::strPath()
{
    return CIDMacroEng_InputStreamClasses::strConsoleInStreamClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngConInStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngConInStreamInfo::TMEngConInStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"ConsoleInStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextInStream"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
{
}

TMEngConInStreamInfo::~TMEngConInStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngConInStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngConInStreamInfo::Init(TCIDMacroEngine&)
{
    //
    //  We add some extra methods related to this class, above and beyond the
    //  base input stream.
    //

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.ConsoleInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngConInStreamInfo::pmecvMakeStorage( const   TString&                strName
                                        ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Create one of the text out stream value object, and put our kind of
    //  of streams on it, and set it to our class id.
    //
    return new TMEngTextInStreamVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngConInStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngConInStreamInfo::bInvokeMethod(       TCIDMacroEngine&
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextInStreamVal& mecvActual
                    = static_cast<TMEngTextInStreamVal&>(mecvInstance);

    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Create and set the stream, and empty the extra field
        mecvActual.SetStream(new TInConsole);
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFileInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFileInStreamInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngFileInStreamInfo::strPath()
{
    return CIDMacroEng_InputStreamClasses::strFileInStreamClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngFileInStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFileInStreamInfo::TMEngFileInStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"FileInStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextInStream"
    )
    , m_c2MethId_Close(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_EncCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetFileName(kCIDMacroEng::c2BadId)
    , m_c2MethId_Open(kCIDMacroEng::c2BadId)
    , m_strDotDot(L"..")
{
    // Add imports for any non-intrinsic classes we use in our signatures
    bAddClassImport(TMEngFileSysInfo::strPath());
}

TMEngFileInStreamInfo::~TMEngFileInStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngFileInStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngFileInStreamInfo::Init(TCIDMacroEngine&)
{
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
            L"ctor1_MEng.System.Runtime.FileInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that provides an encoding
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.FileInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"Encoding", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_EncCtor = c2AddMethodInfo(methiNew);
    }

    // Get the file name
    {
        TMEngMethodInfo methiNew
        (
            L"GetFileName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetFileName = c2AddMethodInfo(methiNew);
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
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngFileInStreamInfo::pmecvMakeStorage(const   TString&                strName
                                        ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Create one of the text out stream value object, and put our kind of
    //  of streams on it, and set it to our class id.
    //
    return new TMEngTextInStreamVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngFileInStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFileInStreamInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextInStreamVal& mecvActual
                    = static_cast<TMEngTextInStreamVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    if (methiTarget.c2Id() == m_c2MethId_Close)
    {
        if (mecvActual.bIsSet())
            static_cast<TTextFileInStream&>(mecvActual.strmSrc(meOwner)).Close();
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Set the encoding to the default USASCII, and clear the name
        mecvActual.strEncoding(L"USASCII");
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else if (methiTarget.c2Id() == m_c2MethId_EncCtor)
    {
        // Set the passed encoding and clear the name
        mecvActual.strEncoding(meOwner.strStackValAt(c4FirstInd));
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetFileName)
    {
        // Get the return value object, and set it
        TMEngStringVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        if (mecvActual.bIsSet())
            mecvRet.strValue(mecvActual.strExtra());
        else
            mecvRet.strValue().Clear();
    }
     else if (methiTarget.c2Id() == m_c2MethId_Open)
    {
        // Get the file path parm
        const TString& strPath = meOwner.strStackValAt(c4FirstInd);

        // We throw file system errore here, so look up that class
        TMEngFileSysInfo* pmeciFileSys = meOwner.pmeciFindAs<TMEngFileSysInfo>
        (
            TMEngFileSysInfo::strPath(), kCIDLib::True
        );

        //
        //  The path must start with a slash and cannot have any .. items in
        //  it, i.e. it must be a fully qualified path. This prevents any
        //  attempts to .. themselves up above the controlled root imposed
        //  by the macro engine.
        //
        tCIDLib::TCard4 c4Pos = 0;
        if ((strPath.chFirst() != L'\\')
        ||  strPath.bFirstOccurrence(m_strDotDot, c4Pos))
        {
            pmeciFileSys->ThrowPathNotFQ(meOwner, strClassPath());
        }

        // Let's try to open the file
        try
        {
            // Ask the macro engine to expand the file name to a real file
            TPathStr pathFile;
            meOwner.ExpandFilePath(strPath, pathFile);

            //
            //  Create and store the stream, creating a text converter
            //  of the type indicated by the set encoding.
            //
            mecvActual.SetStream
            (
                new TTextFileInStream
                (
                    pathFile
                    , tCIDLib::ECreateActs::OpenIfExists
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::None
                    , tCIDLib::EAccessModes::Read
                    , facCIDEncode().ptcvtMake(mecvActual.strEncoding())
                )
            );

            //
            //  Ok, we were successful, so store the file name (the
            //  file name at our level, not the real file name.)
            //
            mecvActual.strExtra(strPath);
        }

        catch(TError& errToCatch)
        {
            // If in verbose mode we'll log the C++ error
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
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
//  CLASS: TMEngStringInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringInStreamInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStringInStreamInfo::strPath()
{
    return CIDMacroEng_InputStreamClasses::strStringInStreamClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngStringInStreamInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringInStreamInfo::TMEngStringInStreamInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"StringInStream"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.System.Runtime.TextInStream"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetText(kCIDMacroEng::c2BadId)
    , m_c2MethId_SyncCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_StrCtor(kCIDMacroEng::c2BadId)
{
}

TMEngStringInStreamInfo::~TMEngStringInStreamInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringInStreamInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStringInStreamInfo::Init(TCIDMacroEngine& meOwner)
{
    // Look up our sibling output stream class
    const tCIDLib::TCard2 c2ClassId_OutStrm = meOwner.meciFind
    (
        TMEngStringOutStreamInfo::strPath()
    ).c2Id();

    //
    //  We add some extra methods related to this class, above and beyond the
    //  base input stream.
    //

    // Default ctor, will set the actual text later
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.StringInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that syncs with an output string stream
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.StringInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"SyncWith", c2ClassId_OutStrm);
        m_c2MethId_SyncCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes some string content to stream from
    {
        TMEngMethodInfo methiNew
        (
            L"ctor3_MEng.System.Runtime.StringInStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"Contents", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_StrCtor = c2AddMethodInfo(methiNew);
    }

    // Set up new text content on an existing stream
    {
        TMEngMethodInfo methiNew
        (
            L"SetText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Contents", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetText = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngStringInStreamInfo::pmecvMakeStorage( const   TString&                strName
                                        ,       TCIDMacroEngine&
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngTextInStreamVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngStringInStreamInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringInStreamInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    // Cast the value object to it's actual type
    TMEngTextInStreamVal& mecvActual
                    = static_cast<TMEngTextInStreamVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Just set it up with an empty text string
        mecvActual.SetStream
        (
            new TTextStringInStream(new TString(L""), tCIDLib::EAdoptOpts::Adopt)
        );
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else if (methiTarget.c2Id() == m_c2MethId_SyncCtor)
    {
        //
        //  The parameter will be a text out stream value object, so get it
        //  as that type.
        //
        TMEngTextOutStreamVal& mecvOut
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);

        //
        //  Now create a new C++ text input stream, synced to the C++ output
        //  stream that is in the passed parameter.
        //
        mecvActual.SetStream
        (
            new TTextStringInStream
            (
                static_cast<TTextStringOutStream&>(mecvOut.strmTarget(meOwner))
            )
        );
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else if (methiTarget.c2Id() == m_c2MethId_StrCtor)
    {
        //
        //  Create and set the stream, and empty the extra field. The default
        //  is not to adopt the string, but we are allocating a copy of the
        //  caller's string, so we need it to adopt.
        //
        mecvActual.SetStream
        (
            new TTextStringInStream
            (
                new TString(meOwner.strStackValAt(c4FirstInd))
                , tCIDLib::EAdoptOpts::Adopt
            )
        );
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetText)
    {
        mecvActual.SetStream
        (
            new TTextStringInStream
            (
                new TString(meOwner.strStackValAt(c4FirstInd))
                , tCIDLib::EAdoptOpts::Adopt
            )
        );
        mecvActual.strExtra(kCIDLib::pszEmptyZStr);
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


