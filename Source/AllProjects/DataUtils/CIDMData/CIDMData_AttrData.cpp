//
// FILE NAME: CIDMData_AttrData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
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
//  This file implements the attribute data class
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMData_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAttrData, TObject)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMData_AttrData
{
    namespace
    {
        //
        //  Version 2 -
        //      Added the user data string member
        //
        constexpr tCIDLib::TCard2   c2FmtVersion = 2;
    }
}


// ---------------------------------------------------------------------------
//  CLASS: TAttrData
// PREFIX: adat
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAttrData: Public, static methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TAttrData::bParseAreaLim(const  TString&    strLims
                        ,       TArea&      areaLim
                        ,       TString&    strErrMsg)
{
    CIDAssert
    (
        strLims.bStartsWithI(kCIDMData::strAttrLim_AreaPref)
        , L"The passed limit string did not start with Area:"
    );

    TString strLim = strLims;
    strLim.Cut(0, kCIDMData::strAttrLim_AreaPref.c4Length());
    if (!areaLim.bParseFromText(strLim))
    {
        strErrMsg = L"The area limit was not correctly formatted";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TAttrData::bParseSizeLim(const  TString&    strLims
                        ,       TSize&      szMin
                        ,       TSize&      szMax
                        ,       TString&    strErrMsg)
{
    CIDAssert
    (
        strLims.bStartsWithI(kCIDMData::strAttrLim_SizePref)
        , L"The passed limit string did not start with Size:"
    );

    TString strLim1 = strLims;
    strLim1.Cut(0, kCIDMData::strAttrLim_SizePref.c4Length());

    TString strLim2;
    if (!strLim1.bSplit(strLim2, kCIDLib::chForwardSlash))
    {
        strErrMsg = L"The size limit was not correctly formatted";
        return kCIDLib::False;
    }
    strLim1.StripWhitespace();
    strLim2.StripWhitespace();

    if (!szMin.bParseFromText(strLim1))
    {
        strErrMsg = L"The minimum size limits are invalid";
        return kCIDLib::False;
    }

    if (!szMax.bParseFromText(strLim2))
    {
        strErrMsg = L"The maximum size limits are invalid";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


const TString& TAttrData::strId(const TAttrData& attrSrc)
{
    return attrSrc.strId();
}


// ---------------------------------------------------------------------------
//  TAttrData: Constructors and Destructor
// ---------------------------------------------------------------------------
TAttrData::TAttrData() :

    m_bAlwaysReport(kCIDLib::False)
    , m_c8User(0)
    , m_eEditType(tCIDMData::EAttrEdTypes::None)
    , m_eType(tCIDMData::EAttrTypes::String)
    , m_pmbufVal(nullptr)
{
    // Make sure all the value fields are reset
    ClearValue();
}

TAttrData::TAttrData( const TString&                strName
                    , const TString&                strId
                    , const tCIDMData::EAttrTypes   eType
                    , const tCIDMData::EAttrEdTypes eEdType) :

    m_bAlwaysReport(kCIDLib::False)
    , m_c8User(0)
    , m_eType(eType)
    , m_eEditType(eEdType)
    , m_pmbufVal(nullptr)
    , m_strId(strId)
    , m_strName(strName)
{
    //
    //  Make sure all the value fields are reset. This will create a default buffer
    //  if the type is binary. Where possible call the one below that takes the
    //  binary data up front.
    //
    ClearValue();

    // Set an initial edit type if they don't set a specific one
    if (m_eEditType == tCIDMData::EAttrEdTypes::Count)
        SetEditTypes();
}

TAttrData::TAttrData( const TString&                strName
                    , const TString&                strId
                    , const TString&                strLimits
                    , const tCIDMData::EAttrTypes   eType
                    , const tCIDMData::EAttrEdTypes eEdType) :

    m_bAlwaysReport(kCIDLib::False)
    , m_c8User(0)
    , m_eType(eType)
    , m_eEditType(eEdType)
    , m_pmbufVal(nullptr)
    , m_strId(strId)
    , m_strLimits(strLimits)
    , m_strName(strName)
{
    //
    //  Make sure all the value fields are reset. If they called this for binary
    //  type, instead of the one below that takes the binary data, we'll get a
    //  default buffer for now.
    //
    ClearValue();

    // Set an initial edit type if they don't set a specific one
    if (m_eEditType == tCIDMData::EAttrEdTypes::Count)
        SetEditTypes();
}

TAttrData::TAttrData( const TString&                strName
                    , const TString&                strId
                    , const TMemBuf&                mbufBinary
                    , const tCIDLib::TCard4         c4Bytes
                    , const TString&                strLimits
                    , const tCIDMData::EAttrEdTypes eEdType) :

    m_bAlwaysReport(kCIDLib::False)
    , m_c4Bytes(0)
    , m_c8User(0)
    , m_eType(tCIDMData::EAttrTypes::Binary)
    , m_eEditType(eEdType)
    , m_pmbufVal(nullptr)
    , m_strId(strId)
    , m_strLimits(strLimits)
    , m_strName(strName)
{
    //
    //  Create the buffer first, so that ClearValue() won't create a default. Then
    //  make sure the other values are clear. It will reset our byte count so we have
    //  set it below, instead of above in the initializer.
    //
    m_pmbufVal = new THeapBuf(c4Bytes ? c4Bytes : 1);
    ClearValue();

    // Now we can set the byte size and copy over the data
    m_c4Bytes = c4Bytes ? c4Bytes : 1;
    m_pmbufVal->CopyIn(mbufBinary, m_c4Bytes);

    // Set an initial edit type if they don't set a specific one
    if (m_eEditType == tCIDMData::EAttrEdTypes::Count)
        SetEditTypes();
}

TAttrData::TAttrData(const TAttrData& adatSrc) :

    m_bAlwaysReport(adatSrc.m_bAlwaysReport)
    , m_bEnabled(adatSrc.m_bEnabled)
    , m_c8User(adatSrc.m_c8User)
    , m_eEditType(adatSrc.m_eEditType)
    , m_eType(adatSrc.m_eType)
    , m_pmbufVal(nullptr)
    , m_strSpecType(adatSrc.m_strSpecType)
    , m_strGroup(adatSrc.m_strGroup)
    , m_strId(adatSrc.m_strId)
    , m_strLimits(adatSrc.m_strLimits)
    , m_strName(adatSrc.m_strName)
    , m_strUserData(adatSrc.m_strUserData)
{
    // We use a value reset method in this case to be efficient
    ResetValue(adatSrc);
}

TAttrData::~TAttrData()
{
    // If we allocated the binary data buffer, delete it
    if (m_pmbufVal)
        delete m_pmbufVal;
}

// ---------------------------------------------------------------------------
//  TAttrData: Public operators
// ---------------------------------------------------------------------------

TAttrData& TAttrData::operator=(const TAttrData& adatSrc)
{
    if (this != &adatSrc)
    {
        m_bAlwaysReport = adatSrc.m_bAlwaysReport;
        m_bEnabled      = adatSrc.m_bEnabled;
        m_c8User        = adatSrc.m_c8User;
        m_eEditType     = adatSrc.m_eEditType;
        m_strSpecType   = adatSrc.m_strSpecType;
        m_strGroup      = adatSrc.m_strGroup;
        m_strId         = adatSrc.m_strId;
        m_strLimits     = adatSrc.m_strLimits;
        m_strName       = adatSrc.m_strName;
        m_strUserData   = adatSrc.m_strUserData;

        //
        //  We use a reset method in this case to be efficient and handle a change
        //  in type.
        //
        ResetValue(adatSrc);
    }
    return *this;
}

//
//  Normally we wouldn't bother with this for this type of class, but we
//  pre-instantiate a vector of these and that picks up the move oriented
//  add method.
//
TAttrData& TAttrData::operator=(TAttrData&& adatSrc)
{
    if (&adatSrc != this)
    {
        tCIDLib::Swap(m_areaVal, adatSrc.m_areaVal);
        tCIDLib::Swap(m_bAlwaysReport, adatSrc.m_bAlwaysReport);
        tCIDLib::Swap(m_bEnabled, adatSrc.m_bEnabled);
        tCIDLib::Swap(m_bVal, adatSrc.m_bVal);
        tCIDLib::Swap(m_c4Bytes, adatSrc.m_c4Bytes);
        tCIDLib::Swap(m_c4Val, adatSrc.m_c4Val);
        tCIDLib::Swap(m_c4Val2, adatSrc.m_c4Val2);
        tCIDLib::Swap(m_c8User, adatSrc.m_c8User);
        tCIDLib::Swap(m_c8Val, adatSrc.m_c8Val);
        tCIDLib::Swap(m_chVal, adatSrc.m_chVal);
        tCIDLib::Swap(m_clrVal, adatSrc.m_clrVal);
        tCIDLib::Swap(m_eEditType, adatSrc.m_eEditType);
        tCIDLib::Swap(m_eType, adatSrc.m_eType);
        tCIDLib::Swap(m_f8Val, adatSrc.m_f8Val);
        tCIDLib::Swap(m_i4Val, adatSrc.m_i4Val);
        tCIDLib::Swap(m_i4Val2, adatSrc.m_i4Val2);
        tCIDLib::Swap(m_pmbufVal, adatSrc.m_pmbufVal);
        tCIDLib::Swap(m_pntVal, adatSrc.m_pntVal);
        tCIDLib::Swap(m_szVal, adatSrc.m_szVal);
        m_strGroup = tCIDLib::ForceMove(adatSrc.m_strGroup);
        m_strId = tCIDLib::ForceMove(adatSrc.m_strId);
        m_strLimits = tCIDLib::ForceMove(adatSrc.m_strLimits);
        m_strName = tCIDLib::ForceMove(adatSrc.m_strName);
        m_strSpecType = tCIDLib::ForceMove(adatSrc.m_strSpecType);
        m_strValue = tCIDLib::ForceMove(adatSrc.m_strValue);
        m_strUserData = tCIDLib::ForceMove(adatSrc.m_strUserData);
    }
    return *this;
}


tCIDLib::TBoolean TAttrData::operator==(const TAttrData& adatSrc) const
{
    if (this == &adatSrc)
        return kCIDLib::True;

    if ((m_eType != adatSrc.m_eType)
    ||  (m_eEditType != adatSrc.m_eEditType)
    ||  (m_bAlwaysReport != adatSrc.m_bAlwaysReport)
    ||  (m_c8User != adatSrc.m_c8User)
    ||  (m_strUserData != adatSrc.m_strUserData))
    {
        return kCIDLib::False;
    }

    // Otherwise it's just the specific value type we care about
    return bSameValue(adatSrc);
}

tCIDLib::TBoolean TAttrData::operator!=(const TAttrData& adatSrc) const
{
    return !operator==(adatSrc);
}



// ---------------------------------------------------------------------------
//  TAttrData: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the area val
const TArea& TAttrData::areaVal() const
{
    CheckType(tCIDMData::EAttrTypes::Area);
    return m_areaVal;
}


// Get or set the always report flag
tCIDLib::TBoolean TAttrData::bAlwaysReport() const
{
    return m_bAlwaysReport;
}

tCIDLib::TBoolean TAttrData::bAlwaysReport(const tCIDLib::TBoolean bToSet)
{
    m_bAlwaysReport = bToSet;
    return m_bAlwaysReport;
}


// Return the boolean value
tCIDLib::TBoolean TAttrData::bVal() const
{
    CheckType(tCIDMData::EAttrTypes::Bool);
    return m_bVal;
}


// A convenience to check the edit type
tCIDLib::TBoolean TAttrData::bInPlaceEdit() const
{
    return (m_eEditType == tCIDMData::EAttrEdTypes::Both)
           || (m_eEditType == tCIDMData::EAttrEdTypes::InPlace);
}

tCIDLib::TBoolean TAttrData::bReadOnly() const
{
    return (m_eEditType == tCIDMData::EAttrEdTypes::None);
}

tCIDLib::TBoolean TAttrData::bVisEdit() const
{
    return (m_eEditType == tCIDMData::EAttrEdTypes::Both)
           || (m_eEditType == tCIDMData::EAttrEdTypes::Visual);
}


// Compare our value to the passed attribute objects value
tCIDLib::TBoolean TAttrData::bSameValue(const TAttrData& adatTest) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::Area :
            bRet = m_areaVal == adatTest.m_areaVal;
            break;

        case tCIDMData::EAttrTypes::Binary :
            bRet = m_c4Bytes == adatTest.m_c4Bytes;
            if (bRet)
            {
                bRet = m_pmbufVal->eCompare(*adatTest.m_pmbufVal, m_c4Bytes)
                                                        == tCIDLib::ESortComps::Equal;
            }
            break;

        case tCIDMData::EAttrTypes::Bool :
            bRet = m_bVal == adatTest.m_bVal;
            break;

        case tCIDMData::EAttrTypes::Card :
            bRet = m_c4Val == adatTest.m_c4Val;
            break;

        case tCIDMData::EAttrTypes::CardRange :
            bRet = (m_c4Val == adatTest.m_c4Val) && (m_c4Val2 == adatTest.m_c4Val2);
            break;

        case tCIDMData::EAttrTypes::Card64 :
            bRet = m_c8Val == adatTest.m_c8Val;
            break;

        case tCIDMData::EAttrTypes::Char :
            bRet = (m_chVal == adatTest.m_chVal);
            break;

        case tCIDMData::EAttrTypes::Float :
            bRet = m_f8Val == adatTest.m_f8Val;
            break;

        case tCIDMData::EAttrTypes::Int :
            bRet = m_i4Val == adatTest.m_i4Val;
            break;

        case tCIDMData::EAttrTypes::IntRange :
            bRet = (m_i4Val == adatTest.m_i4Val) && (m_i4Val2 == adatTest.m_i4Val2);
            break;

        case tCIDMData::EAttrTypes::Point :
            bRet = m_pntVal == adatTest.m_pntVal;
            break;

        case tCIDMData::EAttrTypes::RGBColor :
            bRet = m_clrVal == adatTest.m_clrVal;
            break;

        case tCIDMData::EAttrTypes::AppImage :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::MLString :
        case tCIDMData::EAttrTypes::String :
            bRet = m_strValue == adatTest.m_strValue;
            break;

        case tCIDMData::EAttrTypes::Size :
            bRet = m_szVal == adatTest.m_szVal;
            break;

        case tCIDMData::EAttrTypes::Time :
            bRet = m_c8Val == adatTest.m_c8Val;
            break;

        default :
            CIDAssert2(L"Unknown attribute type in same value comparison");
            break;
    };
    return bRet;
}


// Return the bytes in the memory buffer
tCIDLib::TCard4 TAttrData::c4Bytes() const
{
    return m_c4Bytes;
}


// Return cardinal values
tCIDLib::TCard4 TAttrData::c4Val() const
{
    return m_c4Val;
}

tCIDLib::TCard4 TAttrData::c4Val2() const
{
    return m_c4Val2;
}


// Get/set the 64 bit user value
tCIDLib::TCard8 TAttrData::c8User() const
{
    return m_c8User;
}

tCIDLib::TCard8 TAttrData::c8User(const tCIDLib::TCard8 c8ToSet)
{
    m_c8User = c8ToSet;
    return m_c8User;
}


tCIDLib::TCard8 TAttrData::c8Val() const
{
    return m_c8Val;
}

tCIDLib::TCh TAttrData::chVal() const
{
    return m_chVal;
}

const TRGBClr& TAttrData::clrVal() const
{
    CheckType(tCIDMData::EAttrTypes::RGBColor);
    return m_clrVal;
}


// Change the data type of this attribute
tCIDLib::TVoid TAttrData::ChangeType(const tCIDMData::EAttrTypes eToSet)
{
    m_eType = eToSet;

    // Make sure all the values get reset
    ClearValue();
}


tCIDLib::TVoid TAttrData::ClearValue()
{
    //
    //  Just clear them all, since they may be resetting the object and we don't want
    //  old data hanging around.
    //
    m_areaVal.ZeroAll();
    m_bVal = kCIDLib::False;
    m_c4Bytes = 0;
    m_c4Val = 0;
    m_c4Val2 = 0;
    m_chVal = kCIDLib::chSpace;
    m_clrVal.ToBlack();
    m_f8Val = 0;
    m_i4Val = 0;
    m_i4Val2 = 0;
    m_pntVal.Zero();
    m_szVal.Zero();
    m_strValue.Clear();

    //
    //  If we have a buffer, but we aren't binary anymore, get rid of it. If we don't
    //  have one and we are binary, then create one with a default size. The latter
    //  maybe inefficient, but generally the other code tries to avoid this. We do
    //  need to make sure something is there though worst case if that is not done.
    //
    if (m_pmbufVal && (m_eType != tCIDMData::EAttrTypes::Binary))
    {
        delete m_pmbufVal;
        m_pmbufVal = nullptr;
    }
     else if (!m_pmbufVal && (m_eType == tCIDMData::EAttrTypes::Binary))
     {
         m_pmbufVal = new THeapBuf(1);
     }
}


tCIDLib::TVoid TAttrData::CopyVal(const TAttrData& adatSrc)
{
    // Make sure the types are the same
    if (m_eType != adatSrc.m_eType)
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcAttr_ValAssign
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strId
            , tCIDMData::strXlatEAttrTypes(m_eType)
            , tCIDMData::strXlatEAttrTypes(adatSrc.m_eType)
        );
    }

    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::AppImage :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::MLString :
        case tCIDMData::EAttrTypes::String :
            m_strValue = adatSrc.m_strValue;
            break;

        case tCIDMData::EAttrTypes::Area :
            m_areaVal = adatSrc.m_areaVal;
            break;

        case tCIDMData::EAttrTypes::Binary :
        {
            CIDAssert(m_pmbufVal != nullptr, L"Attribute is binary but no buffer exists");
            CIDLib_Suppress(6011) // We just null checked it
            m_pmbufVal->Reallocate(adatSrc.m_c4Bytes);
            m_pmbufVal->CopyIn(*adatSrc.m_pmbufVal, adatSrc.m_c4Bytes);
            m_c4Bytes = adatSrc.m_c4Bytes;
            break;
        }

        case tCIDMData::EAttrTypes::Bool :
            m_bVal = adatSrc.m_bVal;
            break;

        case tCIDMData::EAttrTypes::Card :
            m_c4Val = adatSrc.m_c4Val;
            break;

        case tCIDMData::EAttrTypes::CardRange :
            m_c4Val = adatSrc.m_c4Val;
            m_c4Val2 = adatSrc.m_c4Val2;
            break;

        case tCIDMData::EAttrTypes::Card64 :
            m_c8Val = adatSrc.m_c8Val;
            break;

        case tCIDMData::EAttrTypes::Char :
            m_chVal = adatSrc.m_chVal;
            break;

        case tCIDMData::EAttrTypes::Float :
            m_f8Val = adatSrc.m_f8Val;
            break;

        case tCIDMData::EAttrTypes::Int :
            m_i4Val = adatSrc.m_i4Val;
            break;

        case tCIDMData::EAttrTypes::IntRange :
            m_i4Val = adatSrc.m_i4Val;
            m_i4Val2 = adatSrc.m_i4Val2;
            break;

        case tCIDMData::EAttrTypes::Point :
            m_pntVal = adatSrc.m_pntVal;
            break;

        case tCIDMData::EAttrTypes::RGBColor :
            m_clrVal = adatSrc.m_clrVal;
            break;

        case tCIDMData::EAttrTypes::Size :
            m_szVal = adatSrc.m_szVal;
            break;

        case tCIDMData::EAttrTypes::Time :
            m_c8Val = adatSrc.m_c8Val;
            break;

        case tCIDMData::EAttrTypes::Separator :
            // Nothing to do here
            break;

        default :
            CIDAssert2(L"Unknown attribute type in value copy");
            break;
    };
}


