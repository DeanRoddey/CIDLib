//
// FILE NAME: CIDIDL_EnumInfo.cpp
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
//  This file implements the enumeration info classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDIDL.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCGenEnumVal,TObject)
RTTIDecls(TCGenEnumInfo,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCGenEnumVal
//  PREFIX: tInfo
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCGenEnumVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TCGenEnumVal::TCGenEnumVal() :

    m_bHasText(kCIDLib::False)
    , m_i4AltNumVal(0)
{
}

TCGenEnumVal::~TCGenEnumVal()
{
}


// ---------------------------------------------------------------------------
//  TCGenEnumVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TCGenEnumVal::ClearText()
{
    m_strText.Clear();
}


tCIDLib::TVoid
TCGenEnumVal::Set(  const   TString&                strTypeName
                    , const TXMLTreeElement&        xtnodeVal
                    , const tCIDLib::TBoolean       bAltText2
                    , const tCIDLib::TBoolean       bAltNumVal
                    , const tCIDIDL::EEnumTypes     eType
                    , const tCIDIDL::EAltTextSrcs   eAltTextSrc
                    , const TString&                strAltPattern)
{
    m_strName = xtnodeVal.xtattrNamed(L"CIDIDL:Name").strValue();

    //
    //  For a bitmapped value or a synonym, we will get a value. For regular values of
    //  contiguous enums, it will not be present but clear it anyway to be sure.
    //
    if (xtnodeVal.bAttrExists(L"CIDIDL:Value", m_strValue))
    {
        // Type must be one that uses the value attr
        if (eType != tCIDIDL::EEnumTypes::Bmp)
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_ValueNotUsed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotSupported
                , strTypeName
            );
        }
    }
     else
    {
        // Cannot be a type that requires a value
        if (eType == tCIDIDL::EEnumTypes::Bmp)
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_ValueRequired
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strTypeName
            );
        }
        m_strValue.Clear();
    }

    //
    //  See if we have a text attribute. If so, store the text, else clear it.
    //
    m_bHasText = xtnodeVal.bAttrExists(L"CIDIDL:Text", m_strText);
    if (!m_bHasText)
        m_strText.Clear();


    //
    //  If alt text is inline, we have to have to have that value. If pattern based
    //  we generate the value. Only do this for a non-synonym type.
    //
    if (eAltTextSrc == tCIDIDL::EAltTextSrcs::Inline)
    {
        m_strAltText = xtnodeVal.xtattrNamed(L"CIDIDL:AltText").strValue();
    }
     else if (eAltTextSrc == tCIDIDL::EAltTextSrcs::Pattern)
    {
        // Set up the value via pattern
        m_strAltText = strAltPattern;
        m_strAltText.eReplaceToken(m_strName, L'v');
    }
     else if (eAltTextSrc == tCIDIDL::EAltTextSrcs::None)
    {
        m_strAltText.Clear();
    }
     else
    {
        CIDAssert2X(L"Unknown alt text source: %(1)", m_strName);
    }

    // Check for an alt text 2 if we are set up for it
    if (bAltText2)
        m_strAltText2 = xtnodeVal.xtattrNamed(L"CIDIDL:AltText2").strValue();
    else
        m_strAltText2.Clear();

    // And do the alt numeric value if set up for that
    if (bAltNumVal)
        m_i4AltNumVal = xtnodeVal.xtattrNamed(L"CIDIDL:AltNumVal").i4ValueAs();
    else
        m_i4AltNumVal = 0;
}






// ---------------------------------------------------------------------------
//   CLASS: TCGenEnumInfo
//  PREFIX: mparm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCGenEnumInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TCGenEnumInfo::TCGenEnumInfo(const TXMLTreeElement& xtnodeEnum) :

    m_bAltNumVal(kCIDLib::False)
    , m_bAltText2(kCIDLib::False)
    , m_bBinStreamable(kCIDLib::False)
    , m_c4BaseCount(0)
    , m_eAltMap(TEnumMap::ETextVals::None)
    , m_eAltMap2(TEnumMap::ETextVals::None)
    , m_eAltTextSrc(tCIDIDL::EAltTextSrcs::None)
    , m_eDefFormatMap(TEnumMap::ETextVals::None)
    , m_eIncDecOps(tCIDIDL::EIncDecOps::None)
    , m_eLoadMap(TEnumMap::ETextVals::None)
    , m_eTextSrc(tCIDIDL::ETextSrcs::None)
    , m_eTextStreamMap(TEnumMap::ETextVals::None)
    , m_eType(tCIDIDL::EEnumTypes::Count)
    , m_eXlatMap(TEnumMap::ETextVals::None)
{
    Set(xtnodeEnum);
}

TCGenEnumInfo::~TCGenEnumInfo()
{
}


// ---------------------------------------------------------------------------
//  TCGenEnumInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Returns true if this enum is of a contiguously numbered type. Currently that means
//  it's of the standard type.
//
tCIDLib::TBoolean TCGenEnumInfo::bIsContig() const
{
    return (m_eType == tCIDIDL::EEnumTypes::Std);
}


