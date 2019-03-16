//
// FILE NAME: CIDGraphDev_FontSelAttrs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/02/1998
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
//  This file implements the font selection attributes class, TFontSelAttrs.
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
RTTIDecls(TFontSelAttrs,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TFontSelAttrs
// PREFIX: fsel
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFontSelAttrs: Public, static methods
// ---------------------------------------------------------------------------
tCIDGraphDev::EFontWeights
TFontSelAttrs::eWeightFromName(const tCIDLib::TCh* const pszName)
{
    if (TRawStr::bCompareStrI(pszName, L"Thin"))
        return tCIDGraphDev::EFontWeights::Thin;
    else if (TRawStr::bCompareStrI(pszName, L"ExtraLight"))
        return tCIDGraphDev::EFontWeights::ExtraLight;
    else if (TRawStr::bCompareStrI(pszName, L"Light"))
        return tCIDGraphDev::EFontWeights::Light;
    else if (TRawStr::bCompareStrI(pszName, L"Normal"))
        return tCIDGraphDev::EFontWeights::Normal;
    else if (TRawStr::bCompareStrI(pszName, L"Medium"))
        return tCIDGraphDev::EFontWeights::Medium;
    else if (TRawStr::bCompareStrI(pszName, L"SemiBold"))
        return tCIDGraphDev::EFontWeights::SemiBold;
    else if (TRawStr::bCompareStrI(pszName, L"Bold"))
        return tCIDGraphDev::EFontWeights::Bold;
    else if (TRawStr::bCompareStrI(pszName, L"ExtraBold"))
        return tCIDGraphDev::EFontWeights::ExtraBold;
    else if (TRawStr::bCompareStrI(pszName, L"Heavy"))
        return tCIDGraphDev::EFontWeights::Heavy;
    else
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_UnknownWeight
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TString(pszName)
        );
    }

    // Make the compiler happy
    return tCIDGraphDev::EFontWeights::DontCare;
}

tCIDGraphDev::EFontWeights TFontSelAttrs::eWeightFromName(const TString& strName)
{
    return eWeightFromName(strName.pszBuffer());
}



// ---------------------------------------------------------------------------
//  TFontSelAttrs: Constructors and Destructor
// ---------------------------------------------------------------------------
TFontSelAttrs::TFontSelAttrs() :

    m_pHostInfo(nullptr)
{
    static LOGFONTW ZeroLogFont = {0};

    // Allocate a host info structure and default it in
    m_pHostInfo = new LOGFONTW;
    *m_pHostInfo = ZeroLogFont;
    m_pHostInfo->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    m_pHostInfo->lfOutPrecision = OUT_TT_PRECIS;
    m_pHostInfo->lfQuality = CLEARTYPE_QUALITY;
}

