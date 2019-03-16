//
// FILE NAME: CIDLib_Money.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/15/1997
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
//  This file provides a formatting class for monetary values. Such values
//  have special formatting needs, which need to conform to locale specific
//  rules.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TMoney,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TMoney
//  PREFIX: mon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMoney: Public, static methods
// ---------------------------------------------------------------------------
TMoney& TMoney::Nul_TMoney()
{
    static TMoney* pmonNull = nullptr;
    if (!pmonNull)
    {
        TBaseLock lockInit;
        if (!pmonNull)
            TRawMem::pExchangePtr(&pmonNull, new TMoney);
    }
    return *pmonNull;
}


// ---------------------------------------------------------------------------
//  TMoney: Constructors and Destructor
// ---------------------------------------------------------------------------
TMoney::TMoney(const tCIDLib::TFloat8 f8Val) :

    m_f8Val(f8Val)
{
}


// ---------------------------------------------------------------------------
//  TMoney: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMoney::operator==(const TMoney& monToTest) const
{
    if (this == &monToTest)
        return kCIDLib::True;

    // Compare the values of the objects
    return (monToTest.m_f8Val == m_f8Val);
}

TMoney& TMoney::operator=(const tCIDLib::TFloat8 f8NewVal)
{
    m_f8Val = f8NewVal;
    return *this;
}

tCIDLib::TBoolean TMoney::operator==(const tCIDLib::TFloat8 f8ToTest) const
{
    return (f8ToTest == m_f8Val);
}

tCIDLib::TBoolean TMoney::operator!=(const TMoney& monToTest) const
{
    return !operator==(monToTest);
}

tCIDLib::TBoolean TMoney::operator!=(const tCIDLib::TFloat8 f8ToTest) const
{
    return (f8ToTest != m_f8Val);
}

tCIDLib::TBoolean TMoney::operator<(const TMoney& monToTest) const
{
    return (m_f8Val < monToTest.m_f8Val);
}

tCIDLib::TBoolean TMoney::operator<(const tCIDLib::TFloat8 f8ToTest) const
{
    return (m_f8Val < f8ToTest);
}

tCIDLib::TBoolean TMoney::operator<=(const TMoney& monToTest) const
{
    return (m_f8Val <= monToTest.m_f8Val);
}

tCIDLib::TBoolean TMoney::operator<=(const tCIDLib::TFloat8 f8ToTest) const
{
    return (m_f8Val <= f8ToTest);
}

tCIDLib::TBoolean TMoney::operator>(const TMoney& monToTest) const
{
    return (m_f8Val > monToTest.m_f8Val);
}

tCIDLib::TBoolean TMoney::operator>(const tCIDLib::TFloat8 f8ToTest) const
{
    return (m_f8Val > f8ToTest);
}

tCIDLib::TBoolean TMoney::operator>=(const TMoney& monToTest) const
{
    return (m_f8Val >= monToTest.m_f8Val);
}

tCIDLib::TBoolean TMoney::operator>=(const tCIDLib::TFloat8 f8ToTest) const
{
    return (m_f8Val >= f8ToTest);
}


// ---------------------------------------------------------------------------
//  TMoney: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TFloat8 TMoney::f8Val() const
{
    return m_f8Val;
}

tCIDLib::TFloat8 TMoney::f8Val(const tCIDLib::TFloat8 f8NewVal)
{
    m_f8Val = f8NewVal;
    return m_f8Val;
}


// ---------------------------------------------------------------------------
//  TMoney: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMoney::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Get a quick ref to the value
    const tCIDLib::TFloat8 f8Fmt = f8Val();

    //
    //  Format to a temp string. We pass in the basic information for
    //  formatting a monetary value. This will do it all for us, but all
    //  we want here is a value formatted basically correct. We pass a
    //  null character for the sign, cause we are going to do that ourself.
    //
    tCIDLib::TZStr128  szTmp;
    if (!TRawStr::bFormatVal(f8Fmt
                            , szTmp
                            , TLocale::c4MonetaryDecimalDigits()
                            , 128
                            , tCIDLib::ETrailFmts::Zeroes
                            , kCIDLib::chNull
                            , TLocale::chMonetaryDecimalSymbol()
                            , TLocale::c4MonetaryGroupSize()
                            , TLocale::chMonetaryGroupSeparator()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_MoneyFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Now lets query the currency format string from the default
    //  locale and replace the tokens in it. There is a different format
    //  string for positive and negative values.
    //
    TString strTmp(kCIDLib::pszEmptyZStr, 128);
    if (f8Fmt < 0.0)
        strTmp = TLocale::strNegMonFormat();
    else
        strTmp = TLocale::strPosMonFormat();

    // Replace the tokens with their values
    strTmp.eReplaceToken(szTmp, L'v');

    strTmp.eReplaceToken(TLocale::chCurrencySymbol(), L'y');

    if (f8Fmt < 0.0)
        strTmp.eReplaceToken(TLocale::chNegativeSign(), L's');

    strmToWriteTo << strTmp;
}

tCIDLib::TVoid TMoney::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom >> m_f8Val;
}

tCIDLib::TVoid TMoney::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_f8Val;
}

