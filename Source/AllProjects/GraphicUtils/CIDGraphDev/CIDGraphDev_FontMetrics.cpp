//
// FILE NAME: CIDWnd_FontMetrics.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1997
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
//  This file implements a simple font metrics class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFontMetrics,TObject)


// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------
struct TEnumData
{
    TCollection<TFontMetrics>*  pcolTarget;
    tCIDGraphDev::EFontTypes    eType;
    tCIDGraphDev::EFontBases    eBase;
    tCIDGraphDev::EFontPitches  ePitch;
};


// ---------------------------------------------------------------------------
//  Local functions
//
//  Because of the moronic callback scheme used by MS for enum functions,
//  we have to have this callback function.
// ---------------------------------------------------------------------------
static int CALLBACK EnumFontsProc(  ENUMLOGFONTEXW*     pLogInfo
                                    , NEWTEXTMETRICEXW* pPhysInfo
                                    , DWORD             c4Type
                                    , LPARAM            i4Param)
{
    // Look at the parameter as what it really is
    TEnumData* pData = reinterpret_cast<TEnumData*>(i4Param);

    //
    //  Create a new font metrics object from the info we got, if it matches
    //  the filter criteria.
    //
    const tCIDLib::TCard1 c1Flags = pPhysInfo->ntmTm.tmPitchAndFamily;
    if (pData->eType != tCIDGraphDev::EFontTypes::Either)
    {
        tCIDGraphDev::EFontTypes eActual;
        if (c4Type & RASTER_FONTTYPE)
            eActual = tCIDGraphDev::EFontTypes::Raster;
        else if (c4Type & TRUETYPE_FONTTYPE)
            eActual = tCIDGraphDev::EFontTypes::Scalable;
        else
            return 1;

        if (pData->eType != eActual)
            return 1;
    }

    if (pData->eBase != tCIDGraphDev::EFontBases::Either)
    {
        const tCIDGraphDev::EFontBases eActual = (c1Flags & TMPF_DEVICE)
                                            ? tCIDGraphDev::EFontBases::Device
                                            : tCIDGraphDev::EFontBases::Software;
        if (pData->eBase != eActual)
            return 1;
    }

    if (pData->ePitch != tCIDGraphDev::EFontPitches::Either)
    {
        const tCIDGraphDev::EFontPitches eActual = (c1Flags & TMPF_FIXED_PITCH)
                                            ? tCIDGraphDev::EFontPitches::Variable
                                            : tCIDGraphDev::EFontPitches::Fixed;
        if (pData->ePitch != eActual)
            return 1;
    }

    TFontMetrics fmtrCur
    (
        TString(pLogInfo->elfLogFont.lfFaceName)
        , *(TEXTMETRICW*)pPhysInfo
    );

    // And add it to the collection
    pData->pcolTarget->objAdd(fmtrCur);

    return 1;
}


// ---------------------------------------------------------------------------
//   CLASS: TFontMetrics
//  PREFIX: fmtr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFontMetrics: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TFontMetrics::c4EnumFontFace(const  TGraphDrawDev&              gdevTarget
                            ,       TCollection<TFontMetrics>&  colToFill
                            , const TString&                    strFace
                            , const tCIDGraphDev::EFontTypes    eType
                            , const tCIDGraphDev::EFontBases    eBase
                            , const tCIDGraphDev::EFontPitches  ePitch)
{
    // Flush the collection just in case
    colToFill.RemoveAll();

    TEnumData EnumData;
    EnumData.pcolTarget = &colToFill;
    EnumData.eType = eType;
    EnumData.eBase = eBase;
    EnumData.ePitch = ePitch;

    // Enumerate the fonts in this face
    LOGFONT Search = {0};
    strFace.ToZStr(Search.lfFaceName, c4MaxBufChars(Search.lfFaceName));
    ::EnumFontFamiliesExW
    (
        gdevTarget.hdevThis()
        , &Search
        , (FONTENUMPROCW)EnumFontsProc
        , reinterpret_cast<long>(&EnumData)
        , 0
    );
    return colToFill.c4ElemCount();
}

