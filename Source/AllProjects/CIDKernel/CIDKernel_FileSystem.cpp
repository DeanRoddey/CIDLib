//
// FILE NAME: CIDKernel_FileSystem.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/14/1999
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
//  This file provides the platform independent implementation of the
//  TKrnlError class, the kernel's exception class. Most of it is platform
//  dependent and in CIDKernel_FileSystem_Win32.Cpp in the per-platform
//  directories.
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
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TKrnlVolFailureInfo
//  PREFIX: kvolfi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlVolFailureInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlVolFailureInfo::TKrnlVolFailureInfo() :

    m_eHWType(tCIDLib::EVolHWTypes::Unknown)
    , m_pszVolume(0)
{
}

TKrnlVolFailureInfo::TKrnlVolFailureInfo(const  tCIDLib::TCh* const  pszVolume
                                        , const tCIDLib::EVolHWTypes eHWType
                                        , const TKrnlError&          kerrReason) :
    m_kerrReason(kerrReason)
    , m_eHWType(eHWType)
    , m_pszVolume(0)
{
    m_pszVolume = TRawStr::pszReplicate(pszVolume);
}

TKrnlVolFailureInfo::TKrnlVolFailureInfo(const TKrnlVolFailureInfo& kvolfiToCopy) :

    m_eHWType(kvolfiToCopy.m_eHWType)
    , m_kerrReason(kvolfiToCopy.m_kerrReason)
    , m_pszVolume(0)
{
    m_pszVolume = TRawStr::pszReplicate(kvolfiToCopy.m_pszVolume);
}

TKrnlVolFailureInfo::~TKrnlVolFailureInfo()
{
    delete [] m_pszVolume;
}


