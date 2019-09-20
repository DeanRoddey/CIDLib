//
// FILE NAME: CIDLib_LogEvent.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/07/1993
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
//  This file provides support for logging and error reporting. It implements
//  the very simple TLogEvent class. This class is used by all Facilities to
//  log events and throw errors.
//
//  TLogEvent objects are almost never created manually. TModule provides a set
//  of logging/throwing methods that will create the logevor and log it or throw
//  it or both. All facilities inherit this capability, so there is never any
//  reason to build logevor objects out in the outside world.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TLogEvent,TObject)
AdvRTTIAlias(TLogEvent,TError)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_LogEvent
{
    // -----------------------------------------------------------------------
    //  Streaming stuff.
    //
    //  To compact the format a bit, and leave space for adding more flags
    //  in the future with minimal overhead, we stream the boolean members
    //  into a TCard2 value, and need a set of masks to put them in and get
    //  them out.
    //
    //  We also need a marker for the aux text, which isn't streamed unless
    //  it's there, so we need to know whether to stream it back out. So we
    //  use the top bit of the flags value for this purpose.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2Logged        = 0x0001;
    const tCIDLib::TCard2   c2Reported      = 0x0002;
    const tCIDLib::TCard2   c2AuxText       = 0x8000;


    // -----------------------------------------------------------------------
    //  The persistent format version.
    //
    //  Version 2 -
    //      We added the stack dump string.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtVersion    = 2;



    // -----------------------------------------------------------------------
    //  Format version for the kernel error that we provide global streaming
    //  operators for.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2KErrFmtVersion = 1;
}



// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------

//
//  We provide global streaming operators for the kernel level error class
//  since it cannot provide them itself.
//
TBinInStream& operator>>(TBinInStream& strmToReadFrom, TKrnlError& kerrToRead)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDLib_LogEvent::c2KErrFmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , TString(L"TKrnlError")
        );
    }

    tCIDLib::TErrCode errcTmp;
    tCIDLib::TOSErrCode errcHostTmp;
    strmToReadFrom  >> errcTmp
                    >> errcHostTmp;

    // And we should end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    // Set the object now
    kerrToRead.Set(errcTmp, errcHostTmp);

    return strmToReadFrom;
}

TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TKrnlError& kerrToWrite)
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_LogEvent::c2KErrFmtVersion
                    << kerrToWrite.errcId()
                    << kerrToWrite.errcHostId()
                    << tCIDLib::EStreamMarkers::EndObject;

    return strmToWriteTo;
}





// ---------------------------------------------------------------------------
//   CLASS: TLogEvent
//  PREFIX: logev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLogEvent: Constructors and Destructor
// ---------------------------------------------------------------------------
TLogEvent::TLogEvent() :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(0)
    , m_eClass(tCIDLib::EErrClasses::Unknown)
    , m_enctLogged(0)
    , m_errcHostId(0)
    , m_errcId(0)
    , m_errcKrnlId(0)
    , m_eSeverity(tCIDLib::ESeverities::Info)
    , m_pstrAuxText(nullptr)
    , m_strHostName(TSysInfo::strIPHostName())
{
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const TString&                strErrText
                    , const tCIDLib::ESeverities    eSev
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(0)
    , m_errcId(0)
    , m_errcKrnlId(0)
    , m_eSeverity(eSev)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name, and the process name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const TString&                strErrText
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSev
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(0)
    , m_errcId(0)
    , m_errcKrnlId(0)
    , m_eSeverity(eSev)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    // Copy the aux text
    m_pstrAuxText = new TString(strAuxText);

    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcId
                    , const TString&                strErrText
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(0)
    , m_errcId(errcId)
    , m_errcKrnlId(0)
    , m_eSeverity(eSeverity)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    // Copy the aux text
    m_pstrAuxText = new TString(strAuxText);

    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name, and the process name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcId
                    , const TString&                strErrText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(0)
    , m_errcId(errcId)
    , m_errcKrnlId(0)
    , m_eSeverity(eSeverity)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name, and the process name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcId
                    , const TKrnlError&             kerrIds
                    , const TString&                strErrText
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(kerrIds.errcHostId())
    , m_errcId(errcId)
    , m_errcKrnlId(kerrIds.errcId())
    , m_eSeverity(eSeverity)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    // Copy the aux text
    m_pstrAuxText = new TString(strAuxText);

    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name, and the process name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const  TString&                strFacName
                    , const TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcId
                    , const TKrnlError&             kerrIds
                    , const TString&                strErrText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) :

    m_bLogged(kCIDLib::False)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(c4LineNum)
    , m_eClass(eClass)
    , m_enctLogged(0)
    , m_errcHostId(kerrIds.errcHostId())
    , m_errcId(errcId)
    , m_errcKrnlId(kerrIds.errcId())
    , m_eSeverity(eSeverity)
    , m_pstrAuxText(nullptr)
    , m_strErrText(strErrText)
    , m_strFacName(strFacName)
    , m_strHostName(TSysInfo::strIPHostName())
{
    //
    //  Strip the path part off the file name, because its fed by the
    //  file macro that sometimes puts the whole friggin path on there.
    //
    tCIDLib::TCard4 c4Dummy;
    if (strFileName.bFirstOccurrence(L'\\', c4Dummy))
    {
        TPathStr pathTmp(strFileName);
        pathTmp.bQueryNameExt(m_strFileName);
    }
     else
    {
        m_strFileName = strFileName;
    }

    // Get the calling thread's name, and the process name
    TThread* pthrErr = TThread::pthrCaller();
    if (pthrErr)
        m_strThread = pthrErr->strName();
    else
        m_strThread = L"????";
    m_strProcess = TProcess::strProcessName();
}

