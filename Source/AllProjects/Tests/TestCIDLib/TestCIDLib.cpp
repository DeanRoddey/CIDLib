//
// FILE NAME: TestCIDLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
//  This is the main module for the test program. It does some setup then
//  calls all of the testing modules.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Global data
// ----------------------------------------------------------------------------
TFacTestCIDLib  facTestCIDLib;


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestCIDLibMainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local data
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean    bRunAll;
static TTestFuncRecord      aTestFunctions[] =
{
        { TFacTestCIDLib::CoreTests         , L"Core"           , kCIDLib::False }
    ,   { TFacTestCIDLib::ShowSystemInfo    , L"SysInfo"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestEnvironment   , L"Environment"    , kCIDLib::False }
    ,   { TFacTestCIDLib::TestPointerClasses, L"Pointers"       , kCIDLib::False }
    ,   { TFacTestCIDLib::TestStrings       , L"Strings"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestTextConverters, L"TextConverters" , kCIDLib::False }
    ,   { TFacTestCIDLib::TestLocales       , L"Locales"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestRTTI          , L"RTTI"           , kCIDLib::False }
    ,   { TFacTestCIDLib::TestPerThreadData , L"PerThread"      , kCIDLib::False }
    ,   { TFacTestCIDLib::TestFundData      , L"FundData"       , kCIDLib::False }
    ,   { TFacTestCIDLib::TestSearchSort    , L"SearchSort"     , kCIDLib::False }
    ,   { TFacTestCIDLib::TestVectors       , L"Vectors"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestProcess       , L"Process"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestMemoryClasses , L"Memory"         , kCIDLib::False }
    ,   { TFacTestCIDLib::TestFiles         , L"Files"          , kCIDLib::False }
    ,   { TFacTestCIDLib::TestBinStreams    , L"BinStreams"     , kCIDLib::False }
    ,   { TFacTestCIDLib::TestTextStreams   , L"TextStreams"    , kCIDLib::False }
    ,   { TFacTestCIDLib::TestColCommon     , L"ColComm"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestBags          , L"Bags"           , kCIDLib::False }
    ,   { TFacTestCIDLib::TestDeques        , L"Deques"         , kCIDLib::False }
    ,   { TFacTestCIDLib::TestQueues        , L"Queues"         , kCIDLib::False }
    ,   { TFacTestCIDLib::TestQStreams      , L"QStreams"       , kCIDLib::False }
    ,   { TFacTestCIDLib::TestObjArrays     , L"ObjArrays"      , kCIDLib::False }
    ,   { TFacTestCIDLib::TestKeyedHashSet  , L"KeyedHashSet"   , kCIDLib::False }
    ,   { TFacTestCIDLib::TestHashSet       , L"HashSet"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestHashSet       , L"HashMap"        , kCIDLib::False }
    ,   { TFacTestCIDLib::TestStacks        , L"Stacks"         , kCIDLib::False }
    ,   { TFacTestCIDLib::TestTreeCols      , L"Trees"          , kCIDLib::False }
};
static const tCIDLib::TCard4 c4TestFuncCount = tCIDLib::c4ArrayElems(aTestFunctions);



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::TVoid RunTests()
{
    //
    //  Call all of the testing methods. These are all methods of the facility
    //  object, but they are spread out in the modules that are relevant to
    //  that particular subsystem.
    //
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4TestFuncCount; c4Index++)
    {
        if (aTestFunctions[c4Index].bRun || bRunAll)
        {
            facTestCIDLib.strmOut() << L"Testing "
                                    << aTestFunctions[c4Index].pszName
                                    << L"..." << kCIDLib::EndLn << kCIDLib::FlushIt;

            // Call the test function.
            aTestFunctions[c4Index].pfnTester();

            facTestCIDLib.strmOut()
                << aTestFunctions[c4Index].pszName
                << L" tests complete" << kCIDLib::DNewLn << kCIDLib::FlushIt;
        }
    }
}


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    {
        // Set our logging threshold down low for testing purposes
        TFacility::eGlobalLogMode(tCIDLib::ESeverities::Warn);

        // Log our arrival
        facTestCIDLib.LogMsg
        (
            CID_FILE
            , CID_LINE
            , L"TestCIDLib starting up..."
            , tCIDLib::ESeverities::Status
        );

        // Announce ourself
        facTestCIDLib.Announce();

        // If there is 1 param, and it's /?, then display all of the tests we can do.
        TString strTmp;
        if (TSysInfo::c4CmdLineParmCount() == 1)
        {
            if (TSysInfo::strCmdLineParmAt(0) == L"/?")
            {
                facTestCIDLib.ShowTests();
                return tCIDLib::EExitCodes::Normal;
            }
        }

        // Speak an announcement
        TSpeech spchAnnounce;
        spchAnnounce.Speak(L"CIDLib Tests In Progress", tCIDLib::ESpeechFlags::Async);

        //
        //  See what tests we need to run. The user indicates which tests by naming them
        //  on the command line. If none were named then we run them all.
        //
        tCIDLib::TCard4 c4FnInd;
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        if (!cursParms)
        {
            bRunAll = kCIDLib::True;
        }
         else
        {
            for (; cursParms; ++cursParms)
            {
                const TString& strParm = *cursParms;

                tCIDLib::TBoolean bMatched = kCIDLib::False;
                for (c4FnInd = 0; c4FnInd < c4TestFuncCount; c4FnInd++)
                {
                    if (strParm.bCompareI(aTestFunctions[c4FnInd].pszName))
                    {
                        bMatched = kCIDLib::True;
                        aTestFunctions[c4FnInd].bRun = kCIDLib::True;
                    }
                }

                if (!bMatched)
                {
                    facTestCIDLib.strmOut()
                                << strParm
                                << L" is not a valid test name" << kCIDLib::EndLn;
                }
            }
        }
        facTestCIDLib.strmOut() << kCIDLib::DNewLn;

        try
        {
            RunTests();

            //
            //  If a memory debugging, then do another round of tests with
            //  a memory check object on the stack.
            //
            #if CID_DEBUG_MEM
            facTestCIDLib.strmOut() << L"Doing memory test pass....\n"
                                    << TTextOutStream::RepChars(L'-', 24)
                                    << kCIDLib::NewLn << kCIDLib::EndLn;
            TPathStr pathTmp;
            TProcEnvironment::bFind(L"CID_RESDIR", pathTmp);
            pathTmp.Normalize();
            pathTmp.AddLevel(L"MemDump_TestCIDLib.Txt");

            TKrnlMemCheck kmchkTest;
            kmchkTest.ReportToFile(pathTmp.pszBuffer());
            RunTests();
            #endif
        }

        // Catch and report any error that occured
        catch(const TError& errToCatch)
        {
            facTestCIDLib.strmOut()
                << L"Unhandled exception occured!\n"
                << L"    File: " << errToCatch.strFileName() << kCIDLib::NewLn
                << L"    Line: " << errToCatch.c4LineNum() << kCIDLib::NewLn
                << L"   Error: " << errToCatch.strErrText() << kCIDLib::NewLn
                << L" Aux Txt: " << errToCatch.strAuxText() << kCIDLib::NewLn
                << L" ErrCode: " << errToCatch.errcId() << kCIDLib::NewLn
                << L"Krnl Err: " << errToCatch.errcKrnlId() << kCIDLib::NewLn
                << L" OS Code: " << errToCatch.errcHostId() << kCIDLib::NewLn
                << L"  Thread: " << errToCatch.strThread()
                << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        if (!spchAnnounce.bWaitTillDone(10000))
        {
            facTestCIDLib.strmOut() << L"Timed out waiting for voice to complete"
                                    << kCIDLib::EndLn;
        }
    }

    // Log our exit
    facTestCIDLib.LogMsg
    (
        CID_FILE
        , CID_LINE
        , L"TestCIDLib terminating..."
        , tCIDLib::ESeverities::Status
    );

    return tCIDLib::EExitCodes::Normal;
}



