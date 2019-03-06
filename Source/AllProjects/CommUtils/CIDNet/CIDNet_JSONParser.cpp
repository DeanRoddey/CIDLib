//
// FILE NAME: CIDNet_JSONParser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/22/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TJSONParser class.
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
#include    "CIDNet_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TJSONParser,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TJSONValue
//  PREFIX: jprsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONValue: Destructor
// ---------------------------------------------------------------------------
TJSONValue::~TJSONValue()
{
}


// ---------------------------------------------------------------------------
//  TJSONValue: Pubic, inherited methods
// ---------------------------------------------------------------------------

//
//  At this level we just put out the name (if set) and colon.
//
tCIDLib::TVoid
TJSONValue::FormatToText(       TTextOutStream&     strmTar
                        , const tCIDLib::TCard4     c4Level
                        , const tCIDLib::TCard4     c4IdentSize) const
{
    //
    //  If doing indentation, then do that if we have a name, or the
    //  type is simple.
    //
    const tCIDLib::TBoolean bHaveName = !m_strName.bIsEmpty();
    if (c4IdentSize && bHaveName || !bIsContType())
        strmTar << TTextOutStream::Spaces(c4Level * c4IdentSize);

    // And if we have a name, output that
    if (bHaveName)
        strmTar << L"\"" << m_strName << L"\" :";
}


// Just reset our type and name
tCIDLib::TVoid TJSONValue::Reset()
{
    m_eType = tCIDNet::EJSONVTypes::Null;
    m_strName.Clear();
}


// If not overridden, return an empty string
const TString& TJSONValue::strValue() const
{
    return TString::strEmpty();
}



// ---------------------------------------------------------------------------
//  TJSONValue: Pubic, non-virtual methods
// ---------------------------------------------------------------------------

// Helpers to check for overall container vs. simple value types
tCIDLib::TBoolean TJSONValue::bIsContType() const
{
    return ((m_eType == tCIDNet::EJSONVTypes::Array)
           || (m_eType == tCIDNet::EJSONVTypes::Object));

}

tCIDLib::TBoolean TJSONValue::bIsSimpleType() const
{
    return ((m_eType != tCIDNet::EJSONVTypes::Array)
           && (m_eType != tCIDNet::EJSONVTypes::Object));
}


tCIDNet::EJSONVTypes TJSONValue::eType() const
{
    return m_eType;
}

