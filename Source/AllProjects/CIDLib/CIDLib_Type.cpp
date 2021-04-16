//
// FILE NAME: CIDLib_Type.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/27/1994
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
//  This file provides formatting for the facility specific enums from
//  CIDLib_Type.Hpp. Because CIDLib is special (i.e. the core platform
//  independent facility), its types and constants namespaces are actually
//  down in the kernel facility.
//
// CAVEATS/GOTCHAS:
//
//  1)  Some enums are very commonly understood and therefore will be used
//      in very non-software senses, such as boolean values. Therefore, the
//      text for those types are loaded dyanmically in order to support
//      translation for them. Others are very tied to the source code and
//      anyone expected to understand the code (which is in english) would
//      understand them in their english form.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_Type
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The arrays of message ids and hard coded text for respectively loading
        //  up the human readable text for and translating between text/binary
        //  formats of some of our enums.
        //
        //  Normally this stuff is provided by the IDL compiler, but we exist
        //  below the IDL compiler and cannot use it.
        // -----------------------------------------------------------------------
        constexpr const tCIDLib::TCh* const apszDirsVals[] =
        {
            L"Left"
            , L"Right"
            , L"Down"
            , L"Up"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EDirs, tCIDLib::EDirs::Count> apszDirs(apszDirsVals);


        constexpr tCIDLib::TMsgId amidErrClassVals[] =
        {
            kCIDMsgs::midGen_None
            , kCIDMsgs::midErrC_OutResource
            , kCIDMsgs::midErrC_TempSituation
            , kCIDMsgs::midErrC_Authority
            , kCIDMsgs::midErrC_Internal
            , kCIDMsgs::midErrC_HardwareError
            , kCIDMsgs::midErrC_SystemError
            , kCIDMsgs::midErrC_AppError
            , kCIDMsgs::midErrC_BadParms
            , kCIDMsgs::midErrC_Locked
            , kCIDMsgs::midErrC_Media
            , kCIDMsgs::midErrC_Already
            , kCIDMsgs::midErrC_Unknown
            , kCIDMsgs::midErrC_CantDo
            , kCIDMsgs::midErrC_TimeOut
            , kCIDMsgs::midErrC_DynamicType
            , kCIDMsgs::midErrC_TypeMatch
            , kCIDMsgs::midErrC_Format
            , kCIDMsgs::midErrC_UserInput
            , kCIDMsgs::midErrC_NotReady
            , kCIDMsgs::midErrC_ObjState
            , kCIDMsgs::midErrC_NotSupported
            , kCIDMsgs::midErrC_AppStatus
            , kCIDMsgs::midErrC_NotFound
            , kCIDMsgs::midErrC_Protocol
            , kCIDMsgs::midErrC_Range
            , kCIDMsgs::midErrC_Duplicate
            , kCIDMsgs::midErrC_Config
            , kCIDMsgs::midErrC_Index
            , kCIDMsgs::midErrC_NotAllRead
            , kCIDMsgs::midErrC_NotAllWritten
            , kCIDMsgs::midErrC_LostConnection
            , kCIDMsgs::midErrC_Overflow
            , kCIDMsgs::midErrC_Underflow
            , kCIDMsgs::midErrC_InitFailed
            , kCIDMsgs::midErrC_OutOfSync
            , kCIDMsgs::midErrC_XmitFailed
            , kCIDMsgs::midErrC_ReceiveFailed
            , kCIDMsgs::midErrC_NoReply
            , kCIDMsgs::midErrC_Assert
            , kCIDMsgs::midErrC_InUse
            , kCIDMsgs::midErrC_Term
            , kCIDMsgs::midErrC_Init
            , kCIDMsgs::midErrC_Shutdown
            , kCIDMsgs::midErrC_RejectedConn
            , kCIDMsgs::midErrC_Busy
            , kCIDMsgs::midErrC_Nak
        };
        const TEArray<tCIDLib::TMsgId, tCIDLib::EErrClasses, tCIDLib::EErrClasses::Count> amidErrClasses
        (
            amidErrClassVals
        );

        constexpr const tCIDLib::TCh* const apszErrClassVals[] =
        {
            L"None"
            , L"OutResource"
            , L"TempSituation"
            , L"Authority"
            , L"Internal"
            , L"HardwareFailure"
            , L"SystemFailure"
            , L"AppError"
            , L"BadParms"
            , L"Locked"
            , L"Media"
            , L"Already"
            , L"Unknown"
            , L"CantDo"
            , L"Timeout"
            , L"DynamicType"
            , L"TypeMatch"
            , L"Format"
            , L"UserInput"
            , L"NotReady"
            , L"ObjState"
            , L"NotSupported"
            , L"AppStatus"
            , L"NotFound"
            , L"Protocol"
            , L"Range"
            , L"Duplicate"
            , L"Config"
            , L"Index"
            , L"NotAllRead"
            , L"NotAllWritten"
            , L"LostConnection"
            , L"Overflow"
            , L"Underflow"
            , L"InitFailed"
            , L"OutOfSync"
            , L"XmitFailed"
            , L"ReceiveFailed"
            , L"NoReply"
            , L"Assert"
            , L"InUse"
            , L"Termination"
            , L"Initialization"
            , L"Shutdown"
            , L"RejectedConn"
            , L"Busy"
            , L"Nak"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EErrClasses, tCIDLib::EErrClasses::Count, 1> apszErrClasses
        (
            apszErrClassVals
        );


        constexpr tCIDLib::TMsgId amidHJustifications[tCIDLib::c4EnumOrd(tCIDLib::EHJustify::Count)] =
        {
            kCIDMsgs::midGen_Left
            , kCIDMsgs::midGen_Right
            , kCIDMsgs::midGen_Center
        };

        constexpr const tCIDLib::TCh* const apszHJustVals[] =
        {
            L"EHJustify_Left"
            , L"EHJustify_Right"
            , L"EHJustify_Center"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EHJustify, tCIDLib::EHJustify::Count, 1> apszHJustifications
        (
            apszHJustVals
        );

        constexpr const tCIDLib::TCh* const apszAltHJustVals[] =
        {
            L"Left"
            , L"Right"
            , L"Center"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EHJustify, tCIDLib::EHJustify::Count, 1> apszAltHJustifications
        (
            apszAltHJustVals
        );

        //
        //  These are added onto the overall stats cache path for the logging flags
        //  stats cache item. THESE MUST be in the same order as the enum, as all these
        //  lists are, but most important here since it will be used to translate from
        //  a text path to a stats cache index.
        //
        constexpr const tCIDLib::TCh* apszLogFlagVals[] =
        {
            L"CIDLib/Audio"
            , L"CIDLib/Base64"
            , L"CIDLib/BinStreams"
            , L"CIDLib/BitsBytes"
            , L"CIDLib/Collects"
            , L"CIDLib/Color"
            , L"CIDLib/Coords"
            , L"CIDLib/DataSrc"
            , L"CIDLib/Facility"
            , L"CIDLib/Files"
            , L"CIDLib/FileSystem"
            , L"CIDLib/FundCollects"
            , L"CIDLib/MemBuf"
            , L"CIDLib/Other"
            , L"CIDLib/Processes"
            , L"CIDLib/Speech"
            , L"CIDLib/SearchNSort"
            , L"CIDLib/Strings"
            , L"CIDLib/Synchro"
            , L"CIDLib/TextConvert"
            , L"CIDLib/TextStreams"
            , L"CIDLib/Threads"
            , L"CIDLib/ValFormat"
            , L"CIDLib/Volumes"
            , L"CIDLib/Trace"
            , L"CIDLib/Verbose"

            , L"CIDLib/Unknown"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::ELogFlags, tCIDLib::ELogFlags::Count, 1> apszLogFlags
        (
            apszLogFlagVals
        );

        constexpr tCIDLib::TMsgId amidLanguages[tCIDLib::c4EnumOrd(tCIDLib::ELanguages::Count)] =
        {
            kCIDMsgs::midLang_Afrikaans
            , kCIDMsgs::midLang_Albanian
            , kCIDMsgs::midLang_Arabic
            , kCIDMsgs::midLang_Basque
            , kCIDMsgs::midLang_Belarusian
            , kCIDMsgs::midLang_Bulgarian
            , kCIDMsgs::midLang_Catalan
            , kCIDMsgs::midLang_Chinese
            , kCIDMsgs::midLang_Croatian
            , kCIDMsgs::midLang_Czech
            , kCIDMsgs::midLang_Danish
            , kCIDMsgs::midLang_Dutch
            , kCIDMsgs::midLang_English
            , kCIDMsgs::midLang_Farsi
            , kCIDMsgs::midLang_Finnish
            , kCIDMsgs::midLang_French
            , kCIDMsgs::midLang_German
            , kCIDMsgs::midLang_Greek
            , kCIDMsgs::midLang_Hebrew
            , kCIDMsgs::midLang_Hungarian
            , kCIDMsgs::midLang_Icelandic
            , kCIDMsgs::midLang_Indonesian
            , kCIDMsgs::midLang_Italian
            , kCIDMsgs::midLang_Japanese
            , kCIDMsgs::midLang_Korean
            , kCIDMsgs::midLang_Latvian
            , kCIDMsgs::midLang_Lithuanian
            , kCIDMsgs::midLang_Norwegian
            , kCIDMsgs::midLang_Polish
            , kCIDMsgs::midLang_Portuguese
            , kCIDMsgs::midLang_Romanian
            , kCIDMsgs::midLang_Russian
            , kCIDMsgs::midLang_Serbian
            , kCIDMsgs::midLang_Slovak
            , kCIDMsgs::midLang_Slovenian
            , kCIDMsgs::midLang_Spanish
            , kCIDMsgs::midLang_Swedish
            , kCIDMsgs::midLang_Turkish
            , kCIDMsgs::midLang_Ukrainian
            , kCIDMsgs::midLang_Vietnamese
        };

        constexpr const tCIDLib::TCh* apszSevTextVals[] =
        {
            L"Info"
            , L"Warn"
            , L"Failed"
            , L"ProcFatal"
            , L"SysFatal"
            , L"Status"
            , L"Unknown"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::ESeverities, tCIDLib::ESeverities::Count, 1> apszSeveritiesTxt
        (
            apszSevTextVals
        );

        constexpr const tCIDLib::TCh* apszSeverVals[] =
        {
            L"ESev_Info"
            , L"ESev_Warn"
            , L"ESev_Failed"
            , L"ESev_ProcFatal"
            , L"ESev_SysFatal"
            , L"ESev_Status"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::ESeverities, tCIDLib::ESeverities::Count, 1> apszSeverities
        (
            apszSeverVals
        );

        constexpr tCIDLib::TMsgId amidVJustifications[tCIDLib::c4EnumOrd(tCIDLib::EVJustify::Count)] =
        {
            kCIDMsgs::midGen_Bottom
            , kCIDMsgs::midGen_Center
            , kCIDMsgs::midGen_Top
        };

        constexpr const tCIDLib::TCh* const apszVJustVals[] =
        {
            L"EVJustify_Bottom"
            , L"EVJustify_Center"
            , L"EVJustify_Top"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EVJustify, tCIDLib::EVJustify::Count, 1> apszVJustifications
        (
            apszVJustVals
        );

        constexpr const tCIDLib::TCh* const apszAltVJustVals[] =
        {
            L"Bottom"
            , L"Center"
            , L"Top"
            , L"???"
        };
        const TEArray<const tCIDLib::TCh*, tCIDLib::EVJustify, tCIDLib::EVJustify::Count, 1> apszAltVJustifications
        (
            apszAltVJustVals
        );


        // -----------------------------------------------------------------------
        //  Our list of text values for the enums we support loadable human text
        //  for. We fault them in upon use.
        // -----------------------------------------------------------------------
        TEArray<TString, tCIDLib::EErrClasses, tCIDLib::EErrClasses::Count> astrErrClasses(TString::strEmpty());
        TEArray<TString, tCIDLib::EHJustify, tCIDLib::EHJustify::Count> apstrHJustifications(TString::strEmpty());
        TEArray<TString, tCIDLib::ELanguages, tCIDLib::ELanguages::Count> apstrLanguages(TString::strEmpty());
        TEArray<TString, tCIDLib::ESeverities, tCIDLib::ESeverities::Count> apstrSeverities(TString::strEmpty());
        TEArray<TString, tCIDLib::EVJustify, tCIDLib::EVJustify::Count> apstrVJustifications(TString::strEmpty());
    }
}



// ---------------------------------------------------------------------------
//  Text to enum translation methods
// ---------------------------------------------------------------------------
tCIDLib::EDirs tCIDLib::eXlatEDirs(const TString& strToXlat)
{
    tCIDLib::EDirs eIndex = tCIDLib::EDirs::Min;
    for (; eIndex < tCIDLib::EDirs::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszDirs[eIndex]))
            return eIndex;
    }
    return tCIDLib::EDirs::Count;
}


tCIDLib::EErrClasses tCIDLib::eXlatEErrClasses(const TString& strToXlat)
{
    tCIDLib::EErrClasses eIndex = tCIDLib::EErrClasses::Min;
    for (; eIndex < tCIDLib::EErrClasses::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszErrClasses[eIndex]))
            return eIndex;
    }
    return tCIDLib::EErrClasses::Count;
}


tCIDLib::ELogFlags tCIDLib::eXlatELogFlags(const TString& strToXlat)
{
    tCIDLib::ELogFlags eIndex = tCIDLib::ELogFlags::Min;
    for (; eIndex < tCIDLib::ELogFlags::Max; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszLogFlags[eIndex]))
            return eIndex;
    }
    return tCIDLib::ELogFlags::Count;
}


