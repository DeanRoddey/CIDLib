//
// FILE NAME: CIDLib_DataSrc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2014
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
//  This is the header for the CIDLib_DataSrc.cpp file. This file implements an
//  abstract base class that is used by various other classes as a abstracted source
//  of data.
//
//  It allows us to get around a lot of issues wrt to buffering of data, of dealing
//  with encrypted vs. non-encrypted sources, and so forth.
//
//  We also have to deal with block size issues, so a write may not write all of the
//  bytes because it's not enough for a full block. There is a FlushOut() method to
//  force any remaining data to be sent. It should be called after a complete msg has
//  been write to the data source.
//
//  We maintain two cache buffers, one for reading and one for writing. For reading it
//  is used to pre-read data from the derived class, which will be passed to any
//  installed processor and the resulting output stored and spooled out of. For
//  writing it is to hold onto data until a flush.
//
//  You'd think that streams would be the thing to use for this type of thing, but they
//  are a little too generic for the requirements of many uses of this class, which
//  have to deal with things like encryption or compression and which require special
//  init/term operations.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Data source MUST be initialized and terminated, because they may be used for
//      things like encrypted communications. Before they can be used the Initialize()
//      method must be called to let the derived class set up any per-session stuff.
//
//      When completed, the Terminate() method should be called. If not, the dtor
//      will make no attempt to do it, because it could involve some time consuming
//      process. Instead the data source will just be closed down hard without any
//      attempt to gracefully close down any session stuff.
//
//  2)  Termination doesn't necessarily close the communications resource (e.g. socket)
//      it just cleans up any session connection stuff. The data source can be re-
//      initialized to create a new session, though of course only over the original
//      communications resource unless the derived class creates a new one per
//      init.
//
//      A janitor is provided for convenient scope based management of data sources,
//      to insure they get initialized and terminated. Be aware though that this means
//      no exception can be thrown if termination fails, because it happens in the dtor
//      of the janitor. But a msg will be logged if in verbose mode.
//
//      This is a flag on the terminate command to indicate if you want to just run
//      the per-session termination, or to do that plus close the connection.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------
class CIDLIBEXP TCIDDataSrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCIDDataSrc(const TCIDDataSrc&) = delete;
        TCIDDataSrc(TCIDDataSrc&&) = delete;

        virtual ~TCIDDataSrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDataSrc& operator=(const TCIDDataSrc&) = delete;
        TCIDDataSrc& operator=(TCIDDataSrc&&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsConnected() const = 0;

        virtual tCIDLib::TVoid FormatSrcInfo
        (
                    TString&                strToFill
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDataAvailMS
        (
            const   tCIDLib::TCard4         c4WaitMSs
        );

        tCIDLib::TBoolean bDataAvail
        (
            const   tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TBoolean bIsInitialized() const;

        tCIDLib::TCard1 c1ReadByte
        (
            const   tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TCard4 c4ReadBytes
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bReadAll = kCIDLib::False
        );

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bReadAll = kCIDLib::False
        );

        tCIDLib::ELineRes eGetLine
        (
                    TString&                strToFill
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::ELineRes eGetLine
        (
                    TString&                strToFill
            , const tCIDLib::TEncodedTime   enctEnd
            ,       TTextConverter&         tcvtToUse
        );

        tCIDLib::TVoid FlushOut
        (
            const   tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid Initialize
        (
            const   tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid PushBack
        (
            const   tCIDLib::TVoid* const   pToPush
            , const tCIDLib::TCard4         c4Bytes
        );

        tCIDLib::TVoid ResetBufs();

        tCIDLib::TVoid Terminate
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bClose
        );

        tCIDLib::TVoid WriteBytes
        (
            const   TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4ToWrite
        );

        tCIDLib::TVoid WriteRawBytes
        (
            const   tCIDLib::TVoid* const   pData
            , const tCIDLib::TCard4         c4ToWrite
        );

        tCIDLib::TVoid WriteString
        (
            const   TString&                strToWrite
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDDataSrc();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4GetData
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctEnd
        )  = 0;

        virtual tCIDLib::TVoid PutData
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4ToSend
            , const tCIDLib::TEncodedTime   enctEnd
        ) = 0;

        virtual tCIDLib::TVoid SetupSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
        ) = 0;

        virtual tCIDLib::TVoid TerminateSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bClose
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReloadInCache
        (
            const   tCIDLib::TEncodedTime   enctEnd
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bInitialized
        //      Data sources must be initialized before use, and terminated when they
        //      are used up. This flag is used to make sure that has been done and to
        //      know if we need to do anything on termnation. Anya ttempt to read or
        //      write data before init is called will cause an exception.
        //
        //  m_c4ReadSz
        //      The number of bytes we try get from the derived class at a time.
        //
        //  m_c4InBufCount
        //  m_c4InBufIndex
        //  m_mbufInData
        //      Our Read spool buffer that we put data from the derived class
        //      into and spool out of.
        //
        //  m_mbufTmpSpool
        //      To make things easier on the derived classes, we only call them
        //      with an empty buffer, so they don't have to deal with previous
        //      data. But, if we have to reload the input spool buffer before it's
        //      empty, then we need to let the derived class load into a temp
        //      which we will copy into m_mbufInData after the carry-over data.
        //
        //  m_c4OutBufCount
        //  m_mbufOutData
        //      Our Write spool buffer where we store data until the user code
        //      flushes us. We always flush it all out, so we only need a count
        //      in this case, not an index.
        //
        //  m_mbufTmp
        //      If we have a processor, then we sometimes need a temp buffer to move
        //      data through the processing.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bInitialized;
        tCIDLib::TCard4     m_c4ReadSz;
        tCIDLib::TCard4     m_c4InBufCount;
        tCIDLib::TCard4     m_c4InBufIndex;
        THeapBuf            m_mbufInData;
        THeapBuf            m_mbufTmpSpool;
        tCIDLib::TCard4     m_c4OutBufCount;
        THeapBuf            m_mbufOutData;
        THeapBuf            m_mbufTmp;
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDDataSrcJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TCIDDataSrcJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDataSrcJan();

        TCIDDataSrcJan
        (
                    TCIDDataSrc* const      pcdsToSanitize
            , const tCIDLib::EAdoptOpts     eAdopt
            , const tCIDLib::TBoolean       bClose
            , const tCIDLib::TEncodedTime   enctEnd = 0
            , const tCIDLib::TCard4         c4CleanupMSs = 0
        );

        TCIDDataSrcJan(const TCIDDataSrcJan&) = delete;
        TCIDDataSrcJan(TCIDDataSrcJan&&) = delete;

        ~TCIDDataSrcJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDataSrcJan& operator=(const TCIDDataSrcJan&) = delete;
        TCIDDataSrcJan& operator=(TCIDDataSrcJan&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCIDDataSrc& cdsData() const;

        TCIDDataSrc& cdsData();

        TCIDDataSrc* pcdsData();

        TCIDDataSrc* pcdsOrphan();

        tCIDLib::TVoid Orphan();

        tCIDLib::TVoid SetNew
        (
                    TCIDDataSrc* const      pcdsToSanitize
            , const tCIDLib::EAdoptOpts     eAdopt
            , const tCIDLib::TBoolean       bClose
            , const tCIDLib::TEncodedTime   enctEnd = 0
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bClose
        //      When we call terminate, we can optionally tell it to close the
        //      underlying connection as well.
        //
        //  m_eAdopt
        //      We may adopt, in which case we terminate and delete, or not in which
        //      case we just terminate. In the later case we would be used across
        //      a scope based session.
        //
        //  m_c4CleanupMSs
        //      The max time we'll wait for cleanup to complete
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bClose;
        tCIDLib::EAdoptOpts     m_eAdopted;
        tCIDLib::TCard4         m_c4CleanupMSs;
        TCIDDataSrc*            m_pcdsToSanitize;
};

#pragma CIDLIB_POPPACK


