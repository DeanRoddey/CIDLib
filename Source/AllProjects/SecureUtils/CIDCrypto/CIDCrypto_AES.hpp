//
// FILE NAME: CIDCrypto_AES.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/2005
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  TAESEncrypter is a derivative of the standard encryption class that
//  implements the AES encryption standard. AES is a bit swizzling block
//  encryptor, that uses tables and the key bits to mangle the input. It
//  uses a 16 byte block.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAESEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TAESEncrypter : public TBlockEncrypter
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4    c4MinKeyBits = 128;
        static constexpr tCIDLib::TCard4    c4MaxKeyBits = 256;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAESEncrypter
        (
            const   tCIDCrypto::EBlockModes eMode = tCIDCrypto::EBlockModes::ECB
        );

        TAESEncrypter
        (
            const   TCryptoKey&             ckeyToUse
            , const tCIDCrypto::EBlockModes eMode = tCIDCrypto::EBlockModes::ECB
        );

        TAESEncrypter(const TAESEncrypter&) = delete;

        ~TAESEncrypter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAESEncrypter& operator=(const TAESEncrypter&) = delete ;


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
        //  Private data members
        //
        //  m_ac4DRK
        //  m_ac4ERK
        //      The decryption and encryption round keys, respectively. These
        //      are the tables that are set up based on the key, so these are
        //      instance specific data. The other stuff is not instance
        //      specific and is all internal to the CPP file.
        //
        //  m_c4Rounds
        //      The number of rounds we are doing. The number of rounds is
        //      driven by the key length.
        //
        //  m_ckeyThis
        //      This is the key being used for encryption/decryption in
        //      this object.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_ac4DRK[64];
        tCIDLib::TCard4     m_ac4ERK[64];
        tCIDLib::TCard4     m_c4Rounds;
        TCryptoKey          m_ckeyThis;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAESEncrypter,TBlockEncrypter)
};

#pragma CIDLIB_POPPACK