tCIDLib::EHJustify tCIDLib::eXlatEHJustify(const TString& strToXlat)
{
    tCIDLib::EHJustify eIndex = tCIDLib::EHJustify::Min;
    for (; eIndex < tCIDLib::EHJustify::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszHJustifications[eIndex]))
            return eIndex;
    }
    return tCIDLib::EHJustify::Count;
}

tCIDLib::EHJustify tCIDLib::eAltXlatEHJustify(const TString& strToXlat)
{
    tCIDLib::EHJustify eIndex = tCIDLib::EHJustify::Min;
    for (; eIndex < tCIDLib::EHJustify::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszAltHJustifications[eIndex]))
            return eIndex;
    }
    return tCIDLib::EHJustify::Count;
}


tCIDLib::ERadices tCIDLib::eXlatERadices(const TString& strToXlat)
{
    if (strToXlat == L"Auto")
        return tCIDLib::ERadices::Auto;
    else if (strToXlat.bCompareI(L"Bin"))
        return tCIDLib::ERadices::Bin;
    else if (strToXlat.bCompareI(L"Oct"))
        return tCIDLib::ERadices::Oct;
    else if (strToXlat.bCompareI(L"Dec"))
        return tCIDLib::ERadices::Dec;
    else if (strToXlat.bCompareI(L"Hex"))
        return tCIDLib::ERadices::Hex;

    return tCIDLib::ERadices::Auto;
}