//
//  These save a lot of work for the code generator, particularly now that we have to
//  deal with old style and new safe style. This is called to spit out all of the
//  stuff for this enum, one for the header content and one for the implementation
//  content. We have all the info so it's just easier for us to do it.
//
//  The issues are that:
//
//  1.  We may be doing a globals style generation, to a namespace, or it may be
//      within a generated class. If a namespace, then strEnclosing is the namespace
//      name, if a class it's the class name. The class name will be either the server
//      or client side class name depending on which output mode is being done.
//  2.  If a class is the enclosing bit is not required for the header of course.
//  3.  If a namespace enclosure then we need to provide appropriate export keyword
//      which we get in strExport. If a class this is not needed since the class itself
//      will be exported if appropriate. This is only need in the header bits.
//  4.  For the header stuff their is an X stream. This is for stuff that needs to be
//      global scope (operators and such.) So the generator has to save up all this
//      stuff until the namespace/class is closed off, then it can go back and spit
//      out this stuff to the main header stream.
//
//  There's a lot of code so we call various private helpers to do specific bits of
//  it.
//
tCIDLib::TVoid
TCGenEnumInfo::FmtHeader(const  tCIDLib::TBoolean   bClassType
                        , const TString&            strEnclosing
                        , const TString&            strExport
                        ,       TTextOutStream&     strmTar
                        ,       TTextOutStream&     strmXHdr)
{
    const tCIDLib::TCard4 c4BaseIndent = bClassType ? 8 : 4;

    // Spit out the actual enum definition, which is the same either way
    FormatEnumDef(strmTar, c4BaseIndent);

    // Spit out the method signatures to the header
    FormatEnumMethSigs(strmTar, bClassType, c4BaseIndent, strExport);

    //
    //  Do the global operators which are put into the X header for later insertion
    //  at the bottom of the header file.
    //
    FormatEnumGlobMethSigs(strmXHdr, strExport, strEnclosing);

    //
    //  If a class type output, operator sigs go into the X header, else the main one. If
    //  class type, we also have a base indent of zero.
    //
    if (bClassType)
        FormatOperatorSigs(strmXHdr, bClassType, 0, strExport, strEnclosing);
    else
        FormatOperatorSigs(strmTar, bClassType, c4BaseIndent, strExport, strEnclosing);
}


tCIDLib::TVoid
TCGenEnumInfo::FmtImpl( const   tCIDLib::TBoolean   bClassType
                        , const TString&            strEnclosing
                        ,       TTextOutStream&     strmTar)
{
    // Format the mapping table
    FormatEnumMap(strmTar, strEnclosing);

    // And finally spit out all the method impls
    FormatEnumImpl(strmTar, strEnclosing);
}


#if 0

const TString& TCGenEnumInfo::strMsgFac() const
{
    return m_strMsgFac;
}


const TString& TCGenEnumInfo::strMsgNS() const
{
    return m_strMsgNS;
}


const TString& TCGenEnumInfo::strTypeName() const
{
    return m_strTypeName;
}


const TString& TCGenEnumInfo::strOldPrefix() const
{
    return m_strOldValPref;
}

#endif


