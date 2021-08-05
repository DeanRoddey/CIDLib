//
// FILE NAME: CIDAudStream_TarStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the base class that defined a output (target) audio stream. We assume that
//  the data is PCM sample data.
//
//  bWriteBytes returns the bytes it was able to write. The caller must pass the remainder
//  back in again next time. If there's partial data (in terms of its itnernal implementation
//  buffers) it'll store it and feed it out next time.
//
//  Always call FlushBufs at the end of input to wait for any final buffers to be
//  transmitted and for any final partial buffer (it'll be padded with silence internally
//  as required.)
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
//   CLASS: TCIDAudioTarStream
//  PREFIX: cass
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDAudioTarStream : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAudioTarStream();

        TCIDAudioTarStream(const TCIDAudioTarStream&) = delete;
        TCIDAudioTarStream(TCIDAudioTarStream&&) = delete;

        virtual ~TCIDAudioTarStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAudioTarStream& operator=(const TCIDAudioTarStream&) = delete;
        TCIDAudioTarStream& operator=(TCIDAudioTarStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bWriteBytes
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            , const tCIDLib::TCard4         c4WaitMSs
            ,       tCIDLib::TCard4&        c4BytesWritten
        )   = 0;

        virtual tCIDLib::TVoid Initialize()   = 0;

        virtual tCIDLib::TVoid FlushBufs() = 0;

        virtual tCIDLib::TVoid QueryFormat
        (
            COP     tCIDLib::TCard4&        c4Channels
            , COP   tCIDLib::TCard4&        c4SamplesPerSec
            , COP   tCIDLib::TCard4&        c4BytesPerSample
        )   const = 0;

        virtual tCIDLib::TVoid Terminate() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDAudioTarStream, TObject)
};

#pragma CIDLIB_POPPACK
