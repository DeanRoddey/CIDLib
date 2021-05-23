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
TKrnlRscName::TKrnlRscName( const   tCIDLib::TCh* const pszCompany
                            , const tCIDLib::TCh* const pszSubsystem
                            , const tCIDLib::TCh* const pszResource
                            , const tCIDLib::TProcessId pidOfName) :
    m_pidOfName(pidOfName)
{
    m_pszCompany = TRawStr::pszReplicate(pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(pszResource);
}

TKrnlRscName::TKrnlRscName(const TKrnlRscName& krscnSrc) :

    m_pidOfName(krscnSrc.m_pidOfName)
{
    if (krscnSrc.m_pszCompany != nullptr)
        m_pszCompany = TRawStr::pszReplicate(krscnSrc.m_pszCompany);
    if (krscnSrc.m_pszSubsystem != nullptr)
        m_pszSubsystem = TRawStr::pszReplicate(krscnSrc.m_pszSubsystem);
    if (krscnSrc.m_pszResource != nullptr)
        m_pszResource = TRawStr::pszReplicate(krscnSrc.m_pszResource);
}

TKrnlRscName::TKrnlRscName(TKrnlRscName&& krscnSrc) :

    m_pidOfName(kCIDLib::pidInvalid)
{
    *this = tCIDLib::ForceMove(krscnSrc);
}


TKrnlRscName::~TKrnlRscName()
{
    delete [] m_pszCompany;
    m_pszCompany = nullptr;
    delete [] m_pszSubsystem;
    m_pszCompany = nullptr;
    delete [] m_pszResource;
    m_pszResource = nullptr;
}


// ---------------------------------------------------------------------------
//  TKrnlRscName: Public operators
// ---------------------------------------------------------------------------
TKrnlRscName& TKrnlRscName::operator=(const TKrnlRscName& krscnSrc)
{
    if (this == &krscnSrc)
        return *this;

    m_pidOfName = krscnSrc.m_pidOfName;

    // Clean up our current strings
    delete [] m_pszCompany;
    m_pszCompany = nullptr;
    delete [] m_pszSubsystem;
    m_pszSubsystem = nullptr;
    delete [] m_pszResource;
    m_pszResource = nullptr;

    // Now replicate the source strings
    if (krscnSrc.m_pszCompany != nullptr)
        m_pszCompany = TRawStr::pszReplicate(krscnSrc.m_pszCompany);
    if (krscnSrc.m_pszSubsystem != nullptr)
        m_pszSubsystem = TRawStr::pszReplicate(krscnSrc.m_pszSubsystem);
    if (krscnSrc.m_pszResource != nullptr)
        m_pszResource = TRawStr::pszReplicate(krscnSrc.m_pszResource);

    return *this;
}

TKrnlRscName& TKrnlRscName::operator=(TKrnlRscName&& krscnSrc)
{
    if (this != &krscnSrc)
    {
        tCIDLib::Swap(m_pidOfName, krscnSrc.m_pidOfName);
        tCIDLib::Swap(m_pszCompany, krscnSrc.m_pszCompany);
        tCIDLib::Swap(m_pszSubsystem, krscnSrc.m_pszSubsystem);
        tCIDLib::Swap(m_pszResource, krscnSrc.m_pszResource);
    }
    return *this;
}


tCIDLib::TBoolean TKrnlRscName::operator==(const TKrnlRscName& krscnSrc) const
{
    if (this == &krscnSrc)
        return kCIDLib::True;

    // Compare the PID first since its fast
    if (m_pidOfName != krscnSrc.m_pidOfName)
        return kCIDLib::False;

    //
    //  Then compare in the order that they are most likely to differ. We use the safe
    //  compare since either side could be null.
    //
    if (!TRawStr::bCompareStr(m_pszResource, krscnSrc.m_pszResource))
        return kCIDLib::False;

    if (!TRawStr::bCompareStr(m_pszSubsystem, krscnSrc.m_pszSubsystem))
        return kCIDLib::False;

    if (!TRawStr::bCompareStr(m_pszCompany, krscnSrc.m_pszCompany))
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlRscName::operator!=(const TKrnlRscName& krsnToCompare) const
{
    return !operator==(krsnToCompare);
}


// ---------------------------------------------------------------------------
//  TKrnlRscName: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlRscName::bIsValid() const
{
    if (!m_pszResource || !m_pszSubsystem || !m_pszCompany)
        return kCIDLib::False;

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
    FaultInStrs();
    return m_pszCompany;
}

const tCIDLib::TCh* TKrnlRscName::pszSubsystemName() const
{
    FaultInStrs();
    return m_pszSubsystem;
}

const tCIDLib::TCh* TKrnlRscName::pszResourceName() const
{
    FaultInStrs();
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
    m_pszCompany = nullptr;
    delete [] m_pszSubsystem;
    m_pszSubsystem = nullptr;
    delete [] m_pszResource;
    m_pszResource = nullptr;

    // Now replicate the source strings
    m_pszCompany = TRawStr::pszReplicate(pszCompany);
    m_pszSubsystem = TRawStr::pszReplicate(pszSubsystem);
    m_pszResource = TRawStr::pszReplicate(pszResource);

    m_pidOfName = pidToFormat;
}


// ---------------------------------------------------------------------------
//  TKrnlRscName: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlRscName::FaultInStrs() const
{
    if (m_pszCompany == nullptr)
        m_pszCompany = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);

    if (m_pszSubsystem == nullptr)
        m_pszSubsystem = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);

    if (m_pszResource == nullptr)
        m_pszResource = TRawStr::pszReplicate(kCIDLib::pszEmptyZStr);
}
