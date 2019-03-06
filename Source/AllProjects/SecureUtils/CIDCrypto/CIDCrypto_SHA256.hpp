//
// FILE NAME: CIDCrypto_SHA256.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/2009
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDCrypto_SHA256.cpp file. This file implements
//  the SHA-1 message digest function. It returns an TSHA256Hash object that
//  holds a message hash for the data hashed.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TSHA256Hasher
//  PREFIX: mdig
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA256Hasher : public THashDigest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSHA256Hasher();

        TSHA256Hasher(const TSHA256Hasher&) = delete;

        ~TSHA256Hasher();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSHA256Hasher& operator=(const TSHA256Hasher&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Complete
        (
                    TMsgHash&               mhashToFill
        )   override;

        tCIDLib::TVoid DigestRaw
        (
            const   tCIDLib::TCard1* const  pc1ToDigest
            , const tCIDLib::TCard4         c4Bytes
        )   override;

        tCIDLib::TVoid DigestSrc
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard4         c4Bytes
        )   override;

        tCIDLib::TVoid StartNew() override;


    private :
        // -------------------------------------------------------------------
        //  Private constants
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4    c4BufCnt = 8;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitContext();

        tCIDLib::TVoid PadMsg();

        tCIDLib::TVoid ProcessMsgBlock
        (
            const   tCIDLib::TCard1* const  pc1Block
        );

        tCIDLib::TVoid ScrubContext();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1Partial
        //      A single block of input, which is twice the size of the hash. This is
        //      used both as a temporary and it holds partial blocks across digest
        //      calls so that we can pick up again where we left off. m_c4PartialCnt
        //      indicates how many bytes are in here.
        //
        //  m_ac4H
        //      Storage for our digest buffers
        //
        //  m_c4ByteCnt
        //      The total count of input bytes so far added to the actual hash. Doesn't
        //      include any partial block not yet added.
        //
        //  m_c4PartialCnt
        //      Used to track partial buffers so that we can pick up where we left
        //      off next time, and to pad the final block if it is not complete.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_ac1Partial[kCIDCrypto::c4SHA256HashBytes * 2];
        tCIDLib::TCard4 m_ac4H[c4BufCnt];
        tCIDLib::TCard4 m_c4ByteCnt;
        tCIDLib::TCard4 m_c4PartialCnt;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSHA256Hasher,TObject)
};

#pragma CIDLIB_POPPACK