// ---------------------------------------------------------------------------
//  TKrnlVolFailureInfo: Public operators
// ---------------------------------------------------------------------------
TKrnlVolFailureInfo&
TKrnlVolFailureInfo::operator=(const TKrnlVolFailureInfo& kvolfiToAssign)
{
    if (this == &kvolfiToAssign)
        return *this;

    m_eHWType       = kvolfiToAssign.m_eHWType;
    m_kerrReason    = kvolfiToAssign.m_kerrReason;
    delete [] m_pszVolume;
    m_pszVolume     = TRawStr::pszReplicate(kvolfiToAssign.m_pszVolume);

    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlVolFailureInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EVolHWTypes TKrnlVolFailureInfo::eHWType() const
{
    return m_eHWType;
}


tCIDLib::TErrCode TKrnlVolFailureInfo::errcReason() const
{
    return m_kerrReason.errcId();
}


const TKrnlError& TKrnlVolFailureInfo::kerrReason() const
{
    return m_kerrReason;
}


const tCIDLib::TCh* TKrnlVolFailureInfo::pszVolume() const
{
    return m_pszVolume;
}

tCIDLib::TVoid
TKrnlVolFailureInfo::Set(   const   tCIDLib::TCh* const     pszVolume
                            , const tCIDLib::EVolHWTypes    eHWType
                            , const TKrnlError&             kerrReason)
{
    // Overwrite our existing type and error info
    m_eHWType = eHWType;
    m_kerrReason = kerrReason;

    // Delete our current volume and replicate the new one
    delete [] m_pszVolume;
    m_pszVolume = TRawStr::pszReplicate(pszVolume);
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlVolumeInfo
//  PREFIX: kvoli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlVolumeInfo::TKrnlVolumeInfo() :

    m_c4MaxPathCompLen(0)
    , m_eFlags(tCIDLib::EVolumeFlags::None)
    , m_eHWType(tCIDLib::EVolHWTypes::Unknown)
    , m_pszFileSysType(0)
    , m_pszVolume(0)
    , m_pszVolumeLabel(0)
    , m_pszVolumeSerialNum(0)
{
}

TKrnlVolumeInfo::TKrnlVolumeInfo(const  tCIDLib::TCard4         c4MaxPathCompLen
                                , const tCIDLib::EVolumeFlags   eFlags
                                , const tCIDLib::EVolHWTypes    eHWType
                                , const tCIDLib::TCh* const     pszFileSysType
                                , const tCIDLib::TCh* const     pszVolume
                                , const tCIDLib::TCh* const     pszVolumeLabel
                                , const tCIDLib::TCh* const     pszVolumeSerialNum) :

    m_c4MaxPathCompLen(c4MaxPathCompLen)
    , m_eFlags(eFlags)
    , m_eHWType(eHWType)
    , m_pszFileSysType(0)
    , m_pszVolume(0)
    , m_pszVolumeLabel(0)
    , m_pszVolumeSerialNum(0)
{
    m_pszFileSysType      = TRawStr::pszReplicate(pszFileSysType);
    m_pszVolume           = TRawStr::pszReplicate(pszVolume);
    m_pszVolumeLabel      = TRawStr::pszReplicate(pszVolumeLabel);
    m_pszVolumeSerialNum  = TRawStr::pszReplicate(pszVolumeSerialNum);
}

TKrnlVolumeInfo::TKrnlVolumeInfo(const TKrnlVolumeInfo& kvoliToCopy) :

    m_c4MaxPathCompLen(kvoliToCopy.m_c4MaxPathCompLen)
    , m_eFlags(kvoliToCopy.m_eFlags)
    , m_eHWType(kvoliToCopy.m_eHWType)
    , m_pszFileSysType(0)
    , m_pszVolume(0)
    , m_pszVolumeLabel(0)
    , m_pszVolumeSerialNum(0)
{
    m_pszFileSysType      = TRawStr::pszReplicate(kvoliToCopy.m_pszFileSysType);
    m_pszVolume           = TRawStr::pszReplicate(kvoliToCopy.m_pszVolume);
    m_pszVolumeLabel      = TRawStr::pszReplicate(kvoliToCopy.m_pszVolumeLabel);
    m_pszVolumeSerialNum  = TRawStr::pszReplicate(kvoliToCopy.m_pszVolumeSerialNum);
}

TKrnlVolumeInfo::~TKrnlVolumeInfo()
{
    delete [] m_pszFileSysType;
    delete [] m_pszVolume;
    delete [] m_pszVolumeLabel;
    delete [] m_pszVolumeSerialNum;
}


// ---------------------------------------------------------------------------
//  Public operators
// ---------------------------------------------------------------------------
TKrnlVolumeInfo& TKrnlVolumeInfo::operator=(const TKrnlVolumeInfo& kvoliToAssign)
{
    if (this == &kvoliToAssign)
        return *this;

    m_c4MaxPathCompLen  = kvoliToAssign.m_c4MaxPathCompLen;
    m_eFlags            = kvoliToAssign.m_eFlags;
    m_eHWType           = kvoliToAssign.m_eHWType;

    delete [] m_pszFileSysType;
    m_pszFileSysType = 0;
    delete [] m_pszVolume;
    m_pszVolume = 0;
    delete [] m_pszVolumeLabel;
    m_pszVolumeLabel = 0;
    delete [] m_pszVolumeSerialNum;
    m_pszVolumeSerialNum = 0;

    m_pszFileSysType      = TRawStr::pszReplicate(kvoliToAssign.m_pszFileSysType);
    m_pszVolume           = TRawStr::pszReplicate(kvoliToAssign.m_pszVolume);
    m_pszVolumeLabel      = TRawStr::pszReplicate(kvoliToAssign.m_pszVolumeLabel);
    m_pszVolumeSerialNum  = TRawStr::pszReplicate(kvoliToAssign.m_pszVolumeSerialNum);

    return *this;
}


tCIDLib::TVoid
TKrnlVolumeInfo::Set(const  tCIDLib::TCard4         c4MaxPathCompLen
                    , const tCIDLib::EVolumeFlags   eFlags
                    , const tCIDLib::EVolHWTypes    eHWType
                    , const tCIDLib::TCh* const     pszFileSysType
                    , const tCIDLib::TCh* const     pszVolume
                    , const tCIDLib::TCh* const     pszVolumeLabel
                    , const tCIDLib::TCh* const     pszVolumeSerialNum)
{
    m_c4MaxPathCompLen  = c4MaxPathCompLen;
    m_eFlags            = eFlags;
    m_eHWType           = eHWType;

    delete [] m_pszFileSysType;
    m_pszFileSysType = 0;
    delete [] m_pszVolume;
    m_pszVolume = 0;
    delete [] m_pszVolumeLabel;
    m_pszVolumeLabel = 0;
    delete [] m_pszVolumeSerialNum;
    m_pszVolumeSerialNum = 0;

    m_pszFileSysType      = TRawStr::pszReplicate(pszFileSysType);
    m_pszVolume           = TRawStr::pszReplicate(pszVolume);
    m_pszVolumeLabel      = TRawStr::pszReplicate(pszVolumeLabel);
    m_pszVolumeSerialNum  = TRawStr::pszReplicate(pszVolumeSerialNum);
}


// ---------------------------------------------------------------------------
//  TKrnlVolumeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TKrnlVolumeInfo::c4MaxPathCompLen() const
{
    return m_c4MaxPathCompLen;
}


//
//  Called when a removal notification is received, to clear the info that
//  we no longer know for sure.
//
tCIDLib::TVoid TKrnlVolumeInfo::ClearUsedInfo()
{
    m_c4MaxPathCompLen = 0;
    m_eFlags = tCIDLib::EVolumeFlags::None;
    m_eHWType = tCIDLib::EVolHWTypes::Unknown;

    if (m_pszFileSysType)
        m_pszFileSysType[0] = kCIDLib::chNull;

    if (m_pszVolumeLabel)
        m_pszVolumeLabel[0] = kCIDLib::chNull;

    if (m_pszVolumeSerialNum)
        m_pszVolumeSerialNum[0] = kCIDLib::chNull;
}


tCIDLib::EVolumeFlags TKrnlVolumeInfo::eFlags() const
{
    return m_eFlags;
}

tCIDLib::EVolHWTypes TKrnlVolumeInfo::eHWType() const
{
    return m_eHWType;
}

const tCIDLib::TCh* TKrnlVolumeInfo::pszFileSysType() const
{
    return m_pszFileSysType;
}

const tCIDLib::TCh* TKrnlVolumeInfo::pszVolume() const
{
    return m_pszVolume;
}

const tCIDLib::TCh* TKrnlVolumeInfo::pszVolumeLabel() const
{
    return m_pszVolumeLabel;
}

const tCIDLib::TCh* TKrnlVolumeInfo::pszVolumeSerialNum() const
{
    return m_pszVolumeSerialNum;
}


// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirSearch
//  PREFIX: kds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlDirSearch: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EDirSearchFlags TKrnlDirSearch::eSearchFlags() const
{
    return m_eSearchFlags;
}


