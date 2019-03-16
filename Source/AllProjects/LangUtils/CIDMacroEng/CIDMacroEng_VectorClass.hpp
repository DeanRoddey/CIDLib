//
// FILE NAME: CIDMacroEng_VectorClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_VectorClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class that implements a vector of objects. It is actually an abstract
//  class that must be derived from. And that derivation is done automagically
//  by the compiler, when it sees a templatized vector type declaration.
//
//  So the Info class is designed to take ctor parameters that tell it what
//  it's class details are and what type of class it's elements will be. And
//  internally, it's Init() method will do one set of methods for the base
//  class and another for the derived class, so it creates a type safe
//  interface for each instantiation.
//
//  All derived classes will use the same value object, so we don't have to
//  worry about derivation issues wrt to member storage. We just create our
//  own value class that holds the C++ level vector. The C++ vector works
//  at the TMEngClassVal level, holding a set of value objects via that
//  abstract base class that all class value classes derive from.
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
//  CLASS: TMEngVectorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngVectorVal : public TMEngCollectVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngVectorVal() = delete;

        TMEngVectorVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard2         c2ElemId
            , const tCIDMacroEng::EConstTypes  eConst
            ,       TMEngCollectInfo*       pmeciBase
        );

        TMEngVectorVal(const TMEngVectorVal&) = delete;

        ~TMEngVectorVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngVectorVal& operator=(const TMEngVectorVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvGetAt
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngClassVal&          mecvIndex
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddObject
        (
                    TMEngClassVal* const    pmecvToAdd
        );

        tCIDLib::TCard4 c4ElemCount() const;

        tCIDLib::TVoid InsertObject
        (
                    TCIDMacroEngine&        meOwner
            ,       TMEngClassVal* const    pmecvToAdd
            , const tCIDLib::TCard4         c4At
        );

        const TMEngClassVal& mecvAt
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4At
        )   const;

        TMEngClassVal& mecvAt
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveAt
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4At
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_colElems
        //      This is collection that we store the macro level value objects
        //      in. It's an adopting by ref vector.
        //
        //  m_pmeciBase
        //      Our class class gives us a pointer to the base collection
        //      macro level class info class, so that we can throw macro
        //      level exceptions that it provides.
        // -------------------------------------------------------------------
        tCIDMacroEng::TClassValList m_colElems;
        TMEngCollectInfo*           m_pmeciBase;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngVectorVal,TMEngCollectVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngVectorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngVectorInfo : public TMEngColBaseInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngVectorInfo() = delete;

        TMEngVectorInfo
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strName
            , const TString&                strBasePath
            , const TString&                strParentClassPath
            , const tCIDLib::TCard2         c2ElemId
        );

        TMEngVectorInfo(const TMEngVectorInfo&) = delete;

        ~TMEngVectorInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngVectorInfo& operator=(const TMEngVectorInfo&) = delete;


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
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_pmeciBase
        //      During init we look up the class info for our CML level base
        //      collection class, which provides us with some common errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2MethId_AddObject;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_GetElemCount;
        tCIDLib::TCard2     m_c2MethId_InsertObject;
        tCIDLib::TCard2     m_c2MethId_IsEmpty;
        tCIDLib::TCard2     m_c2MethId_RemoveAll;
        tCIDLib::TCard2     m_c2MethId_RemoveAt;
        TMEngCollectInfo*   m_pmeciBase;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngVectorInfo,TMEngColBaseInfo)
};

#pragma CIDLIB_POPPACK