tCIDLib::TVoid
TCGenEnumInfo::Set(const TXMLTreeElement& xtnodeEnum)
{
    // Get the type name and other stuff that applies to the enum as a whole
    m_strTypeName = xtnodeEnum.xtattrNamed(L"CIDIDL:Name").strValue();

    // See if there is a base type
    xtnodeEnum.bAttrExists(L"CIDIDL:BaseType", m_strBaseType);

    // Set our overall enumeration type (standard contiguous or bitmapped non-contiguous)
    const TString& strEnumType = xtnodeEnum.xtattrNamed(L"CIDIDL:Type").strValue();
    if (strEnumType == L"Std")
    {
        m_eType = tCIDIDL::EEnumTypes::Std;
    }
     else if (strEnumType == L"Bmp")
    {
        m_eType = tCIDIDL::EEnumTypes::Bmp;

        //
        //  If no explicit base type, make it Card4. If we do have a type, make
        //  sure it's an unsigned type.
        //
        if (m_strBaseType.bIsEmpty())
        {
            m_strBaseType = L"TCard4";
        }
         else if (!m_strBaseType.bStartsWith(L"TCard"))
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_BmpMustBeSigned
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , m_strTypeName
            );
        }
    }
     else
    {
        facCIDIDL.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kIDLErrs::errcInp_BadEnumType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strEnumType
            , m_strTypeName
        );
    }

    // These are just simple flags
    m_bAltNumVal = xtnodeEnum.xtattrNamed(L"CIDIDL:AltNumVal").strValue() == L"Yes";
    m_bAltText2 = xtnodeEnum.xtattrNamed(L"CIDIDL:AltText2").strValue() == L"Yes";
    m_bBinStreamable = xtnodeEnum.xtattrNamed(L"CIDIDL:BinStream").strValue() == L"Yes";

    //
    //  Set up the other types of text we have first since other stuff will later
    //  refer to these and we want to know if those references are valid.
    //
    const TString& strAltTSrc = xtnodeEnum.xtattrNamed(L"CIDIDL:AltTextSrc").strValue();
    if (strAltTSrc == L"Inline")
        m_eAltTextSrc = tCIDIDL::EAltTextSrcs::Inline;
    else if (strAltTSrc == L"Pattern")
        m_eAltTextSrc = tCIDIDL::EAltTextSrcs::Pattern;
    else if (strAltTSrc == L"No")
        m_eAltTextSrc = tCIDIDL::EAltTextSrcs::None;
    else
    {
        facCIDIDL.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kIDLErrs::errcInp_BadAltTextSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strAltTSrc
            , m_strTypeName
        );
    }

    // Store the various mapping method options
    m_eAltMap = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:AltMap");
    m_eAltMap2 = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:AltMap2");
    m_eDefFormatMap = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:FormatMap");
    m_eLoadMap = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:LoadMap");
    m_eTextStreamMap = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:TextStreamMap");
    m_eXlatMap = eXlatEnumTextVal(xtnodeEnum, L"CIDIDL:XlatMap");

    //
    //  If pattern based, we have to have a pattern, and it must have the appropriate
    //  replacement token in it.
    //
    if (m_eAltTextSrc == tCIDIDL::EAltTextSrcs::Pattern)
    {
        m_strAltPattern = xtnodeEnum.xtattrNamed(L"CIDIDL:AltPattern").strValue();
        if (!m_strAltPattern.bTokenExists(L'v'))
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_BadEnumPattern
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , m_strTypeName
            );
        }
    }

    const TString& strIncDec = xtnodeEnum.xtattrNamed(L"CIDIDL:IncDec").strValue();
    if (strIncDec == L"Inc")
        m_eIncDecOps = tCIDIDL::EIncDecOps::Inc;
    else if (strIncDec == L"Both")
        m_eIncDecOps = tCIDIDL::EIncDecOps::Both;
    else
        m_eIncDecOps = tCIDIDL::EIncDecOps::None;


    //
    //  Loop through the children and fill in our enum values list. Set the
    //  base count to zero and count up the actual values we find.
    //
    m_colVals.RemoveAll();
    m_colSyns.RemoveAll();
    m_c4BaseCount = 0;

    TCGenEnumVal    evalNew;
    const tCIDLib::TCard4 c4Count = xtnodeEnum.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCur
                            = xtnodeEnum.xtnodeChildAtAsElement(c4Index);

        // If it's the doc text node, then skip it
        if (xtnodeCur.strQName() == L"CIDIDL:DocText")
            continue;

        // Then it has to be the enum value, either a regular one or a synonym
        const tCIDLib::TBoolean bSynType(xtnodeCur.strQName() == L"CIDIDL:EnumSyn");
        AssertElemName2(xtnodeCur, L"CIDIDL:EnumVal", L"CIDIDL:EnumSyn");

        if (bSynType)
        {
            m_colSyns.objAdd
            (
                TKeyValuePair
                (
                    xtnodeCur.xtattrNamed(L"CIDIDL:Name").strValue()
                    , xtnodeCur.xtattrNamed(L"CIDIDL:Value").strValue()
                )
            );
        }
         else
        {
            evalNew.Set
            (
                m_strTypeName
                , xtnodeCur
                , m_bAltText2
                , m_bAltNumVal
                , m_eType
                , m_eAltTextSrc
                , m_strAltPattern
            );
            m_colVals.objAdd(evalNew);

            // For non-synonym (real) values, bump the count of base values
            m_c4BaseCount++;
        }
    }

    //
    //  Figure out the text source. If any of our children have text, then we have to
    //  set the text source.
    //
    tCIDLib::TBoolean bHasText = kCIDLib::False;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_colVals.c4ElemCount(); c4Index++)
        bHasText |= m_colVals[c4Index].bHasText();

    if (bHasText)
    {
        // See if we have the loadable text oriented values
        if (xtnodeEnum.bAttrExists(L"CIDIDL:MsgFac", m_strMsgFac)
        &&  xtnodeEnum.bAttrExists(L"CIDIDL:MsgNS", m_strMsgNS))
        {
            m_eTextSrc = tCIDIDL::ETextSrcs::LoadRes;
        }
         else
        {
            m_eTextSrc = tCIDIDL::ETextSrcs::Inline;
        }
    }
     else
    {
        m_eTextSrc = tCIDIDL::ETextSrcs::None;
    }

    //
    //  Make sure any text values referenced by any of the maps or methods are
    //  enabled. We don't have to check name or base name since they are always
    //  possible.
    //
    if ((m_eAltMap == TEnumMap::ETextVals::AltText)
    ||  (m_eAltMap2 == TEnumMap::ETextVals::AltText)
    ||  (m_eDefFormatMap == TEnumMap::ETextVals::AltText)
    ||  (m_eLoadMap == TEnumMap::ETextVals::AltText)
    ||  (m_eTextStreamMap == TEnumMap::ETextVals::AltText)
    ||  (m_eXlatMap == TEnumMap::ETextVals::AltText))
    {
        if (m_eAltTextSrc == tCIDIDL::EAltTextSrcs::None)
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_IncompTextOpts
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Config
                , TString(L"AltText")
                , m_strTypeName
            );
        }
    }

    if ((m_eAltMap == TEnumMap::ETextVals::AltText2)
    ||  (m_eAltMap2 == TEnumMap::ETextVals::AltText2)
    ||  (m_eDefFormatMap == TEnumMap::ETextVals::AltText2)
    ||  (m_eLoadMap == TEnumMap::ETextVals::AltText2)
    ||  (m_eTextStreamMap == TEnumMap::ETextVals::AltText2)
    ||  (m_eXlatMap == TEnumMap::ETextVals::AltText2))
    {
        if (!m_bAltText2)
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_IncompTextOpts
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Config
                , TString(L"AltText2")
                , m_strTypeName
            );
        }
    }

    // If any operations reference the main text make sure we support it
    if ((m_eAltMap == TEnumMap::ETextVals::Text)
    ||  (m_eAltMap2 == TEnumMap::ETextVals::Text)
    ||  (m_eDefFormatMap == TEnumMap::ETextVals::Text)
    ||  (m_eLoadMap == TEnumMap::ETextVals::Text)
    ||  (m_eTextStreamMap == TEnumMap::ETextVals::Text)
    ||  (m_eXlatMap == TEnumMap::ETextVals::Text))
    {
        if (m_eTextSrc == tCIDIDL::ETextSrcs::None)
        {
            facCIDIDL.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_IncompTextOpts
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Config
                , TString(L"Text")
                , m_strTypeName
            );
        }
    }
}



