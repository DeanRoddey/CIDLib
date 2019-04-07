//
// FILE NAME: TestORB_Basic.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/2009
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
//  This file does some basic tests of ORB functionality, prior to actually
//  cranking up the two sides of the ORB and doing communications.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our main header and anything else we need
// ---------------------------------------------------------------------------
#include    "TestORB.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_ORBBasic,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_ORBBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ORBBasic: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ORBBasic::TTest_ORBBasic() :

    TTestFWTest
    (
        L"ORB Basic", L"Basic tests of ORB support code", 3
    )
{
}

TTest_ORBBasic::~TTest_ORBBasic()
{
}


// ---------------------------------------------------------------------------
//  TTest_ORBBasic: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ORBBasic::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  A fixed interface and instance id we work with here. They are not
    //  for any actual interface.
    //
    const TString strIntfId(L"5EE5012A0A5595B0-1B796F127BC99BF7");
    const TString strInstId(L"BF81F04E647D5D53-DF785C220333FF4B");

    //
    //  Just some basic tests of the ORB Id class. Just make sure what we
    //  put in comes out.
    //
    {
        TOrbId oidTest;
        oidTest.Set(strIntfId, strInstId);

        if (!oidTest.bIsInterfaceId(strIntfId))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Interface id was not recognized" << L"\n\n";
        }

        //
        //  We know what the hash of this one should be, so check it in order
        //  to catch any change in the hashing algorithm.
        //
        if (oidTest.hshThis() != 29)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Did not get known id hash" << L"\n\n";
        }

        //
        //  Get the hash, and then ask for a hash using what we know is the
        //  modulus it uses. They should be the same.
        //
        const tCIDLib::THashVal hshInt = oidTest.hshCalcHash(kCIDOrb::c4IdModulus);
        if (hshInt != oidTest.hshThis())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Interface hash is inconsistent" << L"\n\n";
        }

        //
        //  Create another one that we set through the binary hashes instead
        //  of the ASCII format as above.
        //
        TOrbId oidTest2;
        TMD5Hash mhashIntf;
        TMD5Hash mhashInst;
        mhashIntf.ParseFormatted(strIntfId);
        mhashInst.ParseFormatted(strInstId);
        oidTest2.Set(mhashIntf, mhashInst);

        // We should get these hashes back out
        if (oidTest2.mhashInterface() != mhashIntf)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got incorrect intf hash back" << L"\n\n";
        }

        if (oidTest2.mhashInstance() != mhashInst)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got incorrect instance hash back" << L"\n\n";
        }

        // And it also should have the known hash value
        if (oidTest2.hshThis() != 29)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Did not get known id hash" << L"\n\n";
        }

        // And test that we still get the interface id recognized via string
        if (!oidTest2.bIsInterfaceId(strIntfId))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Interface id was not recognized" << L"\n\n";
        }

        // And, for completenes, do one via constructor using string and hash
        TOrbId oidTest3(strIntfId, mhashInst);

        // It should be equal to the ones above
        if (oidTest3 != oidTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Ctor vs Set created differences" << L"\n\n";
        }

        // Do round trip assignment and copy ctor testing
        TOrbId oidTest4(oidTest3);
        TOrbId oidTest5;
        oidTest5 = oidTest4;

        if (oidTest5 != oidTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round trip copy/assign failed" << L"\n\n";
        }
    }


    //
    //  OK, let's test up another level for the orb object id, which is
    //  used to represent an actual in place interface, and uses the above
    //  object id as the identifier, but includes target host and port
    //  info as well.
    //
    //  In this case, since we are not using the name server or the usual
    //  auto-rebinder, we use the version of the end point that will do
    //  a name resolution to get the IP address and use localhost as the
    //  host name since this test always runs both sides locally.
    //
    {
        TOrbObjId ooidTest
        (
            strIntfId
            , strInstId
            , L"localhost"
            , 9999
            , L"TTestOrbIntfClientProxy"
        );

        // The key should be the same as above
        TOrbId oidTest(strIntfId, strInstId);

        if (ooidTest.oidKey() != oidTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The obj id key is wrong" << L"\n\n";
        }

        if (ooidTest.ippnHost() != 9999)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The host port was not set correctly" << L"\n\n";
        }

        if (ooidTest.strClientProxyClass() != L"TTestOrbIntfClientProxy")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The client proxy class was not set correctly"
                    << L"\n\n";
        }

        // Do a round trip copy/assign
        TOrbObjId ooidTest2(ooidTest);
        TOrbObjId ooidTest3;
        ooidTest3 = ooidTest2;

        if (ooidTest3 != ooidTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round trip copy/assign failed" << L"\n\n";
        }
    }


    // Test the command class (and by extension the reply class it derives from)
    {
        // Create an id for testing purposes
        TOrbId oidTest;
        oidTest.Set(strIntfId, strInstId);

        // Create a reply and round trip the id through it
        TOrbCmd ocmdTest(64);
        ocmdTest.SetTarget(oidTest);
        if (ocmdTest.oidKey() != oidTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The orb cmd target id round trip failed"
                    << L"\n\n";
        }

        // The return status should initially be false
        if (ocmdTest.bRetStatus())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Initial reply status was incorrect"
                    << L"\n\n";
        }

        // Set it and read it back out
        ocmdTest.bRetStatus(kCIDLib::True);
        if (!ocmdTest.bRetStatus())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The return status was not set correctly"
                    << L"\n\n";
        }

        // The sequence id should initially be zero
        if (ocmdTest.c4SequenceId())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Initial sequence id was incorrect"
                    << L"\n\n";
        }

        // Set it and read it back out
        ocmdTest.c4SequenceId(15);
        if (ocmdTest.c4SequenceId() != 15)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"The sequence id was not set correctly"
                    << L"\n\n";
        }

        // Stream an object into the command, and round trip it
        ocmdTest.Reset(128);
        {
            TArea areaIn(1,2,3,4);
            ocmdTest.strmOut() << areaIn;

            tCIDLib::TCard4 c4Bytes;
            THeapBuf        mbufTmp(1024UL);
            {
                TBinMBufOutStream strmOut(&mbufTmp);
                strmOut << ocmdTest << kCIDLib::FlushIt;
                c4Bytes = strmOut.c4CurPos();
            }

            TOrbCmd ocmdNew(c4Bytes + 64);
            {
                TBinMBufInStream strmIn(&mbufTmp, c4Bytes);
                strmIn >> ocmdNew;
            }

            TArea areaOut;
            ocmdNew.strmIn() >> areaOut;

            if (areaIn != areaOut)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Cmd object round trip stream failed"
                        << L"\n\n";
            }
        }
    }
    return eRes;
}

