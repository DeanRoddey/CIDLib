//
// FILE NAME: CIDMacroEng_NamedValMapClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/31/2004
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
//  This is the header file for the CIDMacroEng_NamedValMapClass.cpp file,
//  which implements a class that contains a list of named items, and each
//  named item contains one or more key/value pairs. So, a particular value
//  in the map is addressed by the key of the item and the sub-key of the
//  value within that item.
//
//  So the hierarchy is:
//
//      Item1 (which has a key)
//          SubKey1 - Value1
//              .
//              .
//          SubKeyX - ValueX
//              .
//              .
//              .
//              .
//      ItemX (which has a key)
//          SubKey1 - Value1
//              .
//              .
//          SubKeyX - ValueX
//
//  We use the standard TNamedValMap class, which provides this type of
//  functionality.
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
//  CLASS: TMEngNamedValMapVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngNamedValMapVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngNamedValMapVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngNamedValMapVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
            , const TNamedValMap<TString>&  nvmInit
        );

        TMEngNamedValMapVal(const TMEngNamedValMapVal&) = delete;

        ~TMEngNamedValMapVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngNamedValMapVal& operator=(const TMEngNamedValMapVal&) = delete;


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
        )   const override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendFrom
        (
            const   TNamedValMap<TString>&  nvmSrc
        );

        TNamedValMap<TString>& nvmItems();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_nvmItems
        //      The named value map which holds the data. It's templatized so
        //      it can hold any type of mapped values. We want them to be
        //      just strings.
        // -------------------------------------------------------------------
        TNamedValMap<TString>   m_nvmItems;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngNamedValMapVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngNamedValMapInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngNamedValMapInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngNamedValMapInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngNamedValMapInfo(const TMEngNamedValMapInfo&) = delete;

        ~TMEngNamedValMapInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngNamedValMapInfo& operator=(const TMEngNamedValMapInfo&) = delete;


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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowCIDErr
        (
                    TCIDMacroEngine&        meOwner
            , const TError&                 errCaught
            , const tCIDLib::TCard4         c4Default
            , const TString&                strKey
        );

        tCIDLib::TVoid ThrowCIDErr
        (
                    TCIDMacroEngine&        meOwner
            , const TError&                 errCaught
            , const tCIDLib::TCard4         c4Default
            , const TString&                strKey
            , const TString&                strSubKey
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
            , const MFormattable&           fmtblToken3
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c2TypeId_XXX
        //      The type ids of any nested types we create.
        //
        //  m_c4ErrId_XXXX
        //      To aid in throwing exceptions, we store the ids of the error
        //      enum values as we create them. This way, we don't have to
        //      do some kind of by name lookup each time.
        //
        //  m_pmeciErrors
        //      We need to keep around a pointer to the error enum that we
        //      create, because we need it when throwing an exception. We
        //      don't own it, we just keep a pointer around.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_AddItem;
        tCIDLib::TCard2 m_c2MethId_AddValue;
        tCIDLib::TCard2 m_c2MethId_ClearMap;
        tCIDLib::TCard2 m_c2MethId_ClearItem;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetValue;
        tCIDLib::TCard2 m_c2MethId_GetValueIfExists;
        tCIDLib::TCard2 m_c2MethId_IsEmpty;
        tCIDLib::TCard2 m_c2MethId_ItemExists;
        tCIDLib::TCard2 m_c2MethId_ItemIsEmpty;
        tCIDLib::TCard2 m_c2MethId_RemoveItem;
        tCIDLib::TCard2 m_c2MethId_RemoveSubKey;
        tCIDLib::TCard2 m_c2MethId_SetValue;
        tCIDLib::TCard2 m_c2MethId_SubKeyExists;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard4 m_c4ErrId_AddItem;
        tCIDLib::TCard4 m_c4ErrId_AddValue;
        tCIDLib::TCard4 m_c4ErrId_ClearItem;
        tCIDLib::TCard4 m_c4ErrId_GetValue;
        tCIDLib::TCard4 m_c4ErrId_KeyAlreadyExists;
        tCIDLib::TCard4 m_c4ErrId_KeyNotFound;
        tCIDLib::TCard4 m_c4ErrId_RemoveItem;
        tCIDLib::TCard4 m_c4ErrId_RemoveValue;
        tCIDLib::TCard4 m_c4ErrId_SetValue;
        tCIDLib::TCard4 m_c4ErrId_SubKeyAlreadyExists;
        tCIDLib::TCard4 m_c4ErrId_SubKeyNotFound;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngNamedValMapInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



