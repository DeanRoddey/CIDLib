//
// FILE NAME: CIDDAE_Codec.hpp
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
//  This file defineds the base encoder and decoder interfaces. All of the
//  encoder and decoder classes implement this interface. Encoders always
//  write to a file, instead of using the generic binary output stream
//  interface, because we cannot guarantee that a particular format won't
//  require random access.
//
//  The derived encoder classes are still not aware of this. The base class
//  just provides them with read/write methods, which the base class calls.
//  Because of the nature of this data, these read/write methods consider
//  any failure to read or write all the indicated bytes to be a fatal error.
//
//  Decoders just decode to a memory buffer in chunks, because they are
//  generally used in players to decode to raw WAV format. They just ask
//  for chunks at a time to be decoded. It can never know how much it'll
//  get because it doesn't know how compressed any particular next chunk
//  of source content is.
//
//  And finally we define a small class that is used for the read/write
//  buffers during encoding and decoding. They provide the storage for the
//  data plus a little housekeeping data. The ripper class, for instance,
//  creates a pool of these that it's spooling threads use.
//
//  The encoder and decoder share a common base class which provides a
//  few virtuals that would apply to the codec in general, not just to
//  the encoding or decoding sides.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TKrnlRemMediaDrv;


// ---------------------------------------------------------------------------
//  CLASS: TDAEBuf
// PREFIX: daeb
//
//  For each buffer (except the possible last partial one), we read in a
//  fixed number of blocks. When no jitter correction is on, the next buffer
//  will start at block SP+NP, the previous start block plus the previous
//  number of blocks read. This is pretty obvious, each buffer holds the next
//  N number of blocks.
//
//  If we get less than the requested number of bytes, then we retry the read
//  until we get the requested bytes or we use up our retries and fail the
//  rip session.
//
//  So we read the buffer, m_c4BytesRead is set to the bytes read, m_c4StartBlk
//  is set to the starting block, m_c4EndBlk is set to the end block, and
//  m_pc1StartPtr is set to the start of the buffer (i.e. ==m_pc1Buf.)
//
//  If jitter correction is on, then things are different. When doing jitter
//  correction, each new buffer starts at SP+NP-O, where O is a fixed overlap
//  number, usually 2. So we read O blocks before where we'd read if we were
//  doing strict end to end buffers.
//
//  The bytes read indicates the full bytes including overlap. The end block
//  is set to S+(N-1), which is likely going to be right, but we may have to
//  adjust it downwards if we got a bad jitter error in the direction of the
//  current buffer having started too early.
//
//  The first buffer starts at the start block of the track being ripped. But
//  subsequent blocks are processed like this:
//
//  1. The subsequent buffers we start at SP+NP-O.
//  2. We first do an optimistic comparison of O blocks starting at SP+NP-O
//     in the previous buffer, and at m_pc1Buf in the new buffer. If they
//     are equal, then no jitter error occured. We move m_pc1StartPtr up to
//     O blocks past the start of the buffer, and reduce the bytes read by O
//     blocks.
//  3. If the optimistic compare doesn't work, then we start an exhaustive
//     search to find out where in the new buffer the last block of the previous
//     buffer is at. Once we find it, we adjust m_pc1StartPtr to point at the
//     first byte in the new buffer to point at the actual start of the next
//     contiguous block. We adjust bytes read and end block accordingly if
//     that is required.
//
//  If the jitter is such that the new buffer starts before it should have,
//  then we will hvae more than 0 blocks of overlap, so we'll push the start
//  pointer forward and move the bytes read down and the end block down.
//  If it's such that the new buffer starts after it should have, then we
//  we move the start point down closer to the start of the buffer. The end
//  block and bytes read stays the same.
//
//  In either case when jitter correcting, we adjust bytes read to get rid of
//  any overlap, since after the initial read it includes all the bytes read.
// ---------------------------------------------------------------------------
class CIDDAEEXP TDAEBuf : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDAEBuf();

        TDAEBuf
        (
            const   tCIDLib::TCard4         c4BufSz
        );

        TDAEBuf(const TDAEBuf&) = delete;

        ~TDAEBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDAEBuf& operator=(const TDAEBuf&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4BufSz
        );


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_c4BufSz
        //      The allocated size of the buffer
        //
        //  m_c4BytesRead
        //      Initially this is set to the actual bytes read. It will be
        //      adjusted if jitter correction is on, to reflect the number
        //      of bytes we want to keep, relative to m_pc1StartPtr. So it
        //      will never, by the time it gets to the encoding thread,
        //      reflect more than N blocks, though it might be less than
        //      that.
        //
        //  m_c4EndBlk
        //      Based on the adjusted number of bytes read, this is the last
        //      block that this chunk holds. This is the jitter corrected
        //      value, so it's the end block we actually store.
        //
        //  m_c4StartBlk
        //      The start block of this chunk. It's always SP+NP. Since we must
        //      store contiguous chunks, the start block always has to be this,
        //      it's just that the position of this start block within our
        //      buffer might move around if jitter errors occur.
        //
        //  m_pc1Buf
        //      The buffer we provide for read/write. It's faultd in upon the
        //      first reset call (which is called each time an object is
        //      pulled from the pool.) This is the raw buffer. The m_pcStartBlk
        //      member reflects the actual start of the data we keep.
        //
        //  m_pc1StartPtr
        //      Since the drive can sometimes return data not aligned on the
        //      actual start of the buffer, this is used to point at the
        //      actual start of data we want to keep. If no jitter correction
        //      is being done, then m_pc1StartPtr==m_pc1Buf. Else, m_pc1StartPtr
        //      will usually be some amount beyond it. If no actual jitter
        //      error occurs, and jitter correction is on, then it will be O
        //      blocks past m_pc1Buf. If a jitter error occurs, then it will
        //      be somewhere beyond m_pc1Buf.
        //
        //      The data that the encoding thread uses, starts here and goes
        //      for m_c4BytesRead bytes. m_c4BytesRead is adjusted to reflect
        //      the actual number of bytes we want to keep.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BufSz;
        tCIDLib::TCard4     m_c4BytesRead;
        tCIDLib::TCard4     m_c4EndBlk;
        tCIDLib::TCard4     m_c4StartBlk;
        tCIDLib::TCard1*    m_pc1Buf;
        tCIDLib::TCard1*    m_pc1StartPtr;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEEncoder,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAECodec
