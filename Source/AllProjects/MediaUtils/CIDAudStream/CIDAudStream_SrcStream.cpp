//
// FILE NAME: CIDAudStream_SrcStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the wee bit of non-virtual functionality of our base
//  audio source stream.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDAudStream_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TCIDAudioSrcStream
//  PREFIX: audss
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TCIDAudioSrcStream: Constructors and Destructor
// -------------------------------------------------------------------
TCIDAudioSrcStream::TCIDAudioSrcStream() :

    m_eState(tCIDAudStream::EStrmStates::WaitConnect)
{
}

TCIDAudioSrcStream::~TCIDAudioSrcStream()
{
}


// -------------------------------------------------------------------
//  TCIDAudioSrcStream: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDAudStream::EStrmStates TCIDAudioSrcStream::eState() const
{
    return m_eState;
}

tCIDAudStream::EStrmStates
TCIDAudioSrcStream::eState(const tCIDAudStream::EStrmStates eState)
{
    m_eState = eState;
    return m_eState;
}