tCIDLib::ESeverities tCIDLib::eXlatESeverities(const TString& strToXlat)
{
    tCIDLib::ESeverities eIndex = tCIDLib::ESeverities::Min;
    for (; eIndex < tCIDLib::ESeverities::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszSeverities[eIndex]))
            break;
    }
    return eIndex;
}


tCIDLib::EVJustify tCIDLib::eXlatEVJustify(const TString& strToXlat)
{
    tCIDLib::EVJustify eIndex = tCIDLib::EVJustify::Min;
    for (; eIndex < tCIDLib::EVJustify::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszVJustifications[eIndex]))
            break;
    }
    return eIndex;
}

tCIDLib::EVJustify tCIDLib::eAltXlatEVJustify(const TString& strToXlat)
{
    tCIDLib::EVJustify eIndex = tCIDLib::EVJustify::Min;
    for (; eIndex < tCIDLib::EVJustify::Count; eIndex++)
    {
        if (strToXlat.bCompareI(CIDLib_Type::apszAltVJustifications[eIndex]))
            break;
    }
    return eIndex;
}



// ---------------------------------------------------------------------------
//  Enum to text translation methods
// ---------------------------------------------------------------------------
const tCIDLib::TCh*
tCIDLib::pszXlatEDirs(const tCIDLib::EDirs eToXlat)
{
    if (eToXlat < tCIDLib::EDirs::Count)
        return CIDLib_Type::apszDirs[eToXlat];
    return CIDLib_Type::apszDirs[tCIDLib::EDirs::Count];
}

