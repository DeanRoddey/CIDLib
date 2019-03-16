//
// FILE NAME: CIDLogAdmin.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/15/2001
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
//  This is a small command line utility to administer the CIDLib Log
//  Server and remotely set logging levels in servers.
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
#include    "CIDRegX.hpp"
#include    "CIDOrbUC.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local data
// ----------------------------------------------------------------------------
namespace CIDLogAdmin
{
    // ------------------------------------------------------------------------
    //  Local types
    // ------------------------------------------------------------------------
    enum ECmds
    {
        ECmd_Dump
        , ECmd_RemoveAll
        , ECmd_FullDump
        , ECmd_SetLogLevel
        , ECmd_SetLogTrace
    };


    // ------------------------------------------------------------------------
    //  Local data
    //
    //  bErrsOnly
    //      The /ErrOnly flag will cause us to only show events that have
    //      a severity of APIFailed or greater.
    //
    //  bStatusOnly
    //      The /StatusOnly flag will cause us to only show events that have
    //      the 'status' severity. Can't be on if bErrsOnly is on.
    //
    //  eCmd
    //      The command that is to be done. If no parameters are provided,
    //      then it defaults to a dump, which is always safe.
    //
    //  regxProc
    //  regxThread
    //      These are the regular expressions that we create for the string
    //      fields that are supported by the filtering mechanism. The
    //      expressions for these are set via /Proc= and /Thread= parms.
    // ------------------------------------------------------------------------
    tCIDLib::TBoolean   bErrsOnly;
    tCIDLib::TBoolean   bStatusOnly;
    ECmds               eCmd = ECmd_Dump;
    TRegEx              regxProc;
    TRegEx              regxThread;
    TString             strOutFile;
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

// If they run us without providing correct parms
static tCIDLib::TVoid ShowUsage()
{
    TSysInfo::strmOut() << L"Usage:\n"
                           L"   CIDLogAdmin cmd [options]\n\n"
                           L"Cmds:\n"
                           L"    dump           - Dump out log events\n"
                           L"    flush          - Flush all log events\n"
                           L"    fulldump       - Dump the entire log contents\n"
                           L"\nOptions:\n"
                           L"   /NSAddr=xx      - Where to find Name Server\n"
                           L"   /Proc=xx        - Set reg ex process name filter\n"
                           L"   /Thread=xx      - Set reg ex thread name filter\n"
                           L"   /ErrOnly        - Only show error events\n"
                           L"   /StatusOnly     - Only show status events\n"
                           L"   /OutFile=xxx    - Path to out file, for full dump\n"
                           L"\nAlso:\n"
                           L"   setlogtrace binding [Yes|No]\n"
                           L"   setloglevel binding module level\n\n"
                        << kCIDLib::FlushIt;
}


static tCIDLib::TBoolean bApplyFilter(const TLogEvent& logevCur)
{
    if (CIDLogAdmin::bErrsOnly)
    {
        if ((logevCur.eSeverity() < tCIDLib::ESeverities::Failed)
        ||  (logevCur.eSeverity() == tCIDLib::ESeverities::Status))
        {
            return kCIDLib::False;
        }
    }
     else if (CIDLogAdmin::bStatusOnly
          &&  (logevCur.eSeverity() != tCIDLib::ESeverities::Status))
    {
        return kCIDLib::False;
    }

    if (!CIDLogAdmin::regxThread.bFullyMatches(logevCur.strThread(), kCIDLib::True)
    ||  !CIDLogAdmin::regxProc.bFullyMatches(logevCur.strProcess(), kCIDLib::True))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Parse and validate the command line parameters
static tCIDLib::TBoolean bParseParms()
{
    // If none, then assume a dump command
    const tCIDLib::TCard4 c4Count = TSysInfo::c4CmdLineParmCount();
    if (!c4Count)
        return kCIDLib::True;

    // Get the command
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    TString strCurParm = *cursParms++;

    //
    //  We have one special case one, which is to set the logging level
    //  of a specific facility in a specific server. So we handle it
    //  separately
    //
    if (strCurParm.bCompareI(L"setloglevel"))
    {
        // We only get the command. The caller will do the rest
        CIDLogAdmin::eCmd = CIDLogAdmin::ECmd_SetLogLevel;

        // But do at least make sure we have the right count
        if (c4Count != 4)
        {
            ShowUsage();
            return kCIDLib::False;
        }
    }
     else if (strCurParm.bCompareI(L"setlogtrace"))
    {
        // We only get the command. The caller will do the rest
        CIDLogAdmin::eCmd = CIDLogAdmin::ECmd_SetLogTrace;

        // But do at least make sure we have the right count
        if (c4Count != 4)
        {
            ShowUsage();
            return kCIDLib::False;
        }
    }
     else
    {
        if (strCurParm.bCompareI(L"dump"))
            CIDLogAdmin::eCmd = CIDLogAdmin::ECmd_Dump;
        else if (strCurParm.bCompareI(L"flush"))
            CIDLogAdmin::eCmd = CIDLogAdmin::ECmd_RemoveAll;
        else if (strCurParm.bCompareI(L"fulldump"))
            CIDLogAdmin::eCmd = CIDLogAdmin::ECmd_FullDump;
        else
        {
            TSysInfo::strmOut() << L"Unknown command '" << strCurParm
                                    << L"'" << kCIDLib::EndLn;
            return kCIDLib::False;
        }

        // Else go through them and process the ones we understand
        for (; cursParms; ++cursParms)
        {
            strCurParm = *cursParms;

            if (strCurParm.bStartsWithI(L"/Proc="))
            {
                strCurParm.Cut(0, 6);
                CIDLogAdmin::regxProc.SetExpression(strCurParm);
            }
             else if (strCurParm.bStartsWithI(L"/Thread="))
            {
                strCurParm.Cut(0, 8);
                CIDLogAdmin::regxThread.SetExpression(strCurParm);
            }
             else if (strCurParm.bCompareI(L"/ErrOnly"))
            {
                CIDLogAdmin::bErrsOnly = kCIDLib::True;
            }
             else if (strCurParm.bStartsWithI(L"/OutFile="))
            {
                strCurParm.Cut(0, 9);
                CIDLogAdmin::strOutFile = strCurParm;
            }
             else if (strCurParm.bCompareI(L"/StatusOnly"))
            {
                CIDLogAdmin::bStatusOnly = kCIDLib::True;
            }
             else
            {
                TSysInfo::strmOut() << L"Unknown parameter '" << strCurParm
                                    << L"'. Ignoring it" << kCIDLib::EndLn;
            }
        }

        if (CIDLogAdmin::bErrsOnly && CIDLogAdmin::bStatusOnly)
        {
            TSysInfo::strmOut() << L"/ErrOnly and /StatusOnly are both on."
                                   L" One will be ignored" << kCIDLib::EndLn;
        }
    }

    // If the full dump, we have to have the output file
    if ((CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_FullDump)
    &&  CIDLogAdmin::strOutFile.bIsEmpty())
    {
        TSysInfo::strmOut() << L"Full dump requires the /OutFile= parameter"
                            << kCIDLib::NewEndLn;
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Handle the 'dump' command. This is called from the main thread if the
//  users indicated a dump.
//
static tCIDLib::TVoid DoDumpCmd(tCIDOrbUC::TLSrvProxy& orbcLogger)
{
    static const tCIDLib::TCh* const pszColon = L": ";
    static const tCIDLib::TCh* const pszComma = L", ";

    //
    //  Create a collection of event objects and query up to 128 of the most
    //  recent events.
    //
    TVector<TLogEvent> colEvents;
    const tCIDLib::TCard4 c4Count = orbcLogger->c4QueryEvents(colEvents, 256);

    if (!c4Count)
    {
        TSysInfo::strmOut() << L"No events were available in the log server"
                            << kCIDLib::EndLn;
    }

    TTime tmLogged;
    tmLogged.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
    tCIDLib::TCard4 c4MatchedCount = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TLogEvent& logevCur = colEvents[c4Index];

        //
        //  Apply the filter to this event. If it doesn't pass, then go
        //  get another.
        //
        if (!bApplyFilter(logevCur))
            continue;

        // It passed so display it and bump the count
        c4MatchedCount++;
        logevCur.AdvFormat(TSysInfo::strmOut(), tmLogged);
    }

    if (!c4MatchedCount)
        TSysInfo::strmOut() << L"No events matched the filter" << kCIDLib::EndLn;
    else
        TSysInfo::strmOut() << kCIDLib::NewLn << c4MatchedCount
                            << L" events matched" << kCIDLib::EndLn;
    TSysInfo::strmOut() << kCIDLib::EndLn;
}


//
//  This is the the thread function for the main thread. We just parse the
//  command line parameters, validate them, crank up the Orb client, and do
//  the requested operation.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  Initialize the filter regular expressions, so that if they aren't
        //  set explicitly, they'll have the right values.
        //
        CIDLogAdmin::regxProc.SetExpression(L".*");
        CIDLogAdmin::regxThread.SetExpression(L".*");

        // Now parse parms, which might override our filter defaults
        if (!bParseParms())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Crank up the Orb client
        facCIDOrb().InitClient();

        // And get a name server client proxy
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();

        //
        //  We have a special case with the set log level, else all the
        //  others just talk to the log server.
        //
        if (CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_SetLogLevel)
        {
            //
            //  We should get a name server binding for a core admin ORB
            //  interface. A module to target, and a logging level.
            //
            TString strBinding = TSysInfo::strCmdLineParmAt(1);
            TString strLevel = TSysInfo::strCmdLineParmAt(2);
            TString strModule = TSysInfo::strCmdLineParmAt(3);

            // Looks ok, so let's create a client proxy
            tCIDOrbUC::TCoreAdminProxy orbcAdmin = facCIDOrbUC().orbcCoreSrvAdminProxy
            (
                orbcNS, strBinding, 8000
            );

            // And make the call. If it fails, it will throw
            orbcAdmin->SetModLogLevel(strModule, strLevel);
        }
         else if (CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_SetLogTrace)
        {
            //
            //  We should get a name server binding for a core admin ORB
            //  interface and a logging level.
            //
            TString strBinding = TSysInfo::strCmdLineParmAt(1);
            TString strModule = TSysInfo::strCmdLineParmAt(2);
            TString strState = TSysInfo::strCmdLineParmAt(3);

            // Looks ok, so let's create a client proxy
            tCIDOrbUC::TCoreAdminProxy orbcAdmin = facCIDOrbUC().orbcCoreSrvAdminProxy
            (
                orbcNS, strBinding, 8000
            );

            // And make the call. If it fails, it will throw
            orbcAdmin->SetModLogTrace(strModule, strState.bCompareI(L"Yes"));
        }
         else
        {
            // Get a log server client proxy
            tCIDOrbUC::TLSrvProxy orbcLogger = facCIDOrbUC().orbcLogSrvProxy();
            if (CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_Dump)
            {
                DoDumpCmd(orbcLogger);
            }
             else if (CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_FullDump)
            {
                // Try to open the output file
                TTextFileOutStream strmOut;
                strmOut.Open
                (
                    CIDLogAdmin::strOutFile
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Excl_OutStream
                );

                tCIDLib::TCard4 c4Bytes;
                THeapBuf mbufDump;
                orbcLogger->DebugDump(c4Bytes, mbufDump);

                // Create a text input stream over the buffer
                TTextMBufInStream strmSrc
                (
                    &mbufDump, c4Bytes, tCIDLib::EAdoptOpts::NoAdopt, new TUTF8Converter
                );

                // For each line, pull it in and write it out
                TString strLine;
                while (!strmSrc.bEndOfStream())
                {
                    strmSrc >> strLine;
                    strmOut << strLine << kCIDLib::NewLn;
                }
                strmOut << kCIDLib::EndLn;
            }
             else if (CIDLogAdmin::eCmd == CIDLogAdmin::ECmd_RemoveAll)
            {
                // This one is easy, so just do it here in line
                orbcLogger->RemoveAll();
            }
             else
            {
                TSysInfo::strmOut() << L"Unknown command enum:"
                                    << tCIDLib::TInt4(CIDLogAdmin::eCmd)
                                    << kCIDLib::EndLn;
            }
        }
    }

    catch(TError& errToCatch)
    {
        // If not logged, then log it.
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TSysInfo::strmOut() << L"An exception occured:\n"
                            << errToCatch << kCIDLib::EndLn;
    }

    // Terminate the Orb support
    facCIDOrb().Terminate();

    return tCIDLib::EExitCodes::Normal;
}