const TString& TJSONValue::strName() const
{
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TJSONValue: Hidden constructors
// ---------------------------------------------------------------------------
TJSONValue::TJSONValue( const   tCIDNet::EJSONVTypes    eType
                        , const TString&                strName) :

    m_eType(eType)
    , m_strName(strName)
{
}


// ---------------------------------------------------------------------------
//  TJSONValue: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Change our type, for derived classes to do this if needed, generally only workable
//  for chaning simple value nodes.
//
tCIDLib::TVoid TJSONValue::SetType(const tCIDNet::EJSONVTypes eToSet)
{
    m_eType = eToSet;
}



// ---------------------------------------------------------------------------
//   CLASS: TJSONSimpleVal
//  PREFIX: jprsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONSimpleVal: Constructors and Destructor
// ---------------------------------------------------------------------------

TJSONSimpleVal::TJSONSimpleVal( const   TString&                strValue
                                , const tCIDNet::EJSONVTypes    eValType
                                , const TString&                strName
                                , const tCIDLib::TBoolean       bEscape) :

    TJSONValue(eValType, strName)
    , m_strValue(strValue)
{
    //
    //  Make sure the passed type is not one of the nested types. It must
    //  be a simple value type.
    //
    if ((eValType == tCIDNet::EJSONVTypes::Array)
    ||  (eValType == tCIDNet::EJSONVTypes::Object))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_NotASimpleValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }

    //
    //  If there's a string value, then we have to deal with any single \ slashes
    //  by escaping them with another \ if asked to. We run through and double any
    //  slashes.
    //
    if (bEscape && !m_strValue.bIsEmpty() && (eValType == tCIDNet::EJSONVTypes::String))
    {
        tCIDLib::TCard4 c4At = 0;
        m_strValue.bReplaceSubStr(L"\\", L"\\\\", c4At, kCIDLib::True);
    }
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TBoolean bVal) :

    TJSONValue(bVal ? tCIDNet::EJSONVTypes::True : tCIDNet::EJSONVTypes::False, strName)
    , m_strValue(bVal ? L"true" : L"false")
{
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TCard1 c1Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(4UL)
{
    m_strValue.AppendFormatted(c1Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TCard2 c2Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(6UL)
{
    m_strValue.AppendFormatted(c2Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TCard4 c4Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(8UL)
{
    m_strValue.AppendFormatted(c4Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TCard8 c8Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(12UL)
{
    m_strValue.AppendFormatted(c8Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TFloat8 f8Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(8UL)
{
    m_strValue.AppendFormatted(f8Val, 5);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TInt1 i1Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(4UL)
{
    m_strValue.AppendFormatted(i1Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TInt2 i2Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(6UL)
{
    m_strValue.AppendFormatted(i2Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TInt4 i4Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(8UL)
{
    m_strValue.AppendFormatted(i4Val);
}

TJSONSimpleVal::TJSONSimpleVal(const TString& strName, const tCIDLib::TInt8 i8Val) :

    TJSONValue(tCIDNet::EJSONVTypes::Number, strName)
    , m_strValue(12UL)
{
    m_strValue.AppendFormatted(i8Val);
}

TJSONSimpleVal::TJSONSimpleVal( const   TString&          strName
                                , const TString&          strValue
                                , const tCIDLib::TBoolean bEscape) :

    TJSONValue(tCIDNet::EJSONVTypes::String, strName)
    , m_strValue(strValue)
{
    //
    //  If there's a value, then we have to deal with any single \ slashes by
    //  escaping them with another \ if asked to.
    //
    if (bEscape && !m_strValue.bIsEmpty())
    {
        tCIDLib::TCard4 c4At = 0;
        m_strValue.bReplaceSubStr(L"\\", L"\\\\", c4At, kCIDLib::True);
    }
}

TJSONSimpleVal::~TJSONSimpleVal()
{
}


// ---------------------------------------------------------------------------
//  TJSONSimpleVal: Public, inherited methods
// ---------------------------------------------------------------------------

// At this level we just put out the name (if set) and colon
tCIDLib::TVoid
TJSONSimpleVal::FormatToText(       TTextOutStream&     strmTar
                            , const tCIDLib::TCard4     c4Level
                            , const tCIDLib::TCard4     c4IndentSize) const
{
    // Call our parent to put out the name
    TJSONValue::FormatToText(strmTar, c4Level, c4IndentSize);

    // And do our value
    switch(eType())
    {
        case tCIDNet::EJSONVTypes::False :
            strmTar << L" false";
            break;

        case tCIDNet::EJSONVTypes::Null :
            strmTar << L" null";
            break;

        case tCIDNet::EJSONVTypes::Number :
        case tCIDNet::EJSONVTypes::String :
            strmTar << L" \"" << m_strValue << L"\"";
            break;

        case tCIDNet::EJSONVTypes::True :
            strmTar << L" true";
            break;

        default :
            CIDAssert2(L"Unknown JSON simple value type");
            break;
    };
}


// Call our parent and then clear our vlue string
tCIDLib::TVoid TJSONSimpleVal::Reset()
{
    TJSONValue::Reset();
    m_strValue.Clear();
}


const TString& TJSONSimpleVal::strValue() const
{
    return m_strValue;
}


//
//  Set the value as various types. If our type isn't of the type implied by the
//  set call, we throw.
//
tCIDLib::TVoid TJSONSimpleVal::SetBoolValue(const tCIDLib::TBoolean bToSet)
{
    if ((eType() != tCIDNet::EJSONVTypes::False)
    &&  (eType() != tCIDNet::EJSONVTypes::True))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }

    if (bToSet)
    {
        SetType(tCIDNet::EJSONVTypes::True);
        m_strValue = L"true";
    }
     else
    {
        SetType(tCIDNet::EJSONVTypes::False);
        m_strValue = L"false";
    }
}

tCIDLib::TVoid TJSONSimpleVal::SetNumValue(const tCIDLib::TCard4 c4ToSet)
{
    if (eType() != tCIDNet::EJSONVTypes::Number)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    m_strValue.SetFormatted(c4ToSet);
}

tCIDLib::TVoid TJSONSimpleVal::SetNumValue(const tCIDLib::TFloat8 f8ToSet)
{
    if (eType() != tCIDNet::EJSONVTypes::Number)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    m_strValue.SetFormatted(f8ToSet, 6);
}

tCIDLib::TVoid TJSONSimpleVal::SetNumValue(const tCIDLib::TInt4 i4ToSet)
{
    if (eType() != tCIDNet::EJSONVTypes::Number)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    m_strValue.SetFormatted(i4ToSet);
}

tCIDLib::TVoid TJSONSimpleVal::SetStrValue(const TString& strToSet)
{
    if (eType() != tCIDNet::EJSONVTypes::String)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    m_strValue = strToSet;
}





// ---------------------------------------------------------------------------
//   CLASS: TJSONCont
//  PREFIX: jprsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONCont: Destructor
// ---------------------------------------------------------------------------

TJSONCont::~TJSONCont()
{
    try
    {
        m_colValues.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TJSONCont: Pubic, inherited methods
// ---------------------------------------------------------------------------

//
//  At this level we handle foramtting for all of the container derivatives.
//
tCIDLib::TVoid
TJSONCont::FormatToText(        TTextOutStream&     strmTar
                        , const tCIDLib::TCard4     c4Level
                        , const tCIDLib::TCard4     c4IndentSize) const
{
    // Call our parent to put out the name if set
    TJSONValue::FormatToText(strmTar, c4Level, c4IndentSize);

    //
    //  Now loop through all of the children, recursing where required. We have to
    //  do the opening/closing brace/bracket according to the container type. Put
    //  out indentation if doing that. If the name is empty, then we didn't do any
    //  output above, so just stay on this line, else put out a newline.
    //
    if (!strName().bIsEmpty())
        strmTar << kCIDLib::NewLn;

    // Indent over if we are doing that
    if (c4IndentSize)
        strmTar << TTextOutStream::Spaces(c4Level * c4IndentSize);

    // And put out the opening bracket/brace
    const tCIDLib::TBoolean bArray(eType() == tCIDNet::EJSONVTypes::Array);
    if (bArray)
        strmTar << L"[\n";
    else
        strmTar << L"{\n";

    // Loop though the contained nodes and recurse.
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TJSONValue* pjprsnCur = m_colValues[c4Index];
        pjprsnCur->FormatToText(strmTar, c4Level + 1, c4IndentSize);

        // If not the last one, put out a comma
        if (c4Index + 1 < c4Count)
            strmTar << L',';

        // And do a new line
        strmTar << kCIDLib::NewLn;
    }

    // Do the closing, with indent if doing that
    if (c4IndentSize)
        strmTar << TTextOutStream::Spaces(c4Level * c4IndentSize);
    if (eType() == tCIDNet::EJSONVTypes::Array)
        strmTar << L"]";
    else
        strmTar << L"}";
}


// Call our parent and clear out any existing elements
tCIDLib::TVoid TJSONCont::Reset()
{
    TJSONValue::Reset();
    m_colValues.RemoveAll();
}



// ---------------------------------------------------------------------------
//  TJSONCont: Pubic, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TJSONCont::AddBoolean(const TString& strName, const tCIDLib::TBoolean bValue)
{
    AdoptValue(new TJSONSimpleVal(strName, bValue));
}

tCIDLib::TVoid TJSONCont::AddCard(const TString& strName, const tCIDLib::TCard1 c1Value)
{
    AdoptValue(new TJSONSimpleVal(strName, c1Value));
}

tCIDLib::TVoid TJSONCont::AddCard(const TString& strName, const tCIDLib::TCard2 c2Value)
{
    AdoptValue(new TJSONSimpleVal(strName, c2Value));
}

tCIDLib::TVoid TJSONCont::AddCard(const TString& strName, const tCIDLib::TCard4 c4Value)
{
    AdoptValue(new TJSONSimpleVal(strName, c4Value));
}

tCIDLib::TVoid TJSONCont::AddCard(const TString& strName, const tCIDLib::TCard8 c8Value)
{
    AdoptValue(new TJSONSimpleVal(strName, c8Value));
}

tCIDLib::TVoid TJSONCont::AddFloat(const TString& strName, const tCIDLib::TFloat8 f8Value)
{
    AdoptValue(new TJSONSimpleVal(strName, f8Value));
}

tCIDLib::TVoid TJSONCont::AddInt(const TString& strName, const tCIDLib::TInt1 i1Value)
{
    AdoptValue(new TJSONSimpleVal(strName, i1Value));
}

tCIDLib::TVoid TJSONCont::AddInt(const TString& strName, const tCIDLib::TInt2 i2Value)
{
    AdoptValue(new TJSONSimpleVal(strName, i2Value));
}

tCIDLib::TVoid TJSONCont::AddInt(const TString& strName, const tCIDLib::TInt4 i4Value)
{
    AdoptValue(new TJSONSimpleVal(strName, i4Value));
}

tCIDLib::TVoid TJSONCont::AddInt(const TString& strName, const tCIDLib::TInt8 i8Value)
{
    AdoptValue(new TJSONSimpleVal(strName, i8Value));
}

tCIDLib::TVoid TJSONCont::AddString(const TString& strName, const TString& strValue)
{
    AdoptValue(new TJSONSimpleVal(strName, strValue));
}


// Just adopt the passed value object into our list
tCIDLib::TVoid TJSONCont::AdoptValue(TJSONValue* const pjprsnToAdopt)
{
    m_colValues.Add(pjprsnToAdopt);
}


//
//  Find a boolean value by the indicated name. This one we have to get the raw
//  value and check teh type ourself, since it could be one of two types.
//
tCIDLib::TBoolean TJSONCont::bFindVal(const TString& strToFind) const
{
    const TJSONValue* pjprsnRet = const_cast<TJSONCont*>(this)->pjprsnValByName
    (
        strToFind, kCIDLib::True
    );

    tCIDLib::TBoolean bRet;
    if (pjprsnRet->eType() == tCIDNet::EJSONVTypes::False)
        bRet = kCIDLib::False;
    else if (pjprsnRet->eType() == tCIDNet::EJSONVTypes::True)
        bRet = kCIDLib::True;
    else
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcJSON_TypeMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return bRet;
}


// Test to see if a value is present and return the text of it if so
tCIDLib::TBoolean TJSONCont::bQueryVal(const TString& strToFind, TString& strToFill) const
{
    const TJSONValue* pjprsnRet = const_cast<TJSONCont*>(this)->pjprsnValByName
    (
        strToFind, kCIDLib::False
    );
    if (!pjprsnRet)
        return kCIDLib::False;

    strToFill = pjprsnRet->strValue();
    return kCIDLib::True;
}


// Find a numeric value by the indicated name
tCIDLib::TCard4 TJSONCont::c4FindVal(const TString& strToFind) const
{
    return ((TJSONCont*)(this))->pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, kCIDLib::True
    )->strValue().c4Val();
}


// Return the count of values we contain
tCIDLib::TCard4 TJSONCont::c4ValCount() const
{
    return m_colValues.c4ElemCount();
}



// Return the value type of the node at the indicated index
tCIDNet::EJSONVTypes TJSONCont::eTypeAt(const tCIDLib::TCard4 c4At) const
{
    return pjprsnTestIndex(c4At, CID_LINE)->eType();
}


// Find a numeric value by the indicated name
tCIDLib::TFloat8 TJSONCont::f8FindVal(const TString& strToFind) const
{
    return ((TJSONCont*)(this))->pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, kCIDLib::True
    )->strValue().f8Val();
}


// Find a numeric value by the indicated name
tCIDLib::TInt4 TJSONCont::i4FindVal(const TString& strToFind) const
{
    return ((TJSONCont*)(this))->pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, kCIDLib::True
    )->strValue().i4Val();
}


//
//  Get the generic value node at a specific index. Throw if bad index.
//  In this case we can return any of the value types, so no need to
//  check the type. This is what is used for generic iteration.
//
const TJSONValue&
TJSONCont::jprsnValueAt(const tCIDLib::TCard4 c4At) const
{
    return *pjprsnTestIndex(c4At, CID_LINE);
}


//
//  Get an array at a specific index. Throw if bad index. If not an array,
//  either throw or return null.
//
const TJSONArray*
TJSONCont::pjprsnArrayAt(const  tCIDLib::TCard4     c4At
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const TJSONValue* pjprsnRet = pjprsnTestIndex(c4At, CID_LINE);
    if (!bTestNodeType(*pjprsnRet, tCIDNet::EJSONVTypes::Array, CID_LINE, bThrowIfNot))
        return nullptr;

    return static_cast<const TJSONArray*>(pjprsnRet);
}



//
//  Find an array child by name. If not an array or not found, we either
//  throw or return null.
//
const TJSONArray*
TJSONCont::pjprsnFindArray( const   TString&            strToFind
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TCard4 c4FndAt;
    const TJSONValue* pjprsnVal = pjprsnFindVal(strToFind, 0, c4FndAt);
    if (!pjprsnVal)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_NameNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strToFind
            );
        }
        return nullptr;
    }

    if (pjprsnVal->eType() != tCIDNet::EJSONVTypes::Array)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_TypeMismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
            );
        }
        return nullptr;
    }

    // It's the right type, so cast and return
    return static_cast<const TJSONArray*>(pjprsnVal);
}


//
//  Find an object child by name. If not an object or not found, we either
//  throw or return null.
//
const TJSONObject*
TJSONCont::pjprsnFindObj(const  TString&            strToFind
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TCard4 c4FndAt;
    const TJSONValue* pjprsnVal = pjprsnFindVal(strToFind, 0, c4FndAt);
    if (!pjprsnVal)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_NameNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strToFind
            );
        }
        return nullptr;
    }

    if (pjprsnVal->eType() != tCIDNet::EJSONVTypes::Object)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_TypeMismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
            );
        }
        return nullptr;
    }

    // It's the right type, so cast and return
    return static_cast<const TJSONObject*>(pjprsnVal);
}


