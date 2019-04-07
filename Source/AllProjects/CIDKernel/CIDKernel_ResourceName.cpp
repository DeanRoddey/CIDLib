//
// FILE NAME: CIDKernel_ResourceName.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This file implements the the TKrnlRscName class, which is an abstraction
//  for the names of shareable resources. Most of it is platform independent
//  and implmented here, however the stuff that actually builds the internal
//  presentation of the name is implemented in the per-platform directories
//  in the CIDKernel_ResourceName_Win32.CPp file.
//
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
//   CLASS: TKrnlRscName
//  PREFIX: krscn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlRscName: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlRscName::TKrnlRscName() :

    m_pidOfName(kCIDLib::pidInvalid)
    , m_pszCompany(0)
    , m_pszSubsystem(0)
    , m_pszResource(0)
{
    m_pszCompany = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);
    m_pszSubsystem = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);
    m_pszResource = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);
}

TKrnlRscName::TKrnlRscName( const   tCIDLib::TCh* const pszCompany
                            , const tCIDLib::TCh* const pszSubsystem
                            , const tCIDLib::TCh* const pszResource
                            , const tCIDLib::TProcessId pidOfName) :
    m_pidOfName(pidOfName)
    , m_pszCompany(0)
    , m_pszSubsystem(0)
    , m_pszResource(0)
{
    m_pszCompany = TRawStr::pszReplicate(pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(pszResource);
}

TKrnlRscName::TKrnlRscName(const TKrnlRscName& krscnToCopy) :

    m_pidOfName(krscnToCopy.m_pidOfName)
    , m_pszCompany(0)
    , m_pszSubsystem(0)
    , m_pszResource(0)
{
    m_pszCompany = TRawStr::pszReplicate(krscnToCopy.m_pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(krscnToCopy.m_pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(krscnToCopy.m_pszResource);
}

TKrnlRscName::~TKrnlRscName()
{
    delete [] m_pszCompany;
    m_pszCompany = 0;
    delete [] m_pszSubsystem;
    m_pszCompany = 0;
    delete [] m_pszResource;
    m_pszResource = 0;
}


// ---------------------------------------------------------------------------
//  TKrnlRscName: Public operators
// ---------------------------------------------------------------------------
TKrnlRscName& TKrnlRscName::operator=(const TKrnlRscName& krscnToAssign)
{
    if (this == &krscnToAssign)
        return *this;

    m_pidOfName = krscnToAssign.m_pidOfName;

    // Clean up our current strings
    delete [] m_pszCompany;
    m_pszCompany = 0;
    delete [] m_pszSubsystem;
    m_pszSubsystem = 0;
    delete [] m_pszResource;
    m_pszResource = 0;

    // Now replicate the source strings
    m_pszCompany = TRawStr::pszReplicate(krscnToAssign.m_pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(krscnToAssign.m_pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(krscnToAssign.m_pszResource);

    return *this;
}

tCIDLib::TBoolean
TKrnlRscName::operator==(const TKrnlRscName& krscnToCompare) const
{
    if (this == &krscnToCompare)
        return kCIDLib::True;

    // Compare the PID first since its fast
    if (m_pidOfName != krscnToCompare.m_pidOfName)
        return kCIDLib::False;

    // Then compare in the order that they are most likely to differe
    if (!TRawStr::bCompareStr(m_pszResource, krscnToCompare.m_pszResource))
        return kCIDLib::False;

    if (!TRawStr::bCompareStr(m_pszSubsystem, krscnToCompare.m_pszSubsystem))
        return kCIDLib::False;

    if (!TRawStr::bCompareStr(m_pszCompany, krscnToCompare.m_pszCompany))
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlRscName::operator!=(const TKrnlRscName& krsnToCompare) const
{
    return !operator==(krsnToCompare);
}


// ---------------------------------------------------------------------------
//  TKrnlRscName: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlRscName::bIsValid() const
{
    // If any substring is empty, its not valid
    if (!m_pszCompany[0] || !m_pszSubsystem[0] || !m_pszResource[0])
        return kCIDLib::False;
    return kCIDLib::True;
}

tCIDLib::TProcessId TKrnlRscName::pidOfName() const
{
    return m_pidOfName;
}

const tCIDLib::TCh* TKrnlRscName::pszCompanyName() const
{
    return m_pszCompany;
}

const tCIDLib::TCh* TKrnlRscName::pszSubsystemName() const
{
    return m_pszSubsystem;
}

const tCIDLib::TCh* TKrnlRscName::pszResourceName() const
{
    return m_pszResource;
}


tCIDLib::TVoid
TKrnlRscName::SetName(  const   tCIDLib::TCh* const     pszCompany
                        , const tCIDLib::TCh* const     pszSubsystem
                        , const tCIDLib::TCh* const     pszResource
                        , const tCIDLib::TProcessId     pidToFormat)
{
    // Free the existing strings
    delete [] m_pszCompany;
    m_pszCompany = 0;
    delete [] m_pszSubsystem;
    m_pszSubsystem = 0;
    delete [] m_pszResource;
    m_pszResource = 0;

    // Now replicate the source strings
    m_pszCompany = TRawStr::pszReplicate(pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(pszResource);

    m_pidOfName = pidToFormat;
}

