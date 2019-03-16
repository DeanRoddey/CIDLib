//
// FILE NAME: CIDLib_Size.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This file implements the TSize class, which is a 2D extent in an
//  integral coordinate space.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TSize,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSize
//  PREFIX: sz
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSize: Public, static methods
// ---------------------------------------------------------------------------
TSize& TSize::Nul_TSize()
{
    static TSize* pszNull = nullptr;
    if (!pszNull)
    {
        TBaseLock lockInit;
        if (!pszNull)
            TRawMem::pExchangePtr(&pszNull, new TSize);
    }
    return *pszNull;
}


// ---------------------------------------------------------------------------
//  TSize: Constructors and Destructor
// ---------------------------------------------------------------------------
TSize::TSize(const tCIDLib::THostSize& szSrc) :

    m_c4CX(szSrc.i4CX)
    , m_c4CY(szSrc.i4CY)
{
}

TSize::TSize(const tCIDLib::TRawSize& szSrc) :

    m_c4CX(szSrc.c4CX)
    , m_c4CY(szSrc.c4CY)
{
}

// ---------------------------------------------------------------------------
//  TSize: Public operators
// ---------------------------------------------------------------------------
TSize::operator const tCIDLib::THostSize&() const
{
    return *((const tCIDLib::THostSize*)&m_c4CX);
}


tCIDLib::TVoid TSize::operator=(const tCIDLib::TRawSize& szSrc)
{
    m_c4CX = szSrc.c4CX;
    m_c4CY = szSrc.c4CY;
    return;
}

tCIDLib::TVoid TSize::operator+=(const TSize& szToAdd)
{
    m_c4CX += szToAdd.m_c4CX;
    m_c4CY += szToAdd.m_c4CY;
}

tCIDLib::TVoid TSize::operator-=(const TSize& szToSub)
{
    m_c4CX -= szToSub.m_c4CX;
    m_c4CY -= szToSub.m_c4CY;
}

TSize operator+(const TSize& sz1, const TSize& sz2)
{
    return TSize(sz1.m_c4CX + sz2.m_c4CX, sz1.m_c4CY + sz2.m_c4CY);
}

TSize operator-(const TSize& sz1, const TSize& sz2)
{
    return TSize(sz1.m_c4CX - sz2.m_c4CX, sz1.m_c4CY - sz2.m_c4CY);
}


tCIDLib::TVoid TSize::operator|=(const TSize& szToOR)
{
    // Take the largest of each field
    if (szToOR.m_c4CX > m_c4CX)
        m_c4CX = szToOR.m_c4CX;

    if (szToOR.m_c4CY > m_c4CY)
        m_c4CY = szToOR.m_c4CY;
}

TSize operator|(const TSize& szOR1, const TSize& szOR2)
{
    // Take the largest of each field
    return TSize
    (
        tCIDLib::MaxVal(szOR1.m_c4CX, szOR2.m_c4CX)
        , tCIDLib::MaxVal(szOR1.m_c4CY, szOR2.m_c4CY)
    );
}

tCIDLib::TVoid TSize::operator&=(const TSize& szToAND)
{
    // Take the smallest of each field
    if (szToAND.m_c4CX < m_c4CX)
        m_c4CX = szToAND.m_c4CX;

    if (szToAND.m_c4CY < m_c4CY)
        m_c4CY = szToAND.m_c4CY;
}

TSize operator&(const TSize& szAND1, const TSize& szAND2)
{
    // Take the smallest of each field
    return TSize
    (
        tCIDLib::MinVal(szAND1.m_c4CX, szAND2.m_c4CX)
        , tCIDLib::MinVal(szAND1.m_c4CY, szAND2.m_c4CY)
    );
}


// ---------------------------------------------------------------------------
//  TSize: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSize::Adjust(const tCIDLib::TInt4 i4XOfs, const tCIDLib::TInt4 i4YOfs)
{
    m_c4CX += i4XOfs;
    m_c4CY += i4YOfs;
}

tCIDLib::TVoid TSize::Adjust(const tCIDLib::TInt4 i4Ofs)
{
    m_c4CX += i4Ofs;
    m_c4CY += i4Ofs;
}


