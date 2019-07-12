//
// FILE NAME: CIDLib_SystemInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1992
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
//  This file implements the TSysInfo class, which provides access to system
//  information.
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
RTTIDecls(TSysInfo,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_SystemInfo
{
    volatile tCIDLib::TBoolean bInitDone = kCIDLib::False;
}


// ---------------------------------------------------------------------------
//   CLASS: TSysInfo
//  PREFIX: sysi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSysInfo: Static data members
// ---------------------------------------------------------------------------
TSysInfo::TSpecPathList TSysInfo::s_apstrSpecPaths((TString*)nullptr);
tCIDLib::TBoolean       TSysInfo::s_bInstallMode = kCIDLib::False;
tCIDLib::TBoolean       TSysInfo::s_bTestMode = kCIDLib::False;
tCIDLib::TStrList*      TSysInfo::s_pcolCmdLineParms;
TString*                TSysInfo::s_pstrOSHostName;
TString*                TSysInfo::s_pstrIPHostName;
TString*                TSysInfo::s_pstrDefModuleLog;
TString*                TSysInfo::s_pstrNSAddr;
TString*                TSysInfo::s_pstrUserName;
TTextOutStream*         TSysInfo::s_pstrmErr;
TTextInStream*          TSysInfo::s_pstrmIn;
TTextOutStream*         TSysInfo::s_pstrmOut;
const tCIDLib::TCh*     TSysInfo::s_pszLogInfo;



// ---------------------------------------------------------------------------
//  TSysInfo: Public, static methods
// ---------------------------------------------------------------------------

//
//  See if we have a command line parameter that matches that passed find string.
//  if s
tCIDLib::TBoolean
TSysInfo::bFindCmdLineParm( const   TString&            strFind
                            , const tCIDLib::TBoolean   bCase)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();

    const tCIDLib::TCard4 c4Count = s_pcolCmdLineParms->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TString& strCur = s_pcolCmdLineParms->objAt(c4Index);

        tCIDLib::TBoolean bMatch;
        if (bCase)
            bMatch = strCur.bCompare(strFind);
        else
            bMatch = strCur.bCompareI(strFind);

        if (bMatch)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Same as above but looks for one that starts with a prefix and removes that
//  prefix from the returned value if found.
//
tCIDLib::TBoolean
TSysInfo::bFindCmdLinePref( const   TString&            strPref
                            ,       TString&            strToFill
                            , const tCIDLib::TBoolean   bCase)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();

    const tCIDLib::TCard4 c4Count = s_pcolCmdLineParms->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TString& strCur = s_pcolCmdLineParms->objAt(c4Index);

        tCIDLib::TBoolean bMatch;
        if (bCase)
            bMatch = strCur.bStartsWith(strPref);
        else
            bMatch = strCur.bStartsWithI(strPref);

        if (bMatch)
        {
            strToFill.CopyInSubStr(strPref, strPref.c4Length());
            return kCIDLib::False;
        }
    }
    return kCIDLib::False;
}


// Returns a basic check if the current user is an administrator
tCIDLib::TBoolean TSysInfo::bIsHostAdmin()
{
    return TKrnlSysInfo::bIsHostAdmin();
}


// Return the install mode flag from the command line
tCIDLib::TBoolean TSysInfo::bInstallMode()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return s_bInstallMode;
}


//
//  For now this is bogus. We haven't implemented any location services stuff
//  yet. But we want to have it in place. For testing purposes, if in debug
//  mode, we can get bogus values from an environment variable.
//
tCIDLib::TBoolean TSysInfo::bQueryLatLong(  tCIDLib::TFloat8&   f8Lat
                                            , tCIDLib::TFloat8& f8Long)
{
    #if CID_DEBUG_ON
    tCIDLib::TCh szTmp[64];
    if (TKrnlEnvironment::bFind(L"CID_FAUXLOCINFO", szTmp, c4MaxBufChars(szTmp)))
    {
        // Break it into two floating point numbers
        tCIDLib::TCh* pszCtx = 0;
        const tCIDLib::TCh* pszLat = TRawStr::pszStrTokenize(szTmp, L" ", &pszCtx);
        const tCIDLib::TCh* pszLong = TRawStr::pszStrTokenize(0, L" ", &pszCtx);

        if (pszLat && pszLong)
        {
            tCIDLib::TBoolean bLatGood, bLongGood;
            f8Lat = TRawStr::f8AsBinary(pszLat, bLatGood);
            f8Long = TRawStr::f8AsBinary(pszLong, bLongGood);

            if (bLatGood && bLongGood)
                return kCIDLib::True;
        }
    }

    // If anything is not right above, we fall through and return false
    #endif

    return kCIDLib::False;
}


