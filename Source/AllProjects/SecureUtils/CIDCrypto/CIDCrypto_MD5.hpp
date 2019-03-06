//
// FILE NAME: CIDCrypto_MD5.hpp
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
//  This is the header for the CIDCrypto_MD5.cpp file. This file implements
//  the MD5 message digest function. It returns an MD5Hash object that holds
//  a message hash for the data hashed.
//
//  Required Notice:
//
//      This algorithm is made available by RSA Data Security as long as it
//      is identified as such. This notice is to indicate that this header
//      and its associated cpp file implement that RSA owned algorithm.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TMessageDigest5
//  PREFIX: mdig
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TMessageDigest5 : public THashDigest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMessageDigest5();

        TMessageDigest5(const TMessageDigest5&) = delete;

        ~TMessageDigest5();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMessageDigest5& operator=(const TMessageDigest5&) = delete;


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
        static constexpr tCIDLib::TCard4 c4HashBits    = kCIDCrypto::c4MD5HashBytes * 8;
        static constexpr tCIDLib::TCard4 c4HashBytes   = kCIDCrypto::c4MD5HashBytes;
        static constexpr tCIDLib::TCard4 c4StateLen    = 4;
        static constexpr tCIDLib::TCard4 c4CountLen    = 2;
        static constexpr tCIDLib::TCard4 c4BlockLen    = 64;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Complete
        (
                    tCIDLib::TCard1* const  pc1Digest
        );

        tCIDLib::TVoid Decode
        (
            const   tCIDLib::TCard1* const  pc1Input
            ,       tCIDLib::TCard4* const  pc4Output
            , const tCIDLib::TCard4         c4InputLen
        );

        tCIDLib::TVoid Encode
        (
            const   tCIDLib::TCard4* const  pc4Input
            ,       tCIDLib::TCard1* const  pc1Output
            , const tCIDLib::TCard4         c4InputLen
        );

        tCIDLib::TVoid InitContext();

        tCIDLib::TVoid ScrubContext();

        tCIDLib::TVoid Transform
        (
            const   tCIDLib::TCard1* const  pac1Block
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1Buffer
        //      This is a buffer used to hold a single block of data that
        //      is operated on at once.
        //
        //  m_ac4Count
        //      Number of bits in the message, modulo 2^64 (LSB first). So its
        //      a 64 bit number that indicates the length of the message in
        //      bits. Its updated as the message is processed and digested as
        //      part of the message at the end.
        //
        //  m_ac4State
        //      This is the state buffer that is used to maintain some
        //      context during the operation. It represents the ABCD variables
        //      of the MD5 algorithm.
        // -------------------------------------------------------------------
        tCIDLib::TCard1     m_ac1Buffer[c4BlockLen];
        tCIDLib::TCard4     m_ac4Count[c4CountLen];
        tCIDLib::TCard4     m_ac4State[c4StateLen];


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMessageDigest5,TObject)
};

#pragma CIDLIB_POPPACK


