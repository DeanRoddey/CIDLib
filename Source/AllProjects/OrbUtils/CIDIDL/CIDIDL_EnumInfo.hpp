//
// FILE NAME: CIDIDL_EnumInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/10/2004
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
//  This is the header for the CIDIDL_EnumInfo.cpp file. This file is used to
//  pass enumerated value info into the code generator. There's a class
//  for the individual enumerated values and one for the overall enumerated
//  type, which contains a list of the individual values.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TCGenEnumVal
//  PREFIX: tInfo
// ---------------------------------------------------------------------------
class TCGenEnumVal : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCGenEnumVal();

        ~TCGenEnumVal();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasText() const
        {
            return m_bHasText;
        }

        tCIDLib::TInt4 i4AltNumVal() const
        {
            return m_i4AltNumVal;
        }

        tCIDLib::TVoid ClearText();

        const TString& strAltText() const
        {
            return m_strAltText;
        }

        const TString& strAltText2() const
        {
            return m_strAltText2;
        }

        const TString& strName() const
        {
            return m_strName;
        }

        const TString& strValue() const
        {
            return m_strValue;
        }

        const TString& strText() const
        {
            return m_strText;
        }

        tCIDLib::TVoid Set
        (
            const   TString&                strTypeName
            , const TXMLTreeElement&        xtnodeVal
            , const tCIDLib::TBoolean       bAltText2
            , const tCIDLib::TBoolean       bAltNumVal
            , const tCIDIDL::EEnumTypes     eType
            , const tCIDIDL::EAltTextSrcs   eAltTextSrc
            , const TString&                strAltPattern
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bHasText
        //      If we had a CIDIDL:Text attribute. The IDL compiler uses this to decide
        //      if it needs to set a text source type.
        //
        //  m_i4AltNumVal
        //      If the enum supports an alt numeric value, this is used to store that.
        //      If the enum is marked as supported an alt numeric value, all entries
        //      must have the value. We'll generate mapping methods to go between the
        //      the enum and the value, and vice versa.
        //
        //  m_strAltText
        //      If the enum supports alternative text values, this is used to store
        //      that. It's an enum level setting, so all (non-magical) values of a
        //      given enum either have it or not.
        //
        //  m_strAltText2
        //      We also support a 2nd alt value
        //
        //  m_strName
        //      The base name of the enum value. It will be combined with other stuff
        //      as required.
        //
        //  m_strValue
        //      The value to assign to it, if any. It might be empty.
        //
        //  m_strText
        //      The main text value if any. If to be loaded from a resource, this is the
        //      text of the resource id name (when added to the msg namespace we
        //      have the compile time id of the resource.)
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bHasText;
        tCIDLib::TInt4      m_i4AltNumVal;
        TString             m_strAltText;
        TString             m_strAltText2;
        TString             m_strName;
        TString             m_strValue;
        TString             m_strText;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCGenEnumVal,TObject)
};