// ---------------------------------------------------------------------------
//  TCGenEnumInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Translates from the XML representation of the enum text value enum to the actual
//  enum. Throws if bad.
//
TEnumMap::ETextVals
TCGenEnumInfo::eXlatEnumTextVal(const   TXMLTreeElement&    xtnodeSrc
                                , const TString&            strAttrName) const
{
    TEnumMap::ETextVals eRet = TEnumMap::ETextVals::Count;
    const TString& strXlat = xtnodeSrc.xtattrNamed(strAttrName).strValue();
    if (strXlat == L"AltText")
        eRet = TEnumMap::ETextVals::AltText;
    else if (strXlat == L"AltText2")
        eRet = TEnumMap::ETextVals::AltText2;
    else if (strXlat == L"BaseName")
        eRet = TEnumMap::ETextVals::BaseName;
    else if (strXlat == L"Name")
        eRet = TEnumMap::ETextVals::Name;
    else if (strXlat == L"Text")
        eRet = TEnumMap::ETextVals::Text;
    else if (strXlat == L"No")
        eRet = TEnumMap::ETextVals::None;
    else
    {
        facCIDIDL.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kIDLErrs::errcInp_BadXlatType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strXlat
            , m_strTypeName
        );
    }
    return eRet;
}


// Spits out the enum definition into the header file
tCIDLib::TVoid
TCGenEnumInfo::FormatEnumDef(       TTextOutStream& strmTar
                            , const tCIDLib::TCard4 c4Indent) const
{
    //
    //  We already have the old style value prefix, but we will also need the
    //  old style name prefix for old style ones, so do this to avoid a lot of
    //  redundancy below.
    //
    const TString strTypePref(TStrCat(m_strTypeName, kCIDLib::chUnderscore));

    // Generate the opening of the enum definition
    strmTar << TTextOutStream::Spaces(c4Indent) << L"enum class "
            << m_strTypeName;

    // If we have a base type, then do that
    if (!m_strBaseType.bIsEmpty())
        strmTar << L" : tCIDLib::" << m_strBaseType;

    strmTar << L"\n"
            << TTextOutStream::Spaces(c4Indent)
            << L"{\n";

    //
    //  Do the regular values. While we are going, if a bitmapped type, accumlate
    //  all of the bits for use below.
    //
    tCIDLib::TCard4 c4AllBits = 0;
    const tCIDLib::TCard4 c4ValCnt = m_colVals.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ValCnt; c4Index++)
    {
        const TCGenEnumVal& evalCur = m_colVals[c4Index];

        strmTar << TTextOutStream::Spaces(c4Indent + 4);
        if (c4Index)
            strmTar << L", ";

        strmTar << evalCur.strName();

        if (!evalCur.strValue().bIsEmpty())
            strmTar << L" = " << evalCur.strValue();
        strmTar << kCIDLib::NewLn;

        if (m_eType == tCIDIDL::EEnumTypes::Bmp)
            c4AllBits |= evalCur.strValue().c4Val(tCIDLib::ERadices::Hex);
    }

    // Do any magic values
    if (m_eType == tCIDIDL::EEnumTypes::Std)
    {
        // These get count, min and max
        strmTar << TTextOutStream::Spaces(c4Indent + 4)
                << L", Count" << kCIDLib::NewLn
                << TTextOutStream::Spaces(c4Indent + 4)
                << L", Min = " << m_colVals[0].strName() << kCIDLib::NewLn
                << TTextOutStream::Spaces(c4Indent + 4)
                << L", Max = " << m_colVals[c4ValCnt - 1].strName() << kCIDLib::NewLn;
    }
     else if (m_eType == tCIDIDL::EEnumTypes::Bmp)
    {
        // These get count, no bits, all bits values
        strmTar << TTextOutStream::Spaces(c4Indent + 4)
                << L", Count = " << c4ValCnt << kCIDLib::NewLn
                << TTextOutStream::Spaces(c4Indent + 4)
                << L", None = 0" << kCIDLib::NewLn
                << TTextOutStream::Spaces(c4Indent + 4)
                << L", AllBits = 0x" << TCardinal(c4AllBits, tCIDLib::ERadices::Hex)
                << kCIDLib::NewLn;
    }

    // Do any synonyms
    const tCIDLib::TCard4 c4SynCount = m_colSyns.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SynCount; c4Index++)
    {
        const TKeyValuePair& kvalCur = m_colSyns[c4Index];

        strmTar << TTextOutStream::Spaces(c4Indent + 4) << L", "
                << kvalCur.strKey() << L" = "
                << kvalCur.strValue() << kCIDLib::NewLn;
    }

    // And close off the enum now
    strmTar << TTextOutStream::Spaces(c4Indent) << L"};\n";
}


