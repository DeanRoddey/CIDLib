//
// FILE NAME: CIDXML_Attr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/1999
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
//  This is the header file for the CIDXML_Attr.Cpp file, which implements
//  The TXMLAttr class. When a start tag is parsed and it contains attributes,
//  the start tag even must pass these out via the event callback. A list of
//  these objects is used to pass that data out. It just needs to hold the
//  key/value information and a little other data about the attribute.
//
//  In order to support canonical XML foramatting, which requires that attrs
//  be sorted, we provide a comparator method for this class. This allows
//  a sorted collection to manipulate them.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLAttr
// PREFIX: xattr
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLAttr : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TXMLAttr&               xad1
            , const TXMLAttr&               xad2
        );


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLAttr();

        TXMLAttr
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCh* const     pszPrefix
            , const tCIDLib::TCh* const     pszURI
            , const tCIDLib::TCh* const     pszValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDLib::TBoolean       bExplicit
        );

        TXMLAttr
        (
            const   TString&                strBaseName
            , const TString&                strPrefix
            , const TString&                strURI
            , const TString&                strValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDLib::TBoolean       bExplicit
        );

        TXMLAttr
        (
            const   TXMLAttr&               xattrToCopy
        );

        ~TXMLAttr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLAttr& operator=
        (
            const   TXMLAttr&               xattrToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TXMLAttr&               xattrToComp
        );

        tCIDLib::TBoolean operator!=
        (
            const   TXMLAttr&               xattrToComp
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bExplicit() const;

        tCIDXML::EAttrTypes eType() const;

        const TString& strBaseName() const;

        const TString& strPrefix() const;

        const TString& strQName() const;

        const TString& strURI() const;

        const TString& strValue() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCh* const     pszPrefix
            , const tCIDLib::TCh* const     pszURI
            , const tCIDLib::TCh* const     pszValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDLib::TBoolean       bExplicit
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strBaseName
            , const TString&                strPrefix
            , const TString&                strURI
            , const TString&                strValue
            , const tCIDXML::EAttrTypes     eType
            , const tCIDLib::TBoolean       bExplicit
        );



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bExplicit
        //      This flag indicates whether the attribute was explicitly
        //      provided or defaulted in.
        //
        //  m_eType
        //      The type of the attribute.
        //
        //  m_strBaseName
        //      This is the base name of the attribute, which is always
        //      required.
        //
        //  m_strPrefix
        //      This is the prefix part of the name, if any. Regardless of
        //      whether namespaces are enabled, this can be empty.
        //
        //  m_strQName
        //      This is the QName, which is built from the base name and
        //      prefix. Because of the common need to access it, we don't want
        //      to force client code to build it up themselves over and over.
        //
        //  m_strURI
        //      This is the URI the prefix maps to, empty if namespace
        //      processing is not enabled.
        //
        //  m_strValue
        //      The value of this attribute.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bExplicit;
        tCIDXML::EAttrTypes m_eType;
        TString             m_strBaseName;
        TString             m_strPrefix;
        TString             m_strQName;
        TString             m_strURI;
        TString             m_strValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLAttr,TObject)
};


#pragma CIDLIB_POPPACK


