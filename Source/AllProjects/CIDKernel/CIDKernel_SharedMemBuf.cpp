//
// FILE NAME: CIDKernel_SharedMemBuf.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/17/2002
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
//  This file implements the non-platform specific parts of the shared
//  memory class.
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
//   CLASS: TKrnlSharedMemBuf
//  PREFIX: ksmb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSharedMemBuf::operator!=(const TKrnlSharedMemBuf& ksmbToCompare) const
{
    return !operator==(ksmbToCompare);
}


// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TKrnlSharedMemBuf::c4AllocatedPages() const
{
    return m_c4AllocatedPages;
}


tCIDLib::TCard4 TKrnlSharedMemBuf::c4MaxPages() const
{
    return m_c4MaxPages;
}


tCIDLib::TCard4 TKrnlSharedMemBuf::c4MaxSize() const
{
    return m_c4MaxSize;
}


tCIDLib::EMemAccFlags TKrnlSharedMemBuf::eAccess() const
{
    return m_eAccess;
}


tCIDLib::EAllocTypes TKrnlSharedMemBuf::eAllocType() const
{
    return m_eAllocType;
}


tCIDLib::TVoid* TKrnlSharedMemBuf::pData()
{
    return m_pBuffer;
}


const tCIDLib::TVoid* TKrnlSharedMemBuf::pData() const
{
    return m_pBuffer;
}


const tCIDLib::TVoid* TKrnlSharedMemBuf::pNextToCommit() const
{
    return reinterpret_cast<const tCIDLib::TVoid*>
    (
        reinterpret_cast<const tCIDLib::TCard1*>(m_pBuffer)
        + (m_c4AllocatedPages * kCIDLib::c4MemPageSize)
    );
}

tCIDLib::TVoid* TKrnlSharedMemBuf::pNextToCommit()
{
    return reinterpret_cast<tCIDLib::TVoid*>
    (
        reinterpret_cast<tCIDLib::TCard1*>(m_pBuffer)
        + (m_c4AllocatedPages * kCIDLib::c4MemPageSize)
    );
}


const tCIDLib::TCh* TKrnlSharedMemBuf::pszName() const
{
    return m_pszName;
}


