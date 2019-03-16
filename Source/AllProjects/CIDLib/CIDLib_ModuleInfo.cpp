//
// FILE NAME: CIDLib_ModuleInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/23/2002
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
//  This file implements the TCIDModuleInfo class.
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
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDModuleInfo,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_ModuleInfo
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDModuleInfo
// PREFIX: modi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDModuleInfo::TCIDModuleInfo() :

    m_c4MajVersion(0)
    , m_c4MinVersion(0)
    , m_eFlags(tCIDLib::EModFlags::None)
    , m_eModType(tCIDLib::EModTypes::Exe)
{
}


TCIDModuleInfo::TCIDModuleInfo( const   TString&            strBaseName
                                , const tCIDLib::EModTypes  eModType
                                , const tCIDLib::TCard4     c4MajVersion
                                , const tCIDLib::TCard4     c4MinVersion
                                , const tCIDLib::EModFlags  eFlags) :
    m_c4MajVersion(c4MajVersion)
    , m_c4MinVersion(c4MinVersion)
    , m_eFlags(eFlags)
    , m_eModType(eModType)
    , m_strBaseName(strBaseName)
{
}

TCIDModuleInfo::TCIDModuleInfo( const   TString&            strBaseName
                                , const tCIDLib::EModTypes  eModType
                                , const tCIDLib::EModFlags  eFlags) :
    m_c4MajVersion(0)
    , m_c4MinVersion(0)
    , m_eFlags(eFlags)
    , m_eModType(eModType)
    , m_strBaseName(strBaseName)
{
}

TCIDModuleInfo::TCIDModuleInfo(const TCIDModuleInfo& modiToCopy) :

    m_c4MajVersion(modiToCopy.m_c4MajVersion)
    , m_c4MinVersion(modiToCopy.m_c4MinVersion)
    , m_eFlags(modiToCopy.m_eFlags)
    , m_eModType(modiToCopy.m_eModType)
    , m_strBaseName(modiToCopy.m_strBaseName)
{
}

TCIDModuleInfo::~TCIDModuleInfo()
{
}


// ---------------------------------------------------------------------------
//  Public operators
// ---------------------------------------------------------------------------
TCIDModuleInfo& TCIDModuleInfo::operator=(const TCIDModuleInfo& modiToAssign)
{
    if (this == &modiToAssign)
        return *this;

    m_c4MajVersion  = modiToAssign.m_c4MajVersion;
    m_c4MinVersion  = modiToAssign.m_c4MinVersion;
    m_eFlags        = modiToAssign.m_eFlags;
    m_eModType      = modiToAssign.m_eModType;
    m_strBaseName   = modiToAssign.m_strBaseName;

    return *this;
}


tCIDLib::TBoolean
TCIDModuleInfo::operator==(const TCIDModuleInfo& modiToCompare) const
{
    if ((m_strBaseName  != modiToCompare.m_strBaseName)
    ||  (m_c4MajVersion != modiToCompare.m_c4MajVersion)
    ||  (m_c4MinVersion != modiToCompare.m_c4MinVersion)
    ||  (m_eFlags       != modiToCompare.m_eFlags)
    ||  (m_eModType     != modiToCompare.m_eModType))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TCIDModuleInfo::operator!=(const TCIDModuleInfo& modiToCompare) const
{
    return !operator==(modiToCompare);
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCIDModuleInfo::c4MajVersion() const
{
    return m_c4MajVersion;
}

tCIDLib::TCard4 TCIDModuleInfo::c4MinVersion() const
{
    return m_c4MinVersion;
}

tCIDLib::EModFlags TCIDModuleInfo::eFlags() const
{
    return m_eFlags;
}

tCIDLib::EModTypes TCIDModuleInfo::eModType() const
{
    return m_eModType;
}

const TString& TCIDModuleInfo::strBaseName() const
{
    return m_strBaseName;
}


tCIDLib::TVoid TCIDModuleInfo::Set( const   TString&            strBaseName
                                    , const tCIDLib::EModTypes  eModType
                                    , const tCIDLib::TCard4     c4MajVersion
                                    , const tCIDLib::TCard4     c4MinVersion
                                    , const tCIDLib::EModFlags  eFlags)
{
    m_c4MajVersion  = c4MajVersion;
    m_c4MinVersion  = c4MinVersion;
    m_eFlags        = eFlags;
    m_eModType      = eModType;
    m_strBaseName   = strBaseName;
}

tCIDLib::TVoid TCIDModuleInfo::Set( const   TString&            strBaseName
                                    , const tCIDLib::EModTypes  eModType
                                    , const tCIDLib::EModFlags  eFlags)
{
    m_c4MajVersion  = 0;
    m_c4MinVersion  = 0;
    m_eFlags        = eFlags;
    m_eModType      = eModType;
    m_strBaseName   = strBaseName;
}


// ---------------------------------------------------------------------------
//  Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDModuleInfo::FormatTo(TTextOutStream& strmDest) const
{
    // Build up the platform specific name and format it
    TString strTmp;
    TModule::BuildRawModName(*this, strTmp);
    strmDest << strTmp;
}


tCIDLib::TVoid TCIDModuleInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDLib_ModuleInfo::c2FmtVersion)
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

    strmToReadFrom  >> m_c4MajVersion
                    >> m_c4MinVersion
                    >> m_eFlags
                    >> m_eModType
                    >> m_strBaseName;

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TCIDModuleInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_ModuleInfo::c2FmtVersion
                    << m_c4MajVersion
                    << m_c4MinVersion
                    << m_eFlags
                    << m_eModType
                    << m_strBaseName
                    << tCIDLib::EStreamMarkers::EndObject;
}


