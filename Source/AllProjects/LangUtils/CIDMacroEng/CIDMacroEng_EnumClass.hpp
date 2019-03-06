//
// FILE NAME: CIDMacroEng_EnumClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_EnumClass.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data object for that class) and another that provides the value storage
//  for instances of that class.
//
//  We provide these derivatives here for the MEng.Enum class, which is the
//  base enumerated class from which all user enumerated types are derived.
//  MEng.Enum itself cannot be created directly, but we need a common form
//  to store enumerated types and to deal with them polymorphically internally
//  at the C++ level.
//
//  An enum is a list of items, each of which has three values, an ordinal
//  numeric value (0 based, consequtive numbers) which is implied by the
//  ordering of the items, the parsed representation (what is used in the source
//  code to indicate a value of that type), and a text representation that is
//  used if the enum is formatted to an output text stream.
//
//  In the case of enums, we don't have a one to one relationship between
//  C++ class and macro language class. We create new instances of the info
//  and value class, with a different class path, for each enum defined. The
//  factory method of the info class just sets the right class path on the
//  value objects it creates. But, since we use the same value class for all
//  enums, we don't have to store the values as macro level member objects.
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
//  CLASS: TMEngEnumVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngEnumVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngEnumVal() = delete;

        TMEngEnumVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCard4         c4MaxOrdinal
            , const tCIDLib::TCard4         c4InitOrdinal = 0
        );

        TMEngEnumVal(const TMEngEnumVal&) = delete;

        ~TMEngEnumVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngEnumVal& operator=(const TMEngEnumVal&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAtMax() const;

        tCIDLib::TBoolean bAtMin() const;

        tCIDLib::TCard4 c4Ordinal() const;

        tCIDLib::TCard4 c4Ordinal
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4MaxOrdinal() const;

        tCIDLib::TVoid Decrement();

        tCIDLib::TVoid Increment();

        tCIDLib::TVoid SetToMax();

        tCIDLib::TVoid SetToMin();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c4MaxOrdValue
        //      Though this is available via the class info class, we get our
        //      own copy of it so that range checking can be done without
        //      having to go back to the class object.
        //
        //  m_c4OrdValue
        //      The current value of the enum, as an ordinal value. The named
        //      values exist only to the outside world. Interally, we just
        //      store the
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MaxOrdValue;
        tCIDLib::TCard4     m_c4OrdValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngEnumVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngEnumInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngEnumInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //   CLASS: Item
        //  PREFIX: item
        // -------------------------------------------------------------------
        class TItem
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TItem();

                TItem
                (
                    const   TString&        strName
                    , const TString&        strText
                    , const tCIDLib::TCard4 c4MapVal
                );

                TItem
                (
                    const   TItem&          itemToCopy
                );

                ~TItem();


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TItem& operator=
                (
                    const   TItem&          itemToAssign
                );


                // -----------------------------------------------------------
                //  Public data members
                //
                //  m_c4MapVal
                //      Sometimes we need to map enum values in the macro
                //      language to underlying enum values in the C++ world,
                //      so this let's us store a value to map to.
                //
                //  m_strName
                //  m_strText
                //      The name and text for this enum item. The ordinal is
                //      implicit in it's order of addition.
                // -----------------------------------------------------------
                tCIDLib::TCard4 m_c4MapVal;
                TString         m_strName;
                TString         m_strText;
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngEnumInfo() = delete;

        TMEngEnumInfo(const TMEngEnumInfo&) = delete;

        TMEngEnumInfo
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strName
            , const TString&                strBasePath
            , const TString&                strParentClassPath
            , const tCIDLib::TCard4         c4ExpectedItems
        );

        ~TMEngEnumInfo();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngEnumInfo& operator=(const TMEngEnumInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanCastFrom
        (
            const   tCIDLib::TCard2         c2SrcId
        )   override;

        tCIDMacroEng::ECastRes eCastFrom
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngClassVal&          mecvSrc
            ,       TMEngClassVal&          mecvTarget
        )   override;

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
        tCIDLib::TBoolean bItemExists
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard4 c4AddEnumItem(  const   TString&    strName
                                        , const TString&    strText)
        {
            return c4AddEnum(strName, strText, 0);
        }

        template <typename T>
        tCIDLib::TCard4 c4AddEnumItem(  const   TString&    strName
                                        , const TString&    strText
                                        , const T           eMapValue)
        {
            return c4AddEnum(strName, strText, tCIDLib::TCard4(eMapValue));
        }

        tCIDLib::TCard4 c4FindOrdinal
        (
            const   TString&                strName
        )   const;

        template <typename T>
        tCIDLib::TCard4 c4FromMapValue(const T eToFind) const
        {
            return c4FromMap(tCIDLib::c4EnumOrd(eToFind));
        }

        tCIDLib::TCard4 c4MapValue
        (
            const   TMEngEnumVal&           mecvValue
        )   const;

        tCIDLib::TCard4 c4OrdinalFromText
        (
            const   TString&                strText
        )   const;

        tCIDLib::TCard4 c4ValueCount() const;

        TString strFullName
        (
            const   TMEngEnumVal&           mecvValue
        )   const;

        TString strFullName
        (
            const   tCIDLib::TCard4         c4Ordinal
        )   const;

        const TString& strItemName
        (
            const   TMEngEnumVal&           mecvValue
        )   const;

        const TString& strItemName
        (
            const   tCIDLib::TCard4         c4Ordinal
        )   const;

        TString strPartialName
        (
            const   TMEngEnumVal&           mecvValue
        )   const;

        TString strPartialName
        (
            const   tCIDLib::TCard4         c4Ordinal
        )   const;

        const TString& strTextValue
        (
            const   TMEngEnumVal&           mecvValue
        )   const;

        const TString& strTextValue
        (
            const   tCIDLib::TCard4         c4Ordinal
        )   const;


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


        // -------------------------------------------------------------------
        //  Protected class types
        // -------------------------------------------------------------------
        enum class EBaseErrs
        {
            BadOrdinal
            , NameNotFound
            , TextNotFound
        };


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowBaseErr
        (
                    TCIDMacroEngine&        meOwner
            , const EBaseErrs               eToThrow
            , const MFormattable&           fmtblToken1
        );

        tCIDLib::TVoid ThrowBaseErr
        (
                    TCIDMacroEngine&        meOwner
            , const EBaseErrs               eToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AddEnum
        (
            const   TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4MapValue
        );

        tCIDLib::TCard4 c4FromMap
        (
            const   tCIDLib::TCard4         c4ToFind
        )   const;

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_colItems
        //      The list of items for this enum class. The position implies
        //      the ordinal value.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_AtMax;
        tCIDLib::TCard2 m_c2MethId_AtMin;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_FromName;
        tCIDLib::TCard2 m_c2MethId_FromText;
        tCIDLib::TCard2 m_c2MethId_FormatList;
        tCIDLib::TCard2 m_c2MethId_GetOrd;
        tCIDLib::TCard2 m_c2MethId_GetOrdCount;
        tCIDLib::TCard2 m_c2MethId_GetName;
        tCIDLib::TCard2 m_c2MethId_GetText;
        tCIDLib::TCard2 m_c2MethId_GreaterThan;
        tCIDLib::TCard2 m_c2MethId_GreaterThanEq;
        tCIDLib::TCard2 m_c2MethId_Inc;
        tCIDLib::TCard2 m_c2MethId_LessThan;
        tCIDLib::TCard2 m_c2MethId_LessThanEq;
        tCIDLib::TCard2 m_c2MethId_MapFromText;
        tCIDLib::TCard2 m_c2MethId_SetMax;
        tCIDLib::TCard2 m_c2MethId_SetMin;
        tCIDLib::TCard2 m_c2MethId_SetOrd;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard4 m_c4ErrBadOrdinal;
        tCIDLib::TCard4 m_c4ErrNameNotFound;
        tCIDLib::TCard4 m_c4ErrTextNotFound;
        TVector<TItem>  m_colItems;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngEnumInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



