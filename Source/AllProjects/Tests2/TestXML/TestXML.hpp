//
// FILE NAME: TestXML.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/05/2008
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
//  This is the main header file of the CIDLib XML parser test app. This is
//  a standard CIDLib test framework test app. The actual tests are d
//  distributed through a set of ancillary files.
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
#include    "CIDXML.hpp"
#include    "TestFWLib.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TTest_Attr
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Attr : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Attr();

        ~TTest_Attr();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  We use a set of test values over and over, so we store them here
        //  once.
        // -------------------------------------------------------------------
        TString m_strBaseName;
        TString m_strPrefix;
        TString m_strURI;
        TString m_strValue;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckContents
        (
            const   TXMLAttr&               xattrToTest
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Attr,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TXMLTestApp
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TXMLTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TXMLTestApp();

        TXMLTestApp(const TXMLTestApp&) = delete;

        ~TXMLTestApp();


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
        RTTIDefs(TXMLTestApp,TTestFWApp)
};


