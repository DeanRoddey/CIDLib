//
// FILE NAME: CIDLib_Duplicable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/06/1992
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Duplicable.Cpp file. This file
//  defines the simple polymorphic duplicability mixin class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TClass;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MDuplicable
// PREFIX: dup
// ---------------------------------------------------------------------------
class CIDLIBEXP MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable&            dupToCast
            , const TClass&                 clsToCastTo
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable* const      pdupToCast
            , const TClass&                 clsToCastTo
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable&            dupToCast
            , const tCIDLib::TCh* const     pszTypeName
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable* const      pdupToCast
            , const tCIDLib::TCh* const     pszTypeName
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~MDuplicable() = default;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&                 clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;

        virtual TObject* pobjDuplicate() const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators, so that including classes can still
        //  have default copy/assignment
        // -------------------------------------------------------------------
        MDuplicable() = default;
        MDuplicable(const MDuplicable&) = default;
        MDuplicable& operator=(const MDuplicable&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        NulObject(MDuplicable)
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Inlines for polymorphic duplication
// ---------------------------------------------------------------------------
template <class T> T* pDupObject(const MDuplicable& dupToDup)
{
    // Optimize because this is a quick hashed compare
    MDuplicable::TestCanDupTo(dupToDup, T::clsThis());
    return static_cast<T*>(dupToDup.pobjDuplicate());
}

template <class T> T* pDupObject(const MDuplicable* const pdupToDup)
{
    // Optimize because this is a quick hashed compare
    MDuplicable::TestCanDupTo(pdupToDup, T::clsThis());
    return static_cast<T*>(pdupToDup->pobjDuplicate());
}


// ---------------------------------------------------------------------------
//  This macro will provide the implemention of pobjDuplicate(). The class must
//  support copy construction.
// ---------------------------------------------------------------------------
#define DefPolyDup(type) \
public : \
TObject* pobjDuplicate() const \
{ \
    return new type(*this); \
}
