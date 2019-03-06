//
// FILE NAME: CIDDAE_Ripoper.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TCIDDAERipper class. This class provides the
//  core functionality involved in ripping data from a CDROM drive to am
//  encoder object. Ripping is done at the track level, so the caller
//  provides a TKrnlCDROM object that represents the CDROM object to do the
//  ripping on, the track to rip, ripping options, and the encoder to send
//  the data to.
//
//  The ripper works in terms of the encoder interfaces defined in the
//  CIDDAE_Codec.hpp header. This allows it to work with various codecs
//  generically.
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
class TDAEBufPool;


// ---------------------------------------------------------------------------
//  CLASS: TCIDDAERipper
// PREFIX: daer
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAERipper : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDAERipper();

        TCIDDAERipper(const TCIDDAERipper&) = delete;

        ~TCIDDAERipper();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAERipper& operator=(const TCIDDAERipper&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4PercentDone() const;

        tCIDLib::TCard4 c4ReadErrs() const;

        tCIDLib::TCard4 c4StitchErrs() const;

        tCIDLib::TCard4 c4Stitches() const;

        tCIDDAE::EJitterOpts eJitterOpt() const;

        tCIDDAE::EJitterOpts eJitterOpt
        (
            const   tCIDDAE::EJitterOpts    eToSet
        );

        tCIDLib::TVoid RipTrack
        (
                    TKrnlRemMediaDrv&       rmmdToUse
            , const TString&                strCodecName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            ,       TCIDDAEEncoder&         daeeToUse
            , const TString&                strTrackName
            ,       tCIDLib::TBoolean&      bBreakFlag
        );

        tCIDLib::TVoid RipTrack
        (
                    TKrnlRemMediaDrv&       rmmdToUse
            , const TString&                strCodecName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            ,       TCIDDAEEncoder&         daeeToUse
            , const TString&                strTrackName
            ,       tCIDLib::TBoolean&      bBreakFlag
            , const TString&                strAlbumTitle
            , const TString&                strTrackTitle
            , const TString&                strArtist
            , const TString&                strLabel
            , const TString&                strGenre
            , const tCIDLib::TCard4         c4Year
        );


    private :
        // -------------------------------------------------------------------
        //  Private non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompareBlocks
        (
            const   TDAEBuf&                daebPrev
            ,       TDAEBuf&                daebNew
        );

        tCIDLib::EExitCodes eReadThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eWriteThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        TDAEBuf* pdaebGetChunk
        (
                    TDAEBuf* const          pdaebPrev
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bReadComplete
        //      When the read thread is done, it sets this flag. This lets
        //      the write thread know when it can stop. If it has no more
        //      items in the work queue, and this flag is set, it exists.
        //
        //  m_bReadErr
        //  m_bWriteErr
        //      These are set by the read/write threads if they exit early,
        //      to let the main thread know the process didn't go to the end.
        //      The errors are stored in m_errRead and m_errWrite;
        //
        //  m_c4BlkCount
        //  m_c4EndBlk
        //  m_c4StartBlk
        //      These are set by the RipTrack method before it kicks off the
        //      threads. It tells them what sectors make up the track that
        //      we care about.
        //
        //  m_c4PercentDone
        //      This is kept up to date by the write thread as he takes
        //      care of chunks. It's available for the encoding client to
        //      monitor the progress.
        //
        //  m_c4ReadErrs
        //      Counts the number of read errors (where we have to retry a
        //      read.)
        //
        //  m_c4StitchErr
        //  m_c4Stitches
        //      If we have any uncorrectable stitch errors, we bump the stitch
        //      error counter. If we have any correctable ones we bump the
        //      stitches by the number of sectors it was off. The former
        //      tells the user the results might not be useful. The latter
        //      is used in automatic tuning.
        //
        //  m_c4TrackNum
        //      The current track number being ripped.
        //
        //  m_colWorkQ
        //      This is a thread safe, non-adopting queue that the read thread
        //      dumps buffers into and the encoding thread pulls them out of.
        //
        //  m_eJitterOpt
        //      The jitter option that is currently set. Defaults to none. The
        //      containing app can change it.
        //
        //  m_pbBreakFlag
        //      We store a pointer to the caller's break flag so that our
        //      own threads can set it. If one of them has an unrecoverable
        //      error it exits and needs to make the other one exit.
        //
        //  m_pdaeeToUse
        //      During the call to RipTrack, a pointer to the caller's encoder
        //      is stored here so that it can be accessed by the write thread.
        //      The write thread is only running during that time.
        //
        //  m_prmmdToUse
        //      A pointer to the caller's CDROM object that we use to do the
        //      ripping.
        //
        //  m_psplBuffers
        //      We use a pool of buffer objects so that the read thread can
        //      get buffers out and put them into the queue and the write
        //      thread can pull them out, use them, and release them back to
        //      the pool. We make it a pointer so that we can hide the template
        //      generated code involved, which is pretty substantial.
        //
        //  m_strCodecName
        //  m_strFmtName
        //      The name of the codec and the name of the format within
        //      that codec. We pass these to the encoder to tell him what
        //      codec and format to use. The user selects them
        //
        //  m_strOutFile
        //      We store this here so that the write thread can get to it.
        //
        //  m_thrRead
        //  m_thrWrite
        //      We use two threads for the ripping process. The read thread
        //      pulls data off the disk and the write thread does the stitching
        //      and invokes the encoder to encode the data. We start them up
        //      on the eReadThread and eWriteThread methods.
        //
        //  m_TOCData
        //      We load the table of contents data early in the ripping
        //      process.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bReadComplete;
        tCIDLib::TBoolean           m_bReadErr;
        tCIDLib::TBoolean           m_bWriteErr;
        tCIDLib::TCard4             m_c4BlkCount;
        tCIDLib::TCard4             m_c4EndBlk;
        tCIDLib::TCard4             m_c4PercentDone;
        tCIDLib::TCard4             m_c4ReadErrs;
        tCIDLib::TCard4             m_c4StartBlk;
        tCIDLib::TCard4             m_c4StitchErrs;
        tCIDLib::TCard4             m_c4Stitches;
        tCIDLib::TCard4             m_c4TrackNum;
        TRefQueue<TDAEBuf>          m_colWorkQ;
        TError                      m_errRead;
        TError                      m_errWrite;
        tCIDDAE::EJitterOpts        m_eJitterOpt;
        tCIDLib::TBoolean*          m_pbBreakFlag;
        TCIDDAEEncoder*             m_pdaeeToUse;
        TKrnlRemMediaDrv*           m_prmmdToUse;
        TDAEBufPool*                m_psplBuffers;
        TThread*                    m_pthrRead;
        TThread*                    m_pthrWrite;
        TString                     m_strCodecName;
        TString                     m_strFmtName;
        TString                     m_strOutFile;
        TKrnlRemMedia::TCDTOCInfo   m_TOCData;

        // We copy the incoming meta data here so the write thread can see it
        tCIDLib::TCard4             m_c4Year;
        TString                     m_strAlbumTitle;
        TString                     m_strArtist;
        TString                     m_strGenre;
        TString                     m_strLabel;
        TString                     m_strTrackTitle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAERipper,TObject)
};


#pragma CIDLIB_POPPACK

