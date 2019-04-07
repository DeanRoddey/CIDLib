//
// FILE NAME: CIDCrypto_MsgHash.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/1997
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
//  This is the header for the CIDCrypto_MsgHash.cpp file. This file
//  implements the TMsgHash class, which is a base class from which specific
//  types of message hash classes are derived. This guy provides the grunt
//  work, and the derived class enforces some particular hash size.
//
//  The derived class sets the hash size during construction and it cannot
//  change, since most types of hashes have a particular size, e.g. MD5Hash.
//  So any attempt to set the hash after ctor must provide the expected
//  number of bytes.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TMsgHash
//  PREFIX: mhash
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TMsgHash :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TMsgHash();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TCard1& operator[]
        (
            const   tCIDLib::TCard4         c4ByteIndex
        );

        tCIDLib::TCard1 operator[]
        (
            const   tCIDLib::TCard4         c4ByteIndex
        )   const;

        TMsgHash& operator&=
        (
            const   TMsgHash&               mhashToAND
        );

        TMsgHash& operator|=
        (
            const   TMsgHash&               mhashToOR
        );

        TMsgHash& operator^=
        (
            const   TMsgHash&               mhashToXOR
        );


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid FormatToStr
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Bytes() const;

        tCIDLib::TCard4 c4ToBuffer
        (
                    TMemBuf&                mbufToFill
        )   const;

        tCIDLib::TCard4 c4ToRawArray
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
        )   const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;

        const tCIDLib::TCard1* pc1Buffer() const;

        tCIDLib::TVoid RawFormat
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1* const  pc1Bytes
            , const tCIDLib::TCard4         c4ProvidedBytes
        );

        tCIDLib::TVoid Set
        (
            const   TMemBuf&                mbufBytes
            , const tCIDLib::TCard4         c4ProvidedBytes
        );

        tCIDLib::TVoid Zero();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TMsgHash
        (
            const   tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Expected
        );

        TMsgHash
        (
            const   tCIDLib::TCard1* const  pc1Bytes
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Expected
        );

        TMsgHash
        (
            const   TMemBuf&                mbufBytes
            , const tCIDLib::TCard4         c4Expected
        );

        TMsgHash
        (
            const   TMsgHash&               mhashSrc
        );

        TMsgHash
        (
                    TMsgHash&&              mhashSrc
        );

        TMsgHash& operator=
        (
            const   TMsgHash&               mhashSrc
        );

        TMsgHash& operator=
        (
                    TMsgHash&&              mhashSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TMsgHash&               mhashSrc
        )   const;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1* pc1Writeable();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Hidden constructor, just for use in move ctor
        // -------------------------------------------------------------------
        TMsgHash();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Bytes
        //      The number of bytes in the hash
        //
        //  m_pc1Hash
        //      This is an array of bytes that make up the hash value. It is
        //      m_c4Bytes in size.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Bytes;
        tCIDLib::TCard1*    m_pc1Hash;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMsgHash,TObject)
};

#pragma CIDLIB_POPPACK


