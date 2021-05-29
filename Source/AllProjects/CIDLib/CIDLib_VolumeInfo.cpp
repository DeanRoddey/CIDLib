//
// FILE NAME: CIDLib_VolumeInfo.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1993
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
//  This file implements the TVolumeInfo class.
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
AdvRTTIDecls(TVolumeInfo,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDLib_VolumeInfo
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TVolumeInfo
//  PREFIX: voli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TVolumeInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TVolumeInfo::strVolKey(const TVolumeInfo& voliSrc)
{
    return voliSrc.m_strVolume;
}


// ---------------------------------------------------------------------------
//  TVolumeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TVolumeInfo::TVolumeInfo() :

    m_c4MaxPathCompLen(0)
    , m_eFlags(tCIDLib::EVolumeFlags::None)
    , m_eHWType(tCIDLib::EVolHWTypes::Unknown)
{
}

TVolumeInfo::TVolumeInfo(const TString& strSrcPath) :

    m_c4MaxPathCompLen(0)
    , m_eFlags(tCIDLib::EVolumeFlags::None)
    , m_eHWType(tCIDLib::EVolHWTypes::Unknown)
{
    //
    //  Query the volume info for the volume that includes this path and,
    //  if found, copy them to our members.
    //
    TKrnlVolumeInfo kvoliToFill;
    if (!TKrnlFileSys::bQueryVolumeInfo(strSrcPath.pszBuffer(), kvoliToFill))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_VolumeQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strSrcPath
        );
    }

    // Copy over the info we got
    FromRaw(kvoliToFill);
}

TVolumeInfo::TVolumeInfo(const TKrnlVolumeInfo& kvoliSrc)
{
    FromRaw(kvoliSrc);
}

TVolumeInfo::~TVolumeInfo()
{
}


// ---------------------------------------------------------------------------
//  TVolumeInfo: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TVolumeInfo::operator==(const TVolumeInfo& voliToCompare) const
{
    if (this == &voliToCompare)
        return kCIDLib::True;

    if ((m_c4MaxPathCompLen != voliToCompare.m_c4MaxPathCompLen)
    ||  (m_eFlags != voliToCompare.m_eFlags)
    ||  (m_eHWType != voliToCompare.m_eHWType))
    {
        return kCIDLib::False;
    }

    if (m_strVolume != voliToCompare.m_strVolume)
        return kCIDLib::False;

    if (m_strVolumeLabel != voliToCompare.m_strVolumeLabel)
        return kCIDLib::False;

    if (m_strFileSysType != voliToCompare.m_strFileSysType)
        return kCIDLib::False;

    if (m_strVolumeSerialNum != voliToCompare.m_strVolumeSerialNum)
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TVolumeInfo::operator!=(const TVolumeInfo& voliToCompare) const
{
    return !operator==(voliToCompare);
}

TVolumeInfo&
TVolumeInfo::operator=(const TKrnlVolumeInfo& kvoliSrc)
{
    FromRaw(kvoliSrc);
    return *this;
}



// ---------------------------------------------------------------------------
//  TVolumeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TVolumeInfo::c4MaxPathLenComp() const
{
    return m_c4MaxPathCompLen;
}


tCIDLib::EVolumeFlags TVolumeInfo::eFlags() const
{
    return m_eFlags;
}


tCIDLib::EVolHWTypes TVolumeInfo::eHWType() const
{
    return m_eHWType;
}


const TString& TVolumeInfo::strFileSysType() const
{
    return m_strFileSysType;
}


const TString& TVolumeInfo::strVolumeLabel() const
{
    return m_strVolumeLabel;
}


const TString& TVolumeInfo::strVolume() const
{
    return m_strVolume;
}


const TString& TVolumeInfo::strVolumeSerialNum() const
{
    return m_strVolumeSerialNum;
}


tCIDLib::TVoid TVolumeInfo::SetFromPath(const TString& strSrcPath)
{
    //
    //  Query the volume info for the volume that contains this path and, if
    //  found, copy them to our members.
    //
    TKrnlVolumeInfo kvoliToFill;
    if (!TKrnlFileSys::bQueryVolumeInfo(strSrcPath.pszBuffer(), kvoliToFill))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_VolumeQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strSrcPath
        );
    }

    // Copy over the info we got
    FromRaw(kvoliToFill);
}


// ---------------------------------------------------------------------------
//  TVolumeInfo: Protected, inherited method
// ---------------------------------------------------------------------------
tCIDLib::TVoid TVolumeInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDLib_VolumeInfo::c2FmtVersion)
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

    strmToReadFrom  >> m_c4MaxPathCompLen
                    >> m_eFlags
                    >> m_eHWType
                    >> m_strVolume
                    >> m_strVolumeLabel
                    >> m_strFileSysType
                    >> m_strVolumeSerialNum;

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TVolumeInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_VolumeInfo::c2FmtVersion
                    << m_c4MaxPathCompLen
                    << m_eFlags
                    << m_eHWType
                    << m_strVolume
                    << m_strVolumeLabel
                    << m_strFileSysType
                    << m_strVolumeSerialNum
                    << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TVolumeInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TVolumeInfo::FromRaw(const TKrnlVolumeInfo& kvoliSrc)
{
    m_c4MaxPathCompLen  = kvoliSrc.c4MaxPathCompLen();
    m_eFlags            = kvoliSrc.eFlags();
    m_eHWType           = kvoliSrc.eHWType();
    m_strFileSysType    = kvoliSrc.pszFileSysType();
    m_strVolume         = kvoliSrc.pszVolume();
    m_strVolumeLabel    = kvoliSrc.pszVolumeLabel();
    m_strVolumeSerialNum= kvoliSrc.pszVolumeSerialNum();
}

