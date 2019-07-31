//
// FILE NAME: TestKernel.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/07/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main module of the program. This program tests the
//  services of the CIDKernel facility, which is the 'virtual kernel' layer
//  of CIDLib. It only encapsulates non-GUI stuff.
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is a terrible example of a CIDLib program because of its
//      special needs, to test out the lowest level stuff. This means it
//      makes some use of C/C++ RTL stuff in places, and isn't structured
//      like a real CIDLib program since it exists below that level.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"
#include    "CIDKernel_RemMedia.hpp"


// ----------------------------------------------------------------------------
//  Externs for the testing functions
// ----------------------------------------------------------------------------
extern tCIDLib::TVoid TestEnums(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestFlagJanitors(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestEvents(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestEnvironment(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestExtProcess(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestFiles(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestHashMap(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestModules(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestMutexes(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestPaths(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestRawStrings(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestRawMemory(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestSafeCounters(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestSemaphores(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestTime(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestThreads(const tCIDLib::TCard4);
extern tCIDLib::TVoid TestCDROM(const tCIDLib::TCard4);


// ----------------------------------------------------------------------------
//  Global data
//
//  kmodTestKernel
//      We don't have facilities at this level, however the kernel class
//      that encapsulates the Exe/Dll module concept is what the facility
//      class is built on, so we test it out here.
//
//  strmOut
//      This is our output stream. Its actually a simple wrapper around the
//      standard output streams that helps us deal with short/wide char issues
//      according to the character mode we are built in.
// ----------------------------------------------------------------------------
TKrnlModule     kmodTestKernel;
TOutStrm        strmOut;



// ----------------------------------------------------------------------------
//  Local, static data
//
//  aTestFunctions
//      An array of test function structure that are used by the main code
//      below to invoke the testing functions.
//
//  bMemCheck
//      A flag that indicates that a memory check second run of the tests
//      should be done. Its set by the /MemCheck flag.
//
//  c4TestFuncCount
//      The number of entries in the test function array.
// ----------------------------------------------------------------------------
static TTestFuncRecord          aTestFunctions[] =
{
        { TestEnums         , L"Enums"          , kCIDLib::False }
    ,   { TestRawStrings    , L"Strings"        , kCIDLib::False }
    ,   { TestFlagJanitors  , L"FlagJans"       , kCIDLib::False }
    ,   { TestRawMemory     , L"Memory"         , kCIDLib::False }
    ,   { TestHashMap       , L"HashMap"        , kCIDLib::False }
    ,   { TestEnvironment   , L"Environment"    , kCIDLib::False }
    ,   { TestEvents        , L"Events"         , kCIDLib::False }
    ,   { TestMutexes       , L"Mutexes"        , kCIDLib::False }
    ,   { TestSemaphores    , L"Semaphores"     , kCIDLib::False }
    ,   { TestSafeCounters  , L"Counters"       , kCIDLib::False }
    ,   { TestTime          , L"Time"           , kCIDLib::False }
    ,   { TestPaths         , L"Paths"          , kCIDLib::False }
    ,   { TestFiles         , L"Files"          , kCIDLib::False }
    ,   { TestThreads       , L"Threads"        , kCIDLib::False }
    ,   { TestModules       , L"Modules"        , kCIDLib::False }
    ,   { TestExtProcess    , L"ExtProcess"     , kCIDLib::False }
};
static tCIDLib::TBoolean        bMemCheck;
static const tCIDLib::TCard4    c4TestFuncCount = tCIDLib::c4ArrayElems(aTestFunctions);
static tCIDLib::TCard4          c4SignalCount = 0;



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This method is called to do the standard program announcement. Its just
//  here to keep it separated out.
//
static tCIDLib::TVoid Announce()
{
    strmOut << L"\nTestKernel.Exe, Version: "
            << kCIDLib::pszVersion << L"\n"
            << L"A Testing Suite for the CIDKernel Facility\n"
            << L"Compiled On: " << __DATE__ << L"\n" << EndLn;
}


//
//  This is our test signal handler, which is installed to handle all signals
//  and just bumps a count which is displayed at the end.
//
static tCIDLib::TBoolean bTestSigHandler(const tCIDLib::ESignals eSignal)
{
    c4SignalCount++;

    // Indicate that we've handled it and don't want it to propogate
    return kCIDLib::True;
}


// Dump out some TTS info and do a quick speech test
static tCIDLib::TVoid SpeechInfo()
{
    TKrnlSpeech kspchTest;

    TKrnlLList<TKrnlString> kllstVoices;
    if (kspchTest.bQueryVoiceList(kllstVoices))
    {
        // If we got any, display the ones we got
        if (kllstVoices.bResetCursor())
        {
            strmOut << L"Available TTS Voices:\n--------------------\n";
            TKrnlString* pkstrCur;
            while (kllstVoices.bNext(pkstrCur))
                strmOut << L"   " << pkstrCur->pszValue() << L"\n";
            strmOut.flush();

            // Try to set a voice and speak some text
            if (kspchTest.bSetVoice(kllstVoices.pobjHead()->pszValue()))
            {
                const tCIDLib::TCh* const pszText =
                (
                    L"Kernel Test in Progress. I have the greatest enthusiasm for these tests"
                );

                if (kspchTest.bSpeak(pszText, tCIDLib::ESpeechFlags::None))
                {
                    // Make sure that the voice announcement has completed
                    tCIDLib::TBoolean bTimedOut;
                    if (!kspchTest.bWaitTillDone(10000, bTimedOut) || bTimedOut)
                        strmOut << L"Failed to wait for voice to complete" << EndLn;
                }
                 else
                {
                    strmOut << L"Failed to speak text" << EndLn;
                }
            }
             else
            {
                strmOut << L"Failed to set a voice" << EndLn;
            }
        }
         else
        {
            strmOut << L"No TTS voices were available" << EndLn;
        }
    }
     else
    {
        strmOut << L"Failed to query any TTS voices" << EndLn;
    }


    // List the available speech outputs
    TKrnlLList<TKrnlKVPair> kllstOutputs;
    if (kspchTest.bQueryOutputList(kllstOutputs))
    {
        TKrnlKVPair* pkkvpCur;
        if (kllstOutputs.bResetCursor())
        {
            while (kllstOutputs.bNext(pkkvpCur))
            {
                strmOut << L"   " << pkkvpCur->pszValue()
                        << L" (" << pkkvpCur->pszKey() << L")\n";
            }

            strmOut << L"\nFound TTS outputs, setting the first one and testing...\n";
            strmOut.flush();

            // The key is the unique id we use to set it
            if (kspchTest.bSetOutput(kllstOutputs.pobjHead()->pszKey()))
            {
                const tCIDLib::TCh* const pszText =
                (
                    L"Testing text to speech via manually set output"
                );

                if (!kspchTest.bSpeak(pszText, tCIDLib::ESpeechFlags::None))
                    strmOut << L"Speech to manually set output failed\n\n";
            }
             else
            {
                strmOut << L"Failed to set the output: "
                        << kllstOutputs.pobjHead()->pszValue()
                        << L"\n\n";
            }
            strmOut.flush();
        }
         else
        {
            strmOut << L"There were no available TTS outputs"
                    << EndLn;
        }
    }
     else
    {
        strmOut << L"Failed to query any available TTS outputs"
                << EndLn;
        return;
    }
}


//
//  This method is called to dump out all of the general system information
//  gatherable via the kernel facility.
//
static tCIDLib::TVoid ShowSysInfo()
{
    TKrnlString kstrTmp;

    tCIDLib::TCard4 c4OSMaj, c4OSMin, c4OSRev, c4OSBuild;
    TKrnlSysInfo::QueryOSInfo(c4OSMaj, c4OSMin, c4OSRev, c4OSBuild);

    tCIDLib::TCard4 c4TCPMaj, c4TCPMin;
    if (!TKrnlIP::bQueryTCPVersion(c4TCPMaj, c4TCPMin))
        throw TKrnlError::kerrLast();

    tCIDLib::TZStr128 szUser;
    if (!TKrnlSysInfo::bQueryUserName(szUser, c4MaxBufChars(szUser)))
        throw TKrnlError::kerrLast();

    TKrnlString kstrIPHost;
    if (!TKrnlIP::bQueryLocalName(kstrIPHost))
        throw TKrnlError::kerrLast();

    TKrnlIPAddr kipaDefAddrV4;
    if (TKrnlIP::bIPV4Avail())
    {
        if (!TKrnlIP::bQueryDefLocalAddr(kipaDefAddrV4, tCIDSock::EAddrTypes::IPV4))
            throw TKrnlError::kerrLast();
    }

    TKrnlIPAddr kipaDefAddrV6;
    if (TKrnlIP::bIPV6Avail())
    {
        if (!TKrnlIP::bQueryDefLocalAddr(kipaDefAddrV6, tCIDSock::EAddrTypes::IPV6))
            throw TKrnlError::kerrLast();
    }

    // This is only available on domain based systems
    const tCIDLib::TCard4 c4MaxMacId = 128;
    tCIDLib::TZStr128 szMachineId;
    if (!TKrnlSysInfo::bQueryMachineID(szMachineId, c4MaxMacId))
        TRawStr::CopyStr(szMachineId, L"[Not Avail]");

    //
    //  These are 64 bit values, so we will convert them K and store them
    //  in TCard4 values for display. The standard streams don't deal with
    //  64 bit values.
    //
    tCIDLib::TCard4 c4TotalMem = tCIDLib::TCard4
    (
        TKrnlSysInfo::c8TotalPhysicalMem() / 1024
    );

    tCIDLib::TCard8 c8Tmp;
    if (!TKrnlSysInfo::bQueryAvailPhysicalMem(c8Tmp))
        throw TKrnlError::kerrLast();
    tCIDLib::TCard4 c4AvailMem = tCIDLib::TCard4(c8Tmp / 1024);

    strmOut << L"General System Information\n"
            << L"-------------------------------------------------\n"
            << L"        Node Name: " << TKrnlSysInfo::pszNodeName() << L"\n"
            << L"       Machine Id: " << szMachineId << L"\n"
            << L"   CIDLib Version: " << kCIDLib::pszVersion << "\n"
            << L"         CPU Type: " << tCIDLib::TInt4(TKrnlSysInfo::eCPUType()) << "\n"
            << L"        CPU Count: " << TKrnlSysInfo::c4CPUCount() << "\n"
            << L"     Total Memory: " << c4TotalMem << "K\n"
            << L"     Avail Memory: " << c4AvailMem << "K\n"
            << L"       OS Version: " << c4OSMaj << L"." << c4OSMin << L"." << c4OSRev
                                      << L"  Build:" << c4OSBuild << L"\n"
            << L"        SSE Level: " << TKrnlSysInfo::c4SSELevel() << L"\n"
            << L"     Current User: " << szUser << L"\n"
            << EndLn;

    strmOut << L"TCP/IP Information\n"
            << L"-------------------------------------------------\n"
            << L"      TCP Version: " << c4TCPMaj << L"." << c4TCPMin << L"\n"
            << L"     IP Host Name: " << kstrIPHost.pszValue() << L"\n";

    strmOut << L"    Def IPV4 Addr: ";
    if (TKrnlIP::bIPV4Avail())
    {
        if (!TKrnlIP::bTextFromIPAddr(kipaDefAddrV4, kstrTmp))
            throw TKrnlError::kerrLast();
        strmOut << kstrTmp.pszValue() << EndLn;
    }

    strmOut << L"    Def IPV6 Addr: ";
    if (TKrnlIP::bIPV6Avail())
    {
        if (!TKrnlIP::bTextFromIPAddr(kipaDefAddrV6, kstrTmp))
            throw TKrnlError::kerrLast();
        strmOut << kstrTmp.pszValue() << EndLn;
    }

    // Set the format to hex
    strmOut.setf(std::ios::hex, std::ios::dec | std::ios::hex);

    strmOut << L"\n  Network Adapters\n  ---------------------\n";
    TKrnlLList<TKrnlIP::TAdaptorInfo> kllstAdaptors;

    if (!TKrnlIP::bQueryAdaptorList(kllstAdaptors))
        throw TKrnlError::kerrLast();

    TKrnlIP::TAdaptorInfo* pkadpCur;
    kllstAdaptors.bResetCursor();
    while (kllstAdaptors.bNext(pkadpCur))
    {
        strmOut << L"  " << pkadpCur->szName << L"\n"
                << L"  " << pkadpCur->szDescr << L"\n"
                << L"        Is Ready: "
                << (pkadpCur->bReady ? L"Yes" : L"No") << L"\n"
                << L"       Dedicated: "
                << (pkadpCur->bDedicated ? L"Yes" : L"No") << L"\n"
                << L"    DHCP Enabled: "
                << (pkadpCur->bDHCPEnabled ? L"Yes" : L"No") << L"\n"
                << L"    IPV4 Enabled: "
                << (pkadpCur->bIPV4Enabled ? L"Yes" : L"No") << L"\n"
                << L"    IPV6 Enabled: "
                << (pkadpCur->bIPV6Enabled ? L"Yes" : L"No") << L"\n"
                << L"      Is  Tunnel: "
                << (pkadpCur->bTunnel ? L"Yes" : L"No") << L"\n"
                << L"    Is Loop Back: "
                << (pkadpCur->bLoopback ? L"Yes" : L"No") << L"\n";

        strmOut << L"    HW Addr: ";
        for (tCIDLib::TCard4 c4HWInd = 0; c4HWInd < pkadpCur->c4HWAddrSz; c4HWInd++)
            strmOut << tCIDLib::TCard4(pkadpCur->ac1HWAddr[c4HWInd]);
        strmOut << L"\n";

        for (tCIDLib::TCard4 c4AInd = 0; c4AInd < pkadpCur->c4AddrCnt; c4AInd++)
        {
            if (!TKrnlIP::bTextFromIPAddr(pkadpCur->akipaAddrs[c4AInd], kstrTmp))
                throw TKrnlError::kerrLast();

            strmOut << L"    IP Addr" << tCIDLib::TCard4(c4AInd + 1) << L": "
                    << kstrTmp.pszValue() << L"\n";
        }
        strmOut << L"\n\n";
    }

    strmOut << L"\n  IP Addresses (non-loopback)\n  ---------------------\n";
    TKrnlLList<TKrnlIPAddr> kllstIPAddrs;
    if (!TKrnlIP::bQueryHostAddrs(  kstrIPHost.pszValue()
                                    , kllstIPAddrs
                                    , tCIDSock::EAddrInfoFlags::None
                                    , kCIDLib::False
                                    , kCIDLib::False))
    {
        throw TKrnlError::kerrLast();
    }

    kllstIPAddrs.bResetCursor();
    TKrnlIPAddr* pkipaCur;
    while (kllstIPAddrs.bNext(pkipaCur))
    {
        if (TKrnlIP::bTextFromIPAddr(*pkipaCur, kstrTmp))
        {
            strmOut << L"     " << kstrTmp.pszValue()
                    << L"  Type=";

            if (pkipaCur->eType() == tCIDSock::EAddrTypes::IPV4)
                strmOut << L"IPV4";
            else
                strmOut << L"IPV6";
        }
         else
        {
            strmOut << L"     " << L" ---- Failed to format ---";
        }
        strmOut << L"\n";
    }
    strmOut << EndLn;


    strmOut << L"\n  IP Addresses (loopback)\n  ---------------------\n";
    if (!TKrnlIP::bQueryHostAddrs(  L"localhost"
                                    , kllstIPAddrs
                                    , tCIDSock::EAddrInfoFlags::None
                                    , kCIDLib::True
                                    , kCIDLib::False))
    {
        throw TKrnlError::kerrLast();
    }

    kllstIPAddrs.bResetCursor();
    while (kllstIPAddrs.bNext(pkipaCur))
    {
        if (TKrnlIP::bTextFromIPAddr(*pkipaCur, kstrTmp))
        {
            strmOut << L"     " << kstrTmp.pszValue()
                    << L"  Type=";

            if (pkipaCur->eType() == tCIDSock::EAddrTypes::IPV4)
                strmOut << L"IPV4";
            else
                strmOut << L"IPV6";
        }
         else
        {
            strmOut << L"     " << L" ---- Failed to format ---";
        }
        strmOut << L"\n";
    }
    strmOut << EndLn;


    // Set the format back to decimal
    strmOut.setf(std::ios::dec, std::ios::dec | std::ios::hex);
    strmOut << L"\nLocale Information\n"
            << L"-------------------------------------------------\n"
            << L"   24 Hour Format: " << TKrnlLocale::b24Hour() << L"\n"
            << L"    Decimal Point: " << TKrnlLocale::chDecimalSymbol() << L"\n"
            << L"   Mon  Dec Point: " << TKrnlLocale::chMonetaryDecimalSymbol() << L"\n"
            << L"   Decimal Digits: " << TKrnlLocale::c4DecimalDigits() << L"\n"
            << L"   Mon Dec Digits: " << TKrnlLocale::c4MonetaryDecimalDigits() << L"\n"
            << L"       Group Size: " << TKrnlLocale::c4GroupSize() << L"\n"
            << L"   Mon Group Size: " << TKrnlLocale::c4MonetaryGroupSize() << L"\n"
            << L"  Group Separator: " << TKrnlLocale::chGroupSeparator() << L"\n"
            << L"    Mon Group Sep: " << TKrnlLocale::chMonetaryGroupSeparator() << L"\n"
            << L"  Currency Symbol: " << TKrnlLocale::chCurrencySymbol() << L"\n"
            << L"    Positive Sign: " << TKrnlLocale::chPositiveSign() << L"\n"
            << L"    Negative Sign: " << TKrnlLocale::chNegativeSign() << L"\n"
            << L"   Date Separator: " << TKrnlLocale::chDateSeparator() << L"\n"
            << L"   Time Separator: " << TKrnlLocale::chTimeSeparator() << L"\n"
            << L"\n\n";

    // Format out the date/time info
    {
        TKrnlTimeStamp ktsCurrent;
        ktsCurrent.bSetTo(tCIDLib::ESpecialTimes::CurrentTime);
        tCIDLib::TCard4 c4Hours;
        tCIDLib::TCard4 c4Minutes;
        tCIDLib::TCard4 c4Seconds;
        tCIDLib::TCard4 c4Millis;
        ktsCurrent.bAsTimeInfo(c4Hours, c4Minutes, c4Seconds, c4Millis);

        tCIDLib::TCard4 c4Year;
        tCIDLib::EMonths eMonth;
        tCIDLib::TCard4 c4Day;
        tCIDLib::EWeekDays eWeekDay;
        ktsCurrent.bAsDateInfo(c4Year, eMonth, c4Day, eWeekDay);

        tCIDLib::TInt4 i4TZOffset;
        if (!ktsCurrent.bTZOffset(i4TZOffset))
            strmOut << L"Couldn't get time zone offset" << EndLn;

        tCIDLib::TBoolean bDSTState;
        if (!ktsCurrent.bIsDST(bDSTState))
            strmOut << L"Couldn't get DST state" << EndLn;

        tCIDLib::TZStr64 szTZName;
        if (!ktsCurrent.bTZName(szTZName, c4MaxBufChars(szTZName)))
            strmOut << L"Couldn't get DST name" << EndLn;

        strmOut << L"Date and Time Information\n"
                << L"-------------------------------------------------\n"
                << L"     Current Time: " << c4Hours << L":" << c4Minutes
                                          << L":" << c4Seconds << L"\n"
                << L"     Current Date: " << c4Year << L"/" << (tCIDLib::TInt4(eMonth) + 1)
                                          << L"/" << c4Day << L"\n"

                // We have to cast the encoded time, since its a 64 bit value
                << L"     Encoded Time: " << tCIDLib::TCard4(ktsCurrent.enctTime()) << L"\n"
                << L"    Time Zone Ofs: " << i4TZOffset << L"\n"
                << L"   Time Zone Name: " << szTZName << L"\n"
                << L"           Is DST: " << bDSTState << L"\n"
                << L"\n\n";

        tCIDLib::TBoolean abPorts[1024];
        TKrnlCommPort::c4EnumPorts(abPorts, 1024);
        strmOut << L"Available Comm Ports\n"
                << L"-------------------------------------------------\n";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 128; c4Index++)
        {
            if (abPorts[c4Index])
                strmOut << "    COM" << c4Index + 1 << "\n";
        }
        strmOut << L"\n\n";
    }

    // Iterate available volumes and shared directories
    {
        tCIDLib::TCard4 c4BadVolCnt;
        tCIDLib::TCard4 c4GoodVolCnt;
        TKrnlLList<TKrnlVolFailureInfo> kllistBad;
        TKrnlLList<TKrnlVolumeInfo> kllistGood;
        tCIDLib::TBoolean bRes = TKrnlFileSys::bQueryVolumeList
        (
             kllistGood
             , c4GoodVolCnt
             , kllistBad
             , c4BadVolCnt
             , kCIDLib::True
             , kCIDLib::False
        );

        if (kllistGood.bResetCursor())
        {
            strmOut << L"Available Volumes\n"
                    << L"-------------------------------------------------\n";
            TKrnlVolumeInfo* pkvoliCur;
            while (kllistGood.bNext(pkvoliCur))
            {
                strmOut << L"  " << pkvoliCur->pszVolume()
                        << L"  (" << pkvoliCur->pszFileSysType()
                        << L")\n";
            }
            strmOut << L"\n\n";
        }

        if (kllistBad.bResetCursor())
        {
            strmOut << L"Unavailable Volumes\n"
                    << L"-------------------------------------------------\n";

            TKrnlVolFailureInfo* pkvolfiCur;
            while (kllistBad.bNext(pkvolfiCur))
                strmOut << L"  " << pkvolfiCur->pszVolume() << L"\n";
            strmOut << L"\n\n";
        }
    }


    // Iterate any media changers
    const tCIDLib::TCard4 c4MaxChangers = 256;
    tCIDLib::TCard4 c4ChangerCnt;
    TKrnlMediaChanger::TMediaChgInfo aciList[c4MaxChangers];
    if (TKrnlMediaChanger::bEnumChangers(aciList, c4MaxChangers, c4ChangerCnt))
    {
        strmOut << L"Available Media Changers\n"
                << L"------------------------------------\n";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChangerCnt; c4Index++)
        {
            strmOut << (c4Index + 1) << L". ";
            const TKrnlMediaChanger::TMediaChgInfo& ciCur = aciList[c4Index];
            strmOut << L"Volume: " << ciCur.achDevPath
                    << L"\n   Id: " << ciCur.achProductId
                    << L"\n   Number: " << ciCur.c4ChangerNum << L"\n";
        }

        if (!c4ChangerCnt)
            strmOut << L"  [NONE FOUND]\n";
    }
     else
    {
        strmOut << L"Could not enumerate media changers\n";
    }
    strmOut << L"\n\n";

    // Show some of the special directories
    strmOut << L"Special Directories\n"
            << L"------------------------------------\n";
    {
        struct TDirInfo
        {
            tCIDLib::ESpecialPaths  ePath;
            const tCIDLib::TCh*     pszName;
        };

        const TDirInfo aPaths[] =
        {
            { tCIDLib::ESpecialPaths::CommonAppData, L"Common App Data" }
            , { tCIDLib::ESpecialPaths::UserAppData, L"User App Data" }
            , { tCIDLib::ESpecialPaths::ProgramFiles, L"Program Files" }
            , { tCIDLib::ESpecialPaths::ProgramFilesCommon, L"Common Program Files" }
        };
        const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(aPaths);
        tCIDLib::TCh achPath[kCIDLib::c4MaxPathLen + 1];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TDirInfo& CurPath = aPaths[c4Index];
            const tCIDLib::TBoolean bRes = TKrnlSysInfo::bQuerySpecialPath
            (
                achPath, kCIDLib::c4MaxPathLen, CurPath.ePath
            );

            strmOut << L"  " << CurPath.pszName << L" = ";
            if (bRes)
                strmOut << achPath;
            else
                strmOut << L"  [UNKNOWN]";
            strmOut << L"\n";
        }
        strmOut << L"\n\n";
    }

    // Do a little TTS testing
    SpeechInfo();

    // Flush any remainder out now
    strmOut.flush();
}

tCIDLib::TBoolean bTestTypes()
{
    if ((sizeof(tCIDLib::TCard1)    != 1)
    ||  (sizeof(tCIDLib::TCard2)    != 2)
    ||  (sizeof(tCIDLib::TCard4)    != 4)
    ||  (sizeof(tCIDLib::TCard8)    != 8)
    ||  (sizeof(tCIDLib::TInt1)     != 1)
    ||  (sizeof(tCIDLib::TInt2)     != 2)
    ||  (sizeof(tCIDLib::TInt4)     != 4)
    ||  (sizeof(tCIDLib::TInt8)     != 8)
    ||  (sizeof(tCIDLib::TFloat4)   != 4)
    ||  (sizeof(tCIDLib::TFloat8)   != 8)
    ||  (sizeof(tCIDLib::TCh)    != kCIDLib::c4CharBytes)
    ||  (sizeof(tCIDLib::TSCh)   != kCIDLib::c4SCharBytes))
    {
        strmOut << L"One or more platformd defined types are not the right size"
                << L"\n";
        strmOut.flush();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------

//
//  This is the program entry point. It will do some overall setup and
//  announcement, then call each of the testing modules in the correct order.
//
tCIDLib::TSInt main(const tCIDLib::TInt4 i4ArgC, tCIDLib::TSCh* apszArgs[])
{
    // Turn off buffering for output stream
    strmOut.setf(std::ios::unitbuf, std::ios::unitbuf);

    try
    {
        //
        //  Do the most fundamental test which is to insure the per platform
        //  define types are the right sizes.
        //
        if (!bTestTypes())
            return 0;

        //
        //  We have to do a special case thing here since this is a kernel
        //  level only program. There are some things that are done for us
        //  when creating CIDLib threads that aren't going to happen for this
        //  program entry thread. Normally the main thread is used internally
        //  and a CIDLib thread is created to do the actual main program
        //  entry point. That isn't the case here so we need to do the basic
        //  thread init stuff for this initial thread.
        //
        TKrnlThread::KrnlThreadInit();

        // Display an announcment thing, to say what we are, version, etc...
        Announce();

        // Initialize the module object
        if (!kmodTestKernel.bQueryFromName
        (
            L"TestKernel"
            , tCIDLib::EModTypes::Exe
            , kCIDLib::c4MajVersion
            , kCIDLib::c4MinVersion
            , tCIDLib::EModFlags::HasMsgFile))
        {
            throw TKrnlError::kerrLast();
        }

        // Transcode the parameters
        const tCIDLib::TCard4 c4MaxArgs = 128;
        tCIDLib::TCh* apszArgList[c4MaxArgs];
        for (tCIDLib::TInt4 i4ArgIndex = 0; i4ArgIndex < i4ArgC; i4ArgIndex++)
            apszArgList[i4ArgIndex] = TRawStr::pszConvert(apszArgs[i4ArgIndex]);


        //
        //  See if there was a command line parm, which indicates the test
        //  to run. If not, then assume all tests.
        //
        tCIDLib::TBoolean   bRunAll = kCIDLib::False;
        tCIDLib::TCard4     c4FnInd;
        if (i4ArgC < 2)
        {
            bRunAll = kCIDLib::True;
        }
         else
        {
            tCIDLib::TCard4 c4ArgInd = 1;

            if (TRawStr::bCompareStrI(apszArgList[1], L"/MemCheck"))
            {
                bMemCheck = kCIDLib::True;
                c4ArgInd++;

                if (c4ArgInd == tCIDLib::TCard4(i4ArgC))
                    bRunAll = kCIDLib::True;
            }

            //
            //  Search the list of test functions and turn on each one that
            //  has a match in the list.
            //
            for (; c4ArgInd < tCIDLib::TCard4(i4ArgC); c4ArgInd++)
            {
                for (c4FnInd = 0; c4FnInd < c4TestFuncCount; c4FnInd++)
                {
                    if (TRawStr::bCompareStrI
                    (
                        apszArgList[c4ArgInd]
                        , aTestFunctions[c4FnInd].pszName))
                    {
                        aTestFunctions[c4FnInd].bRun = kCIDLib::True;
                    }
                }
            }
        }

        //
        //  First of all just dump the system information values. This is
        //  just for examining by eye. Its a local function.
        //
        ShowSysInfo();

        strmOut << L"\n\nStarting Tests...\n\n";

        for (c4FnInd = 0; c4FnInd < c4TestFuncCount; c4FnInd++)
        {
            if (aTestFunctions[c4FnInd].bRun || bRunAll)
            {
                strmOut << L"Testing "
                        << aTestFunctions[c4FnInd].pszName
                        << EndLn;

                // Run the test method
                aTestFunctions[c4FnInd].pfnTester(0);

                //
                //  If the user wants to do a memory check, then create a
                //  faux block to force destruction of heap checker
                //
                if (bMemCheck)
                {
                    #if CID_DEBUG_ON
                    TKrnlMemCheck kmchkTest;
                    kmchkTest.SetHeapCheckLevel(tCIDLib::EHeapChkLevels::Medium, kCIDLib::True);
                    #endif

                    aTestFunctions[c4FnInd].pfnTester(1);
                }
            }
        }

        strmOut << L"\nTests complete\n\n";
    }

    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"Caught unhandled kernel exception. "
                << kerrToCatch.errcId() << L"/" << kerrToCatch.errcHostId()
                << EndLn
                << kmodCIDKernel.pszLoadCIDFacMsg(kerrToCatch.errcId())
                << EndLn;
        return 1;
    }

    catch(...)
    {
        strmOut << L"Caught unknown exception" << EndLn;
        return 1;
    }

    // We made it
    return 0;
}
