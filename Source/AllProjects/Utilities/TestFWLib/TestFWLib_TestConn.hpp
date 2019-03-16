//
// FILE NAME: TestFWLib_TestConn.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
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
//  This is the header file for the TestFWLib_TesConn.cpp file. This
//  file implements the class that provides the glue between the executed
//  test program and the test framework.
//
//  It hides the details of how information is passed in and out. But it's
//  done via two named shared memory blocks. The test framework creates one
//  of these objects and sets it up, then invokes the target test program.
//  This buffer is small and just has some fields used to pass info in and
//  out, in a fixed structure, no objects. For output it includes some basic
//  status info.
//
//  The other is for getting the full test results back out. So the test app
//  being invoked will create it and fill it in with test results, which the
//  test framework can then read back out when the test finishes (or fails.)
//  This one is larger and is just flattened test results objects that are
//  streamed out, one per test that the invoked test app runs. It indicates in
//  the buffer how many subsequent bytes are valid data, so that the test
//  framework can set up a binary input stream and stream the objects back in.
//
//  If the test program fails for some reason, then this output buffer is used
//  to return error information instead, such as an exception. The completion
//  status indicates how to interpret this error data.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)



// ---------------------------------------------------------------------------
//  CLASS: TTFWOneTestRes
// PREFIX: tfwotr
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TTFWOneTestRes : public TObject, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTFWOneTestRes();

        TTFWOneTestRes
        (
            const   TTFWOneTestRes&         tfwotrSrc
        );

        TTFWOneTestRes
        (
            const   TString&                strSubName
            , const TString&                strDescr
        );

        ~TTFWOneTestRes();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTFWOneTestRes& operator=
        (
            const   TTFWOneTestRes&         tfwotrSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bWarning() const;

        tCIDLib::TBoolean bWarning
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tTestFWLib::ETestRes eResult() const;

        tTestFWLib::ETestRes eResult
        (
            const   tTestFWLib::ETestRes    eToSet
        );

        const TString& strDescription() const;

        const TString& strSubName() const;

        const TString& strOutput() const;

        TString& strOutput();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bWarning
        //      The test passed, but there were warnings. This cannot be one
        //      of the result codes, because it causes a lot of complexities
        //      there.
        //
        //  m_eRes
        //      The overall result of the tests.
        //
        //  m_strDescr
        //      A short description of the test, for human consumption.
        //
        //  m_strOutput
        //      The test will put any output here, such as error text or
        //      other commentary.
        //
        //  m_strSubName
        //      The TestFW has an overall name for the test program. This
        //      is the name of a particular test within that program.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bWarning;
        tTestFWLib::ETestRes    m_eRes;
        TString                 m_strDescr;
        TString                 m_strOutput;
        TString                 m_strSubName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWOneTestRes,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TTFWTestRes
// PREFIX: tfwtr
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TTFWTestRes : public TObject, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTFWTestRes();

        TTFWTestRes
        (
            const   TTFWTestRes&            tfwtrSrc
        );

        ~TTFWTestRes();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTFWTestRes& operator=
        (
            const   TTFWTestRes&            tfwtrSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddTestResult
        (
            const   TTFWOneTestRes&         tfwotrToAdd
        );

        tCIDLib::TBoolean bAnyFailures() const;

        tCIDLib::TBoolean bAnyNotRun() const;

        tCIDLib::TCard4 c4ResCount() const;

        const TTFWOneTestRes& tfwotrAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colResults
        //      The list of test result objects that were set on us.
        // -------------------------------------------------------------------
        TVector<TTFWOneTestRes>     m_colResults;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWTestRes,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TTestFWConn
// PREFIX: tfwcn
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TTestFWConn : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestFWConn() = delete;

        TTestFWConn
        (
            const   tCIDLib::TBoolean       bCreate
        );

        TTestFWConn(const TTestFWConn&) = delete;

        ~TTestFWConn();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestFWConn& operator=(const TTestFWConn&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::EVerbosity eQueryTestData
        (
                    tCIDLib::TCard4&        c4MaxLevel
        );

        tTestFWLib::EPhases eQueryTestRes
        (
                    TTFWTestRes&            tfwtrToFill
            ,       TString&                strErrInfo
        );

        tCIDLib::TVoid SetException
        (
            const   TError&                 errToSet
        );

        tCIDLib::TVoid SetKrnlException
        (
            const   TKrnlError&             kerrToSet
        );

        tCIDLib::TVoid SetSysException
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetPhase
        (
            const   tTestFWLib::EPhases     ePhase
        );

        tCIDLib::TVoid SetTestData
        (
            const   tTestFWLib::EVerbosity  eVerbosity
            , const tCIDLib::TCard4         c4MaxLevel
        );

        tCIDLib::TVoid SetTestRes
        (
            const   TTFWTestRes&            tfwtrToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        #pragma CIDLIB_PACK(1)
        struct TConnInfo
        {
            // Incoming info from the test framework to the app
            tCIDLib::TCard4         c4MaxLevel;
            tTestFWLib::EVerbosity  eVerbosity;
            tCIDLib::TCard4         c4InBytes;


            //
            //  Outgoing info from the app to the test framework. If the
            //  phase doesn't indicate completed (results were returned)
            //  then the result buffer is assumed to contain exception info
            //  of the type indicated by the phase and c4OutBytes is the
            //  size of that data. Else, it's the size of the results data.
            //
            tTestFWLib::EPhases     ePhase;
            tCIDLib::TCard4         c4OutBytes;
        };
        #pragma CIDLIB_POPPACK


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pmbufInfo
        //      A small named system memory buffer which is looked at via
        //      the TConnInfo structure above.
        //
        //  m_pmbufRes
        //      The named system memory buffer used to return the result data,
        //      which doesn't have a fixed structure (it's just a flattened
        //      object) and could be pretty large.
        //
        //  m_pConnInfo
        //      After we create the info buffer, we point this at it, so that
        //      this typed view is always available.
        // -------------------------------------------------------------------
        TSharedMemBuf*  m_pmbufInfo;
        TSharedMemBuf*  m_pmbufRes;
        TConnInfo*      m_pConnInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFWConn,TObject)
};

#pragma CIDLIB_POPPACK