//
//  Find a simple value child by name. If not a value or not found, we throw.
//
const TJSONSimpleVal*
TJSONCont::pjprsnFindSVal(  const   TString&            strToFind
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    //
    //  Call a helper to find the passed value and verify it's a simpel value,
    //  but not a specific simple value type.
    //
    return ((TJSONCont*)(this))->pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Count, bThrowIfNot
    );
}


//
//  Find the first value with a given name, starting at a specific index.
//  If we find one, we return a pointer to the value and return the index
//  we found it at. Else we return null.
//
const TJSONValue*
TJSONCont::pjprsnFindVal(const  TString&            strToFind
                        , const tCIDLib::TCard4     c4StartAt
                        ,       tCIDLib::TCard4&    c4FoundAt) const
{
    // If the index is beyond the child count, obviously not
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    if (c4StartAt >= c4Count)
        return nullptr;

    // Else let's try to find it
    const TJSONValue* pjprsnRet = nullptr;
    for (c4FoundAt = c4StartAt; c4FoundAt < c4Count; c4FoundAt++)
    {
        const TJSONValue* pjprsnCur = m_colValues[c4FoundAt];
        if (pjprsnCur->strName() == strToFind)
        {
            pjprsnRet = pjprsnCur;
            break;
        }
    }
    return pjprsnRet;
}