// PREFIX: daec
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAECodec : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TCIDDAECodec() = delete;

        TCIDDAECodec(const TCIDDAECodec&) = delete;

        ~TCIDDAECodec();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAECodec& operator=(const TCIDDAECodec&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strGenericName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDDAECodec
        (
            const   TString&                strCodecName
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strName
        //      The derived classes must provide us a unique name for their
        //      codec, which we make available. This is a generic name,
        //      as apposed to the specific codec (e.g. "WMA" or "MP3"). This
        //      is just for human consumption.
        // -------------------------------------------------------------------
        TString m_strGenericName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAECodec,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEDecoder
// PREFIX: daed
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEDecoder : public TCIDDAECodec
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TCIDDAEDecoder() = delete;

        TCIDDAEDecoder(const TCIDDAEDecoder&) = delete;

        ~TCIDDAEDecoder();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEDecoder& operator=(const TCIDDAEDecoder&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4LoadChunk
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4StartInd = 0
        );

        tCIDLib::TVoid Cleanup();

        tCIDLib::TVoid EndDecode();

        tCIDLib::TVoid StartDecode
        (
            const   TString&                strSrcFile
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDDAEDecoder
        (
            const   TString&                strCodecName
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4LoadChunkImpl
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4StartInd = 0
        ) = 0;

        virtual tCIDLib::TVoid CleanupImpl() = 0;

        virtual tCIDLib::TVoid EndDecodeImpl() = 0;

        virtual tCIDLib::TVoid StartDecodeImpl
        (
            const   TString&                strSrcFile
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEDecoder,TCIDDAECodec)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEEncoder
// PREFIX: daee
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEEncoder : public TCIDDAECodec
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TCIDDAEEncoder() = delete;

        TCIDDAEEncoder(const TCIDDAEEncoder&) = delete;

        ~TCIDDAEEncoder();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEEncoder& operator=(const TCIDDAEEncoder&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCodecInfoValid
        (
            const   TString&                strCodec
            , const TString&                strFormat
        )   const = 0;

        virtual tCIDLib::TCard4 c4QueryCodecNames
        (
                    tCIDLib::TStrList&      colToFill
        ) = 0;

        virtual tCIDLib::TCard4 c4QueryCodecFmtNames
        (
            const   TString&                strCodec
            ,       tCIDLib::TStrList&      colToFill
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Cleanup();

        tCIDLib::TVoid EndEncode();

        tCIDLib::TVoid StartEncode
        (
            const   TString&                strTargetFile
            , const TString&                strCodecName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbumTitle
            , const TString&                strTrackTitle
            , const TString&                strArtist
            , const TString&                strLabel
            , const TString&                strGenre
            , const tCIDLib::TCard4         c4Year
        );

        tCIDLib::TVoid StoreChunk
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Bytes
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDDAEEncoder
        (
            const   TString&                strCodecName
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid CleanupImpl() = 0;

        virtual tCIDLib::TVoid EndEncodeImpl() = 0;

        virtual tCIDLib::TVoid StartEncodeImpl
        (
            const   TString&                strTargetFile
            , const TString&                strCodecName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbumTitle
            , const TString&                strTrackTitle
            , const TString&                strArtist
            , const TString&                strLabel
            , const TString&                strGenre
            , const tCIDLib::TCard4         c4Year
        ) = 0;

        virtual tCIDLib::TVoid StoreChunkImpl
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Bytes
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEEncoder,TObject)
};

#pragma CIDLIB_POPPACK