const tCIDLib::TCh*
tCIDLib::pszXlatEErrClasses(const tCIDLib::EErrClasses eToXlat)
{
    if (eToXlat < tCIDLib::EErrClasses::Count)
        return CIDLib_Type::apszErrClasses[eToXlat];
    return CIDLib_Type::apszErrClasses[tCIDLib::EErrClasses::Count];
}


const tCIDLib::TCh* tCIDLib::pszXlatELogFlags(const tCIDLib::ELogFlags eToXlat)
{
    if (eToXlat < tCIDLib::ELogFlags::Count)
        return CIDLib_Type::apszLogFlags[eToXlat];
    return CIDLib_Type::apszLogFlags[tCIDLib::ELogFlags::Count];
}


const tCIDLib::TCh* tCIDLib::pszXlatEHJustify(const tCIDLib::EHJustify eToXlat)
{
    if (eToXlat < tCIDLib::EHJustify::Count)
        return CIDLib_Type::apszHJustifications[eToXlat];
    return CIDLib_Type::apszHJustifications[tCIDLib::EHJustify::Count];
}

const tCIDLib::TCh* tCIDLib::pszAltXlatEHJustify(const tCIDLib::EHJustify eToXlat)
{
    if (eToXlat < tCIDLib::EHJustify::Count)
        return CIDLib_Type::apszAltHJustifications[eToXlat];
    return CIDLib_Type::apszAltHJustifications[tCIDLib::EHJustify::Count];
}


const tCIDLib::TCh* tCIDLib::pszXlatESeverities(const tCIDLib::ESeverities eToXlat)
{
    if (eToXlat < tCIDLib::ESeverities::Count)
        return CIDLib_Type::apszSeverities[eToXlat];
    return CIDLib_Type::apszSeverities[tCIDLib::ESeverities::Count];
}


const tCIDLib::TCh* tCIDLib::pszXlatEVJustify(const tCIDLib::EVJustify eToXlat)
{
    if (eToXlat < tCIDLib::EVJustify::Count)
        return CIDLib_Type::apszVJustifications[eToXlat];
    return CIDLib_Type::apszVJustifications[tCIDLib::EVJustify::Count];
}

const tCIDLib::TCh* tCIDLib::pszAltXlatEVJustify(const tCIDLib::EVJustify eToXlat)
{
    if (eToXlat < tCIDLib::EVJustify::Count)
        return CIDLib_Type::apszAltVJustifications[eToXlat];
    return CIDLib_Type::apszAltVJustifications[tCIDLib::EVJustify::Count];
}