tCIDLib::TVoid TSize::AdjustCX(const tCIDLib::TInt4 i4Ofs)
{
    m_c4CX += i4Ofs;
}

tCIDLib::TVoid TSize::AdjustCY(const tCIDLib::TInt4 i4Ofs)
{
    m_c4CY += i4Ofs;
}


tCIDLib::TBoolean TSize::bEitherLargerThan(const TSize& szThan) const
{
    return (m_c4CX > szThan.m_c4CX) || (m_c4CY > szThan.m_c4CY);
}


//
//  Parses a Size of out a string. It expects the format to be cx,cy but
//  you can indicate another separate character as a parameter, as long
//  as the values are in the right order.
//
tCIDLib::TBoolean
TSize::bParseFromText(  const   TString&            strText
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // We need to pull the separate tokens out from the string
    TString strTmp(chSepChar);
    strTmp.Append(kCIDLib::chSpace);
    TStringTokenizer stokParse(&strText, strTmp);

    tCIDLib::TBoolean bOk;
    tCIDLib::TCard4 c4CX;
    tCIDLib::TCard4 c4CY;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    c4CX = TRawStr::c4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    c4CY = TRawStr::c4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    // Looks ok, so store the values
    m_c4CX = c4CX;
    m_c4CY = c4CY;

    return kCIDLib::True;
}



