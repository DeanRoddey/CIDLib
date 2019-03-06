//
// FILE NAME: CIDCrypto_SHA1.hpp
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
//  This is the header for the CIDCrypto_SHA1.cpp file. This file implements
//  the SHA-1 message digest function. It returns an TSHA1Hash object that
//  holds a message hash for the data hashed.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TSHA1Hasher
//  PREFIX: mdig
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA1Hasher : public THashDigest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSHA1Hasher();

        TSHA1Hasher(const TSHA1Hasher&) = delete;

        ~TSHA1Hasher();


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
        //  Private class constants
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4    c4BlockSz = 64;
        static constexpr tCIDLib::TCard4    c4BufCnt  = 5;


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------

        tCIDLib::TVoid operator=(const TSHA1Hasher&);


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
        //      We have to remember any partial block across calls to Digest,
        //      so that we can pick up from there the next time, or pad out
        //      any final block at the end.
        //
        //  m_ac4H
        //      Message digest buffers
        //
        //  m_c4ByteCnt
        //      We have to remember the full count of bytes we have hashed,
        //      because it's used at the end.
        //
        //  m_c4PartialCnt
        //      The number of bytes in the m_ac1Partial buffer.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_ac1Partial[c4BlockSz];
        tCIDLib::TCard4 m_ac4H[c4BufCnt];
        tCIDLib::TCard4 m_c4ByteCnt;
        tCIDLib::TCard4 m_c4PartialCnt;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSHA1Hasher,TObject)
};

#pragma CIDLIB_POPPACK


