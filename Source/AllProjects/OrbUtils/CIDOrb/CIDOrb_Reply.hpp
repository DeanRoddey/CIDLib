//
// FILE NAME: CIDOrb_Reply.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/08/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file defines the format of a reply packet, which is returned from
//  an Orb remote call. It contains a flag that indicates whether the call
//  went ok, in which case it contains the return info, or it failed, in
//  which case it contains the TError object.
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
//   CLASS: TOrbReply
//  PREFIX: orbr
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbReply : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TOrbReply() = delete;

        TOrbReply
        (
            const   tCIDLib::TCard4         c4InitSz
        );

        TOrbReply(const TOrbReply&) = delete;

        ~TOrbReply();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbReply& operator=(const TOrbReply&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4Size
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRetStatus() const;

        tCIDLib::TBoolean bRetStatus
        (
            const   tCIDLib::TBoolean       bNew
        );

        tCIDLib::TCard4 c4BufSize() const;

        tCIDLib::TCard4 c4PayloadBytes();

        tCIDLib::TCard4 c4SequenceId() const;

        tCIDLib::TCard4 c4SequenceId
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        TBinMBufInStream& strmIn();

        TBinMBufOutStream& strmOut();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bRetStatus
        //      This is used by the server to indicate whether its returning
        //      back an exception or a regular return.
        //
        //  m_c4SequenceId
        //      This is the sequence number which lets the client know that
        //      the response it gets is really in response to the command it
        //      sent. This is set for each new command sent.
        //
        //  m_mbufData
        //      Our data buffer that is streamed into and out of using the
        //      streams below, by the IDL code (and sometimes by the ORB
        //      code directly in special cases such as exception returns
        //      or internally handled commands.)
        //
        //  m_strmIn
        //  m_strmOut
        //      These are input and output streams that are used to marshall
        //      parms in and out of this command object. The output stream
        //      must be mutable so that we can flush it when we are streamed.
        //      They are set up on the m_mbufData member above, and they are
        //      linked as well.
        //
        //      The output stream has to be mutable so that we can flush it
        //      just to be sure, in the StreamTo method.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bRetStatus;
        tCIDLib::TCard4             m_c4SequenceId;
        THeapBuf                    m_mbufData;
        mutable TBinMBufOutStream   m_strmOut;
        TBinMBufInStream            m_strmIn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbReply,TObject)
        BefriendFactory(TOrbReply)
};

#pragma CIDLIB_POPPACK

