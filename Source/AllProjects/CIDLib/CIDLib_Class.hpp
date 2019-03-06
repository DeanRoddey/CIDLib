//
// FILE NAME: CIDLib_Class.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/02/1995
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header holds one of the most primal classes in the system. It is
//  the class class. Every class is given a static const member of this type
//  which holds its class type. At the same time it is not the most primal
//  since it is derived from TObject. But this one is still more primal in
//  a way because all of the RTTI methods depend upon it. This of course
//  makes this class special due to the circular nature of its relationship
//  with TObject.
//
//  This class provides a number of static methods that provide the under
//  pinnings of the RTTI system. Also provided here are a few template methods
//  that support the advanced RTTI stuff in a typesafe way.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because of the very special nature of this class, it cannot use
//      the standard members macros (because of order of inclusion stuff.)
//      So it does the standard RTTI members manually.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TString;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TClass
// PREFIX: cls
// ---------------------------------------------------------------------------
class CIDLIBEXP TClass :

    public TObject, public MFormattable, public MStreamable, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        //
        //  Some of these are implemented in CIDLib_TypeRegistry.Cpp!
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsClassRegistered
        (
            const   TString&                strClassName
        );

        static tCIDLib::TBoolean bIsClassRegistered
        (
            const   tCIDLib::TCh* const     pszClassName
        );

        static TClass clsForClass
        (
            const   TString&                strClassName
        );

        static TClass clsForClass
        (
            const   tCIDLib::TCh* const     pszClassName
        );

        static TObject* pobjMakeNewOfClass
        (
            const   tCIDLib::TCh* const     pszClassName
        );

        static TObject* pobjMakeNewOfClass
        (
            const   TString&                strClassName
        );

        static tCIDLib::TVoid RegisterClass
        (
            const   TString&                strToRegister
            ,       tCIDLib::TObjFactory    pFactoryFunc
        );

        static tCIDLib::TVoid RegisterClass
        (
            const   tCIDLib::TCh* const     pszToRegister
            ,       tCIDLib::TObjFactory    pFactoryFunc
        );

        static tCIDLib::TVoid ThrowBadCast
        (
            const   TString&                strFromClass
            , const TString&                strToClass
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TClass();

        TClass
        (
            const   tCIDLib::TCh* const     pszClassName
        );

        TClass
        (
            const   TString&                strClassName
        );

        TClass
        (
            const   TClass&                 clsToCopy
        );

        ~TClass();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator<
        (
            const   TClass&                 clsToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TClass&                 clsToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TClass&                 clsToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TClass&                 clsToCompare
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TClass&                 clsToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TClass&                 clsToTest
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TCh* const     pszClassName
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TCh* const     pszClassName
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TString&                strClassName
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TString&                strClassName
        )   const;

        operator const tCIDLib::TCh*() const;

        TClass& operator=
        (
            const   TClass&                 clsToAssign
        );



        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&              clsTarget
        )   const override;

        const TClass& clsIsA() const override;

        const TClass& clsParent() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanFactoryCreate() const;

        tCIDLib::TBoolean bIsNameValid() const;

        tCIDLib::THashVal hshInternal() const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4      c4Modulus
        )   const;

        TObject* pobjMakeNew() const;

        TString strClassName() const;

        const tCIDLib::TCh* pszClassName() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private         :
        // -------------------------------------------------------------------
        //  Private types
        // -------------------------------------------------------------------
        using TParent = TObject;



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufChars
        //      This is the number of characters that the class name buffer
        //      can currently hold. This lets us not necessarily reallocate
        //      it if a new name is assigned that is <= the length of the
        //      current name.
        //
        //  m_hshName
        //      This is used to store the hash value for the name. Since
        //      class objects are often compared in time critical code (as
        //      part of RTTI checks), this helps speed things up a lot.
        //
        //  m_pszClassName
        //      This is the storage for the class name.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BufChars;
        tCIDLib::THashVal   m_hshName;
        tCIDLib::TCh*       m_pszClassName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TClass)
        NulObject(TClass)
};


class CIDLIBEXP TTypeFactoryKicker
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTypeFactoryKicker() = delete;

        TTypeFactoryKicker
        (
            const   tCIDLib::TCh* const     pszClassName
            ,       tCIDLib::TObjFactory    pfnFactory
        );

        TTypeFactoryKicker(const TTypeFactoryKicker&) = delete;

        ~TTypeFactoryKicker();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
         TTypeFactoryKicker& operator==(const TTypeFactoryKicker&) = delete;
};


#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Some template based functions that build upon the pobjMakeNewOfType()
//  method of the facility class above. These automatically invoke the method
//  and do the correct casting.
// ---------------------------------------------------------------------------
template <class T> T* pobjMakeNewOfClass(const tCIDLib::TCh* pszClassName)
{
    // Create it as a base object type first, since we know that's ok
    TObject* const pobjTmp = TClass::pobjMakeNewOfClass(pszClassName);

    // Put a janitor on it until we test the cast
    TJanitor<TObject> janTest(pobjTmp);

    // Do the cast and orphan the object
    T* pobjRet = dynamic_cast<T*>(pobjTmp);
    if (!pobjRet)
        TClass::ThrowBadCast(pszClassName, T::clsThis().strClassName());
    janTest.Orphan();

    // And return the object
    return pobjRet;
}

template <class T> T* pobjMakeNewOfClass(const TString& strClassName)
{
    // Create it as a base object type first, since we know that's ok
    TObject* const pobjTmp = TClass::pobjMakeNewOfClass(strClassName);

    // Put a janitor on it until we test the cast
    TJanitor<TObject> janTest(pobjTmp);

    // Do the cast and orphan the object now
    T* pobjRet = dynamic_cast<T*>(pobjTmp);
    if (!pobjRet)
        TClass::ThrowBadCast(strClassName, T::clsThis().strClassName());
    janTest.Orphan();

    // And return the new object
    return pobjRet;
}

template <class T> T* pobjMakeNewOfClass(const TClass& clsToMake)
{
    // Create it as a base object type first, since we know that's ok
    TObject* const pobjTmp = clsToMake.pobjMakeNew();

    // Put a janitor on it until we test the cast
    TJanitor<TObject> janTest(pobjTmp);

    // Do the cast and orphan the object now
    T* pobjRet = dynamic_cast<T*>(pobjTmp);
    if (!pobjRet)
        TClass::ThrowBadCast(clsToMake.strClassName(), T::clsThis().strClassName());
    janTest.Orphan();

    // And return the new object
    return pobjRet;
}