tCIDLib::TEncodedTime TAttrData::enctVal() const
{
    return m_c8Val;
}



// Get or set the edit types this attribute supports
tCIDMData::EAttrEdTypes TAttrData::eEditType() const
{
    return m_eEditType;
}

tCIDMData::EAttrEdTypes TAttrData::eEditType(const tCIDMData::EAttrEdTypes eToSet)
{
    m_eEditType = eToSet;
    return m_eEditType;
}


tCIDMData::EAttrTypes TAttrData::eType() const
{
    return m_eType;
}


tCIDLib::TFloat8 TAttrData::f8Val() const
{
    return m_f8Val;
}


//
//  We need to be able to format any values for display, though binary values aren't
//  really displayable to we return an empty string for those.
//
tCIDLib::TVoid TAttrData::FormatToText(TString& strToFill) const
{
    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::AppImage :
            strToFill = m_strValue;
            break;

        case tCIDMData::EAttrTypes::Area :
            m_areaVal.FormatToText(strToFill);
            break;

        case tCIDMData::EAttrTypes::Bool :
            strToFill = facCIDMData().strBoolAttr(m_bVal);
            break;

        case tCIDMData::EAttrTypes::Card :
            strToFill.SetFormatted(m_c4Val);
            break;

        case tCIDMData::EAttrTypes::CardRange :
            strToFill.SetFormatted(m_c4Val);
            strToFill.Append(L", ");
            strToFill.AppendFormatted(m_c4Val2);
            break;

        case tCIDMData::EAttrTypes::Card64 :
            strToFill.SetFormatted(m_c8Val);
            break;

        case tCIDMData::EAttrTypes::Char :
            strToFill.Clear();
            strToFill.Append(m_chVal);
            break;

        case tCIDMData::EAttrTypes::File :
            strToFill = m_strValue;
            break;

        case tCIDMData::EAttrTypes::Float :
            strToFill.SetFormatted(m_f8Val, 5);
            break;

        case tCIDMData::EAttrTypes::Int :
            strToFill.SetFormatted(m_i4Val);
            break;

        case tCIDMData::EAttrTypes::IntRange :
            strToFill.SetFormatted(m_i4Val);
            strToFill.Append(L", ");
            strToFill.AppendFormatted(m_i4Val2);
            break;

        case tCIDMData::EAttrTypes::Point :
            m_pntVal.FormatToText(strToFill);
            break;

        case tCIDMData::EAttrTypes::RGBColor :
            m_clrVal.FormatToText(strToFill, kCIDLib::chComma);
            break;

        case tCIDMData::EAttrTypes::MLString :
            // We don't display these, since it could be really long
            strToFill.Clear();
            break;

        case tCIDMData::EAttrTypes::Size :
            m_szVal.FormatToText(strToFill);
            break;

        case tCIDMData::EAttrTypes::String :
            strToFill = m_strValue;
            break;

        case tCIDMData::EAttrTypes::Time :
            strToFill.SetFormatted(m_c8Val);
            break;

        default :
            strToFill.Clear();
            break;
    };
}