TFontSelAttrs::TFontSelAttrs(const TGraphDrawDev& gdevSource) :

    m_pHostInfo(nullptr)
{
    // Get the font handle from this device
    HGDIOBJ hFontTmp = ::GetCurrentObject(gdevSource.hdevThis(), OBJ_FONT);
    if (!hFontTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_NoneSet
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // And get the logical font data from it
    m_pHostInfo = new LOGFONTW;
    if (::GetObject(hFontTmp, sizeof(LOGFONTW), m_pHostInfo) != sizeof(LOGFONTW))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_GetCurSelAttrs
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

TFontSelAttrs::TFontSelAttrs(const TFontSelAttrs& fselSrc) :

    m_pHostInfo(nullptr)
{
    m_pHostInfo = new LOGFONTW;
    *m_pHostInfo = *fselSrc.m_pHostInfo;
}

TFontSelAttrs::~TFontSelAttrs()
{
    delete m_pHostInfo;
    m_pHostInfo = nullptr;
}


// ---------------------------------------------------------------------------
//  TFontSelAttrs: Public operators
// ---------------------------------------------------------------------------
TFontSelAttrs& TFontSelAttrs::operator=(const TFontSelAttrs& fselSrc)
{
    if (this != &fselSrc)
    {
        *m_pHostInfo = *fselSrc.m_pHostInfo;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TFontSelAttrs: Public operators
// ---------------------------------------------------------------------------
TFontSelAttrs::TFontSelAttrs(const tagLOGFONTW& HostInfo) :

    m_pHostInfo(0)
{
    m_pHostInfo = new LOGFONTW;
    *m_pHostInfo = HostInfo;
}



// ---------------------------------------------------------------------------
//  TFontSelAttrs: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Move the weight up/down to the previous/net standard weight type
tCIDLib::TVoid
TFontSelAttrs::AdjustWeight(const tCIDLib::TBoolean bIncrement)
{
    if (bIncrement)
    {
        m_pHostInfo->lfWeight += 100;
        if (m_pHostInfo->lfWeight > 999)
            m_pHostInfo->lfWeight = long(tCIDGraphDev::EFontWeights::Max);
    }
     else
    {
        if (m_pHostInfo->lfWeight >= 100)
            m_pHostInfo->lfWeight -= 100;
        else
            m_pHostInfo->lfWeight = 0;
    }
}


// Get/set the preference for device fonts
tCIDLib::TBoolean TFontSelAttrs::bDevicePreferred() const
{
    return (m_pHostInfo->lfOutPrecision == OUT_DEVICE_PRECIS);
}

tCIDLib::TBoolean
TFontSelAttrs::bDevicePreferred(const tCIDLib::TBoolean bNewState)
{
    if (bNewState)
        m_pHostInfo->lfOutPrecision = OUT_DEVICE_PRECIS;
    else
        m_pHostInfo->lfOutPrecision = OUT_TT_PRECIS;
    return bNewState;
}


// Get/set the italic attribute
tCIDLib::TBoolean TFontSelAttrs::bItalic() const
{
    return (m_pHostInfo->lfItalic != 0);
}

tCIDLib::TBoolean TFontSelAttrs::bItalic(const tCIDLib::TBoolean bNewState)
{
    if (bNewState)
        m_pHostInfo->lfItalic = 1;
    else
        m_pHostInfo->lfItalic = 0;
    return bNewState;
}


// Get/set the scalable attribute
tCIDLib::TBoolean TFontSelAttrs::bScalable() const
{
    return (m_pHostInfo->lfOutPrecision == OUT_TT_PRECIS);
}

tCIDLib::TBoolean TFontSelAttrs::bScalable(const tCIDLib::TBoolean bNewState)
{
    if (bNewState)
        m_pHostInfo->lfOutPrecision = OUT_TT_PRECIS;
    else
        m_pHostInfo->lfOutPrecision = OUT_DEVICE_PRECIS;
    return bNewState;
}


// Get/set the strikeout attribute
tCIDLib::TBoolean TFontSelAttrs::bStruckOut() const
{
    return (m_pHostInfo->lfStrikeOut != 0);
}

tCIDLib::TBoolean TFontSelAttrs::bStruckOut(const tCIDLib::TBoolean bNewState)
{
    if (bNewState)
        m_pHostInfo->lfStrikeOut = 1;
    else
        m_pHostInfo->lfStrikeOut = 0;
    return bNewState;
}


// Get/set the underlined attribute
tCIDLib::TBoolean TFontSelAttrs::bUnderlined() const
{
    return (m_pHostInfo->lfUnderline != 0);
}

tCIDLib::TBoolean TFontSelAttrs::bUnderlined(const tCIDLib::TBoolean bNewState)
{
    if (bNewState)
        m_pHostInfo->lfUnderline = 1;
    else
        m_pHostInfo->lfUnderline = 0;
    return bNewState;
}


// Get/set the font height
tCIDLib::TCard4 TFontSelAttrs::c4Height() const
{
    // If less than zero flip it for return
    if (m_pHostInfo->lfHeight < 0)
        return tCIDLib::TCard4(m_pHostInfo->lfHeight * -1);

    return tCIDLib::TCard4(m_pHostInfo->lfHeight);
}

tCIDLib::TCard4
TFontSelAttrs::c4Height(const   tCIDLib::TCard4             c4NewHeight
                        , const tCIDGraphDev::EFontHeights  eNewHeight)
{
    //
    //  According to the height mapping, we make it positive or negative.
    //  If they want the default, then ignore the height field and set it
    //  it to zero.
    //
    if (eNewHeight == tCIDGraphDev::EFontHeights::Default)
        m_pHostInfo->lfHeight = 0;
    else if (eNewHeight == tCIDGraphDev::EFontHeights::EM)
        m_pHostInfo->lfHeight = -tCIDLib::TInt4(c4NewHeight);
    else if (eNewHeight == tCIDGraphDev::EFontHeights::Cell)
        m_pHostInfo->lfHeight = tCIDLib::TInt4(c4NewHeight);
    return c4NewHeight;
}


// Get/set the font width
tCIDLib::TCard4 TFontSelAttrs::c4Width() const
{
    return m_pHostInfo->lfWidth;
}

tCIDLib::TCard4 TFontSelAttrs::c4Width(const tCIDLib::TCard4 c4NewWidth)
{
    m_pHostInfo->lfWidth = c4NewWidth;
    return c4NewWidth;
}


// Get/set the font direction
tCIDGraphDev::EFontDirs TFontSelAttrs::eDir() const
{
    if (m_pHostInfo->lfEscapement == 900)
        return tCIDGraphDev::EFontDirs::D90;
    else if (m_pHostInfo->lfEscapement == 1809)
        return tCIDGraphDev::EFontDirs::D180;
    else if (m_pHostInfo->lfEscapement == 2700)
        return tCIDGraphDev::EFontDirs::D270;

    //
    //  We only support these four directions, so zero or anything other
    //  that one of the supported directions we report as zero.
    //
    return tCIDGraphDev::EFontDirs::D0;
}

tCIDGraphDev::EFontDirs TFontSelAttrs::eDir(const tCIDGraphDev::EFontDirs eToSet)
{
    switch(eToSet)
    {
        case tCIDGraphDev::EFontDirs::D0 :
            m_pHostInfo->lfEscapement = 0;
            m_pHostInfo->lfOrientation = 0;
            break;

        case tCIDGraphDev::EFontDirs::D90 :
            m_pHostInfo->lfEscapement = 900;
            m_pHostInfo->lfOrientation = 900;
            break;

        case tCIDGraphDev::EFontDirs::D180 :
            m_pHostInfo->lfEscapement = 1800;
            m_pHostInfo->lfOrientation = 1800;
            break;

        case tCIDGraphDev::EFontDirs::D270 :
            m_pHostInfo->lfEscapement = 2700;
            m_pHostInfo->lfOrientation = 2700;
            break;

        default :
            facCIDGraphDev().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcFont_UnknownDir
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(tCIDLib::c4EnumOrd(eToSet))
            );
            break;
    };

    return eToSet;
}


// Get/set the font family
tCIDGraphDev::EFontFamilies TFontSelAttrs::eFamily() const
{
    const tCIDLib::TCard4 c4Value = tCIDLib::TCard4
    (
        m_pHostInfo->lfPitchAndFamily & 0xF0
    );

    tCIDGraphDev::EFontFamilies eRet = tCIDGraphDev::EFontFamilies::DontCare;
    if ((c4Value == FF_DONTCARE) || (c4Value == 0))
        eRet = tCIDGraphDev::EFontFamilies::DontCare;
    else if (c4Value == FF_DECORATIVE)
        eRet = tCIDGraphDev::EFontFamilies::Decorative;
    else if (c4Value == FF_MODERN)
        eRet = tCIDGraphDev::EFontFamilies::Modern;
    else if (c4Value == FF_ROMAN)
        eRet = tCIDGraphDev::EFontFamilies::Roman;
    else if (c4Value == FF_SCRIPT)
        eRet = tCIDGraphDev::EFontFamilies::Script;
    else if (c4Value == FF_SWISS)
        eRet = tCIDGraphDev::EFontFamilies::Swiss;
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_UnknownFamily
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Value)
        );
    }
    return eRet;
}