// ---------------------------------------------------------------------------
//  Human readable text loading methods for enums
// ---------------------------------------------------------------------------
const TString& tCIDLib::strLoadEErrClasses(const tCIDLib::EErrClasses eToXlat)
{
    if (eToXlat >= tCIDLib::EErrClasses::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::TInt4(eToXlat))
            , TString(L"tCIDLib::EClasses")
        );
    }

    // Fault in the text if required
    if (!CIDLib_Type::astrErrClasses.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!CIDLib_Type::astrErrClasses.bIsLoaded())
        {
            tCIDLib::EErrClasses eIndex = tCIDLib::EErrClasses::Min;
            for (; eIndex < tCIDLib::EErrClasses::Count; eIndex++)
            {
                CIDLib_Type::astrErrClasses[eIndex] = TString
                (
                    CIDLib_Type::amidErrClasses[eIndex], facCIDLib()
                );
            }
            CIDLib_Type::astrErrClasses.SetLoaded();
        }
    }
    return CIDLib_Type::astrErrClasses[eToXlat];
}


TString tCIDLib::strLoadEExitCodes(const tCIDLib::EExitCodes eToXlat)
{
    TString strRet;

    switch(eToXlat)
    {
        case tCIDLib::EExitCodes::Deschedule        :
            strRet = L"Deschedule";
            break;

        case tCIDLib::EExitCodes::AdminStop         :
            strRet = L"AdminStop";
            break;

        case tCIDLib::EExitCodes::ConfigError       :
            strRet = L"ConfigError";
            break;

        case tCIDLib::EExitCodes::InternalError     :
            strRet = L"InternalError";
            break;

        case tCIDLib::EExitCodes::AdminRestart      :
            strRet = L"AdminRestart";
            break;

        case tCIDLib::EExitCodes::FatalError        :
            strRet = L"FatalError";
            break;

        case tCIDLib::EExitCodes::RuntimeError      :
            strRet = L"RuntimeError";
            break;

        case tCIDLib::EExitCodes::SystemException   :
            strRet = L"SystemException";
            break;

        case tCIDLib::EExitCodes::BadParameters     :
            strRet = L"BadParameters";
            break;

        case tCIDLib::EExitCodes::ResourceAccess    :
            strRet = L"ResourceAccess";
            break;

        case tCIDLib::EExitCodes::PermissionLevel   :
            strRet = L"PermissionLevel";
            break;

        case tCIDLib::EExitCodes::NotFound          :
            strRet = L"NotFound";
            break;

        case tCIDLib::EExitCodes::InitFailed        :
            strRet = L"InitFailed";
            break;

        case tCIDLib::EExitCodes::BadEnvironment    :
            strRet = L"BadEnvironment";
            break;

        default :
            strRet = L"ExitCode_";
            strRet.AppendFormatted(tCIDLib::c4EnumOrd(eToXlat));
            break;
    }

    return tCIDLib::ForceMove(strRet);
}


const TString& tCIDLib::strLoadEHJustify(const tCIDLib::EHJustify eToXlat)
{
    if (eToXlat >= tCIDLib::EHJustify::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDLib::EHJustify")
        );
    }

    // Fault in the strings if not already
    if (!CIDLib_Type::apstrHJustifications.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!CIDLib_Type::apstrHJustifications.bIsLoaded())
        {
            tCIDLib::EHJustify eIndex = tCIDLib::EHJustify::Min;
            for (; eIndex < tCIDLib::EHJustify::Count; eIndex++)
            {
                CIDLib_Type::apstrHJustifications[eIndex] = TString
                (
                    CIDLib_Type::amidHJustifications[tCIDLib::TCard4(eIndex)], facCIDLib()
                );
            }
        }
    }
    return CIDLib_Type::apstrHJustifications[eToXlat];
}


const TString& tCIDLib::strLoadELanguages(const tCIDLib::ELanguages eToXlat)
{
    if (eToXlat >= tCIDLib::ELanguages::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDLib::ELanguages")
        );
    }

    // Fault in the text values if not already
    if (!CIDLib_Type::apstrLanguages.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!CIDLib_Type::apstrLanguages.bIsLoaded())
        {
            tCIDLib::ELanguages eIndex = tCIDLib::ELanguages::Min;
            for (; eIndex < tCIDLib::ELanguages::Count; eIndex++)
            {
                CIDLib_Type::apstrLanguages[eIndex] = TString
                (
                    CIDLib_Type::amidLanguages[tCIDLib::c4EnumOrd(eIndex)], facCIDLib()
                );
            }
        }
    }
    return CIDLib_Type::apstrLanguages[eToXlat];
}


const TString& tCIDLib::strLoadEVJustify(const tCIDLib::EVJustify eToXlat)
{
    if (eToXlat >= tCIDLib::EVJustify::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDLib::EVJustify")
        );
    }

    // Fault in the strings if not already
    if (!CIDLib_Type::apstrVJustifications.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!CIDLib_Type::apstrVJustifications.bIsLoaded())
        {
            tCIDLib::EVJustify eIndex = tCIDLib::EVJustify::Min;
            for (; eIndex < tCIDLib::EVJustify::Count; eIndex++)
            {
                CIDLib_Type::apstrVJustifications[eIndex] = TString
                (
                    CIDLib_Type::amidVJustifications[tCIDLib::c4EnumOrd(eIndex)], facCIDLib()
                );
            }
        }
    }
    return CIDLib_Type::apstrVJustifications[eToXlat];
}


