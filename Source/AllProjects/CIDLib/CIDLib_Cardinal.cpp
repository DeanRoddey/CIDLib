//
// FILE NAME: CIDLib_Cardinal.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/1992
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
//  This file provides some simple wrapper classes for 32 and 64 bit
//  cardinals, which allow them to be treated as objects when needed.
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
AdvRTTIDecls(TCardinal,TObject)
AdvRTTIDecls(TCardinal64,TObject)
AdvRTTIDecls(TLocCardinal,TCardinal)
AdvRTTIDecls(TLocCardinal64,TCardinal64)



// ---------------------------------------------------------------------------
//   CLASS: TCardinal
//  PREFIX: c
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCardinal: Public, static methods
// ---------------------------------------------------------------------------
TCardinal& TCardinal::Nul_TCardinal() noexcept
{
    static TCardinal cNull;
    return cNull;
}


// ---------------------------------------------------------------------------
//  TCardinal: Constructors and Destructor
// ---------------------------------------------------------------------------
TCardinal::TCardinal(const tCIDLib::TCard1 c1Val)  noexcept:

    m_c4Val(c1Val)
{
}

TCardinal::TCardinal(const tCIDLib::TCard2 c2Val)  noexcept:

    m_c4Val(c2Val)
{
}

TCardinal::TCardinal(const tCIDLib::TCard4 c4Val)  noexcept:

    m_c4Val(c4Val)
{
}

TCardinal::TCardinal(const tCIDLib::TUInt uVal)  noexcept:

    m_c4Val(uVal)
{
}

TCardinal::TCardinal(const tCIDLib::TSInt iVal)  noexcept:

    m_c4Val(tCIDLib::TCard4(iVal))
{
}

TCardinal::TCardinal(   const   tCIDLib::TCard4     c4Val
                        , const tCIDLib::ERadices   eRadix)  noexcept:
    m_c4Val(c4Val)
    , m_eRadix(eRadix)
{
}


// ---------------------------------------------------------------------------
//  TCardinal: Public operators
// ---------------------------------------------------------------------------
TCardinal& TCardinal::operator=(const tCIDLib::TCard4 c4Src)
{
    m_c4Val = c4Src;
    return *this;
}