//
//  Same as above but alway from zero and doesn't return a found at index. Also can
//  optionally throw if not found. We call a common helper, which is non-const, but
//  all it does is return a node pointer, so we cast off our const-ness to call it.
//
const TJSONValue*
TJSONCont::pjprsnFindVal(const TString&             strToFind
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    return const_cast<TJSONCont*>(this)->pjprsnValByName(strToFind, bThrowIfNot);
}



//
//  Get an object at a specific index. Throw if bad index. If not an object,
//  either throw or return null.
//
const TJSONObject*
TJSONCont::pjprsnObjAt( const   tCIDLib::TCard4     c4At
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const TJSONValue* pjprsnRet = pjprsnTestIndex(c4At, CID_LINE);
    if (!bTestNodeType(*pjprsnRet, tCIDNet::EJSONVTypes::Object, CID_LINE, bThrowIfNot))
        return nullptr;

    return static_cast<const TJSONObject*>(pjprsnRet);
}



//
//  Get the simple value at a specific index. Throw if bad index. If not
//  a simple value, either throw or return false.
//
const TJSONSimpleVal*
TJSONCont::pjprsnSValAt(const   tCIDLib::TCard4     c4At
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const TJSONValue* pjprsnRet = pjprsnTestIndex(c4At, CID_LINE);
    if (!bTestIsSimpleType(*pjprsnRet, CID_LINE, bThrowIfNot))
        return nullptr;

    return static_cast<const TJSONSimpleVal*>(pjprsnRet);
}


//
//  Return the name of the value node at the indicated index. It may be empty
//  if it's not a named type value.
//
const TString& TJSONCont::strNameAt(const tCIDLib::TCard4 c4At) const
{
    return pjprsnTestIndex(c4At, CID_LINE)->strName();
}


//
//  Return the simple value string of the node at the indicated index. It
//  may be empty if it's not a simple value type.
//
const TString& TJSONCont::strValueAt(const tCIDLib::TCard4 c4At) const
{
    return pjprsnTestIndex(c4At, CID_LINE)->strValue();
}


//
//  Look up the first child value with the indicated name and return its value. We
//  can do any value type node here. We call a common helper, which is non-const
//  but all it does is return a node pointer, so we just cast off our constness to
//  call it.
//
const TString& TJSONCont::strValByName(const TString& strToFind) const
{
    const TJSONValue* pjprsnRet = const_cast<TJSONCont*>(this)->pjprsnValByName
    (
        strToFind, kCIDLib::True
    );
    return pjprsnRet->strValue();
}



//
//  These allow you to add or update simple value nodes. If the node exists and is
//  not of the indicated type, it will throw. We can optinally add the node if it
//  does not exist, or throw if not found.
//
tCIDLib::TVoid
TJSONCont::SetBoolValue(const   TString&            strToFind
                        , const tCIDLib::TBoolean   bToSet
                        , const tCIDLib::TBoolean   bAddIfNot)
{
    //
    //  This one is special case, since it can be either a True or a False type,
    //  so we can't check for a specific type via the helper. We let the node
    //  check the type and throw if needed.
    //
    TJSONSimpleVal* pjprsnVal = pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Count, !bAddIfNot
    );
    if (!pjprsnVal)
        AdoptValue(new TJSONSimpleVal(strToFind, bToSet));
    else
        pjprsnVal->SetBoolValue(bToSet);
}

tCIDLib::TVoid
TJSONCont::SetNumValue( const   TString&            strToFind
                        , const tCIDLib::TCard4     c4ToSet
                        , const tCIDLib::TBoolean   bAddIfNot)
{
    TJSONSimpleVal* pjprsnVal = pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, !bAddIfNot
    );
    if (!pjprsnVal)
        AdoptValue(new TJSONSimpleVal(strToFind, c4ToSet));
    else
        pjprsnVal->SetNumValue(c4ToSet);
}

tCIDLib::TVoid
TJSONCont::SetNumValue( const   TString&            strToFind
                        , const tCIDLib::TFloat8    f8ToSet
                        , const tCIDLib::TBoolean   bAddIfNot)
{
    TJSONSimpleVal* pjprsnVal = pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, !bAddIfNot
    );
    if (!pjprsnVal)
        AdoptValue(new TJSONSimpleVal(strToFind, f8ToSet));
    else
        pjprsnVal->SetNumValue(f8ToSet);
}

tCIDLib::TVoid
TJSONCont::SetNumValue( const   TString&            strToFind
                        , const tCIDLib::TInt4      i4ToSet
                        , const tCIDLib::TBoolean   bAddIfNot)
{
    // Look it up and insure it's a number type, else throw
    TJSONSimpleVal* pjprsnVal = pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::Number, !bAddIfNot
    );
    if (!pjprsnVal)
        AdoptValue(new TJSONSimpleVal(strToFind, i4ToSet));
    else
        pjprsnVal->SetNumValue(i4ToSet);
}

tCIDLib::TVoid
TJSONCont::SetStrValue( const   TString&            strToFind
                        , const TString&            strToSet
                        , const tCIDLib::TBoolean   bAddIfNot)
{
    // Look it up and insure it's a string type, else throw
    TJSONSimpleVal* pjprsnVal = pjprsnSValByName
    (
        strToFind, tCIDNet::EJSONVTypes::String, !bAddIfNot
    );
    if (!pjprsnVal)
        AdoptValue(new TJSONSimpleVal(strToFind, strToSet));
    else
        pjprsnVal->SetStrValue(strToSet);
}



// ---------------------------------------------------------------------------
//  TJSONCont: Hidden constructor
// ---------------------------------------------------------------------------

// Pass on the name and type to our parent and init our value list
TJSONCont::TJSONCont(const tCIDNet::EJSONVTypes eType, const TString& strName) :

    TJSONValue(eType, strName)
    , m_colValues(tCIDLib::EAdoptOpts::Adopt, 8)
{
}