//
//  This is called to spit out the method and global operators type signatures for
//  the enum
//
tCIDLib::TVoid
TCGenEnumInfo::FormatEnumMethSigs(          TTextOutStream&     strmTar
                                    , const tCIDLib::TBoolean   bClassType
                                    , const tCIDLib::TCard4     c4Indent
                                    , const TString&            strExport) const
{
    // Set up a static keyword if class type, to use with some of the class methods
    const TString strStatic(bClassType ? L"static " : TString::strEmpty());

    TStreamIndentJan janIndent(&strmTar, c4Indent);
    if (m_eXlatMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"[[nodiscard]] " << strExport << strStatic << L" "
                << m_strTypeName
                << L" eXlat" << m_strTypeName
                << L"(const TString& strToXlat, const tCIDLib::TBoolean "
                   L"bThrowIfNot = kCIDLib::False);\n"

                << L"[[nodiscard]] " << strExport << strStatic
                << L" const TString& strXlat"
                << m_strTypeName
                << L"(const " << m_strTypeName
                << L" eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);\n";
    }

    if (m_eLoadMap != TEnumMap::ETextVals::None)
    {
        strmTar << strExport << strStatic
                << L" const TString& strLoad"
                << m_strTypeName << L"(const " << m_strTypeName
                << L" eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);\n";
    }


    // If we support an alt map, do the sig for xlat method in both directions.
    if (m_eAltMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"[[nodiscard]] " << strExport << strStatic
                << L" const TString& strAltXlat"
                << m_strTypeName << L"(const " << m_strTypeName
                << L" eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);\n"

                << L"[[nodiscard]] " << strExport  << strStatic << L" "
                << m_strTypeName << L" eAltXlat" << m_strTypeName
                << L"(const TString& strToXlat, const tCIDLib::TBoolean "
                   L"bThrowIfNot = kCIDLib::False);\n";
    }

    // If it supports a second alt map, do that one
    if (m_eAltMap2 != TEnumMap::ETextVals::None)
    {
        strmTar << L"[[nodiscard]] " << strExport << strStatic
                << L" const TString& strAltXlat2"
                << m_strTypeName
                << L"(const " << m_strTypeName
                << L" eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);\n"

                << L"[[nodiscard]] "
                << strExport  << strStatic << L" " << m_strTypeName
                << L" eAltXlat2" << m_strTypeName
                << L"(const TString& strToXlat, const tCIDLib::TBoolean "
                   L"bThrowIfNot = kCIDLib::False);\n";
    }

    //
    //  If it supports an alt numeric value, do the sig for xlat method in both
    //  directions.
    //
    if (m_bAltNumVal)
    {
        strmTar << L"[[nodiscard]] "  << strStatic << strExport
                << L" tCIDLib::TInt4 i4AltNum"
                << m_strTypeName
                << L"(const " << m_strTypeName
                << L" eToXlat, const tCIDLib::TBoolean "
                   L"bThrowIfNot = kCIDLib::False);\n"

                << TTextOutStream::Spaces(c4Indent) << L"[[nodiscard]] " << strStatic << strExport << L" "
                << m_strTypeName
                << L" eAltNum" << m_strTypeName
                << L"(const tCIDLib::TInt4 i4Xlat, const tCIDLib::TBoolean "
                   L"bThrowIfNot = kCIDLib::False);\n";
    }

    // If format option is set for this one, generate that method decl
    if (m_eDefFormatMap != TEnumMap::ETextVals::None)
    {
        strmTar << strStatic << strExport
                << L" tCIDLib::TVoid Format" << m_strTypeName
                << L"\n(\n"
                << L"               TString&            strTarget\n"
                << L"    , const    TString&            strPrefix\n"
                << L"    , const    tCIDLib::TCh        chSepChar\n"
                << L"    , const    TEnumMap::ETextVals eVal = "
                << TEnumMap::strFormatEnumTextVals(m_eDefFormatMap)
                << L"\n);\n";
    }

    // Do the valid enum method
    strmTar  << L"[[nodiscard]] " << strStatic << strExport
             << L" tCIDLib::TBoolean bIsValidEnum(const "
             << m_strTypeName
             << L" eVal);\n";


    strmTar << kCIDLib::NewLn;
}



