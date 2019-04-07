//
// FILE NAME: CIDCrypto_Blowfish.hpp
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
//  TBlowfishEncrypter is a derivative of the standard encryption class that
//  implements the (suprise) Blowfish encryption method. This is a public
//  domain encryption scheme designed by Bruce Schneier, the author of the
//  book "Applied Cryptography".
//
//  Note that this encrypter is reentrant, since it sets up a set of tables
//  during init and after that no member data is changed during encryption
//  and decryption, the same applies to the parent block encrypter class,
//  so multiple threads can use an encrypter at the same time.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBlowfishEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TBlowfishEncrypter : public TBlockEncrypter
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4    c4MinKeyBits = 8;
        static constexpr tCIDLib::TCard4    c4MaxKeyBits = 448;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBlowfishEncrypter
        (
            const   tCIDCrypto::EBlockModes eMode = tCIDCrypto::EBlockModes::ECB
        );

        TBlowfishEncrypter
        (
            const   TCryptoKey&             ckeyToUse
            , const tCIDCrypto::EBlockModes eMode = tCIDCrypto::EBlockModes::ECB
        );

        TBlowfishEncrypter(const TBlowfishEncrypter&) = delete;

        ~TBlowfishEncrypter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBlowfishEncrypter& operator=(const TBlowfishEncrypter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCryptoKey& ckeyThis() const;

        tCIDLib::TVoid SetNewKey
        (
            const   TCryptoKey&             ckeyNew
            , const tCIDCrypto::EBlockModes eNewMode = tCIDCrypto::EBlockModes::ECB
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DecryptImpl
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            ,       tCIDLib::TCard1* const  pc1Plain
        )   override;

        tCIDLib::TVoid EncryptImpl
        (
            const   tCIDLib::TCard1* const  pc1Plain
            ,       tCIDLib::TCard1* const  pc1CypherBuf
        )   override;

        tCIDLib::TVoid ResetImpl() override;


    private :
        // -------------------------------------------------------------------
        //  Private constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4NumSBoxes    = 4;
        static const tCIDLib::TCard4 c4SBoxSize     = 256;
        static const tCIDLib::TCard4 c4Rounds       = 16;
        static const tCIDLib::TCard4 c4PArraySize   = c4Rounds + 2;
        static const tCIDLib::TCard4 c4BlockBits    = 64;
        static const tCIDLib::TCard4 c4BlockBytes   = 8;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Feistel
        (
            const   tCIDLib::TCard4         c4ToUse
        );

        tCIDLib::TVoid DecryptBlock
        (
                    tCIDLib::TCard4&        c4Left
            ,       tCIDLib::TCard4&        c4Right
        );

        tCIDLib::TVoid EncryptBlock
        (
                    tCIDLib::TCard4&        c4Left
            ,       tCIDLib::TCard4&        c4Right
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac4PArray
        //      The permutation data for the key we are using. It is set up
        //      during construction and used for the life of this encrypter
        //      object.
        //
        //  m_ac4SBox
        //      The S-Box data for the key we are using. Its set up during
        //      construction and used for the life of this encrypter object.
        //
        //  m_ckeyThis
        //      This is the key being used for encryption/decryption in
        //      this object.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_ac4PArray[c4PArraySize];
        tCIDLib::TCard4     m_ac4SBox[c4NumSBoxes][c4SBoxSize];
        TCryptoKey          m_ckeyThis;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBlowfishEncrypter,TBlockEncrypter)
};

#pragma CIDLIB_POPPACK

