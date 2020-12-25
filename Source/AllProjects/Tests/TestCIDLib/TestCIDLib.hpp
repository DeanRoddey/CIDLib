//
// FILE NAME: TestCIDLib.hpp
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
//  These are very basic tests, just to make sure there's nothing fundamentally wrong
//  and that the test framework based tests shouldn't have an issue running.
//
//  We keep things quite simple here, since we are testing stuff, we don't want to
//  use features aren't even really tested until up in the framework based tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Bring in the CIDlib facility we test
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Bring in any intrafacility headers that we have
// ----------------------------------------------------------------------------
#include    "TestCIDLib_ErrorIds.hpp"
#include    "TestCIDLib_MessageIds.hpp"


// ----------------------------------------------------------------------------
//  A tiny little class that allows me to easily format the current file and
//  line to the output stream.
// ----------------------------------------------------------------------------
class TCurLn
{
    public :
        TCurLn(   const   tCIDLib::TSCh* const  pszFile
                  , const tCIDLib::TCard4       c4Line) :
            m_pszFile(0)
            , m_c4Line(c4Line)
        {
            m_pszFile = TRawStr::pszConvert(pszFile);
        }

        TCurLn(   const   tCIDLib::TCh* const   pszFile
                  , const tCIDLib::TCard4       c4Line) :
            m_pszFile(0)
            , m_c4Line(c4Line)
        {
            m_pszFile = TRawStr::pszReplicate(pszFile);
        }

        ~TCurLn()
        {
            delete [] m_pszFile;
        }

        tCIDLib::TCh*   m_pszFile;
        tCIDLib::TCard4 m_c4Line;
};

inline TTextOutStream& operator<<(TTextOutStream& strmOut, const TCurLn& Cur)
{
    strmOut << L"(" << Cur.m_pszFile << L"." << Cur.m_c4Line << L" - ";
    return strmOut;
}

#define CUR_LN TCurLn(CID_FILE, CID_LINE)



// ----------------------------------------------------------------------------
//  CLASS: TFacTestCIDLib
// PREFIX: fac
// ----------------------------------------------------------------------------
class TFacTestCIDLib : public TFacility
{
    public  :
        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        TFacTestCIDLib();

        TFacTestCIDLib(const TFacTestCIDLib&) = delete;

        ~TFacTestCIDLib();


        // ---------------------------------------------------------------------
        //  Public operators
        // ---------------------------------------------------------------------
        TFacTestCIDLib& operator=(const TFacTestCIDLib&) = delete;


        // --------------------------------------------------------------------
        //  Public, static methods
        // --------------------------------------------------------------------
        static tCIDLib::TVoid Announce();
        static tCIDLib::TVoid CoreTests();

        static TTextOutStream& strmOut()
        {
            return *s_pstrmOutput;
        }

        static tCIDLib::TVoid ShowSystemInfo();
        static tCIDLib::TVoid ShowTests();
        static tCIDLib::TVoid TestBags();
        static tCIDLib::TVoid TestBinStreams();
        static tCIDLib::TVoid TestColCommon();
        static tCIDLib::TVoid TestDeques();
        static tCIDLib::TVoid TestEnvironment();
        static tCIDLib::TVoid TestFundData();
        static tCIDLib::TVoid TestFiles();
        static tCIDLib::TVoid TestKeyedHashSet();
        static tCIDLib::TVoid TestHashMap();
        static tCIDLib::TVoid TestHashSet();
        static tCIDLib::TVoid TestLocales();
        static tCIDLib::TVoid TestMemoryClasses();
        static tCIDLib::TVoid TestObjArrays();
        static tCIDLib::TVoid TestPointerClasses();
        static tCIDLib::TVoid TestProcess();
        static tCIDLib::TVoid TestQueues();
        static tCIDLib::TVoid TestQStreams();
        static tCIDLib::TVoid TestRTTI();
        static tCIDLib::TVoid TestSearchSort();
        static tCIDLib::TVoid TestStacks();
        static tCIDLib::TVoid TestStrings();
        static tCIDLib::TVoid TestTextConverters();
        static tCIDLib::TVoid TestTextStreams();
        static tCIDLib::TVoid TestTreeCols();
        static tCIDLib::TVoid TestVectors();


    private :
        // ---------------------------------------------------------------------
        //  Private data members
        //
        //  s_pstrmOutput
        //      This is the stream to which all output will go. This allows
        //      us to redirect the output to where it is desired.
        // ---------------------------------------------------------------------
        static TTextOutStream*  s_pstrmOutput;
};



// ----------------------------------------------------------------------------
//  Let everyone see the facility object
// ----------------------------------------------------------------------------
extern TFacTestCIDLib   facTestCIDLib;


// ----------------------------------------------------------------------------
//  Define a small record that is used by the main module to set up a list
//  of test functions and their names. We have to provide a function
//  prototype that all of the test functions use.
// ----------------------------------------------------------------------------
using TTestFunction = tCIDLib::TVoid (*)();
struct TTestFuncRecord
{
    TTestFunction           pfnTester;
    const tCIDLib::TCh*     pszName;
    tCIDLib::TBoolean       bRun;
};
