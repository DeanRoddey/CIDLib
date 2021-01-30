//
// FILE NAME: CIDDAE_WaveEncoder.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
//  This file implements the TCIDDAEWaveEnc class. This class implements the
//  base encoder interface and supports ripping to the simple WAV format.
//
//  <TBD> We need to update this class to account for running on a big endian
//  system. It would be optimal if first we updated the binary stream classes to
//  allow the client code to indicate a specific endianness, and then just move
//  from using a header structure to streaming the values out to a stream set for
//  little endian.
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
//  CLASS: TCIDDAEWaveDec
// PREFIX: daed
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEWaveDec : public TCIDDAEDecoder
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strCodecName();
        static const TString& strFormatName();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDAEWaveDec();

        TCIDDAEWaveDec(const TCIDDAEWaveDec&) = delete;
        TCIDDAEWaveDec(TCIDDAEWaveDec&&) = delete;

        ~TCIDDAEWaveDec();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEWaveDec& operator=(const TCIDDAEWaveDec&) = delete;
        TCIDDAEWaveDec& operator=(TCIDDAEWaveDec&&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4LoadChunkImpl
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4StartInd = 0
        );

        tCIDLib::TVoid CleanupImpl();

        tCIDLib::TVoid EndDecodeImpl();

        tCIDLib::TVoid StartDecodeImpl
        (
            const   TString&                strSrcFile
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1 c1ReadNext();

        tCIDLib::TCard2 c2ReadNext();

        tCIDLib::TCard4 c4ReadNext();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SoFar
        //      The bytes we've read from the data chunk so far.
        //
        //  m_c4DataBytes
        //      The number of bytes in the data chunk, which is the part that
        //      we 'decode'. In reality we just hand it back as we read it.
        //
        //  m_flSrc
        //      The source file that we are reading from
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4SoFar;
        tCIDLib::TCard4 m_c4DataBytes;
        TBinaryFile     m_flSrc;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEWaveDec,TCIDDAEDecoder)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWaveEnc
// PREFIX:
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEWaveEnc : public TCIDDAEEncoder
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strCodecName();
        static const TString& strFormatName();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDAEWaveEnc();

        TCIDDAEWaveEnc(const TCIDDAEWaveEnc&) = delete;
        TCIDDAEWaveEnc(TCIDDAEWaveEnc&&) = delete;

        ~TCIDDAEWaveEnc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEWaveEnc& operator=(const TCIDDAEWaveEnc&) = delete;
        TCIDDAEWaveEnc& operator=(TCIDDAEWaveEnc&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCodecInfoValid
        (
            const   TString&                strCodec
            , const TString&                strFormat
        )   const;

        tCIDLib::TCard4 c4QueryCodecNames
        (
                    tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TCard4 c4QueryCodecFmtNames
        (
            const   TString&                strCodec
            ,       tCIDLib::TStrList&      colToFill
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CleanupImpl() override;

        tCIDLib::TVoid EndEncodeImpl() override;

        tCIDLib::TVoid StartEncodeImpl
        (
            const   TString&                strTargetFile
            , const TString&                strEncodingName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbumTitle
            , const TString&                strTrackTitle
            , const TString&                strArtist
            , const TString&                strLabel
            , const TString&                strGenre
            , const tCIDLib::TCard4         c4Year
        )   override;

        tCIDLib::TVoid StoreChunkImpl
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Bytes
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Total
        //      We keep up with how many data bytes we write out, so that
        //      we can go back later and update the header.
        //
        //  m_flTarget
        //      The target file that we are writing to.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Total;
        TBinaryFile     m_flTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEWaveEnc,TCIDDAEEncoder)
};

#pragma CIDLIB_POPPACK


