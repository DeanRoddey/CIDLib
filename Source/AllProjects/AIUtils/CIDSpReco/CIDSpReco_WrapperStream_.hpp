//
// FILE NAME: CIDSpReco_WrapperStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  We have some special concerns wrt to getting audio into the reco engine. So we end
//  using three layers of streams:
//
//  Application specific - This allows the application to let us get audio in from
//  multiple sources without having to know about them. These must return 16K, 16 bit
//  integer sample format. This is some stream provided by the application, derived from
//  the base source audio stream class in CIDAudStream.
//
//  Wrapper - We have to provide an IStream, but we can't have public classes derived
//  from a COM class. So this is an internal class that reads data from the application
//  specific stream and just passes it through to the reco engine. This one can derive
//  from IStream.
//
//  Reco Stream - The recognition engine uses its own stream type, which works in terms
//  an IStream object. So we set the above wrapper as the base stream for this stream.
//
//  The Reco stream gets set on the reco engine. The reco engine gets created and destroyed
//  as we set up and tear down the reco engine. The other two we keep around and just
//  give back to the reco stream again each time.
//
//
//  A common issue is TTS triggering bogus events. We cannot just ignore them even if
//  low confidence, because if we are trying to get clarification from the user, we have
//  to tell him we didn't understand his reply (if we don't.) So the application
//  generally wants to pause recognition while speaking.
//
//  There are some ways to do it via the grammar or context, but they don't really work
//  because accumulated data is still seen afterwards, and you can't leave it paused long
//  or it will fail.
//
//  So we provide a 'suppress input' flag. If this is set, we eat the incoming data from
//  the app stream, and then pass silence to the reco stream. This provides an effective
//  and efficient means to stop recognition that always works and never risks input
//  overflow.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TSpWrapperStream
//  PREFIX: spstrm
// ---------------------------------------------------------------------------
class TSpWrapperStream : public IStream
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSpWrapperStream
        (
                TCIDAudioSrcStream* const   pstrmSrc
        );

        TSpWrapperStream(const TSpWrapperStream&) = delete;

        ~TSpWrapperStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSpWrapperStream& operator=(const TSpWrapperStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        STDMETHODIMP_(ULONG) AddRef();

        STDMETHODIMP Clone(IStream**);

        STDMETHODIMP Commit(DWORD);

        STDMETHODIMP CopyTo
        (
            IStream*
            , ULARGE_INTEGER
            , ULARGE_INTEGER *
            , ULARGE_INTEGER *
        );

        STDMETHODIMP LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);

        STDMETHODIMP QueryInterface(REFIID riid, void **ppv);

        STDMETHODIMP Read
        (
            void *pBuffer
            , ULONG cbBuffer
            , ULONG *pcbRead
        );

        STDMETHODIMP_(ULONG) Release();

        STDMETHODIMP Revert();

        STDMETHODIMP Seek
        (
            LARGE_INTEGER
            , DWORD
            , ULARGE_INTEGER *
        );

        STDMETHODIMP SetSize(ULARGE_INTEGER);

        STDMETHODIMP Stat(STATSTG*, DWORD);

        STDMETHODIMP UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);

        STDMETHODIMP Write
        (
            const void *pv
            , ULONG cb
            , ULONG *pcbWritten
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSpeechState() const;

        tCIDLib::TBoolean bSpeechState
        (
            const   tCIDLib::TBoolean       bToSet
        );


        tCIDLib::TBoolean bSuppressInput() const;

        tCIDLib::TBoolean bSuppressInput
        (
            const   tCIDLib::TBoolean       bToSet
        );

    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSpeechActive
        //      The is used to prevent any input during startup, until we are ready
        //      to process it. It just returns errors to indicate it can't read
        //      anything, if this is set.
        //
        //  m_bSuppressInput
        //      This will cause us to return silence to the recognition engine. See
        //      the class comments above.
        //
        //  m_c4RefCnt
        //      The usual COM reference count
        //
        //  m_pstrmSrc
        //      The underlying stream provided by the containing application. We
        //      don't own this guy, the engine does.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bSpeechActive;
        tCIDLib::TBoolean   m_bSuppressInput;
        tCIDLib::TCard4     m_c4CurBufSz;
        tCIDLib::TCard4     m_c4RefCnt;
        TCIDAudioSrcStream* m_pstrmSrc;
};

