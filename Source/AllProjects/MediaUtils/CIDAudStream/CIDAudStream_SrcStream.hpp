//
// FILE NAME: CIDAudStream_rcStream.hpp
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
//  This is the base class that defined a input (source) audio stream. It defines a simple
//  interface that should suit most purposes. Where something more is required then one
//  of these can be encapsulated. We assume that the data is PCM of some sort.
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
//   CLASS: TCIDAudioSrcStream
//  PREFIX: cass
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDAudioSrcStream
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAudioSrcStream();

        TCIDAudioSrcStream(const TCIDAudioSrcStream&) = delete;

        virtual ~TCIDAudioSrcStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAudioSrcStream& operator=(const TCIDAudioSrcStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bReadBytes
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4WaitMSs
        )   = 0;

        virtual tCIDLib::TVoid Initialize()   = 0;

        virtual tCIDLib::TVoid FlushBufs() = 0;

        virtual tCIDLib::TVoid QueryFormat
        (
                    tCIDLib::TCard4&        c4Channels
            ,       tCIDLib::TCard4&        c4SamplesPerSec
            ,       tCIDLib::TCard4&        c4BytesPerSample
        )   const = 0;

        virtual tCIDLib::TVoid Terminate() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDAudStream::EStrmStates eState() const;

        tCIDAudStream::EStrmStates eState
        (
            const   tCIDAudStream::EStrmStates eState
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eState
        //      The derived class sets this to let us know what state he is in and
        //      we make it available
        // -------------------------------------------------------------------
        tCIDAudStream::EStrmStates m_eState;
};

#pragma CIDLIB_POPPACK
