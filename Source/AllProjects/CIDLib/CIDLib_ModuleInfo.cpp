//
// FILE NAME: CIDLib_ModuleInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/23/2002
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
namespace
{
    namespace CIDLib_ModuleInfo
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDModuleInfo
// PREFIX: modi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TCIDModuleInfo: Constructors and Destructor
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



// ---------------------------------------------------------------------------
//  TCIDModuleInfo: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDModuleInfo::operator==(const TCIDModuleInfo& midoSrc) const
{
    if ((m_strBaseName  != midoSrc.m_strBaseName)
    ||  (m_c4MajVersion != midoSrc.m_c4MajVersion)
    ||  (m_c4MinVersion != midoSrc.m_c4MinVersion)
    ||  (m_eFlags       != midoSrc.m_eFlags)
    ||  (m_eModType     != midoSrc.m_eModType))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TCIDModuleInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
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
//  TCIDModuleInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDModuleInfo::FormatTo(CIOP TTextOutStream& strmDest) const
{
    TString strPortable, strLoadable;
    TModule::BuildModName
    (
        m_strBaseName
        , m_c4MajVersion
        , m_c4MinVersion
        , m_eModType
        , strPortable
        , strLoadable
    );

    strmDest << L"(" << strPortable << L"/" << strLoadable << L")";
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


