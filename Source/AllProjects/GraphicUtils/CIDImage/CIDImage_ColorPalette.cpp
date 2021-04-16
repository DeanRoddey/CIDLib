//
// FILE NAME: CIDImage_ColorPalette.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/1994
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
//  This file implements the TClrPalette class, which supports color
//  palettes of arbitary size up to 8K colors.
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
#include    "CIDImage_.hpp"
#include    "CIDImage_RawPalettes_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TClrPalette,TObject)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDImage_ColorPalette
{
    namespace
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TClrPalette
//  PREFIX: pal
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TClrPalette: Public, static methods
// ---------------------------------------------------------------------------
TClrPalette& TClrPalette::Nul_TClrPalette()
{
    static TClrPalette palNull;
    return palNull;
}


// ---------------------------------------------------------------------------
//  TClrPalette: Constructors and Destructor
// ---------------------------------------------------------------------------
TClrPalette::TClrPalette() :

    m_c4ClrCount(0)
    , m_pargbClrs(nullptr)
{
    m_c4ClrCount = 2;
    m_pargbClrs = new TRGBClr[2];
    m_pargbClrs[0] = TRGBClr(0,0,0);
    m_pargbClrs[1] = TRGBClr(255,255,255);
}

TClrPalette::TClrPalette(const tCIDImage::EDefPalettes ePalette) :

    m_c4ClrCount(0)
    , m_pargbClrs(nullptr)
{
    if (ePalette == tCIDImage::EDefPalettes::BlackAndWhite)
    {
        m_c4ClrCount = 2;
        m_pargbClrs = new TRGBClr[2];
        m_pargbClrs[0] = TRGBClr(0,0,0);
        m_pargbClrs[1] = TRGBClr(255,255,255);
    }
     else if (ePalette == tCIDImage::EDefPalettes::Default16)
    {
        m_c4ClrCount = 16;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
        for (tCIDLib::TCard4 c4Val = 0; c4Val < m_c4ClrCount; c4Val++)
            m_pargbClrs[c4Val] = CIDImage_RawPalettes::argbDefault16[c4Val];
    }
     else if (ePalette == tCIDImage::EDefPalettes::GrayScale16)
    {
        m_c4ClrCount = 16;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
        for (tCIDLib::TCard1 c1Val = 0; c1Val < m_c4ClrCount; c1Val++)
            m_pargbClrs[c1Val] = TRGBClr(c1Val, c1Val, c1Val);
    }
     else if (ePalette == tCIDImage::EDefPalettes::Default256)
    {
        m_c4ClrCount = 256;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
        for (tCIDLib::TCard4 c4Val = 0; c4Val < m_c4ClrCount; c4Val++)
            m_pargbClrs[c4Val] = CIDImage_RawPalettes::argbDefault256[c4Val];
    }
     else if (ePalette == tCIDImage::EDefPalettes::GrayScale256)
    {
        m_c4ClrCount = 256;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
        for (tCIDLib::TCard4 c4Val = 0; c4Val < m_c4ClrCount; c4Val++)
            m_pargbClrs[c4Val] = CIDImage_RawPalettes::argbGrayScale[c4Val];
    }
     else if (ePalette == tCIDImage::EDefPalettes::LinearGray)
    {
        m_c4ClrCount = 256;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
        for (tCIDLib::TCard4 c4Val = 0; c4Val < m_c4ClrCount; c4Val++)
        {
            const tCIDLib::TCard1 c1Cur = tCIDLib::TCard1(c4Val);
            m_pargbClrs[c4Val].Set(c1Cur, c1Cur, c1Cur);
        }
    }
    #if CID_DEBUG_ON
     else
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPal_Type
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(ePalette))
        );
    }
    #endif
}

TClrPalette::TClrPalette(const tCIDLib::TCard4 c4ClrCnt) :

    m_c4ClrCount(c4ClrCnt)
    , m_pargbClrs(nullptr)
{
    // The count cannot be 0
    if (!m_c4ClrCount)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPal_ZeroElems
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    m_pargbClrs = new TRGBClr[c4ClrCnt];
}