const TString& tCIDLib::strLoadESeverities(const tCIDLib::ESeverities eToXlat)
{
    if (eToXlat >= tCIDLib::ESeverities::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDLib::ESeverities")
        );
    }

    // Fault in the text values if not already
    if (!CIDLib_Type::apstrSeverities.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!CIDLib_Type::apstrSeverities.bIsLoaded())
        {
            tCIDLib::ESeverities eIndex = tCIDLib::ESeverities::Min;
            for (; eIndex < tCIDLib::ESeverities::Count; eIndex++)
            {
                CIDLib_Type::apstrSeverities[eIndex] = TString
                (
                    CIDLib_Type::apszSeveritiesTxt[eIndex]
                );
            }
        }
    }
    return CIDLib_Type::apstrSeverities[eToXlat];
}


// ---------------------------------------------------------------------------
//  Global text streaming functions
// ---------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EAngleTypes eAngle)
{
    if (eAngle == tCIDLib::EAngleTypes::Radians)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Radians);
    else if (eAngle == tCIDLib::EAngleTypes::Degrees)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Degrees);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eAngle))
            , TString(L"tCIDLib::EAngleTypes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EAudioCues eCue)
{
    switch(eCue)
    {
        case tCIDLib::EAudioCues::Alert :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Alert);
            break;

        case tCIDLib::EAudioCues::Bell :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Bell);
            break;

        case tCIDLib::EAudioCues::Click :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Click);
            break;

        case tCIDLib::EAudioCues::Chime :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Chime);
            break;

        case tCIDLib::EAudioCues::Error :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Error);
            break;

        case tCIDLib::EAudioCues::HiPulse :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_HiPulse);
            break;

        case tCIDLib::EAudioCues::Information :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Information);
            break;

        case tCIDLib::EAudioCues::LowPulse :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_LoPulse);
            break;

        case tCIDLib::EAudioCues::MedPulse :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_MedPulse);
            break;

        case tCIDLib::EAudioCues::Question :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Question);
            break;

        case tCIDLib::EAudioCues::Rising :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Rising);
            break;

        case tCIDLib::EAudioCues::Sinking :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Sinking);
            break;

        case tCIDLib::EAudioCues::Warning :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midACue_Warning);
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eCue))
                , TString(L"tCIDLib::EAudioCues")
            );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::TBoolean bBool)
{
    if (bBool == kCIDLib::True)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_True);
    else if (bBool == kCIDLib::False)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_False);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(bBool)
            , TString(L"tCIDLib::TBoolean")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ECreateActs eAction)
{
    if (eAction == tCIDLib::ECreateActs::None)
        strmToWriteTo << L"None";
    else if (eAction == tCIDLib::ECreateActs::CreateIfNew)
        strmToWriteTo << L"CreateIfNew";
    else if (eAction == tCIDLib::ECreateActs::CreateAlways)
        strmToWriteTo << L"CreateAlways";
    else if (eAction == tCIDLib::ECreateActs::OpenIfExists)
        strmToWriteTo << L"OpenIfExists";
    else if (eAction == tCIDLib::ECreateActs::OpenOrCreate)
        strmToWriteTo << L"OpenOrCreate";
    else if (eAction == tCIDLib::ECreateActs::TruncateExisting)
        strmToWriteTo << L"TruncateIfExists";
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::TInt4(eAction))
            , TString(L"tCIDLib::ECreateActs")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EDelModes eDelMode)
{
    if (eDelMode == tCIDLib::EDelModes::NoDelete)
        strmToWriteTo << L"No Delete";
    else if (eDelMode == tCIDLib::EDelModes::Default)
        strmToWriteTo << L"Default";
    else if (eDelMode == tCIDLib::EDelModes::Delete)
        strmToWriteTo << L"Delete";
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eDelMode))
            , TString(L"tCIDLib::EDelModes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EEndianModes eMode)
{
    if (eMode == tCIDLib::EEndianModes::Little)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Little);
    else if (eMode == tCIDLib::EEndianModes::Big)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Big);
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eMode))
            , TString(L"tCIDLib::EEndianModes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EEnds eAnEnd)
{
    if (eAnEnd == tCIDLib::EEnds::First)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_First);
    else if (eAnEnd == tCIDLib::EEnds::Last)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Last);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eAnEnd))
            , TString(L"tCIDLib::EEnds")
        );
    }
    return strmToWriteTo;
}


