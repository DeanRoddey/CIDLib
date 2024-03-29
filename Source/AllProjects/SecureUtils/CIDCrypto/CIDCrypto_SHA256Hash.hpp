//
// FILE NAME: CIDCrypto_SHA256Hash.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/2017
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
//  This is the header for the CIDCrypto_SHA256Hash.cpp file. This file
//  implements the TSHA256Hash class. This is a derivative of the base message
//  hash class, specialized for SHA256 hashes. Basically, we just have to
//  enforce the size of our type of hashes, which is 32 bytes.
//
//  Since this will commonly be used as a key in a collection, we provide a
//  key ops class for it.
//
//  Note the raw data is stored big endian, i.e. the 32 bit values are
//  just put end to end in the buffer in big endian format. It's assumed that
//  incoming raw buffers used to set a hash will be in that format.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TSHA256Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA256Hash : public TMsgHash
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSHA256Hash();

        TSHA256Hash
        (
            const   tCIDLib::TCard1* const  pc1Bytes
            , const tCIDLib::TCard4         c4Bytes
        );

        TSHA256Hash
        (
            const   TMemBuf&                mbufBytes
        );

        TSHA256Hash(const TSHA256Hash&) = default;
        TSHA256Hash(TSHA256Hash&&) = default;

        ~TSHA256Hash();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSHA256Hash& operator=(const TSHA256Hash&) = default;
        TSHA256Hash& operator=(TSHA256Hash&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TSHA256Hash&            mhashSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSHA256Hash&            mhashSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatToStr
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseFormatted
        (
            const   TString&                strFormatted
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSHA256Hash,TMsgHash)
        DefPolyDup(TSHA256Hash)
};



// ---------------------------------------------------------------------------
//   CLASS: TSHA256KeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA256KeyOps
{
    public :
        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys(const   TSHA256Hash& mhash1
                                    , const TSHA256Hash& mhash2) const
        {
            return (mhash1 == mhash2);
        }

        tCIDLib::THashVal hshKey(const  TSHA256Hash&    mhashToHash
                                , const tCIDLib::TCard4 c4Modulus) const
        {
            return mhashToHash.hshCalcHash(c4Modulus);
        }
};

#pragma CIDLIB_POPPACK