TClrPalette::TClrPalette(   const   tCIDLib::TCard4         c4ClrCnt
                            ,       TColCursor<TRGBClr>&    cursInitList) :
    m_c4ClrCount(c4ClrCnt)
    , m_pargbClrs(nullptr)
{
    // Cannot be zero
    if (!c4ClrCnt)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPal_ZeroElems
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    m_pargbClrs = new TRGBClr[c4ClrCnt];

    // If not enough colors in list, then an error
    if (m_c4ClrCount > cursInitList.c4ElemCount())
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPal_InitCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(cursInitList.c4ElemCount())
            , TCardinal(m_c4ClrCount)
        );
    }

    // Iterate the container and init the array
    tCIDLib::TCard4 c4Ind   = 0;
    for (; cursInitList; ++cursInitList)
        m_pargbClrs[c4Ind++] = *cursInitList;
}

TClrPalette::TClrPalette(const TClrPalette& palSrc) :

    m_c4ClrCount(palSrc.m_c4ClrCount)
    , m_pargbClrs(new TRGBClr[palSrc.m_c4ClrCount])
{
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
        m_pargbClrs[c4Ind] = palSrc.m_pargbClrs[c4Ind];
}

TClrPalette::TClrPalette(TClrPalette&& palSrc) :

    m_c4ClrCount(2)
    , m_pargbClrs(new TRGBClr[2])
{
    m_pargbClrs[0] = TRGBClr(0,0,0);
    m_pargbClrs[1] = TRGBClr(255,255,255);

    tCIDLib::Swap(m_c4ClrCount, palSrc.m_c4ClrCount);
    tCIDLib::Swap(m_pargbClrs, palSrc.m_pargbClrs);
}

TClrPalette::~TClrPalette()
{
    delete [] m_pargbClrs;
    m_pargbClrs = nullptr;
}


// ---------------------------------------------------------------------------
//  TClrPalette: Public operators
// ---------------------------------------------------------------------------
TClrPalette& TClrPalette::operator=(const TClrPalette& palSrc)
{
    if (this == &palSrc)
        return *this;

    // If the source is a different size, then reallocate
    if (m_c4ClrCount != palSrc.m_c4ClrCount)
    {
        // Free the current color list
        delete [] m_pargbClrs;
        m_pargbClrs = 0;

        // Allocate the new list
        m_c4ClrCount = palSrc.m_c4ClrCount;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
    }

    // Loop though and copy the colors
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
        m_pargbClrs[c4Ind] = palSrc.m_pargbClrs[c4Ind];

    return *this;
}


TClrPalette& TClrPalette::operator=(TClrPalette&& palSrc)
{
    if (&palSrc != this)
    {
        tCIDLib::Swap(m_c4ClrCount, palSrc.m_c4ClrCount);
        tCIDLib::Swap(m_pargbClrs, palSrc.m_pargbClrs);
    }
    return *this;
}