// -----------------------------------------------------------------------------
//  CLASS: TFacTestCIDLib
// PREFIX: fac
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Static data
// -----------------------------------------------------------------------------
TTextOutStream* TFacTestCIDLib::s_pstrmOutput = nullptr;

// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Constructors and destructors
// -----------------------------------------------------------------------------
TFacTestCIDLib::TFacTestCIDLib() :

    TFacility
    (
        L"TestCIDLib"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
    // Set our static stream member
    s_pstrmOutput = new TOutConsole();
}

TFacTestCIDLib::~TFacTestCIDLib()
{
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------
tCIDLib::TVoid TFacTestCIDLib::Announce()
{
    // Announce ourselves
    strmOut()   << L"\nTestCIDLib\n"
                << L"Version " << facCIDLib().strVersion() << kCIDLib::NewLn
                << L"Test facility for the CIDLib libraries\n"
                << kCIDLib::EndLn;
}


tCIDLib::TVoid TFacTestCIDLib::CoreTests()
{
    //
    //  Do some core tests. These are to make sure that we are very basically
    //  ok and hopefully sane enough to report on our own state.
    //

    // <TBD>
}


tCIDLib::TVoid TFacTestCIDLib::ShowSystemInfo()
{
    TTextOutStream& strmTar = strmOut();

    // Display the command line parameters
    strmTar << L"Command line parm count: "
            << TSysInfo::c4CmdLineParmCount() << kCIDLib::NewLn;

    tCIDLib::TCard4 c4Index{0};
    TSysInfo::colCmdLineParms().bForEach
    (
        [&c4Index, &strmTar](const TString& strCur)
        {
            strmTar << L"  Arg" << c4Index << L"=" << strCur<< kCIDLib::NewLn;
            c4Index++;
            return kCIDLib::True;
        }
    );
    strmTar << kCIDLib::DNewLn;

    //
    //  We use two alternating formatting schemes here, so we just create
    //  two stream format objects with those schemes. Then we can just
    //  dump each one in to set the formatting.
    //
    //  The legends are right justified in a field large enough that they
    //  are all indented over a little. The data is not justified and the
    //  field width is set back to 0, which means just whatever space that
    //  they naturally take.
    //
    TStreamFmt  strmfLegend(22, 2, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
    TStreamFmt  strmfData(0, 2, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    // Get some version information for later display.
    tCIDLib::TCard4 c4OSMajVer, c4OSMinVer, c4OSRev, c4BuildNum;
    TSysInfo::QueryOSInfo(c4OSMajVer, c4OSMinVer, c4OSRev, c4BuildNum);

    // Display some of the core info available
    strmTar << L"Test CIDLib System Info methods\n"
            << TTextOutStream::RepChars(L'-', 32) << kCIDLib::NewLn
            << strmfLegend << L"Process Name: "
            << strmfData << TProcess::strProcessName() << kCIDLib::NewLn
            << strmfLegend << L"User Name: "
            << strmfData << TSysInfo::strUserName() << kCIDLib::NewLn

            << strmfLegend << L"Fac Path: "
            << strmfData << facTestCIDLib.strPath()
            << kCIDLib::NewLn

            << strmfLegend << L"CPU Type: "
            << strmfData << TSysInfo::eCPUType() << kCIDLib::NewLn

            << strmfLegend << L"OS Version: "
            << strmfData << c4OSMajVer << L"." << c4OSMinVer << L"." << c4OSRev
            << L", Build: " << c4BuildNum << kCIDLib::NewLn

            << strmfLegend << L"OS Node Name: "
            << strmfData << TSysInfo::strOSHostName() << kCIDLib::NewLn

            << strmfLegend << L"IP Node Name: "
            << strmfData << TSysInfo::strIPHostName() << kCIDLib::NewLn

            << strmfLegend << L"Physical Memory: "
            << strmfData << TLocCardinal64(TSysInfo::c8TotalPhysicalMem())
            << kCIDLib::NewLn

            << strmfLegend << L"Available Memory: "
            << strmfData << TLocCardinal64(TSysInfo::c8AvailPhysicalMem())
            << kCIDLib::NewLn

            << strmfLegend << L"Max Path Length: "
            << strmfData << kCIDLib::c4MaxPathLen << kCIDLib::NewLn

            << strmfLegend << L"Global Log Mode: "
            << strmfData << TFacility::eGlobalLogMode() << kCIDLib::NewLn

            << kCIDLib::NewLn << kCIDLib::EndLn;

    strmTar << L"Special System Paths\n"
            << TTextOutStream::RepChars(L'-', 32) << kCIDLib::NewLn

            << strmfLegend << L"Common App Data: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::CommonAppData) << kCIDLib::NewLn

            << strmfLegend << L"Common Docs: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::CommonDocs) << kCIDLib::NewLn

            << strmfLegend << L"Program Files: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::ProgramFiles) << kCIDLib::NewLn

            << strmfLegend << L"Program Files Common: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::ProgramFilesCommon) << kCIDLib::NewLn

            << strmfLegend << L"System Path: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::SystemPath) << kCIDLib::NewLn

            << strmfLegend << L"User App Data: "
            << strmfData << TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::UserAppData) << kCIDLib::NewLn
            << kCIDLib::EndLn;

    strmTar << L"Time Information\n"
            << TTextOutStream::RepChars(L'-', 32) << kCIDLib::NewLn

            << strmfLegend << L"TZ Offset: "
            << strmfData << TTime::i4Offset() << kCIDLib::NewLn

            << strmfLegend << L"TZ Name: "
            << strmfData << TTime::strTimeZoneName() << kCIDLib::DNewLn;

    // Show some lists of things
    tCIDLib::TKVPList colKVList;
    tCIDLib::TStrList colStrList;

    strmTar   << L"Available Audio Outputs\n"
                << TTextOutStream::RepChars(L'-', 32) << kCIDLib::NewLn;
    tCIDLib::TCard4 c4Count{TAudio::c4EnumAudioDevices(colKVList)};
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colKVList[c4Index];
        strmTar << L"   " << (c4Index + 1) << L". " << kvalCur.strKey()
                << L"  " << kvalCur.strValue() << kCIDLib::NewLn;
    }
    strmTar << kCIDLib::NewLn;

    strmTar   << L"Available TTS Voices\n"
                << TTextOutStream::RepChars(L'-', 32) << kCIDLib::NewLn;
    TSpeech::QueryVoiceList(colStrList);
    c4Count = colStrList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmTar << L"   " << (c4Index + 1) << L". " << colStrList[c4Index]
                << L"\n";
    }

    strmTar << kCIDLib::EndLn;
}


tCIDLib::TVoid TFacTestCIDLib::ShowTests()
{
    strmOut() << L"\nThis program provides the following tests:\n\n";

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestFuncCount; c4Index++)
        strmOut() << L"    " << aTestFunctions[c4Index].pszName << kCIDLib::NewLn;

    strmOut() << L"\nIndicate the tests to run as separate parameters\n"
              << L"No parameters will run all tests." << kCIDLib::NewLn << kCIDLib::EndLn;
}

