//
// FILE NAME: CIDCrypto_BlockEncrypt.hpp
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
//  This file implements the abstract base class TBlockEncrypter, which is
//  the base abstraction for all block based encryption schemes. This allows
//  many different block encryption schemes to be manipulated via this basic
//  interface.
//
//  Actually most of the work is done here. The derived classes just have
//  to provide the basic ability to encrypt/decrypt blocks of data upon our
//  request. We handle the various block encryption modes for them. This is
//  slower, but drastically reduces the likelihood of error.
//
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
//   CLASS: TBlockEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TBlockEncrypter : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBlockEncrypter() = delete;

        TBlockEncrypter(const TBlockEncrypter&) = delete;

        ~TBlockEncrypter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBlockEncrypter& operator=(const TBlockEncrypter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BlockSize() const
        {
            return m_c4BlockSize;
        }

        tCIDLib::TCard4 c4Decrypt
        (
            const   TMemBuf&                mbufCypher
            ,       TMemBuf&                mbufPlain
            , const tCIDLib::TCard4         c4CypherBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Decrypt
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            ,       TMemBuf&                mbufPlain
            , const tCIDLib::TCard4         c4CypherBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Decrypt
        (
            const   TMemBuf&                mbufCypher
            ,       TString&                strPlain
            , const tCIDLib::TCard4         c4CypherBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Decrypt
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            ,       tCIDLib::TCard1* const  pc1Plain
            , const tCIDLib::TCard4         c4CypherBytes
            , const tCIDLib::TCard4         c4MaxOutBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Encrypt
        (
            const   TMemBuf&                mbufPlain
            ,       TMemBuf&                mbufCypher
            , const tCIDLib::TCard4         c4PlainBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Encrypt
        (
            const   tCIDLib::TCard1* const  pc1Plain
            ,       TMemBuf&                mbufCypher
            , const tCIDLib::TCard4         c4PlainBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Encrypt
        (
            const   TString&                strPlain
            ,       TMemBuf&                mbufCypher
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDLib::TCard4 c4Encrypt
        (
            const   tCIDLib::TCard1* const  pc1Plain
            ,       tCIDLib::TCard1* const  pc1Cypher
            , const tCIDLib::TCard4         c4PlainBytes
            , const tCIDLib::TCard4         c4MaxOutBytes
            , const tCIDLib::TCard1* const  pc1IV = nullptr
        );

        tCIDCrypto::EBlockModes eMode() const;

        tCIDCrypto::EBlockModes eMode
        (
            const   tCIDCrypto::EBlockModes eNewMode
        );

        tCIDLib::TVoid Reset();


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TBlockEncrypter
        (
            const   tCIDLib::TCard4         c4BlockSize
            , const tCIDCrypto::EBlockModes eMode
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DecryptImpl
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            ,       tCIDLib::TCard1* const  pc1Plain
        ) = 0;

        virtual tCIDLib::TVoid EncryptImpl
        (
            const   tCIDLib::TCard1* const  pc1Plain
            ,       tCIDLib::TCard1* const  pc1CypherBuf
        ) = 0;

        virtual tCIDLib::TVoid ResetImpl() = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ECBDecrypt
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            , const tCIDLib::TCard4         c4CypherBytes
            ,       TMemBuf&                mbufPlainBuf
        );

        tCIDLib::TCard4 c4ECBEncrypt
        (
            const   tCIDLib::TCard1* const  pc1Plain
            , const tCIDLib::TCard4         c4PlainBytes
            ,       TMemBuf&                mbufCypherBuf
        );

        tCIDLib::TCard4 c4CBCDecrypt
        (
            const   tCIDLib::TCard1* const  pc1Cypher
            , const tCIDLib::TCard4         c4CypherBytes
            ,       TMemBuf&                mbufPlainBuf
            , const tCIDLib::TCard1* const  pac1IV
        );

        tCIDLib::TCard4 c4CBCEncrypt
        (
            const   tCIDLib::TCard1* const  pc1Plain
            , const tCIDLib::TCard4         c4PlainBytes
            ,       TMemBuf&                mbufCypherBuf
            , const tCIDLib::TCard1* const  pc1IV
        );

        tCIDLib::TCard4 c4OFBProcess
        (
            const   tCIDLib::TCard1* const  pc1Input
            , const tCIDLib::TCard4         c4InputBytes
            ,       TMemBuf&                mbufOut
            , const tCIDLib::TCard1* const  pc1IV
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BlockSize
        //      The block size used by the derived class. It is passed to the
        //      protected constructor by the derived class and we just store
        //      it and provide access to it.
        //
        //  m_eMode
        //      The block encryption mode to use.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4BlockSize;
        tCIDCrypto::EBlockModes m_eMode;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBlockEncrypter,TObject)
};

#pragma CIDLIB_POPPACK