tCIDLib::TVoid
TCGenEnumInfo::FormatEnumGlobMethSigs(          TTextOutStream&     strmXHdr
                                        , const TString&            strExport
                                        , const TString&            strEnclosing) const
{
    // Build up a full type name to make things simpler below
    TString strFullType(strEnclosing, m_strTypeName.c4Length() + 8UL);
    strFullType.Append(L"::");
    strFullType.Append(m_strTypeName);

    if (m_bBinStreamable)
    {
        strmXHdr    << strExport
                    << L" tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, "
                    << strFullType
                    << L"* const aeList, const tCIDLib::TCard4 c4Count);\n";

        strmXHdr    << strExport
                    << L" tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const "
                    << strFullType
                    << L"* const aeList, const tCIDLib::TCard4 c4Count);\n";
    }
}


// This spits out the mapping table contents to the impl file
tCIDLib::TVoid
TCGenEnumInfo::FormatEnumMap(TTextOutStream& strmTar, const TString& strEnclosing)
{
    const tCIDLib::TCard4 c4ValCnt = m_colVals.c4ElemCount();
    strmTar << L"\n\nstatic TEnumMap::TEnumValItem aeitemValues_" << m_strTypeName
            << L"[" << c4ValCnt << L"] = \n{\n";

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ValCnt; c4Index++)
    {
        TCGenEnumVal& evalCur = m_colVals[c4Index];
        if (c4Index)
            strmTar << L"  , { ";
        else
            strmTar << L"    { ";

        //
        //  Initialize the values based on their order in the TEnumMap::ETextVals
        //  enum. We use that enum to index the values.
        //
        strmTar << L" tCIDLib::TInt4(" << strEnclosing << L"::" << m_strTypeName
                << L"::" << evalCur.strName() << L"), " << evalCur.i4AltNumVal() << L", ";

        //
        //  If this is a loadable text enum, we need to output the compile time info for
        //  the loadable resource. Else just zero.
        //
        if (m_eTextSrc == tCIDIDL::ETextSrcs::LoadRes)
        {
            strmTar << m_strMsgNS << L"::" << evalCur.strText() << L", ";
            evalCur.ClearText();
        }
         else
        {
            strmTar << L"0, ";
        }

        strmTar << L" { L\""
                << L"\", L\"" << evalCur.strAltText()
                << L"\", L\"" << evalCur.strAltText2()
                << L"\", L\"" << evalCur.strName()
                << L"\", L\""
                << m_strTypeName << L"::" << evalCur.strName()
                << L"\", L\"" << evalCur.strText()
                << L"\" } }\n";
    }
    strmTar  << L"\n};\n\n";

    //
    //  Now generate the global ctor for the actual map object, passing the values
    //  array above to it as its enum values.
    //
    strmTar << L"static TEnumMap emap" << m_strTypeName << L"\n(\n"
            << L"     L\"" << m_strTypeName << L"\""
            << L"\n     , " << c4ValCnt
            << L"\n     , " << (bIsContig() ? L"kCIDLib::False" : L"kCIDLib::True")
            << L"\n     , aeitemValues_" << m_strTypeName
            << L"\n     , ";

    // If loadable, generate a pointer to the facility, else null pointer
    if (m_eTextSrc == tCIDIDL::ETextSrcs::LoadRes)
        strmTar << L"&fac" << m_strMsgFac;
    else
        strmTar << L"nullptr";

    strmTar << L"\n     , ";

    strmTar << L"tCIDLib::TCard4(" << strEnclosing << L"::" << m_strTypeName
            << L"::" << (bIsContig() ? L"Count" : L"None")
            << L")\n);\n\n";
}


