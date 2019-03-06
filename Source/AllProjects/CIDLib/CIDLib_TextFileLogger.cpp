//
// FILE NAME: CIDLib_TexTTextFileLogger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a file logger, which sends logged errors to a
//  text output file, as formatted text strings.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TTextFileLogger,TObject)


// ---------------------------------------------------------------------------
//  TTextFileLogger: Constructors and operators
// ---------------------------------------------------------------------------
TTextFileLogger::TTextFileLogger() :

    m_bNeedsBOM(kCIDLib::False)
    , m_c8RolloverSize(kCIDLib::c8MaxCard)
    , m_eAccess(tCIDLib::EAccessModes::Excl_OutStream)
    , m_pmtxSync(0)
    , m_pstrmTarget(0)
{
}

TTextFileLogger::TTextFileLogger(const  tCIDLib::TCh* const     pszFile
                                , const tCIDLib::EAccessModes   eAccess
                                ,       TTextConverter* const   ptcvtToAdopt
                                , const TResourceName&          rsnMutex) :

    m_bNeedsBOM(kCIDLib::False)
    , m_c8RolloverSize(kCIDLib::c8MaxCard)
    , m_eAccess(eAccess)
    , m_pmtxSync(0)
    , m_pstrmTarget(0)
{
    // Open the text file stream
    Open(pszFile, ptcvtToAdopt, rsnMutex);
}

TTextFileLogger::TTextFileLogger(const  TString&                strFile
                                , const tCIDLib::EAccessModes   eAccess
                                ,       TTextConverter* const   ptcvtToAdopt
                                , const TResourceName&          rsnMutex) :

    m_bNeedsBOM(kCIDLib::False)
    , m_c8RolloverSize(kCIDLib::c8MaxCard)
    , m_eAccess(eAccess)
    , m_pmtxSync(0)
    , m_pstrmTarget(0)
{
    // Open the text file stream
    Open(strFile, ptcvtToAdopt, rsnMutex);
}

TTextFileLogger::~TTextFileLogger()
{
    delete m_pmtxSync;
    delete m_pstrmTarget;
}


// ---------------------------------------------------------------------------
//  TTextFileLogger: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTextFileLogger::LogEvent(const TLogEvent& logevToLog)
{
    //
    //  Create some static stuff for us in logging. We know we can do this
    //  because calls to this method are thread serialized by the CIDLib
    //  code that calls it. This lets us avoid the creation on every call.
    //
    static volatile tCIDLib::TBoolean   bFirstTime = kCIDLib::True;
    static TTime                        tmCurrent;

    //
    //  If this is the first time through, then we need to set the default
    //  format for the time object so that it will naturally format in the
    //  style we want. It will be in the form:
    //
    //  MM:DD HH:MM:SS
    //
    //  In a 24 hour format.
    //
    if (bFirstTime)
    {
        TBaseLock lockInit;
        if (bFirstTime)
        {
            tmCurrent.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
            bFirstTime = kCIDLib::False;
        }
    }

    // Set the time object to the time in the event
    tmCurrent = logevToLog.enctLogged();

    // If there is a sync mutex then lock it
    if (m_pmtxSync)
        m_pmtxSync->Lock();

    try
    {
        //
        //  If the file has reached or exceeded the rollover size and it is
        //  not a shared file (has no mutex), then we need to close this one
        //  and open a new one, moving this one to the backup name.
        //
        if (!m_pmtxSync && (m_pstrmTarget->c8CurSize() >= m_c8RolloverSize))
        {
            // Get the current file name and build the backup file name
            TPathStr pathCur = m_pstrmTarget->strFileName();

            TPathStr pathBack = pathCur;
            pathBack.bRemoveExt();
            pathBack.AppendExt(L"BakLog");

            enum EStates
            {
                EState_DelBack
                , EState_CloseCur
                , EState_RenCur
                , EState_OpenNew
            };

            EStates eCurState = EState_DelBack;
            try
            {
                // If a backup file exists, then delete it
                if (TFileSys::bExists(pathBack))
                    TFileSys::DeleteFile(pathBack);

                //
                //  Close the current stream so that we can rename this
                //  current file.
                //
                eCurState = EState_CloseCur;
                m_pstrmTarget->Close();

                eCurState = EState_RenCur;
                TFileSys::Rename(pathCur, pathBack);

                //
                //  And open up the current file again, which will create
                //  a fresh log file.
                //
                eCurState = EState_OpenNew;
                m_pstrmTarget->Open
                (
                    pathCur
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::AllAccess
                    , tCIDLib::EFileFlags::SafeStream
                    , m_eAccess
                );

                //
                //  If the output encoding was one that required a BOM
                //  the write one out.
                //
                if (m_bNeedsBOM)
                    *m_pstrmTarget << kCIDLib::chUniBOM;
            }

            catch(...)
            {
                switch(eCurState)
                {
                    case EState_DelBack :
                        // Ignore it and stick with the current file
                        break;

                    case EState_CloseCur :
                    case EState_OpenNew :
                    {
                        // We just cannot reasonably recover from these
                        #if CID_DEBUG_ON
                        TPopUp::PopUpMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , L"CIDLib C++ Frameworks"
                            , 0
                            , L"The local log file could not be rolled over. "
                              L"In order to recover, the program will exit."
                        );
                        #endif
                        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
                        break;
                    }

                    case EState_RenCur :
                    {
                        //
                        //  Probably someone has the file open, so we just
                        //  want to try to reopen and continue with the
                        //  same file. If we cannot, give up.
                        //
                        try
                        {
                            m_pstrmTarget->Open
                            (
                                pathCur
                                , tCIDLib::ECreateActs::OpenIfExists
                                , tCIDLib::EFilePerms::AllAccess
                                , tCIDLib::EFileFlags::SafeStream
                                , m_eAccess
                            );

                            //
                            //  If the output encoding was one that required
                            //  a BOM the write one out.
                            //
                            if (m_bNeedsBOM)
                                *m_pstrmTarget << kCIDLib::chUniBOM;
                        }

                        catch(...)
                        {
                            #if CID_DEBUG_ON
                            TPopUp::PopUpMsg
                            (
                                CID_FILE
                                , CID_LINE
                                , L"CIDLib C++ Frameworks"
                                , 0
                                , L"Could not re-open the local log file after "
                                  L"a failed rollover. In order to recover, the "
                                  L"program will exit."
                            );
                            #endif
                            TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
                        }
                        break;
                    };
                };
            }
        }

        //
        //  Seek to the end so that we append.
        //
        //  NOTE:   This is NOT redundant. This can be logging to a shared
        //          file, so other processes may have written to it since
        //          we last did, so we won't always be pointing to the end.
        //
        m_pstrmTarget->SeekToEnd();

        // Stream out the basic info about who/what/where
        tmCurrent = logevToLog.enctLogged();
        *m_pstrmTarget  << tmCurrent << L"-"
                        << logevToLog.strProcess() << L": "
                        << logevToLog.strFacName() << L", "
                        << logevToLog.strThread() << L", "
                        << logevToLog.strFileName() << L"."
                        << logevToLog.c4LineNum() << L", "
                        << logevToLog.eSeverity() << L"/"
                        << logevToLog.eClass();

        // Only do the error stuff if there are any non-zero codes
        if (logevToLog.bAnyError())
        {
            *m_pstrmTarget  << L", " << facCIDLib().strMsg(kCIDMsgs::midGen_Error)
                            << L":" << logevToLog.errcId()
                            << L"/" << logevToLog.errcKrnlId()
                            << L"/" << logevToLog.errcHostId();
        }
        *m_pstrmTarget << L"\n{\n";

        // Now append the error text itself, indenting it over
        *m_pstrmTarget << L"   " << logevToLog.strErrText();

        // If there is aux text, log it next and indent it also.
        if (logevToLog.bHasAuxText())
            *m_pstrmTarget << L"\n   " << logevToLog.strAuxText();

        // If there's a stack dump, do that
        if (!logevToLog.strStackTrace().bIsEmpty())
        {
            *m_pstrmTarget  << logevToLog.strStackTrace()
                            << kCIDLib::NewLn;
        }

        // Close the brace, add new line, and flush the stream
        *m_pstrmTarget << L"\n}\n" << kCIDLib::FlushIt;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // If there is a sync mutex then unlock it
        if (m_pmtxSync)
            m_pmtxSync->Unlock();

        throw;
    }

    // If there is a sync mutex then unlock it
    if (m_pmtxSync)
        m_pmtxSync->Unlock();
}


