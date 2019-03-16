//
// FILE NAME: CIDLib_Module.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/05/1997
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
//  TModule is the basic public class that represents a loadable file,
//  a DLL or Exe module.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TModule,TObject)


namespace CIDLib_Module
{
    // -----------------------------------------------------------------------
    //  Local static data
    //
    //  bInitLogger
    //      Indicates that we've initialized the logger. We can't just use
    //      the plgrCurrent pointer, since if there is no env or command
    //      line info to set up a logger, we don't set up one, and we don't
    //      want to continue to check over and over for that info.
    //
    //  bInitMsgs
    //      A flag to control whether we've done local static initialization
    //      yet.
    //
    //  c4EntryCount
    //      A counter to watch for nested exceptions and bail out before a
    //      stack overflow. Since we synchronize the work below, it basically
    //      just works as a reentrancy counter on the logging current thread,
    //      which is what we want.
    //
    //  plgrCurrent
    //      This is the current logger that we send all errors and messages
    //      logged through this process to. We don't own it.
    //
    //  pszTitle1
    //  pszTitle2
    //      The two title strings used in standard popups, support for which
    //      is provided here. They are defaulted so something is there until
    //      they get loaded from translatable text.
    //
    //  pszExceptDuringLog
    //  pszRecursiveError
    //  pszStrLoadFailure
    //      Errors displayed when things go wrong during fundamental ops
    //      provided here. They are defaulted so something is there until
    //      they get loaded from translatable text.
    // -----------------------------------------------------------------------
    volatile tCIDLib::TBoolean  bInitLogger = kCIDLib::False;
    volatile tCIDLib::TBoolean  bInitMsgs = kCIDLib::False;
    tCIDLib::TCard4             c4EntryCount = 0;
    MLogger*                    plgrCurrent = 0;
    const tCIDLib::TCh*         pszTitle1 = kCIDLib_::pszTitle1;
    const tCIDLib::TCh*         pszTitle2 = kCIDLib_::pszTitle2;
    const tCIDLib::TCh*         pszExceptDuringLog = kCIDLib_::pszExceptDuringLog;
    const tCIDLib::TCh*         pszRecursiveError = kCIDLib_::pszRecursiveError;
    const tCIDLib::TCh*         pszStrLoadFailure = kCIDLib_::pszStrLoadFailure;


    // -----------------------------------------------------------------------
    //  m_bInitStats
    //  m_sciStartTime
    //
    //      We maintain some stats cache values. This is the storage for those
    //      and a lazy init flag to fault them in.
    // -----------------------------------------------------------------------
    volatile tCIDLib::TBoolean  bInitStats = kCIDLib::False;
    TStatsCacheItem             sciStartTime;
}


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TVoid InitMsgs(const TModule& modSrc)
{
    if (!CIDLib_Module::bInitMsgs)
    {
        TBaseLock lockInit;
        if (!CIDLib_Module::bInitMsgs)
        {
            tCIDLib::TBoolean   bOk;
            const tCIDLib::TCh* pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_ExceptDuringLog, bOk);
            if (bOk)
                CIDLib_Module::pszExceptDuringLog = pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_RecursiveErr, bOk);
            if (bOk)
                CIDLib_Module::pszRecursiveError = pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_DefStrLoad, bOk);
            if (bOk)
                CIDLib_Module::pszStrLoadFailure = pszTmp;

            CIDLib_Module::bInitMsgs = kCIDLib::True;
        }
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TModule
//  PREFIX: mod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TModule::bLoadOSMsg(const tCIDLib::TOSErrCode errcId, TString& strMsgBuf)
{
    const tCIDLib::TCard4 c4BufSz = 1023;
    tCIDLib::TCh achMsg[c4BufSz + 1];

    if (!TKrnlError::bQuerySysErrMsg(errcId, achMsg, c4BufSz))
    {
        strMsgBuf = CIDLib_Module::pszStrLoadFailure;
        strMsgBuf.eReplaceToken(TCardinal(errcId), L'1');
        strMsgBuf.eReplaceToken(TCardinal(TKrnlError::kerrLast().errcId()), L'2');
        return kCIDLib::False;
    }

    // Copy the text back to the string
    strMsgBuf = achMsg;

    return kCIDLib::True;
}


tCIDLib::TVoid
TModule::BuildRawModName(const  TString&            strBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        ,       TString&            strToFill)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently. We just pass in a ridicuously big buffer
    //  for it to fill in. There is no path involved so someone would have
    //  to be insane to have an actual file name longer than this.
    //
    const tCIDLib::TCard4 c4BufSz = 2047;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlModule::bBuildRawModName( strBaseName.pszBuffer()
                                        , c4MajVersion
                                        , c4MinVersion
                                        , eModType
                                        , c4BufSz
                                        , achBuf))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strBaseName
        );
    }

    // Just copy the result back to the caller's string
    strToFill = achBuf;
}

tCIDLib::TVoid
TModule::BuildRawModName(const  TCIDModuleInfo&     modiSrc
                        ,       TString&            strToFill)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently. We just pass in a ridicuously big buffer
    //  for it to fill in. There is no path involved so someone would have
    //  to be insane to have an actual file name longer than this.
    //
    const tCIDLib::TCard4 c4BufSz = 2047;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlModule::bBuildRawModName( modiSrc.strBaseName().pszBuffer()
                                        , modiSrc.c4MajVersion()
                                        , modiSrc.c4MinVersion()
                                        , modiSrc.eModType()
                                        , c4BufSz
                                        , achBuf))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , modiSrc.strBaseName()
        );
    }

    // Just copy the result back to the caller's string
    strToFill = achBuf;
}


