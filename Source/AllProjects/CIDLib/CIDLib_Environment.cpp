//
// FILE NAME: CIDLib_Environment.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/15/1997
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
//  TProcEnvironment and TEnvironment are mechanism for manipulating the
//  process wide environment and copies of it.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TProcEnvLocker,TObject)
RTTIDecls(TEnvironment,TObject)



namespace CIDLib_Environment
{
    // -----------------------------------------------------------------------
    //  Local, const data
    //
    //  c4Modulus
    //      This is the modulus used in the hashing of the key strings.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4    c4Modulus = 23;
}


// ---------------------------------------------------------------------------
//  Static functions
// ---------------------------------------------------------------------------
static const TString& strGetKey(const TKeyValuePair& elemData)
{
    return elemData.strKey();
}


// ---------------------------------------------------------------------------
//  NAMESPACE: TProcEnvironment
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TProcEnvironment::Add(const TString& strKey, const TString& strNewValue)
{
    if (!TKrnlEnvironment::bAdd(strKey.pszBuffer(), strNewValue.pszBuffer()))
    {
        tCIDLib::TErrCode errcToThrow = kCIDErrs::errcEnv_AddError;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() == kKrnlErrs::errcGen_AlreadyExists)
        {
            errcToThrow = kCIDErrs::errcEnv_KeyAlreadyExists;
            eClass = tCIDLib::EErrClasses::Already;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
            , strKey
        );
    }
}


tCIDLib::TBoolean
TProcEnvironment::bAddOrUpdate(const TString& strKey, const TString& strNewValue)
{
    tCIDLib::TBoolean   bAdded;
    if (!TKrnlEnvironment::bAddOrUpdate(strKey.pszBuffer()
                                        , strNewValue.pszBuffer()
                                        , bAdded))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEnv_AddOrUpdateError
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strKey
        );
    }
    return bAdded;
}


tCIDLib::TVoid
TProcEnvironment::AddToExePath( const   TString&            strNewValue
                                , const tCIDLib::EStartEnd  eWhere)
{
    if (!TKrnlEnvironment::bAddToExePath(strNewValue.pszBuffer(), eWhere))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEnv_UpdateExePath
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid
TProcEnvironment::AddToLibPath( const   TString&            strNewValue
                                , const tCIDLib::EStartEnd  eWhere)
{
    if (!TKrnlEnvironment::bAddToLibPath(strNewValue.pszBuffer(), eWhere))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEnv_UpdateLibPath
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TBoolean
TProcEnvironment::bFind(const TString& strKey, TString& strToFill)
{
    TProcEnvLocker lockEnv;

    const tCIDLib::TCh* pszValue = TKrnlEnvironment::pszFind(strKey.pszBuffer());
    if (!pszValue)
        return kCIDLib::False;

    strToFill = pszValue;
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindExePath(TString& strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 8192;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlEnvironment::bFindExePath(achBuf, c4BufSz))
        return kCIDLib::False;

    strToFill = achBuf;
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindLibPath(TString& strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 8192;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlEnvironment::bFindLibPath(achBuf, c4BufSz))
        return kCIDLib::False;

    strToFill = achBuf;
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindTempPath(TString& strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 8192;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    if (!TKrnlEnvironment::bFindTempPath(achBuf, c4BufSz))
        return kCIDLib::False;

    strToFill = achBuf;
    return kCIDLib::True;
}


tCIDLib::TCard4 TProcEnvironment::c4Entries()
{
    return TKrnlEnvironment::c4Entries();
}


tCIDLib::TCard4 TProcEnvironment::c4CharsInValue(const TString& strKey)
{
    tCIDLib::TCard4 c4Chars;
    if (!TKrnlEnvironment::bCharsInValue(strKey.pszBuffer(), c4Chars))
    {
        tCIDLib::TErrCode errcToThrow = kCIDErrs::errcEnv_FindError;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() == kKrnlErrs::errcGen_NotFound)
        {
            errcToThrow = kCIDErrs::errcEnv_KeyNotFound;
            eClass = tCIDLib::EErrClasses::NotFound;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
            , strKey
        );
    }
    return c4Chars;
}


TString TProcEnvironment::strFind(const TString& strKey)
{
    TProcEnvLocker lockEnv;

    const tCIDLib::TCh* pszValue = TKrnlEnvironment::pszFind(strKey.pszBuffer());
    if (!pszValue)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEnv_KeyNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKey
        );
    }
    return TString(pszValue);
}


