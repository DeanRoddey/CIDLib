//
// FILE NAME: CIDComm_PortBase.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/23/2004
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
//  This file implements the TCommPortBase class, which provides the virtual
//  base class abstraction for comm ports.
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCommPortBase,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TCommPortBase
// PREFIX: comm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCommPortBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TCommPortBase::c4ReadMBuf(          TMemBuf&                mbufToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            , const tCIDLib::TEncodedTime   enctWait)
{
    //
    //  Make sure that the max bytes requested is not bigger than the max
    //  growth size of the caller's buffer.
    //
    if (c4MaxBytes > mbufToFill.c4MaxSize())
    {
        facCIDComm().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_RBufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4MaxBytes)
            , TCardinal(mbufToFill.c4MaxSize())
        );
    }

    //
    //  If the max size is within the limit of our local buffer, then we'll
    //  use the local buffer. Otherwise, we'll have to allocate one.
    //
    const tCIDLib::TCard4   c4BufSz = 2048;
    tCIDLib::TCard1         ac1TmpBuf[c4BufSz];
    tCIDLib::TCard1*        pc1ToFill = ac1TmpBuf;
    tCIDLib::TCard1*        pc1Alloc = 0;
    if (c4MaxBytes > c4BufSz)
    {
        pc1ToFill = new tCIDLib::TCard1[c4MaxBytes];
        pc1Alloc = pc1ToFill;
    }

    //
    //  Put a janitor on the possibly allocated buffer. If we didn't allocate
    //  one, then the pointer is zero and nothing happens. Then call the
    //  virtual method that works in terms of raw pointers.
    //
    TArrayJanitor<tCIDLib::TCard1> janAlloc(pc1Alloc);
    tCIDLib::TCard4 c4Read = c4ReadRawBuf(pc1ToFill, c4MaxBytes, enctWait);

    // Copy the data into the caller's buffer
    mbufToFill.CopyIn(pc1ToFill, c4Read);
    return c4Read;
}

tCIDLib::TCard4
TCommPortBase::c4ReadMBufMS(        TMemBuf&        mbufToFill
                            , const tCIDLib::TCard4 c4MaxBytes
                            , const tCIDLib::TCard4 c4Wait)
{
    //
    //  Make sure that the max bytes requested is not bigger than the max
    //  growth size of the caller's buffer.
    //
    if (c4MaxBytes > mbufToFill.c4MaxSize())
    {
        facCIDComm().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_RBufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4MaxBytes)
            , TCardinal(mbufToFill.c4MaxSize())
        );
    }

    //
    //  If the max size is within the limit of our local buffer, then we'll
    //  use the local buffer. Otherwise, we'll have to allocate one.
    //
    const tCIDLib::TCard4   c4BufSz = 4096;
    tCIDLib::TCard1         ac1TmpBuf[c4BufSz];
    tCIDLib::TCard1*        pc1ToFill = ac1TmpBuf;
    tCIDLib::TCard1*        pc1Alloc = 0;
    if (c4MaxBytes > c4BufSz)
    {
        pc1ToFill = new tCIDLib::TCard1[c4MaxBytes];
        pc1Alloc = pc1ToFill;
    }

    //
    //  Put a janitor on the possibly allocated buffer. If we didn't allocate
    //  one, then the pointer is zero and nothing happens. Then call the
    //  virtual method that works in terms of raw pointers.
    //
    TArrayJanitor<tCIDLib::TCard1> janAlloc(pc1Alloc);
    tCIDLib::TCard4 c4Read = c4ReadRawBufMS(pc1ToFill, c4MaxBytes, c4Wait);

    // Copy the data into the caller's buffer
    mbufToFill.CopyIn(pc1ToFill, c4Read);
    return c4Read;
}


tCIDLib::TCard4
TCommPortBase::c4WriteMBufMS(const  TMemBuf&        mbufToWrite
                            , const tCIDLib::TCard4 c4BytesToWrite
                            , const tCIDLib::TCard4 c4Wait)
{
    // Make sure the buffer has enough bytes
    if (mbufToWrite.c4Size() < c4BytesToWrite)
    {
        facCIDComm().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_WBufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4BytesToWrite)
            , TCardinal(mbufToWrite.c4Size())
        );
    }

    //
    //  And call the virtual method that works in terms of the raw buffer
    //  pointer.
    //
    return c4WriteRawBufMS(mbufToWrite.pc1Data(), c4BytesToWrite, c4Wait);
}


const TString& TCommPortBase::strPath() const
{
    return m_strPath;
}


// ---------------------------------------------------------------------------
//  TCommPortBase: Hidden Constructor
// ---------------------------------------------------------------------------
TCommPortBase::TCommPortBase()
{
}

TCommPortBase::TCommPortBase(const TString& strPath) :

    m_strPath(strPath)
{
}


