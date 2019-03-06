//
// FILE NAME: CIDMacroEng_MethodInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_MethodInfo.cpp file, which
//  implements the TMEngMethodInfo class. This class obviously provides
//  some information about a method. Because of inheritance, there can be
//  multiple implementations of a method (in the various derived classes of
//  a base class that defines it), but only one definition of the method,
//  which is the name, the return type, the parameters it takes, it's
//  visibility and other attributes.
//
//  So the base class that defines a method creates a method info object and
//  stores it, and that is used to validate parameters and such. Each derived
//  class that overrides the method will provide its own TMEngMethodImpl
//  object, which contains the compiled opcodes and the locals descriptions.
//  The method objects also store the name and method id so that they can be
//  correlated back to the method info.
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
//  CLASS: TMEngMethodInfo
// PREFIX: methi
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngMethodInfo : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TParmList = TVector<TMEngParmInfo>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TMEngMethodInfo& meinfoSrc);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMethodInfo();

        TMEngMethodInfo
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2RetTypeId
            , const tCIDMacroEng::EVisTypes    eVisibility
            , const tCIDMacroEng::EMethExt     eExtend
            , const tCIDMacroEng::EConstTypes  eConst = tCIDMacroEng::EConstTypes::NonConst
        );

        TMEngMethodInfo
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eRetType
            , const tCIDMacroEng::EVisTypes    eVisibility
            , const tCIDMacroEng::EMethExt     eExtend
            , const tCIDMacroEng::EConstTypes  eConst = tCIDMacroEng::EConstTypes::NonConst
        );

        TMEngMethodInfo
        (
            const   TMEngMethodInfo&        methiToCopy
        );

        ~TMEngMethodInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMethodInfo& operator=
        (
            const   TMEngMethodInfo&        methiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasParm
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bIsConst() const;

        tCIDLib::TBoolean bIsCtor() const;

        tCIDLib::TBoolean bIsCtor
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bIsLegalOverride
        (
            const   TMEngMethodInfo&        methiToCheck
        );

        tCIDLib::TCard2 c2AddParm
        (
            const   TMEngParmInfo&          mepiToAdd
        );

        tCIDLib::TCard2 c2AddInParm
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
        );

        tCIDLib::TCard2 c2AddInParm
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eClassType
        );

        tCIDLib::TCard2 c2AddInOutParm
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
        );

        tCIDLib::TCard2 c2AddInOutParm
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eClassType
        );

        tCIDLib::TCard2 c2AddOutParm
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
        );

        tCIDLib::TCard2 c2AddOutParm
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eClassType
        );

        tCIDLib::TCard2 c2FindParm
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard2 c2RetClassId() const;

        tCIDLib::TCard2 c2RetClassId
        (
            const   tCIDLib::TCard2         c2ToSet
        );

        tCIDLib::TCard4 c4ParmCount() const;

        tCIDMacroEng::EConstTypes eConst() const;

        tCIDMacroEng::EConstTypes eConst
        (
            const   tCIDMacroEng::EConstTypes  eToSet
        );

        tCIDMacroEng::EMethExt eExtend() const;

        tCIDMacroEng::EMethExt eExtend
        (
            const   tCIDMacroEng::EMethExt     eToSet
        );

        tCIDMacroEng::EVisTypes eVisibility() const;

        const TMEngParmInfo& mepiFind
        (
            const   tCIDLib::TCard2         c2Id
        )   const;

        TMEngParmInfo& mepiFind
        (
            const   tCIDLib::TCard2         c2Id
        );

        const TMEngParmInfo* pmepiFind
        (
            const   TString&                strName
        )   const;

        TMEngParmInfo* pmepiFind
        (
            const   TString&                strName
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCtor
        //      Indicates if this is a constructor or just a regular method.
        //      Constructor methods have special constraints.
        //
        //  m_c2RetClassId
        //      The class id of the return value. If none, then it will be
        //      the id of the void type.
        //
        //  m_colParms
        //      This is our list of parameters. We store them as a vector
        //      since they are always accessed either sequentially or via
        //      index, and we know how many will be required at ctor time.
        //      As with all other entities, they are given sequential, zero
        //      based, ids as they are added, so the index is the id.
        //
        //  m_eConst
        //      Indicates whether this is a const method or not. A const
        //      method cannot modify any class members, and a non-const method
        //      cannot be called on a const object.
        //
        //  m_eExtend
        //      The type of extension allowed for this method at the current
        //      class level.
        //
        //  m_eVisTypes
        //      The visibility of this method.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bCtor;
        tCIDLib::TCard2             m_c2RetClassId;
        TParmList                   m_colParmList;
        tCIDMacroEng::EConstTypes  m_eConst;
        tCIDMacroEng::EMethExt     m_eExtend;
        tCIDMacroEng::EVisTypes    m_eVisibility;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMethodInfo,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK


