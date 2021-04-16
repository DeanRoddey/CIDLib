// ----------------------------------------------------------------------------
//  FILE: ORB1SampC_IntfClientProxy.hpp
//  DATE: Fri, Feb 12 21:10:40 2021 -0500
//    ID: E1CCC9CBCFA1FDAF-29A8F7CB04763265
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

class  TORB1SampClientProxy : public TOrbClientBase
{
    public :
        // --------------------------------------------------------------------
        // Public, static data
        // --------------------------------------------------------------------
        static const TString strInterfaceId;
        static const TString strBinding;



        // --------------------------------------------------------------------
        // Public Constructors and Destructor
        // --------------------------------------------------------------------
        TORB1SampClientProxy();
        TORB1SampClientProxy(const TOrbObjId& ooidSrc, const TString& strNSBinding);
        TORB1SampClientProxy(const TORB1SampClientProxy&) = delete;
        TORB1SampClientProxy(TORB1SampClientProxy&&) = delete;
        ~TORB1SampClientProxy();

        // --------------------------------------------------------------------
        // Public operators
        // --------------------------------------------------------------------
        TORB1SampClientProxy& operator=(const TORB1SampClientProxy&) = delete;
        TORB1SampClientProxy& operator=(TORB1SampClientProxy&&) = delete;

        // --------------------------------------------------------------------
        // Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid SetObjId
        (
            const   TOrbObjId& ooidToSet
            , const TString& strNSBinding
            , const tCIDLib::TBoolean bCheck = kCIDLib::True
        )   override;

        // --------------------------------------------------------------------
        // Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bQueryByName
        (
            const TString& strToFind
            , COP TORB1SampRec& recFound
        );

        tCIDLib::TBoolean bQueryAllStatus
        (
            const tORB1SampSh::EStatus eToFind
            , COP TVector<TORB1SampRec>& colFound
        );

    protected :
        // --------------------------------------------------------------------
        // Declare our friends
        // --------------------------------------------------------------------
        friend class TFacCIDOrb;

    private :
        // --------------------------------------------------------------------
        // Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TORB1SampClientProxy,TOrbClientBase)
};

#pragma CIDLIB_POPPACK

inline tCIDLib::TVoid
TORB1SampClientProxy::SetObjId(const  TOrbObjId& ooidToSet
              , const TString& strNSBinding
              , const tCIDLib::TBoolean bCheck)
{
    TParent::SetObjId(ooidToSet, strNSBinding, bCheck);
}