TTextOutStream &
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EErrClasses eClass)
{
    strmToWriteTo << tCIDLib::strLoadEErrClasses(eClass);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EExitCodes eCode)
{
    strmToWriteTo << tCIDLib::strLoadEExitCodes(eCode);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EHJustify eJustify)
{
    strmToWriteTo << tCIDLib::strLoadEHJustify(eJustify);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ELanguages eLanguage)
{
    strmToWriteTo << tCIDLib::strLoadELanguages(eLanguage);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ELockStates eState)
{
    if (eState == tCIDLib::ELockStates::Unlocked)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Unlocked);
    else if (eState == tCIDLib::ELockStates::Locked)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Locked);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eState))
            , TString(L"tCIDLib::ELockStates")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ELogMapModes eMode)
{
    if (eMode == tCIDLib::ELogMapModes::Regular)
        strmToWriteTo << L"Regular";
    else if (eMode == tCIDLib::ELogMapModes::Spread)
        strmToWriteTo << L"Spread";
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eMode))
            , TString(L"tCIDLib::ELogMapModes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EMeasures eMeasure)
{
    if (eMeasure == tCIDLib::EMeasures::Metric)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Metric);
    else if (eMeasure == tCIDLib::EMeasures::US)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_US);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eMeasure))
            , TString(L"tCIDLib::EMeasures")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EMonths eMonth)
{
    if ((eMonth >= tCIDLib::EMonths::Min)
    &&  (eMonth <= tCIDLib::EMonths::Max))
    {
        // Get it from the locale
        strmToWriteTo << TLocale::strMonth(eMonth);
    }
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eMonth))
            , TString(L"tCIDLib::EMonths")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EPathParts ePart)
{
    if (ePart == tCIDLib::EPathParts::Path)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPath_Path);
    else if (ePart == tCIDLib::EPathParts::Name)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPath_Name);
    else if (ePart == tCIDLib::EPathParts::Extension)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPath_Extension);
    else if (ePart == tCIDLib::EPathParts::Volume)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPath_Volume);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(ePart))
            , TString(L"tCIDLib::EPathParts")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EPressStates eState)
{
    if (eState == tCIDLib::EPressStates::Released)
        strmToWriteTo << L"Released";
    else if (eState == tCIDLib::EPressStates::Pressed)
        strmToWriteTo << L"Pressed";
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eState))
            , TString(L"tCIDLib::EPressStates")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EPrioClasses ePrioClass)
{
    if (ePrioClass == tCIDLib::EPrioClasses::IdleTime)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Idle);
    else if (ePrioClass == tCIDLib::EPrioClasses::Normal)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Regular);
    else if (ePrioClass == tCIDLib::EPrioClasses::High)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Critical);
    else if (ePrioClass == tCIDLib::EPrioClasses::RealTime)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_FgnServer);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(ePrioClass))
            , TString(L"tCIDLib::EPrioClasses")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EPrioLevels ePrioLevel)
{
    if (ePrioLevel == tCIDLib::EPrioLevels::Lowest)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Lowest);
    else if (ePrioLevel == tCIDLib::EPrioLevels::BelowNormal)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_BelowNormal);
    else if (ePrioLevel == tCIDLib::EPrioLevels::Normal)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Normal);
    else if (ePrioLevel == tCIDLib::EPrioLevels::AboveNormal)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_AboveNormal);
    else if (ePrioLevel == tCIDLib::EPrioLevels::Highest)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midPrio_Highest);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(ePrioLevel))
            , TString(L"tCIDLib::EPrioLevels")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ERadices eRadix)
{
    if (eRadix == tCIDLib::ERadices::Auto)
        strmToWriteTo << L"Auto";
    else if (eRadix == tCIDLib::ERadices::Bin)
        strmToWriteTo << L"Bin";
    else if (eRadix == tCIDLib::ERadices::Oct)
        strmToWriteTo << L"Oct";
    else if (eRadix == tCIDLib::ERadices::Dec)
        strmToWriteTo << L"Dec";
    else if (eRadix == tCIDLib::ERadices::Hex)
        strmToWriteTo << L"Hex";
    else
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eRadix))
            , TString(L"tCIDLib::ERadices")
        );
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ERectlTypes eType)
{
    if (eType == tCIDLib::ERectlTypes::Inclusive)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Inclusive);
    else if (eType == tCIDLib::ERectlTypes::NonInclusive)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_NonInclusive);
    else
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDLib::ERectlTypes")
        );
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ESeverities eSeverity)
{
    strmToWriteTo << tCIDLib::strLoadESeverities(eSeverity);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EShareStates eState)
{
    if (eState == tCIDLib::EShareStates::Unshared)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Unshared);
    else if (eState == tCIDLib::EShareStates::Shared)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Shared);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eState))
            , TString(L"tCIDLib::EShareStates")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ESpecialTimes eTime)
{
    if (eTime == tCIDLib::ESpecialTimes::Base)
        strmToWriteTo << L"Base";
    else if (eTime == tCIDLib::ESpecialTimes::CurrentDate)
        strmToWriteTo << L"Current Date";
    else if (eTime == tCIDLib::ESpecialTimes::CurrentTime)
        strmToWriteTo << L"Current Time";
    else if (eTime == tCIDLib::ESpecialTimes::NoonToday)
        strmToWriteTo << L"Noon Today";
    else if (eTime == tCIDLib::ESpecialTimes::CurrentUTC)
        strmToWriteTo << L"Current UTC";
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eTime))
            , TString(L"tCIDLib::ESpecialTimes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ESymmetries eSymmetry)
{
    if (eSymmetry == tCIDLib::ESymmetries::None)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_None);
    else if (eSymmetry == tCIDLib::ESymmetries::VerticalFlip)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_vertical);
    else if (eSymmetry == tCIDLib::ESymmetries::HorizontalFlip)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_horizontal);
    else if (eSymmetry == tCIDLib::ESymmetries::QuadrantFlip)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_quadrant);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eSymmetry))
            , TString(L"tCIDLib::ESymmetries")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ESysExcepts eExcept)
{
    switch(eExcept)
    {
        case tCIDLib::ESysExcepts::Unknown :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Unknown);
            break;

        case tCIDLib::ESysExcepts::BreakPoint :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_BreakPoint);
            break;

        case tCIDLib::ESysExcepts::DataAlignment :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_DataAlignment);
            break;

        case tCIDLib::ESysExcepts::Float_Denormal :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_DenormalFloat);
            break;

        case tCIDLib::ESysExcepts::Float_DivByZero :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_FloatDivByZero);
            break;

        case tCIDLib::ESysExcepts::Float_InexactResult :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_InexactResult);
            break;

        case tCIDLib::ESysExcepts::Float_InvalidOp :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_FloatInvalidOp);
            break;

        case tCIDLib::ESysExcepts::Float_Overflow :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_FloatOverflow);
            break;

        case tCIDLib::ESysExcepts::Float_Stack :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_FloatStack);
            break;

        case tCIDLib::ESysExcepts::Float_Underflow :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_FloatUnderflow);
            break;

        case tCIDLib::ESysExcepts::Int_DivByZero :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_IntDivByZero);
            break;

        case tCIDLib::ESysExcepts::Int_Overflow :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_IntOverflow);
            break;

        case tCIDLib::ESysExcepts::PageLoadFailed :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_PageLoadFailed);
            break;

        case tCIDLib::ESysExcepts::ReadAccess :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_ReadAccess);
            break;

        case tCIDLib::ESysExcepts::StackOverflow :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_StackOverflow);
            break;

        case tCIDLib::ESysExcepts::WriteAccess :
            strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midSysE_WriteAccess);
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , TInteger(tCIDLib::i4EnumOrd(eExcept))
                , TString(L"tCIDLib::ESysExcepts")
            );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ETrailFmts eTrail)
{
    if (eTrail == tCIDLib::ETrailFmts::Zeroes)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Zeros);
    else if (eTrail == tCIDLib::ETrailFmts::Spaces)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Spaces);
    else if (eTrail == tCIDLib::ETrailFmts::Ignore)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Ignore);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eTrail))
            , TString(L"tCIDLib::ETrailFmts")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::ETriStates eState)
{
    if (eState == tCIDLib::ETriStates::Off)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Off);
    else if (eState == tCIDLib::ETriStates::Kinda)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Kinda);
    else if (eState == tCIDLib::ETriStates::On)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_On);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eState))
            , TString(L"tCIDLib::ETriStates")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EVisible eState)
{
    if (eState == tCIDLib::EVisible::Invisible)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Invisible);
    else if (eState == tCIDLib::EVisible::Visible)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Visible);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eState))
            , TString(L"tCIDLib::EVisible")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EVJustify eJustify)
{
    strmToWriteTo << tCIDLib::strLoadEVJustify(eJustify);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EVolHWTypes eType)
{
    if (eType == tCIDLib::EVolHWTypes::Unknown)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_Unknown);
    else if (eType == tCIDLib::EVolHWTypes::Floppy35)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_35Floppy);
    else if (eType == tCIDLib::EVolHWTypes::Floppy525)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_525Floppy);
    else if (eType == tCIDLib::EVolHWTypes::Removable)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_Removable);
    else if (eType == tCIDLib::EVolHWTypes::Fixed)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_Fixed);
    else if (eType == tCIDLib::EVolHWTypes::Remote)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_Remote);
    else if (eType == tCIDLib::EVolHWTypes::CD)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_CD);
    else if (eType == tCIDLib::EVolHWTypes::RAMDisk)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_RAMDisk);
    else if (eType == tCIDLib::EVolHWTypes::Share)
        strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midVol_Share);
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDLib::EVolHWTypes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDLib::EWeekDays eWeekDay)
{
    if ((eWeekDay >= tCIDLib::EWeekDays::Sunday)
    &&  (eWeekDay <= tCIDLib::EWeekDays::Saturday))
    {
        // Get it from the locale
        strmToWriteTo << TLocale::strDay(eWeekDay);
    }
     else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eWeekDay))
            , TString(L"tCIDLib::EWeekDays")
        );
    }
    return strmToWriteTo;
}
