//
// FILE NAME: TestRegX_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
//  This test program doesn't have a lot of tests, so we just implemennt them
//  all in this same file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestServers.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_NameSrv,TTestServersBase)



// ---------------------------------------------------------------------------
//  CLASS: TTest_NameSrv
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_NameSrv: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_NameSrv::TTest_NameSrv() :

    TTestServersBase(L"Name Server Abuse", L"Multi-threaded Name Server abuse test")
{
}

TTest_NameSrv::~TTest_NameSrv()
{
}


// ---------------------------------------------------------------------------
//  TTest_NameSrv: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_NameSrv::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Call our base class and tell it to run 16 threads
    InvokeThreads(16);

    // Get the overall result of the threaded tests
    eRes = eProcessThreadRes(strmOut);

    // If that's good, get the stats on the name server and check those
    if (eRes == tTestFWLib::ETestRes::Success)
    {
        try
        {
            tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();
            tCIDOrbUC::TCoreAdminProxy orbcAdmin
            (
                facCIDOrbUC().orbcCoreSrvAdminProxy
                (
                    orbcNS, TCIDNameSrvClientProxy::strAdminBinding
                )
            );

            // And query the stats
            tCIDLib::TCard8 c8Stamp = 0;
            TVector<TStatsCacheItemInfo> colValues;
            tCIDLib::TCard4 c4ValueCnt = orbcAdmin->c4QueryStats
            (
                kCIDLib::pszStat_Scope_Stats, colValues, kCIDLib::False, c8Stamp
            );

            TVector<TStatsCacheItemInfo>::TCursor cursStats(&colValues);
            for (; cursStats; ++cursStats)
            {
                const TStatsCacheItemInfo& sciiCur = *cursStats;

                if (sciiCur.strName() == kCIDOrb::pszStat_Srv_WorkQItems)
                {
                    // Make sure the work queue items are back to zero
                    if (sciiCur.c8Value() != 0)
                    {
                        eRes = tTestFWLib::ETestRes::Failed;
                        strmOut << TFWCurLn << L"WorkQ items were still in use\n\n";
                    }
                }
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_NameSrv: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTest_NameSrv::TestThreadCallback(const tCIDLib::TCard4 c4ThreadIndex)
{
    // Get a name server proxy
    tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();

    //
    //  Create a scope for our test bindings, using our index as the name. Doing
    //  it as a path means we won't have to worry about conflicts with the other
    //  therads getting the main TestServers containing scope created.
    //
    TString strMainScope(L"/TestServers/");
    strMainScope.AppendFormatted(c4ThreadIndex);
    TString strScopeDesc(L"Scope for test thread ");
    strScopeDesc.AppendFormatted(c4ThreadIndex);
    orbcNS->CreateScopePath(strMainScope, strScopeDesc);

    // Create some sub-scopes under that. Don't throw if they already exist
    orbcNS->CreateScope(strMainScope, L"SubScope1", L"Sub-scope #1", kCIDLib::False);
    orbcNS->CreateScope(strMainScope, L"SubScope2", L"Sub-scope #2", kCIDLib::False);
    orbcNS->CreateScope
    (
        strMainScope + TString(L"/SubScope1"), L"SubSubScope1", L"Sub-sub-scope #1"
        , kCIDLib::False
    );

    //  We'll run for the pre-set time
    const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMins(c4TestMinutes);

    //
    //  Set up a list of bindings that we'll use for testing. WE have to set upa n
    //  object id to create the bindings, but they can be bogus. No one is going to
    //  come looking for these and try to find the server side object. We don't even
    //  initialize the server side ORB.
    //
    //  The paths are relative to our per-thread scope created above. We save extra
    //  value for for making dynamic changes, so it's not part of the test data.
    //
    struct TTestBinding
    {
        const tCIDLib::TCh* const   pszParPath;
        const tCIDLib::TCh* const   pszName;
        const tCIDLib::TCh* const   pszRelPath;
        const tCIDLib::TCh* const   pszDescr;
        TOrbObjId                   ooidBinding;
        const tCIDLib::TCh* const   pszExtra1 = L"";
        const tCIDLib::TCh* const   pszExtra2 = L"";
        const tCIDLib::TCh* const   pszExtra3 = L"";
    };
    TTestBinding aTests[] =
    {
        {
            L"/"
            , L"Globular"
            , L"/Globular"
            , L"A catalogue of globular clusters"
            , {}
        },
        {
            L"/"
            , L"Spaghetti"
            , L"/Spaghetti"
            , L"A list of favorite pasta variations"
            , {}
            , L"By manufacturer"
        },
        {
            L"/SubScope1/"
            , L"Rock"
            , L"/SubScope1/Rock"
            , L"A source of rocks"
            , {}
            , L"Hard Rocks"
            , L"Filter=Test"
        },
        {
            L"/SubScope2/"
            , L"Green"
            , L"/SubScope1/Green"
            , L"Get any color of green you need"
            , {}
            , L"The greenest greens"
            , L"Start=0,255,0"
            , L"Hue Mode"
        }
    };
    const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aTests);

    //
    //  Run through and set the object ids. Each one needs two MD5 hashes, one
    //  for the unique interface id and another for the instance id. It doesn't
    //  really make any difference what they are here for these bogus bindings,
    //  so we just generate unique ids.
    //
    {
        TString strIntfId;
        TString strInstId;
        TMD5Hash mhashTmp;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            mhashTmp = TUniqueId::mhashMakeId();
            mhashTmp.FormatToStr(strIntfId);
            mhashTmp = TUniqueId::mhashMakeId();
            mhashTmp.FormatToStr(strInstId);

            TOrbObjId ooidTmp
            (
                strIntfId
                , strInstId
                , TSysInfo::strIPHostName()
                , 999
                , L"TBogusServerTestClass"
            );
            aTests[c4Index].ooidBinding = ooidTmp;
        }
    }

    tCIDLib::TBoolean bFirstTime = kCIDLib::True;
    tCIDLib::TBoolean bFailed = kCIDLib::False;
    tCIDLib::TStrList colNameList;
    do
    {
        TString strPath;

        //
        //  Run through the tests and delete all the bindings. We do this at the
        //  top of the loop instead of the bottom in case there is stuff left
        //  around from the previous run. So, the first time, we will tell it not
        //  to throw if the binding doesn't exist. After that it must exist.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            const TTestBinding& testCur = aTests[c4Index];

            strPath = strMainScope;
            strPath.Append(testCur.pszRelPath);
            orbcNS->RemoveBinding(strPath, !bFirstTime);
        }

        // Sleep a little bit, based on thread index for variation
        TThread::Sleep(50 + (2 * c4ThreadIndex));

        // Run through the tests and create the all bindings
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            const TTestBinding& testCur = aTests[c4Index];

            strPath = strMainScope;
            strPath.Append(testCur.pszRelPath);

            orbcNS->BindObj
            (
                strPath
                , testCur.pszDescr
                , testCur.pszExtra1
                , testCur.pszExtra2
                , testCur.pszExtra3
                , TString::strEmpty()
                , testCur.ooidBinding
            );
        }

        // Sleep a little bit again
        TThread::Sleep(100 + (2 * c4ThreadIndex));

        // Go back now and read back all of the bindings and compare them
        TString strName;
        TString strDescr;
        TString strExtra1;
        TString strExtra2;
        TString strExtra3;
        TString strExtra4;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            const TTestBinding& testCur = aTests[c4Index];

            strPath = strMainScope;
            strPath.Append(testCur.pszRelPath);

            orbcNS->QueryNameInfo2
            (
                strPath, strName, strDescr, strExtra1, strExtra2, strExtra3, strExtra4
            );

            //
            //  Cut the main scope prefix off of the path we got, sice we store
            //  just the relative part.
            //
            strPath.Cut(0, strMainScope.c4Length());
            if (strPath != testCur.pszRelPath)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct path for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strPath);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }

            if (strName != testCur.pszName)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct path for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strName);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }

            if (strDescr != testCur.pszDescr)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct description for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strDescr);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }

            if (strExtra1 != testCur.pszExtra1)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct Extra1 for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strExtra1);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }

            if (strExtra2 != testCur.pszExtra2)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct Extra2 for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strExtra2);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }

            if (strExtra3 != testCur.pszExtra3)
            {
                TString strErr(128UL);
                strErr = L"Did not read back correct Extra3 for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L". Got=");
                strErr.Append(strExtra3);
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }
        }

        if (bFailed)
            break;

        // Sleep a little bit again
        TThread::Sleep(100 + (3 * c4ThreadIndex));

        //
        //  For each one, update the 4th extra value and then query it back again
        //  and check it.
        //
        const TString strTest4Val(L"New Extra 4 Value");
        TOrbObjId ooidTest;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            const TTestBinding& testCur = aTests[c4Index];

            strPath = strMainScope;
            strPath.Append(testCur.pszRelPath);
            orbcNS->UpdateExtraVal(strPath, 4, strTest4Val);

            orbcNS->QueryNameInfo2
            (
                strPath, strName, strDescr, strExtra1, strExtra2, strExtra3, strExtra4
            );

            if (strExtra4 != strTest4Val)
            {
                TString strErr(128UL);
                strErr = L"Modifying extra 4 for test #";
                strErr.AppendFormatted(c4Index);
                strErr.Append(L" did not return the new value");
                SetThreadFailure(c4ThreadIndex, strErr);
                bFailed = kCIDLib::True;
                break;
            }
        }

        if (bFailed)
            break;

        //
        //  Query all the entries in our top level per-thread scope. We have two sub-scopes
        //  and two bindings.
        //
        tCIDLib::TCard4 c4SerNum = 0;
        if (!orbcNS->bQueryScopeKeys(c4SerNum, strMainScope, colNameList, kCIDLib::False, kCIDLib::True))
        {
            TString strErr(128UL);
            strErr = L"Failed to enumerate values in main scope: ";
            strErr.AppendFormatted(strMainScope);
            SetThreadFailure(c4ThreadIndex, strErr);
            bFailed = kCIDLib::True;
            break;
        }

        if (colNameList.c4ElemCount() != 4)
        {
            TString strErr(128UL);
            strErr = L"Expected 4 values in main scope: ";
            strErr.AppendFormatted(strMainScope);
            SetThreadFailure(c4ThreadIndex, strErr);
            bFailed = kCIDLib::True;
            break;
        }

        //
        //  Do it again, which should get nothing back since we pass in the serial number
        //  we got above and the scope shouldn't have changed.
        //
        if (orbcNS->bQueryScopeKeys(c4SerNum, strMainScope, colNameList, kCIDLib::False, kCIDLib::True))
        {
            TString strErr(128UL);
            strErr = L"Main scope: ";
            strErr.AppendFormatted(strMainScope);
            strErr.Append(L" should not have changed since the last query");
            SetThreadFailure(c4ThreadIndex, strErr);
            bFailed = kCIDLib::True;
            break;
        }

        bFirstTime = kCIDLib::False;
    }   while (TTime::enctNow() < enctEnd);

    // If no failures set success status
    if (!bFailed)
        SetThreadSuccess(c4ThreadIndex);
}
