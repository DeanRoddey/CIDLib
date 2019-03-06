//
// FILE NAME: CIDCrypto_Hashers.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/21/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDCrypto_Hasher.cpp file. This file implements
//  the THashDigest class, which is the abstract base from which all the
//  hashers are derived.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: THashDisgest
//  PREFIX: mdig
// ---------------------------------------------------------------------------
class CIDCRYPTEXP THashDigest : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        THashDigest(const THashDigest&) = delete;

        ~THashDigest();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THashDigest& operator=(const THashDigest&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Complete
        (
                    TMsgHash&               mhashToFill
        ) = 0;

        virtual tCIDLib::TVoid DigestSrc
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard4         c4Count
        ) = 0;

        virtual tCIDLib::TVoid DigestRaw
        (
            const   tCIDLib::TCard1* const  pc1ToDigest
            , const tCIDLib::TCard4         c4Bytes
        ) = 0;

        virtual tCIDLib::TVoid StartNew() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        //
        //  These all work in terms of the two virtual digest methods above.
        //
        //  NOTE, STUPID! After removing the seemingly redundant method below
        //  3 times only to have to put it back, I've left myself a note.
        //  There has to be one version that takes a byte count and one does
        //  not. Defaulting the byte count to zero to use the actual size of
        //  the buffer won't work because zero is a valid length!
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BlockSize() const;

        tCIDLib::TVoid DigestText
        (
            const   tCIDLib::TCh* const     pszToDigest
            , const tCIDLib::TCard4         c4Length = 0
        );

        tCIDLib::TVoid DigestStr
        (
            const   TString&                strToDigest
        );

        tCIDLib::TVoid DigestBuf
        (
            const   TMemBuf&                mbufToDigest
        );

        tCIDLib::TVoid DigestBuf
        (
            const   TMemBuf&                mbufToDigest
            , const tCIDLib::TCard4         c4Bytes
        );



    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        THashDigest
        (
            const   tCIDLib::TCard4         c4BlockSize
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BlockSize
        //      The derived class tells us the block size. Some things, outside of
        //      the actual hasher needs to know this. So we make it available via a
        //      public method.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4BlockSize;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THashDigest,TObject)
};

#pragma CIDLIB_POPPACK