tCIDGraphDev::EFontFamilies
TFontSelAttrs::eFamily(const tCIDGraphDev::EFontFamilies eToSet)
{
    tCIDLib::TCard4 c4Value = 0;

    if (eToSet == tCIDGraphDev::EFontFamilies::DontCare)
        c4Value = FF_DONTCARE;
    else if (eToSet == tCIDGraphDev::EFontFamilies::Decorative)
        c4Value = FF_DECORATIVE;
    else if (eToSet == tCIDGraphDev::EFontFamilies::Modern)
        c4Value = FF_MODERN;
    else if (eToSet == tCIDGraphDev::EFontFamilies::Roman)
        c4Value = FF_ROMAN;
    else if (eToSet == tCIDGraphDev::EFontFamilies::Script)
        c4Value = FF_SCRIPT;
    else if (eToSet == tCIDGraphDev::EFontFamilies::Swiss)
        c4Value = FF_SWISS;
    else
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_UnknownFamily
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TInteger(tCIDLib::i4EnumOrd(eToSet))
        );
    }

    m_pHostInfo->lfPitchAndFamily &= 0x0F;
    m_pHostInfo->lfPitchAndFamily |= c4Value;
    return eToSet;
}


// Get the font height mappign type we are using
tCIDGraphDev::EFontHeights TFontSelAttrs::eHeightMapping() const
{
    if (m_pHostInfo->lfHeight == 0)
        return tCIDGraphDev::EFontHeights::Default;
    else if (m_pHostInfo->lfHeight < 0)
        return tCIDGraphDev::EFontHeights::EM;
    return tCIDGraphDev::EFontHeights::Cell;
}



