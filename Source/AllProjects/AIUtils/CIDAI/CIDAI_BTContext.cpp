//
// FILE NAME: CIDAI_BTContext.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the context related classes for our behavior trees.
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
#include    "CIDAI_.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TAIBTCtxVar
//  PREFIX: ctxv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTCtxVar: Public, static methods
// ---------------------------------------------------------------------------
const TString& TAIBTCtxVar::strVarKey(const TAIBTCtxVar& ctxvSrc)
{
    return ctxvSrc.m_strKey;
}


// ---------------------------------------------------------------------------
//  TAIBTCtxVar: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBTCtxVar::TAIBTCtxVar() :

    m_colMembers(109, new TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey)
{
}

TAIBTCtxVar::TAIBTCtxVar(const  TString& strKey) :

    m_colMembers(109, new TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey)
    , m_strKey(strKey)
{
}

TAIBTCtxVar::TAIBTCtxVar(const TAIBTCtxVar& ctxvSrc) :

    m_colMembers(ctxvSrc.m_colMembers)
    , m_strKey(ctxvSrc.m_strKey)
{
}


TAIBTCtxVar::~TAIBTCtxVar()
{
}


// ---------------------------------------------------------------------------
//  TAIBTCtxVar: Public operators
// ---------------------------------------------------------------------------
TAIBTCtxVar& TAIBTCtxVar::operator=(const TAIBTCtxVar& ctxvSrc)
{
    if (this != &ctxvSrc)
    {
        m_colMembers = ctxvSrc.m_colMembers;
        m_strKey = ctxvSrc.m_strKey;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TAIBTCtxVar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get this variable as a boolean
tCIDLib::TBoolean TAIBTCtxVar::bValAs(const TString& strValKey) const
{
    const TKeyValuePair& kvalVal = *pkvalFindVal(strValKey, kCIDLib::True);

    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (kvalVal.strValue() == kCIDAI::strVal_False)
        bRet = kCIDLib::False;
    else if (kvalVal.strValue() == kCIDAI::strVal_True)
        bRet = kCIDLib::True;
    else
    {
        TypeCastErr(strValKey, L"Bool");
    }
    return bRet;
}


// Get this variable as a Card4
tCIDLib::TCard4 TAIBTCtxVar::c4ValAs(const TString& strValKey) const
{
    const TKeyValuePair& kvalVal = *pkvalFindVal(strValKey, kCIDLib::True);

    tCIDLib::TCard4 c4Ret;
    if (!kvalVal.strValue().bToCard4(c4Ret, tCIDLib::ERadices::Auto))
        TypeCastErr(strValKey, L"Card4");

    return c4Ret;
}


// Get this variable as an Int4
tCIDLib::TInt4 TAIBTCtxVar::i4ValAs(const TString& strValKey) const
{
    const TKeyValuePair& kvalVal = *pkvalFindVal(strValKey, kCIDLib::True);

    tCIDLib::TInt4 i4Ret;
    if (!kvalVal.strValue().bToInt4(i4Ret, tCIDLib::ERadices::Auto))
        TypeCastErr(strValKey, L"Int4");

    return i4Ret;
}


// Returns a ref to our variable key
const TString& TAIBTCtxVar::strVarKey() const
{
    return m_strKey;
}


// Get a value as a string
const TString& TAIBTCtxVar::strVal(const TString& strValKey) const
{
    return pkvalFindVal(strValKey, kCIDLib::True)->strValue();
}


// Set the value as various types
tCIDLib::TVoid
TAIBTCtxVar::SetVal(const TString& strValKey, const tCIDLib::TBoolean bToSet)
{
    TKeyValuePair* pkvalVal = pkvalFindVal(strValKey, kCIDLib::False);
    if (!pkvalVal)
        pkvalVal = &m_colMembers.objAdd(TKeyValuePair(strValKey, TString::strEmpty()));

    pkvalVal->strValue(bToSet ? kCIDAI::strVal_True : kCIDAI::strVal_False);
}

tCIDLib::TVoid
TAIBTCtxVar::SetVal(const TString& strValKey, const tCIDLib::TCard4 c4ToSet)
{
    TKeyValuePair* pkvalVal = pkvalFindVal(strValKey, kCIDLib::False);
    if (!pkvalVal)
        pkvalVal = &m_colMembers.objAdd(TKeyValuePair(strValKey, TString::strEmpty()));

    pkvalVal->strValue().SetFormatted(c4ToSet);
}

tCIDLib::TVoid
TAIBTCtxVar::SetVal(const TString& strValKey, const tCIDLib::TInt4 i4ToSet)
{
    TKeyValuePair* pkvalVal = pkvalFindVal(strValKey, kCIDLib::False);
    if (!pkvalVal)
        pkvalVal = &m_colMembers.objAdd(TKeyValuePair(strValKey, TString::strEmpty()));

    pkvalVal->strValue().SetFormatted(i4ToSet);
}

tCIDLib::TVoid
TAIBTCtxVar::SetVal(const TString& strValKey, const TString& strToSet)
{
    TKeyValuePair* pkvalVal = pkvalFindVal(strValKey, kCIDLib::False);
    if (pkvalVal)
        pkvalVal->strValue(strToSet);
    else
        m_colMembers.objAdd(TKeyValuePair(strValKey, strToSet));
}


// ---------------------------------------------------------------------------
//  TAIBTCtxVar: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Look for a value with the indicated key. We throw if not found
const TKeyValuePair*
TAIBTCtxVar::pkvalFindVal(const TString& strValKey, const tCIDLib::TBoolean bThrowIfNot) const
{
    return const_cast<TAIBTCtxVar&>(*this).pkvalFindVal(strValKey, bThrowIfNot);
}

TKeyValuePair*
TAIBTCtxVar::pkvalFindVal(const TString& strValKey, const tCIDLib::TBoolean bThrowIfNot)
{
    TKeyValuePair* pkvapRet = nullptr;
    try
    {
        pkvapRet = m_colMembers.pobjFindByKey(strValKey);
    }

    catch(...)
    {
    }

    if (!pkvapRet && bThrowIfNot)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcCtxV_VarMember
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strKey
            , strValKey
        );
    }
    return pkvapRet;
}