//
//  Packs both values into a 32 bit value. If either size is too large for
//  16 bits, it throws an error.
//
tCIDLib::TCard4 TSize::c4Packed() const
{
    if ((m_c4CX > kCIDLib::c2MaxCard) || (m_c4CY > kCIDLib::c2MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSize_CantPack
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TRawBits::c4From16(tCIDLib::TCard2(m_c4CX), tCIDLib::TCard2(m_c4CY));
}


// Format to text in a controlled fashin
tCIDLib::TVoid
TSize::FormatToText(        TString&            strToFill
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCh        chSepChar
                    , const tCIDLib::TBoolean   bAppend) const
{
    tCIDLib::ERadices eRealRad = eRadix;
    if (eRealRad == tCIDLib::ERadices::Auto)
        eRealRad = tCIDLib::ERadices::Dec;

    // If not appending, start with a cleared string
    if (!bAppend)
        strToFill.Clear();

    strToFill.AppendFormatted(m_c4CX, eRealRad);
    strToFill.Append(chSepChar);
    strToFill.AppendFormatted(m_c4CY, eRealRad);
}


// Like bParseFromText but this one throws if bad
tCIDLib::TVoid
TSize::ParseFromText(   const   TString&            strText
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // Call the other version and throw if it returns false
    if (!bParseFromText(strText, eRadix, chSepChar))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSize_BadFormat
            , strText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}


//
//  Scale ourself by a single scaling factor or separate x/y factors.
//
tCIDLib::TVoid TSize::Scale(const   tCIDLib::TFloat8&   f8HScale
                            , const tCIDLib::TFloat8&   f8VScale)
{
    m_c4CX = TMathLib::c4Round(f8HScale * m_c4CX);
    m_c4CY = TMathLib::c4Round(f8VScale * m_c4CY);
}

tCIDLib::TVoid TSize::Scale(const tCIDLib::TFloat8& f8Scale)
{
    m_c4CX = TMathLib::c4Round(f8Scale * m_c4CX);
    m_c4CY = TMathLib::c4Round(f8Scale * m_c4CY);
}


//
//  Scale to fit within the passed area, keeping our original aspect ratio
//
tCIDLib::TVoid TSize::ScaleAR(const TSize& szTarArea)
{
    // Calculate the AR of the source (us) and target areas
    const tCIDLib::TFloat8 f8OrgAR
    (
        tCIDLib::TFloat8(m_c4CX) / tCIDLib::TFloat8(m_c4CY)
    );

    const tCIDLib::TFloat8 f8TarAR
    (
        tCIDLib::TFloat8(szTarArea.m_c4CX) / tCIDLib::TFloat8(szTarArea.m_c4CY)
    );

    //
    //  If the target AR is wider, then let the height fit and scale the
    //  width to keep the original AR. Otherwise, do the opposite. If they
    //  are the same, just take the target area as is.
    //
    if (f8TarAR > f8OrgAR)
    {
        m_c4CY = szTarArea.m_c4CY;
        m_c4CX = TMathLib::c4Round(m_c4CY * f8OrgAR);
    }
     else if (f8TarAR < f8OrgAR)
    {
        m_c4CX = szTarArea.m_c4CX;
        m_c4CY = TMathLib::c4Round(m_c4CX / f8OrgAR);
    }
     else
    {
        m_c4CX = szTarArea.m_c4CX;
        m_c4CY = szTarArea.m_c4CY;
    }
}


//
//  Sets a new width and scales the height to keep the original aspect ratio
//
tCIDLib::TVoid TSize::ScaleToWidthAR(const tCIDLib::TCard4 c4NewWidth)
{
    // Calculate the our AR
    const tCIDLib::TFloat8 f8AR
    (
        tCIDLib::TFloat8(m_c4CY) / tCIDLib::TFloat8(m_c4CX)
    );

    // Set the width and scale the height to keep the AR
    m_c4CX = c4NewWidth;
    m_c4CY = TMathLib::c4Round(m_c4CX * f8AR);
}


// Sets a new height and scales the width to keep the original aspect ratio
tCIDLib::TVoid TSize::ScaleToHeightAR(const tCIDLib::TCard4 c4NewHeight)
{
    // Calculate the our AR
    const tCIDLib::TFloat8 f8AR
    (
        tCIDLib::TFloat8(m_c4CX) / tCIDLib::TFloat8(m_c4CY)
    );

    // Set the height and scale the width to keep the AR
    m_c4CY = c4NewHeight;
    m_c4CX = TMathLib::c4Round(m_c4CY * f8AR);
}


// We can give back our size as a packed value, and can take it as one as well
tCIDLib::TVoid TSize::SetFromPacked(const tCIDLib::TCard4 c4ToSet)
{
    m_c4CX = TRawBits::c2Low16From32(c4ToSet);
    m_c4CY = TRawBits::c2High16From32(c4ToSet);
}


//
//  Take the larger or smaller of the X and Y sizes
//
tCIDLib::TVoid TSize::TakeLarger(const TSize& szOther)
{
    m_c4CX = tCIDLib::MaxVal(m_c4CX, szOther.m_c4CX);
    m_c4CY = tCIDLib::MaxVal(m_c4CY, szOther.m_c4CY);
}

tCIDLib::TVoid TSize::TakeSmaller(const TSize& szOther)
{
    m_c4CX = tCIDLib::MinVal(m_c4CX, szOther.m_c4CX);
    m_c4CY = tCIDLib::MinVal(m_c4CY, szOther.m_c4CY);
}


// Convert to host specific point or size, or raw size
tCIDLib::TVoid TSize::ToHostPt(tCIDLib::THostPoint& ptlToFill) const
{
    ptlToFill.i4X = tCIDLib::TInt4(m_c4CX);
    ptlToFill.i4Y = tCIDLib::TInt4(m_c4CY);
}

tCIDLib::TVoid TSize::ToHostSize(tCIDLib::THostSize& szToFill) const
{
    szToFill.i4CX = tCIDLib::TInt4(m_c4CX);
    szToFill.i4CY = tCIDLib::TInt4(m_c4CY);
}

tCIDLib::TVoid TSize::ToRawSize(tCIDLib::TRawSize& szToFill) const
{
    szToFill.c4CX = m_c4CX;
    szToFill.c4CY = m_c4CY;
}


// ---------------------------------------------------------------------------
//  TSize: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSize::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Uses the default sep char that ParseFromText() does
    strmToWriteTo << m_c4CX << kCIDLib::chComma << m_c4CY;
}

tCIDLib::TVoid TSize::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Get the vector value
    strmToReadFrom  >> m_c4CX
                    >> m_c4CY;
}

tCIDLib::TVoid TSize::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Write out the origin and size information
    strmToWriteTo   << m_c4CX
                    << m_c4CY;
}