tCIDLib::TVoid
TModule::BuildRawModName(const  TString&            strBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        ,       TString&            strToFill
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently. We just pass in a ridicuously big buffer
    //  for it to fill in. There is no path involved so someone would have
    //  to be insane to have an actual file name longer than this.
    //
    const tCIDLib::TCard4 c4BufSz = 2047;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlModule::bBuildRawModName( strBaseName.pszBuffer()
                                        , c4MajVersion
                                        , c4MinVersion
                                        , eModType
                                        , c4BufSz
                                        , achBuf))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strBaseName
        );
    }

    // Just copy the result back to the caller's string
    strToFill = achBuf;

    //
    //  Now lets fill in the prefered and default language versions of the
    //  associated message file name. It has the same base name, plus the
    //  language suffix and the CIDMsg extension.
    //
    TPathStr pathTmp = strToFill;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffix());
    pathTmp.AppendExt(L"CIDMsgs");
    strPrefLangMsgs = pathTmp;

    pathTmp = strToFill;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strDefLanguageSuffix());
    pathTmp.AppendExt(L"CIDMsgs");
    strDefLangMsgs = pathTmp;
}

tCIDLib::TVoid
TModule::BuildRawModName(const  TCIDModuleInfo& modiSrc
                        ,       TString&        strToFill
                        ,       TString&        strPrefLangMsgs
                        ,       TString&        strDefLangMsgs)
{
    // Just call the other version
    BuildRawModName
    (
        modiSrc.strBaseName()
        , modiSrc.c4MajVersion()
        , modiSrc.c4MinVersion()
        , modiSrc.eModType()
        , strToFill
        , strPrefLangMsgs
        , strDefLangMsgs
    );
}

tCIDLib::TVoid
TModule::BuildRawModName(const  TString&            strBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        , const tCIDLib::ELanguages ePrefLanguage
                        , const tCIDLib::ELanguages eDefLanguage
                        ,       TString&            strToFill
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs
                        ,       TString&            strEnLangMsgs)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently. We just pass in a ridicuously big buffer
    //  for it to fill in. There is no path involved so someone would have
    //  to be insane to have an actual file name longer than this.
    //
    const tCIDLib::TCard4 c4BufSz = 2047;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlModule::bBuildRawModName( strBaseName.pszBuffer()
                                        , c4MajVersion
                                        , c4MinVersion
                                        , eModType
                                        , c4BufSz
                                        , achBuf))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strBaseName
        );
    }

    // Just copy the result back to the caller's string
    strToFill = achBuf;

    //
    //  Now lets fill in the prefered, default, and english language
    //  versions of the associated message file name. It has the same base
    //  name, plus the language suffix and the CIDMsg extension.
    //
    TPathStr pathTmp = strToFill;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(ePrefLanguage));
    pathTmp.AppendExt(L"CIDMsg");
    strPrefLangMsgs = pathTmp;

    pathTmp = strToFill;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(eDefLanguage));
    pathTmp.AppendExt(L"CIDMsg");
    strDefLangMsgs = pathTmp;

    pathTmp = strToFill;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(tCIDLib::ELanguages::English));
    pathTmp.AppendExt(L"CIDMsg");
    strEnLangMsgs = pathTmp;
}


tCIDLib::TVoid
TModule::BuildRawModName(const  TCIDModuleInfo&     modiSrc
                        , const tCIDLib::ELanguages ePrefLanguage
                        , const tCIDLib::ELanguages eDefLanguage
                        ,       TString&            strToFill
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs
                        ,       TString&            strEnLangMsgs)
{
    // Just call the other version
    BuildRawModName
    (
        modiSrc.strBaseName()
        , modiSrc.c4MajVersion()
        , modiSrc.c4MinVersion()
        , modiSrc.eModType()
        , ePrefLanguage
        , eDefLanguage
        , strToFill
        , strPrefLangMsgs
        , strDefLangMsgs
        , strEnLangMsgs
    );
}


tCIDLib::TCard8
TModule::c8ParseVersionStr( const   TString&            strToParse
                            , const tCIDLib::TBoolean   bFullFormat)
{
    // Call the other version to get the values out
    tCIDLib::TCard4 c4Maj;
    tCIDLib::TCard4 c4Min;
    tCIDLib::TCard4 c4Revision = 0;
    if (bFullFormat)
        ParseVersionStr(strToParse, c4Maj, c4Min, c4Revision);
     else
        ParseVersionStr(strToParse, c4Maj, c4Min);

    tCIDLib::TCard8 c8Ver = c4Maj;
    c8Ver <<= 16;
    c8Ver |= c4Min;
    c8Ver <<= 32;
    c8Ver |= c4Revision;

    return c8Ver;
}


tCIDLib::TVoid TModule::InstallLogger(MLogger* const plgrToAdopt)
{
    TMtxLocker lockLog(pmtxLogSync());

    // Store the new logger
    CIDLib_Module::plgrCurrent = plgrToAdopt;
    CIDLib_Module::bInitLogger = kCIDLib::True;
}



tCIDLib::TVoid TModule::LogEventObj(const TLogEvent& logevToLog)
{
    // Get the severity out since its used a lot below
    tCIDLib::ESeverities eSev = logevToLog.eSeverity();

    // Get control of the sync semaphore before we do anything
    TMtxLocker lockLog(pmtxLogSync());

    //
    //  See if there is a recursive error going on. If so, we need to
    //  do an emergency popup and give up.
    //
    if (CIDLib_Module::c4EntryCount > 16)
    {
        // Let the lock go so we don't hang up everyone
        lockLog.Release();

        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , CIDLib_Module::pszTitle1
            , CIDLib_Module::pszTitle2
            , 0
            , 0
            , 0
            , CIDLib_Module::pszRecursiveError
            , kCIDLib::pszEmptyZStr
        );
        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
    }

    //
    //  Bump the entry counter. We use a janitor so its sure to get put
    //  back on any entry, and before the mutex locker above lets the
    //  mutex go.
    //
    TCardJanitor janEntry
    (
        &CIDLib_Module::c4EntryCount, CIDLib_Module::c4EntryCount + 1
    );

    //
    //  If there is an installed logger, then we need to log it. Mark
    //  it as logged, so that if its throw also, the catcher will know
    //  if its been logged or not.
    //
    if (plgrTarget())
    {
        logevToLog.SetLogged();
        try
        {
            plgrTarget()->LogEvent(logevToLog);
        }

        catch(const TError& errToCatch)
        {
            // Let the lock go so we don't hang up everyone
            lockLog.Release();

            #if CID_DEBUG_ON
            TKrnlPopup::Show
            (
                errToCatch.strFileName().pszBuffer()
                , errToCatch.c4LineNum()
                , CIDLib_Module::pszTitle1
                , CIDLib_Module::pszExceptDuringLog
                , errToCatch.errcId()
                , 0
                , 0
                , errToCatch.strErrText().pszBuffer()
                , errToCatch.strAuxText().pszBuffer()
            );
            #endif
        }

        catch(...)
        {
            // Let the lock go so we don't hang up everyone
            lockLog.Release();

            #if CID_DEBUG_ON
            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , CIDLib_Module::pszTitle1
                , CIDLib_Module::pszTitle2
                , 0
                , 0
                , 0
                , CIDLib_Module::pszExceptDuringLog
                , kCIDLib::pszEmptyZStr
            );
            #endif
        }
    }

    //
    //  If its at or above the process fatal level, then we need to exit the
    //  program now. We use the standard runtime error code.
    //
    //  If we are in testing mode, then we don't exit since the host process
    //  is just probing to insure that errors are caught and wants to continue
    //  processing.
    //
    if ((eSev >= tCIDLib::ESeverities::ProcFatal)
    &&  (eSev != tCIDLib::ESeverities::Status)
    &&  !TSysInfo::bTestMode())
    {
        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
    }
}


