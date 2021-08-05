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
//  We also define a simple class that is used to return information about the available
//  streams (of whatever type are being queried.) It includes a platform specific encoded
//  identifier in text form. Each platform can use that however needed to get back to the
//  indicated stream.
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
//   CLASS: TCIDAudioStreamInfo
//  PREFIX: asinfo
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDAudioStreamInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAudioStreamInfo() = default;

        TCIDAudioStreamInfo
        (
            const   TString&                strName
            , const TString&                strId
        );

        TCIDAudioStreamInfo(const TCIDAudioStreamInfo&) = default;
        TCIDAudioStreamInfo(TCIDAudioStreamInfo&&) = default;

        ~TCIDAudioStreamInfo() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAudioStreamInfo& operator=(const TCIDAudioStreamInfo&) = default;
        TCIDAudioStreamInfo& operator=(TCIDAudioStreamInfo&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strId() const
        {
            return m_strId;
        }

        const TString& strName() const
        {
            return m_strName;
        }

        tCIDLib::TVoid Set
        (
            const   TString&                strName
            , const TString&                strId
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strId
        //      A platform specific id to let us find the stream again.
        //
        //  m_strName
        //      The human readable name for the stream.
        // -------------------------------------------------------------------
        TString             m_strId;
        TString             m_strName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDAudioStreamInfo, TObject)
};
using TCIDAudioStreamList = TVector<TCIDAudioStreamInfo>;


// ---------------------------------------------------------------------------
//   CLASS: TCIDAudioSrcStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDAudioSrcStream : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAudioSrcStream();

        TCIDAudioSrcStream(const TCIDAudioSrcStream&) = delete;
        TCIDAudioSrcStream(TCIDAudioSrcStream&&) = delete;

        ~TCIDAudioSrcStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAudioSrcStream& operator=(const TCIDAudioSrcStream&) = delete;
        TCIDAudioSrcStream& operator=(TCIDAudioSrcStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bReadBytes
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , COP   tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4WaitMSs
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
        [[nodiscard]] tCIDAudStream::EStrmStates eState() const;

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


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDAudioSrcStream, TObject)
};

#pragma CIDLIB_POPPACK
