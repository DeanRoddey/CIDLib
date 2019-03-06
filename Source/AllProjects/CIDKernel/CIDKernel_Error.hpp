//
// FILE NAME: CIDKernel_Error.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/30/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Error.Cpp file. This file implements
//  the TKrnlError class, which is how all errors are returned to higher
//  level code. Errors are returned using a 'last error' scheme, in which
//  each thread can store the last error that occured so that it can be
//  queried by calling code.
//
// CAVEATS/GOTCHAS:
//
//  1)  The default constructor just creates an object with zero values for
//      the error ids. The others set up the ids as passed. If a CIDkernel
//      error level is the only id provided, then the host error id is just
//      set to 'no error'.
//
//  2)  In general, these are not ever created directly. Instead, a set of
//      static methods are provided to store last error information for the
//      calling thread. They store the raw ids into a preexisting kernel
//      error object that exists for each thread.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlError
//  PREFIX: kerr
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlError
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bQuerySysErrMsg
        (
            const   tCIDLib::TOSErrCode     errcGet
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static const TKrnlError& kerrLast();

        static tCIDLib::TVoid SetLastError
        (
            const   TKrnlError&             kerrToSet
        );

        static tCIDLib::TVoid SetLastHostError
        (
            const   tCIDLib::TOSErrCode     errcHostId
        );

        static tCIDLib::TVoid SetLastKrnlError
        (
            const   tCIDLib::TErrCode       errcKrnlId
            , const tCIDLib::TOSErrCode     errcHostId = 0
        );

        static tCIDLib::TVoid ThrowHostError
        (
            const   tCIDLib::TOSErrCode     errcHostId
        );

        static tCIDLib::TVoid ThrowKrnlError
        (
            const   tCIDLib::TErrCode       errcId
            , const tCIDLib::TOSErrCode     errcHostId
        );

        static tCIDLib::TVoid ThrowKrnlError
        (
            const   tCIDLib::TErrCode       errcId
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlError();

        TKrnlError
        (
            const   tCIDLib::TErrCode       errcId
        );

        TKrnlError
        (
            const   tCIDLib::TErrCode       errcId
            , const tCIDLib::TOSErrCode     errcHostId
        );

        TKrnlError
        (
            const   TKrnlError&             kerrToCopy
        );

        ~TKrnlError();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlError& operator=
        (
            const   TKrnlError&             kerrToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TKrnlError&             kerrToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKrnlError&             kerrToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bError() const;

        tCIDLib::TErrCode errcId() const;

        tCIDLib::TErrCode errcId
        (
            const   tCIDLib::TErrCode       errcKrnlId
        );

        tCIDLib::TOSErrCode errcHostId() const;

        tCIDLib::TOSErrCode errcHostId
        (
            const   tCIDLib::TOSErrCode     errcHostId
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TErrCode       errcKrnlId
            , const tCIDLib::TOSErrCode     errcHostId
        );

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_errcId
        //      This is the error id of the error, zero if no error. If zero,
        //      then m_errcHostId means nothing.
        //
        //  m_errcHostId
        //      This is the underlying host OS error, if any, that caused it.
        // -------------------------------------------------------------------
        tCIDLib::TErrCode       m_errcId;
        tCIDLib::TOSErrCode     m_errcHostId;
};

#pragma CIDLIB_POPPACK