//
//  This method is designed to log bulk events, generally when they recieved
//  by a logging server from clients. So we don't do much of the stuff that
//  the singular version of this method does. We just log them, but never throw
//  or exit the program or do any runtime dumps, since that wouldn't make any
//  sense in the context of a server handling errors from a remote client. The
//  client is the one who needs to do all of that stuff, in its own context.
//
tCIDLib::TVoid TModule::LogEventObjs(const TCollection<TLogEvent>& colToLog)
{
    // Get control of the sync semaphore before we do anything
    TMtxLocker lockLog(pmtxLogSync());

    //
    //  See if there is a recursive error going on. If so, we need to
    //  do an emergency popup and give up.
    //
    if (CIDLib_Module::c4EntryCount > 16)
    {
        // Let the lock go so we don't hang up everyone
        lockLog.Release();

        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , CIDLib_Module::pszTitle1
            , CIDLib_Module::pszTitle2
            , 0
            , 0
            , 0
            , CIDLib_Module::pszRecursiveError
            , kCIDLib::pszEmptyZStr
        );
        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
    }

    //
    //  Bump the entry counter. We use a janitor so its sure to get put
    //  back on any entry, and before the mutex locker above lets the
    //  mutex go.
    //
    TCardJanitor janEntry
    (
        &CIDLib_Module::c4EntryCount
        , CIDLib_Module::c4EntryCount + 1
    );

    // Get a cursor for the collection and iterate through it
    TColCursor<TLogEvent>* pcursErrs = colToLog.pcursNew();
    TJanitor<TColCursor<TLogEvent> > janCursor(pcursErrs);

    // If we have any elements and a logger installed, then lets log them
    MLogger* plgrCur = plgrTarget();
    if (pcursErrs->bIsValid() && plgrCur)
    {
        for (; pcursErrs->bIsValid(); pcursErrs->bNext())
        {
            const TLogEvent& errCur = pcursErrs->objRCur();
            errCur.SetLogged();

            try
            {
                plgrCur->LogEvent(errCur);
            }

            catch(const TError&)
            {
                // Let the lock go so we don't hang up everyone
                lockLog.Release();

                #if CID_DEBUG_ON
                TKrnlPopup::Show
                (
                    errCur.strFileName().pszBuffer()
                    , errCur.c4LineNum()
                    , CIDLib_Module::pszTitle1
                    , CIDLib_Module::pszExceptDuringLog
                    , errCur.errcId()
                    , 0
                    , 0
                    , errCur.strErrText().pszBuffer()
                    , errCur.strAuxText().pszBuffer()
                );
                #endif
            }

            catch(...)
            {
                // Let the lock go so we don't hang up everyone
                lockLog.Release();

                #if CID_DEBUG_ON
                TKrnlPopup::Show
                (
                    CID_FILE
                    , CID_LINE
                    , CIDLib_Module::pszTitle1
                    , CIDLib_Module::pszTitle2
                    , 0
                    , 0
                    , 0
                    , CIDLib_Module::pszExceptDuringLog
                    , kCIDLib::pszEmptyZStr
                );
                #endif
            }
        }
    }
}


tCIDLib::TVoid TModule::ParseVersionStr(const   TString&            strToParse
                                        ,       tCIDLib::TCard4&    c4MajVer
                                        ,       tCIDLib::TCard4&    c4MinVer)
{
    // Find the period, which separates the two parts
    tCIDLib::TCard4 c4Ofs;
    if (!strToParse.bFirstOccurrence(kCIDLib::chPeriod, c4Ofs))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToParse
        );
    }

    // Copy out the two parts to a temp and convert to binary
    try
    {
        TString strTmp;
        strToParse.CopyOutSubStr(strTmp, 0, c4Ofs);
        c4MajVer = strTmp.c4Val();
        strToParse.CopyOutSubStr(strTmp, c4Ofs + 1);
        c4MinVer = strTmp.c4Val();
    }

    catch(...)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToParse
        );
    }
}

tCIDLib::TVoid TModule::ParseVersionStr(const   TString&            strToParse
                                        ,       tCIDLib::TCard4&    c4MajVer
                                        ,       tCIDLib::TCard4&    c4MinVer
                                        ,       tCIDLib::TCard4&    c4Revision)
{
    // Find the periods, which separates the threeparts
    tCIDLib::TCard4 c4Ofs1;
    tCIDLib::TCard4 c4Ofs2 = 0;
    if (!strToParse.bFirstOccurrence(kCIDLib::chPeriod, c4Ofs1)
    ||  !strToParse.bLastOccurrence(kCIDLib::chPeriod, c4Ofs2)
    ||  ((c4Ofs2 - c4Ofs1) < 1))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToParse
        );
    }

    // Copy out the two parts to a temp and convert to binary
    try
    {
        TString strTmp;
        strToParse.CopyOutSubStr(strTmp, 0, c4Ofs1);
        c4MajVer = strTmp.c4Val();
        strToParse.CopyOutSubStr(strTmp, c4Ofs1 + 1, (c4Ofs2 - c4Ofs1) - 1);
        c4MinVer = strTmp.c4Val();
        strToParse.CopyOutSubStr(strTmp, c4Ofs2 + 1);
        c4Revision= strTmp.c4Val();
    }

    catch(...)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToParse
        );
    }
}


