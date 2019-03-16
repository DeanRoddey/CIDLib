//
// FILE NAME: TestCIDMData_AttrData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/2018
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
//  The header for the attribute data tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTest_AttrDataBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_AttrDataBasic : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_AttrDataBasic();

        ~TTest_AttrDataBasic();


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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestState
        (
                    TTextOutStream&         strmOut
            , const TString&                strTestName
            , const tCIDLib::TCard4         c4Line
            , const TAttrData&              adatTest
            , const TString&                strLimits
            , const tCIDMData::EAttrTypes   eType
            , const TString&                strFmtValue
        );

        tCIDLib::TBoolean bTestState2
        (
                    TTextOutStream&         strmOut
            , const TString&                strTestName
            , const tCIDLib::TCard4         c4Line
            , const TAttrData&              adatTest
            , const TString&                strAttrId
            , const TString&                strSpecType
            , const TString&                strLimits
            , const tCIDMData::EAttrTypes   eType
            , const tCIDMData::EAttrEdTypes eEditType
            , const TString&                strFmtValue
        );

        tCIDLib::TBoolean bTestState3
        (
                    TTextOutStream&         strmOut
            , const TString&                strTestName
            , const tCIDLib::TCard4         c4Line
            , const TAttrData&              adatTest
            , const TString&                strAttrId
            , const TString&                strSpecType
            , const TString&                strLimits
            , const tCIDMData::EAttrTypes   eType
            , const tCIDMData::EAttrEdTypes eEditType
            , const TString&                strFmtValue
            , const tCIDLib::TCard8         c8UserData
            , const TString&                strUserData
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_AttrDataBasic, TTestFWTest)
};
