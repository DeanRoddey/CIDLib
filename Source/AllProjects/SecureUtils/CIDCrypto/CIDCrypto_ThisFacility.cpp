//
// FILE NAME: CIDCrypto_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
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
#include    "CIDCrypto_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDCrypto,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDCrypto
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDCrypto: Constructors and Destructors
// ---------------------------------------------------------------------------

TFacCIDCrypto::TFacCIDCrypto() :

    TFacility
    (
        L"CIDCrypto"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_prandGen(new TRandomNum)
{
    // Seed our random number generator
    m_prandGen->Seed(TTime::c4Millis());
}

TFacCIDCrypto::~TFacCIDCrypto()
{
    try
    {
        delete m_prandGen;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TFacCIDCrypto: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Get a random number, optionally requiring it to be non-zero. We have one for 32
//  and one for 64 bit values. The latter we have to build up from two 32s.
//
tCIDLib::TCard4 TFacCIDCrypto::c4GetRandom(const tCIDLib::TBoolean bNonZero) const
{
    TMtxLocker mtxlSync(&m_mtxSync);

    const tCIDLib::TCard4 c4MaxTries = 32;
    tCIDLib::TCard4 c4Count = 0;

    tCIDLib::TCard4 c4RetVal;
    while (c4Count < c4MaxTries)
    {
        c4RetVal = m_prandGen->c4GetNextNum();

        // If it's non-zero or zero is ok, then break out
        if (c4RetVal || !bNonZero)
            break;

        // Else try again
        c4Count++;
    }

    // If we never got an acceptable one, then return something at least
    if (c4Count == c4MaxTries)
        c4RetVal = TTime::c4Millis();

    return c4RetVal;
}


tCIDLib::TCard8 TFacCIDCrypto::c8GetRandom(const tCIDLib::TBoolean bNonZero) const
{
    // If non-zero is required make sure the bottom part is non-zero
    tCIDLib::TCard4 c4Low = c4GetRandom(bNonZero);
    tCIDLib::TCard4 c4High = c4GetRandom(kCIDLib::False);

    return TRawBits::c8From32(c4Low, c4High);
}


//
//  Generates a standard HMAC (hash-based message authentication code.) Since we have
//  polymoprphic message digesters and hashes, this will work with whatever hash
//  algorithm the caller chooses, by passing in the right objects, such as a
//  TSHA256Hasher and a TSHA256Hash to fill in. The caller must be sure that both
//  are for the same algorithm. The message digester tells us the block size which we
//  need to know. The hash object tells us the number of bytes in a hash.
//
//  WE have another that takes the key as a string. This one assumes that UTF-8 is the
//  required format for the bytes used. If not, you have to convert it yourself to a
//  buffer.
//
tCIDLib::TVoid
TFacCIDCrypto::GenerateHMAC(const   TString&            strKey
                            , const TMemBuf&            mbufMsg
                            , const tCIDLib::TCard4     c4MsgBytes
                            ,       THashDigest&        mdigToUse
                            ,       TMsgHash&           mhashToFill)
{
    THeapBuf mbufKey(strKey.c4Length() * 2);
    TUTF8Converter tcvtEnc;

    tCIDLib::TCard4 c4KeyBytes;
    tcvtEnc.c4ConvertTo(strKey, mbufKey, c4KeyBytes);

    // Now call the other version
    GenerateHMAC(mbufKey, c4KeyBytes, mbufMsg, c4MsgBytes, mdigToUse, mhashToFill);
}

tCIDLib::TVoid
TFacCIDCrypto::GenerateHMAC(const   TMemBuf&            mbufKey
                            , const tCIDLib::TCard4     c4KeyBytes
                            , const TMemBuf&            mbufMsg
                            , const tCIDLib::TCard4     c4MsgBytes
                            ,       THashDigest&        mdigToUse
                            ,       TMsgHash&           mhashToFill)
{
    const tCIDLib::TCard4 c4BlockSz = mdigToUse.c4BlockSize();

    // Allocate our two pad lists
    tCIDLib::TCard1* pc1IPad = new tCIDLib::TCard1[c4BlockSz];
    tCIDLib::TCard1* pc1OPad = new tCIDLib::TCard1[c4BlockSz];
    TArrayJanitor<tCIDLib::TCard1> janIPad(pc1IPad);
    TArrayJanitor<tCIDLib::TCard1> janOPad(pc1OPad);

    // Initialize them
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BlockSz; c4Index++)
    {
        pc1IPad[c4Index] = 0x36;
        pc1OPad[c4Index] = 0x5C;
    }

    // Set up the real key
    THeapBuf mbufRealKey(c4BlockSz, c4BlockSz);
    if (c4KeyBytes > c4BlockSz)
    {
        //
        //  The key is longer so hash it and use the hash. Use the caller's hash as a temp.
        //  Zero pad it out since the hash bytes are usually less than the block size.
        //
        mdigToUse.StartNew();
        mdigToUse.DigestBuf(mbufKey, c4KeyBytes);
        mdigToUse.Complete(mhashToFill);

        tCIDLib::TCard4 c4HashBytes = mhashToFill.c4ToBuffer(mbufRealKey);
        while (c4HashBytes < c4BlockSz)
            mbufRealKey[c4HashBytes++] = 0;
    }
     else if (c4KeyBytes < c4BlockSz)
    {
        // The key is shorter so just zero pad it out
        mbufRealKey.CopyIn(mbufKey, c4KeyBytes);
        for (tCIDLib::TCard4 c4Index = c4KeyBytes; c4Index < c4BlockSz; c4Index++)
            mbufRealKey[c4Index] = 0;
    }
     else if (c4KeyBytes == c4BlockSz)
    {
        // They are the same so just copy in the bytes
        mbufRealKey.CopyIn(mbufKey, c4BlockSz);
    }

    // And now let's XOR the two pads with the key
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BlockSz; c4Index++)
    {
        pc1IPad[c4Index] ^= mbufRealKey[c4Index];
        pc1OPad[c4Index] ^= mbufRealKey[c4Index];
    }

    // Now we have to hash the concatenation of the in pad and message
    mdigToUse.StartNew();
    mdigToUse.DigestRaw(pc1IPad, c4BlockSz);
    mdigToUse.DigestBuf(mbufMsg, c4MsgBytes);
    mdigToUse.Complete(mhashToFill);

    // Now we need to get that out to a temp buffer so we can set up the final hash
    THeapBuf mbufIPMsg(c4BlockSz);
    const tCIDLib::TCard4 c4HashBytes = mhashToFill.c4ToBuffer(mbufIPMsg);

    //
    //  And now create the final hash which is the hash of the concatenaton of the
    //  out pad and the other hash we created above.
    //
    mdigToUse.StartNew();
    mdigToUse.DigestRaw(pc1OPad, c4BlockSz);
    mdigToUse.DigestBuf(mbufIPMsg, c4HashBytes);
    mdigToUse.Complete(mhashToFill);
}


// Loads up count random bytes into the target buffer starting at the indicated index
tCIDLib::TVoid
TFacCIDCrypto::GetRandomBytes(          TMemBuf&            mbufTarget
                                , const tCIDLib::TCard4     c4At
                                , const tCIDLib::TCard4     c4Count)
{
    TMtxLocker mtxlSync(&m_mtxSync);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        mbufTarget[c4At + c4Index] = tCIDLib::TCard1(m_prandGen->c4GetNextNum());
}


// And one that gets a count of random bytes into a target byte array
tCIDLib::TVoid
TFacCIDCrypto::GetRandomBytes(          tCIDLib::TCard1* const  pc1Target
                                , const tCIDLib::TCard4         c4Count)
{
    TMtxLocker mtxlSync(&m_mtxSync);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        pc1Target[c4Index] = tCIDLib::TCard1(m_prandGen->c4GetNextNum());
}