// Get/set the test mode flag
tCIDLib::TBoolean TSysInfo::bTestMode()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return s_bTestMode;
}

tCIDLib::TBoolean TSysInfo::bTestMode(const tCIDLib::TBoolean bNewState)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    s_bTestMode = bNewState;
    return s_bTestMode;
}


// Return the number of (non-magic) command line parms
tCIDLib::TCard4 TSysInfo::c4CmdLineParmCount()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return s_pcolCmdLineParms->c4ElemCount();
}


// Return the count of CPUs in this host
tCIDLib::TCard4 TSysInfo::c4CPUCount()
{
    return TKrnlSysInfo::c4CPUCount();
}


// Return the total physical memory in bytes
tCIDLib::TCard8 TSysInfo::c8TotalPhysicalMem()
{
    return TKrnlSysInfo::c8TotalPhysicalMem();
}


// Return the CPU type
tCIDLib::ECPUTypes TSysInfo::eCPUType()
{
    return TKrnlSysInfo::eCPUType();
}


//
//  This provides access to the info in the /MODULELOG= parameter, or the
//  CID_MODULELOG environment variable, which is used to allow the user to
//  set verbose logging on specific modules.
//
tCIDLib::ESeverities TSysInfo::eDefModuleLog(const TString& strTarModule)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();

    //
    //  We'll return the default level if not set explicitly or invalidly
    //  set, which will log anything other that info and warnings.
    //
    tCIDLib::ESeverities eRet(tCIDLib::ESeverities::Failed);
    if (!s_pstrDefModuleLog)
        return eRet;

    //
    //  Let's run through it. We break it apart on semi-colons. Each
    //  value should be in the form 'module=value', without the quotes.
    //
    TString strModule;
    const tCIDLib::TCh* pszCur = s_pstrDefModuleLog->pszBuffer();
    const tCIDLib::TCh* pszEnd = pszCur + s_pstrDefModuleLog->c4Length();

    // Run up to the end
    while (pszCur < pszEnd)
    {
        // First find the = sign, which leaves us the name
        strModule.Clear();
        while ((pszCur < pszEnd) && (*pszCur != kCIDLib::chEquals))
        {
            if (!TRawStr::bIsAlpha(*pszCur))
            {
                // It can't be valid, so return a zero
                pszCur = pszEnd;
                break;
            }

            strModule.Append(*pszCur);
            pszCur++;
        }

        //
        //  If we forced a break out due to failure or hit the end
        //  before we got a legal value, give up and reutrn the default.
        //
        if (pszCur == pszEnd)
            break;

        //
        //  OK, we have a module name now. Skip the equals and get the
        //  the value out, which should be a single digit, and move
        //  past it in the process.
        //
        pszCur++;
        tCIDLib::TCh chVal = *pszCur++;

        //
        //  If it's the module we care about, then process the value
        //  Otherwise, it's not worth messing with.
        //
        if (strModule.bCompareI(strTarModule))
        {
            // If a valid digit and value, take it, else use the default
            if (TRawStr::bIsDigit(chVal))
            {
                chVal -= 0x30;
                if (chVal < tCIDLib::c4EnumOrd(tCIDLib::ESeverities::Count))
                    eRet = tCIDLib::ESeverities(chVal);
            }

            // And we can break out now
            break;
        }

        // We have to be on a semi-colon now, otherwise return the default
        if (*pszCur != kCIDLib::chSemiColon)
            break;

        // Move up to the next slot, if any
        pszCur++;
    }
    return eRet;
}




// Return the available physical memory in bytes
tCIDLib::TCard8 TSysInfo::c8AvailPhysicalMem()
{
    tCIDLib::TCard8 c8Ret;
    if (!TKrnlSysInfo::bQueryAvailPhysicalMem(c8Ret))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSysI_QueryAvailMem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c8Ret;
}


//
//  Return the list of command line arguments. This doesn't include the default 0th
//  one, so as to avoid a lot of confusion. We can return the list or a cursor to
//  the list.
//
const tCIDLib::TStrList& TSysInfo::colCmdLineParms()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_pcolCmdLineParms;
}

tCIDLib::TStrList::TCursor TSysInfo::cursCmdLineParms()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return tCIDLib::TStrList::TCursor(s_pcolCmdLineParms);
}


