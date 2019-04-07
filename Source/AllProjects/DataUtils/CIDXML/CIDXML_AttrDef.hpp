//
// FILE NAME: CIDXML_AttDef.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/1999
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
//  This is the header file for the CIDXML_AttDef.Cpp file, which implements
//  The TXMLAttrDef class. This is the basic definition for an attribute def
//  which defines the name of an attribute, what type it is, any default or
//  fixed value, etc...
//
//  This is an abstract base class which will be derived from by each type of
//  validator, to create an attribute def that is correct for that type of
//  validator. Most of the functionality though is provided here since XML
//  1.0 rules dictate much of what an attribute def must be like.
//
//  If the type is Notation or Enumeration, then we maintain a string pool
//  of the available values for later use. Otherwise, we just maintain any
//  default or fixed value as a normalized string.
//
// CAVEATS/GOTCHAS:
//
//  1)  Since we store ids to things, not the text. We have to have the
//      validator (which has the pools) in order to format anything to text
//      for user viewing.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLAttrDef
// PREFIX: xad
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLAttrDef : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TCursor = TBasicDLinkedCol<TString>::TCursor;


        // -------------------------------------------------------------------
        //  Public Destructor
        // -------------------------------------------------------------------
        ~TXMLAttrDef();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const = 0;

        virtual const TString& strFullName() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsInEnumList
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bIsInEnumList
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bProvided() const;

        tCIDLib::TBoolean bProvided
        (
            const   tCIDLib::TBoolean       bNewValue
        );

        tCIDLib::TBoolean bMultiValued() const;

        tCIDLib::TBoolean bNamedType() const;

        tCIDLib::TBoolean bRefType() const;

        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4Id
        );

        tCIDXML::EDefAttrTypes eDefaultType() const;

        tCIDXML::EDefAttrTypes eDefaultType
        (
            const   tCIDXML::EDefAttrTypes  eNewType
        );

        tCIDXML::EAttrTypes eType() const;

        tCIDXML::EAttrTypes eType
        (
            const   tCIDXML::EAttrTypes     eNewType
        );

        TXMLAttrDef::TCursor cursEnumValues() const;

        tCIDLib::TVoid SetEnumValues
        (
            const   TString&                strValueList
        );

        const TString& strValue() const;

        const TString& strValue
        (
            const   TString&                strNewValue
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TXMLAttrDef();

        TXMLAttrDef
        (
            const   tCIDLib::TCh* const     pszValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDXML::EDefAttrTypes  eDefaultType
        );

        TXMLAttrDef
        (
            const   TString&                strValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDXML::EDefAttrTypes  eDefaultType
        );

        TXMLAttrDef
        (
            const   TXMLAttrDef&             xadToCopy
        );

        TXMLAttrDef& operator=
        (
            const   TXMLAttrDef&             xadToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bProvided
        //      This flag is used when parsing a start tag. As we find each
        //      attribute, we mark it as provided. After we're done we can
        //      go back through the list and see what was provided (and fault
        //      in the ones that weren't and have default/fixed values.)
        //
        //  m_c4Id
        //      This is the pool id for the attribute. This is only unique
        //      within the owning element.
        //
        //  m_eDefType
        //      This is the default type for this attribute, which indicates
        //      whether the value is required, implied, fixed.
        //
        //  m_eType
        //      This is the type of the attribute, which controls its
        //      normalization.
        //
        //  m_pcolMultiValues
        //      If the attribute is of an enum or notation type, then this
        //      holds the possible values. Its only allocated if needed.
        //
        //  m_strValue
        //      If the attribute def has any default or fixed value, it is
        //      stored here.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bProvided;
        tCIDLib::TCard4         m_c4Id;
        tCIDXML::EDefAttrTypes  m_eDefType;
        tCIDXML::EAttrTypes     m_eType;
        mutable TBag<TString>*  m_pcolEnumValues;
        TString                 m_strValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLAttrDef,TObject)
};


#pragma CIDLIB_POPPACK

