//
// FILE NAME: CIDCrypto_Xor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This module implements some very simple XOR based encrypters. These are
//  provided just for completion. Xor is not a real encryption mechanism,
//  but is often used just to be discreet and avoid offense and such.
//
//  These is a block based and stream based version provided, so that either
//  encryption style can be handled. Because this encryption method is so
//  simple, there is no need to provide any implementation class that provides
//  common encryption services to both versions.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TXorBlockEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TXorBlockEncrypter : public TBlockEncrypter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TXorBlockEncrypter() = delete;

        TXorBlockEncrypter
        (
            const   TCryptoKey&             ckeyToUse
            , const tCIDCrypto::EBlockModes eMode = tCIDCrypto::EBlockModes::ECB
        );

        TXorBlockEncrypter(const TXorBlockEncrypter&) = delete;

        ~TXorBlockEncrypter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXorBlockEncrypter& operator=(const TXorBlockEncrypter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCryptoKey& ckeyThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DecryptImpl
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            ,       tCIDLib::TCard1* const  pc1PlainBuf
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
        //  m_c4CurKeyByte
        //      This is used to rotate through the key bytes.
        //
        //  m_ckeyThis
        //      The key that is used for this encrypter.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4CurKeyByte;
        TCryptoKey      m_ckeyThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXorBlockEncrypter,TBlockEncrypter)
};


// ---------------------------------------------------------------------------
//   CLASS: TXorStreamEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TXorStreamEncrypter : public TStreamEncrypter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TXorStreamEncrypter() = delete;

        TXorStreamEncrypter
        (
            const   TCryptoKey&             ckeyTouse
            ,       TBinInStream* const     pstrmIn
            ,       TBinOutStream* const    pstrmOut
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TXorStreamEncrypter(const TXorStreamEncrypter&) = delete;

        ~TXorStreamEncrypter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXorStreamEncrypter& operator=(const TXorStreamEncrypter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCryptoKey& ckeyThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DecryptImpl() override;

        tCIDLib::TVoid EncryptImpl() override;

        tCIDLib::TVoid ResetImpl() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurKeyByte
        //      This is used to rotate through the key bytes.
        //
        //  m_ckeyThis
        //      The key that is used for this encrypter.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4CurKeyByte;
        TCryptoKey      m_ckeyThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXorStreamEncrypter,TStreamEncrypter)
};

#pragma CIDLIB_POPPACK


