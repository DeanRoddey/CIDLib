//
// FILE NAME: CIDSock_SocketStreamImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This is the header for the CIDSock_SocketStreamImpl.cpp file. This file
//  implements the two input and output stream impl classes that allow a
//  binary stream to stream to/from a socket.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSockOutStreamImpl;


// ---------------------------------------------------------------------------
//   CLASS: TSockInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSockInStreamImpl : public TInStreamImpl
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSockInStreamImpl() = delete;

        TSockInStreamImpl
        (
            const   tCIDLib::TEncodedTime   enctWait
            ,       TStreamSocket* const    psockToAdopt
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TSockInStreamImpl
        (
            const   tCIDLib::TEncodedTime   enctWait
            , const TSockOutStreamImpl&     strmiToSyncTo
        );

        TSockInStreamImpl(const TSockInStreamImpl&) = delete;

        ~TSockInStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSockInStreamImpl& operator=(const TSockInStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDataAvailable() const;

        tCIDLib::TBoolean bWaitForDataReady
        (
            const   tCIDLib::TEncodedTime   enctWait
        )   const;

        tCIDLib::TBoolean bWaitForDataReadyMS
        (
            const   tCIDLib::TCard4         c4WaitMillis
        )   const;

        tCIDLib::TEncodedTime enctMaxWait() const;

        tCIDLib::TEncodedTime enctMaxWait
        (
            const   tCIDLib::TEncodedTime   enctToSet
        );

        TStreamSocket& sockThis();

        const TStreamSocket& sockThis() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8Cur
        //      To meet stream semantics, we have to be able to report our
        //      current position. So we just maintain a count of bytes read
        //      so far.
        //
        //  m_enctMaxWait
        //      The max time we are to wait for data to arrive when a
        //      read is done. If it's set to enctMaxWait, then basically we
        //      only get EOS if the other side closes the socket.
        //
        //  m_psockIn
        //      A pointer to the socket we read from. We may or may not own
        //      it, according to the m_eAdopt member.
        //
        //  m_eAdopt
        //      Indicates whether we've adopted the socket or not. If not,
        //      then we always orphan the socket out of the counted pointer
        //      so that it doesn't delete it.
        // -------------------------------------------------------------------
        tCIDLib::TCard8         m_c8Cur;
        tCIDLib::TEncodedTime   m_enctMaxWait;
        TStreamSocket*          m_psockIn;
        tCIDLib::EAdoptOpts     m_eAdopt;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSockInStreamImpl,TInStreamImpl)
};




// ---------------------------------------------------------------------------
//   CLASS: TSockOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSockOutStreamImpl : public TOutStreamImpl
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSockOutStreamImpl() = delete;

        TSockOutStreamImpl
        (
                    TStreamSocket* const    psockToAdopt
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TSockOutStreamImpl(const TSockOutStreamImpl&) = delete;

        ~TSockOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSockOutStreamImpl& operator=(const TSockOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SeekToEnd() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TStreamSocket& sockThis();

        const TStreamSocket& sockThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TSockInStreamImpl;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8Cur
        //      To meet stream semantics, we have to be able to report our
        //      current position. So we just maintain a count of bytes read
        //      so far.
        //
        //  m_psockOut
        //      The socket that we work in terms of. We may own it accrding to
        //      the m_eAdopt member.
        // -------------------------------------------------------------------
        tCIDLib::TCard8     m_c8Cur;
        tCIDLib::EAdoptOpts m_eAdopt;
        TStreamSocket*      m_psockOut;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSockOutStreamImpl,TOutStreamImpl)
};


#pragma CIDLIB_POPPACK