//
//  This method handles the lazy initialization of the logging sync mutex
//  that is used to serialize multiple threads logging at the same time.
//
TMutex* TModule::pmtxLogSync()
{
    static TMutex* pmtxLogSync = nullptr;

    if (!pmtxLogSync)
    {
        TBaseLock lockInit;
        if (!pmtxLogSync)
            TRawMem::pExchangePtr(&pmtxLogSync, new TMutex(tCIDLib::ELockStates::Unlocked));
    }
    return pmtxLogSync;
}



// ---------------------------------------------------------------------------
//  TModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TModule::TModule(const  TCIDModuleInfo&     modiSrc
                , const tCIDLib::TBoolean   bLoad) :

    m_eModType(modiSrc.eModType())
    , m_kmodThis()
    , m_pc1Res(0)
    , m_strName(modiSrc.strBaseName())
{
    // Call a common init that builds up names based on type and version info
    DoInit
    (
        modiSrc.c4MajVersion(), modiSrc.c4MinVersion(), modiSrc.eFlags(), bLoad
    );
}

TModule::TModule(   const   TString&            strName
                    , const tCIDLib::EModTypes  eModType
                    , const tCIDLib::TCard4     c4MajVer
                    , const tCIDLib::TCard4     c4MinVer
                    , const tCIDLib::EModFlags  eFlags
                    , const tCIDLib::TBoolean   bLoad) :

    m_eModType(eModType)
    , m_kmodThis()
    , m_pc1Res(nullptr)
    , m_strName(strName)
{
    // Call a common init that builds up names based on type and version info
    DoInit(c4MajVer, c4MinVer, eFlags, bLoad);
}


//
//  Load a facility from a specific path, not via the library path. It may or may not
//  be version stamped, we don't care at this level.
//
TModule::TModule(const  TString&            strFullPath
                , const tCIDLib::EModTypes  eModType
                , const tCIDLib::EModFlags  eFlags
                , const tCIDLib::TBoolean   bLoad) :

    m_eModType(eModType)
    , m_kmodThis()
    , m_pc1Res(nullptr)
{
    // If not already done, initialize statistics
    InitStats();

    // Get a copy of the name part (no extension) and store that
    TPathStr pathTmp(strFullPath);
    pathTmp.bQueryName(m_strName);

    // Remove the name part and store that as the path
    pathTmp.bRemoveNameExt();
    m_strPath = pathTmp;

    // Load from the original full path
    tCIDLib::TBoolean bRes;
    if (bLoad)
        bRes = m_kmodThis.bLoadFromName(strFullPath.pszBuffer());
    else
        bRes = m_kmodThis.bQueryFromName(strFullPath.pszBuffer());

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , bLoad ? kCIDErrs::errcMod_LoadByName : kCIDErrs::errcMod_QueryByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , m_strName
        );
    }

    DoInit2(strFullPath, eFlags);
}

//
//  Load a facility from a specific path, not the library path. It may or may not be
//  version stamped, we don't care at this level.
//
TModule::TModule(const  TString&            strFacName
                , const TString&            strLoadFrom
                , const tCIDLib::EModTypes  eModType
                , const tCIDLib::EModFlags  eFlags
                , const tCIDLib::TBoolean   bLoad) :

    m_eModType(eModType)
    , m_kmodThis()
    , m_pc1Res(0)
    , m_strName(strFacName)
    , m_strPath(strLoadFrom)
{
    // If not already done, initialize statistics
    InitStats();

    // Create the full path to load
    TPathStr pathFull(strLoadFrom);
    pathFull.AddLevel(strFacName);

    // Load from the full path
    tCIDLib::TBoolean bRes;
    if (bLoad)
        bRes = m_kmodThis.bLoadFromName(pathFull.pszBuffer());
    else
        bRes = m_kmodThis.bQueryFromName(pathFull.pszBuffer());

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , bLoad ? kCIDErrs::errcMod_LoadByName : kCIDErrs::errcMod_QueryByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , m_strName
        );
    }

    DoInit2(pathFull, eFlags);
}

TModule::~TModule()
{
    // Clean up our resource buffer if we loaded one
    try
    {
        delete [] m_pc1Res;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TModule: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TModule::bHasMessageFile() const
{
    // Just delegate it to the implementation object
    return m_kmodThis.bHasMessageFile();
}


tCIDLib::TBoolean
TModule::bLoadCIDMsg(const  tCIDLib::TMsgId midToLoad
                    ,       TString&        strMsgBuf) const
{
    //
    //  We don't have to replicate this loaded message because assigning it
    //  to the string object effectively does that inside the string ojbect.
    //
    tCIDLib::TBoolean bRet;
    strMsgBuf = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);
    return bRet;
}

tCIDLib::TBoolean
TModule::bLoadCIDMsg(const  tCIDLib::TMsgId     midToLoad
                    ,       TString&            strMsgBuf
                    , const MFormattable&       fmtblToken1
                    , const MFormattable&       fmtblToken2
                    , const MFormattable&       fmtblToken3
                    , const MFormattable&       fmtblToken4) const
{
    //
    //  Lload up the string. If it failed, then we got a default string, so don't
    //  try to do token replacement in that case.
    //
    tCIDLib::TBoolean bRet;
    strMsgBuf = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);

    if (bRet)
    {
        if (!MFormattable::bIsNullObject(fmtblToken1))
            strMsgBuf.eReplaceToken(fmtblToken1, L'1');
        if (!MFormattable::bIsNullObject(fmtblToken2))
            strMsgBuf.eReplaceToken(fmtblToken2, L'2');
        if (!MFormattable::bIsNullObject(fmtblToken3))
            strMsgBuf.eReplaceToken(fmtblToken3, L'3');
        if (!MFormattable::bIsNullObject(fmtblToken4))
            strMsgBuf.eReplaceToken(fmtblToken4, L'4');
    }
    return bRet;
}


