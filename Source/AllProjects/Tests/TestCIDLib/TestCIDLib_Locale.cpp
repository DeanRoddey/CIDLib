//
// FILE NAME: TestCIDLib_Locale.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/1997
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  locale support functionality.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// -----------------------------------------------------------------------------
//  Locale test methods
// -----------------------------------------------------------------------------

static tCIDLib::TVoid BasicLocaleTests(TTextOutStream& strmOut)
{
    // Display the locale information for the default locale
    strmOut << kCIDLib::NewLn << L"Default locale information" << kCIDLib::NewLn
            << L"--------------------------" << kCIDLib::NewLn;

    strmOut << L"          Country: " << TLocale::strCountry() << kCIDLib::NewLn
            << L"  English Country: " << TLocale::strEnglishCountry() << kCIDLib::NewLn
            << L"      ISO Country: " << TLocale::strISOCountry() << kCIDLib::NewLn
            << L"         Language: " << TLocale::strLanguage() << kCIDLib::NewLn
            << L" English Language: " << TLocale::strEnglishLanguage() << kCIDLib::NewLn
            << L"        Neg Money: " << TLocale::strNegMonFormat() << kCIDLib::NewLn
            << L"        Pos Money: " << TLocale::strPosMonFormat() << kCIDLib::NewLn
            << L"  Currency Symbol: " << TString(TLocale::chCurrencySymbol()) << kCIDLib::NewLn
            << L"   Mon Group Size: " << TLocale::c4MonetaryGroupSize() << kCIDLib::NewLn
            << L"    Mon Group Sep: " << TString(TLocale::chMonetaryGroupSeparator()) << kCIDLib::NewLn
            << L"      Mon Decimal: " << TString(TLocale::chMonetaryDecimalSymbol()) << kCIDLib::NewLn
            << L"       Group Size: " << TLocale::c4GroupSize() << kCIDLib::NewLn
            << L"        Group Sep: " << TString(TLocale::chGroupSeparator()) << kCIDLib::NewLn
            << L"   Decimal Symbol: " << TString(TLocale::chDecimalSymbol()) << kCIDLib::NewLn
            << L"      Time Format: " << TLocale::strTimeFormat() << kCIDLib::NewLn
            << L"      Date Format: " << TLocale::strDateFormat() << kCIDLib::NewLn
            << kCIDLib::NewLn << kCIDLib::EndLn;
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the locale support classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestLocales()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic info";
        BasicLocaleTests(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" locale test" << kCIDLib::EndLn;
        throw;
    }
}
