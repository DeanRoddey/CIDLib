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
//  implementation files. The constructors are hidden to insure this, and the
//  destructor is deleted for the same reason.
//
//  We also create a little janitor class that will clean up an array of pointers
//  to raw strings, since that's a format required in various places to pass
//  parameter or environment info to system APIs.
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
//   CLASS: TStrArrayJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class KRNLEXPORT TStrArrayJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TStrArrayJan() = delete;

        TStrArrayJan(const TStrArrayJan&) = delete;
        TStrArrayJan(TStrArrayJan&&) = delete;

        TStrArrayJan(       tCIDLib::TCh**      apszEnv
                    , const tCIDLib::TCard4     c4Count
                    , const tCIDLib::TBoolean   bFreeArray) :

            m_bFreeArray(bFreeArray)
            , m_apszEnv(apszEnv)
            , m_c4Count(c4Count)
        {
        }

        ~TStrArrayJan()
        {
            if (m_apszEnv)
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Count; c4Index++)
                    delete [] m_apszEnv[c4Index];
            }

            if (m_bFreeArray)
                delete [] m_apszEnv;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStrArrayJan& operator=(const TStrArrayJan&) = delete;
        TStrArrayJan& operator=(TStrArrayJan&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_apszEnv = nullptr;
        }

        tCIDLib::TCh** apszEnv()
        {
            return m_apszEnv;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFreeArray
        //      The array itself may or may not be dynamically allocated. The
        //      creator tells whether or not we should delete it.
        //
        //  m_c4Count
        //      The size of the array, i.e. the number of strings to delete.
        //
        //  m_pszEnv
        //      The environment data to clean up if we still have it on dtor.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bFreeArray;
        tCIDLib::TCard4     m_c4Count;
        tCIDLib::TCh**      m_apszEnv;
};



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

                    m_pszKey(nullptr)
                    , m_pszValue(nullptr)
                {
                }

                TElem(const TElem&) = delete;
                TElem(TElem&&) = delete;

                ~TElem()
                {
                    delete [] m_pszKey;
                    delete [] m_pszValue;
                }

                TElem& operator=(const TElem&) = delete;
                TElem& operator=(TElem&&) = delete;

                tCIDLib::TVoid Set( const   tCIDLib::TCh* const pszTheKey
                                    , const tCIDLib::TCh* const pszTheValue)
                {
                    m_pszKey = TRawStr::pszReplicate(pszTheKey);
                    m_pszValue = TRawStr::pszReplicate(pszTheValue);
                }

                tCIDLib::TCh*   m_pszKey;
                tCIDLib::TCh*   m_pszValue;
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
            ,       TKrnlString&            kstrToFill
        );

        static tCIDLib::TBoolean bFindExePath
        (
                    TKrnlString&            kstrToFill
        );

        static tCIDLib::TBoolean bFindLibPath
        (
                    TKrnlString&            kstrToFill
        );

        static tCIDLib::TBoolean bFindTempPath
        (
                    TKrnlString&            kstrToFill
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
        TKrnlEnvironment(TKrnlEnvironment&&) = delete;
        ~TKrnlEnvironment() = delete;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlEnvironment& operator=(const TKrnlEnvironment&) = delete;
        TKrnlEnvironment& operator=(TKrnlEnvironment&&) = delete;


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


