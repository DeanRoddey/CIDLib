//
// FILE NAME: CIDMacroEng_ArrayClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/10/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ArrayClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class that implements an array of objects. It is actually an abstract
//  class that must be derived from. And that derivation is done automagically
//  by the compiler, when it sees a templatized array type declaration.
//
//  So the Info class is designed to take ctor parameters that tell it what
//  it's class details are, what type of class it's elements will be, and the
//  number of elements. And internally, it's Init() method will do one set
//  of methods for the base class and another for the derived class, so it
//  creates a type safe interface for each instantiation.
//
//  All derived classes will use the same value object, so we don't have to
//  worry about derivation issues wrt to member storage. We just create our
//  own value class that holds the C++ level array. The C++ array works
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
//  CLASS: TMEngArrayVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngArrayVal : public TMEngCollectVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngArrayVal() = delete;

        TMEngArrayVal(const TMEngArrayVal&)  = delete;

        TMEngArrayVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard2         c2ElemId
            , const tCIDMacroEng::EConstTypes eConst
            ,       TMEngCollectInfo*       pmeciBase
        );

        ~TMEngArrayVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngArrayVal& operator=(const TMEngArrayVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts  eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;

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
        tCIDLib::TCard4 c4ElemCount() const;

        tCIDLib::TVoid Initialize
        (
            const   tCIDLib::TCard4         c4ElemCount
        );

        const TMEngClassVal& mecvAt
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid Reallocate
        (
            const   tCIDLib::TCard4         c4ElemCount
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CleanUpArray();

        tCIDLib::TVoid FaultInElem
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4At
        );


        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c4ElemCount
        //      This is the number of elements at the CML level. We don't
        //      get this till the object is constructed at the CML level, so
        //      it is initialized to zero until then.
        //
        //  m_pElems
        //      This is an array of class value objects that make up the
        //      storage for the array. We won't allocate it until the CML
        //      level constructor is called, which will tell us the element
        //      count we will have.
        //
        //      Note that, to minimize overhead, we fault in the elements
        //      upon need.
        //
        //  m_pmeciBase
        //      Our class class gives us a pointer to the base collection
        //      macro level class info class, so that we can throw macro
        //      level exceptions that it provides.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ElemCount;
        TMEngClassVal**     m_pElems;
        TMEngCollectInfo*   m_pmeciBase;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngArrayVal,TMEngCollectVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngArrayInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngArrayInfo : public TMEngColBaseInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngArrayInfo() = delete;

        TMEngArrayInfo
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strName
            , const TString&                strBasePath
            , const TString&                strParentClassPath
            , const tCIDLib::TCard2         c2ElemId
        );

        TMEngArrayInfo(const TMEngArrayInfo&) = delete;

        ~TMEngArrayInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngArrayInfo& operator=(const TMEngArrayInfo&) = delete;


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
        //  m_pmeciParent
        //      During init we look up the class info for our CML level base
        //      collection class, which provides us with some common errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2MethId_Ctor;
        tCIDLib::TCard2     m_c2MethId_GetElemCount;
        tCIDLib::TCard2     m_c2MethId_Reallocate;
        TMEngCollectInfo*   m_pmeciBase;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngArrayInfo,TMEngColBaseInfo)
};

#pragma CIDLIB_POPPACK