// ---------------------------------------------------------------------------
//  TJSONCont: protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A generic lookup for simple value nodes, which is used in a few places above.
//  We find it, throwing or returning null if not found, and make sure it's at
//  least a simple value type, and optionally a specific simple type.
//
TJSONSimpleVal*
TJSONCont::pjprsnSValByName(const   TString&                strToFind
                            , const tCIDNet::EJSONVTypes    eType
                            , const tCIDLib::TBoolean       bThrowIfNot)
{
    TJSONValue* pjprsnVal = pjprsnValByName(strToFind, bThrowIfNot);
    if (!pjprsnVal)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_NameNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strToFind
            );
        }
        return 0;
    }

    //
    //  It has to be a simple type, and if they provided a specific type they want
    //  us to check for, do that as well.
    //
    if (!pjprsnVal->bIsSimpleType()
    ||  ((eType != tCIDNet::EJSONVTypes::Count) && (pjprsnVal->eType() != eType)))
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_TypeMismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
            );
        }
        return nullptr;
    }

    // Cast it to its real type
    return static_cast<TJSONSimpleVal*>(pjprsnVal);
}


TJSONValue*
TJSONCont::pjprsnValByName( const   TString&            strToFind
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    TJSONValue* pjprsnRet = nullptr;
    m_colValues.ForEachNC
    (
        [&strToFind, &pjprsnRet](TJSONValue& jprsnCur) -> tCIDLib::TBoolean
        {
            if (jprsnCur.strName() == strToFind)
            {
                pjprsnRet = &jprsnCur;
                return kCIDLib::False;
            }
            return kCIDLib::True;
        }
    );

    if (!pjprsnRet)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_NameNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strToFind
            );
        }
    }
    return pjprsnRet;
}



