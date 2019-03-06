//
// FILE NAME: CIDCrypto_SHA1Hash.hpp
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
//  This is the header for the CIDCrypto_SHA1Hash.cpp file. This file
//  implements the TSHA1Hash class. This is a derivative of the base message
//  hash class, specialized for SHA1 hashes. Basically, we just have to
//  enforce the size of our type of hashes, which is 20 bytes.
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
//   CLASS: TSHA1Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA1Hash : public TMsgHash
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSHA1Hash();

        TSHA1Hash
        (
            const   tCIDLib::TCard1* const  pc1Bytes
            , const tCIDLib::TCard4         c4Bytes
        );

        TSHA1Hash
        (
            const   TMemBuf&                mbufBytes
        );

        TSHA1Hash(const TSHA1Hash&) = default;

        TSHA1Hash
        (
                    TSHA1Hash&&             mhashSrc
        );

        ~TSHA1Hash();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSHA1Hash& operator=(const TSHA1Hash&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TSHA1Hash&              mhashSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSHA1Hash&              mhashSrc
        )   const;

        TSHA1Hash& operator=
        (
                    TSHA1Hash&&             mhashrRc
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatToStr
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const override;


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
        RTTIDefs(TSHA1Hash,TMsgHash)
        DefPolyDup(TSHA1Hash)
};



// ---------------------------------------------------------------------------
//   CLASS: TSHA1KeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TSHA1KeyOps : public TObject, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSHA1KeyOps();

        TSHA1KeyOps(const TSHA1KeyOps&) = default;

        ~TSHA1KeyOps();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSHA1KeyOps& operator=(const TSHA1KeyOps&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys(const   TSHA1Hash& mhash1
                                    , const TSHA1Hash& mhash2) const
        {
            return (mhash1 == mhash2);
        }

        tCIDLib::THashVal hshKey
        (
            const   TSHA1Hash&              mhashToHash
            , const tCIDLib::TCard4         c4Modulus
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TSHA1KeyOps, TObject)
        DefPolyDup(TSHA1KeyOps)
};


#pragma CIDLIB_POPPACK


