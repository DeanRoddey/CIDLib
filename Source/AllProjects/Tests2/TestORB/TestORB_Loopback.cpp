//
// FILE NAME: TestORB_LocalLoopback.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2009
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
//  This file does a test of the ORB but just within this app, so we create
//  an interface, implement the server side, create an instance an register
//  it, then we just make calls to ourself all within the same process.
//
//  Since we are all just within the same process, there's no issue with
//  the client side getting the object id of the target, we just store it
//  as we create the server side object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our main header and anything else we need. We can't directly pull
//  in the IDL generated files here, though we are the only user of them,
//  because the IDL generated files assumes the standard inclusion through
//  the main facility header, so we have to include them there.
// ---------------------------------------------------------------------------
#include    "TestORB.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_ORBLoopback,TTestFWTest)



// ---------------------------------------------------------------------------
//  Implement the server side interface
// ---------------------------------------------------------------------------
class TLoopbackImpl : public TTestOrbIntfServerBase
{
    public :
        // --------------------------------------------------------------------
        // Public Destructor
        // --------------------------------------------------------------------
        TLoopbackImpl(  tCIDLib::TBoolean* const    pbInitCalled
                        , tCIDLib::TBoolean* const  pbTermCalled) :

            m_areaData(1, 2, 3, 4)
            , m_c4SerialNum(1)
            , m_pbInitCalled(pbInitCalled)
            , m_pbTermCalled(pbTermCalled)
        {
        }

        ~TLoopbackImpl()
        {
        }

        // --------------------------------------------------------------------
        // Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TCard4 c4BumpIt(const tCIDLib::TCard4 c4Input)
        {
            return c4Input + 1;
        }

        tCIDLib::TBoolean bBagParamTest
        (
            const   TBag<TArea>&            colIn
            ,       TBag<TPoint>&           colOut
            ,       TString&                strFailReason
        );

        tCIDLib::TBoolean bPollData(tCIDLib::TCard4& c4SerialNum, TArea& areaRet)
        {
            //
            //  If the serial number is different from ours, then update the
            //  area and return true. Else return false, but modify the area
            //  anyway, so that if the ORB incorrectly streams it back, it
            //  will be noticed.
            //
            if (c4SerialNum != m_c4SerialNum)
            {
                areaRet = m_areaData;
                c4SerialNum = m_c4SerialNum;
                return kCIDLib::True;
            }

            // Bogus modification. Shouldn't be streamed back
            areaRet.AdjustOrg(1, 1);
            areaRet.AdjustSize(1, 1);

            return kCIDLib::False;
        }

        tCIDLib::TBoolean bParamTest
        (
            const   tCIDLib::TCard4         c4In
            ,       tCIDLib::TCard4&        c4Out
            , const tCIDLib::TFloat8        f8In
            ,       tCIDLib::TFloat8&       f8Out
            , const TString&                strIn
            ,       TString&                strOut
            ,       TString&                strInOut
            , const tCIDLib::TBoolean       bIn
            ,       tCIDLib::TBoolean&      bOut
            ,       TString&                strFailReason
        );

        tCIDLib::TBoolean bVectorParamTest
        (
                    TVector<TString>&       colInOut
            ,       TString&                strFailReason
        );