TLogEvent::TLogEvent(const TLogEvent& logevSrc) :

    m_bLogged(logevSrc.m_bLogged)
    , m_bReported(kCIDLib::False)
    , m_c4LineNum(logevSrc.m_c4LineNum)
    , m_eClass(logevSrc.m_eClass)
    , m_enctLogged(logevSrc.m_enctLogged)
    , m_errcHostId(logevSrc.m_errcHostId)
    , m_errcId(logevSrc.m_errcId)
    , m_errcKrnlId(logevSrc.m_errcKrnlId)
    , m_eSeverity(logevSrc.m_eSeverity)
    , m_pstrAuxText(nullptr)
    , m_strErrText(logevSrc.m_strErrText)
    , m_strFacName(logevSrc.m_strFacName)
    , m_strFileName(logevSrc.m_strFileName)
    , m_strHostName(logevSrc.m_strHostName)
    , m_strProcess(logevSrc.m_strProcess)
    , m_strStackTrace(logevSrc.m_strStackTrace)
    , m_strThread(logevSrc.m_strThread)
{
    // Copy the aux text if any
    if (logevSrc.m_pstrAuxText)
        m_pstrAuxText = new TString(*logevSrc.m_pstrAuxText);
}

TLogEvent::~TLogEvent()
{
    // Clean up the aux text if any
    delete m_pstrAuxText;
}