// ---------------------------------------------------------------------------
//  TJSONCont: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Test the passed value to see if it's not one of the simple value
//  type nodes. We either throw if not, or return false, as requested.
//
tCIDLib::TBoolean
TJSONCont::bTestIsSimpleType(const  TJSONValue&         jprsnTest
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    if ((jprsnTest.eType() == tCIDNet::EJSONVTypes::Array)
    ||  (jprsnTest.eType() == tCIDNet::EJSONVTypes::Object))
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , c4Line
                , kNetErrs::errcJSON_NotASimpleValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Test the passed value to see if it's of the indicated type. We either
//  throw if not, or return false, as requested.
//
tCIDLib::TBoolean
TJSONCont::bTestNodeType(const  TJSONValue&             jprsnTest
                        , const tCIDNet::EJSONVTypes    eTest
                        , const tCIDLib::TCard4         c4Line
                        , const tCIDLib::TBoolean       bThrowIfNot) const
{
    if (jprsnTest.eType() != eTest)
    {
        if (bThrowIfNot)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , c4Line
                , kNetErrs::errcJSON_TypeMismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Test the index and if ok return the value at the indicated index
const TJSONValue*
TJSONCont::pjprsnTestIndex(const   tCIDLib::TCard4 c4At
                            , const tCIDLib::TCard4 c4Line) const
{
    if (c4At >= m_colValues.c4ElemCount())
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , c4Line
            , kNetErrs::errcJSON_BadChildInd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
        );
    }
    return m_colValues[c4At];
}






// ---------------------------------------------------------------------------
//   CLASS: TJSONArray
//  PREFIX: jprsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONArray: Constructors and Destructor
// ---------------------------------------------------------------------------

TJSONArray::TJSONArray(const TString& strName) :

    TJSONCont(tCIDNet::EJSONVTypes::Array, strName)
{
}

TJSONArray::~TJSONArray()
{
}


// ---------------------------------------------------------------------------
//  TJSONArray: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TJSONArray::Reset()
{
    // Call our parent and that's all we need for now
    TJSONCont::Reset();
}





// ---------------------------------------------------------------------------
//   CLASS: TJSONObject
//  PREFIX: jprsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONObject: Constructors and Destructor
// ---------------------------------------------------------------------------

TJSONObject::TJSONObject(const TString& strName) :

    TJSONCont(tCIDNet::EJSONVTypes::Object, strName)
{
}

TJSONObject::~TJSONObject()
{
}


// ---------------------------------------------------------------------------
//  TJSONObject: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TJSONObject::Reset()
{
    // Call our parent, which is all we need for now
    TJSONCont::Reset();
}







// ---------------------------------------------------------------------------
//   CLASS: TJSONParser
//  PREFIX: emsg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJSONParser: Public, static methods
// ---------------------------------------------------------------------------

//
//  Handles formatting out JSON content. You can just call the format method on the
//  JSON object itself, but this insures we get the outer {} braces and can flush the
//  stream before return and reset it before we start, since we have a known starting
//  point for the recursive operation.
//
tCIDLib::TVoid
TJSONParser::FormatJSON(const   TJSONValue&         jprsnStart
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TCard4     c4IndentSize
                        , const tCIDLib::TBoolean   bResetFirst)
{
    // Reset the stream first if asked
    if (bResetFirst)
        strmTar.Reset();

    //
    //  Do the recursive operation, starting at indent level 0. If the indent size
    //  is zero, the indent levels will be ignored anyway.
    //
    jprsnStart.FormatToText(strmTar, 0, c4IndentSize);

    // And flush it at the end, with a trailing new line
    strmTar << kCIDLib::EndLn;
}


tCIDLib::TVoid
TJSONParser::OutputArrayHdr(const TString& strName, TTextOutStream& strmTar)
{
    strmTar << L"\"" << strName << L"\" : [";
}


//
//  To make outputing JSON text from raw data less wordy. The bulk of it ends up being
//  the outputting of individual values.
//
tCIDLib::TVoid
TJSONParser::OutputBool(const   TString&            strName
                        , const tCIDLib::TBoolean   bValue
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : ";
    if (bValue)
        strmTar << L"true";
    else
        strmTar << L"false";

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputCard(const   TString&            strName
                        , const tCIDLib::TCard4     c4Value
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : "
            << c4Value;

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputFloat(const  TString&            strName
                        , const tCIDLib::TFloat8    f8Value
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : "
            << f8Value;

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputInt( const   TString&            strName
                        , const tCIDLib::TInt4      i4Value
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : "
            << i4Value;

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputKVPs(const   TString&                strName
                        , const tCIDLib::TKVPCollect&   colList
                        ,       TTextOutStream&         strmTar
                        , const tCIDLib::TBoolean       bLastVal)
{
    TColCursor<TKeyValuePair>* pcursList = colList.pcursNew();
    TJanitor<TColCursor<TKeyValuePair> > janCursor(pcursList);
    if (pcursList->bReset())
    {
        do
        {
            const TKeyValuePair& kvalCur = pcursList->objRCur();
            strmTar << L"\"" << kvalCur.strKey() << L"\" : \""
                    << kvalCur.strValue()
                    << L"\"";

            if (pcursList->bIsValid())
                strmTar << kCIDLib::chComma;
            strmTar << kCIDLib::NewLn;

        }   while (pcursList->bNext());
    }
}


tCIDLib::TVoid
TJSONParser::OutputObjHdr(const TString& strName, TTextOutStream& strmTar)
{
    strmTar << L"\"" << strName << L"\" : {\n\n";
}


//
//  This one has to deal with the fact that certain characters must be escaped in
//  JSON. So we have to do a loop and output the characters one at a time, doing
//  escapes as needed.
//
tCIDLib::TVoid
TJSONParser::OutputStr( const   TString&            strName
                        , const TString&            strValue
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : \"";

    // Call a helper since we have to do this below for string arrays as well
    TJSONParser::EscapeStr(strmTar, strValue.pszBuffer());
    strmTar << L"\"";

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputStrArray(const   TString&                strName
                            , const tCIDLib::TStrCollect&   colList
                            ,       TTextOutStream&         strmTar
                            , const tCIDLib::TBoolean       bLastVal)
{
    TColCursor<TString>* pcursList = colList.pcursNew();
    TJanitor<TColCursor<TString> > janCursor(pcursList);

    strmTar << L"[";
    if (pcursList->bReset())
    {
        tCIDLib::TCard4 c4Cnt = 0;
        do
        {
            // We have to deal with escapes in text

            strmTar << L"\"";
            TJSONParser::EscapeStr(strmTar, pcursList->objRCur().pszBuffer());
            strmTar << L"\"";

            if (pcursList->bIsValid())
                strmTar << kCIDLib::chComma;
            strmTar << kCIDLib::NewLn;

        }   while (pcursList->bNext());
    }

    strmTar << L"]";

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}

tCIDLib::TVoid
TJSONParser::OutputText(const   TString&            strName
                        , const tCIDLib::TCh* const pszValue
                        ,       TTextOutStream&     strmTar
                        , const tCIDLib::TBoolean   bLastVal)
{
    strmTar << L"\"" << strName << L"\" : \"";
    TJSONParser::EscapeStr(strmTar, pszValue);
    strmTar << L"\"";

    if (!bLastVal)
        strmTar << kCIDLib::chComma;
    strmTar << kCIDLib::NewLn;
}



// ---------------------------------------------------------------------------
//  TJSONParser: Constructors and Destructor
// ---------------------------------------------------------------------------
TJSONParser::TJSONParser() :

    m_chPushback(kCIDLib::chNull)
{
}

TJSONParser::~TJSONParser()
{
}



// ---------------------------------------------------------------------------
//  TJSONParser: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We get an object node as the root, since ultimately everything has to be
//  in a top level object.
//
TJSONValue* TJSONParser::pjprsnParse(TTextInStream& strmSrc)
{
    // Reset the stream to the start
    strmSrc.Reset();

    // Reset any per-parse round stuff
    m_c4Col = 1;
    m_c4Line = 1;
    m_chPushback = kCIDLib::chNull;


    TJSONValue* pjprsnRoot = pjprsnParseValue(strmSrc, 0, TString::strEmpty());
    TJanitor<TJSONValue> janRoot(pjprsnRoot);

    // And there should not be anything else but whitespace
    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCh chCur = chGetNext(strmSrc);
        if (!TRawStr::bIsSpace(chCur))
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcJSON_TrailingData
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
    }

    // It worked, so hand the root node back. Orphan it out of the janitor
    return janRoot.pobjOrphan();
}


// ---------------------------------------------------------------------------
//  TJSONParser: Private, static methods
// ---------------------------------------------------------------------------

//
//  JSON requires that certain characters be escaped. So this is called by the
//  helper methods we provide for outputing string content.
//
tCIDLib::TVoid
TJSONParser::EscapeStr(TTextOutStream& strmTar, const tCIDLib::TCh* const pszText)
{
    const tCIDLib::TCh* pszCur = pszText;
    while (*pszCur)
    {
        const tCIDLib::TCh chCur = *pszCur++;

        switch(chCur)
        {
            // Output an escape and then the character
            case kCIDLib::chBackSlash :
            case kCIDLib::chForwardSlash :
            case kCIDLib::chQuotation :
                strmTar << kCIDLib::chBackSlash << chCur;
                break;

            //
            //  These require replacing the actual character. We return any new line combo
            //  into \r\n
            //
            case kCIDLib::chBS :
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_b;
                break;

            case kCIDLib::chCR :
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_r;

                //
                //  If the next char is an LF, eat it. So, if we see an LF below, we know
                //  it's a bare one. Either way, put out a \n as well.
                //
                if (*pszCur == kCIDLib::chLF)
                    pszCur++;
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_n;
                break;

            case kCIDLib::chFF :
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_f;
                break;

            case kCIDLib::chLF :
                // We know this is a bare LF, so convert to CR/LF
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_r
                        << kCIDLib::chBackSlash << kCIDLib::chLatin_n;
                break;

            case kCIDLib::chTab :
                strmTar << kCIDLib::chBackSlash << kCIDLib::chLatin_t;
                break;

            default :
                // Nothing special, just output it
                strmTar << chCur;
                break;
        };
    }
}


// ---------------------------------------------------------------------------
//  TJSONParser: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Tests to see if the next non-white space is a specific close character,
//  array or object. If so returns true, else ungets the character and returns
//  false.
//
tCIDLib::TBoolean
TJSONParser::bTestClose(        TTextInStream&  strmSrc
                        , const tCIDLib::TCard4 c4Line
                        , const ETokens         eExpType)
{
    // If we hit end of stream, we'll return false
    tCIDLib::TBoolean bRet = kCIDLib::False;

    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCh chToFill = chGetNext(strmSrc);

        if (!TRawStr::bIsSpace(chToFill))
        {
            //
            //  We got something. So it's either one of the character we
            //  want, or push it back and return false. Either way, we break
            //  out now.
            //
            if (((chToFill == kCIDLib::chCloseBrace) && (eExpType == ETokens::CloseObj))
            ||  ((chToFill == kCIDLib::chCloseBracket) && (eExpType == ETokens::CloseArray)))
            {
                bRet = kCIDLib::True;
            }
             else
            {
                m_chPushback = chToFill;
            }
            break;
        }
    }
    return bRet;
}


//
//  Tests to see if the next non-white space is an open character. If so
//  it returns true and the token type for the type found. Else false and
//  it ungets the character.
//
tCIDLib::TBoolean
TJSONParser::bTestOpen(         TTextInStream&  strmSrc
                        , const tCIDLib::TCard4 c4Line
                        ,       ETokens&        eTokFound)
{
    // If we hit end of stream, we'll return false
    tCIDLib::TBoolean bRet = kCIDLib::False;

    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCh chToFill = chGetNext(strmSrc);

        if (!TRawStr::bIsSpace(chToFill))
        {
            //
            //  If we found an open, return true and give back the token
            //  type. Else push back. Break out either way.
            //
            if (chToFill == kCIDLib::chOpenBrace)
            {
                eTokFound = ETokens::OpenObj;
                bRet = kCIDLib::True;
            }
             else if (chToFill == kCIDLib::chCloseBracket)
            {
                eTokFound = ETokens::OpenArray;
                bRet = kCIDLib::True;
            }
             else
            {
                m_chPushback = chToFill;
            }
            break;
        }
    }
    return bRet;
}


//
//  The low level character spooler. We keep up with the line/col info here,
//  so that we don't have to do it in multiple places below.
//
//  Note that we support a single pushback character. We check it first and
//  return it if we have one. Newlines or whitspace are never pushed back,
//  only actual content characters.
//
tCIDLib::TCh TJSONParser::chGetNext(TTextInStream& strmSrc)
{
    tCIDLib::TCh chRet;
    if (m_chPushback != kCIDLib::chNull)
    {
        chRet = m_chPushback;
        m_chPushback = kCIDLib::chNull;
    }
     else
    {
        chRet = strmSrc.chRead();
    }

    m_c4Col++;
    if (chRet == L'\n')
    {
        m_c4Col = 1;
        m_c4Line++;
    }

    return chRet;
}


//
//  Pull the next token from the input stream. End of stream is only valid
//  after the last closing brace is seen and that is checked for at that point,
//  so it's never valid here and we just let the stream throw if we hit it.
//
TJSONParser::ETokens
TJSONParser::eGetNextToken(TString& strToFill, TTextInStream& strmSrc)
{
    tCIDLib::TCh chCur;
    strToFill.Clear();

    // Skip over any whitespace
    while (kCIDLib::True)
    {
        chCur = chGetNext(strmSrc);
        if (!TRawStr::bIsSpace(chCur))
        {
            strToFill.Append(chCur);
            break;
        }
    }

    // Look at the first character and decide what to do
    if (chCur == kCIDLib::chColon)
        return ETokens::Colon;
     else if (chCur == kCIDLib::chComma)
        return ETokens::Comma;
     else if (chCur == kCIDLib::chOpenBrace)
        return ETokens::OpenObj;
     else if (chCur == kCIDLib::chCloseBrace)
        return ETokens::CloseObj;
     else if (chCur == kCIDLib::chOpenBracket)
        return ETokens::OpenArray;
     else if (chCur == kCIDLib::chCloseBracket)
        return ETokens::CloseArray;

    //
    //  If a quote, then it's a quoted string. Else, it has to be a number
    //  or boolean value., so we'll read up to the next special character
    //  or white space. In either case we return that it's a value token.
    //  In the non-quoted case, we don't bother to figure out here whether
    //  it's a boolean or number.
    //
    if (chCur == kCIDLib::chQuotation)
    {
        //
        //  It's a quoted value, so read up to the next quote. Get rid of the
        //  quote we put into the string above. Deal with the possibility of
        //  escaped embedded quotes.
        //
        strToFill.Clear();
        while (kCIDLib::True)
        {
            chCur = chGetNext(strmSrc);

            // And unescaped quote, and we are done
            if (chCur == kCIDLib::chQuotation)
                break;

            // Check for an escaped character
            if (chCur == kCIDLib::chBackSlash)
            {
                chCur = chGetNext(strmSrc);

                switch(chCur)
                {
                    case L'b' :
                        chCur = 0x8;
                        break;

                    case L'f' :
                        chCur = 0xC;
                        break;

                    case L'n' :
                        chCur = 0xA;
                        break;

                    case L'r' :
                        chCur = 0xD;
                        break;

                    case L't' :
                        chCur = 0x9;
                        break;

                    case L'v' :
                        chCur = 0xB;
                        break;

                    case L'\'' :
                    case L'"' :
                    case L'\\' :
                        // Just keep the character
                        break;

                    default :
                        // An illegal escape, just take the new character
                        break;
                };
            }
            strToFill.Append(chCur);
        }

        return ETokens::String;
    }

    // It's one fo the simple values, null, true/false, number
    while(kCIDLib::True)
    {
        chCur = chGetNext(strmSrc);
        if ((chCur == kCIDLib::chColon)
        ||  (chCur == kCIDLib::chComma)
        ||  (chCur == kCIDLib::chOpenBrace)
        ||  (chCur == kCIDLib::chCloseBrace)
        ||  (chCur == kCIDLib::chOpenBracket)
        ||  (chCur == kCIDLib::chCloseBracket))
        {
            // Push it back and break out
            m_chPushback = chCur;
            break;
        }

        // Just break out, throwing any white space away
        if (TRawStr::bIsSpace(chCur))
            break;

        // Keep it and keep going
        strToFill.Append(chCur);
    }

    if (strToFill == L"true")
        return ETokens::True;
    else if (strToFill == L"false")
        return ETokens::False;
    else if (strToFill == L"null")
        return ETokens::Null;

    // Has to be a number, so indicate that
    return ETokens::Number;
}


//
//  Parse a value and return a value node that contains it. This can be
//  called from either parsing an array or an object. If the value we find
//  is an array or object, we will recurse in order to create that value.
//
//  If we do find an array or object, it's returned within a value node
//  just as a simple value would be, so that all values are value nodes.
//
TJSONValue*
TJSONParser::pjprsnParseValue(          TTextInStream&  strmSrc
                                , const tCIDLib::TCard4 c4NestingLevel
                                , const TString&        strName)
{
    TString strVal;
    const ETokens eCurToken = eGetNextToken(strVal, strmSrc);

    //
    //  Probe for an open array or object token. If so, do that. Else it
    //  has to be a basic value type.
    //
    TJSONValue* pjprsnRet = nullptr;
    if (eCurToken == ETokens::OpenArray)
    {
        TJSONArray* pjpsrnNew = new TJSONArray(strName);
        TJanitor<TJSONArray> janArray(pjpsrnNew);

        ParseArray(*pjpsrnNew, strmSrc, c4NestingLevel + 1);

        // It worked, so return it, in a value node
        pjprsnRet = janArray.pobjOrphan();
    }
     else if (eCurToken == ETokens::OpenObj)
    {
        TJSONObject* pjpsrnNew = new TJSONObject(strName);
        TJanitor<TJSONObject> janObject(pjpsrnNew);

        ParseObject(*pjpsrnNew, strmSrc, c4NestingLevel + 1);

        // It worked, so return it, in a value node
        pjprsnRet = janObject.pobjOrphan();
    }
     else
    {
        // Map the token to a simple value type
        tCIDNet::EJSONVTypes eType = tCIDNet::EJSONVTypes::Null;
        switch(eCurToken)
        {
            case ETokens::False :
                eType = tCIDNet::EJSONVTypes::False;
                break;

            case ETokens::Null :
                eType = tCIDNet::EJSONVTypes::Null;
                strVal.Clear();
                break;

            case ETokens::Number :
                eType = tCIDNet::EJSONVTypes::Number;
                break;

            case ETokens::String :
                eType = tCIDNet::EJSONVTypes::String;
                break;

            case ETokens::True :
                eType = tCIDNet::EJSONVTypes::True;
                break;

            default :
                ThrowExpected(L"simple JSON value", strVal, CID_LINE);
                break;
        };
        pjprsnRet = new TJSONSimpleVal(strVal, eType, strName);
    }

    // If we couldn't create a node, that's bad at this point
    if (!pjprsnRet)
        ThrowExpected(L"object, array, or JSON value", strVal, CID_LINE);

    return pjprsnRet;
}


//
//  Given a parent array and a stream that is just after the opening bracket
//  of the new array. We will parse out a list of unnamed values and add
//  them to the array. If we hit another array or object, we recurse on that
//  node.
//
tCIDLib::TVoid
TJSONParser::ParseArray(        TJSONArray&     jprsnCont
                        ,       TTextInStream&  strmSrc
                        , const tCIDLib::TCard4 c4NestingLevel)
{
    //
    //  It's possible it's an empty array, in which case we'll immediately
    //  see a close array, so test that now. It makes the code below a lot
    //  simpler.
    //
    if (bTestClose(strmSrc, CID_LINE, ETokens::CloseArray))
        return;

    //
    //  The first thing we need to see is the opening brace. It will either
    //  be a nested element or an array.
    //
    TString strToken;
    while (kCIDLib::True)
    {
        //
        //  Get a value node, which may end up recursing into here again
        //  directly or indirectly. We have to pass in the current nesting
        //  level it case it recurses. Array items have no name, so we
        //  pass an empty string.
        //
        TJSONValue* pjprsnValue = pjprsnParseValue
        (
            strmSrc, c4NestingLevel, TString::strEmpty()
        );
        jprsnCont.AdoptValue(pjprsnValue);

        //
        //  And it should end with either a comma, which means we have more
        //  to do, or the close of the array, which case we are done.
        //
        ETokens eCurToken = eGetNextToken(strToken, strmSrc);
        if (eCurToken == ETokens::CloseArray)
            break;

        if (eCurToken != ETokens::Comma)
            ThrowExpected(L"comma", strToken, CID_LINE);
    }
}


//
//  Given a parent object and a stream that is just after the opening brace
//  of the new element. So we are now expecting child elements, child arrays,
//  and key/value pairs.
//
tCIDLib::TVoid
TJSONParser::ParseObject(           TJSONObject&    jprsnParent
                            ,       TTextInStream&  strmSrc
                            , const tCIDLib::TCard4 c4NestingLevel)
{
    //
    //  It's possible it's an empty object, in which case we'll immediately
    //  see a close object, so test that now. It makes the code below a lot
    //  simpler.
    //
    if (bTestClose(strmSrc, CID_LINE, ETokens::CloseObj))
        return;

    //
    //  The first thing we need to see is the opening brace. It will either be
    //  a nested element or an array.
    //
    TString strToken;
    while (kCIDLib::True)
    {
        //
        //  Get the next token. At this point it has to be a name of an
        //  attribute, which will be a string token.
        //
        ETokens eCurToken = eGetNextToken(strToken, strmSrc);
        if (eCurToken != ETokens::String)
            ThrowExpected(L"attribute or element name", strToken, CID_LINE);

        // Next we should see a colon
        TestColon(strmSrc, CID_LINE);

        //
        //  Now we need to get a value node, which may end up recursing
        //  into here again directly or indirectly. We have to pass in the
        //  current nesting level it case it recurses. the name of the new
        //  value is the token we just got above.
        //
        TJSONValue* pjprsnValue = pjprsnParseValue
        (
            strmSrc, c4NestingLevel, strToken
        );

        // Add this new value to the parent object
        jprsnParent.AdoptValue(pjprsnValue);

        // We now have to see either an end of object or comma for another one
        eCurToken = eGetNextToken(strToken, strmSrc);
        if (eCurToken == ETokens::CloseObj)
            break;

        if (eCurToken != ETokens::Comma)
            ThrowExpected(L"comma", strToken, CID_LINE);
    }
}


//
//  Some helpers that will check that the next token in the source is this or that
//  and throw if not.
//
tCIDLib::TVoid
TJSONParser::TestColon(TTextInStream& strmSrc, const tCIDLib::TCard4 c4Line)
{
    if (eGetNextToken(m_strTmpTest, strmSrc) != ETokens::Colon)
        ThrowExpected(L"colon", m_strTmpTest, c4Line);
}

tCIDLib::TVoid
TJSONParser::TestOpenElem(TTextInStream& strmSrc, const tCIDLib::TCard4 c4Line)
{
    if (eGetNextToken(m_strTmpTest, strmSrc) != ETokens::OpenObj)
        ThrowExpected(L"open element", m_strTmpTest, c4Line);
}


// A helper that throws the expected to see this but got that exception
tCIDLib::TVoid
TJSONParser::ThrowExpected( const   tCIDLib::TCh* const pszExpected
                            , const TString&            strGot
                            , const tCIDLib::TCard4     c4Line)
{
    //
    //  If the incoming is badly formatted, we might end up with a leading or trailing
    //  quote on the two values, so strip those out or we will create an invalid
    //  response in return.
    //
    const tCIDLib::TCh* const pszStrip = L"\"'";
    TString strRealExp(pszExpected);
    TString strRealGot(strGot);
    strRealExp.Strip(pszStrip, tCIDLib::EStripModes::Full);

    facCIDNet().ThrowErr
    (
        CID_FILE
        , c4Line
        , kNetErrs::errcJSON_BadFormat
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , strRealExp
        , strRealGot
        , TCardinal(m_c4Line)
        , TCardinal(m_c4Col)
    );
}