// Return the int values
tCIDLib::TInt4 TAttrData::i4Val() const
{
    return m_i4Val;
}

tCIDLib::TInt4 TAttrData::i4Val2() const
{
    return m_i4Val2;
}


// Make the memory buffer value available
const TMemBuf& TAttrData::mbufVal() const
{
    CheckType(tCIDMData::EAttrTypes::Binary);
    return *m_pmbufVal;
}


// Make the point value available
const TPoint& TAttrData::pntVal() const
{
    CheckType(tCIDMData::EAttrTypes::Point);
    return m_pntVal;
}


const TString& TAttrData::strGroup() const
{
    return m_strGroup;
}


const TString& TAttrData::strId() const
{
    return m_strId;
}


const TString& TAttrData::strLimits() const
{
    return m_strLimits;
}

const TString& TAttrData::strLimits(const TString& strToSet)
{
    m_strLimits = strToSet;
    return m_strLimits;
}


// Get/set the the name of this attribute
const TString& TAttrData::strName() const
{
    return m_strName;
}

const TString& TAttrData::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


// Get the special type of this attribute
const TString& TAttrData::strSpecType() const
{
    return m_strSpecType;
}

// Get/set the the user data string member
const TString& TAttrData::strUserData() const
{
    return m_strUserData;
}