tCIDLib::EModTypes TModule::eModType() const
{
    return m_eModType;
}


tCIDLib::TVoid
TModule::ErrorPopUp(const   tCIDLib::TErrCode   errcToDisplay
                    , const TString&            strFileName
                    , const tCIDLib::TCard4     c4Line
                    , const TString&            strAuxMsg)
{
    //
    //  See if we can load the text for the message. If not, then we get
    //  a default message.
    //
    TString strMsg(kCIDLib::pszEmptyZStr, 512);
    bLoadCIDMsg(errcToDisplay, strMsg);

    //
    //  Get a pointer to the correct raw aux message. It is an optional
    //  parameter, so check it for the nul object.
    //
    const tCIDLib::TCh* pszAux = kCIDLib::pszEmptyZStr;
    if (!TString::bIsNullObject(strAuxMsg))
        pszAux = strAuxMsg.pszBuffer();

    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , 0
        , errcToDisplay
        , 0
        , strMsg.pszBuffer()
        , pszAux
    );
}

tCIDLib::TVoid TModule::ErrorPopUp(const TLogEvent& logevToShow)
{
    TKrnlPopup::Show
    (
        logevToShow.strFileName().pszBuffer()
        , logevToShow.c4LineNum()
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , logevToShow.errcId()
        , logevToShow.errcKrnlId()
        , logevToShow.errcHostId()
        , logevToShow.strErrText().pszBuffer()
        , logevToShow.strAuxText().pszBuffer()
    );
}


tCIDLib::TVoid
TModule::KrnlErrorPopUp(const   TString&                strFile
                        , const tCIDLib::TCard4         c4Line
                        , const tCIDLib::TErrCode       errcToDisplay
                        , const TKrnlError&             kerrToDisplay)
{
    // Load the CIDLib error text
    TString strMsg(kCIDLib::pszEmptyZStr, 512);
    bLoadCIDMsg(errcToDisplay, strMsg);

    // Load the kernel error text
    const tCIDLib::TCh* pszKrnlMsg = kmodCIDKernel.pszLoadCIDFacMsg
    (
        kerrToDisplay.errcId()
    );

    TKrnlPopup::Show
    (
        strFile.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , errcToDisplay
        , kerrToDisplay.errcId()
        , kerrToDisplay.errcHostId()
        , strMsg.pszBuffer()
        , pszKrnlMsg
    );
}


const TKrnlModule& TModule::kmodThis() const
{
    return m_kmodThis;
}


tCIDLib::TVoid
TModule::LogKrnlErr(const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToLog
                    , const TKrnlError&             klogevToLog
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strMsg(kCIDLib::pszEmptyZStr, 128);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToLog.errcHostId())
        bLoadOSMsg(klogevToLog.errcHostId(), strKrnlMsg);

    // Load the CIDLib error message
    bLoadCIDMsg(errcToLog, strMsg);

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToLog
        , klogevToLog
        , strMsg
        , strKrnlMsg
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogKrnlErr(const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToLog
                    , const TKrnlError&             klogevToLog
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strText(kCIDLib::pszEmptyZStr, 256);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToLog.errcHostId())
        bLoadOSMsg(klogevToLog.errcHostId(), strKrnlMsg);

    // Get a copy of the err text and do the token replacement
    bLoadCIDMsg
    (
        errcToLog
        , strText
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToLog
        , klogevToLog
        , strText
        , strKrnlMsg
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}


tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , strAuxText
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strMsg(kCIDLib::pszEmptyZStr, 256);
    bLoadCIDMsg(midToLog, strMsg);

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strMsg(kCIDLib::pszEmptyZStr, 256);
    bLoadCIDMsg(midToLog, strMsg);

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , strAuxText
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strTmp(strMsg, 256);

    // Do parameter replacement
    if (!MFormattable::bIsNullObject(fmtblToken1))
        strTmp.eReplaceToken(fmtblToken1, L'1');
    if (!MFormattable::bIsNullObject(fmtblToken2))
        strTmp.eReplaceToken(fmtblToken2, L'2');
    if (!MFormattable::bIsNullObject(fmtblToken3))
        strTmp.eReplaceToken(fmtblToken3, L'3');
    if (!MFormattable::bIsNullObject(fmtblToken4))
        strTmp.eReplaceToken(fmtblToken4, L'4');

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strTmp
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strMsg(kCIDLib::pszEmptyZStr, 256);
    bLoadCIDMsg
    (
        midToLog
        , strMsg
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    // If there's no target logger, then short circuit
    if (!plgrTarget())
        return;

    TString strMsg(kCIDLib::pszEmptyZStr, 256);
    bLoadCIDMsg
    (
        midToLog, strMsg, fmtblToken1, fmtblToken2, fmtblToken3, fmtblToken4
    );

    // Create the event object, and log it
    TLogEvent logevToLog
    (
        m_strName
        , strFileName
        , c4LineNum
        , strMsg
        , strAuxText
        , eSeverity
        , eClass
    );
    LogEventObj(logevToLog);
}

tCIDLib::TVoid
TModule::MsgPopUp(  const   TString&        strMsg
                    , const TString&        strFileName
                    , const tCIDLib::TCard4 c4Line
                    , const TString&        strAuxMsg)
{
    const tCIDLib::TCh* pszAux = kCIDLib::pszEmptyZStr;
    if (!TString::bIsNullObject(strAuxMsg))
        pszAux = strAuxMsg.pszBuffer();

    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , 0
        , 0
        , 0
        , strMsg.pszBuffer()
        , pszAux
    );
}