// Return the OS version and build info
tCIDLib::TVoid
TSysInfo::QueryOSInfo(  tCIDLib::TCard4&    c4OSMajVersion
                        , tCIDLib::TCard4&  c4OSMinVersion
                        , tCIDLib::TCard4&  c4OSRev
                        , tCIDLib::TCard4&  c4OSBuildNum)
{
    TKrnlSysInfo::QueryOSInfo
    (
        c4OSMajVersion
        , c4OSMinVersion
        , c4OSRev
        , c4OSBuildNum
    );
}


// REturn the OS info formatted into a string
tCIDLib::TVoid TSysInfo::QueryOSInfoStr(TString& strToFill)
{
    tCIDLib::TCard4 c4OSMajVersion;
    tCIDLib::TCard4 c4OSMinVersion;
    tCIDLib::TCard4 c4OSRev;
    tCIDLib::TCard4 c4OSBuildNum;

    TKrnlSysInfo::QueryOSInfo
    (
        c4OSMajVersion
        , c4OSMinVersion
        , c4OSRev
        , c4OSBuildNum
    );

    strToFill.SetFormatted(c4OSMajVersion);
    strToFill.Append(kCIDLib::chPeriod);
    strToFill.AppendFormatted(c4OSMinVersion);
    strToFill.Append(kCIDLib::chPeriod);
    strToFill.AppendFormatted(c4OSRev);
    strToFill.Append(L" [");
    strToFill.AppendFormatted(c4OSBuildNum);
    strToFill.Append(kCIDLib::chCloseBracket);
}


//
//  Sometimes higher level code wants to treat command line parameters as magical
//  and remove them after processing, so that application specific code doesn't
//  have to ignore them. We do that at our level and sometimes they want to as well.
//
//  Of course they should do it before starting any threads that might access the
//  command line info.
//
tCIDLib::TVoid TSysInfo::RemoveCmdLineParm(TCmdLineCursor& cursAt)
{
    // This has to have been done in order for them to have a cursor
    if (!CIDLib_SystemInfo::bInitDone)
    {
        CIDAssert2(L"System info init not done, at remove command line parm");
    }

    TBaseLock lockInit;
    s_pcolCmdLineParms->RemoveAt(cursAt);
}


// Provie access to the standard error stream, faulting it in if needed
TTextOutStream& TSysInfo::strmErr()
{
    if (!s_pstrmErr)
    {
        TBaseLock lockInit;
        if (!s_pstrmErr)
        {
            if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdErr))
            {
                TRawMem::pExchangePtr<TTextOutStream>
                (
                    &s_pstrmErr, new TTextFileOutStream(tCIDLib::EStdFiles::StdErr)
                );
            }
             else
            {
                //
                //  If its not redirected, it should be the same console object
                //  as std out. So create the standard output if we haven't
                //  already and set the error stream to the same thing.
                //
                if (!s_pstrmOut)
                    TRawMem::pExchangePtr<TTextOutStream>(&s_pstrmOut, new TOutConsole);

                TRawMem::pExchangePtr<TTextOutStream>(&s_pstrmErr, s_pstrmOut);
            }
        }
    }
    return *s_pstrmOut;
}


// Provie access to the standard input stream, faulting it in if needed
TTextInStream& TSysInfo::strmIn()
{
    if (!s_pstrmIn)
    {
        TBaseLock lockInit;
        if (!s_pstrmIn)
        {
            if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdIn))
            {
                TRawMem::pExchangePtr<TTextInStream>
                (
                    &s_pstrmIn, new TTextFileInStream(tCIDLib::EStdFiles::StdIn)
                );
            }
             else
            {
                TRawMem::pExchangePtr<TTextInStream>(&s_pstrmIn, new TInConsole);
            }
        }
    }
    return *s_pstrmIn;
}


// Provie access to the standard output stream, faulting it in if needed
TTextOutStream& TSysInfo::strmOut()
{
    if (!s_pstrmOut)
    {
        TBaseLock lockInit;
        if (!s_pstrmOut)
        {
            if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdOut))
            {
                TRawMem::pExchangePtr<TTextOutStream>
                (
                    &s_pstrmOut, new TTextFileOutStream(tCIDLib::EStdFiles::StdOut)
                );
            }
             else
            {
                TRawMem::pExchangePtr<TTextOutStream>(&s_pstrmOut, new TOutConsole);
            }
        }
    }
    return *s_pstrmOut;
}


// Return a specific (non-magic) command line parameter
const TString& TSysInfo::strCmdLineParmAt(const tCIDLib::TCard4 c4At)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return s_pcolCmdLineParms->objAt(c4At);
}


// Return the OS level host name
const TString& TSysInfo::strOSHostName()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_pstrOSHostName;
}


// Return the IP (DNS) host name, which may be different from the regular host name
const TString& TSysInfo::strIPHostName()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_pstrIPHostName;
}