tCIDLib::TBoolean TClrPalette::operator==(const TClrPalette& palToTest) const
{
    if (this == &palToTest)
        return kCIDLib::True;

    if (m_c4ClrCount != palToTest.m_c4ClrCount)
        return kCIDLib::False;

    // Look through the colors
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
    {
        if (m_pargbClrs[c4Ind] != palToTest.m_pargbClrs[c4Ind])
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TClrPalette::operator!=(const TClrPalette& palToTest) const
{
    return !operator==(palToTest);
}

const TRGBClr& TClrPalette::operator[](const tCIDLib::TCard4 c4Index) const
{
    return rgbAt(c4Index);
}

TRGBClr& TClrPalette::operator[](const tCIDLib::TCard4 c4Index)
{
    return rgbAt(c4Index);
}


// ---------------------------------------------------------------------------
//  TClrPalette: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TClrPalette::c4BuildGrayScale(const tCIDImage::EBitDepths eDepth)
{
    //
    //  Calculate how many colors. If the bit depth is beyond 8 bit, just
    //  go with 8 bit since it's a 16 bit gray scale and it will be down
    //  converted to 8 bit in the end anyway.
    //
    tCIDLib::TCard4 c4Clrs;
    if (eDepth > tCIDImage::EBitDepths::Eight)
        c4Clrs = 256;
    else
        c4Clrs = 0x1UL << tCIDLib::c4EnumOrd(eDepth);

    // Calculate an increment to run up the value on each round
    const tCIDLib::TCard1 c1Inc = tCIDLib::TCard1
    (
        255 / (c4Clrs - 1)
    );

    // Resize if needed
    ResetSize(c4Clrs);

    tCIDLib::TCard1 c1Cur = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Clrs; c4Index++)
    {
        m_pargbClrs[c4Index].Set(c1Cur, c1Cur, c1Cur);
        c1Cur += c1Inc;
    }
    return c4Clrs;
}


tCIDLib::TCard4 TClrPalette::c4ClrCount() const
{
    return m_c4ClrCount;
}


tCIDLib::TCard4 TClrPalette::c4FindColor(const TRGBClr& rgbFind) const
{
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
    {
        TRGBClr& rgbCur = m_pargbClrs[c4Ind];

        if (rgbCur == rgbFind)
            return c4Ind;
    }
    return kCIDLib::c4MaxCard;
}


tCIDLib::TCard4
TClrPalette::c4ParseFromText(   const   TString&                strFile
                                , const tCIDImage::EPalTxtFmts  eFormat
                                ,       TTextConverter* const   ptcvtToAdopt)
{
    //
    //  Create a text stream based on a file, and open it for exclusive
    //  read/write. Fail if it does not exist.
    //
    TTextFileInStream strmSource
    (
        strFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
        , ptcvtToAdopt
    );

    //
    //  Create a stream lexer for the file. We tell it not to adopt since
    //  the stream is a stack based object!
    //
    TStreamLexer lexrSource(&strmSource, tCIDLib::EAdoptOpts::NoAdopt);

    // And call the other version
    return c4ParseFromText(lexrSource, eFormat);
}

tCIDLib::TCard4
TClrPalette::c4ParseFromText(       TStreamLexer&           lexrSrc
                            , const tCIDImage::EPalTxtFmts  eFormat)
{
    tCIDLib::TCard4 c4Ind;
    TString strSeparators(kCIDLib::szWhitespace, 16);

    // Add to the correct list of separators if needed
    if ((eFormat == tCIDImage::EPalTxtFmts::CommaDelimited)
    ||  (eFormat == tCIDImage::EPalTxtFmts::CommaDelimited_OnePer))
    {
        strSeparators.Append(kCIDLib::chComma);
    }

    TString strTmp(kCIDLib::pszEmptyZStr, 64);
    for (c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
    {
        //
        //  Get the next value, which should be the red component. Try to
        //  convert it to a TCard1. If this fails, then it will log an
        //  error. Do the same for the green and blue. Check for nothing
        //  coming back on the red, which is legal and means we hit the
        //  end.
        //
        lexrSrc.GetNextToken(strSeparators, strTmp);
        if (!strTmp.c4Length())
            break;
        m_pargbClrs[c4Ind].c1Red(tCIDLib::TCard1(strTmp.c4Val()));

        lexrSrc.GetNextToken(strSeparators, strTmp);
        if (!strTmp.c4Length())
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcPal_TextFormat
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(lexrSrc.c4CurLine())
                , facCIDImage().strMsg(kImgMsgs::midCPal_GreenComp)
            );
        }
        m_pargbClrs[c4Ind].c1Green(tCIDLib::TCard1(strTmp.c4Val()));

        lexrSrc.GetNextToken(strSeparators, strTmp);
        if (!strTmp.c4Length())
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcPal_TextFormat
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(lexrSrc.c4CurLine())
                , facCIDImage().strMsg(kImgMsgs::midCPal_BlueComp)
            );
        }
        m_pargbClrs[c4Ind].c1Blue(tCIDLib::TCard1(strTmp.c4Val()));

        // If its a one-per type format, then flush the rest of this line
        if ((eFormat == tCIDImage::EPalTxtFmts::CommaDelimited_OnePer)
        ||  (eFormat == tCIDImage::EPalTxtFmts::SpaceDelimited_OnePer))
        {
            lexrSrc.FlushLine();
        }
    }
    return c4Ind;
}


