//
// FILE NAME: CIDCrypto_Key.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/1997
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
//  TCryptoKey is a small class that is designed to make dealing with
//  decryption keys more safe and convenient. Keys can be ASCII text, Unicode
//  text, or binary data, so this class handles making all of those key
//  types look the same way to the encryption classes. And it deals with
//  issues of key length and such. Each keyed encryption class can create
//  a nested derivative of this class that enforce its key rules.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMsgHash;

// ---------------------------------------------------------------------------
//   CLASS: TCryptoKey
//  PREFIX: ckey
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TCryptoKey :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TCryptoKey
        (
            const   tCIDLib::TCard4         c4KeyBytes = 16
        );

        TCryptoKey
        (
            const   TString&                strKey
        );

        TCryptoKey
        (
            const   TMsgHash&               mhashBytes
        );

        TCryptoKey
        (
            const   TMemBuf&                mbufKey
            , const tCIDLib::TCard4         c4KeyBytes = 0
        );

        TCryptoKey
        (
            const   tCIDLib::TCard1* const  apc1Key
            , const tCIDLib::TCard4         c4KeyBytes
        );

        TCryptoKey
        (
            const   TCryptoKey&             ckeySrc
        );

        TCryptoKey
        (
                    TCryptoKey&&            ckeySrc
        );

        ~TCryptoKey();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TCryptoKey&             ckeySrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TCryptoKey&             ckeySrc
        )   const;

        TCryptoKey& operator=
        (
            const   TCryptoKey&             ckeySrc
        );

        TCryptoKey& operator=
        (
                    TCryptoKey&&            ckeySrc
        );

        tCIDLib::TCard1 operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard1& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        );

        TCryptoKey& operator&=
        (
            const   TCryptoKey&             ckeyToAND
        );

        TCryptoKey& operator|=
        (
            const   TCryptoKey&             ckeyToOR
        );

        TCryptoKey& operator^=
        (
            const   TCryptoKey&             ckeyToXOR
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Bytes() const;

        tCIDLib::TVoid FormatKey
        (
                    TString&                strToFill
            , const tCIDLib::TCh            chSepChar = kCIDLib::chNull
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        const tCIDLib::TCard1* pc1Buffer() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid Set
        (
            const   TMemBuf&                mbufKeyBytes
            , const tCIDLib::TCard4         c4KeyBytes = 0
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1* const  apc1KeyBytes
            , const tCIDLib::TCard4         c4KeyBytes
        );

        tCIDLib::TVoid Set
        (
            const   TMsgHash&               mhashBytes
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strKey
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Bytes
        //      The number of bytes in the key array.
        //
        //  m_pc1Buffer
        //      A pointer to an array of data bytes that make up the key.
        //      It is allocated and the data copied from the provided key
        //      data in the constructor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Bytes;
        tCIDLib::TCard1*    m_pc1Buffer;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCryptoKey,TObject)
        DefPolyDup(TCryptoKey)
};

#pragma CIDLIB_POPPACK