// ---------------------------------------------------------------------------
//   CLASS: TCGenEnumInfo
//  PREFIX: einfo
// ---------------------------------------------------------------------------
class TCGenEnumInfo : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCGenEnumInfo
        (
            const   TXMLTreeElement&        xtnodeSrc
        );

        ~TCGenEnumInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsContig() const;

        tCIDLib::TVoid FmtHeader
        (
            const   tCIDLib::TBoolean       bClassType
            , const TString&                strEnclosing
            , const TString&                strExport
            ,       TTextOutStream&         strmTar
            ,       TTextOutStream&         strmXHdr
        );

        tCIDLib::TVoid FmtImpl
        (
            const   tCIDLib::TBoolean       bClassType
            , const TString&                strEnclosing
            ,       TTextOutStream&         strmTar
        );

        tCIDLib::TVoid Set
        (
            const   TXMLTreeElement&        xtnodeEnum
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TEnumMap::ETextVals eXlatEnumTextVal
        (
            const   TXMLTreeElement&        xtnodeSrc
            , const TString&                strAttrName
        )   const;

        tCIDLib::TVoid FormatEnumDef
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Indent
        )   const;

        tCIDLib::TVoid FormatEnumImpl
        (
                    TTextOutStream&         strmTar
            , const TString&                strEnclosing
        )   const;

        tCIDLib::TVoid FormatEnumMap
        (
                    TTextOutStream&         strmTar
            , const TString&                strEnclosing
        );

        tCIDLib::TVoid FormatEnumMethSigs
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bClassType
            , const tCIDLib::TCard4         c4Indent
            , const TString&                strExport
        )   const;

        tCIDLib::TVoid FormatEnumGlobMethSigs
        (
                    TTextOutStream&         strmTar
            , const TString&                strExport
            , const TString&                strEnclosing
        )   const;

        tCIDLib::TVoid FormatOperatorSigs
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bClassType
            , const tCIDLib::TCard4         c4Indent
            , const TString&                strExport
            , const TString&                strEnclosing
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAltNumVal
        //      Enums can have an alternative numeric value for each entry if so marked.
        //      For these we'll generate mapping methods to go between the enum and the
        //      alt numeric value.
        //
        //  m_bAltText2
        //      We support both alt text (which has a few options) and a second alt
        //      text which is purely inline and typically used for some maybe internal
        //      only mapping or longer human readable text. Since it is always inlined,
        //      we only need to know if it is supported or not.
        //
        //  m_bBinStreamable
        //      If set, we generate binary in/out streaming methods for the enum.
        //
        //  m_c4BaseCount
        //      For standard enums, we add some magic items (min, max, count), in
        //      some cases and they are not wanted in some bindings. So this is set
        //      to the number of actual enum values, so that enumeration can stop
        //      here if the magic ones aren't wanted. This also doesn't include
        //      the synonym values, which are in a separate list, and go after any of
        //      these magic values as well.
        //
        //  m_colVals
        //      Storage for the definitions of each value of this enum
        //
        //  m_colSyns
        //      Another storage list for synonym values. For these we just need a
        //      name and value, so they are stored as key/value string pairs.
        //
        //  m_eAltMap
        //  m_eAltMap2
        //      Indicates what text value to use for the alt/alt2 mapping methods, or
        //      if they are to be supported at all.
        //
        //  m_eAltTextSrc
        //      An enum can have an alternate text value for each enumerated value,
        //      which is often convenient to use as a mapping thing. This defines whether
        //      it is enabled and if so whether it comes from an inline value or via a
        //      pattern.
        //
        //  m_eDefFormatMap
        //      The default format mapping. We pass this as the default text value to
        //      the formatting method (generated if this is not the _None value.)
        //
        //  m_eIncDecOps
        //      If true, then global inc/dec lines will be generated for the enumeration.
        //      This is only valid for Std enums, i.e. contiguous ones.
        //
        //  m_eLoadMap
        //      Indicates if we support a strLoadXXX() method to load text for an enum
        //      value, and if so which of the text values is used. This is typically used
        //      for human readable text, which may not be required for more internal type
        //      enums.
        //
        //  m_eTextSrc
        //      Indicates if we have main text and if so whether it is inlined or loaded
        //      from a text resource.
        //
        //  m_eTextStreamMap
        //      If not set the to None value, we generate a text output stream method for
        //      this enum, and stream out this text value.
        //
        //  m_eType
        //      The type of enumerated value, which tells us what stuff we
        //      want to generate.
        //
        //  m_eXlatMap
        //      Indicates if we support eXlatXXX/strXlatXXX methods, and if so which of
        //      the text values is used.
        //
        //  m_strAltPattern
        //      If we have a pattern based alt value, they have to provide us with the
        //      pattern.
        //
        //  m_strBaseType
        //      IF a safe style, then they can also indicate the underlying type.
        //      This can be card or int 12, or 4. If not safe style or not indicated
        //      it is empty.
        //
        //  m_strMsgFac
        //  m_strMsgNS
        //      If the xlat type is loadable text, then they have to provide the
        //      message id and facility name. These are used to load the text.
        //
        //  m_strTypeName
        //      The name of the enumeration.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bAltNumVal;
        tCIDLib::TBoolean       m_bAltText2;
        tCIDLib::TBoolean       m_bBinStreamable;
        tCIDLib::TCard4         m_c4BaseCount;
        TEnumMap::ETextVals     m_eAltMap;
        TEnumMap::ETextVals     m_eAltMap2;
        tCIDIDL::EAltTextSrcs   m_eAltTextSrc;
        TEnumMap::ETextVals     m_eDefFormatMap;
        tCIDIDL::EIncDecOps     m_eIncDecOps;
        TEnumMap::ETextVals     m_eLoadMap;
        tCIDIDL::ETextSrcs      m_eTextSrc;
        TEnumMap::ETextVals     m_eTextStreamMap;
        tCIDIDL::EEnumTypes     m_eType;
        TEnumMap::ETextVals     m_eXlatMap;
        TVector<TCGenEnumVal>   m_colVals;
        tCIDLib::TKVPList       m_colSyns;
        TString                 m_strAltPattern;
        TString                 m_strBaseType;
        TString                 m_strMsgFac;
        TString                 m_strMsgNS;
        TString                 m_strTypeName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCGenEnumInfo,TObject)
};

