//
// FILE NAME: CIDLib_TextStringStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/03/1994
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
//  This file implements the TTextStringInStream and TTextStringOutStream
//  classes. These are convenience classes which derive from TTextInStream
//  and TTextOutStream, respectively. They just do the grunt work required to
//  set up a string based binary stream under the text stream. You could
//  easily do the same stuff, but these just make it more convient and more
//  self documenting.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextStringInStream,TTextInStream)
RTTIDecls(TTextStringOutStream,TTextOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TTextStringInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringStream: Constructors and operators
// ---------------------------------------------------------------------------
TTextStringInStream::TTextStringInStream(const  TString* const      pstrToUse
                                        , const tCIDLib::EAdoptOpts eAdopt) :

    TTextInStream(new TNativeWCConverter)
    , m_pstrmiIn(nullptr)
{
    // Create an impl object for the passed string and store it
    m_pstrmiIn = new TStringInStreamImpl(pstrToUse, eAdopt);

    //
    //  Create a binary stream and give it the input string stream impl to
    //  own. Give the new stream to our parent to own.
    //
    AdoptStream(new TBinInStream(m_pstrmiIn));
}

TTextStringInStream::TTextStringInStream(const TTextStringOutStream& strmToSyncWith) :

    TTextInStream(new TNativeWCConverter)
    , m_pstrmiIn(nullptr)
{
    // Create and store our implementation object
    m_pstrmiIn = new TStringInStreamImpl(*strmToSyncWith.m_pstrmiOut);

    // Now create the binary stream and give it to our parent
    AdoptStream(new TBinInStream(m_pstrmiIn));
}


TTextStringInStream::~TTextStringInStream()
{
    //
    //  Our parent class cleans up the binary stream object, and the binary
    //  stream owns the implementation object.
    //
}


// ---------------------------------------------------------------------------
//  TTextStringInStream: Public operators
// ---------------------------------------------------------------------------
TTextStringInStream::operator const TString&() const
{
    return m_pstrmiIn->strData();
}

// ---------------------------------------------------------------------------
//  TTextStringInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TTextStringInStream::strData() const
{
    return m_pstrmiIn->strData();
}



// ---------------------------------------------------------------------------
//   CLASS: TTextStringOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringStream: Constructors and operators
// ---------------------------------------------------------------------------
TTextStringOutStream::TTextStringOutStream(         TString* const      pstrToUse
                                            , const TStreamFmt&         strmfToUse
                                            , const tCIDLib::EAdoptOpts eAdopt
                                            , const tCIDLib::TBoolean   bAppend) :

    TTextOutStream(strmfToUse, new TNativeWCConverter)
    , m_pstrmiOut(nullptr)
{
    // We want the newline format to be just LF
    eNewLineType(tCIDLib::ENewLineTypes::LF);

    // Create an impl object for the passed string and store it
    m_pstrmiOut = new TStringOutStreamImpl(pstrToUse, eAdopt, bAppend);

    //
    //  Create a binary stream and give it the output string stream impl to
    //  own. Give the new stream to our parent to own.
    //
    AdoptStream(new TBinOutStream(m_pstrmiOut));
}

TTextStringOutStream::TTextStringOutStream(         TString* const      pstrToUse
                                            , const tCIDLib::EAdoptOpts eAdopt
                                            , const tCIDLib::TBoolean   bAppend) :

    TTextOutStream(new TNativeWCConverter)
    , m_pstrmiOut(nullptr)
{
    // We want the newline format to be just LF
    eNewLineType(tCIDLib::ENewLineTypes::LF);

    // Create an impl object for the passed string and store it
    m_pstrmiOut = new TStringOutStreamImpl(pstrToUse, eAdopt, bAppend);

    //
    //  Create a binary stream and give it the output string stream impl to
    //  own. Give the new stream to our parent to own.
    //
    AdoptStream(new TBinOutStream(m_pstrmiOut));
}

TTextStringOutStream::TTextStringOutStream( const   tCIDLib::TCard4 c4InitChars
                                            , const TStreamFmt&     strmfToUse) :

    TTextOutStream(strmfToUse, new TNativeWCConverter)
    , m_pstrmiOut(nullptr)
{
    // We want the newline format to be just LF
    eNewLineType(tCIDLib::ENewLineTypes::LF);

    //
    //  Create an impl object, with a new string object. It adopts by
    //  default, which is what we want.
    //
    m_pstrmiOut = new TStringOutStreamImpl(new TString(c4InitChars));

    //
    //  Create a binary stream and give it the output string stream impl to
    //  own. Give the new stream to our parent to own.
    //
    AdoptStream(new TBinOutStream(m_pstrmiOut));
}

TTextStringOutStream::TTextStringOutStream(const tCIDLib::TCard4 c4InitChars) :

    TTextOutStream(new TNativeWCConverter)
    , m_pstrmiOut(nullptr)
{
    // We want the newline format to be just LF
    eNewLineType(tCIDLib::ENewLineTypes::LF);

    //
    //  Create an impl object, with a new string object. It adopts by
    //  default, which is what we want.
    //
    m_pstrmiOut = new TStringOutStreamImpl(new TString(c4InitChars));

    //
    //  Create a binary stream and give it the output string stream impl to
    //  own. Give the new stream to our parent to own.
    //
    AdoptStream(new TBinOutStream(m_pstrmiOut));
}

TTextStringOutStream::~TTextStringOutStream()
{
    //
    //  Our parent class cleans up the binary stream object, and the binary
    //  stream owns the implementation object.
    //
}



// ---------------------------------------------------------------------------
//  TTextStringOutStream: Public operators
// ---------------------------------------------------------------------------
TTextStringOutStream::operator const TString&() const
{
    return m_pstrmiOut->strData();
}


// ---------------------------------------------------------------------------
//  TTextStringOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TTextStringOutStream::strData() const
{
    return m_pstrmiOut->strData();
}