tCIDLib::TVoid
TClrPalette::CopyNFrom( const   TClrPalette&    palSrc
                        , const tCIDLib::TCard4 c4CopyUpTo)
{
    //
    //  Copy up to the indicated count, or the count of colors available,
    //  or the count that we can accept, whichever is smaller.
    //
    tCIDLib::TCard4 c4Count = tCIDLib::MinVal(palSrc.c4ClrCount(), c4CopyUpTo);
    c4Count = tCIDLib::MinVal(c4Count, m_c4ClrCount);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_pargbClrs[c4Index] = palSrc[c4Index];
}


tCIDLib::TVoid TClrPalette::CvtToGrayScale()
{
    tCIDLib::TCard1  c1New;
    tCIDLib::TFloat4 f4Tmp;
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
    {
        TRGBClr& rgbCur = m_pargbClrs[c4Ind];

        // Convert this color to a gray scale
        f4Tmp = (rgbCur.c1Red() * 0.299F)
                + (rgbCur.c1Green() * 0.587F)
                + (rgbCur.c1Blue() * 0.114F);

        // Clip it back to the limit
        if (f4Tmp > 255.0)
            c1New = 255;
        else
            c1New = tCIDLib::TCard1(f4Tmp);

        // Now store this back as all three components
        rgbCur.Set(c1New, c1New, c1New);
    }
}


const TRGBClr& TClrPalette::rgbAt(const tCIDLib::TCard4 c4Ind) const
{
    if (c4Ind >= m_c4ClrCount)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Ind)
            , clsThis()
            , TCardinal(m_c4ClrCount)
        );
    }
    return m_pargbClrs[c4Ind];
}

TRGBClr& TClrPalette::rgbAt(const tCIDLib::TCard4 c4Ind)
{
    if (c4Ind >= m_c4ClrCount)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Ind)
            , clsThis()
            , TCardinal(m_c4ClrCount)
        );
    }
    return m_pargbClrs[c4Ind];
}


tCIDLib::TVoid TClrPalette::ResetSize(const tCIDLib::TCard4 c4NewSize)
{
    // Make sure that there is something to actually do
    if (c4NewSize != m_c4ClrCount)
    {
        delete [] m_pargbClrs;
        m_pargbClrs = 0;

        m_c4ClrCount = c4NewSize;
        m_pargbClrs = new TRGBClr[c4NewSize];
    }
     else
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ClrCount; c4Index++)
            m_pargbClrs[c4Index].ToBlack();
    }
}


tCIDLib::TVoid TClrPalette::RotateBackward(const tCIDLib::TCard4 c4Count)
{
    // Check for a count same as palette or 0. If so, nothing to do
    if ((c4Count == m_c4ClrCount) || !c4Count)
        return;

    // Check for a count larger than palette size. If so, do modulo
    tCIDLib::TCard4 c4ActualCnt = c4Count;
    if (c4ActualCnt > m_c4ClrCount)
        c4ActualCnt = (c4ActualCnt % m_c4ClrCount);

    // Allocate a new buffer
    TRGBClr* const pargbTmp = new TRGBClr[m_c4ClrCount];

    try
    {
        //
        //  Start filling in the new buffer at the 0th element, from the
        //  c4ActualCnt'th element in the current buffer. Go to the end of the
        //  current array.
        //
        tCIDLib::TCard4 c4NewInd = 0;
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = c4ActualCnt; c4Ind < m_c4ClrCount; c4Ind++)
            pargbTmp[c4NewInd++] = m_pargbClrs[c4ActualCnt];

        //
        //  Now copy from the old array at the 0'th element into the new
        //  array where the c4NewInd left off.
        //
        for (c4Ind = 0; c4Ind < c4ActualCnt; c4Ind++)
            pargbTmp[c4NewInd++] = m_pargbClrs[c4Ind];
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        delete [] pargbTmp;
        throw;
    }

    // Delete the old array and keep the new one
    delete [] m_pargbClrs;
    m_pargbClrs = pargbTmp;
}

