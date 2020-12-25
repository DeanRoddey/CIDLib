//
// FILE NAME: CIDKernel_IP.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2012
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


// ---------------------------------------------------------------------------
//  TKrnlIPAddr: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Set from a socket address. The passed value is assumed on this platform to
//  be one of the sockaddr variants for IPV4 or IPV6. Those are the only types
//  we support.
//
//  We are getting in incoming address here, so we flip the port to little
//  endian mode if that's the local order.
//
tCIDLib::TBoolean
TKrnlIPAddr::bFromSockAddr( const   tCIDLib::TVoid* const   pAddr
                            , const tCIDLib::TCard4         c4Len
                            ,       tCIDLib::TIPPortNum&    ippnToFill)
{
    // Cast it to the generic socket address class to look at the family
    const sockaddr* pSockAddr = static_cast<const sockaddr*>(pAddr);

    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (pSockAddr->sa_family == AF_INET)
    {
        // It's V4
        if (c4Len < sizeof(sockaddr_in))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }
        const sockaddr_in* p4Addr = reinterpret_cast<const sockaddr_in*>(pSockAddr);

        // If we are little endian, swap the port number bytes
        #if defined(CIDLIB_LITTLEENDIAN)
        ippnToFill = tCIDLib::TIPPortNum(TRawBits::c2SwapBytes(p4Addr->sin_port));
        #else
        ippnToFill = p4Addr->sin_port;
        #endif

        bRet = bSet
        (
            (tCIDLib::TCard1*)&p4Addr->sin_addr, 4, tCIDSock::EAddrTypes::IPV4, 0, 0
        );
    }
     else if (pSockAddr->sa_family == AF_INET6)
    {
        // It's V6
        if (c4Len < sizeof(sockaddr_in6))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }
        const sockaddr_in6* p6Addr = reinterpret_cast<const sockaddr_in6*>(pSockAddr);

        // If we are little endian, we have to flip some stuff
        tCIDLib::TCard4 c4ScopeId;
        tCIDLib::TCard4 c4FlowInfo;
        #if defined(CIDLIB_LITTLEENDIAN)
        ippnToFill = tCIDLib::TIPPortNum(TRawBits::c2SwapBytes(p6Addr->sin6_port));
        c4ScopeId = TRawBits::c4SwapBytes(p6Addr->sin6_scope_id);
        c4FlowInfo = TRawBits::c4SwapBytes(p6Addr->sin6_flowinfo);
        #else
        ippnToFill = p6Addr->sin6_port;
        c4ScopeId = p6Addr->sin6_scope_id;
        c4FlowInfo = p6Addr->sin6_flowinfo;
        #endif

        // And store the address info, passing in the native order values!
        bRet = bSet
        (
            (tCIDLib::TCard1*)&p6Addr->sin6_addr
            , kCIDSock::c4MaxIPAddrBytes
            , tCIDSock::EAddrTypes::IPV6
            , c4ScopeId
            , c4FlowInfo
        );
    }
     else
    {
        // Unknown, so reset it
        Reset();
    }
    return bRet;
}


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


//
//  Copies out our address info when the caller only wants the in_addr stuff,
//  not the whole sockaddr_in stuff.
//
tCIDLib::TBoolean
TKrnlIPAddr::bToInAddr( tCIDLib::TVoid* const   pAddr
                        , tCIDLib::TCard4&      c4SzInOut) const
{
    TRawMem::SetMemBuf(pAddr, tCIDLib::TCard1(0), c4SzInOut);

    if (m_eType == tCIDSock::EAddrTypes::IPV4)
    {
        if (c4SzInOut < sizeof(in_addr))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }

        c4SzInOut = sizeof(in_addr);
        TRawMem::CopyMemBuf(pAddr, m_ac1Data, 4);
    }
     else if (m_eType == tCIDSock::EAddrTypes::IPV6)
    {
        if (c4SzInOut < sizeof(in6_addr))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }

        c4SzInOut = sizeof(in6_addr);
        TRawMem::CopyMemBuf(pAddr, m_ac1Data, 16);
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_UnknownAddrType);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Set up a socket address in the passed data buffer. We get the size coming
//  in so we can be sure it's large enough. And we set the actual size of the
//  structure we put into it as the output.
//
//  We are creating an address for outgoing here, so we flip the port to
//  network order if that's not the native endian order.
//
tCIDLib::TBoolean
TKrnlIPAddr::bToSockAddr(       tCIDLib::TVoid* const  pAddr
                        ,       tCIDLib::TCard4&       c4SzInOut
                        , const tCIDLib::TIPPortNum    ippnPort) const
{
    TRawMem::SetMemBuf(pAddr, tCIDLib::TCard1(0), c4SzInOut);

    if (m_eType == tCIDSock::EAddrTypes::IPV4)
    {
        if (c4SzInOut < sizeof(sockaddr_in))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }

        sockaddr_in* pRealAddr = (sockaddr_in*)pAddr;
        c4SzInOut = sizeof(sockaddr_in);
        pRealAddr->sin_family = AF_INET;

        // If we are little endian, swap the port number, else take it as is
        pRealAddr->sin_port = tCIDLib::TCard2(ippnPort);
        #if defined(CIDLIB_LITTLEENDIAN)
        pRealAddr->sin_port = TRawBits::c2SwapBytes(pRealAddr->sin_port);
        #endif

        TRawMem::CopyMemBuf(&pRealAddr->sin_addr, m_ac1Data, 4);
    }
     else if (m_eType == tCIDSock::EAddrTypes::IPV6)
    {
        if (c4SzInOut < sizeof(sockaddr_in6))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_BadAddrSize);
            return kCIDLib::False;
        }

        sockaddr_in6* pRealAddr = (sockaddr_in6*)pAddr;
        c4SzInOut = sizeof(sockaddr_in6);
        pRealAddr->sin6_family = AF_INET6;

        // Handle endian specific stuff
        #if defined(CIDLIB_LITTLEENDIAN)
        pRealAddr->sin6_port = TRawBits::c2SwapBytes(tCIDLib::TCard2(ippnPort));
        pRealAddr->sin6_scope_id = TRawBits::c4SwapBytes(m_c4ScopeId);
        pRealAddr->sin6_flowinfo = TRawBits::c4SwapBytes(m_c4FlowInfo);
        #else
        pRealAddr->sin6_port = tCIDLib::TCard2(ippnPort);
        pRealAddr->sin6_scope_id = m_c4ScopeId;
        pRealAddr->sin6_flowinfo = m_c4FlowInfo;
        #endif

        TRawMem::CopyMemBuf(&pRealAddr->sin6_addr, m_ac1Data, 16);
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcIP_UnknownAddrType);
        return kCIDLib::False;
    }
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


//
//  This one is more for higher level code that doesn't have access to the socket
//  headers but needs to get raw IP address data. We allocate a buffer for them
//  and tell them the number of relevant bytes (though the buffer we give back may
//  be bigger than that.)
//
tCIDLib::TCard1*
TKrnlIPAddr::pc1ToSockAddr(tCIDLib::TCard4& c4SzOut, const tCIDLib::TIPPortNum ippnPort) const
{
    // Worst case it's an IPV6 socket address
    tCIDLib::TCard1* pc1Ret = new tCIDLib::TCard1[sizeof(sockaddr_in6)];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Ret);

    //
    //  Tell the converter method how many bytes we have available so he can sanity check.
    //  If successfull, he updates it with the actual bytes.
    //
    c4SzOut = sizeof(sockaddr_in6);
    if (!bToSockAddr(pc1Ret, c4SzOut, ippnPort))
        return nullptr;

    return janBuf.paOrphan();
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