tCIDLib::TVoid
TAIBTCtxVar::TypeCastErr(const TString& strValKey, const tCIDLib::TCh* const pszType) const
{
    facCIDAI().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAIErrs::errcCtxV_VarAsType
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::TypeMatch
        , m_strKey
        , strValKey
        , TString(pszType)
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TAIBTCtx
//  PREFIX: ctx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTCtx: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBTCtx::TAIBTCtx() :

    m_colVars(109, new TStringKeyOps(kCIDLib::False), &TAIBTCtxVar::strVarKey)
{
}

TAIBTCtx::~TAIBTCtx()
{
}


// ---------------------------------------------------------------------------
//  TAIBTCtx: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper to find a value in the key/numeric list collection and return the values
//  associated with it.
//
tCIDLib::TBoolean
TAIBTCtx::bFindKNumListVal( const   TString&            strKey
                            ,       tCIDLib::TFloat4&   f4Value)
{
    const tCIDLib::TCard4 c4Count = m_colKNumList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colKNumList[c4Index];
        if (knumCur.strKey().bCompareI(strKey))
        {
            f4Value = knumCur.f4Value();
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean
TAIBTCtx::bFindKNumListVal(const TString& strKey, TString& strToFill)

{
    const tCIDLib::TCard4 c4Count = m_colKNumList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colKNumList[c4Index];
        if (knumCur.strKey().bCompareI(strKey))
        {
            strToFill = knumCur.strValue();
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}



//
//  A generic parameter query for those scenarios where the caller doesn't know what
//  type the value is, and where it might not exist.
//
tCIDLib::TBoolean
TAIBTCtx::bQueryValue(  const   TString&    strVarKey
                        , const TString&    strValKey
                        ,       TString&    strToFill) const
{
    const TAIBTCtxVar* pctxvSrc = m_colVars.pobjFindByKey(strVarKey);
    if (!pctxvSrc)
        return kCIDLib::False;

    strToFill = pctxvSrc->strVal(strValKey);
    return kCIDLib::True;
}


// A check to see if a variable exists at all
tCIDLib::TBoolean TAIBTCtx::bVarExists(const TString& strVarKey) const
{
    return (pctxvCheckVarExists(strVarKey) != nullptr);
}


//
//  Convenience methods to get the values of variables we hold as various binary
//  types.
//
tCIDLib::TBoolean
TAIBTCtx::bVarValue(const   TString&    strVarKey
                    , const TString&    strValKey) const
{
    const TAIBTCtxVar* pctxvGet = pctxvCheckVarExists(strVarKey);
    return pctxvGet->bValAs(strValKey);
}

tCIDLib::TCard4
TAIBTCtx::c4VarValue(const TString& strVarKey, const TString& strValKey) const
{
    const TAIBTCtxVar* pctxvGet = pctxvCheckVarExists(strVarKey);
    return pctxvGet->c4ValAs(strValKey);
}

tCIDLib::TInt4
TAIBTCtx::i4VarValue(const TString& strVarKey, const TString& strValKey) const
{
    const TAIBTCtxVar* pctxvGet = pctxvCheckVarExists(strVarKey);
    return pctxvGet->i4ValAs(strValKey);
}


// Let the client code access the application use collection
const tCIDLib::TKNumList& TAIBTCtx::colKNumList() const
{
    return m_colKNumList;
}


//
//  Resets us back to initial state, optinoally with any initial variables the app
//  wants to have from the start.
//
tCIDLib::TVoid TAIBTCtx::Reset()
{
    // Clear up our variables list and the application lists
    m_colKNumList.RemoveAll();
    m_colVars.RemoveAll();
}


tCIDLib::TVoid TAIBTCtx::Reset(const tCIDLib::TKValsList& colInitVars)
{
    // Call the other version
    Reset();

    //
    //  And set any initial variables. The key is the variable name, value 1 is the
    //  variable value name, and value 2 is the value to set. We have to set these
    //  as variables on ourself.
    //
    const tCIDLib::TCard4 c4Count = colInitVars.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValues& kvalsCur = colInitVars[c4Index];
        SetStringVar(kvalsCur.strKey(), kvalsCur.strVal1(), kvalsCur.strVal2());
    }

}

tCIDLib::TVoid
TAIBTCtx::Reset(const   tCIDLib::TKValsList&    colInitVars
                , const tCIDLib::TKNumList&     colKNumList)
{
    // Call the other version
    Reset(colInitVars);

    // And copy over the application info
    m_colKNumList = colKNumList;
}


// Find a value in the application key/num list or throw if not found
const TString&
TAIBTCtx::strFindKNumListVal(const  TString&            strKey
                            ,       tCIDLib::TFloat4&   f4Value)
{
    const tCIDLib::TCard4 c4Count = m_colKNumList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colKNumList[c4Index];
        if (knumCur.strKey().bCompareI(strKey))
        {
            f4Value = knumCur.f4Value();
            break;
        }
    }

    if (c4Index == c4Count)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcCtx_AppValNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKey
        );
    }
    return m_colKNumList[c4Index].strValue();
}


