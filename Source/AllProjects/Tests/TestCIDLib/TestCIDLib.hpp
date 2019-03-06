//
// FILE NAME: TestCIDLib.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This is the main header for the facility. It includes anything that is
//  needed by our program, and is the precompiled headers file.
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

        ~TFacTestCIDLib();


        // --------------------------------------------------------------------
        //  Public, static methods
        // --------------------------------------------------------------------
        static tCIDLib::TVoid Announce();
        static tCIDLib::TVoid CoreTests();
        static TTextOutStream& strmOut();
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
        static tCIDLib::TVoid TestPerThreadData();
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
        //  Unimplimented constructors and operators
        // ---------------------------------------------------------------------
        TFacTestCIDLib(const TFacTestCIDLib&);
        tCIDLib::TVoid operator=(const TFacTestCIDLib&);


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
//  Public, non-virtual methods
// ----------------------------------------------------------------------------
inline TTextOutStream& TFacTestCIDLib::strmOut()
{
    return *s_pstrmOutput;
}


// ----------------------------------------------------------------------------
//  Let everyone see the facility object
// ----------------------------------------------------------------------------
extern TFacTestCIDLib   facTestCIDLib;


// ----------------------------------------------------------------------------
//  Define a small record that is used by the main module to set up a list
//  of test functions and their names. We have to provide a function
//  prototype that all of the test functions use.
// ----------------------------------------------------------------------------
typedef tCIDLib::TVoid (*TTestFunction)();

struct TTestFuncRecord
{
    TTestFunction           pfnTester;
    const tCIDLib::TCh*     pszName;
    tCIDLib::TBoolean       bRun;
};