//
//  This spits out the implementations of all of the out of line methods and operators,
//  most of which is just fairly simple invocations of methods on the generated
//  mapping table.
//
tCIDLib::TVoid
TCGenEnumInfo::FormatEnumImpl(          TTextOutStream&     strmTar
                                , const TString&            strEnclosing) const
{
    // Build up a full type name to make things simpler below
    TString strFullType(strEnclosing, m_strTypeName.c4Length() + 8UL);
    strFullType.Append(L"::");
    strFullType.Append(m_strTypeName);


    // If they defined an alt mapping method, generate those methods
    if (m_eAltMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"const TString& " << strEnclosing << L"::strAltXlat"
                << m_strTypeName << L"(const " << strFullType
                << L" eVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return emap" << m_strTypeName
                << L".strMapEnumVal(tCIDLib::TCard4(eVal), "
                << TEnumMap::strFormatEnumTextVals(m_eAltMap)
                << L", bThrowIfNot);\n}\n\n";

        strmTar << strFullType << L" " << strEnclosing << L"::eAltXlat"
                << m_strTypeName
                << L"(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                   L"    return " << strFullType << L"(emap" << m_strTypeName
                << L".i4MapEnumText(strVal, "
                << TEnumMap::strFormatEnumTextVals(m_eAltMap)
                << L", bThrowIfNot));\n}\n\n";
    }


    // If they defined an alt mapping 2 method, generate those methods
    if (m_eAltMap2 != TEnumMap::ETextVals::None)
    {
        strmTar << L"const TString& " << strEnclosing << L"::strAltXlat2"
                << m_strTypeName << L"(const " << strFullType
                << L" eVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return emap" << m_strTypeName
                << L".strMapEnumVal(tCIDLib::TCard4(eVal), "
                << TEnumMap::strFormatEnumTextVals(m_eAltMap2)
                << L", bThrowIfNot);\n}\n\n";

        strmTar << strFullType << L" " << strEnclosing << L"::eAltXlat2"
                << m_strTypeName
                << L"(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                   L"    return " << strFullType << L"(emap" << m_strTypeName
                << L".i4MapEnumText(strVal, "
                << TEnumMap::strFormatEnumTextVals(m_eAltMap2)
                << L", bThrowIfNot));\n}\n\n";
    }


    // If the load map is defined, then implement the load method
    if (m_eLoadMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"const TString& " << strEnclosing << L"::strLoad"
                << m_strTypeName << L"(const " << strFullType
                << L" eVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return emap" << m_strTypeName
                << L".strMapEnumVal(tCIDLib::TCard4(eVal), "
                << TEnumMap::strFormatEnumTextVals(m_eLoadMap)
                << L", bThrowIfNot);\n}\n";
    }


    // If they defined a formatting method, generate it
    if (m_eDefFormatMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"tCIDLib::TVoid " << strEnclosing << L"::Format"
                << m_strTypeName
                << L"(TString& strToFill, const TString& strPrefix,"
                   L" const tCIDLib::TCh chSepChar, const TEnumMap::ETextVals eVal)\n{\n"
                << L"    emap" << m_strTypeName
                << L".FormatValues(strToFill, strPrefix, chSepChar, eVal);\n}\n";
    }


    // If they defined an xlat map method, generate those methods
    if (m_eXlatMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"const TString& " << strEnclosing << L"::strXlat"
                << m_strTypeName << L"(const " << strFullType
                << L" eVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return emap" << m_strTypeName
                << L".strMapEnumVal(tCIDLib::TCard4(eVal), "
                << TEnumMap::strFormatEnumTextVals(m_eXlatMap)
                << L", bThrowIfNot);\n}\n\n";

        strmTar << strFullType << L" " << strEnclosing << L"::eXlat"
                << m_strTypeName
                << L"(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                   L"    return " << strFullType << L"(emap" << m_strTypeName
                << L".i4MapEnumText(strVal, "
                << TEnumMap::strFormatEnumTextVals(m_eXlatMap)
                << L", bThrowIfNot));\n}\n\n";
    }


    // If the alternate numeric value is enabled, do those mapping methods
    if (m_bAltNumVal)
    {
        strmTar << L"tCIDLib::TInt4 " << strEnclosing << L"::i4AltNum"
                << m_strTypeName << L"(const " << strFullType
                << L" eVal, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return emap" << m_strTypeName
                << L".i4MapEnumAltNum(tCIDLib::c4EnumOrd(eVal), bThrowIfNot);\n}\n\n";

        strmTar << strFullType << L" " << strEnclosing << L"::eAltNum"
                << m_strTypeName
                << L"(const tCIDLib::TInt4 i4Val, const tCIDLib::TBoolean bThrowIfNot)\n{\n"
                << L"    return " << strFullType
                << L"(emap" << m_strTypeName
                << L".eMapEnumAltNum(i4Val, bThrowIfNot));\n}\n\n";
    }

    // If text streamable, then do that operator
    if (m_eTextStreamMap != TEnumMap::ETextVals::None)
    {
        strmTar << L"TTextOutStream& " << strEnclosing
                << L"::operator<<(TTextOutStream& strmTar, const "
                << strFullType
                << L" eVal)\n{\n    strmTar << emap" << m_strTypeName
                << L".strMapEnumVal(tCIDLib::TCard4(eVal), "
                << TEnumMap::strFormatEnumTextVals(m_eTextStreamMap)
                << L", kCIDLib::False);\n"
                << L"    return strmTar;\n}\n";
    }

    // If binary streamable do those
    if (m_bBinStreamable)
    {
        strmTar << L"tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, "
                << strFullType
                << L"* const aeList, const tCIDLib::TCard4 c4Count)\n"
                << L"{\n"
                << L"    tCIDLib::TCard4 c4Cur;\n"
                << L"    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)\n"
                << L"    {\n"
                << L"        strmSrc >> c4Cur;\n"
                << L"        aeList[c4Index] = " << strFullType << L"(c4Cur);\n"
                << L"    }\n"
                << L"}\n";

        strmTar << L"tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const "
                << strFullType
                << L"* const aeList, const tCIDLib::TCard4 c4Count)\n"
                << L"{\n"
                << L"    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)\n"
                << L"        strmTar << tCIDLib::TCard4(aeList[c4Index]);\n"
                << L"}\n";
    }

    // Do the valid enum method
    strmTar << L"tCIDLib::TBoolean "
            << strEnclosing
            << L"::bIsValidEnum(const "
            << strFullType
            << L" eVal)\n{\n"
            << L"    return emap" << m_strTypeName
            << L".bIsValidEnum(tCIDLib::TCard4(eVal));\n"
            << L"\n}\n";
}


//
//  We have to format operators out from different places, depending on the output type
//
tCIDLib::TVoid
TCGenEnumInfo::FormatOperatorSigs(          TTextOutStream&     strmTar
                                    , const tCIDLib::TBoolean   bClassType
                                    , const tCIDLib::TCard4     c4Indent
                                    , const TString&            strExport
                                    , const TString&            strEnclosing) const
{
    // Set up our scope prefix
    TString strPrefix;
    if (!strEnclosing.bIsEmpty())
    {
        strPrefix = strEnclosing;
        strPrefix.Append(L"::");
    }

    TStreamIndentJan janIndent(&strmTar, c4Indent);
    if (m_eIncDecOps != tCIDIDL::EIncDecOps::None)
    {
        strmTar << L"inline " << strPrefix << m_strTypeName << L" operator++("
                << strPrefix << m_strTypeName
                << L"& eVal, int)\n{\n    " << strPrefix
                << m_strTypeName << L" eTmp = eVal;\n"
                << L"    eVal = " << strPrefix << m_strTypeName << L"(int(eVal)+1);\n"
                << L"    return eTmp;\n}\n\n";
    }

    if (m_eIncDecOps == tCIDIDL::EIncDecOps::Both)
    {
        strmTar << L"inline " << strPrefix << m_strTypeName << L" operator--("
                << strPrefix << m_strTypeName
                << L"& eVal, int)\n{\n    " << strPrefix
                << m_strTypeName << L" eTmp = eVal;\n"
                << L"    eVal = " << strPrefix  << m_strTypeName << L"(int(eVal)-1);\n"
                << L"    return eTmp;\n}\n\n";
    }

    if (m_bBinStreamable)
    {
        // The binary ones we can do inline, since they don't use any enum map stuff
        strmTar << L"inline TBinOutStream& operator<<(TBinOutStream& strmTar, const "
                << strPrefix << m_strTypeName
                << L" eVal)\n{\n    strmTar.WriteEnum(tCIDLib::TCard4(eVal));\n"
                << L"    return strmTar;\n}\n";;

        strmTar << L"inline TBinInStream& operator>>(TBinInStream& strmSrc, COP "
                << strPrefix << m_strTypeName
                << L"& eToFill)\n{\n    "
                << L"eToFill = " << strPrefix << m_strTypeName
                << L"(strmSrc.c4ReadEnum());\n    return strmSrc;\n}\n";
    }

    if (m_eType == tCIDIDL::EEnumTypes::Bmp)
    {
        // Do the bitwise operators and their assignment variations
        strmTar << L"[[nodiscard]] constexpr "
                << strPrefix << m_strTypeName << L" operator|(const "
                << strPrefix << m_strTypeName << L" eLHS, const " << strPrefix << m_strTypeName
                << L" eRHS)\n{    \n"
                << L"    return " << strPrefix << m_strTypeName
                << L"(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));\n"
                << L"}\n";

        strmTar << L"constexpr "
                << strPrefix << m_strTypeName << L"& operator|=("
                << strPrefix << m_strTypeName << L"& eLHS, const " << strPrefix << m_strTypeName
                << L" eRHS)\n{    \n"
                << L"    eLHS = " << strPrefix << m_strTypeName
                << L"(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));\n"
                << L"    return eLHS;\n"
                << L"}\n";

        strmTar << L"[[nodiscard]] constexpr "
                << strPrefix << m_strTypeName << L" operator&(const "
                << strPrefix << m_strTypeName << L" eLHS, const " << strPrefix << m_strTypeName
                << L" eRHS)\n{    \n"
                << L"    return " << strPrefix << m_strTypeName
                << L"(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));\n"
                << L"}\n";

        strmTar << L"constexpr "
                << strPrefix << m_strTypeName << L"& operator&=("
                << strPrefix << m_strTypeName << L"& eLHS, const " << strPrefix << m_strTypeName
                << L" eRHS)\n{    \n"
                << L"    eLHS = " << strPrefix << m_strTypeName
                << L"(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));\n"
                << L"    return eLHS;\n"
                << L"}\n";
    }


    //
    //  If it supports text streaming, do that.
    //
    if (m_eTextStreamMap != TEnumMap::ETextVals::None)
    {
        strmTar << strExport << L" TTextOutStream& operator<<(TTextOutStream& strmTar, const "
                << strPrefix << m_strTypeName << L" eToStream);\n";
    }
}
