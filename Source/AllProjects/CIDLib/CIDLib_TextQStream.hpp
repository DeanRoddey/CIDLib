//
// FILE NAME: CIDLib_TextQStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/2002
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
//  This is the header for the CIDLib_TextQStream.cpp file, which provides
//  a text output stream that streams to a queue of TString objects. This
//  is often useful if you need to provide access to the output of a back
//  end application that has no console output, a detached process or service
//  for instance.
//
//  We have to create a stream implementation class, which provides the
//  actual data sink, and a text stream wrapper class that sets itself up
//  with this implementation class. The wrapper class is trivial. All it
//  has to do is construct a binary stream over an instance of the impl
//  class, and then set the binary stream as its own data sink. The rest
//  of the interface is provided by the base class.
//
//  It will store up to an indicated number text lines. It just tosses old
//  lines when the queue gets full, so it always contains X of the latest
//  lines written.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TQOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TQOutStreamImpl : public TOutStreamImpl
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TQOutStreamImpl() = delete;

        TQOutStreamImpl
        (
            const   tCIDLib::TCard4         c4MaxLines
            , const tCIDLib::EReadModes     eMode
        );

        TQOutStreamImpl(const TQOutStreamImpl&) = delete;

        ~TQOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TQOutStreamImpl& operator=(const TQOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        ) override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SeekToEnd() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryNewMsgs
        (
                    tCIDLib::TCard4&        c4MsgIndex
            ,       tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bAddNewLine
        );

        tCIDLib::TBoolean bQueryNewMsgs
        (
                    tCIDLib::TCard4&        c4MsgIndex
            ,       TString&                strToFill
            ,       tCIDLib::TCard4&        c4LineCount
        );

        tCIDLib::TCard4 c4CurMsgIndex() const;

        tCIDLib::TCard4 c4LineCount() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddNewLine
        (
            const   TString&                strToAdd
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apstrLines
        //      This is the queue that we write lines to. Its just an array
        //      of pointers to strings. We just rotate the pointers down to
        //      add a new item, recycling the one at the end when we get
        //      full and have to start throwing strings out.
        //
        //  m_c4MaxLines
        //      The max lines elements we are to hold, which we are given
        //      in the constructor.
        //
        //  m_c4BaseIndex
        //  m_c4MsgIndex
        //      In order to avoid clients having to pull over every message
        //      in the queue every time, we keep an index and they get the
        //      current index when they read. When they come back next time,
        //      they get only the new messages.
        //
        //      The base index indicates the message index of the item in
        //      the 0th element. Newer elements go at the end, so the 0th
        //      element is the oldest one. Basically, once we run the msg
        //      index up to the max line count, we then just keep bumping
        //      the base index.
        //
        //  m_c8CurPos
        //      We don't have a real stream target per-se, so we just use a
        //      dummy field to count bytes written.
        //
        //  m_eMode
        //      Indicates whether we should retain messages after a query of
        //      new messages, or whether we should dump messages after a read
        //      of new messages. Some applications need to keep the whole
        //      record of the output, and some just want to periodically get
        //      it out and put it somewhere else, and therefore can handle
        //      infinite output with reasonable memory usage.
        //
        //  m_mtxSync
        //      We need to synchronize access to the queue data.
        //
        //  m_strCurLine
        //      This is where we build lines until we get a new line and
        //      can add it to the queue. If we get a partial line during a
        //      write, the partial content stays here until the next write
        //      event.
        //
        //  m_tcvtRead
        //      Since we are underneath a text stream, its going to write
        //      data via the intervening binary stream, and the binary data
        //      it writes will be in the native wide string format. So we use
        //      a native wide character converter to transcode back out of
        //      the binary data we get back to wide character data that we
        //      can store in our queue.
        // -------------------------------------------------------------------
        TString**           m_apstrLines;
        tCIDLib::TCard4     m_c4BaseIndex;
        tCIDLib::TCard4     m_c4MaxLines;
        tCIDLib::TCard4     m_c4MsgIndex;
        tCIDLib::EReadModes m_eMode;
        tCIDLib::TCard8     m_c8CurPos;
        TMutex              m_mtxSync;
        TString             m_strCurLine;
        TNativeWCConverter  m_tcvtRead;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TQOutStreamImpl,TOutStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TTextQOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextQOutStream : public TTextOutStream
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextQOutStream() = delete;

        TTextQOutStream
        (
            const   tCIDLib::TCard4         c4MaxLines
            , const tCIDLib::EReadModes     eMode
        );

        TTextQOutStream(const TTextQOutStream&) = delete;

        ~TTextQOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextQOutStream& operator=(const TTextQOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryNewMsgs
        (
                    tCIDLib::TCard4&        c4MsgIndex
            ,       tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bAddNewLine = kCIDLib::False
        );

        tCIDLib::TBoolean bQueryNewMsgs
        (
                    tCIDLib::TCard4&        c4MsgIndex
            ,       TString&                strToFill
            ,       tCIDLib::TCard4&        c4LineCount
        );

        tCIDLib::TCard4 c4CurMsgIndex() const;

        tCIDLib::TCard4 c4LineCount() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiQ
        //      We keep a typed pointer to our impl object for our own
        //      convenience. But its own by the parent class.
        // -------------------------------------------------------------------
        TQOutStreamImpl*        m_pstrmiQ;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextQOutStream,TTextOutStream)
};

#pragma CIDLIB_POPPACK


