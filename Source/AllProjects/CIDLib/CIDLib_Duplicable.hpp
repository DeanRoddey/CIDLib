//
// FILE NAME: CIDLib_Duplicable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/06/1992
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
// PREFIX: mdup
// ---------------------------------------------------------------------------
class CIDLIBEXP MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable&            mdupToCast
            , const TClass&                 clsToCastTo
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable* const      pmdupToCast
            , const TClass&                 clsToCastTo
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable&            mdupToCast
            , const tCIDLib::TCh* const     pszTypeName
        );

        static tCIDLib::TVoid TestCanDupTo
        (
            const   MDuplicable* const      pmdupToCast
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

        [[nodiscard]] virtual TObject* pobjDuplicate() const = 0;


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
template <class T> T* pDupObject(const MDuplicable& mdupToDup)
{
    // Optimize because this is a quick hashed compare
    MDuplicable::TestCanDupTo(mdupToDup, T::clsThis());
    return static_cast<T*>(mdupToDup.pobjDuplicate());
}

template <class T> T* pDupObject(const MDuplicable* const pmdupToDup)
{
    // Optimize because this is a quick hashed compare
    MDuplicable::TestCanDupTo(pmdupToDup, T::clsThis());
    return static_cast<T*>(pmdupToDup->pobjDuplicate());
}


// ---------------------------------------------------------------------------
//  This macro will provide the implemention of pobjDuplicate(). The class must
//  support copy construction.
// ---------------------------------------------------------------------------
#define DefPolyDup(type) \
public : \
TObject* pobjDuplicate() const override \
{ \
    return new type(*this); \
}