const TString& TAttrData::strUserData(const TString& strToSet)
{
    m_strUserData = strToSet;
    return m_strUserData;
}

// If a string type, get the value of this attribute
const TString& TAttrData::strValue() const
{
    return m_strValue;
}


// Return the size val
const TSize& TAttrData::szVal() const
{
    CheckType(tCIDMData::EAttrTypes::Size);
    return m_szVal;
}


tCIDLib::TVoid
TAttrData::Set( const   TString&                strName
                , const TString&                strId
                , const tCIDMData::EAttrTypes   eType
                , const tCIDMData::EAttrEdTypes eEdType)
{
    m_bAlwaysReport = kCIDLib::False;
    m_c8User = 0;
    m_eType = eType;
    m_eEditType = eEdType;
    m_strGroup.Clear();
    m_strId = strId;
    m_strLimits.Clear();
    m_strName = strName;
    m_strSpecType.Clear();
    m_strUserData.Clear();

    // Make sure all the value fields are reset
    ClearValue();

    // Set an initial edit type if they don't set a specific one
    if (m_eEditType == tCIDMData::EAttrEdTypes::Count)
        SetEditTypes();
}


tCIDLib::TVoid
TAttrData::Set( const   TString&                strName
                , const TString&                strId
                , const TString&                strLimits
                , const tCIDMData::EAttrTypes   eType
                , const tCIDMData::EAttrEdTypes eEdType)
{
    m_bAlwaysReport = kCIDLib::False;
    m_c8User = 0;
    m_eType = eType;
    m_eEditType = eEdType;
    m_strGroup.Clear();
    m_strId = strId;
    m_strLimits = strLimits;
    m_strName = strName;
    m_strSpecType.Clear();
    m_strUserData.Clear();

    // Make sure all the value fields are reset
    ClearValue();

    // Set an initial edit type if they don't set a specific one
    if (m_eEditType == tCIDMData::EAttrEdTypes::Count)
        SetEditTypes();
}



