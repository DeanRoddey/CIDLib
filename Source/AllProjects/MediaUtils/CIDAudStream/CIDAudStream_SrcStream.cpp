//
// FILE NAME: CIDAudStream_SrcStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDAudioSrcStream, TObject)


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

