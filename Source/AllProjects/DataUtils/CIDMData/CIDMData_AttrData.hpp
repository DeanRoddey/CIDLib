//
// FILE NAME: CIDMData_AttrData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
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
//  This file implements a class that is used to represent an editable attribute.
//  It's used in some fundamental object attribute editing mechanisms.
//
//  There are some well known types (directly supported by the TAttrEdWnd class in
//  CIDWUtils) which must be stored in a particular format so that that window knows
//  how to read/write them. So we just provide members to hold those formats. It's
//  a little wasteful but it's not like millions of these will be created. It'll more
//  in the tens, so it's well worth the simplicity and the client code doesn't have
//  to worry about how these basic types are stored.
//
//  There is also a binary type, which can be used for more extended types.
//
//  In addition to the basic types, there is a 'special type' field which is a string
//  and which client code can use to provided extended meaning to the basic types,
//  to provide their own validation and visual editing.
//
//  Here are the basic types, and optional limits that can be used to get more
//  validation out of the basic validation in the TAttrEditWnd class (over in CIDWUtils)
//  which uses this class for the attributes.
//
//    AppImage  - Path is in the m_strVal member
//
//    Area      - Uses m_areaVal
//                Limits: Area:x,y,cx,cy  (the area must be within this area)
//                        Size:cx,cy      (the area's size must be within this size
//                                         and the origin must be 0,0)
//
//    Binary    - Uses m_c4Bytes and m_mbufVal
//
//    Bool      - Uses m_bVal
//
//    Char      - Uses the m_chVal member
//                Limits: Chars:a,b,c,d,...
//
//    Card      - Uses m_c4Val
//    Int       - Uses m_i4Val
//                Limits: Range:min,max
//
//    CardRange - Uses m_c4Val and m_c4Val2 (min and max respectively)
//    IntRange  - Use m_i4Val and m_i4Val2
//                Limits: Range:min,max
//
//    File      - Path is in m_strValue
//                Limits: FileSel:Bgn Image : *.png, *.jpg
//
//    MLString  - Uses m_strValue also, but with possibly newlines in it. What type
//                of newlines is up to the application.
//
//    Point     - Uses m_pntVal
//                Limits: Area:x,y,cx,cy  (the point must be within this area)
//
//    RGBColor  - Uses m_clrVal
//
//    Size      - Uses m_szVal
//                Limits: Size:cx,cy
//
//    String    - Uses m_strValue
//                Limit: Enum:val1,val2, etc... if it has a limited set of values
//
//
//    Separator - No data, m_strValue holds text for display in the separator line
//                in the TAttrEditWnd class.
//
//
//  Each attribute has an 'edit type' which controls how the editor window allows the
//  value to be edited. It can be visual editing only, in place editing only, both,
//  or none. None would be for values that the application wants to show to the user but
//  it can't be modified.
//
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
//  CLASS: TAttrData
// PREFIX: adat
// ---------------------------------------------------------------------------
class CIDMDATAEXP TAttrData : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bParseAreaLim
        (
            const   TString&                strLims
            ,       TArea&                  areaLim
            ,       TString&                strErrMsg
        );

        static tCIDLib::TBoolean bParseSizeLim
        (
            const   TString&                strLims
            ,       TSize&                  szMin
            ,       TSize&                  szMax
            ,       TString&                strErrMsg
        );

        static const TString& strId
        (
            const   TAttrData&              attrSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAttrData();

        TAttrData
        (
            const   TAttrData&              attrSrc
        );

        ~TAttrData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAttrData& operator=
        (
            const   TAttrData&              adatSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TAttrData&              adatSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TAttrData&              adatSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaVal() const;

        tCIDLib::TBoolean bAlwaysReport() const;

        tCIDLib::TBoolean bAlwaysReport
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bVal() const;

        tCIDLib::TBoolean bInPlaceEdit() const;

        tCIDLib::TBoolean bReadOnly() const;

        tCIDLib::TBoolean bSameValue
        (
            const   TAttrData&              adatTest
        )   const;

        tCIDLib::TBoolean bVisEdit() const;

        tCIDLib::TCard4 c4Bytes() const;

        tCIDLib::TCard4 c4Val() const;

        tCIDLib::TCard4 c4Val2() const;

        tCIDLib::TCard8 c8User() const;

        tCIDLib::TCard8 c8User
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TCard8 c8Val() const;

        tCIDLib::TCh chVal() const;

        const TRGBClr& clrVal() const;

        const tCIDLib::TStrList& colVal() const;

        tCIDLib::TVoid ChangeType
        (
            const   tCIDMData::EAttrTypes   eToSet
        );

        tCIDLib::TVoid CopyVal
        (
            const   TAttrData&              adatSrc
        );

        tCIDLib::TVoid ClearValue();

        tCIDLib::TEncodedTime enctVal() const;

        tCIDMData::EAttrTypes eType() const;

        tCIDMData::EAttrEdTypes eEditType() const;

        tCIDMData::EAttrEdTypes eEditType
        (
            const   tCIDMData::EAttrEdTypes eToSet
        );

        tCIDLib::TFloat8 f8Val() const;

        tCIDLib::TVoid FormatToText
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TInt4 i4Val() const;

        tCIDLib::TInt4 i4Val2() const;

        const TMemBuf& mbufVal() const;

        const TPoint& pntVal() const;

        const TString& strGroup() const;

        const TString& strId() const;

        const TString& strLimits() const;

        const TString& strLimits
        (
            const   TString&                strToSet
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );

        const TString& strSpecType() const;

        const TString& strUserData() const;

        const TString& strUserData
        (
            const   TString&                strToSet
        );

        const TString& strValue() const;

        const TSize& szVal() const;

        tCIDLib::TVoid Set
        (
            const   TString&                strName
            , const TString&                strId
            , const tCIDMData::EAttrTypes   eType
            , const tCIDMData::EAttrEdTypes eEdType = tCIDMData::EAttrEdTypes::Count
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strName
            , const TString&                strId
            , const TString&                strLimits
            , const tCIDMData::EAttrTypes   eType
            , const tCIDMData::EAttrEdTypes eEdType = tCIDMData::EAttrEdTypes::Count
        );

        tCIDLib::TVoid SetGroup
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetSpecType
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetType
        (
            const   tCIDMData::EAttrTypes   eType
        );

        tCIDLib::TVoid SetValueFromText
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetArea
        (
            const   TArea&                  areaToSet
        );

        tCIDLib::TVoid SetBool
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetCard
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetCard64
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TVoid SetCardRange
        (
            const   tCIDLib::TCard4         c4Min
            , const tCIDLib::TCard4         c4Max
        );

        tCIDLib::TVoid SetChar
        (
            const   tCIDLib::TCh            chToSet
        );

        tCIDLib::TVoid SetFloat
        (
            const   tCIDLib::TFloat8        f8ToSet
        );

        tCIDLib::TVoid SetInt
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetIntRange
        (
            const   tCIDLib::TInt4          i4Min
            , const tCIDLib::TInt4          i4Max
        );

        tCIDLib::TVoid SetMemBuf
        (
            const   TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Bytes
        );

        tCIDLib::TVoid SetPoint
        (
            const   TPoint&                 pntToSet
        );

        tCIDLib::TVoid SetRawString
        (
            const   tCIDLib::TCh* const     pszValue
        );

        tCIDLib::TVoid SetRGBColor
        (
            const   TRGBClr&                clrToSet
        );

        tCIDLib::TVoid SetSize
        (
            const   TSize&                  szToSet
        );

        tCIDLib::TVoid SetStrList
        (
            const   tCIDLib::TStrList&      colToSet
        );

        tCIDLib::TVoid SetString
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetTime
        (
            const   tCIDLib::TEncodedTime   enctToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckType
        (
            const   tCIDMData::EAttrTypes   eToCheck
        )   const;

        tCIDLib::TVoid ResetValue();

        tCIDLib::TVoid SetEditTypes();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaVal
        //      For area value storage.
        //
        //  m_bAlwaysReport
        //      Some values are indirect, i.e. the value is a symbol for something
        //      else. So, just because the value didn't change doesn't mean that
        //      the actual resolved real value didn't. So this flag tells the
        //      attribute editor to always report these as changed after a visual
        //      edit invocation.
        //
        //  m_bEnabled
        //      Some attributes are not valid for some client code specific reason.
        //      So it should be displayed in a different way to indicate that,
        //      and attempts to change the value should be rejected. This flag can
        //      be used to mark attributes enabled/disabled. It defaults to
        //      enabled.
        //
        //  m_bVal
        //      For boolean value storage.
        //
        //  m_c4Bytes
        //      Stores the byte count when using binary storage, in m_mbufVal.
        //
        //  m_c4Val
        //  m_c4Val2
        //      For cardinal and cardinal range storage.
        //
        //  m_c8User
        //      A value for the client code to use for its own purposes. For instance,
        //      it may want to display something textually, but that's just a visual
        //      representation of something that is driven by a number. So they can put
        //      the number in here and use that for the actual value.
        //
        //  m_c8Val
        //      Used for 64 bit unsized values and times.
        //
        //  m_chVal
        //      For single character storage.
        //
        //  m_clrVal
        //      For color value storage.
        //
        //  m_eEditType
        //      Indicates if this attribute can be edited in line, visually, or
        //      both, or none (read only.) We initialize it based on the data type
        //      set (basd on knowledge of what the attribute editor window supports
        //      for each base type), but the client code may set it to something else.
        //
        //  m_eType
        //      The base data type of this attribute. This drives how the attribute
        //      editor window treats the data, and indicates which of the members
        //      the value is stored in. We also use it to make sure that they call
        //      the correct SetValue() method.
        //
        //  m_f8Val
        //      For floating point value storage.
        //
        //  m_i4Val
        //  m_i4Val2
        //      For integral and integral range storage.
        //
        //  m_mbufVal
        //      For storing binary data.
        //
        //  m_pntVal
        //      For point value storage.
        //
        //  m_strGroup
        //      This can be used to group attributes together. This is purely
        //      for use by client code.
        //
        //  m_strId
        //      The unique id for this attribute. This is used by client code to
        //      find a given attribute in a list of attributes.
        //
        //  m_strLimits
        //      An option limits value, to allow the attribute editor window to do
        //      some or all of the required validation.
        //
        //  m_strName
        //      This is the display name for the attribute. This is only needed if
        //      the attributes will be displayed to the user, such as in the
        //      attribute editor.
        //
        //  m_strSpecType
        //      A 'specific type' that is for use by the client code, to indicate
        //      some more specific than what he puts into the m_strType member. It
        //      be used for custom validation or value selection. Often it means
        //      that m_strType is used for the basic built in validation, and the
        //      special type is used for extra validation.
        //
        //  m_strValue
        //      For string based value storage, so text or multi-line text or any
        //      file path types.
        //
        //  m_szVal
        //      For size value storage
        //
        //  m_strUserData
        //      For the application to pass values through to some editor that uses these
        //      attributes. We don't do anything with it.
        // -------------------------------------------------------------------
        TArea                   m_areaVal;
        tCIDLib::TBoolean       m_bAlwaysReport;
        tCIDLib::TBoolean       m_bEnabled;
        tCIDLib::TBoolean       m_bVal;
        tCIDLib::TCard4         m_c4Bytes;
        tCIDLib::TCard4         m_c4Val;
        tCIDLib::TCard4         m_c4Val2;
        tCIDLib::TCard8         m_c8User;
        tCIDLib::TCard8         m_c8Val;
        tCIDLib::TCh            m_chVal;
        TRGBClr                 m_clrVal;
        tCIDMData::EAttrEdTypes m_eEditType;
        tCIDLib::TFloat8        m_f8Val;
        tCIDMData::EAttrTypes   m_eType;
        tCIDLib::TInt4          m_i4Val;
        tCIDLib::TInt4          m_i4Val2;
        THeapBuf                m_mbufVal;
        TPoint                  m_pntVal;
        TSize                   m_szVal;
        TString                 m_strGroup;
        TString                 m_strId;
        TString                 m_strLimits;
        TString                 m_strName;
        TString                 m_strSpecType;
        TString                 m_strValue;
        TString                 m_strUserData;

        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAttrData, TObject)
};

#pragma CIDLIB_POPPACK

