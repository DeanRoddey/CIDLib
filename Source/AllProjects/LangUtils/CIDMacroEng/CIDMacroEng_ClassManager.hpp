//
// FILE NAME: CIDMacroEng_ClassManager.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/15/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ClassManager.cpp file, which
//  implements the MMEngClassMgr class. This is a pure abstract base class,
//  intended to be used as a mixin, that allows individual implementations to
//  plug in a means of loading in macro source, and writing it back out when
//  necessary.
//
//  It supports readonly and readwrite modes for class loading, in order
//  to support derived implementations of the macro engine that are built on
//  source control systems.
//
//  We supply a prefab derivative that supports the very common scenario of
//  just basing the class hierarchy under some fixed disk directory. But note
//  that it's bDoSelect() is a no-op that returns false. If you want to support
//  selection, you have to derive yet again. But since many uses won't require
//  selection, we want to let people use this class directly.
//
//  We also define an interface for loading 'internal' classes, i.e. those
//  classes that don't need to be compiled because they are just wrappers
//  around C++ classes. The standard runtime classes are of this type, and
//  many applications which use the engine will want to add their own. They
//  can register them with our facility class and the engine will load classes
//  via the facility class. These must be reentrant, and normally just allocate
//  class info objects and return them.
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
//  CLASS: MMEngClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngClassMgr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MMEngClassMgr(const MMEngClassMgr&) = delete;

        ~MMEngClassMgr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MMEngClassMgr& operator=(const MMEngClassMgr&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bMacroExists
        (
            const   TString&                strToCheck
        );

        tCIDLib::EOpenRes eSelectClass
        (
                    TString&                strToFill
            , const tCIDMacroEng::EResModes    eMode
        );

        TTextInStream* pstrmLoadClass
        (
            const   TString&                strClassPath
            , const tCIDMacroEng::EResModes    eMode
        );

        tCIDLib::TVoid StoreClass
        (
            const   TString&                strClassPath
            , const TString&                strText
        );

        tCIDLib::TVoid UndoWriteMode
        (
            const   TString&                strClassPath
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngClassMgr();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCheckIfExists
        (
            const   TString&                strToCheck
        ) = 0;

        virtual tCIDLib::TVoid DoStore
        (
            const   TString&                strClassPath
            , const TString&                strText
        ) = 0;

        virtual tCIDLib::TVoid DoUndoWriteMode
        (
            const   TString&                strClassPath
        ) = 0;

        virtual tCIDLib::EOpenRes eDoSelect
        (
                    TString&                strToFill
            , const tCIDMacroEng::EResModes    eMode
        ) = 0;

        virtual TTextInStream* pstrmDoLoad
        (
            const   TString&                strClassPath
            , const tCIDMacroEng::EResModes    eMode
        ) = 0;
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFixedBaseClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFixedBaseClassMgr

    : public TObject, public MMEngClassMgr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFixedBaseClassMgr();

        TMEngFixedBaseClassMgr
        (
            const   TString&                strBasePath
        );

        TMEngFixedBaseClassMgr(const TMEngFixedBaseClassMgr&) = delete;

        ~TMEngFixedBaseClassMgr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFixedBaseClassMgr& operator=(const TMEngFixedBaseClassMgr&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strBasePath() const;

        const TString& strBasePath
        (
            const   TString&                strToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckIfExists
        (
            const   TString&                strToCheck
        )   override;

        tCIDLib::TVoid DoStore
        (
            const   TString&                strClassPath
            , const TString&                strText
        )   override;

        tCIDLib::TVoid DoUndoWriteMode
        (
            const   TString&                strClassPath
        )   override;

        tCIDLib::EOpenRes eDoSelect
        (
                    TString&                strToFill
            , const tCIDMacroEng::EResModes    eMode
        )   override;

        TTextInStream* pstrmDoLoad
        (
            const   TString&                strClassPath
            , const tCIDMacroEng::EResModes    eMode
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pathTmp
        //  m_pathTmp2
        //      Some temp path strings to use to build up the actual path. We
        //      are not a thread safe class, so we can do this. It prevents
        //      having to allocate and deallocate a couple strings on every
        //      class resolution.
        //
        //  m_strBasePath
        //      This is the base path to which class paths are appended.
        // -------------------------------------------------------------------
        TPathStr    m_pathTmp1;
        TPathStr    m_pathTmp2;
        TString     m_strBasePath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFixedBaseClassMgr,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: MMEngExtClassLoader
// PREFIX: mecl
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngExtClassLoader
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MMEngExtClassLoader(const MMEngExtClassLoader&) = delete;

        ~MMEngExtClassLoader();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MMEngExtClassLoader& operator=(const MMEngExtClassLoader&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual TMEngClassInfo* pmeciLoadClass
        (
                    TCIDMacroEngine&        meTarget
            , const TString&                strClassPath
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngExtClassLoader();

};


#pragma CIDLIB_POPPACK


