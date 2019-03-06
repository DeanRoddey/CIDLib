//
// FILE NAME: CIDCrypto_SChan.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TSChannel class.
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
#include    "CIDSChan_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSChannel,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSChannel
//  PREFIX: schan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TSChannel::TSChannel() :

    m_bClient(kCIDLib::False)
    , m_c4DecBufSz(0)
    , m_mbufDecBuf(32 * 1024)
    , m_pInfo(nullptr)
{
}

TSChannel::~TSChannel()
{
    try
    {
        //
        //  They are supposed to have terminated. If not, then we can't do anything
        //  but a hard cleanup without going through the normal shutdown with the
        //  other side.
        //
        if (m_pInfo)
            Cleanup();
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TSChannel: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We can read into a raw buffer or memory buffer, using either a milliseconds wait
//  time or an end time. These just call a private per-platform method
//
tCIDLib::TCard4
TSChannel::c4ReadRawData(       TCIDDataSrc&            cdsTar
                        ,       tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4ReqBytes
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::EAllData       eAllData)
{
    return c4ReceiveData(cdsTar, pc1ToFill, c4ReqBytes, enctEnd, eAllData);
}

tCIDLib::TCard4
TSChannel::c4ReadData(          TCIDDataSrc&            cdsTar
                        ,       TMemBuf&                mbufToFill
                        , const tCIDLib::TCard4         c4ReqBytes
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::EAllData       eAllData)
{
    //
    //  Make sure the buffer is allocated large enough, then call the other version
    //  and let it write into the buffer's pointer.
    //
    if (mbufToFill.c4Size() < c4ReqBytes)
        mbufToFill.Reallocate(c4ReqBytes, kCIDLib::False);

    return c4ReceiveData(cdsTar, mbufToFill.pc1Data(), c4ReqBytes, enctEnd, eAllData);
}


tCIDLib::TCard4
TSChannel::c4ReadRawDataMS(         TCIDDataSrc&            cdsTar
                            ,       tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4ReqBytes
                            , const tCIDLib::TCard4         c4ToWait
                            , const tCIDLib::EAllData       eAllData)
{
    // Convert millis to end time and call the other version
    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(c4ToWait);
    return c4ReceiveData(cdsTar, pc1ToFill, c4ReqBytes, enctEnd, eAllData);
}


tCIDLib::TCard4
TSChannel::c4ReadDataMS(        TCIDDataSrc&        cdsTar
                        ,       TMemBuf&            mbufToFill
                        , const tCIDLib::TCard4     c4ReqBytes
                        , const tCIDLib::TCard4     c4ToWait
                        , const tCIDLib::EAllData   eAllData)
{
    // Convert millis to end time and call the other version
    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(c4ToWait);

    //
    //  Make sure the buffer is allocated large enough, then call the other version
    //  and let it write into the buffer's pointer.
    //
    if (mbufToFill.c4Size() < c4ReqBytes)
        mbufToFill.Reallocate(c4ReqBytes, kCIDLib::False);

    return c4ReceiveData(cdsTar, mbufToFill.pc1Data(), c4ReqBytes, enctEnd, eAllData);
}


//
//  Both of these just call a private helper that will chunk the data, encrypt it and
//  buffer it to our internal data source's output, then it will flush the data source
//  output when everything is ready.
//
tCIDLib::TVoid
TSChannel::WriteData(       TCIDDataSrc&            cdsTar
                    , const TMemBuf&                mbufToSend
                    , const tCIDLib::TCard4         c4ByteCnt)
{
    TransmitData(cdsTar, mbufToSend.pc1Data(), c4ByteCnt);
}

tCIDLib::TVoid
TSChannel::WriteData(       TCIDDataSrc&            cdsTar
                    , const tCIDLib::TCard1* const  pc1ToSend
                    , const tCIDLib::TCard4         c4ByteCnt)
{
    TransmitData(cdsTar, pc1ToSend, c4ByteCnt);
}