tCIDLib::TVoid TClrPalette::RotateForward(const tCIDLib::TCard4 c4Count)
{
    // Check for a count same as palette or 0. If so, nothing to do
    if ((c4Count == m_c4ClrCount) || !c4Count)
        return;

    // Check for a count larger than palette size. If so, do modulo
    tCIDLib::TCard4 c4ActualCnt = c4Count;
    if (c4ActualCnt > m_c4ClrCount)
        c4ActualCnt = (c4ActualCnt % m_c4ClrCount);

    // Allocate a new buffer
    TRGBClr* const pargbTmp = new TRGBClr[m_c4ClrCount];

    try
    {
        //
        //  Start filling in the new buffer at the c4ActualCnt'th element,
        //  from the 0'th element in the current buffer. Go to the end of
        //  the current array.
        //
        tCIDLib::TCard4 c4OldInd = 0;
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = c4ActualCnt; c4Ind < m_c4ClrCount; c4Ind++)
            pargbTmp[c4Ind] = m_pargbClrs[c4OldInd++];

        //
        //  Now start copying from the c4OldInd'th element of the old array to
        //  the c4Ind'th element of the new array.
        //
        for (c4Ind = 0; c4Ind < c4ActualCnt; c4Ind++)
            pargbTmp[c4Ind] = m_pargbClrs[c4OldInd++];
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        delete [] pargbTmp;
        throw;
    }

    // Delete the old array and keep the new one
    delete [] m_pargbClrs;
    m_pargbClrs = pargbTmp;
}


tCIDLib::TVoid TClrPalette::SetAt(  const   tCIDLib::TCard4 c4At
                                    , const tCIDLib::TCard1 c1Red
                                    , const tCIDLib::TCard1 c1Green
                                    , const tCIDLib::TCard1 c1Blue)
{
    if (c4At >= m_c4ClrCount)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4At)
            , clsThis()
            , TCardinal(m_c4ClrCount)
        );
    }
    m_pargbClrs[c4At].Set(c1Red, c1Green, c1Blue, 0);
}

tCIDLib::TVoid TClrPalette::SetAt(  const   tCIDLib::TCard4 c4At
                                    , const TRGBClr&        rgbToSet)
{
    if (c4At >= m_c4ClrCount)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4At)
            , clsThis()
            , TCardinal(m_c4ClrCount)
        );
    }
    m_pargbClrs[c4At] = rgbToSet;
}



// ---------------------------------------------------------------------------
//  TClrPalette: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TClrPalette::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDImage_ColorPalette::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Get the color count, into a temp so we can test it
    tCIDLib::TCard4  c4SavedCount;
    strmToReadFrom >> c4SavedCount;

    //
    //  If the color count has changed, then delete the current array
    //  and reallocate.
    //
    if (c4SavedCount != m_c4ClrCount)
    {
        // Make sure its not zero
        if (!c4SavedCount)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcPal_ZeroStoredSize
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        // Store the new count and reallocate
        m_c4ClrCount = c4SavedCount;
        delete [] m_pargbClrs;
        m_pargbClrs = new TRGBClr[m_c4ClrCount];
    }

    // Loop through the colors
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
        strmToReadFrom >> m_pargbClrs[c4Ind];

    // And check for the end of object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TClrPalette::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Stream out our start object marker, and the color count. We also
    //  stream out a format version to support auto upgrade of the data
    //  later.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDImage_ColorPalette::c2FmtVersion
                    << m_c4ClrCount;

    // Loop through the colors and write them
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ClrCount; c4Ind++)
        strmToWriteTo << m_pargbClrs[c4Ind];

    // And end it up with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


