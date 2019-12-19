//
// FILE NAME: CIDLib_Environment.hpp
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
//  This is the header for the CIDLib_Environment.Cpp file, which implements
//  the TProcEnvironment and TEnvironment classes.
//
//  TProcEnvironment is a namespace and operates on this process' environment,
//  which all threads of this process share.
//
//  TProcEnvLocker is a lock janitor that allows you to lock the environment
//  when you need to atomically do multiple operations. Single methods of
//  TProcEnvironment are thread safe, but that won't help you if you have to
//  do multiple operations atomically.
//
//  TEnvironment is a means of creating copies of the environment and
//  modifying it without affecting others. This is usually done to create a
//  new environment to pass to an external process which is being spawned,
//  via the TExternalProcess class. TEnvironment objects have their own
//  environment variable data and only operate on it, not affecting the
//  process environment.
//
// CAVEATS/GOTCHAS:
//
//  1)  The hash set that we use to hold the values is set up with a non-case sensitve
//      key ops object, so case of the keys doesn't matter.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  NAMESPACE: TProcEnvironment
// ---------------------------------------------------------------------------
namespace TProcEnvironment
{
    // -----------------------------------------------------------------------
    //  Public static methods
    // -----------------------------------------------------------------------
    CIDLIBEXP tCIDLib::TVoid Add
    (
        const   TString&                strKey
        , const TString&                strNewValue
    );

    CIDLIBEXP tCIDLib::TBoolean bAddOrUpdate
    (
        const   TString&                strKey
        , const TString&                strNewValue
    );

    CIDLIBEXP tCIDLib::TVoid AddToExePath
    (
        const   TString&                strNewValue
        , const tCIDLib::EStartEnd      eWhere = tCIDLib::EStartEnd::End
    );

    CIDLIBEXP tCIDLib::TVoid AddToLibPath
    (
        const   TString&                strNewValue
        , const tCIDLib::EStartEnd      eWhere = tCIDLib::EStartEnd::End
    );

    CIDLIBEXP tCIDLib::TBoolean bFind
    (
        const   TString&                strKey
        ,       TString&                strToFill
    );

    CIDLIBEXP tCIDLib::TBoolean bFindExePath
    (
                TString&                strToFill
    );

    CIDLIBEXP tCIDLib::TBoolean bFindLibPath
    (
                TString&                strToFill
    );

    CIDLIBEXP tCIDLib::TBoolean bFindTempPath
    (
                TString&                strToFill
    );

    CIDLIBEXP tCIDLib::TCard4 c4Entries();

    CIDLIBEXP tCIDLib::TCard4 c4CharsInValue
    (
        const   TString&                strKey
    );

    CIDLIBEXP TString strFind
    (
        const   TString&                strKey
    );

    CIDLIBEXP tCIDLib::TVoid Update
    (
        const   TString&                strKey
        , const TString&                strNewValue
    );
}


// ---------------------------------------------------------------------------
//   CLASS: TProcEnvLocker
//  PREFIX: lock
// ---------------------------------------------------------------------------
class CIDLIBEXP TProcEnvLocker : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProcEnvLocker();

        TProcEnvLocker(const TProcEnvLocker&) = delete;
        TProcEnvLocker(TProcEnvLocker&&) = delete;

        ~TProcEnvLocker();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TProcEnvLocker& operator=(const TProcEnvLocker&) = delete;
        TProcEnvLocker& operator=(TProcEnvLocker&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TProcEnvLocker, TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TEnvironment
//  PREFIX: env
// ---------------------------------------------------------------------------
class CIDLIBEXP TEnvironment : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public nested types
        // -------------------------------------------------------------------
        using TList = tCIDLib::TKVHashSet;
        using TCursor = TList::TCursor;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEnvironment
        (
            const   tCIDLib::TBoolean       bInherit = kCIDLib::False
        );

        TEnvironment
        (
            const   TEnvironment&           envSrc
        );

        TEnvironment
        (
                   TEnvironment&&           envSrc
        );

        ~TEnvironment();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEnvironment& operator=
        (
            const   TEnvironment&           envSrc
        );

        TEnvironment& operator=
        (
                    TEnvironment&&          envSrc
        );


        // -------------------------------------------------------------------
        //  Public non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add
        (
            const   TString&                strKey
            , const TString&                strNewValue
        );

        tCIDLib::TBoolean bAddIfNew
        (
            const   TString&                strKey
            , const TString&                strValue
        );

        tCIDLib::TBoolean bAddOrUpdate
        (
            const   TString&                strKey
            , const TString&                strNewValue
        );

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
        )   const;

        tCIDLib::TBoolean bFind
        (
            const   TString&                strKey
            ,       TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4Entries() const;

        tCIDLib::TCard4 chCharsInValue
        (
            const   TString&                strKey
        );

        TCursor cursThis() const;

        tCIDLib::TVoid RemoveAll();

        const TString& strFind
        (
            const   TString&                strKey
        )   const;

        tCIDLib::TVoid Update
        (
            const   TString&                strKey
            , const TString&                strNewValue
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FaultInList() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolEnv
        //      This is a hash map that is used to hold the environment variable data.
        //      We set the key ops object on it in case insensitive mode, so case of the
        //      keys doesn't matter.
        //
        //      To support efficient move semantics, it can be null, and we just fault
        //      it in if needed.That means the list pointer has to be mutable.
        // -------------------------------------------------------------------
        mutable TList*  m_pcolEnv;


        // -------------------------------------------------------------------
        //  Do any magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEnvironment,TObject)
        DefPolyDup(TEnvironment)
        NulObject(TEnvironment)
};

#pragma CIDLIB_POPPACK


