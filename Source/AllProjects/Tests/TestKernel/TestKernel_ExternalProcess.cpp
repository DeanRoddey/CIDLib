//
// FILE NAME: TestKernel_ExternalProcess.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/1999
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
//  This module tests the TKrnlExtProcess class.
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
#include    "TestKernel.hpp"


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::TVoid CmdLineParse()
{
    struct CmdLineTest
    {
        tCIDLib::TCh*   pszTest;
        tCIDLib::TCard4 c4ExpectedCount;
        tCIDLib::TCh*   pszParms[4];
    };

    static CmdLineTest aTests[] =
    {
        {
            L"Program Parm1 Parm2"
            , 3
            , { L"Program", L"Parm1", L"Parm2", 0 }
        }
      , {
            L"Program \"Parm 1\" Parm2"
            , 3
            , { L"Program", L"Parm 1", L"Parm2", 0 }
        }
      , {
            L"Program \"Parm 1  "
            , 2
            , { L"Program", L"Parm 1", 0, 0 }
        }
      , {
            L"Program"
            , 1
            , { L"Program", 0, 0, 0 }
        }
      , {
            L"Program     Parm1          Parm2         Parm3        "
            , 4
            , { L"Program", L"Parm1", L"Parm2", L"Parm3" }
        }
      , {
            L"'The Program To Run' Parm1"
            , 2
            , { L"The Program To Run", L"Parm1", 0, 0 }
        }
      , {
            L"Program \"'Literally quoted'\""
            , 2
            , { L"Program", L"'Literally quoted'", 0, 0 }
        }
      , {
            L"Program  ParmWithEndingQuote'"
            , 2
            , { L"Program", L"ParmWithEndingQuote'", 0, 0 }
        }
      , {
            L"\"\" \"\" \"\""
            , 3
            , { L"", L"", L"", 0 }
        }

      , {
            L"    \"\""
            , 1
            , { L"", 0, 0, 0 }
        }

      , {
            L"Test\"Value "
            , 1
            , { L"Test\"Value", 0, 0, 0 }
        }
      , {
            L"    "
            , 0
            , { 0, 0, 0, 0 }
        }

    };
    const tCIDLib::TCard4 c4TestCount = sizeof(aTests) / sizeof(aTests[0]);

    const tCIDLib::TCard4 c4MaxParms = 8;
    tCIDLib::TCh* apszParms[c4MaxParms];
    tCIDLib::TCard4 c4Count;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
    {
        c4Count = TKrnlExtProcess::c4BreakOutParms
        (
            aTests[c4Index].pszTest
            , apszParms
            , c4MaxParms
        );

        if (c4Count != aTests[c4Index].c4ExpectedCount)
        {
            strmOut << CUR_LN
                    << L"Got " << c4Count << L" parameters. Expected "
                    << aTests[c4Index].c4ExpectedCount << "\n";
        }
         else
        {
            for (tCIDLib::TCard4 c4ParmInd = 0; c4ParmInd < c4Count; c4ParmInd++)
            {
                if (!TRawStr::bCompareStr(aTests[c4Index].pszParms[c4ParmInd]
                                        , apszParms[c4ParmInd]))
                {
                    strmOut << CUR_LN
                            << L"Test " << c4Index << L". Expected \""
                            << aTests[c4Index].pszParms[c4ParmInd]
                            << L"\" but got \"" << apszParms[c4ParmInd] << "\"\n";
                }
            }
        }
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestExtProcess(const tCIDLib::TCard4)
{
    // Do the test that does command line parsing testing
    CmdLineParse();
}
