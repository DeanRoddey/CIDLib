//
// FILE NAME: CIDKernel_IP.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2012
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
//  This file implements the platform independent portions of the IP support
//  libraries. Mostly it's the IP address abstraction class we use to hold
//  IP addresses.
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
//  We can do a little of the TkrnlIP namespace generically
// ---------------------------------------------------------------------------
tCIDSock::EAddrTypes TKrnlIP::eDefAddrType()
{
    if (bIPV4Avail() && bIPV6Avail())
        return tCIDSock::EAddrTypes::Unspec;

    if (bIPV6Avail())
        return tCIDSock::EAddrTypes::IPV6;

    return tCIDSock::EAddrTypes::IPV4;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlIPAddr
//  PREFIX: kipa
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlIPAddr: Constructors and Destructor
// ---------------------------------------------------------------------------

// Here we just set up an invalid address
TKrnlIPAddr::TKrnlIPAddr() :

    m_c4Count(0)
    , m_c4FlowInfo(0)
    , m_c4ScopeId(0)
    , m_eType(tCIDSock::EAddrTypes::Unspec)
{
    TRawMem::SetMemBuf(m_ac1Data, tCIDLib::TCard1(0), kCIDSock::c4MaxIPAddrBytes);
}


// Allows for setup of some special address easily
TKrnlIPAddr::TKrnlIPAddr(const  tCIDSock::ESpecAddrs eAddr
                        , const tCIDSock::EAddrTypes    eType) :

    m_c4Count(0)
    , m_c4FlowInfo(0)
    , m_c4ScopeId(0)
    , m_eType(eType)
{
    SetToSpecial(eAddr, eType);
}


TKrnlIPAddr::TKrnlIPAddr(const TKrnlIPAddr& kipaToCopy) :

    m_c4Count(kipaToCopy.m_c4Count)
    , m_c4FlowInfo(kipaToCopy.m_c4FlowInfo)
    , m_c4ScopeId(kipaToCopy.m_c4ScopeId)
    , m_eType(kipaToCopy.m_eType)
{
    TRawMem::CopyMemBuf(m_ac1Data, kipaToCopy.m_ac1Data, kCIDSock::c4MaxIPAddrBytes);
}


TKrnlIPAddr::~TKrnlIPAddr()
{
    // Nothing to do
}


// ---------------------------------------------------------------------------
//  TKrnlIPAddr: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlIPAddr::operator==(const TKrnlIPAddr& kipaSrc) const
{
    //
    //  Check the obvious stuff first. If not the same family type, or the
    //  data count is different, then obviously not equal, and no need to
    //  compare the data bytes.
    //
    if ((m_eType != kipaSrc.m_eType)
    ||  (m_c4Count != kipaSrc.m_c4Count)
    ||  (m_c4ScopeId != kipaSrc.m_c4ScopeId)
    ||  (m_c4FlowInfo != kipaSrc.m_c4FlowInfo))
    {
        return kCIDLib::False;
    }
    return TRawMem::bCompareMemBuf(m_ac1Data, kipaSrc.m_ac1Data, m_c4Count);
}

tCIDLib::TBoolean TKrnlIPAddr::operator!=(const TKrnlIPAddr& kipaSrc) const
{
    return !operator==(kipaSrc);
}

TKrnlIPAddr& TKrnlIPAddr::operator=(const TKrnlIPAddr& kipaSrc)
{
    if (this != &kipaSrc)
    {
        m_eType = kipaSrc.m_eType;
        m_c4Count = kipaSrc.m_c4Count;
        m_c4FlowInfo = kipaSrc.m_c4FlowInfo;
        m_c4ScopeId = kipaSrc.m_c4ScopeId;
        TRawMem::CopyMemBuf(m_ac1Data, kipaSrc.m_ac1Data, m_c4Count);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlIPAddr: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Checks if our address is a loopback address
tCIDLib::TBoolean TKrnlIPAddr::bIsLoopback() const
{
    // If it hasn't been set yet, then obviously not
    if (m_eType == tCIDSock::EAddrTypes::Unspec)
        return kCIDLib::False;


    if (m_eType == tCIDSock::EAddrTypes::IPV4)
    {
        if ((m_ac1Data[0] == 127)
        &&  (m_ac1Data[1] == 0)
        &&  (m_ac1Data[2] == 0)
        &&  (m_ac1Data[3] == 1))
        {
            return kCIDLib::True;
        }
    }
     else if (m_eType == tCIDSock::EAddrTypes::IPV6)
    {
        // If the last byte isn't 1, it can't be
        if (m_ac1Data[15] != 1)
            return kCIDLib::False;

        // And if any of the others aren't zero, it can't be
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 15; c4Index++)
        {
            if (m_ac1Data[c4Index] != 0)
                return kCIDLib::False;
        }
    }
    return kCIDLib::False;
}


//
//  Set us up with the passed info, if it's valid.
//
//  NOTE: The scope id and flow control must be in native byte order!
//
tCIDLib::TBoolean
TKrnlIPAddr::bSet(  const   tCIDLib::TVoid* const   pAddrInfo
                    , const tCIDLib::TCard4         c4Count
                    , const tCIDSock::EAddrTypes    eType
                    , const tCIDLib::TCard4         c4ScopeId
                    , const tCIDLib::TCard4         c4FlowInfo)
{
    tCIDLib::TCard4 c4RealCnt = c4Count;
    if (eType == tCIDSock::EAddrTypes::IPV4)
    {
        // It has to be 4 bytes
        if (c4Count != 4)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }
    }
     else if (eType == tCIDSock::EAddrTypes::IPV6)
    {
        // It has to be 16 bytes
        if (c4Count != 16)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }
    }
     else if (eType == tCIDSock::EAddrTypes::Unspec)
    {
        // Can't be valid
        c4RealCnt = 0;
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_UnknownAddrType);
        return kCIDLib::False;
    }

    // Make sure we have a legal count
    if (c4RealCnt > kCIDSock::c4MaxIPAddrBytes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
        return kCIDLib::False;
    }

    // Store the actual count and type now that we look to succeed
    m_c4Count = c4RealCnt;
    m_eType = eType;
    m_c4FlowInfo = c4FlowInfo;
    m_c4ScopeId = c4ScopeId;

    // Copy in the data, which is assumed to be network order
    if (m_c4Count < kCIDSock::c4MaxIPAddrBytes)
        TRawMem::SetMemBuf(m_ac1Data, tCIDLib::TCard1(0), kCIDSock::c4MaxIPAddrBytes);
    if (m_c4Count)
        TRawMem::CopyMemBuf(m_ac1Data, pAddrInfo, m_c4Count);

    return kCIDLib::True;
}