// Get/ set the font pitch
tCIDGraphDev::EFontPitches TFontSelAttrs::ePitch() const
{
    if ((m_pHostInfo->lfPitchAndFamily & DEFAULT_PITCH) == DEFAULT_PITCH)
        return tCIDGraphDev::EFontPitches::Either;
    else if ((m_pHostInfo->lfPitchAndFamily & DEFAULT_PITCH) == DEFAULT_PITCH)
        return tCIDGraphDev::EFontPitches::Fixed;

    return tCIDGraphDev::EFontPitches::Variable;
}

tCIDGraphDev::EFontPitches TFontSelAttrs::ePitch(const tCIDGraphDev::EFontPitches eToSet)
{
    // Turn off the current bits and set the new ones
    m_pHostInfo->lfPitchAndFamily &= ~0x3;
    if (eToSet == tCIDGraphDev::EFontPitches::Variable)
        m_pHostInfo->lfPitchAndFamily |= VARIABLE_PITCH;
    else if (eToSet == tCIDGraphDev::EFontPitches::Fixed)
        m_pHostInfo->lfPitchAndFamily |= FIXED_PITCH;
    else
        m_pHostInfo->lfPitchAndFamily |= DEFAULT_PITCH;

    return eToSet;
}


// Get/set the font weight to use
tCIDGraphDev::EFontWeights TFontSelAttrs::eWeight() const
{
    if (!m_pHostInfo->lfWeight)
        return tCIDGraphDev::EFontWeights::DontCare;

    // Sanity check. If its > 999, then just return max weight
    if (m_pHostInfo->lfWeight > 999)
        return tCIDGraphDev::EFontWeights::Max;

    //
    //  Our enums are 0 to 9 and map to the weight values of 0 to 999. So
    //  we just take the weight, add 99 and divide by 100.
    //
    return tCIDGraphDev::EFontWeights((m_pHostInfo->lfWeight + 99) / 100);
}

tCIDGraphDev::EFontWeights
TFontSelAttrs::eWeight(const tCIDGraphDev::EFontWeights eNewWeight)
{
    m_pHostInfo->lfWeight = tCIDLib::c4EnumOrd(eNewWeight) * 100;
    return eNewWeight;
}


// Get/set the font face name
TString TFontSelAttrs::strFaceName() const
{
    return TString(m_pHostInfo->lfFaceName);
}

TString TFontSelAttrs::strFaceName(const TString& strNewFace)
{
    strNewFace.ToZStr(m_pHostInfo->lfFaceName, c4MaxBufChars(m_pHostInfo->lfFaceName));
    return strNewFace;
}


// Set the passed height, assuming it's in points
tCIDLib::TVoid
TFontSelAttrs::SetHeightAsPoints(   const   TGraphDrawDev&  gdevTarget
                                    , const tCIDLib::TCard4 c4Points)
{
    //
    //  We use the EM scheme here, because our calculation cannot know the
    //  leading, which would be required to do as a cell height.
    //
    m_pHostInfo->lfHeight = -tCIDLib::TInt4
    (
        (c4Points * gdevTarget.c4VPelsPerInch()) / 72
    );
}


// ---------------------------------------------------------------------------
//  TFontSelAttrs: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFontSelAttrs::StreamFrom(TBinInStream& strmToReadFrom)
{
    // We have to have a start object marker first
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Next is supposed to be the size of the data to follow
    tCIDLib::TCard4 c4Size;
    strmToReadFrom >> c4Size;

    // If its not what we expect, then that's bad
    if (c4Size != sizeof(tagLOGFONTW))
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcFont_BadSelAttrsSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(sizeof(tagLOGFONTW))
            , TCardinal(c4Size)
        );
    }

    //
    //  Looks ok, so read it in. The default is to throw if it doesn't
    //  read all of the requested bytes, so we don't have to check the
    //  return value.
    //
    strmToReadFrom.c4ReadRawBuffer(m_pHostInfo, c4Size);

    // And it has to end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TFontSelAttrs::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  This stuff is Windows specific anyway, so just dump out the raw host
    //  data. We dump out the size of the structure first, so that if its
    //  different when we read back in, we'll know it.
    //
    const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(sizeof(tagLOGFONTW));
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << c4Size;

    //
    //  Write the bytes out now. The default is to throw if it doesn't
    //  write all of the requested bytes, so we don't have to check the
    //  return value.
    //
    strmToWriteTo.c4WriteRawBuffer(m_pHostInfo, c4Size);

    // And cap it off with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}

