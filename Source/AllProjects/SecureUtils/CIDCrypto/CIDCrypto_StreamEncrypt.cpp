//
// FILE NAME: CIDCrypto_StreamEncrypt.cpp
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
//  This file implements the TStreamEncrypter class. This is an abstract base
//  class interface for all stream oriented encryption schemes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDCrypto_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TStreamEncrypter,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TStreamEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStreamEncrypter: Constructors and Destructor
// ---------------------------------------------------------------------------
TStreamEncrypter::TStreamEncrypter(         TBinInStream* const  pstrmIn
                                    ,       TBinOutStream* const pstrmOut
                                    , const tCIDLib::EAdoptOpts  eAdopt) :
    m_eAdopt(eAdopt)
    , m_pstrmIn(pstrmIn)
    , m_pstrmOut(pstrmOut)
{
}

TStreamEncrypter::~TStreamEncrypter()
{
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
    {
        delete m_pstrmIn;
        delete m_pstrmOut;
    }
}


// ---------------------------------------------------------------------------
//  TStreamEncrypter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TStreamEncrypter::Decrypt()
{
    // Call the derived class
    DecryptImpl();
}


tCIDLib::TVoid TStreamEncrypter::Encrypt()
{
    // Call the derived class
    EncryptImpl();
}


tCIDLib::TVoid TStreamEncrypter::Reset()
{
    // Reset the both the streams
    m_pstrmIn->Reset();
    m_pstrmOut->Reset();

    // And call the virtual method to reset any context of the derived class
    ResetImpl();
}


TBinInStream& TStreamEncrypter::strmIn()
{
    return *m_pstrmIn;
}


TBinOutStream& TStreamEncrypter::strmOut()
{
    return *m_pstrmOut;
}