tCIDLib::TVoid TAttrData::SetSpecType(const TString& strToSet)
{
    m_strSpecType = strToSet;
}


tCIDLib::TVoid TAttrData::SetType(const tCIDMData::EAttrTypes eType)
{
    ClearValue();
    m_eType = eType;
}


//
//  In many cases, the client code may be generic and dealing with various data
//  types. We don't want to make him convert everything to specific types just to
//  set the value. So we can take it as a string, and convert to the target, or
//  try to.
//
tCIDLib::TVoid TAttrData::SetValueFromText(const TString& strToSet)
{
    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::Area :
            m_areaVal.bParseFromText(strToSet);
            break;

        case tCIDMData::EAttrTypes::Bool :
            m_bVal = (strToSet == L"True") || (strToSet == L"Yes") || (strToSet == L"1");
            break;

        case tCIDMData::EAttrTypes::Card :
            strToSet.bToCard4(m_c4Val);
            break;

        case tCIDMData::EAttrTypes::CardRange :
        {
            TString strLow = strToSet;
            TString strHigh;
            if (strLow.bSplit(strHigh, kCIDLib::chComma))
            {
                strLow.StripWhitespace();
                strHigh.StripWhitespace();

                tCIDLib::TCard4 c4Low = 0, c4High = 0;
                if (strLow.bToCard4(c4Low) && strHigh.bToCard4(c4High))
                {
                    m_c4Val = c4Low;
                    m_c4Val2 = c4High;
                }
            }
            break;
        }

        case tCIDMData::EAttrTypes::Card64 :
            strToSet.bToCard8(m_c8Val);
            break;

        case tCIDMData::EAttrTypes::Char :
            if (strToSet.bIsEmpty())
                m_chVal = kCIDLib::chSpace;
            else
                m_chVal = strToSet[0];
            break;

        case tCIDMData::EAttrTypes::Float :
            strToSet.bToFloat8(m_f8Val);
            break;

        case tCIDMData::EAttrTypes::Int :
            strToSet.bToInt4(m_i4Val);
            break;

        case tCIDMData::EAttrTypes::IntRange :
        {
            TString strLow = strToSet;
            TString strHigh;
            if (strLow.bSplit(strHigh, kCIDLib::chComma))
            {
                strLow.StripWhitespace();
                strHigh.StripWhitespace();

                tCIDLib::TInt4 i4Low = 0, i4High = 0;
                if (strLow.bToInt4(i4Low) && strHigh.bToInt4(i4High))
                {
                    m_i4Val = i4Low;
                    m_i4Val2 = i4High;
                }
            }
            break;
        }

        case tCIDMData::EAttrTypes::Point :
            m_pntVal.bParseFromText(strToSet);
            break;

        case tCIDMData::EAttrTypes::RGBColor :
            m_clrVal.bParseFromText(strToSet);
            break;

        case tCIDMData::EAttrTypes::AppImage :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::MLString :
        case tCIDMData::EAttrTypes::String :
            m_strValue = strToSet;
            break;

        case tCIDMData::EAttrTypes::Size :
            m_szVal.bParseFromText(strToSet);
            break;

        case tCIDMData::EAttrTypes::Time :
            strToSet.bToCard8(m_c8Val, tCIDLib::ERadices::Auto);
            break;

        default :
            m_c4Bytes = 0;
            m_strValue.Clear();
            break;
    };

    // Make sure we get a canaonical text form stored
    FormatToText(m_strValue);
}