//
//  Locate the raw data for a resource of a given type and/or resource id, within our
//  pre-loaded resource data and return a pointer to it, and it's size. In teh first
//  one, we have to match both id resource type and id. In the other one, we just look
//  for the id, and return what resource type it is.
//
const tCIDLib::TCard1*
TModule::pc1FindRes(        tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TResId     ridToFind
                    , const tCIDLib::EResTypes  eType
                    , const tCIDLib::TBoolean   bErrIfNotFound) const
{
    // If we don't have any resources, then just return a failure
    if (!m_pc1Res)
        return 0;

    // Look at the buffer as a resource header
    const tCIDLib::TResHeader* prhFind
                    = reinterpret_cast<const tCIDLib::TResHeader*>(m_pc1Res);

    const tCIDLib::TResItemMap* primFind =
        reinterpret_cast<const tCIDLib::TResItemMap*>(m_pc1Res + sizeof(tCIDLib::TResHeader));

    // Search all of the map items
    for (tCIDLib::TCard4 c4Index = 0; c4Index < prhFind->c4ResCount; c4Index++)
    {
        //
        //  If we find it, then check the type. If the right type, return it,
        //  else keep looking.
        //
        if ((primFind->ridRes == ridToFind) && (primFind->eType == eType))
        {
            c4ToFill = primFind->c4Size;
            return m_pc1Res + primFind->c4At;
        }
        primFind++;
    }

    // Oh well, never found it. If that's bad, then throw here
    if (bErrIfNotFound)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_ResNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strName()
            , TCardinal(ridToFind)
        );
    }
    return 0;
}


const tCIDLib::TCard1*
TModule::pc1FindRes(        tCIDLib::EResTypes& eType
                    ,       tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TResId     ridToFind
                    , const tCIDLib::TBoolean   bErrIfNotFound) const
{
    // If we don't have any resources, then just return a failure
    if (!m_pc1Res)
        return 0;

    // Look at the buffer as a resource header
    const tCIDLib::TResHeader* prhFind
                    = reinterpret_cast<const tCIDLib::TResHeader*>(m_pc1Res);

    const tCIDLib::TResItemMap* primFind =
        reinterpret_cast<const tCIDLib::TResItemMap*>(m_pc1Res + sizeof(tCIDLib::TResHeader));

    // Search all of the map items
    for (tCIDLib::TCard4 c4Index = 0; c4Index < prhFind->c4ResCount; c4Index++)
    {
        // If we find it, then give them back the type and pionter
        if (primFind->ridRes == ridToFind)
        {
            eType = primFind->eType;
            c4ToFill = primFind->c4Size;
            return m_pc1Res + primFind->c4At;
        }
        primFind++;
    }

    // Oh well, never found it. If that's bad, then throw here
    if (bErrIfNotFound)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_ResNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strName()
            , TCardinal(ridToFind)
        );
    }
    return 0;
}


//
//  Get a function pointer within this module. Used mostly when dynamically loading
//  a module, where we need to get a pointer to a function inside it to call for
//  initialization purposes.
//
tCIDLib::FuncPtr
TModule::pfnGetFuncPtr(const tCIDLib::TSCh* const pszFuncName) const
{
    tCIDLib::FuncPtr pRet;
    if (!m_kmodThis.bGetFuncPtr(pszFuncName, pRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_GetFuncPtr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(pszFuncName)
        );
    }
    return pRet;
}


//
//  Load message text in various ways. For the raw pointer type ones, it just reutrns
//  a pointer to the in-place text, the caller is not responsible for cleaning it up.
//
const tCIDLib::TCh*
TModule::pszLoadCIDMsg( const   tCIDLib::TMsgIndex& miSrc
                        , const tCIDLib::TMsgId     midToLoad
                        ,       tCIDLib::TBoolean&  bLoaded) const
{
    return m_kmodThis.pszLoadCIDMsg(miSrc, midToLoad, bLoaded);
}


const tCIDLib::TCh*
TModule::pszLoadCIDMsg( const   tCIDLib::TMsgId     midToLoad
                        ,       tCIDLib::TBoolean&  bLoaded) const
{
    return m_kmodThis.pszLoadCIDFacMsg(midToLoad, bLoaded);
}

const tCIDLib::TCh*
TModule::pszLoadCIDMsg(const tCIDLib::TMsgId midToLoad) const
{
    // Just call the other version with a dummy status
    tCIDLib::TBoolean bDummy;
    return pszLoadCIDMsg(midToLoad, bDummy);
}


TString TModule::strMsg(const tCIDLib::TMsgId midToLoad) const
{
    TString strRet(kCIDLib::pszEmptyZStr, 1024);
    bLoadCIDMsg(midToLoad, strRet);
    return strRet;
}

TString TModule::strMsg(const   tCIDLib::TMsgId midToLoad
                        , const MFormattable&   fmtblToken1
                        , const MFormattable&   fmtblToken2
                        , const MFormattable&   fmtblToken3
                        , const MFormattable&   fmtblToken4) const
{
    TString strRet(kCIDLib::pszEmptyZStr, 1024);
    bLoadCIDMsg
    (
        midToLoad
        , strRet
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );
    return strRet;
}


// REturn the name of this module
const TString& TModule::strName() const
{
    return m_strName;
}


// Return the path to this module
const TString& TModule::strPath() const
{
    return m_strPath;
}


