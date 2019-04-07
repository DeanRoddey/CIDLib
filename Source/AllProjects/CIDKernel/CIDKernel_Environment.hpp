//
// FILE NAME: CIDKernel_Environment.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/15/1997
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
//  This is the header for the CIDKernel_Environment.Cpp file. This file
//  implements the TKrnlEnvironment class, which encapsulates access to
//  the host environment variables. All of it is static and it should be
//  treaded like a namespace, but it is implemented as a class  because there
//  is a need to share data between the platform dependent and independent
//  implementation files. The constructors are hidden to insure this.
//
// CAVEATS/GOTCHAS:
//
//  1)  All operations are synchronized so multiple thread access is not
//      a problem.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TKrnlCritSec;
class   TKrnlHashMap;


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TKrnlEnvironment
//  PREFIX: kenv
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlEnvironment
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        class TElem
        {
            public :
                TElem() :

                    pszKey(nullptr)
                    , pszValue(nullptr)
                {
                }

                TElem(const TElem&) = delete;

                ~TElem()
                {
                    delete [] pszKey;
                    delete [] pszValue;
                }

                TElem& operator=(const TElem&) = delete;

                tCIDLib::TVoid Set( const   tCIDLib::TCh* const pszTheKey
                                    , const tCIDLib::TCh* const pszTheValue)
                {
                    pszKey = TRawStr::pszReplicate(pszTheKey);
                    pszValue = TRawStr::pszReplicate(pszTheValue);
                }

                tCIDLib::TCh*   pszKey;
                tCIDLib::TCh*   pszValue;
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bAdd
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszNewValue
        );

        static tCIDLib::TBoolean bAddOrUpdate
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszNewValue
            ,       tCIDLib::TBoolean&      bAdded
        );

        static tCIDLib::TBoolean bAddToExePath
        (
            const   tCIDLib::TCh* const     pszToAdd
            , const tCIDLib::EStartEnd      eWhere = tCIDLib::EStartEnd::End
        );

        static tCIDLib::TBoolean bAddToLibPath
        (
            const   tCIDLib::TCh* const     pszToAdd
            , const tCIDLib::EStartEnd      eWhere = tCIDLib::EStartEnd::End
        );

        static tCIDLib::TBoolean bCharsInValue
        (
            const   tCIDLib::TCh* const     pszKey
            ,       tCIDLib::TCard4&        c4ToFill
        );

        static tCIDLib::TBoolean bFind
        (
            const   tCIDLib::TCh* const     pszKey
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bFindExePath
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bFindLibPath
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bFindTempPath
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bKeyExists
        (
            const   tCIDLib::TCh* const     pszKey
        );

        static tCIDLib::TBoolean bQueryState
        (
                    TElem*&                 pelemToFill
            ,       tCIDLib::TCard4&        c4ToFill
        );

        static tCIDLib::TBoolean bUpdate
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszNewValue
        );

        static tCIDLib::TCard4 c4Entries();

        static tCIDLib::TVoid Lock();

        static const tCIDLib::TCh* pszFind
        (
            const   tCIDLib::TCh* const     pszKey
        );

        static tCIDLib::TVoid Unlock();


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TKrnlEnvironment(const TKrnlEnvironment&) = delete;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlEnvironment& operator=(const TKrnlEnvironment&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TCIDKrnlModule;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TKrnlEnvironment();


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bSetSysVar
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszNewValue
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  s_pkcrsAccess
        //      This is a critical section used to sync access to the
        //      environment data across threads.
        //
        //  s_pkhshmEnv
        //      The kernel hash map structure used to hold the environmental
        //      variables. Its allocated during facility init.
        // -------------------------------------------------------------------
        static TKrnlCritSec*    s_pkcrsAccess;
        static TKrnlHashMap*    s_pkhshmEnv;
};

#pragma CIDLIB_POPPACK