// ---------------------------------------------------------------------------
//  TLogEvent: Public, virtual methods
// ---------------------------------------------------------------------------
TLogEvent& TLogEvent::operator=(const TLogEvent& logevSrc)
{
    if (this != &logevSrc)
    {
        m_bLogged       = logevSrc.m_bLogged;
        m_bReported     = logevSrc.m_bReported;
        m_c4LineNum     = logevSrc.m_c4LineNum;
        m_eClass        = logevSrc.m_eClass;
        m_eSeverity     = logevSrc.m_eSeverity;
        m_enctLogged    = logevSrc.m_enctLogged;
        m_errcHostId    = logevSrc.m_errcHostId;
        m_errcId        = logevSrc.m_errcId;
        m_errcKrnlId    = logevSrc.m_errcKrnlId;
        m_strErrText    = logevSrc.m_strErrText;
        m_strFacName    = logevSrc.m_strFacName;
        m_strFileName   = logevSrc.m_strFileName;
        m_strHostName   = logevSrc.m_strHostName;
        m_strProcess    = logevSrc.m_strProcess;
        m_strStackTrace = logevSrc.m_strStackTrace;
        m_strThread     = logevSrc.m_strThread;

        // Copy over the aux text if any
        if (logevSrc.m_pstrAuxText)
        {
            // Fault in one if we don't have one, else assign
            if (!m_pstrAuxText)
                m_pstrAuxText = new TString(*logevSrc.m_pstrAuxText);
            else
                *m_pstrAuxText = *logevSrc.m_pstrAuxText;
        }
         else
        {
            // None in the source, so just clean out ours if we have one
            if (m_pstrAuxText)
                m_pstrAuxText->Clear();
        }
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TLogEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Adds a new stack dump line to the stack dump
tCIDLib::TVoid
TLogEvent::AddStackLevel(const TString& strFile, const tCIDLib::TCard4 c4Line)
{
    //
    //  Just in case, don't let it grow too large. If we got stuck in some
    //  recursive freakout, we wouldn't want to eat the system alive by
    //  creating an enormous stack trace. Any realistic trace should easily
    //  fit within this limit, or considerably less.
    //
    if (m_strStackTrace.c4Length() < 2048)
    {
        m_strStackTrace.Append(L"\n      <");
        m_strStackTrace.Append(TProcess::strProcessName());
        m_strStackTrace.Append(L"> ");
        m_strStackTrace.Append(strFile);
        m_strStackTrace.Append(L" - ");
        m_strStackTrace.AppendFormatted(c4Line);
    }
}


//
//  Formats this object out to text. It allows them to pass a time object
//  that we use to format the time stamp.
//
tCIDLib::TVoid
TLogEvent::AdvFormat(TTextOutStream& strmTar, TTime& tmFmt) const
{
    static const tCIDLib::TCh* const pszComma = L", ";

    // Format the logevent info into the string
    tmFmt = m_enctLogged;
    strmTar << tmFmt << kCIDLib::chHyphenMinus
            << m_strHostName << pszComma
            << m_strProcess << pszComma
            << m_strThread;

    // Now the other info goes inside braces, indented
    strmTar << L"\n{\n    "
            << m_strFacName << pszComma
            << m_strFileName
            << kCIDLib::chPeriod << m_c4LineNum
            << pszComma << m_eSeverity
            << kCIDLib::chForwardSlash << m_eClass;

    // Only do the error stuff if there are any non-zero codes
    if (bAnyError())
    {
        strmTar << L", Error: "
                << m_errcId
                << kCIDLib::chForwardSlash << m_errcKrnlId
                << kCIDLib::chForwardSlash << m_errcHostId;
    }

    // Now append the error text itself, indenting it over
    strmTar << L"\n    " << m_strErrText;

    // If there is aux text, log it next and indent it also.
    if (m_pstrAuxText && !m_pstrAuxText->bIsEmpty())
        strmTar << L"\n    " << *m_pstrAuxText;

    // If there's a stack dump, do that
    if (!m_strStackTrace.bIsEmpty())
    {
        strmTar << m_strStackTrace
                << kCIDLib::NewLn;
    }

    // Add the closing brace, add new line, and flush the stream
    strmTar << L"\n}\n" << kCIDLib::FlushIt;
}


// Return whether any of our error ids are non-zero
tCIDLib::TBoolean TLogEvent::bAnyError() const
{
    return (m_errcHostId || m_errcKrnlId || m_errcId);
}


//
//  Check whether this event has the same error id and facility name as
//  the passed ones.
//
tCIDLib::TBoolean
TLogEvent::bCheckEvent( const   TString&            strModName
                        , const tCIDLib::TErrCode   errcToCheck) const
{
    if ((m_errcId == errcToCheck) && (strModName == m_strFacName))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TLogEvent::bCheckEvent( const   tCIDLib::TCh* const pszModName
                        , const tCIDLib::TErrCode   errcToCheck) const
{
    if ((m_errcId == errcToCheck) && (m_strFacName == pszModName))
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Indicate if we have any aux text or not. Since the aux text is only
//  faulted in if used, this allows nicer client code to avoid forcing it
//  to fault in if it's not been already.
//
tCIDLib::TBoolean TLogEvent::bHasAuxText() const
{
    if (m_pstrAuxText == 0)
        return kCIDLib::False;
    return !m_pstrAuxText->bIsEmpty();
}


//
//  Get/set the 'has been logged already' flag. The set is mostly for unit tests so
//  that it can be cleared to make sure it gets appropriately reset.
//
tCIDLib::TBoolean TLogEvent::bLogged() const
{
    return m_bLogged;
}

tCIDLib::TBoolean TLogEvent::bLogged(const tCIDLib::TBoolean bToSet)
{
    m_bLogged = bToSet;
    return m_bLogged;
}


// Get/set the 'has been reported' flag
tCIDLib::TBoolean TLogEvent::bReported() const
{
    return m_bReported;
}

tCIDLib::TBoolean TLogEvent::bReported(const tCIDLib::TBoolean bToSet) const
{
    m_bReported = bToSet;
    return m_bReported;
}


//
//  Check if this is the same event, which means it has the same error
//  id and is from the same facility as the passed log event.
//
tCIDLib::TBoolean TLogEvent::bSameEvent(const TLogEvent& errToCheck) const
{
    return (m_errcId == errToCheck.m_errcId)
           && (m_strFacName == errToCheck.m_strFacName);
}


// Get/set the line number
tCIDLib::TCard4 TLogEvent::c4LineNum() const
{
    return m_c4LineNum;
}

tCIDLib::TCard4 TLogEvent::c4LineNum(const tCIDLib::TCard4 c4NewLineNum)
{
    m_c4LineNum = c4NewLineNum;
    return m_c4LineNum;
}


// Get/set the time logged stamp
tCIDLib::TEncodedTime TLogEvent::enctLogged() const
{
    return m_enctLogged;
}

tCIDLib::TEncodedTime TLogEvent::enctLogged(const tCIDLib::TEncodedTime enctNew)
{
    m_enctLogged = enctNew;
    return m_enctLogged;
}


// Get/set the host id if any
tCIDLib::TOSErrCode TLogEvent::errcHostId() const
{
    return m_errcHostId;
}

tCIDLib::TOSErrCode
TLogEvent::errcHostId(const tCIDLib::TOSErrCode errcNew)
{
    m_errcHostId = errcNew;
    return m_errcHostId;
}


// Get/set the error id
tCIDLib::TErrCode TLogEvent::errcId() const
{
    return m_errcId;
}

tCIDLib::TErrCode TLogEvent::errcId(const tCIDLib::TErrCode errcNew)
{
    m_errcId = errcNew;
    return m_errcId;
}


// Get/set the kernel level error id if any
tCIDLib::TErrCode TLogEvent::errcKrnlId() const
{
    return m_errcKrnlId;
}

tCIDLib::TErrCode TLogEvent::errcKrnlId(const tCIDLib::TErrCode errcNew)
{
    m_errcKrnlId = errcNew;
    return m_errcKrnlId;
}


// Get/set the error class
tCIDLib::EErrClasses TLogEvent::eClass() const
{
    return m_eClass;
}

tCIDLib::EErrClasses
TLogEvent::eClass(const tCIDLib::EErrClasses eClass)
{
    m_eClass = eClass;
    return m_eClass;
}


// Get/set the severity level
tCIDLib::ESeverities TLogEvent::eSeverity() const
{
    return m_eSeverity;
}

tCIDLib::ESeverities
TLogEvent::eSeverity(const tCIDLib::ESeverities eNewSev)
{
    m_eSeverity = eNewSev;
    return m_eSeverity;
}


// Reset this object back to default values
tCIDLib::TVoid TLogEvent::Reset()
{
    m_bLogged       = kCIDLib::False;
    m_bReported     = kCIDLib::False;
    m_c4LineNum     = 0;
    m_eClass        = tCIDLib::EErrClasses::Unknown;
    m_enctLogged    = 0;
    m_errcHostId    = 0;
    m_errcId        = 0;
    m_errcKrnlId    = 0;
    m_eSeverity     = tCIDLib::ESeverities::Info;

    m_strErrText.Clear();
    m_strFacName.Clear();
    m_strFileName.Clear();
    m_strProcess.Clear();
    m_strStackTrace.Clear();
    m_strThread.Clear();

    m_strHostName = TSysInfo::strIPHostName();
    if (m_pstrAuxText)
        m_pstrAuxText->Clear();
}


//
//  Get/set the aux text. If we've not faultd it in yet, then we do so
//  in either case.
//
const TString& TLogEvent::strAuxText() const
{
    if (!m_pstrAuxText)
        m_pstrAuxText = new TString;

    return *m_pstrAuxText;
}

const TString& TLogEvent::strAuxText(const TString& strNewText)
{
    if (!m_pstrAuxText)
        m_pstrAuxText = new TString(strNewText);
    else
        *m_pstrAuxText = strNewText;

    return *m_pstrAuxText;
}


// Get/set the error text
const TString& TLogEvent::strErrText() const
{
    return m_strErrText;
}

const TString& TLogEvent::strErrText(const TString& strNewText)
{
    m_strErrText = strNewText;
    return m_strErrText;
}


// Get/set the facility name
const TString& TLogEvent::strFacName() const
{
    return m_strFacName;
}

const TString& TLogEvent::strFacName(const TString& strNewName)
{
    m_strFacName = strNewName;
    return m_strFacName;
}


// Get/set the file name
const TString& TLogEvent::strFileName() const
{
    return m_strFileName;
}

const TString& TLogEvent::strFileName(const TString& strToSet)
{
    m_strFileName = strToSet;
    return m_strFileName;
}


// Get/set the host name
const TString& TLogEvent::strHostName() const
{
    return m_strHostName;
}

const TString& TLogEvent::strHostName(const TString& strToSet)
{
    m_strHostName = strToSet;
    return m_strHostName;
}


const TString& TLogEvent::strProcess() const
{
    return m_strProcess;
}

const TString& TLogEvent::strProcess(const TString& strToSet)
{
    m_strProcess = strToSet;
    return m_strProcess;
}


// Get the stack dump string
const TString& TLogEvent::strStackTrace() const
{
    return m_strStackTrace;
}


// Get/Set the thread name
const TString& TLogEvent::strThread() const
{
    return m_strThread;
}

const TString& TLogEvent::strThread(const TString& strToSet)
{
    m_strThread = strToSet;
    return m_strThread;
}


//
//  Set the 'has been logged' flag and time stamp. If the time stamp is already
//  set we don't change it. They have to set it explicitly if they want to change
//  it otherwise. This is mostly for the logging framework to call on events that
//  are about to be logged and we don't want to modify ones that are being logged
//  after they've already been created and set up.
//
tCIDLib::TVoid TLogEvent::SetLogged() const
{
    m_bLogged = kCIDLib::True;
    if (!m_enctLogged)
        m_enctLogged = TTime::enctNow();
}



// ---------------------------------------------------------------------------
//  TLogEvent: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLogEvent::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // We just call the advanced format method with a default time format
    TTime tmLogged(m_enctLogged);
    tmLogged.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
    AdvFormat(strmToWriteTo, tmLogged);
}


tCIDLib::TVoid TLogEvent::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Check for a start marker and valid format version
    const tCIDLib::TCard2 c2FmtVersion = TBinInStream::c2CheckFmtVersion
    (
        strmToReadFrom
        , tCIDLib::EStreamMarkers::StartObject
        , CIDLib_LogEvent::c2FmtVersion
        , clsThis()
        , CID_FILE
        , CID_LINE
    );


    // Stream out our raft of members
    tCIDLib::TCard2 c2Flags;
    strmToReadFrom  >> c2Flags
                    >> m_c4LineNum
                    >> m_enctLogged
                    >> m_errcHostId
                    >> m_errcId
                    >> m_errcKrnlId
                    >> m_eClass
                    >> m_eSeverity
                    >> m_strErrText
                    >> m_strFacName
                    >> m_strFileName
                    >> m_strHostName
                    >> m_strThread
                    >> m_strProcess;

    // Break the flags back out of their compressed format
    m_bLogged = (c2Flags & CIDLib_LogEvent::c2Logged) != 0;
    m_bReported  = (c2Flags & CIDLib_LogEvent::c2Reported) != 0;

    // If we stored aux text, then pull it back out
    if (c2Flags & CIDLib_LogEvent::c2AuxText)
    {
        if (!m_pstrAuxText)
            m_pstrAuxText = new TString;
        strmToReadFrom >> *m_pstrAuxText;
    }
     else
    {
        if (m_pstrAuxText)
            m_pstrAuxText->Clear();
    }

    // If V2, stream in the stack dump. Else clear it
    if (c2FmtVersion == 1)
        m_strStackTrace.Clear();
    else
        strmToReadFrom >> m_strStackTrace;

    // And check for the end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TLogEvent::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Get the boolean flags into a masked TCard2 value
    tCIDLib::TCard2 c2Flags = 0;
    if (m_bLogged)
        c2Flags |= CIDLib_LogEvent::c2Logged;
    if (m_bReported)
        c2Flags |= CIDLib_LogEvent::c2Reported;

    // And we use the high bit to indicate aux text presence
    if (m_pstrAuxText)
        c2Flags |= CIDLib_LogEvent::c2AuxText;

    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_LogEvent::c2FmtVersion
                    << c2Flags
                    << m_c4LineNum
                    << m_enctLogged
                    << m_errcHostId
                    << m_errcId
                    << m_errcKrnlId
                    << m_eClass
                    << m_eSeverity
                    << m_strErrText
                    << m_strFacName
                    << m_strFileName
                    << m_strHostName
                    << m_strThread
                    << m_strProcess;

    // And do the optional aux text
    if (m_pstrAuxText)
        strmToWriteTo << *m_pstrAuxText;

    // Write out the stack dump and the end marker
    strmToWriteTo   << m_strStackTrace
                    << tCIDLib::EStreamMarkers::EndObject;
}

