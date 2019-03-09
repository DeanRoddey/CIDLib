//
// FILE NAME: CIDSock_TextSockStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/26/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDSock_TextSockStream.cpp file. This file
//  implements the two simple classes that implement a text stream over a
//  binary stream over a socket. They just have to create the binary socket
//  stream, and store pass it on to the parent class to use.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTextOutSockStream;


// ---------------------------------------------------------------------------
//   CLASS: TTextInSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDSOCKEXP TTextInSockStream : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextInSockStream() = delete;

        TTextInSockStream
        (
            const   tCIDLib::TEncodedTime   enctWait
            ,       TStreamSocket* const    psockToUse
            , const tCIDLib::EAdoptOpts     eAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextInSockStream
        (
            const   tCIDLib::TEncodedTime   enctWait
            , const TTextOutSockStream&     strmToSyncTo
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextInSockStream(const TTextInSockStream&) = delete;

        ~TTextInSockStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextInSockStream& operator=(const TTextInSockStream&) = delete;


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
            const   tCIDLib::TCard4         c4WaitMillis = kCIDLib::c4MaxWait
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiIn
        //      Because we need to have access sometimes to our bin stream,
        //      and don't want to constantly be downcasting the generic form
        //      we can get from our parent class, we store a copy. We don't
        //      own it!
        // -------------------------------------------------------------------
        TBinInSockStream*   m_pstrmiIn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextInSockStream,TTextInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TTextOutSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDSOCKEXP TTextOutSockStream : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextOutSockStream() = delete;

        TTextOutSockStream
        (
                    TStreamSocket* const    psockToAdopt
            , const tCIDLib::EAdoptOpts     eAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextOutSockStream
        (
                    TBinOutSockStream* const pstrmToAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextOutSockStream(const TTextOutSockStream&) = delete;

        ~TTextOutSockStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextOutSockStream& operator=(const TTextOutSockStream&);


    protected :
        // -------------------------------------------------------------------
        //  Make our input sibling our friend
        // -------------------------------------------------------------------
        friend class TTextInSockStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiOut
        //      Because we need to have access sometimes to our bin stream,
        //      and don't want to constantly be downcasting the generic form
        //      we can get from our parent class, we store a copy. We don't
        //      own it!
        // -------------------------------------------------------------------
        TBinOutSockStream*  m_pstrmiOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextOutSockStream,TTextOutStream)
};

#pragma CIDLIB_POPPACK