tCIDLib::TCard4
TFontMetrics::c4EnumAllFonts(const  TGraphDrawDev&              gdevTarget
                            ,       TCollection<TFontMetrics>&  colToFill
                            , const tCIDGraphDev::EFontTypes    eType
                            , const tCIDGraphDev::EFontBases    eBase
                            , const tCIDGraphDev::EFontPitches  ePitch)
{
    // Flush the collection just in case
    colToFill.RemoveAll();

    TEnumData EnumData;
    EnumData.pcolTarget = &colToFill;
    EnumData.eType = eType;
    EnumData.eBase = eBase;
    EnumData.ePitch = ePitch;

    // Enumerate the fonts in this face
    LOGFONT Search = {0};
    ::EnumFontFamiliesExW
    (
        gdevTarget.hdevThis()
        , &Search
        , (FONTENUMPROCW)EnumFontsProc
        , reinterpret_cast<long>(&EnumData)
        , 0
    );
    return colToFill.c4ElemCount();
}


TFontMetrics& TFontMetrics::Nul_TFontMetrics()
{
    static TFontMetrics fmtrNull;
    return fmtrNull;
}



// ---------------------------------------------------------------------------
//  TFontMetrics: Public Constructors and destructor
// ---------------------------------------------------------------------------
TFontMetrics::TFontMetrics() :

    m_pHostInfo(nullptr)
{
    m_pHostInfo = new TEXTMETRIC;
    TRawMem::SetMemBuf(m_pHostInfo, tCIDLib::TCard1(0), sizeof(TEXTMETRIC));
}

