//
// FILE NAME: CIDMacroEng_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ThisFacility.cpp file. This
//  file implements the facility class for this facility. The facility class
//  maintains the type registry, since that will tend to be process wide for
//  any one process. All types used by macros must be registered, which stores
//  a 'type info' object for that type. That type info object is a factory
//  which is stored under the full name of the type (ns:name format) and which
//  can generate storage objects for that type.
//
//  Like other entities, classes are referred to by their id in generated
//  opcodes, so each registered class gets an id that won't change for the run
//  of the process. For the intrinsic classes, since they are overwhelmingly
//  used, we store their ids specially and have dedicated methods to get them.
//  For other types, they are looked up by their type name.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TCIDMacroEngine;
class TMEngRTClassLoader;


// ---------------------------------------------------------------------------
//  CLASS: TFacCIDMacroEng
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TFacCIDMacroEng : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString strFalseKeyword;
        static const TString strRootClassPath;
        static const TString strRuntimeClassPath;
        static const TString strTrueKeyword;
        static const TString strUnknownClass;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDMacroEng();

        TFacCIDMacroEng(const TFacCIDMacroEng&) = delete;

        ~TFacCIDMacroEng();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDMacroEng& operator=(const TFacCIDMacroEng&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virutal methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddClassLoader
        (
                    MMEngExtClassLoader* const pmeclToAdd
            , const tCIDLib::EEnds          eEnd = tCIDLib::EEnds::First
        );

        tCIDLib::TBoolean bIsValidCMLPath
        (
            const   TString&                strVal
            ,       TString&                strErrText
            , const tCIDLib::TBoolean       bEmptyOK
        )   const;

        tCIDLib::TVoid CheckIdOverflow
        (
            const   tCIDLib::TCard4         c4Next
            , const tCIDLib::TCh* const     pszName
        );

        tCIDLib::TVoid MakeFullPath
        (
            const   TString&                strBasePath
            , const TString&                strName
            ,       TString&                strToFill
        );

        TMEngClassInfo* pmeciInvokeExternalLoaders
        (
                    TCIDMacroEngine&        meCaller
            , const TString&                strClassPathToLoad
        );

        const TString& strEmptyClassTemplate() const;

        const TString& strMacroFileDescr() const;

        const TString& strMacroFileSelWC() const;

        tCIDLib::TVoid SplitClassPath
        (
            const   TString&                strClassPath
            ,       TString&                strBasePath
            ,       TString&                strName
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TLoaderList = TRefVector<MMEngExtClassLoader>;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colLoaderList
        //      We allow multiple class loaders to be installed. These are used
        //      to load 'external' classes, which are those that have no macro
        //      source, but are just wrappers around C++ classes. We go through
        //      them in order, and ask each one whether it can load the class.
        //      Once one does, we don't go any further down the chain. One is
        //      always installed for the built in runtime classes, but many
        //      applications will install their own. We own them.
        //
        //      We have to synchronize access to this collection, but we do
        //      it via the m_mtxSync mutex instead of making the collection
        //      thread safe, since we will do multiple accesses at a time
        //      and can avoid multiple locks and unlocks.
        //
        //  m_strEmptyClassTemplate
        //      Apps that let users edit/create macros often want to provide
        //      an empty template when the user create a new class. So we
        //      provide that text, to keep them from all doing it separately.
        //
        //  m_strMacroFileDescr
        //  m_strMacroFileSelWC
        //      Some commonly used strings that we want the outside world to
        //      be able to get from us.
        // -------------------------------------------------------------------
        TLoaderList     m_colLoaderList;
        TMutex          m_mtxSync;
        TString         m_strEmptyClassTemplate;
        TString         m_strMacroFileDescr;
        TString         m_strMacroFileSelWC;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDMacroEng,TFacility)
};

#pragma CIDLIB_POPPACK


