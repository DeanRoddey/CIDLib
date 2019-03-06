//
// FILE NAME: Locale1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/15/1999
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
//  This is the main (only) module of the Locale1 demo program. This program
//  just does a dump of all the locale information that CIDLib provides.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  This program dumps the currently set locale (which is all that CIDLib
//      supports.) I.e. CIDLib does not support the loading of arbitary
//      locales. It just works with whatever locale is set for the process
//      by the host OS.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDLib, which is all we need.
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local data
//
//  strmOut
//      We don't need any interactivity for this program so we just set up
//      an output text stream, which writes to the standard output. Use the
//      default text encoding for this platform.
// ----------------------------------------------------------------------------
static TTextFileOutStream strmOut(tCIDLib::EStdFiles::StdOut);



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Locale11MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        strmOut << L"\nLocale1.Exe\n"
                << L"CIDLib Locale Dump Demo\n\n" << kCIDLib::EndLn;

        //
        //  Set up two stream format objects. One is for the names on the
        //  left, and one for the values on the right. They are also used
        //  for the column headings.
        //
        const tCIDLib::TCard4 c4NWidth = 26;
        TStreamFmt strmfName(c4NWidth, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfValue(0, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

        //
        //  Ok, first are all of the time oriented fields. Output the titles
        //  then all the fields.
        //
        strmOut << strmfName
                << L"Time Oriented Fields"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Time Field Values" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        strmOut << strmfName << L"AM String" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strAMString() << kCIDLib::NewLn

                << strmfName << L"Date Format" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strDateFormat() << kCIDLib::NewLn

                << strmfName << L"DateSeparator" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chDateSeparator()) << kCIDLib::NewLn

                << strmfName << L"PM String" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strPMString() << kCIDLib::NewLn

                << strmfName << L"Time Format" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strTimeFormat() << kCIDLib::NewLn

                << strmfName << L"TimeSeparator" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chTimeSeparator()) << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;


        //
        //  Next output the language oriented fields
        //
        strmOut << strmfName
                << L"Language Oriented Fields"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Language Field Values" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        strmOut << strmfName << L"Def Language Suffix" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strDefLanguageSuffix() << kCIDLib::NewLn

                << strmfName << L"English Language" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strEnglishLanguage() << kCIDLib::NewLn

                << strmfName << L"ISO Language" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strISOLanguage() << kCIDLib::NewLn

                << strmfName << L"Language" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strLanguage() << kCIDLib::NewLn

                << strmfName << L"Language Suffix" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strLanguageSuffix() << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;


        //
        //  Next output the monetary oriented fields
        //
        strmOut << strmfName
                << L"Money Oriented Fields"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Money Field Valuesn" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        strmOut << strmfName << L"Negative Format" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strNegMonFormat() << kCIDLib::NewLn

                << strmfName << L"Positive Format" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strPosMonFormat() << kCIDLib::NewLn

                << strmfName << L"Decimal Digits" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::c4MonetaryDecimalDigits() << kCIDLib::NewLn

                << strmfName << L"Group Size" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::c4MonetaryGroupSize() << kCIDLib::NewLn

                << strmfName << L"Currency Symbol" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chCurrencySymbol()) << kCIDLib::NewLn

                << strmfName << L"Decimal Symbol" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chMonetaryDecimalSymbol()) << kCIDLib::NewLn

                << strmfName << L"Group Separator" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chMonetaryGroupSeparator()) << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;


        //
        //  Next output the numeric oriented fields
        //
        strmOut << strmfName
                << L"Numeric Oriented Fields"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Numeric Field Values" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        strmOut << strmfName << L"Decimal Digits" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::c4DecimalDigits() << kCIDLib::NewLn

                << strmfName << L"Decimal Symbol" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chDecimalSymbol()) << kCIDLib::NewLn

                << strmfName << L"Group Size" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::c4GroupSize() << kCIDLib::NewLn

                << strmfName << L"Group Separator" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chGroupSeparator()) << kCIDLib::NewLn

                << strmfName << L"Positive Sign" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chPositiveSign()) << kCIDLib::NewLn

                << strmfName << L"Negative Sign" << TTextOutStream::Spaces(2)
                << strmfValue << TString(TLocale::chNegativeSign()) << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;


        //
        //  Next output the misellaneous fields
        //
        strmOut << strmfName
                << L"Miscellaneous Fields"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Miscellaneous Field Values" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        strmOut << strmfName << L"Measure System" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::eMeasure() << kCIDLib::NewLn

                << strmfName << L"Country " << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strCountry() << kCIDLib::NewLn

                << strmfName << L"ISO Country " << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strISOCountry() << kCIDLib::NewLn

                << strmfName << L"English Country " << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strEnglishCountry() << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;


        //
        //  Next output the examples fields
        //
        strmOut << strmfName
                << L"Some Formatting Examples"
                << TTextOutStream::Spaces(2)
                << strmfValue
                << L"Formatted Field Values" << kCIDLib::NewLn
                << strmfName
                << TTextOutStream::RepChars(L'-', c4NWidth)
                << TTextOutStream::Spaces(2)
                << strmfValue
                << TTextOutStream::RepChars(L'-', 40)
                << kCIDLib::EndLn;

        //
        //  Create some values to format. These are all classes that are
        //  affected by the current locale.
        //
        TTime tmCurTm(tCIDLib::ESpecialTimes::CurrentTime);
        TTime tmCurDt(tCIDLib::ESpecialTimes::CurrentDate);
        TMoney monNeg(-12345.678);
        TMoney monPos(12345.678);
        TLocFloat fNeg(-12345.678);
        TLocInteger iNeg(-123456);
        TLocCardinal cPos(123456);

        // And output the sample locale formatted values
        strmOut << strmfName << L"Current Time" << TTextOutStream::Spaces(2)
                << strmfValue << tmCurTm << kCIDLib::NewLn

                << strmfName << L"Current Date" << TTextOutStream::Spaces(2)
                << strmfValue << tmCurDt << kCIDLib::NewLn

                << strmfName << L"Negative Money" << TTextOutStream::Spaces(2)
                << strmfValue << monNeg << kCIDLib::NewLn

                << strmfName << L"Positive Money" << TTextOutStream::Spaces(2)
                << strmfValue << monPos << kCIDLib::NewLn

                << strmfName << L"Negative Float" << TTextOutStream::Spaces(2)
                << strmfValue << fNeg << kCIDLib::NewLn

                << strmfName << L"Negative Int" << TTextOutStream::Spaces(2)
                << strmfValue << iNeg << kCIDLib::NewLn

                << strmfName << L"Positive Card" << TTextOutStream::Spaces(2)
                << strmfValue << cPos << kCIDLib::NewLn

                // Note that formatting the enum is like calling TLocale::strMonth()
                << strmfName << L"Name for January" << TTextOutStream::Spaces(2)
                << strmfValue << tCIDLib::EMonths::January << kCIDLib::NewLn

                << strmfName << L"Short Name for October" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strMonthAbbrev(tCIDLib::EMonths::October)
                << kCIDLib::NewLn

                // Note that formatting the enum is like calling TLocale::strDay()
                << strmfName << L"Name for Monday" << TTextOutStream::Spaces(2)
                << strmfValue << tCIDLib::EWeekDays::Monday << kCIDLib::NewLn

                << strmfName << L"Short Name for Tuesday" << TTextOutStream::Spaces(2)
                << strmfValue << TLocale::strDayAbbrev(tCIDLib::EWeekDays::Tuesday)
                << kCIDLib::NewLn

                << kCIDLib::NewLn << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        strmOut << L"A CIDLib runtime error occured during processing.\n  Error: "
                << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

