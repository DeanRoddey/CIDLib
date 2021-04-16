//
// FILE NAME: TestKernel.hpp
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
//  This is the main header for the program. It brings in any other headers
//  required and provides a single point of inclusion for our own sub headers.
//
// CAVEATS/GOTCHAS:
//
//  1)  Since each testing module only has a single entry point, they don't
//      use headers to export them. The main module just provides it own
//      extern prototypes.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// ----------------------------------------------------------------------------
//  CIDLib includes
// ----------------------------------------------------------------------------
#include    "CIDKernel.hpp"


// ----------------------------------------------------------------------------
//  System Includes
// ----------------------------------------------------------------------------
#include    <iostream>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>



// ----------------------------------------------------------------------------
//   CLASS: TOutStrm
//  PREFIX: strm
//
//  This is a simple wrapper class to simplify the issue of having to deal
//  with both wide and short character strings in this test, but before we
//  have our own streams available to use. It just wraps cout basically, and
//  handles providing a Unicode string conversion when compiled in the Unicode
//  mode.
//
//  The new C++ libraries have wide character strings also, but I don't want
//  to tie my stuff to all of that stuff, which some compilers might not have
//  implemented yet.
// ----------------------------------------------------------------------------
class TOutStrm
{
    public :
        // -------------------------------------------------------------------
        //  Class enums
        // -------------------------------------------------------------------
        enum class ECtrls
        {
            EndLn
            , Flush
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOutStrm()
        {
            // Set the format back to decimal
            std::cout.setf(std::ios::fixed, std::ios::fixed | std::ios::scientific);
        }

        ~TOutStrm()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOutStrm& operator<<(const tCIDLib::TCh* const pszOut)
        {
            tCIDLib::TSCh* pszTmp = TRawStr::pszConvert(pszOut);
            std::cout << pszTmp;
            delete [] pszTmp;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TCh chOut)
        {
            tCIDLib::TCh szTmp[2];
            szTmp[0] = chOut;
            szTmp[1] = 0;
            tCIDLib::TSCh* pszTmp = TRawStr::pszConvert(szTmp);
            std::cout << pszTmp;
            delete [] pszTmp;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TSCh schOut)
        {
            std::cout << schOut;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TCard1 c1Out)
        {
            std::cout << c1Out;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TCard4 c4Out)
        {
            std::cout << c4Out;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TSInt iOut)
        {
            std::cout << iOut;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TInt1 i1Out)
        {
            std::cout << i1Out;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TInt2 i2Out)
        {
            std::cout << i2Out;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TInt4 i4Out)
        {
            std::cout << i4Out;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TVoid* const pOut)
        {
            std::cout << pOut;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TSCh* const pszOut)
        {
            std::cout << pszOut;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TUInt uiOut)
        {
            std::cout << uiOut;
            return *this;
        }

        TOutStrm& operator<<(const tCIDLib::TFloat8 f8Out)
        {
            std::cout << f8Out;
            return *this;
        }

        TOutStrm& operator<<(const ECtrls eCtrl)
        {
            if (eCtrl == ECtrls::EndLn)
                std::cout << std::endl;
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public methods
        // -------------------------------------------------------------------
        int flags()
        {
            return std::cout.flags();
        }

        void flush()
        {
            std::cout.flush();
        }

        int setf(int iFlags)
        {
            return std::cout.setf(std::ios::basic_ios::fmtflags(iFlags));
        }

        int setf(int iFlags, int iMask)
        {
            return std::cout.setf
            (
                std::ios::basic_ios::fmtflags(iFlags)
                , std::ios::basic_ios::fmtflags(iMask)
            );
        }
};

extern  TOutStrm    strmOut;
#define EndLn       TOutStrm::ECtrls::EndLn



// ----------------------------------------------------------------------------
//  Facility sub-includes
// ----------------------------------------------------------------------------
#include    "TestKernel_Helper.hpp"
#include    "TestKernel_ErrorIds.hpp"
#include    "TestKernel_MessageIds.hpp"


// ----------------------------------------------------------------------------
//  Define a small record that is used by the main module to set up a list
//  of test functions and their names. We have to provide a function
//  prototype that all of the test functions use.
// ----------------------------------------------------------------------------
using TTestFunction = tCIDLib::TVoid (*)(const tCIDLib::TCard4 c4RunNumber);
struct TTestFuncRecord
{
    TTestFunction           pfnTester;
    const tCIDLib::TCh*     pszName;
    tCIDLib::TBoolean       bRun;
};


// ----------------------------------------------------------------------------
//  Intrafacility stuff from TestKernel.cpp
// ----------------------------------------------------------------------------
extern TKrnlModule kmodTestKernel;

