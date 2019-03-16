//
// FILE NAME: CIDCrypto_StreamEncrypt.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/21/1998
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
//  This file implements the abstract base class TStreamEncrypter, which is
//  the base abstraction for all stream based encryption schemes. This allows
//  many different Stream encryption schemes to be manipulated via this basic
//  interface.
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
//   CLASS: TStreamEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TStreamEncrypter : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamEncrypter
        (
                    TBinInStream* const     pstrmIn
            ,       TBinOutStream* const    pstrmOut
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TStreamEncrypter(const TStreamEncrypter&) = delete;

        ~TStreamEncrypter();


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TStreamEncrypter& operator=(const TStreamEncrypter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Decrypt();

        tCIDLib::TVoid Encrypt();

        tCIDLib::TVoid Reset();

        TBinInStream& strmIn();

        TBinOutStream& strmOut();


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TStreamEncrypter();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DecryptImpl() = 0;

        virtual tCIDLib::TVoid EncryptImpl() = 0;

        virtual tCIDLib::TVoid ResetImpl() = 0;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      Indicates whether we have adopted the streams or not. If so
        //      then we delete them when we are destroyed.
        //
        //  m_pstrmIn
        //  m_pstrmOut
        //      These are the pointers to the streams that we are managing.
        //      When Decrypt() or Encrypt() is called, data is read from the
        //      in stream and written to the out stream.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        TBinInStream*       m_pstrmIn;
        TBinOutStream*      m_pstrmOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStreamEncrypter,TObject)
};

#pragma CIDLIB_POPPACK