//
//  Set the various value type values. We also write the value to the string
//  in a standard way for display purposes.
//
tCIDLib::TVoid TAttrData::SetArea(const TArea& areaToSet)
{
    CheckType(tCIDMData::EAttrTypes::Area);
    m_areaVal = areaToSet;
}

tCIDLib::TVoid TAttrData::SetBool(const tCIDLib::TBoolean bToSet)
{
    CheckType(tCIDMData::EAttrTypes::Bool);
    m_bVal = bToSet;
}

tCIDLib::TVoid TAttrData::SetCard(const tCIDLib::TCard4 c4ToSet)
{
    CheckType(tCIDMData::EAttrTypes::Card);
    m_c4Val = c4ToSet;
}

tCIDLib::TVoid
TAttrData::SetCardRange(const tCIDLib::TCard4 c4Min, const tCIDLib::TCard4 c4Max)
{
    CheckType(tCIDMData::EAttrTypes::CardRange);
    m_c4Val = c4Min;
    m_c4Val2 = c4Min;
}

tCIDLib::TVoid TAttrData::SetCard64(const tCIDLib::TCard8 c8ToSet)
{
    CheckType(tCIDMData::EAttrTypes::Card64);
    m_c8Val = c8ToSet;
}

tCIDLib::TVoid TAttrData::SetChar(const tCIDLib::TCh chToSet)
{
    CheckType(tCIDMData::EAttrTypes::Char);
    m_chVal = chToSet;
}

tCIDLib::TVoid TAttrData::SetFloat(const tCIDLib::TFloat8 f8ToSet)
{
    CheckType(tCIDMData::EAttrTypes::Float);
    m_f8Val = f8ToSet;
}

tCIDLib::TVoid TAttrData::SetInt(const tCIDLib::TInt4 i4ToSet)
{
    CheckType(tCIDMData::EAttrTypes::Int);
    m_i4Val = i4ToSet;
}

tCIDLib::TVoid
TAttrData::SetIntRange(const tCIDLib::TInt4  i4Min, const tCIDLib::TInt4 i4Max)
{
    CheckType(tCIDMData::EAttrTypes::IntRange);
    m_i4Val = i4Min;
    m_i4Val2 = i4Max;
}

tCIDLib::TVoid TAttrData::SetMemBuf(const TMemBuf& mbufData, const tCIDLib::TCard4 c4Bytes)
{
    CheckType(tCIDMData::EAttrTypes::Binary);
    m_pmbufVal->CopyIn(mbufData, c4Bytes);
    m_c4Bytes = c4Bytes;
}

tCIDLib::TVoid TAttrData::SetMemBuf(THeapBuf&& mbufData, const tCIDLib::TCard4 c4Bytes)
{
    CheckType(tCIDMData::EAttrTypes::Binary);
    *m_pmbufVal = tCIDLib::ForceMove(mbufData);
    m_c4Bytes = c4Bytes;
}

