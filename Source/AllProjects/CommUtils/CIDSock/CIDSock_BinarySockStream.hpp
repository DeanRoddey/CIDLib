//
// FILE NAME: CIDSock_BinarySockStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This is the header for the CIDSock_BinarySockStream.cpp file. This file
//  implements the two simple classes that implement a binary stream over a
//  socket. They really just have to create the binary socket impl object
//  and install it on their parent class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TBinOutSockStream;


// ---------------------------------------------------------------------------
//   CLASS: TBinInSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDSOCKEXP TBinInSockStream : public TBinInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinInSockStream() = delete;

        TBinInSockStream
        (
            const   tCIDLib::TEncodedTime   enctWait
            ,       TStreamSocket* const    psockToUse
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TBinInSockStream
        (
            const   tCIDLib::TEncodedTime   enctWait
            , const TBinOutSockStream&      strmToSyncTo
        );

        TBinInSockStream(const TBinInSockStream&) = delete;

        ~TBinInSockStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinInSockStream& operator=(const TBinInSockStream&) = delete;


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


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiIn
        //      We keep a convenience copy of our stream impl object, so that
        //      we have a typed version to use. But we give it to our parent
        //      class to own.
        // -------------------------------------------------------------------
        TSockInStreamImpl*  m_pstrmiIn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinInSockStream,TBinInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TBinOutSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDSOCKEXP TBinOutSockStream : public TBinOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinOutSockStream() = delete;

        TBinOutSockStream
        (
                    TStreamSocket* const    psockToUse
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TBinOutSockStream(const TBinOutSockStream&) = delete;

        ~TBinOutSockStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinOutSockStream& operator=(const TBinOutSockStream&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TBinInSockStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiOut
        //      We keep a convenience copy of our stream impl object, so that
        //      we have a typed version to use. But we give it to our parent
        //      class to own.
        // -------------------------------------------------------------------
        TSockOutStreamImpl* m_pstrmiOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinOutSockStream,TBinOutStream)
};

#pragma CIDLIB_POPPACK


