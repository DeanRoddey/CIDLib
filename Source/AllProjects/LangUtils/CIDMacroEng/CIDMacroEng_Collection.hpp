//
// FILE NAME: CIDMacroEng_Collection.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2003
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
//  This is the header file for the CIDMacroEng_Collection.cpp file, which
//  implements the base class from which all of the collection class' value
//  classes are derived. And the class type class for the macro level base
//  class for all collections.
//
//  Collections are kind of special case, since they contain objects which are
//  not their own members. So, unlike members, when a collection's elements
//  are accessed, they are not copied, but are looked at directly. A special
//  opcode supports this, and special syntax, using [] brackets to access a
//  particular element, is used to know when this type of access is being done.
//
//  So we have to provide a special method to the special opcode, so that he
//  can pass us a value object that represents an index, and we give him
//  back a pointer to the value object, which he will then put on the stack
//  top, getting rid of the index value that was there.
//
//  Since there will be a set of collection classes, we need to provide a
//  base class via which the special indexing opcode can work.
//
//  We also provide an info class for the Collection macro level class, which
//  is what all collections derive from at the macro level. This allows us
//  to provide some common exceptions and maybe some functionality.
//
//  But there is no C++ inheritance there! The individual collection class
//  info classes at the C++ level will derive from TMEngColBaseInfo class,
//  which just provides a way polymorphically get to information like the
//  element id.
//
//  Note that the base (CML level) collection class indicates that it is
//  copyable, so each derived (at the CML level) class can be copyable if
//  it wants to be.
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
//  CLASS: TMEngCollectVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCollectVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCollectVal() = delete;

        TMEngCollectVal(const TMEngCollectVal&) = delete;

        ~TMEngCollectVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCollectVal& operator=(const TMEngCollectVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual TMEngClassVal* pmecvGetAt
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngClassVal&          mecvIndex
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2ElemId() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TMEngCollectVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard2         c2ElemId
            , const tCIDMacroEng::EConstTypes  eConst
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ElemId
        //      The class id of the elements we are to store, i.e. what we are
        //      storing at the macro level. At the C++ level we just store
        //      class value objects polymorphically.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2ElemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCollectVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCollectInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCollectInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TMEngCollectInfo() = delete;

        TMEngCollectInfo(const TMEngCollectInfo&) = delete;

        TMEngCollectInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        ~TMEngCollectInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCollectInfo& operator=(const TMEngCollectInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid IndexErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4Index
            , const TString&                strClassPath
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ErrId
        //      We create an error enum nested class, which provides some
        //      common errors thrown by all collections. This is the error
        //      id.
        //
        //  m_c4ErrXXXX
        //      The ids of the errors we create. We rememember them so that
        //      we can thrown them easily.
        //
        //  m_pmeciErrors
        //      We keep around a pointer to our nested error enum class
        //      info object, so that we can throw errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2ErrId;
        tCIDLib::TCard4 m_c4ErrBadInd;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCollectInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngColBaseInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngColBaseInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TMEngColBaseInfo() = delete;

        TMEngColBaseInfo(const TMEngColBaseInfo&) = delete;

        ~TMEngColBaseInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngColBaseInfo& operator=(const TMEngColBaseInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2ElemId() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructor
        // -------------------------------------------------------------------
        TMEngColBaseInfo
        (
            const   TString&                strName
            , const TString&                strBasePath
            ,       TCIDMacroEngine&        meOwner
            , const tCIDLib::TBoolean       bCopyable
            , const tCIDMacroEng::EClassExt    eExtend
            , const TString&                strParentClass
            , const tCIDLib::TCard2         c2ElemId
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ElemId
        //      The class id of the element type of the collection.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2ElemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngColBaseInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