TFontMetrics::TFontMetrics(const TGraphDrawDev& gdevSource)
{
    // Query the face name
    tCIDLib::TCh szTmp[128];
    if (!::GetTextFaceW(gdevSource.hdevThis(), 128, szTmp))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_GetCurMetrics
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    m_strFaceName = szTmp;

    //
    //  Query the font metrics for the passed device and keep a copy of
    //  it.
    //
    m_pHostInfo = new TEXTMETRIC;
    if (!::GetTextMetricsW(gdevSource.hdevThis(), m_pHostInfo))
    {
        delete m_pHostInfo;

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_GetCurMetrics
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

TFontMetrics::TFontMetrics(const TFontMetrics& fmtrToCopy) :

    m_strFaceName(fmtrToCopy.m_strFaceName)
{
    // Dup the passed object's host info
    m_pHostInfo = new TEXTMETRIC;
    *m_pHostInfo = *fmtrToCopy.m_pHostInfo;
}

TFontMetrics::~TFontMetrics()
{
    // Clean up the host info structure if it got allocated
    delete m_pHostInfo;
    m_pHostInfo = nullptr;
}


// ---------------------------------------------------------------------------
//  TFontMetrics: Public Constructors and destructor
// ---------------------------------------------------------------------------
TFontMetrics& TFontMetrics::operator=(const TFontMetrics& fmtrToAssign)
{
    if (this == &fmtrToAssign)
        return *this;

    m_strFaceName   = fmtrToAssign.m_strFaceName;
    *m_pHostInfo = *fmtrToAssign.m_pHostInfo;

    return *this;
}


tCIDLib::TBoolean
TFontMetrics::operator==(const TFontMetrics& fmtrToCompare) const
{
    if (this == &fmtrToCompare)
        return kCIDLib::True;

    if (fmtrToCompare.m_strFaceName != m_strFaceName)
        return kCIDLib::False;

    if (TRawMem::bCompareMemBuf(m_pHostInfo, fmtrToCompare.m_pHostInfo, sizeof(TEXTMETRIC)))
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TFontMetrics::operator!=(const TFontMetrics& fmtrToAssign) const
{
    return !operator==(fmtrToAssign);
}


// ---------------------------------------------------------------------------
//  TFontMetrics: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFontMetrics::bDeviceFont() const
{
    return (m_pHostInfo->tmPitchAndFamily & TMPF_DEVICE) != 0;
}

tCIDLib::TBoolean TFontMetrics::bDeviceFont(const tCIDLib::TBoolean bState)
{
    if (bState)
        m_pHostInfo->tmPitchAndFamily |= TMPF_DEVICE;
    else
        m_pHostInfo->tmPitchAndFamily &= ~TMPF_DEVICE;
    return bState;
}

tCIDLib::TBoolean TFontMetrics::bFixedPitch() const
{
    //
    //  Notice the reversed sense of this Win32 flag. if the fixed pitch flag
    //  is not on, then it is a fixed pitch font!!
    //
    return (m_pHostInfo->tmPitchAndFamily & TMPF_FIXED_PITCH) == 0;
}

tCIDLib::TBoolean TFontMetrics::bFixedPitch(const tCIDLib::TBoolean bState)
{
    //
    //  Notice the reversed sense of this Win32 flag. if the fixed pitch flag
    //  is not on, then it is a fixed pitch font!!
    //
    if (bState)
        m_pHostInfo->tmPitchAndFamily &= TMPF_FIXED_PITCH;
    else
        m_pHostInfo->tmPitchAndFamily |= TMPF_FIXED_PITCH;
    return bState;
}

tCIDLib::TBoolean TFontMetrics::bItalic() const
{
    return (m_pHostInfo->tmItalic != 0);
}

tCIDLib::TBoolean TFontMetrics::bItalic(const tCIDLib::TBoolean bState)
{
    if (bState)
        m_pHostInfo->tmItalic = 1;
    else
        m_pHostInfo->tmItalic = 0;
    return bState;
}

tCIDLib::TBoolean TFontMetrics::bScalable() const
{
    return (m_pHostInfo->tmPitchAndFamily & (TMPF_VECTOR | TMPF_TRUETYPE)) != 0;
}

tCIDLib::TBoolean TFontMetrics::bScalable(const tCIDLib::TBoolean bState)
{
    if (bState)
        m_pHostInfo->tmPitchAndFamily |= (TMPF_VECTOR | TMPF_TRUETYPE);
    else
        m_pHostInfo->tmPitchAndFamily &= ~(TMPF_VECTOR | TMPF_TRUETYPE);
    return bState;
}

tCIDLib::TBoolean TFontMetrics::bStruckOut() const
{
    return (m_pHostInfo->tmStruckOut != 0);
}

tCIDLib::TBoolean TFontMetrics::bStruckOut(const tCIDLib::TBoolean bState)
{
    if (bState)
        m_pHostInfo->tmStruckOut = 1;
    else
        m_pHostInfo->tmStruckOut = 0;
    return bState;
}

tCIDLib::TBoolean TFontMetrics::bUnderlined() const
{
    return (m_pHostInfo->tmUnderlined != 0);
}

tCIDLib::TBoolean TFontMetrics::bUnderlined(const tCIDLib::TBoolean bState)
{
    if (bState)
        m_pHostInfo->tmUnderlined = 1;
    else
        m_pHostInfo->tmUnderlined = 0;
    return bState;
}

tCIDLib::TCard4 TFontMetrics::c4Ascender() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmAscent);
}

tCIDLib::TCard4 TFontMetrics::c4Ascender(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmAscent = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDLib::TCard4 TFontMetrics::c4AverageWidth() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmAveCharWidth);
}

tCIDLib::TCard4 TFontMetrics::c4AverageWidth(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmAveCharWidth = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDLib::TCard4 TFontMetrics::c4Descender() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmDescent);
}

tCIDLib::TCard4 TFontMetrics::c4Descender(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmDescent = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDLib::TCard4 TFontMetrics::c4ExternalLeading() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmExternalLeading);
}

tCIDLib::TCard4 TFontMetrics::c4ExternalLeading(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmExternalLeading = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDLib::TCard4 TFontMetrics::c4Height() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmHeight);
}

tCIDLib::TCard4 TFontMetrics::c4InternalLeading() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmInternalLeading);
}