tCIDLib::TVoid
TProcEnvironment::Update(   const   TString&    strKey
                            , const TString&    strNewValue)
{
    if (!TKrnlEnvironment::bUpdate(strKey.pszBuffer(), strNewValue.pszBuffer()))
    {
        tCIDLib::TErrCode errcToThrow = kCIDErrs::errcEnv_UpdateError;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() == kKrnlErrs::errcGen_NotFound)
        {
            errcToThrow = kCIDErrs::errcEnv_KeyNotFound;
            eClass = tCIDLib::EErrClasses::NotFound;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
            , strKey
        );
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TProcEnvLocker
//  PREFIX: lock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProcEnvLocker: Constructors and Destructor
// ---------------------------------------------------------------------------
TProcEnvLocker::TProcEnvLocker()
{
    TKrnlEnvironment::Lock();
}

TProcEnvLocker::~TProcEnvLocker()
{
    TKrnlEnvironment::Unlock();
}




// ---------------------------------------------------------------------------
//   CLASS: TEnvironment
//  PREFIX: env
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEnvironment: Public, static methods
// ---------------------------------------------------------------------------
TEnvironment& TEnvironment::Nul_TEnvironment()
{
    static TEnvironment* penvNull = nullptr;

    if (!penvNull)
    {
        TBaseLock lockInit;
        if (!penvNull)
           TRawMem::pExchangePtr(&penvNull, new TEnvironment);
    }
    return *penvNull;
}


// ---------------------------------------------------------------------------
//  TEnvironment: Constructors and Destructor
// ---------------------------------------------------------------------------

// Make sure key ops is in non-case sensitive mode
TEnvironment::TEnvironment(const tCIDLib::TBoolean bInherit) :

    m_pcolEnv
    (
        new tCIDLib::TKVHashSet
        (
            CIDLib_Environment::c4Modulus
            , TStringKeyOps(kCIDLib::False)
            , strGetKey
            , tCIDLib::EMTStates::Unsafe
        )
    )
{
    //
    //  If the bInherit parm is set, then we need to query a snapshot of
    //  the environment and load ourself up with the same info.
    //
    if (bInherit)
    {
        // Load up a list of key value pair objects
        TKrnlEnvironment::TElem* pelemList;

        tCIDLib::TCard4 c4Count = 0;
        if (!TKrnlEnvironment::bQueryState(pelemList, c4Count))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEnv_Inherit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }

        if (c4Count)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                m_pcolEnv->objPlace
                (
                    pelemList[c4Index].pszKey, pelemList[c4Index].pszValue
                );
            }

            // Delete the list
            delete [] pelemList;
        }
    }
}

TEnvironment::TEnvironment(const TEnvironment& envSrc) :

    m_pcolEnv(nullptr)
{
    // Copy construct a new hash map that replicates the source's
    m_pcolEnv = new tCIDLib::TKVHashSet(*envSrc.m_pcolEnv);
}

// Just create the smallest possible hash set then swap
TEnvironment::TEnvironment(TEnvironment&& envSrc) :

    m_pcolEnv
    (
        new tCIDLib::TKVHashSet
        (
            1, TStringKeyOps(kCIDLib::False), strGetKey, tCIDLib::EMTStates::Unsafe
        )
    )
{
    *this = operator=(tCIDLib::ForceMove(envSrc));
}

TEnvironment::~TEnvironment()
{
    delete m_pcolEnv;
    m_pcolEnv = nullptr;
}


// ---------------------------------------------------------------------------
//  TEnvironment: Public operators
// ---------------------------------------------------------------------------
TEnvironment& TEnvironment::operator=(const TEnvironment& envSrc)
{
    if (&envSrc != this)
        *m_pcolEnv = *envSrc.m_pcolEnv;
    return *this;
}

// We just swap our lists
TEnvironment& TEnvironment::operator=(TEnvironment&& envSrc)
{
    if (&envSrc != this)
        tCIDLib::Swap(m_pcolEnv, envSrc.m_pcolEnv);
    return *this;
}


// ---------------------------------------------------------------------------
//  TEnvironment: Public non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TEnvironment::Add(const TString& strKey, const TString& strNewValue)
{
    m_pcolEnv->objPlace(strKey, strNewValue);
}


tCIDLib::TBoolean
TEnvironment::bAddIfNew(const TString& strKey, const TString& strValue)
{
    TKeyValuePair* pkvalTmp = m_pcolEnv->pobjFindByKey(strKey);
    if (!pkvalTmp)
    {
        // It's not there currently, so add it
        m_pcolEnv->objPlace(strKey, strValue);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TEnvironment::bAddOrUpdate(const TString& strKey, const TString& strNewValue)
{
    TKeyValuePair* pkvalTmp = m_pcolEnv->pobjFindByKey(strKey);
    if (pkvalTmp)
    {
        pkvalTmp->strValue(strNewValue);
        return kCIDLib::False;
    }

    m_pcolEnv->objPlace(strKey, strNewValue);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TEnvironment::bFind(const TString& strKey, TString& strToFill) const
{
    try
    {
        strToFill = m_pcolEnv->objFindByKey(strKey).strValue();
    }

    catch(TError& errToCatch)
    {
        // If its not a 'key not found' error, then rethrow
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_KeyNotFound))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }

        // Otherwise, we just didn't find it
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TEnvironment::bKeyExists(const TString& strKey) const
{
    return (m_pcolEnv->pobjFindByKey(strKey) != nullptr);
}


tCIDLib::TCard4 TEnvironment::c4Entries() const
{
    return m_pcolEnv->c4ElemCount();
}


tCIDLib::TCard4 TEnvironment::chCharsInValue(const TString& strKey)
{
    const TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    return kvalFind.strValue().c4Length();
}


TEnvironment::TCursor TEnvironment::cursThis() const
{
    return TCursor(m_pcolEnv);
}


tCIDLib::TVoid TEnvironment::RemoveAll()
{
    m_pcolEnv->RemoveAll();
}


const TString& TEnvironment::strFind(const TString& strKey) const
{
    const TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    return kvalFind.strValue();
}


tCIDLib::TVoid TEnvironment::Update(const   TString&    strKey
                                    , const TString&    strNewValue)
{
    TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    kvalFind.strValue(strNewValue);
}