//
//  Throw errors in various ways.
//
tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg(errcToThrow, strTmp);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , kCIDLib::pszEmptyZStr
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
 }

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg
    (
        errcToThrow
        , strTmp
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , kCIDLib::pszEmptyZStr
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg(errcToThrow, strTmp);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , strAuxText
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg
    (
        errcToThrow
        , strTmp
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , strAuxText
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


tCIDLib::TVoid
TModule::ThrowKrnlErr(  const   TString&                strFileName
                        , const tCIDLib::TCard4         c4LineNum
                        , const tCIDLib::TErrCode       errcToThrow
                        , const TKrnlError&             klogevToThrow
                        , const tCIDLib::ESeverities    eSeverity
                        , const tCIDLib::EErrClasses    eClass) const
{
    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strMsg(kCIDLib::pszEmptyZStr, 128);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToThrow.errcHostId())
        bLoadOSMsg(klogevToThrow.errcHostId(), strKrnlMsg);

    // Load the CIDLib error message
    bLoadCIDMsg(errcToThrow, strMsg);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , klogevToThrow
        , strMsg
        , strKrnlMsg
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


tCIDLib::TVoid
TModule::ThrowKrnlErr(  const   TString&                strFileName
                        , const tCIDLib::TCard4         c4LineNum
                        , const tCIDLib::TErrCode       errcToThrow
                        , const TKrnlError&             klogevToThrow
                        , const tCIDLib::ESeverities    eSeverity
                        , const tCIDLib::EErrClasses    eClass
                        , const MFormattable&           fmtblToken1
                        , const MFormattable&           fmtblToken2
                        , const MFormattable&           fmtblToken3
                        , const MFormattable&           fmtblToken4) const
{
    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strText(kCIDLib::pszEmptyZStr, 256);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToThrow.errcHostId())
        bLoadOSMsg(klogevToThrow.errcHostId(), strKrnlMsg);

    // Get a copy of the err text and do the token replacement
    bLoadCIDMsg
    (
        errcToThrow
        , strText
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , klogevToThrow
        , strText
        , strKrnlMsg
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


// ---------------------------------------------------------------------------
//  TModule: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  In some cases, we want to load a third party DLL which isn't one of our facilities
//  so we provide a default that just says yes, log errors, so that we can create
//  instances of this class.
//
tCIDLib::TBoolean TModule::bTraceErrs() const
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TModule: Protected, static methods
// ---------------------------------------------------------------------------

//
//  This method handles the lazy init of the default logger object. If one
//  has not been set, then a default one is created using the CID_LOCALLOG
//  environment variable if it is set. If that's not set, then no logger is
//  set.
//
MLogger* TModule::plgrTarget()
{
    TMtxLocker lockInit(pmtxLogSync());

    //
    //  If the logger has never been set, then we need to create a default
    //  on. We assume it will log in the default text format for the platform
    //  but the environment can override that.
    //
    if (!CIDLib_Module::bInitLogger)
    {
        //
        //  Indicate that'we tried to set up a logger. If we don't, then
        //  we won't bother to try from here on out.
        //
        CIDLib_Module::bInitLogger = kCIDLib::True;

        tCIDLib::TBoolean       bGotIt = kCIDLib::False;
        const tCIDLib::TCard4   c4BufMax = 512;
        TTextConverter*         ptcvtToUse = 0;
        tCIDLib::TCh            szInfoBuf[c4BufMax+1];

        //
        //  First we need to see if the information was provided on the
        //  command line via the /LocalLog= parameter. If so, then that
        //  takes precedence. If not, then check the environment.
        //
        if (TSysInfo::pszLogInfo())
        {
            bGotIt = kCIDLib::True;
            TRawStr::CopyStr(szInfoBuf, TSysInfo::pszLogInfo(), c4BufMax);
        }
         else
        {
            // Check the environment
            bGotIt = TKrnlEnvironment::bFind(kCIDLib_::pszLocalLog, szInfoBuf, c4BufMax);
        }

        if (bGotIt)
        {
            tCIDLib::TCh szFileName[c4BufMax+1];
            tCIDLib::TCh szMutexName[c4BufMax+1];

            szFileName[0] = kCIDLib::chNull;
            szMutexName[0] = kCIDLib::chNull;

            //
            //  Preload the failure strings. If they fail to load, then put
            //  in a default english messages.
            //
            tCIDLib::TBoolean bOk;
            const tCIDLib::TCh* pszBadFormat = facCIDLib().pszLoadCIDMsg
            (
                kCIDErrs::errcMod_BadLogInfoFmt
                , bOk
            );
            if (!bOk)
                pszBadFormat = kCIDLib_::pszBadLocalLog;

            const tCIDLib::TCh* pszErrOpenLgr = facCIDLib().pszLoadCIDMsg
            (
                kCIDErrs::errcMod_ErrOpenLgr
                , bOk
            );
            if (!bOk)
                pszErrOpenLgr = kCIDLib_::pszErrCreatingLgr;

            //
            //  The local log information is a blob that we have to parse out.
            //  It is in this format:
            //
            //      filename;format;mutexname
            //
            //  The file name is the name of the file to log to. It is
            //  followed by an optional format and mutex name. The format must
            //  be either Ascii or Unicode. The mutex name is optional and
            //  should be just the last component of a standard CIDLib
            //  named resource name. If the format is not provided but the
            //  mutex is, then it should be filename;;mutexname.
            //
            tCIDLib::TCh* pszCtx = 0;
            tCIDLib::TCh* pszTmp = TRawStr::pszStrTokenize(szInfoBuf, L";", &pszCtx);
            if (!pszTmp)
            {
                //
                //  Note that we must call straight to low level popup here
                //  since doing otherwise might cause a recursive call back
                //  here.
                //
                TKrnlPopup::Show
                (
                    CID_FILE
                    , CID_LINE
                    , CIDLib_Module::pszTitle1
                    , CIDLib_Module::pszTitle2
                    , 0
                    , 0
                    , 0
                    , pszBadFormat
                    , kCIDLib::pszEmptyZStr
                );
                TProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
            }
            TRawStr::CopyStr(szFileName, pszTmp, c4BufMax);

            //
            //  The next two are optional so we don't fail if they are not
            //  there. But the format string can be malformed.
            //
            pszTmp = TRawStr::pszStrTokenize(0, L";", &pszCtx);
            if (pszTmp)
            {
                if (pszTmp[0])
                {
                    if (TRawStr::bCompareStrI(pszTmp, L"UTF-8"))
                        ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::UTF8);
                    else if (TRawStr::bCompareStrI(pszTmp, L"UTF-16"))
                        ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::Def16);
                    else
                    {
                        // Indicate unknown encoding
                        TKrnlPopup::Show
                        (
                            CID_FILE
                            , CID_LINE
                            , CIDLib_Module::pszTitle1
                            , CIDLib_Module::pszTitle2
                            , 0
                            , 0
                            , 0
                            , pszBadFormat
                            , kCIDLib::pszEmptyZStr
                        );

                        // And default to a UTF-8 encoding
                        ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::UTF8);
                    }
                }

                // And check for a mutex name
                pszTmp = TRawStr::pszStrTokenize(0, L";", &pszCtx);
                if (pszTmp)
                    TRawStr::CopyStr(szMutexName, pszTmp, c4BufMax);
            }

            // If not explicit converter, then use UTF8
            if (!ptcvtToUse)
                ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::UTF8);

            //
            //  According to whether we have a mutex name, lets create the
            //  resource name object or not.
            //
            TResourceName rsnTmp(L"CharmedQuark", L"CIDLib", szMutexName);
            const TResourceName* prsnMutex = szMutexName[0]
                                             ? &rsnTmp
                                             : &TResourceName::Nul_TResourceName();

            try
            {
                //
                //  If no mutex, then assume it's for exclusive use and set
                //  the access to exclusive out stream, which lets us write
                //  and others read, but only we can write.
                //
                TTextFileLogger* plgrNew = new TTextFileLogger
                (
                    szFileName
                    , szMutexName[0] ? tCIDLib::EAccessModes::Write
                                     : tCIDLib::EAccessModes::Excl_OutStream
                    , ptcvtToUse
                    , *prsnMutex
                );

                // Set the rollover size to something reasonable
                plgrNew->c8RolloverSize(0x10000);

                // And store the pointer now
                CIDLib_Module::plgrCurrent = plgrNew;
            }

            catch(const TError& errToCatch)
            {
                TKrnlPopup::Show
                (
                    CID_FILE
                    , CID_LINE
                    , CIDLib_Module::pszTitle1
                    , CIDLib_Module::pszTitle2
                    , errToCatch.errcId()
                    , errToCatch.errcKrnlId()
                    , errToCatch.errcHostId()
                    , pszErrOpenLgr
                    , szFileName
                );
                TProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
            }

            catch(...)
            {
                TKrnlPopup::Show
                (
                    CID_FILE
                    , CID_LINE
                    , CIDLib_Module::pszTitle1
                    , CIDLib_Module::pszTitle2
                    , 0
                    , 0
                    , 0
                    , pszErrOpenLgr
                    , szFileName
                );
                TProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
            }
        }
    }
    return CIDLib_Module::plgrCurrent;
}