// Return the name server address from the command in, if any
const TString& TSysInfo::strNSAddr()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_pstrNSAddr;
}


// REturn the value of the indicated special path
const TString& TSysInfo::strSpecialPath(const tCIDLib::ESpecialPaths ePath)
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_apstrSpecPaths[ePath];
}


// Return the current OS level user name
const TString& TSysInfo::strUserName()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return *s_pstrUserName;
}


// Set the name server address
tCIDLib::TVoid TSysInfo::SetNSAddr( const   TString&            strDNSAddr
                                    , const tCIDLib::TIPPortNum ippnToUse)
{
    // Make sure this is all atomic
    TBaseLock lockInit;

    s_pstrNSAddr = new TString(strDNSAddr);
    s_pstrNSAddr->Append(kCIDLib::chColon);
    s_pstrNSAddr->AppendFormatted(ippnToUse);
}


//
//  This does a standard CIDLib style command line parameter parse. It will
//  check each parameter. If it starts with an / it considers that an option.
//  If it's in the form /x=y, then it's an option with a value. Otherwise it's
//  just a value. It strips them down to key/value pairs, removing the slash
//  and equal signs. It fills in a collection of key/value/flag objects. If
//  one is an option, it set the flag, else it clears it.
//
tCIDLib::TCard4 TSysInfo::c4StdCmdLineParse(tCIDLib::TKVPFCollect& colToFill)
{
    colToFill.RemoveAll();

    TString strCurParm;
    TString strVal;
    TSysInfo::TCmdLineCursor cursParms = cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        strCurParm = *cursParms;

        if (strCurParm[0] == kCIDLib::chForwardSlash)
        {
            // Cut the slash off
            strCurParm.Cut(0, 1);

            //
            //  Split it on an equal sign if it has one. If so, it's an option with a
            //  value, else an option without one.
            //
            if (strCurParm.bSplit(strVal, kCIDLib::chEquals))
            {
                // An option with a value
                colToFill.objAdd(TKeyValFPair(strCurParm, strVal, kCIDLib::True));
            }
             else
            {
                // An option but no value
                colToFill.objAdd(TKeyValFPair(strCurParm, TString::strEmpty(), kCIDLib::True));
            }
        }
         else
        {
            // Not an option type
            colToFill.objAdd(TKeyValFPair(strCurParm, TString::strEmpty()));
        }
    }
    return colToFill.c4ElemCount();
}



// ---------------------------------------------------------------------------
//  TSysInfo: Hidden Constructors and Destructor
// ---------------------------------------------------------------------------
TSysInfo::TSysInfo()
{
}

TSysInfo::~TSysInfo()
{
}


// ---------------------------------------------------------------------------
//  TSysInfo: Protected, static methods
// ---------------------------------------------------------------------------
const tCIDLib::TCh* TSysInfo::pszLogInfo()
{
    if (!CIDLib_SystemInfo::bInitDone)
        DoInit();
    return s_pszLogInfo;
}


// ---------------------------------------------------------------------------
//  TSysInfo: Private, static methods
// ---------------------------------------------------------------------------

