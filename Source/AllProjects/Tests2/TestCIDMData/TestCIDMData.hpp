//
// FILE NAME: TestCIDMData.hpp
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
//  This is the main header file of the CIDLib CIDMData test app. This is a standard
//  CIDLib test framework test app.
//
//  CIDMData is a bit of a grab bag facility, so it's a little of this and that. MData
//  means miscelleneous data related stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "CIDMData.hpp"
#include    "TestFWLib.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TMDataTest
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework. We
//  just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TMDataTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TMDataTestApp();

        TMDataTestApp(const TMDataTestApp&) = delete;
        TMDataTestApp(TMDataTestApp&&) = delete;

        ~TMDataTestApp();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        )   override;

        tCIDLib::TVoid LoadTests() override;

        tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        )   override;

        tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        )   override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMDataTestApp,TTestFWApp)
};