        tCIDLib::TVoid
        BulkTest(tCIDLib::TCard4& c4BufSz_mbufOut, THeapBuf& mbufOut)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BufSz_mbufOut; c4Index++)
                mbufOut[c4Index] = tCIDLib::TCard1(mbufOut[c4Index]) ^ 0xFF;

            //
            //  We just leave the input count alone, since we return the same
            //  number of bytes we got in.
            //
        }

        tCIDLib::TVoid ExceptTest()
        {
            // Just throw a known exception back to the client
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcGen_NotWhenRunning
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        tCIDLib::TVoid ExceptTest(const tCIDLib::TCard4 c4Dummy)
        {
            // Just throw a known exception back to the client
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_BadSeqReply
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        tCIDLib::TVoid SetData(const TArea& areaToSet)
        {
            // Store the new and bump the serial number
            m_areaData = areaToSet;
            m_c4SerialNum++;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize()
        {
            // Call our parent and then mark that we were initialized correctly
            TParent::Initialize();
            *m_pbInitCalled = kCIDLib::True;
        }

        tCIDLib::TVoid Terminate()
        {
            // Call our parent and then mark that we were initialized correctly
            TParent::Terminate();
            *m_pbTermCalled = kCIDLib::True;
        }


    private :
        // --------------------------------------------------------------------
        // Unimplemented ctors and operators
        // --------------------------------------------------------------------
        TLoopbackImpl(const TLoopbackImpl&);
        tCIDLib::TVoid operator=(const TLoopbackImpl&);


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaData
        //      A value we let the client set and read, to test poll methods.
        //
        //  m_c4SerialNum
        //      A faux data serial number, used by bPollData().
        //
        //  m_pbInitCalled
        //  m_pbTermCalled
        //      Just for the sake of testing we remember if our init and term
        //      were called by the ORB at is should be. We have to take ptrs
        //      to the flags because we'll be destroyed by the time the test
        //      code can check them.
        // --------------------------------------------------------------------
        TArea               m_areaData;
        tCIDLib::TBoolean*  m_pbInitCalled;
        tCIDLib::TBoolean*  m_pbTermCalled;
        tCIDLib::TCard4     m_c4SerialNum;


        // --------------------------------------------------------------------
        // Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TLoopbackImpl,TTestOrbIntfServerBase)
};
RTTIDecls(TLoopbackImpl,TTestOrbIntfServerBase)


//
//  This one takes in various types of parms and checks the incoming values,
//  and sends back specific values that the client can check.
//
tCIDLib::TBoolean
TLoopbackImpl::bParamTest(  const   tCIDLib::TCard4     c4In
                            ,       tCIDLib::TCard4&    c4Out
                            , const tCIDLib::TFloat8    f8In
                            ,       tCIDLib::TFloat8&   f8Out
                            , const TString&            strIn
                            ,       TString&            strOut
                            ,       TString&            strInOut
                            , const tCIDLib::TBoolean   bIn
                            ,       tCIDLib::TBoolean&  bOut
                            ,       TString&            strFailReason)
{
    if (c4In != 10)
    {
        strFailReason = L"Incoming c4In param was bad";
        return kCIDLib::False;
    }
    c4Out = 11;

    if (f8In != 3.11)
    {
        strFailReason = L"Incoming f8In param was bad";
        return kCIDLib::False;
    }
    f8Out = 3.12;

    if (strIn != L"Test string in")
    {
        strFailReason = L"Incoming strIn param was bad";
        return kCIDLib::False;
    }
    strOut = L"Test string out";

    if (strInOut != L"Test string in")
    {
        strFailReason = L"Incoming strInOu param was bad";
        return kCIDLib::False;
    }
    strInOut = L"Test string out";

    if (bIn)
    {
        strFailReason = L"Incoming bIn param was bad";
        return kCIDLib::False;
    }
    bOut = kCIDLib::True;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TLoopbackImpl::bBagParamTest(const  TBag<TArea>&    colIn
                            ,       TBag<TPoint>&   colOut
                            ,       TString&        strFailReason)
{
    // We are expecting 4 elements
    if (colIn.c4ElemCount() != 4)
    {
        strFailReason = L"Element count was wrong";
        return kCIDLib::False;
    }

    // Iterate the strings and make sure that they have the right values
    TBag<TArea>::TCursor cursTest(&colIn);
    if (!cursTest.bReset())
    {
        strFailReason = L"No elements in bag";
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4Cur = 1;
    do
    {
        if (cursTest.objRCur() != TArea(c4Cur, c4Cur+1, c4Cur+2, c4Cur+3))
        {
            strFailReason = L"Test area was wrong";
            return kCIDLib::False;
        }
        c4Cur++;
        colOut.objAdd(cursTest.objRCur().pntOrg());
    }   while (cursTest.bNext());

    return kCIDLib::True;
}


tCIDLib::TBoolean
TLoopbackImpl::bVectorParamTest(TVector<TString>&   colInOut
                                , TString&          strFailReason)
{
    if (colInOut.c4ElemCount() != 4)
    {
        strFailReason = L"Element count was wrong";
        return kCIDLib::False;
    }

    if ((colInOut[0] != L"This is a 1")
    ||  (colInOut[1] != L"This is a 2")
    ||  (colInOut[2] != L"This is a 3")
    ||  (colInOut[3] != L"This is a 4"))
    {
        strFailReason = L"One of the incoming strings has a bad value";
        return kCIDLib::False;
    }

    // Update the strings for return
    colInOut[0] = L"This is a test 1";
    colInOut[1] = L"This is a test 2";
    colInOut[2] = L"This is a test 3";
    colInOut[3] = L"This is a test 4";

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ORBLoopback
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ORBLoopback: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ORBLoopback::TTest_ORBLoopback() :

    TTestFWTest
    (
        L"ORB Loopback", L"Single process client/server test", 4
    )
{
}

TTest_ORBLoopback::~TTest_ORBLoopback()
{
}


// ---------------------------------------------------------------------------
//  TTest_ORBLoopback: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ORBLoopback::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Create and register our server side object. The ORB owns the object
    //  not us. We just keep a pointer so that we can get the object id out.
    //  We provide him pointers to init/term called flags that we'll check
    //  at the end.
    //
    tCIDLib::TBoolean bInitCalled = kCIDLib::False;
    tCIDLib::TBoolean bTermCalled = kCIDLib::False;
    TLoopbackImpl* porbsTest = new TLoopbackImpl(&bInitCalled, &bTermCalled);
    facCIDOrb().RegisterObject(porbsTest);

    // At this point the init method should have been called
    if (!bInitCalled)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Server object init was not called" << L"\n\n";
    }

    //
    //  Now let's create our client proxy. In this case we can get the object
    //  id directly out of the server object since we created it, too.
    //
    TTestOrbIntfClientProxy orbcTest(porbsTest->ooidThis(), TString::strEmpty());

    //
    //  Do the bump test. We just start a value and run it up a few thousand
    //  values, to bang hard on the ORB.
    //
    tCIDLib::TCard4 c4BumpVal = 0;
    while (c4BumpVal < 2048)
    {
        const tCIDLib::TCard4 c4NewVal = orbcTest.c4BumpIt(c4BumpVal);
        if (c4NewVal != c4BumpVal + 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bump tests did not bump value" << L"\n\n";
            break;
        }
        c4BumpVal = c4NewVal;
    }

    //
    //  Test the poll method. If such a method returns false, the ORB should
    //  not stream the parameters back. Else it should. First time we should
    //  get a value back.
    //
    {
        tCIDLib::TCard4 c4SerialNum = 0;
        TArea areaTest;
        if (!orbcTest.bPollData(c4SerialNum, areaTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Failed to get initial poll data"
                    << kCIDLib::DNewLn;
        }

        //
        //  Call again and we should get no data back since we've not called
        //  SetData(). But modify the area so that if it does, we'll know.
        //
        areaTest.ZeroAll();
        if (orbcTest.bPollData(c4SerialNum, areaTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should not have gotten poll data"
                    << kCIDLib::DNewLn;
        }

        if (!areaTest.bIsEmpty())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Poll data was incorrectly streamed back"
                    << kCIDLib::DNewLn;
        }

        // Now call SetData to change the data, and we should get new data next time
        orbcTest.SetData(TArea(10, 120, 28, 277));

        if (!orbcTest.bPollData(c4SerialNum, areaTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should have gotten new poll data"
                    << kCIDLib::DNewLn;
        }

        // Check the value, which should be the new one we just set
        if (areaTest != TArea(10, 120, 28, 277))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"New poll data was not correct"
                    << kCIDLib::DNewLn;
        }

        // And once again we shouldn't get any data
        areaTest.ZeroAll();
        if (orbcTest.bPollData(c4SerialNum, areaTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should not have gotten poll data"
                    << kCIDLib::DNewLn;
        }

        if (!areaTest.bIsEmpty())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Poll data was incorrectly streamed back"
                    << kCIDLib::DNewLn;
        }
    }

    //
    //  Do the first parameter test, which does a good number of parameters of
    //  various basic types.
    //
    {
        tCIDLib::TCard4     c4Test = 10;
        tCIDLib::TFloat8    f8Test = 3.11;
        TString             strTest = L"Test string in";
        TString             strIOTest = L"Test string in";
        tCIDLib::TBoolean   bTest = kCIDLib::False;
        TString             strFailReason;

        if (orbcTest.bParamTest(c4Test
                                , c4Test
                                , f8Test
                                , f8Test
                                , strTest
                                , strTest
                                , strIOTest
                                , bTest
                                , bTest
                                , strFailReason))
        {
            if (c4Test != 11)
            {
                strmOut << L"TCard4 out parm was not correct" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (f8Test != 3.12)
            {
                strmOut << L"TFloat8 out parm was not correct" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (strTest != L"Test string out")
            {
                strmOut << L"Test string out parm was not correct" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (strIOTest != L"Test string out")
            {
                strmOut << L"Test string inout parm was not correct" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (bTest != kCIDLib::True)
            {
                strmOut << L"TBoolean out parm was not correct" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
         else
        {
            strmOut << L"Server error: " << strFailReason << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Do the simple exception test. This call just throws an exception on
    //  the server side, which should propogate back to us here. We call it
    //  in a loop to test for leaks or errors in the error propogation code.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 512; c4Index++)
    {
        tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
        try
        {
            // Call one or the other variation, to test overloading
            if (c4Index & 1)
                orbcTest.ExceptTest();
            else
                orbcTest.ExceptTest(tCIDLib::TCard4(1));
        }

        catch(const TError& errToCatch)
        {
            if (c4Index & 1)
            {
                if (!errToCatch.bCheckEvent(facCIDOrb().strName()
                                            , kOrbErrs::errcGen_NotWhenRunning))
                {
                    strmOut << L"Expected exception 1 to be thrown" << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
            }
             else
            {
                if (!errToCatch.bCheckEvent(facCIDOrb().strName()
                                            , kOrbErrs::errcClient_BadSeqReply))
                {
                    strmOut << L"Expected exception 2 to be thrown" << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
            }

            bCaughtIt = kCIDLib::True;
        }

        if (!bCaughtIt)
        {
            strmOut << L"Expected exception was not thrown" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Break out once we fail. Makes no sense to keep going
        if (eRes != tTestFWLib::ETestRes::Success)
            break;
    }


    //
    //  Fill up a bag of area objects and send it over. The server will return
    //  the origin point of each area in the return bag of points.
    //
    {
        TBag<TArea> colIn;
        colIn.objAdd(TArea(1, 2, 3, 4));
        colIn.objAdd(TArea(2, 3, 4, 5));
        colIn.objAdd(TArea(3, 4, 5, 6));
        colIn.objAdd(TArea(4, 5, 6, 7));

        TBag<TPoint>    colOut;
        TString         strFailReason;
        if (orbcTest.bBagParamTest(colIn, colOut, strFailReason))
        {
            if (colOut.c4ElemCount() != 4)
            {
                strmOut << L"Returned bag has wrong element count"
                        << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
             else
            {
                // Check the output
                TBag<TPoint>::TCursor cursOut(&colOut);
                if (cursOut.bReset())
                {
                    tCIDLib::TCard4 c4Val = 1;
                    do
                    {
                        if (cursOut.objRCur() != TPoint(c4Val, c4Val + 1))
                        {
                            strmOut << L"Returned point has incorrect value" << kCIDLib::EndLn;
                            eRes = tTestFWLib::ETestRes::Failed;
                            break;
                        }
                        c4Val++;
                    }   while (cursOut.bNext());
                }
                 else
                {
                    strmOut << L"No elements were in the output bag" << kCIDLib::EndLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
            }
        }
         else
        {
            strmOut << L"Server error: " << strFailReason << kCIDLib::EndLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Fill up a vector of string objects and send it over. The server will
    //  adjust each element and send them back.
    //
    {
        TVector<TString> colInOut(16);
        colInOut.objAdd(TString(L"This is a 1"));
        colInOut.objAdd(TString(L"This is a 2"));
        colInOut.objAdd(TString(L"This is a 3"));
        colInOut.objAdd(TString(L"This is a 4"));

        tCIDLib::TBoolean   bRet = kCIDLib::True;
        TString             strFailReason;
        if (orbcTest.bVectorParamTest(colInOut, strFailReason))
        {
            if ((colInOut[0] != L"This is a test 1")
            ||  (colInOut[1] != L"This is a test 2")
            ||  (colInOut[2] != L"This is a test 3")
            ||  (colInOut[3] != L"This is a test 4"))
            {
                strmOut << L"Returned string has incorrect value"
                        << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
         else
        {
            strmOut << L"Server error: " << strFailReason << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }


    //
    //  Allocate a big buffer and initialize it to a sequence of values
    //  so we can make sure we get it back correctly.
    //
    {
        const tCIDLib::TCard4 c4Bytes = 0x100000;

        THeapBuf mbufTmp(c4Bytes);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Bytes; c4Index++)
            mbufTmp[c4Index] = tCIDLib::TCard1(c4Index);

        //
        //  Send it to the other side and then get the reply, which should
        //  send it back with each value XOR'd with 0xFF.
        //
        tCIDLib::TCard4 c4ToSend = c4Bytes;
        orbcTest.BulkTest(c4ToSend, mbufTmp);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Bytes; c4Index++)
        {
            if (mbufTmp[c4Index] != (0xFF ^ tCIDLib::TCard1(c4Index)))
            {
                strmOut << L"Bulk data was not modified correctly"
                        << kCIDLib::NewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
    }

    //
    //  And now we can deregister our server object since we are done testing.
    //  This should destry the object after calling its term method.
    //
    facCIDOrb().DeregisterObject(porbsTest);

    // Make sure that the server object's term method was called
    if (!bTermCalled)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Server object term was not called" << kCIDLib::DNewLn;
    }

    return eRes;
}