tCIDLib::TVoid TAttrData::SetPoint(const TPoint& pntToSet)
{
    CheckType(tCIDMData::EAttrTypes::Point);
    m_pntVal = pntToSet;
}

tCIDLib::TVoid TAttrData::SetRawString(const tCIDLib::TCh* const pszValue)
{
    if ((m_eType != tCIDMData::EAttrTypes::AppImage)
    &&  (m_eType != tCIDMData::EAttrTypes::File)
    &&  (m_eType != tCIDMData::EAttrTypes::MLString)
    &&  (m_eType != tCIDMData::EAttrTypes::String))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcAttr_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strId
            , tCIDMData::strXlatEAttrTypes(tCIDMData::EAttrTypes::String)
            , tCIDMData::strXlatEAttrTypes(m_eType)
        );
    }
    m_strValue = pszValue;
}

tCIDLib::TVoid TAttrData::SetRGBColor(const TRGBClr& clrToSet)
{
    CheckType(tCIDMData::EAttrTypes::RGBColor);
    m_clrVal = clrToSet;
}

tCIDLib::TVoid TAttrData::SetSize(const TSize& szToSet)
{
    CheckType(tCIDMData::EAttrTypes::Size);
    m_szVal = szToSet;
}

tCIDLib::TVoid TAttrData::SetString(const TString& strToSet)
{
    if ((m_eType != tCIDMData::EAttrTypes::AppImage)
    &&  (m_eType != tCIDMData::EAttrTypes::File)
    &&  (m_eType != tCIDMData::EAttrTypes::MLString)
    &&  (m_eType != tCIDMData::EAttrTypes::String))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcAttr_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strId
            , tCIDMData::strXlatEAttrTypes(tCIDMData::EAttrTypes::String)
            , tCIDMData::strXlatEAttrTypes(m_eType)
        );
    }
    m_strValue = strToSet;
}

tCIDLib::TVoid TAttrData::SetTime(const tCIDLib::TEncodedTime enctToSet)
{
    CheckType(tCIDMData::EAttrTypes::Time);
    m_c8Val = enctToSet;
}




// ---------------------------------------------------------------------------
//  TAttrData: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Checks that our type is the same as the passed one. If not, we throw
tCIDLib::TVoid TAttrData::CheckType(const tCIDMData::EAttrTypes eToCheck) const
{
    if (m_eType != eToCheck)
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcAttr_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strName
            , tCIDMData::strXlatEAttrTypes(eToCheck)
            , tCIDMData::strXlatEAttrTypes(m_eType)
        );
    }
}


//
//  In an assignment or copy, this is called to let us optimize some scenarios.
//
tCIDLib::TVoid TAttrData::ResetValue(const TAttrData& adatSrc)
{
    if ((adatSrc.m_eType == tCIDMData::EAttrTypes::Binary) && !m_pmbufVal)
    {
        //
        //  The source is binary and we are not. So we just want to make sure our
        //  binary buffer is created first, so that ClearValue won't create a default
        //  one, which the copy will just have to resize.
        //
        m_pmbufVal = new THeapBuf(adatSrc.m_c4Bytes);
    }

    // Now take the source's type and do the usual thing
    m_eType = adatSrc.m_eType;
    ClearValue();
    CopyVal(adatSrc);
}


//
//  Check the type. If it's one of our build in supported types, set the type of
//  editing that we provide for that type of value. Client code must set the edit
//  type for its own defined types, and might override these for its own purposes.
//
tCIDLib::TVoid TAttrData::SetEditTypes()
{
    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::Area :
        case tCIDMData::EAttrTypes::Card :
        case tCIDMData::EAttrTypes::CardRange :
        case tCIDMData::EAttrTypes::Card64 :
        case tCIDMData::EAttrTypes::Char :
        case tCIDMData::EAttrTypes::Float :
        case tCIDMData::EAttrTypes::Int :
        case tCIDMData::EAttrTypes::IntRange :
        case tCIDMData::EAttrTypes::Point :
        case tCIDMData::EAttrTypes::Size :
            m_eEditType = tCIDMData::EAttrEdTypes::InPlace;
            break;

        case tCIDMData::EAttrTypes::AppImage :
        case tCIDMData::EAttrTypes::Bool :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::RGBColor :
        case tCIDMData::EAttrTypes::String :
            m_eEditType = tCIDMData::EAttrEdTypes::Both;
            break;

        case tCIDMData::EAttrTypes::MLString :
            m_eEditType = tCIDMData::EAttrEdTypes::Visual;
            break;

        case tCIDMData::EAttrTypes::Separator :
            m_eEditType = tCIDMData::EAttrEdTypes::None;
            break;

        case tCIDMData::EAttrTypes::Time :
            m_eEditType = tCIDMData::EAttrEdTypes::Visual;
            break;

        default :
            // Pretty much has to be some custom visual edit
            m_eEditType = tCIDMData::EAttrEdTypes::Visual;
            break;
    };
}