// ---------------------------------------------------------------------------
//  TTextFileLogger: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TTextFileLogger::c8RolloverSize() const
{
    return m_c8RolloverSize;
}

tCIDLib::TCard8
TTextFileLogger::c8RolloverSize(const tCIDLib::TCard8 c8ToSet)
{
    m_c8RolloverSize = c8ToSet;
    return m_c8RolloverSize;
}



tCIDLib::TVoid
TTextFileLogger::Open(  const   tCIDLib::TCh* const     pszFile
                        ,       TTextConverter* const   ptcvtToAdopt
                        , const TResourceName&          rsnMutex)
{
    if (m_pstrmTarget)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLgr_AlreadyInitialized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // If no explicit converter, then use a UTF-8 converter
    TTextConverter* ptcvtActual = ptcvtToAdopt;
    if (!ptcvtActual)
        ptcvtActual = new TUTF8Converter;

    //
    //  If the converter format requires a BOM, then set a flag to remember
    //  this.
    //
    if ((ptcvtActual->strEncodingName() == L"UTF-8")
    ||  (ptcvtActual->strEncodingName() == L"UTF-16LE")
    ||  (ptcvtActual->strEncodingName() == L"UTF-16BE"))
    {
        m_bNeedsBOM = kCIDLib::True;
    }

    //
    //  Try to create or open the named mutex, if the user provided a name.
    //  Then lock it, so that we can know that we are doing the init
    //  uninterrupted.
    //
    if (!TResourceName::bIsNullObject(rsnMutex))
        m_pmtxSync = new TMutex(rsnMutex);

    TMtxLocker lockFile(m_pmtxSync);

    // See if it exists yet
    TPathStr pathTmp;
    TFileSys::QueryFullPath(pszFile, pathTmp);
    const tCIDLib::TBoolean bExisted = TFileSys::bExists(pathTmp);

    // If not already existing, then make sure the path gets created
    if (!bExisted)
    {
        pathTmp.bRemoveNameExt();
        TFileSys::MakePath(pathTmp);
    }

    // Create or open the text file stream
    m_pstrmTarget = new TTextFileOutStream
    (
        pszFile
        , tCIDLib::ECreateActs::OpenOrCreate
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SafeStream
        , m_eAccess
        , ptcvtActual
    );

    //
    //  If we created it, then write out a BOM if required. Else, seek to
    //  the end of the file.
    //
    if (!bExisted && m_bNeedsBOM)
        *m_pstrmTarget << kCIDLib::chUniBOM;
    else
        m_pstrmTarget->SeekToEnd();
}


tCIDLib::TVoid
TTextFileLogger::Open(  const   TString&                strFile
                        ,       TTextConverter* const   ptcvtToAdopt
                        , const TResourceName&          rsnMutex)
{
    // Just call the other version
    Open(strFile.pszBuffer(), ptcvtToAdopt, rsnMutex);
}

