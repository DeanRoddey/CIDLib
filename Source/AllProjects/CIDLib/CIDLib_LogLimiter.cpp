//
// FILE NAME: CIDLib_LogLimiter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/2018
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
//  This file implements the TLogLimiter class.
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
RTTIDecls(TLogLimiter,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDLib_LogLimiter
{
    const tCIDLib::TCard4   c4ListSz = 64;
}


// ---------------------------------------------------------------------------
//   CLASS: TLogLimiter
//  PREFIX: loglim
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TLogLimiter: Constructors and Destructor
// -------------------------------------------------------------------
TLogLimiter::TLogLimiter(const tCIDLib::TCard4 c4ThresholdSecs) :

    m_c4Seconds(c4ThresholdSecs)
    , m_colList(CIDLib_LogLimiter::c4ListSz)
{
}

TLogLimiter::~TLogLimiter()
{
}


// -------------------------------------------------------------------
//  TLogLimiter: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TLogLimiter::bLogErr(const TLogEvent& errToLog)
{
    if (bShouldLog(errToLog))
    {
        TModule::LogEventObj(errToLog);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean
TLogLimiter::bLogErr(       TLogEvent&          errToLog
                    , const tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line)
{
    if (bShouldLog(errToLog))
    {
        errToLog.AddStackLevel(pszFile, c4Line);
        TModule::LogEventObj(errToLog);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// -------------------------------------------------------------------
//  TLogLimiter::TErrInfo: Constructors and destructor
// -------------------------------------------------------------------
TLogLimiter::TErrInfo::TErrInfo(const   TLogEvent&              errToLog
                                , const tCIDLib::TEncodedTime   enctNext) :

    m_c4Line(errToLog.c4LineNum())
    , m_eSev(errToLog.eSeverity())
    , m_enctNext(enctNext)
    , m_errcLogged(errToLog.errcId())
    , m_strFacName(errToLog.strFacName())
    , m_strFileName(errToLog.strFileName())
    , m_strProcess(errToLog.strProcess())
{
}

TLogLimiter::TErrInfo::~TErrInfo()
{
}


// -------------------------------------------------------------------
//  TLogLimiter: Private, non-virtual methods
// -------------------------------------------------------------------

// Check if we should log the passed guy and update our list as required
tCIDLib::TBoolean TLogLimiter::bShouldLog(const TLogEvent& errToLog)
{
    // See if it's in our list already
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        const TErrInfo& erriCur = m_colList[c4Index];

        if ((errToLog.c4LineNum() == erriCur.m_c4Line)
        &&  (errToLog.eSeverity() == erriCur.m_eSev)
        &&  (errToLog.errcId() == erriCur.m_errcLogged)
        &&  (errToLog.strFacName() == erriCur.m_strFacName)
        &&  (errToLog.strFileName() == erriCur.m_strFileName)
        &&  (errToLog.strProcess() == erriCur.m_strProcess))
        {
            break;
        }
    }

    // And now update as required
    tCIDLib::TBoolean bLogIt = kCIDLib::False;
    if (c4Index < c4Count)
    {
        TErrInfo& erriCur = m_colList[c4Index];

        // Check the time stamp. If it is before now, then the threshold has passed.
        const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        if (erriCur.m_enctNext < enctCur)
        {
            erriCur.m_enctNext = enctCur + (m_c4Seconds * kCIDLib::enctOneSecond);
            bLogIt = kCIDLib::True;
        }
    }
     else
    {
        // IF the list is full, flush it
        if (m_colList.c4ElemCount() >= CIDLib_LogLimiter::c4ListSz)
            m_colList.RemoveAll();

        // It's a new one, so add it and indicate it needs to be logged
        m_colList.objPlace(errToLog, TTime::enctNowPlusSecs(m_c4Seconds));
        bLogIt = kCIDLib::True;
    }
    return bLogIt;
}