// ---------------------------------------------------------------------------
//  TAttrData: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TAttrData::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Check for a frame marker and valid format version
    const tCIDLib::TCard2 c2FmtVersion = TBinInStream::c2CheckFmtVersion
    (
        strmToReadFrom
        , tCIDLib::EStreamMarkers::Frame
        , CIDMData_AttrData::c2FmtVersion
        , clsThis()
        , CID_FILE
        , CID_LINE
    );

    // Get the type out first
    strmToReadFrom >> m_eType;

    if (m_eType == tCIDMData::EAttrTypes::Binary)
    {
        //
        //  To avoid possibly creating a default buffer only to turn around
        //  and have to reallocate, we handle this one specially. We create
        //  or resize the buffer and load it up first, so that ClearValue won't
        //  create a default one. Then we can make sure the other values are
        //  cleared, then store the right buffer size (which will have gotten
        //  zeroed by the clear.
        //
        tCIDLib::TCard4 c4Size;
        strmToReadFrom >> c4Size;
        if (!m_pmbufVal)
            m_pmbufVal = new THeapBuf(c4Size);
        else
            m_pmbufVal->Reallocate(c4Size);
        strmToReadFrom.c4ReadBuffer(*m_pmbufVal, c4Size);

        ClearValue();
        m_c4Bytes = c4Size;
    }
     else
    {
        //
        //  Now do a clear, with it knowing our new type. It will get rid of any
        //  buffer if we aren't binary anymore.
        //
        ClearValue();

        // Based on the type, read in the value
        switch(m_eType)
        {
            case tCIDMData::EAttrTypes::AppImage :
            case tCIDMData::EAttrTypes::File :
            case tCIDMData::EAttrTypes::MLString :
            case tCIDMData::EAttrTypes::String :
                strmToReadFrom >> m_strValue;
                break;

            case tCIDMData::EAttrTypes::Area :
                strmToReadFrom >> m_areaVal;
                break;

            case tCIDMData::EAttrTypes::Bool :
                strmToReadFrom >> m_bVal;
                break;

            case tCIDMData::EAttrTypes::Card :
                strmToReadFrom >> m_c4Val;
                break;

            case tCIDMData::EAttrTypes::CardRange :
                strmToReadFrom >> m_c4Val >> m_c4Val2;
                break;

            case tCIDMData::EAttrTypes::Card64 :
            case tCIDMData::EAttrTypes::Time :
                strmToReadFrom >> m_c8Val;
                break;

            case tCIDMData::EAttrTypes::Char :
                strmToReadFrom >> m_chVal;
                break;

            case tCIDMData::EAttrTypes::Float :
                strmToReadFrom >> m_f8Val;
                break;

            case tCIDMData::EAttrTypes::Int :
                strmToReadFrom >> m_i4Val;
                break;

            case tCIDMData::EAttrTypes::IntRange :
                strmToReadFrom >> m_i4Val >> m_i4Val2;
                break;

            case tCIDMData::EAttrTypes::Point :
                strmToReadFrom >> m_pntVal;
                break;

            case tCIDMData::EAttrTypes::RGBColor :
                strmToReadFrom >> m_clrVal;
                break;

            case tCIDMData::EAttrTypes::Size :
                strmToReadFrom >> m_szVal;
                break;

            default :
                break;
        };
    }

    // Get the other stuff
    strmToReadFrom  >> m_bAlwaysReport
                    >> m_c8User
                    >> m_bEnabled
                    >> m_eEditType
                    >> m_strGroup
                    >> m_strId
                    >> m_strLimits
                    >> m_strName
                    >> m_strSpecType;

    // Read in or default V2 stuff
    if (c2FmtVersion > 1)
        strmToReadFrom >> m_strUserData;
    else
        m_strUserData.Clear();

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TAttrData::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << CIDMData_AttrData::c2FmtVersion
                    << m_eType;

    // Based on the type, write the correct value out
    switch(m_eType)
    {
        case tCIDMData::EAttrTypes::AppImage :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::MLString :
        case tCIDMData::EAttrTypes::String :
            strmToWriteTo << m_strValue;
            break;

        case tCIDMData::EAttrTypes::Area :
            strmToWriteTo << m_areaVal;
            break;

        case tCIDMData::EAttrTypes::Binary :
            strmToWriteTo << m_c4Bytes;
            strmToWriteTo.c4WriteBuffer(*m_pmbufVal, m_c4Bytes);
            break;

        case tCIDMData::EAttrTypes::Bool :
            strmToWriteTo << m_bVal;
            break;

        case tCIDMData::EAttrTypes::Card :
            strmToWriteTo << m_c4Val;
            break;

        case tCIDMData::EAttrTypes::CardRange :
            strmToWriteTo << m_c4Val << m_c4Val2;
            break;

        case tCIDMData::EAttrTypes::Card64 :
        case tCIDMData::EAttrTypes::Time :
            strmToWriteTo << m_c8Val;
            break;

        case tCIDMData::EAttrTypes::Char :
            strmToWriteTo << m_chVal;
            break;

        case tCIDMData::EAttrTypes::Float :
            strmToWriteTo << m_f8Val;
            break;

        case tCIDMData::EAttrTypes::Int :
            strmToWriteTo << m_i4Val;
            break;

        case tCIDMData::EAttrTypes::IntRange :
            strmToWriteTo << m_i4Val << m_i4Val2;
            break;

        case tCIDMData::EAttrTypes::Point :
            strmToWriteTo << m_pntVal;
            break;

        case tCIDMData::EAttrTypes::RGBColor :
            strmToWriteTo << m_clrVal;
            break;

        case tCIDMData::EAttrTypes::Size :
            strmToWriteTo << m_szVal;
            break;

        default :
            break;
    };

    // And the other stuff and end marker
    strmToWriteTo   << m_bAlwaysReport
                    << m_c8User
                    << m_bEnabled
                    << m_eEditType
                    << m_strGroup
                    << m_strId
                    << m_strLimits
                    << m_strName
                    << m_strSpecType

                    // V2 stuff
                    << m_strUserData

                    << tCIDLib::EStreamMarkers::EndObject;
}
