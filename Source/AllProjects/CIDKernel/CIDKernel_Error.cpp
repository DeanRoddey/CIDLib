//
// FILE NAME: CIDKernel_Error.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/30/1997
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
//  This file provides the platform independent implementation of the
//  TKrnlError class, the kernel's exception class. Most of it is platform
//  dependent and in CIDKernel_Error_Win32.Cpp in the per-platform
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
//   CLASS: TKrnlError
//  PREFIX: kerr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlError: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlError::ThrowKrnlError(const tCIDLib::TErrCode errcId)
{
    throw TKrnlError(errcId);
}

tCIDLib::TVoid
TKrnlError::ThrowKrnlError( const   tCIDLib::TErrCode   errcId
                            , const tCIDLib::TOSErrCode errcHostId)
{
    throw TKrnlError(errcId, errcHostId);
}


// ---------------------------------------------------------------------------
//  TKrnlError: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlError::TKrnlError() :

    m_errcId(0)
    , m_errcHostId(0)
{
}

TKrnlError::TKrnlError(const tCIDLib::TErrCode errcId) :

    m_errcId(errcId)
    , m_errcHostId(0)
{
}

TKrnlError::TKrnlError( const   tCIDLib::TErrCode   errcId
                        , const tCIDLib::TOSErrCode errcHostId) :
    m_errcId(errcId)
    , m_errcHostId(errcHostId)
{
}

TKrnlError::~TKrnlError()
{
}


// ---------------------------------------------------------------------------
//  TKrnlError: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlError::operator==(const TKrnlError& kerrToCompare) const
{
    if (this == &kerrToCompare)
        return kCIDLib::True;

    if ((m_errcId != kerrToCompare.m_errcId)
    ||  (m_errcHostId != kerrToCompare.m_errcHostId))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlError::operator!=(const TKrnlError& kerrToCompare) const
{
    return !operator==(kerrToCompare);
}



// ---------------------------------------------------------------------------
//  TKrnlError: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlError::bError() const
{
    return (m_errcId != 0);
}


tCIDLib::TErrCode TKrnlError::errcId() const
{
    return m_errcId;
}

tCIDLib::TErrCode TKrnlError::errcId(const tCIDLib::TErrCode errcToSet)
{
    m_errcId = errcToSet;
    return m_errcId;
}


tCIDLib::TOSErrCode TKrnlError::errcHostId() const
{
    return m_errcHostId;
}

tCIDLib::TOSErrCode TKrnlError::errcHostId(const tCIDLib::TOSErrCode errcToSet)
{
    m_errcHostId = errcToSet;
    return m_errcHostId;
}


tCIDLib::TVoid TKrnlError::Reset()
{
    m_errcId = 0;
    m_errcHostId = 0;
}

tCIDLib::TVoid
TKrnlError::Set(const   tCIDLib::TErrCode   errcKrnlId
                , const tCIDLib::TOSErrCode errcHostId)
{
    m_errcId = errcKrnlId;
    m_errcHostId = errcHostId;
}


