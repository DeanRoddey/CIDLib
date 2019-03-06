//
// FILE NAME: CIDXML_DTDAttDef.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/09/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_DTDAttDef.cpp file, which implements
//  The TDTDAttrDef class. This is a simple derivative of the basic TXMLAttrDef
//  class. This class creates a DTD specific implementation of an attribute
//  definition.
//
//  Objects of this type are stored in a hash table by the DTD element class,
//  so we make a key ops class for it, which the THashSet class needs.
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
//  CLASS: TDTDAttrDef
// PREFIX: xad
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDAttrDef : public TXMLAttrDef
{
    public  :
        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        static const TString& strExtractKey
        (
            const   TDTDAttrDef&            xadSrc
        );


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDAttrDef
        (
            const   tCIDLib::TCh* const     pszName
        );

        TDTDAttrDef
        (
            const   TString&                strName
        );

        TDTDAttrDef
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDXML::EDefAttrTypes  eDefaultType
        );

        TDTDAttrDef
        (
            const   TString&                strName
            , const TString&                strValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDXML::EDefAttrTypes  eDefaultType
        );

        TDTDAttrDef
        (
            const   TDTDAttrDef&            xadToCopy
        );

        ~TDTDAttrDef();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDTDAttrDef& operator=
        (
            const   TDTDAttrDef&            xadToAssign
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const;

        const TString& strFullName() const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strName
        );

        const TString& strName
        (
            const   tCIDLib::TCh* const     pszName
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strName
        //      This is the full name of the attribute, which for a DTD
        //      based attribute is the full QName. This is also the key for
        //      the hash set that our parent element class uses to hold us.
        // -------------------------------------------------------------------
        TString     m_strName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDAttrDef,TXMLAttrDef)
};



#pragma CIDLIB_POPPACK