// ---------------------------------------------------------------------------
//  TModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TModule::DoInit( const   tCIDLib::TCard4     c4MajVer
                                , const tCIDLib::TCard4     c4MinVer
                                , const tCIDLib::EModFlags  eFlags
                                , const tCIDLib::TBoolean   bLoad)
{
    tCIDLib::TBoolean bRes;

    //
    //  Init the module handle data member. According to the bLoad parameter
    //  we either need to query or load by name.
    //
    if (bLoad)
    {
        bRes = m_kmodThis.bLoadFromName
        (
            m_strName.pszBuffer()
            , m_eModType
            , c4MajVer
            , c4MinVer
            , eFlags
        );
    }
     else
    {
        bRes = m_kmodThis.bQueryFromName
        (
            m_strName.pszBuffer()
            , m_eModType
            , c4MajVer
            , c4MinVer
            , eFlags
        );
    }

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , bLoad ? kCIDErrs::errcMod_LoadByName : kCIDErrs::errcMod_QueryByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , m_strName
        );
    }

    //
    //  Ok, lets try to load up the local strings that are preloaded and
    //  cached. These are attached to the CIDLib module, so we have to make
    //  sure that's what is loading. But it should always be the first module
    //  that loads and uses this class.
    //
    if (!CIDLib_Module::bInitMsgs)
    {
        if (m_strName == L"CIDLib")
            InitMsgs(*this);
    }

    // Find the module path and save it
    const tCIDLib::TCard4 c4NameChars = kCIDLib::c4MaxPathLen;
    tCIDLib::TCh szModPath[c4NameChars+1];
    tCIDLib::TCh szModName[c4NameChars+1];
    if (!TKrnlModule::bRawQueryModName( m_kmodThis.hmodThis()
                                        , szModName
                                        , szModPath
                                        , c4NameChars))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_QueryModPath
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strName
        );
    }
    m_strPath = szModPath;

    // Do the other init, passing the ufll path
    TPathStr pathFull(m_strPath);
    pathFull.AddLevel(szModName);
    DoInit2(pathFull, eFlags);
}


//
//  We get the full module path loaded from which will include the version stamp
//  if the module is versioned. So we can us that to build the path to the resource
//  file.
//
tCIDLib::TVoid
TModule::DoInit2(const  TString&            strModPath
                , const tCIDLib::EModFlags  eFlags)
{
    //
    //  If we have a resource file, then load it up. We build up the file
    //  name and load up the raw resource buffer pointer.
    //
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EModFlags::HasResFile))
    {
        TPathStr pathTmp(strModPath);

        // Remove the module's extension and add the resource file's
        pathTmp.bRemoveExt();
        pathTmp.AppendExt(L"CIDRes");

        // And load up the resources
        LoadRes(pathTmp);
    }
}


// Initialize our stats if not done already
tCIDLib::TVoid TModule::InitStats()
{
    // If we've not faulted in our stats cache stuff yet, then do that.
    if (!CIDLib_Module::bInitStats)
    {
        TBaseLock lockInit;
        if (!CIDLib_Module::bInitStats)
        {
            //
            //  The start time of the app. We just set it here and it
            //  says that way for ever. Any app has to initialize a module
            //  some way or how to do anything useful, so this serves as
            //  start time stamp.
            //
            TStatsCache::RegisterItem
            (
                kCIDLib::pszStat_AppInfo_UpSince
                , tCIDLib::EStatItemTypes::Time
                , CIDLib_Module::sciStartTime
            );
            TStatsCache::SetValue(CIDLib_Module::sciStartTime, TTime::enctNow());

            CIDLib_Module::bInitStats = kCIDLib::True;
        }
    }
}


//
//  Loads up the passed resource file and stores a pointer to the allocated
//  buffer.
//
tCIDLib::TVoid TModule::LoadRes(const TString& strResFile)
{
    TCritSecLocker lockSync(&m_crsSync);
    if (!m_pc1Res)
    {
        // Open a binary file for the resource file
        TBinaryFile flRes(strResFile);
        flRes.Open
        (
            tCIDLib::EAccessModes::Multi_Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );

        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flRes.c8CurSize());
        m_pc1Res = new tCIDLib::TCard1[c4Size];
        if (flRes.c4ReadBuffer(m_pc1Res, c4Size) != c4Size)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMod_NoResFile
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strResFile
            );
        }
    }
}