tCIDLib::TCard4 TFontMetrics::c4InternalLeading(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmInternalLeading = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDLib::TCard4 TFontMetrics::c4MaxWidth() const
{
    return tCIDLib::TCard4(m_pHostInfo->tmMaxCharWidth);
}

tCIDLib::TCard4 TFontMetrics::c4MaxWidth(const tCIDLib::TCard4 c4New)
{
    m_pHostInfo->tmMaxCharWidth = tCIDLib::TInt4(c4New);
    return c4New;
}

tCIDGraphDev::EFontWeights TFontMetrics::eWeight() const
{
    if (!m_pHostInfo->tmWeight)
        return tCIDGraphDev::EFontWeights::DontCare;

    // Sanity check. If its > 999, then just return max weight
    if (m_pHostInfo->tmWeight > 999)
        return tCIDGraphDev::EFontWeights::Max;

    //
    //  Our enums are 0 to 9 and map to the weight values of 0 to 999. So
    //  we just take the weight, add 99 and divide by 100.
    //
    return tCIDGraphDev::EFontWeights((m_pHostInfo->tmWeight + 99) / 100);
}

tCIDGraphDev::EFontWeights
TFontMetrics::eWeight(const tCIDGraphDev::EFontWeights eWeight)
{
    m_pHostInfo->tmWeight = int(eWeight) * 100;
    return eWeight;
}

TSize TFontMetrics::szDevRes() const
{
    return TSize
    (
        m_pHostInfo->tmDigitizedAspectX
        , m_pHostInfo->tmDigitizedAspectY
    );
}


const TString& TFontMetrics::strFaceName() const
{
    return m_strFaceName;
}

const TString& TFontMetrics::strFaceName(const TString& strNewName)
{
    m_strFaceName = strNewName;
    return m_strFaceName;
}


TSize TFontMetrics::szDevRes(const TSize& szNew)
{
    m_pHostInfo->tmDigitizedAspectX = tCIDLib::TInt4(szNew.c4Width());
    m_pHostInfo->tmDigitizedAspectY = tCIDLib::TInt4(szNew.c4Height());
    return szNew;
}


tCIDLib::TVoid TFontMetrics::UpdateFrom(const TGraphDrawDev& gdevSource)
{
    //
    //  Query the font metrics for the passed device and keep a copy of
    //  it.
    //
    if (!::GetTextMetricsW(gdevSource.hdevThis(), m_pHostInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_GetCurMetrics
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// ---------------------------------------------------------------------------
//  TFontMetrics: Hidden constructors
// ---------------------------------------------------------------------------
TFontMetrics::TFontMetrics( const   TString&        strFaceName
                            , const tagTEXTMETRICW& HostMetricInfo) :
    m_pHostInfo(nullptr)
    , m_strFaceName(strFaceName)
{
    m_pHostInfo = new tagTEXTMETRICW;
    *m_pHostInfo = HostMetricInfo;
}



// ---------------------------------------------------------------------------
//  TFontMetrics: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFontMetrics::ToLogFont(LOGFONTW& ToFill) const
{
    static LOGFONTW ZeroLogFont = {0};

    // Zero out the structure first
    ToFill = ZeroLogFont;

    // And fill it in using our font info
    ToFill.lfHeight         = m_pHostInfo->tmHeight;
    ToFill.lfWidth          = m_pHostInfo->tmAveCharWidth;
    ToFill.lfWeight         = m_pHostInfo->tmWeight;
    ToFill.lfItalic         = m_pHostInfo->tmItalic;
    ToFill.lfUnderline      = m_pHostInfo->tmUnderlined;
    ToFill.lfStrikeOut      = m_pHostInfo->tmStruckOut;
    ToFill.lfCharSet        = m_pHostInfo->tmCharSet;
    ToFill.lfPitchAndFamily = m_pHostInfo->tmPitchAndFamily;

    // Fill out some that we always do the same
    ToFill.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    ToFill.lfOutPrecision = OUT_TT_PRECIS;
    ToFill.lfQuality = CLEARTYPE_QUALITY;

    // <TBD> Deal with these!!
//    ToFill.lfEscapement     = m_pHostInfo->tm;
//    ToFill.lfOrientation    = m_pHostInfo->tm;
//    ToFill.lfOutPrecision   = m_pHostInfo->tm;

    m_strFaceName.ToZStr(ToFill.lfFaceName, c4MaxBufChars(ToFill.lfFaceName));
}

