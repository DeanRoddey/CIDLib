//
// FILE NAME: CIDXML_MemBufEntitySrc.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/1999
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
//  This file implements the TMemBufEntitySrc class.
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
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMemBufEntitySrc,TXMLEntitySrc)



// ---------------------------------------------------------------------------
//  CLASS: TMemBufEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemBufEntitySrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemBufEntitySrc::TMemBufEntitySrc( const   TString&            strSysId
                                    , const TMemBuf* const      pmbufToAdopt
                                    , const tCIDLib::TCard4     c4SrcBytes) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer(pmbufToAdopt)
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&        strSysId
                                    , const TMemBuf&        mbufToCopy
                                    , const tCIDLib::TCard4 c4SrcBytes) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer(new THeapBuf(mbufToCopy.pc1Data(), c4SrcBytes, c4SrcBytes))
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&        strSysId
                                    , const TString&        strPubId
                                    , const TMemBuf&        mbufToCopy
                                    , const tCIDLib::TCard4 c4SrcBytes) :
    TXMLEntitySrc(strSysId, strPubId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer(new THeapBuf(mbufToCopy.pc1Data(), c4SrcBytes, c4SrcBytes))
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&    strSysId
                                    , const TString&    strXMLText) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(strXMLText.c4Length() * kCIDLib::c4CharBytes)
    , m_cptrBuffer(new THeapBuf(strXMLText, m_c4SrcBytes, m_c4SrcBytes))
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&    strSysId
                                    , const TString&    strPubId
                                    , const TString&    strXMLText) :
    TXMLEntitySrc(strSysId, strPubId)
    , m_c4SrcBytes(strXMLText.c4Length() * kCIDLib::c4CharBytes)
    , m_cptrBuffer(new THeapBuf(strXMLText, m_c4SrcBytes, m_c4SrcBytes))
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&                strSysId
                                    , const TCntPtr<const TMemBuf>& cptrBuffer
                                    , const tCIDLib::TCard4         c4SrcBytes) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer(cptrBuffer)
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&                strSysId
                                    , const tCIDLib::TVoid* const   pDataToCopy
                                    , const tCIDLib::TCard4         c4SrcBytes) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer
      (
        new THeapBuf
        (
            reinterpret_cast<const tCIDLib::TCard1*>(pDataToCopy)
            , c4SrcBytes
            , c4SrcBytes
        )
      )
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&                strSysId
                                    , const tCIDLib::TSCh* const    pschDataToCopy
                                    , const tCIDLib::TCard4         c4SrcBytes) :
    TXMLEntitySrc(strSysId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer
      (
        new THeapBuf
        (
            reinterpret_cast<const tCIDLib::TCard1*>(pschDataToCopy)
            , c4SrcBytes
            , c4SrcBytes
        )
      )
{
}

TMemBufEntitySrc::TMemBufEntitySrc( const   TString&                strSysId
                                    , const TString&                strPublicId
                                    , const tCIDLib::TSCh* const    pschDataToCopy
                                    , const tCIDLib::TCard4         c4SrcBytes) :
    TXMLEntitySrc(strSysId, strPublicId)
    , m_c4SrcBytes(c4SrcBytes)
    , m_cptrBuffer
      (
        new THeapBuf
        (
            reinterpret_cast<const tCIDLib::TCard1*>(pschDataToCopy)
            , c4SrcBytes
            , c4SrcBytes
        )
      )
{
}

TMemBufEntitySrc::TMemBufEntitySrc(const TMemBufEntitySrc& xsrcSrc) :

    TXMLEntitySrc(xsrcSrc)
    , m_c4SrcBytes(xsrcSrc.m_c4SrcBytes)
    , m_cptrBuffer(xsrcSrc.m_cptrBuffer)
{
}

TMemBufEntitySrc::~TMemBufEntitySrc()
{
    //
    //  If our counted pointer  is the last one referencing the buffer, it
    //  will be destroyed at this point.
    //
}


// ---------------------------------------------------------------------------
//  TMemBufEntitySrc: Public operators
// ---------------------------------------------------------------------------
TMemBufEntitySrc&
TMemBufEntitySrc::operator=(const TMemBufEntitySrc& xsrcSrc)
{
    if (this == &xsrcSrc)
        return *this;

    TParent::operator=(xsrcSrc);
    m_c4SrcBytes    = xsrcSrc.m_c4SrcBytes;
    m_cptrBuffer    = xsrcSrc.m_cptrBuffer;

    return *this;
}


// ---------------------------------------------------------------------------
//  TMemBufEntitySrc: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TMemBufEntitySrc::c4SrcBytes() const
{
    return m_c4SrcBytes;
}

tCIDLib::TCard4 TMemBufEntitySrc::c4SrcBytes(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SrcBytes = c4ToSet;
    return m_c4SrcBytes;
}


// ---------------------------------------------------------------------------
//  TMemBufEntitySrc: Public, inherited methods
// ---------------------------------------------------------------------------
TBinInStream* TMemBufEntitySrc::pstrmMakeNew() const
{
    // Default is for it not to adopt, which is what we want
    const TMemBuf& mbufCur = *m_cptrBuffer;
    return new TBinMBufInStream(&mbufCur, m_c4SrcBytes);
}

