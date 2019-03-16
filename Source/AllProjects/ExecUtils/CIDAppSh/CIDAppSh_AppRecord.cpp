//
// FILE NAME: CIDAppSh_AppRecord.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements TAppShRecord class.
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
#include    "CIDAppSh_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAppShRecord,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TAppShRecord
//  PREFIX: asr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAppShRecord: Constructors and Destructor
// ---------------------------------------------------------------------------
TAppShRecord::TAppShRecord( const   TString&                strTitle
                            , const TString&                strApp
                            , const TString&                strInitialPath
                            , const TString&                strParms
                            , const tCIDAppSh::EAppLevels   eLevel) :
    m_c4ShutdownTime(0)
    , m_eFlags(tCIDLib::EExtProcFlags::Detached)
    , m_eLevel(eLevel)
    , m_eStatus(tCIDAppSh::EStatus::WaitRun)
    , m_strApp(strApp)
    , m_strInitialPath(strInitialPath)
    , m_strParms(strParms)
    , m_strTitle(strTitle)
{
    // Do token replacement on the parameter string
    ReplaceTokens(m_strParms);
}

TAppShRecord::TAppShRecord( const   TString&                strTitle
                            , const TString&                strApp
                            , const TString&                strParms
                            , const tCIDAppSh::EAppLevels   eLevel) :
    m_c4ShutdownTime(0)
    , m_eFlags(tCIDLib::EExtProcFlags::Detached)
    , m_eLevel(eLevel)
    , m_eStatus(tCIDAppSh::EStatus::WaitRun)
    , m_strApp(strApp)
    , m_strParms(strParms)
    , m_strTitle(strTitle)
{
    // Do token replacement on the parameter string
    ReplaceTokens(m_strParms);
}

TAppShRecord::~TAppShRecord()
{
}


// ---------------------------------------------------------------------------
//  TAppShRecord: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TAppShRecord::bIsRunning() const
{
    return m_extpThis.bIsRunning();
}


tCIDLib::TBoolean TAppShRecord::bStart()
{
    // If running, then do nothing, and return false to say we didn't start it
    if (m_extpThis.bIsRunning())
        return kCIDLib::False;

    // Build up the full command line
    TString strFullCmdLine(L"\"");
    strFullCmdLine.Append(m_strApp);
    strFullCmdLine.Append(L"\" ");
    strFullCmdLine.Append(m_strParms);

    // And start it
    m_extpThis.Start(strFullCmdLine, m_strInitialPath, m_eFlags);

    // Return true to indicate that we started it
    return kCIDLib::True;
}

tCIDLib::TBoolean TAppShRecord::bStart(const TEnvironment& envToUse)
{
    // If running, then do nothing, and return false to say we didn't start it
    if (m_extpThis.bIsRunning())
        return kCIDLib::False;

    // Build up the full command line
    TString strFullCmdLine(m_strApp);
    strFullCmdLine.Append(kCIDLib::chSpace);
    strFullCmdLine.Append(m_strParms);

    // And start it, passing along the environment
    m_extpThis.Start(strFullCmdLine, m_strInitialPath, envToUse, m_eFlags);

    // Return true to indicate that we started it
    return kCIDLib::True;
}


tCIDLib::TBoolean
TAppShRecord::bWaitForDeath(        tCIDLib::EExitCodes&    eToFill
                            , const tCIDLib::TCard4         c4MilliSecs) const
{
    return m_extpThis.bWaitForDeath(eToFill, c4MilliSecs);
}


tCIDLib::TCard4 TAppShRecord::c4ShutdownTime() const
{
    return TTime::c4Millis() - m_c4ShutdownTime;
}


tCIDLib::EExtProcFlags TAppShRecord::eFlags() const
{
    return m_eFlags;
}

tCIDLib::EExtProcFlags
TAppShRecord::eFlags(const tCIDLib::EExtProcFlags eToSet)
{
    m_eFlags = eToSet;
    return m_eFlags;
}


TExternalProcess& TAppShRecord::extpThis()
{
    return m_extpThis;
}

const TExternalProcess& TAppShRecord::extpThis() const
{
    return m_extpThis;
}


tCIDLib::EPrioClasses TAppShRecord::ePriorityClass() const
{
    return m_extpThis.ePriorityClass();
}

tCIDLib::EPrioClasses
TAppShRecord::ePriorityClass(const tCIDLib::EPrioClasses eNewClass)
{
    return m_extpThis.ePriorityClass(eNewClass);
}


tCIDAppSh::EAppLevels TAppShRecord::eLevel() const
{
    return m_eLevel;
}


tCIDAppSh::EStatus TAppShRecord::eStatus() const
{
    return m_eStatus;
}

tCIDAppSh::EStatus TAppShRecord::eStatus(const tCIDAppSh::EStatus eToSet)
{
    m_eStatus = eToSet;
    return m_eStatus;
}


tCIDLib::EExitCodes
TAppShRecord::eWaitForDeath(const tCIDLib::TCard4 c4MilliSecs) const
{
    return m_extpThis.eWaitForDeath(c4MilliSecs);
}


const TString& TAppShRecord::strApp() const
{
    return m_strApp;
}


const TString& TAppShRecord::strExtra() const
{
    return m_strExtra;
}

const TString& TAppShRecord::strExtra(const TString& strToSet)
{
    m_strExtra = strToSet;
    return m_strExtra;
}


const TString& TAppShRecord::strInitialPath() const
{
    return m_strInitialPath;
}


const TString& TAppShRecord::strParms() const
{
    return m_strParms;
}


const TString& TAppShRecord::strTitle() const
{
    return m_strTitle;
}


// ---------------------------------------------------------------------------
//  TAppShRecord: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Replaces some standard tokens that are allowed in some of the parameters
tCIDLib::TVoid TAppShRecord::ReplaceTokens(TString& strTarget)
{
    tCIDLib::EFindRes eRes;

    do
    {
        eRes = strTarget.eReplaceToken(TSysInfo::strIPHostName(), L'h');
    }   while (eRes == tCIDLib::EFindRes::Found);

    do
    {
        eRes = strTarget.eReplaceToken(TSysInfo::strUserName(), L'u');
    }   while (eRes == tCIDLib::EFindRes::Found);

    if (strTarget.bTokenExists(L't'))
    {
        TTime tmCur(tCIDLib::ESpecialTimes::CurrentTime);
        tmCur.strDefaultFormat(TTime::strHHMMSS());
        do
        {
            eRes = strTarget.eReplaceToken(tmCur, L't');
        }   while (eRes == tCIDLib::EFindRes::Found);
    }
}


tCIDLib::TVoid TAppShRecord::StartShutdownClock()
{
    m_c4ShutdownTime = TTime::c4Millis();
}