// Return the count of address bytes
tCIDLib::TCard4 TKrnlIPAddr::c4Count() const
{
    return m_c4Count;
}


// Return the flow info
tCIDLib::TCard4 TKrnlIPAddr::c4FlowInfo() const
{
    return m_c4FlowInfo;
}


// Return the scope id
tCIDLib::TCard4 TKrnlIPAddr::c4ScopeId() const
{
    return m_c4ScopeId;
}


tCIDSock::EAddrTypes TKrnlIPAddr::eType() const
{
    return m_eType;
}


// Provide access to the raw address bytes (size is in m_c4Count)
const tCIDLib::TCard1* TKrnlIPAddr::pc1Data() const
{
    return m_ac1Data;
}


tCIDLib::TVoid TKrnlIPAddr::Reset()
{
    m_c4Count = 0;
    m_c4FlowInfo = 0;
    m_c4ScopeId = 0;
    m_eType = tCIDSock::EAddrTypes::Unspec;
    TRawMem::SetMemBuf(m_ac1Data, tCIDLib::TCard1(0), kCIDSock::c4MaxIPAddrBytes);
}


// Set up for one of the special address
tCIDLib::TVoid
TKrnlIPAddr::SetToSpecial(  const   tCIDSock::ESpecAddrs eAddr
                            , const tCIDSock::EAddrTypes    eType)
{
    // Start zeroed out
    TRawMem::SetMemBuf(m_ac1Data, tCIDLib::TCard1(0), kCIDSock::c4MaxIPAddrBytes);

    // Decide how many bytes we need
    if (eType == tCIDSock::EAddrTypes::IPV4)
        m_c4Count = 4;
    else if (eType == tCIDSock::EAddrTypes::IPV6)
        m_c4Count = 16;
    else
    {
        // Set it to unspecified
        m_eType = tCIDSock::EAddrTypes::Unspec;
        m_c4Count = 0;
        return;
    }

    m_c4FlowInfo = 0;
    m_c4ScopeId = 0;
    m_eType = eType;
    switch(eAddr)
    {
        case tCIDSock::ESpecAddrs::AllMulticast :
            // An all nodes multi-cast
            if (eType == tCIDSock::EAddrTypes::IPV4)
            {
                m_ac1Data[0] = 224;
                m_ac1Data[1] = 0;
                m_ac1Data[2] = 0;
                m_ac1Data[3] = 1;
            }
             else
            {
                m_ac1Data[0] = 0xFF;
                m_ac1Data[1] = 0x02;
                m_ac1Data[15] = 1;
            }
            break;

        case tCIDSock::ESpecAddrs::Broadcast :
            if (eType == tCIDSock::EAddrTypes::IPV4)
            {
                m_ac1Data[0] = 0xFF;
                m_ac1Data[1] = 0xFF;
                m_ac1Data[2] = 0xFF;
                m_ac1Data[3] = 0xFF;
            }
             else
            {
                // Doesn't really exist, return an invalid one
                m_eType = tCIDSock::EAddrTypes::Unspec;
                m_c4Count = 0;
            }
            break;

        case tCIDSock::ESpecAddrs::Loopback :
            if (eType == tCIDSock::EAddrTypes::IPV4)
            {
                m_ac1Data[0] = 127;
                m_ac1Data[1] = 0;
                m_ac1Data[2] = 0;
                m_ac1Data[3] = 1;
            }
             else
            {
                m_ac1Data[15] = 1;
            }
            break;

        default :
        case tCIDSock::ESpecAddrs::Any :
            // Nothing to do in these cases
            break;
    };
}