const TString& TAIBTCtx::strFindKNumListVal(const TString& strKey)
{
    const tCIDLib::TCard4 c4Count = m_colKNumList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colKNumList[c4Index];
        if (knumCur.strKey().bCompareI(strKey))
            break;
    }

    if (c4Index == c4Count)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcCtx_AppValNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKey
        );
    }
    return m_colKNumList[c4Index].strValue();
}


// Get a variable value as a string
const TString&
TAIBTCtx::strVarValue(const TString& strVarKey, const TString& strValKey) const
{
    const TAIBTCtxVar* pctxvGet = pctxvCheckVarExists(strVarKey);
    return pctxvGet->strVal(strValKey);
}


//
//  Just updates the application info, which is useful for nested operations. It doesn't
//  clear the stack or the variables.
//
tCIDLib::TVoid
TAIBTCtx::SetAppInfo(const  tCIDLib::TKNumList& colKNumList)
{
    m_colKNumList = colKNumList;
}

//
//  Set variables, create if not already set
//
tCIDLib::TVoid
TAIBTCtx::SetBoolVar(const  TString&            strVarKey
                    , const TString&            strValKey
                    , const tCIDLib::TBoolean   bToSet)
{
    TAIBTCtxVar* pctxvSet = m_colVars.pobjFindByKey(strVarKey);
    if (!pctxvSet)
        pctxvSet = &m_colVars.objAdd(TAIBTCtxVar(strVarKey));

    pctxvSet->SetVal(strValKey, bToSet);
}

tCIDLib::TVoid
TAIBTCtx::SetCardVar(const  TString&        strVarKey
                    , const TString&        strValKey
                    , const tCIDLib::TCard4 c4ToSet)
{
    TAIBTCtxVar* pctxvSet = m_colVars.pobjFindByKey(strVarKey);
    if (!pctxvSet)
        pctxvSet = &m_colVars.objAdd(TAIBTCtxVar(strVarKey));

    pctxvSet->SetVal(strValKey, c4ToSet);
}

tCIDLib::TVoid
TAIBTCtx::SetIntVar(const   TString&        strVarKey
                    , const TString&        strValKey
                    , const tCIDLib::TInt4  i4ToSet)
{
    TAIBTCtxVar* pctxvSet = m_colVars.pobjFindByKey(strVarKey);
    if (!pctxvSet)
        pctxvSet = &m_colVars.objAdd(TAIBTCtxVar(strVarKey));

    pctxvSet->SetVal(strValKey, i4ToSet);
}

tCIDLib::TVoid
TAIBTCtx::SetStringVar( const   TString&    strVarKey
                        , const TString&    strValKey
                        , const TString&    strToSet)
{
    TAIBTCtxVar* pctxvSet = m_colVars.pobjFindByKey(strVarKey);
    if (!pctxvSet)
        pctxvSet = &m_colVars.objAdd(TAIBTCtxVar(strVarKey));

    pctxvSet->SetVal(strValKey, strToSet);
}


// ---------------------------------------------------------------------------
//  TAIBTCtx: Prviate, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Try to find the indicated variable, optionally throwing if not found.
//
const TAIBTCtxVar*
TAIBTCtx::pctxvCheckVarExists(const TString& strKeyToFind) const
{
    const TAIBTCtxVar* pctxvRet = m_colVars.pobjFindByKey(strKeyToFind);
    if (!pctxvRet)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcCtxV_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKeyToFind
        );
    }
    return pctxvRet;
}