tCIDLib::TBoolean TCardinal::operator==(const tCIDLib::TCard4 c4ToTest) const
{
    return (c4ToTest == m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator!=(const tCIDLib::TCard4 c4ToTest) const
{
    return (c4ToTest != m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator<(const TCardinal& cComp) const
{
    return (m_c4Val < cComp.m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator<=(const TCardinal& cComp) const
{
    return (m_c4Val <= cComp.m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator>(const TCardinal& cComp) const
{
    return (m_c4Val > cComp.m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator>=(const TCardinal& cComp) const
{
    return (m_c4Val >= cComp.m_c4Val);
}

tCIDLib::TBoolean TCardinal::operator<(const tCIDLib::TCard4 c4ToTest) const
{
    return (m_c4Val < c4ToTest);
}

tCIDLib::TBoolean TCardinal::operator<=(const tCIDLib::TCard4 c4ToTest) const
{
    return (m_c4Val <= c4ToTest);
}

tCIDLib::TBoolean TCardinal::operator>(const tCIDLib::TCard4 c4ToTest) const
{
    return (m_c4Val > c4ToTest);
}

tCIDLib::TBoolean TCardinal::operator>=(const tCIDLib::TCard4 c4ToTest) const
{
    return (m_c4Val >= c4ToTest);
}

TCardinal& TCardinal::operator+=(const TCardinal& cSrc)
{
    m_c4Val += cSrc.m_c4Val;
    return *this;
}

TCardinal& TCardinal::operator-=(const TCardinal& cSrc)
{
    m_c4Val -= cSrc.m_c4Val;
    return *this;
}

TCardinal& TCardinal::operator+=(const tCIDLib::TCard4 c4Src)
{
    m_c4Val += c4Src;
    return *this;
}

TCardinal& TCardinal::operator-=(const tCIDLib::TCard4 c4Src)
{
    m_c4Val -= c4Src;
    return *this;
}


// ---------------------------------------------------------------------------
//  TCardinal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCardinal::c4Val(const tCIDLib::TCard4 c4NewVal) noexcept
{
    m_c4Val = c4NewVal;
    return m_c4Val;
}

tCIDLib::ERadices TCardinal::eRadix(const tCIDLib::ERadices eNewRadix) noexcept
{
    m_eRadix = eNewRadix;
    return m_eRadix;
}


// ---------------------------------------------------------------------------
//  TCardinal: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCardinal::FormatTo(TTextOutStream& strmToWriteTo) const
{
    tCIDLib::TZStr64    szTmp;
    if (!TRawStr::bFormatVal(m_c4Val, szTmp, tCIDLib::c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}


tCIDLib::TVoid TCardinal::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_c4Val
                    >> m_eRadix;
}

tCIDLib::TVoid TCardinal::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_c4Val
                  << m_eRadix;
}




// ---------------------------------------------------------------------------
//   CLASS: TCardinal64
//  PREFIX: c
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCardinal64: Public, static methods
// ---------------------------------------------------------------------------
TCardinal64& TCardinal64::Nul_TCardinal64() noexcept
{
    static TCardinal64 cNull;
    return cNull;
}


// ---------------------------------------------------------------------------
//  TCardinal64: Constructors and Destructor
// ---------------------------------------------------------------------------
TCardinal64::TCardinal64(const  tCIDLib::TCard8&    c8Val
                        , const tCIDLib::ERadices   eRadix) noexcept :
    m_c8Val(c8Val)
    , m_eRadix(eRadix)
{
}


// ---------------------------------------------------------------------------
//  TCardinal64: Public operators
// ---------------------------------------------------------------------------
TCardinal64& TCardinal64::operator=(const tCIDLib::TCard8& c8ToAssign)
{
    m_c8Val = c8ToAssign;
    return *this;
}

tCIDLib::TBoolean TCardinal64::operator==(const tCIDLib::TCard8& c8ToTest) const
{
    return (c8ToTest == m_c8Val);
}

tCIDLib::TBoolean TCardinal64::operator!=(const tCIDLib::TCard8& c8ToTest) const
{
    return (c8ToTest != m_c8Val);
}

tCIDLib::TBoolean TCardinal64::operator<(const TCardinal64& cComp) const
{
    return (m_c8Val < cComp.m_c8Val);
}

tCIDLib::TBoolean TCardinal64::operator<=(const TCardinal64& cComp) const
{
    return (m_c8Val <= cComp.m_c8Val);
}

tCIDLib::TBoolean TCardinal64::operator>(const TCardinal64& cComp) const
{
    return (m_c8Val > cComp.m_c8Val);
}

tCIDLib::TBoolean TCardinal64::operator>=(const TCardinal64& cComp) const
{
    return (m_c8Val >= cComp.m_c8Val);
}

tCIDLib::TBoolean
TCardinal64::operator<(const tCIDLib::TCard8& c8ToTest) const
{
    return (m_c8Val < c8ToTest);
}

tCIDLib::TBoolean TCardinal64::operator<=(const tCIDLib::TCard8& c8ToTest) const
{
    return (m_c8Val <= c8ToTest);
}

tCIDLib::TBoolean TCardinal64::operator>(const tCIDLib::TCard8& c8ToTest) const
{
    return (m_c8Val > c8ToTest);
}

tCIDLib::TBoolean TCardinal64::operator>=(const tCIDLib::TCard8& c8ToTest) const
{
    return (m_c8Val >= c8ToTest);
}

TCardinal64& TCardinal64::operator+=(const TCardinal64& cSrc)
{
    m_c8Val += cSrc.m_c8Val;
    return *this;
}

TCardinal64& TCardinal64::operator-=(const TCardinal64& cSrc)
{
    m_c8Val -= cSrc.m_c8Val;
    return *this;
}

TCardinal64& TCardinal64::operator+=(const tCIDLib::TCard8 c8Src)
{
    m_c8Val += c8Src;
    return *this;
}

TCardinal64& TCardinal64::operator-=(const tCIDLib::TCard8 c8Src)
{
    m_c8Val -= c8Src;
    return *this;
}



// ---------------------------------------------------------------------------
//  TCardinal64: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TCardinal64::c8Val(const tCIDLib::TCard8& c8NewVal)
{
    m_c8Val = c8NewVal;
    return m_c8Val;
}

tCIDLib::ERadices TCardinal64::eRadix() const
{
    return m_eRadix;
}

tCIDLib::ERadices TCardinal64::eRadix(const tCIDLib::ERadices eNewRadix)
{
    m_eRadix = eNewRadix;
    return m_eRadix;
}


// ---------------------------------------------------------------------------
//  TCardinal64: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCardinal64::FormatTo(TTextOutStream& strmToWriteTo) const
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(m_c8Val, szTmp, tCIDLib::c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}


tCIDLib::TVoid TCardinal64::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_c8Val
                    >> m_eRadix;
}

tCIDLib::TVoid TCardinal64::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c8Val
                    << m_eRadix;
}




// ---------------------------------------------------------------------------
//   CLASS: TLocCardinal
//  PREFIX: c
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocCardinal: Constructor and Destructor
// ---------------------------------------------------------------------------
TLocCardinal::TLocCardinal(const tCIDLib::TCard4 c4Val)  noexcept :

    TCardinal(c4Val, tCIDLib::ERadices::Dec)
{
}


// ---------------------------------------------------------------------------
//  TLocCardinal: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLocCardinal::operator==(const TLocCardinal& cToTest) const
{
    if (this == &cToTest)
        return kCIDLib::True;

    return TParent::operator==(cToTest);
}

tCIDLib::TBoolean TLocCardinal::operator!=(const TLocCardinal& cToTest) const
{
    return !TParent::operator==(cToTest);
}

tCIDLib::TBoolean TLocCardinal::operator<(const TLocCardinal& cComp) const
{
    return TParent::operator<(cComp);
}

tCIDLib::TBoolean TLocCardinal::operator<=(const TLocCardinal& cComp) const
{
    return TParent::operator<=(cComp);
}

tCIDLib::TBoolean TLocCardinal::operator>(const TLocCardinal& cComp) const
{
    return TParent::operator>(cComp);
}

tCIDLib::TBoolean TLocCardinal::operator>=(const TLocCardinal& cComp) const
{
    return TParent::operator>=(cComp);
}


// ---------------------------------------------------------------------------
//  TLocCardinal: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLocCardinal::FormatTo(TTextOutStream& strmToWriteTo) const
{
    tCIDLib::TZStr128  szTmp;
    if (!TRawStr::bFormatVal(c4Val()
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , tCIDLib::ERadices::Dec
                            , TLocale::c4GroupSize()
                            , TLocale::chGroupSeparator()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}




// ---------------------------------------------------------------------------
//   CLASS: TLocCardinal64
//  PREFIX: c
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocCardinal64: Constructor and Destructor
// ---------------------------------------------------------------------------
TLocCardinal64::TLocCardinal64(const tCIDLib::TCard8& c8Val)  noexcept :

    TCardinal64(c8Val, tCIDLib::ERadices::Dec)
{
}


// ---------------------------------------------------------------------------
//  TLocCardinal64: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLocCardinal64::operator==(const TLocCardinal64& cSrc) const
{
    if (this == &cSrc)
        return kCIDLib::True;

    return TParent::operator==(cSrc);
}

tCIDLib::TBoolean TLocCardinal64::operator!=(const TLocCardinal64& cSrc) const
{
    return !TParent::operator==(cSrc);
}

tCIDLib::TBoolean TLocCardinal64::operator<(const TLocCardinal64& cComp) const
{
    return TParent::operator<(cComp);
}

tCIDLib::TBoolean TLocCardinal64::operator<=(const TLocCardinal64& cComp) const
{
    return TParent::operator<=(cComp);
}

tCIDLib::TBoolean TLocCardinal64::operator>(const TLocCardinal64& cComp) const
{
    return TParent::operator>(cComp);
}

tCIDLib::TBoolean TLocCardinal64::operator>=(const TLocCardinal64& cComp) const
{
    return TParent::operator>=(cComp);
}


// ---------------------------------------------------------------------------
//  TLocCardinal64: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLocCardinal64::FormatTo(TTextOutStream& strmToWriteTo) const
{
    tCIDLib::TZStr128  szTmp;
    if (!TRawStr::bFormatVal(c8Val()
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , tCIDLib::ERadices::Dec
                            , TLocale::c4GroupSize()
                            , TLocale::chGroupSeparator()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}