//
//  This method handles lazy init of our module. BE VERY CAREFUL about
//  creating any objects here (at least derived from TObject), since it could
//  cause some circular freakouts!
//
tCIDLib::TVoid TSysInfo::DoInit()
{
    // If init is already done, then just return
    if (CIDLib_SystemInfo::bInitDone)
        return;

    // Make sure this is all atomic
    TBaseLock lockInit;

    // Check again in case we were beat to it
    if (CIDLib_SystemInfo::bInitDone)
        return;

    //
    //  Create the command line arguments collection. Make it big enough for all
    //  of the arguments even though we may not end up exposing all of them.
    //
    //  We take a bit of risk here creating such a 'high level' object but to
    //  have the parms exposed in such a convenient way is well worth it.
    //
    tCIDLib::TCard4 c4ArgCount = TKrnlSysInfo::c4CmdLineArgCount();
    s_pcolCmdLineParms = new tCIDLib::TStrList(c4ArgCount);

    // Start after the magic 0th parameter
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4ArgCount; c4Index++)
    {
        // Get a pointer to this param's text
        const tCIDLib::TCh* pszVal;
        if (!TKrnlSysInfo::bCmdLineArg(c4Index, pszVal))
        {
            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , L"CIDLib System Info Subsystem"
                , 0
                , kCIDErrs::errcSysI_GetCmdLineParm
                , 0
                , 0
                , L"Could not get command line parameters"
            );
            return;
        }

        //
        //  Check to see if its a special parm. If so, then we set the
        //  appropriate flag and remove it from the list.
        //
        if (TRawStr::bCompareStrNI(pszVal, L"/ModuleLog=", 11))
        {
            s_pstrDefModuleLog = new TString(pszVal + 11);
        }
         else if (TRawStr::bCompareStrI(pszVal, L"/TestMode"))
        {
            s_bTestMode = kCIDLib::True;
        }
         else if (TRawStr::bCompareStrI(pszVal, L"/InstallMode"))
        {
            s_bInstallMode = kCIDLib::True;
        }
         else if (TRawStr::bCompareStrNI(pszVal, L"/LocalLog=", 10))
        {
            s_pszLogInfo = TRawStr::pszReplicate(pszVal + 10);
        }
         else if (TRawStr::bCompareStrNI(pszVal, L"/NSAddr=", 8))
        {
            s_pstrNSAddr = new TString(pszVal + 8);
        }
         else if (TRawStr::bCompareStrNI(pszVal, L"/GlobalLogMode=", 15))
        {
            //
            //  We need to set the global logging mode, which is set
            //  statically on the TFacility class.
            //
            tCIDLib::ESeverities eToSet;
            pszVal += 15;
            if (TRawStr::bCompareStrI(pszVal, L"Info"))
            {
                eToSet = tCIDLib::ESeverities::Info;
            }
             else if (TRawStr::bCompareStrI(pszVal, L"Warn"))
            {
                eToSet = tCIDLib::ESeverities::Warn;
            }
             else if (TRawStr::bCompareStrI(pszVal, L"Failed"))
            {
                eToSet = tCIDLib::ESeverities::Failed;
            }
             else if (TRawStr::bCompareStrI(pszVal, L"ProcFatal"))
            {
                eToSet = tCIDLib::ESeverities::ProcFatal;
            }
             else if (TRawStr::bCompareStrI(pszVal, L"SysFatal"))
            {
                eToSet = tCIDLib::ESeverities::SysFatal;
            }
             else
            {
                eToSet = tCIDLib::ESeverities::Status;
            }
            TFacility::eGlobalLogMode(eToSet);
        }
         else
        {
            // Wasn't a magic parameter so store it
            s_pcolCmdLineParms->objAdd(TString(pszVal));
        }
    }

    // Use a local buffer plenty long enough for the strings we'll load
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen];

    //
    //  If there wasn't an explicit entry for the Name Server address, then
    //  see if its in the environment.
    //
    if (!s_pstrNSAddr)
    {
        if (TKrnlEnvironment::bFind(L"CID_NSADDR", szTmp, kCIDLib::c4MaxPathLen))
            s_pstrNSAddr = new TString(szTmp);
        else
            s_pstrNSAddr = new TString;
    }

    // Try to get the user name
    if (!TKrnlSysInfo::bQueryUserName(szTmp, kCIDLib::c4MaxPathLen))
    {
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , L"CIDLib System Info Subsystem"
            , 0
            , kCIDErrs::errcSysI_QueryUserName
            , 0
            , 0
            , L"Could not obtain user name from system"
        );
    }
    s_pstrUserName = new TString(szTmp);

    // If the CID_MODULELOG env variable is set, then store the info
    if (TKrnlEnvironment::bFind(L"CID_MODULELOG", szTmp, kCIDLib::c4MaxPathLen))
    {
        // Only do this if we didn't get anything on the command line
        if (!s_pstrDefModuleLog)
            s_pstrDefModuleLog = new TString(szTmp);
    }

    // Do all the special paths
    tCIDLib::ForEachE<tCIDLib::ESpecialPaths>
    (
        [&szTmp](const tCIDLib::ESpecialPaths ePath)
        {
            if (TKrnlSysInfo::bQuerySpecialPath(szTmp, kCIDLib::c4MaxPathLen, ePath))
                s_apstrSpecPaths[ePath] = new TString(szTmp);
            else
                s_apstrSpecPaths[ePath] = new TString(L"Special Path Not Found");
        }
    );

    // And finally the host names, the regular and IP oriented ones
    s_pstrOSHostName = new TString(TKrnlSysInfo::pszNodeName());

    TKrnlString kstrHost;
    if (!TKrnlIP::bQueryLocalName(kstrHost))
    {
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , L"CIDLib System Info Subsystem"
            , 0
            , kCIDErrs::errcSysI_QueryUserName
            , 0
            , 0
            , L"Could not obtain IP host name from system"
        );
    }
    s_pstrIPHostName = new TString(kstrHost.pszValue());

    // Mark the init done now. MUST be done last!
    CIDLib_SystemInfo::bInitDone = kCIDLib::True;
}


