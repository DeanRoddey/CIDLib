//
// FILE NAME: TestFWLib.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_TESTFWLIB)
#define TESTFWLIBEXP  CID_DLLEXPORT
#else
#define TESTFWLIBEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  Public facilty constants
// ---------------------------------------------------------------------------
namespace kTestFWLib
{
    // -----------------------------------------------------------------------
    //  The maximum complexity level that a test can have, used for filtering
    //  out more complex tests during test framework invocation.
    // -----------------------------------------------------------------------
    const   tCIDLib::TCard4     c4MaxTestLevel = 10;
}



// ---------------------------------------------------------------------------
//  Public facilty types
// ---------------------------------------------------------------------------
namespace tTestFWLib
{
    // -----------------------------------------------------------------------
    //  The phases that a test program goes through. He returns this in the
    //  return information to let the test fw know how far he got. Anything
    //  up to getting connected cannot be indicated since there's no way to
    //  get the info back until we connect to the TF. Completed means that
    //  results were returned. Anything beyond or before Completed indicates
    //  that the test program filed to run normally.
    // -----------------------------------------------------------------------
    enum class EPhases
    {
        Connect
        , Initialize
        , LoadTests
        , RunTests
        , Terminate
        , Completed
        , Exception
        , KrnlException
        , SysException

        , Count
    };


    // -----------------------------------------------------------------------
    //  The test results values for single tests. They are defined in reverse
    //  order of badness.
    // -----------------------------------------------------------------------
    enum class ETestRes
    {
        SysException
        , KrnlException
        , Exception
        , Failed
        , NotRun
        , Success

        , Count
    };


    // -----------------------------------------------------------------------
    //  The verbosity levels supported by the Test Framework
    // -----------------------------------------------------------------------
    enum class EVerbosity
    {
        Low
        , Medium
        , High
    };
}


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "TestFWLib_TestConn.hpp"
#include    "TestFWLib_TestApp.hpp"
#include    "TestFWLib_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern TESTFWLIBEXP TFacTestFWLib& facTestFWLib();


// ---------------------------------------------------------------------------
//  Provide streaming for some enums
// ---------------------------------------------------------------------------
EnumBinStreamMacros(tTestFWLib::EPhases)
EnumBinStreamMacros(tTestFWLib::ETestRes)
EnumBinStreamMacros(tTestFWLib::EVerbosity)



// ----------------------------------------------------------------------------
//  A little class that allows tess to easily format the current file and
//  line to the output stream, which they do whever they are reporting errors.
//  We define macro that they use to create the object with the current file
//  and line.
//
//  NOTE: We know that the file path comes from CID_FILE, so it's a static
//        constant string. So we just store a pointer to it. This makes it
//        efficient.
// ----------------------------------------------------------------------------
class TTFWCurLn
{
    public :
        TTFWCurLn(const   tCIDLib::TCh* const   pszFile
                  , const tCIDLib::TCard4       c4Line) :
            m_pszFile(pszFile)
            , m_c4Line(c4Line)
        {
        }

        ~TTFWCurLn()
        {
        }

        tCIDLib::TCard4         m_c4Line;
        const tCIDLib::TCh*     m_pszFile;
};

inline TTextOutStream& operator<<(TTextOutStream& strmOut, const TTFWCurLn& Cur)
{
    strmOut << L"(" << Cur.m_pszFile << L"." << Cur.m_c4Line << L" - ";
    return strmOut;
}

#define TFWCurLn TTFWCurLn(CID_FILE, CID_LINE)

