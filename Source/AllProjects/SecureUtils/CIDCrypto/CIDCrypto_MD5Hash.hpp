//
// FILE NAME: CIDCrypto_MD5Hash.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/2001
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
//  This is the header for the CIDCrypto_MD5Hash.cpp file. This file
//  implements the TMD5Hash class. This is a derivative of the base message
//  hash class, specialized for MD5 hashes. Basically, we just have to enforce
//  the size of our type of hashes, which is 16 bytes.
//
//  Since this will commonly be used as a key in a collection, we provide a
//  key ops class for it.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TMD5Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TMD5Hash : public TMsgHash
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMD5Hash();

        TMD5Hash
        (
            const   tCIDLib::TCard1* const  pc1Bytes
            , const tCIDLib::TCard4         c4Bytes
        );

        TMD5Hash
        (
            const   TMemBuf&                mbufBytes
        );

        TMD5Hash(const TMD5Hash&) = default;
        TMD5Hash(TMD5Hash&&) = default;

        ~TMD5Hash();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMD5Hash& operator=(const TMD5Hash&) = default;
        TMD5Hash& operator=(TMD5Hash&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TMD5Hash&               mhashSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMD5Hash&               mhashSrc
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
        )   const   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMD5Hash,TMsgHash)
        DefPolyDup(TMD5Hash)
};



// ---------------------------------------------------------------------------
//   CLASS: TMD5KeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TMD5KeyOps
{
    public :
        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys(const   TMD5Hash& mhash1
                                    , const TMD5Hash& mhash2) const
        {
            return (mhash1 == mhash2);
        }

        tCIDLib::THashVal hshKey(   const   TMD5Hash&       mhashToHash
                                    , const tCIDLib::TCard4 c4Modulus) const
        {
            return mhashToHash.hshCalcHash(c4Modulus);
        }
};

#pragma CIDLIB_POPPACK

