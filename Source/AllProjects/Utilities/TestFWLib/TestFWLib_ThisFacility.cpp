//
// FILE NAME: TestFWLib_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
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
//  This file implements TFacTestFWLib class, our facility class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "TestFWLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacTestFWLib,TFacility)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace TTestFWLib
{
    // -----------------------------------------------------------------------
    //  We load up these with the text version of the phase enum
    // -----------------------------------------------------------------------
    const TString astrPhaseVals[] =
    {
        L"Connect"
        , L"Initialize"
        , L"Load Tests"
        , L"Run Tests"
        , L"Terminate"
        , L"Completed"
        , L"Exception"
        , L"Krnl Exception"
        , L"System Exception"
    };
    TEArray<TString, tTestFWLib::EPhases, tTestFWLib::EPhases::Count> astrPhaseText
    (
        astrPhaseVals
    );


    // -----------------------------------------------------------------------
    //  We load up these with the text version of the test results enum
    // -----------------------------------------------------------------------
    const TString astrResVals [] =
    {
        L"System Exception"
        , L"Krnl Exception"
        , L"Exception"
        , L"Failed"
        , L"Not Run"
        , L"Success"
    };
    TEArray<TString, tTestFWLib::ETestRes, tTestFWLib::ETestRes::Count> astrResText
    (
        astrResVals
    );
}



// ---------------------------------------------------------------------------
//  TFacTestFWLib: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacTestFWLib::TFacTestFWLib() :

    TFacility
    (
        L"TestFWLib"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::None
    )
    , m_strUnknown(L"????")
{
}

TFacTestFWLib::~TFacTestFWLib()
{
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TFacTestFWLib::strPhaseText(const tTestFWLib::EPhases eToXlat)
{
    if (eToXlat >= tTestFWLib::EPhases::Count)
        return m_strUnknown;
    return TTestFWLib::astrPhaseText[eToXlat];
}


const TString& TFacTestFWLib::strResText(const tTestFWLib::ETestRes eToXlat)
{
    if (eToXlat >= tTestFWLib::ETestRes::Count)
        return m_strUnknown;
    return TTestFWLib::astrResText[eToXlat];
}

