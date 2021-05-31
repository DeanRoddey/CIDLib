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


namespace
{
    namespace CIDLib_Environment
    {
        // -----------------------------------------------------------------------
        //  Local, const data
        //
        //  c4Modulus
        //      This is the modulus used in the hashing of the key strings.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4    c4Modulus = 23;
    }
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
TProcEnvironment::Add(const TStringView& strvKey, const TStringView& strvNewValue)
{
    if (!TKrnlEnvironment::bAdd(strvKey.pszBuffer(), strvNewValue.pszBuffer()))
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
            , TString(strvKey)
        );
    }
}


tCIDLib::TBoolean
TProcEnvironment::bAddOrUpdate(const TStringView& strvKey, const TStringView& strvNewValue)
{
    tCIDLib::TBoolean   bAdded;
    if (!TKrnlEnvironment::bAddOrUpdate(strvKey.pszBuffer()
                                        , strvNewValue.pszBuffer()
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
            , TString(strvKey)
        );
    }
    return bAdded;
}


tCIDLib::TVoid
TProcEnvironment::AddToExePath( const   TStringView&        strvNewValue
                                , const tCIDLib::EStartEnd  eWhere)
{
    if (!TKrnlEnvironment::bAddToExePath(strvNewValue.pszBuffer(), eWhere))
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
TProcEnvironment::AddToLibPath( const   TStringView&        strNewValue
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
TProcEnvironment::bFind(const TStringView& strvKey, TString& strToFill)
{
    TProcEnvLocker lockEnv;

    const tCIDLib::TCh* pszValue = TKrnlEnvironment::pszFind(strvKey.pszBuffer());
    if (!pszValue)
        return kCIDLib::False;

    strToFill = pszValue;
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindExePath(TString& strToFill)
{
    TKrnlString kstrToFill;
    if (!TKrnlEnvironment::bFindExePath(kstrToFill))
        return kCIDLib::False;

    strToFill = kstrToFill.pszValue();
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindLibPath(TString& strToFill)
{
    TKrnlString kstrToFill;
    if (!TKrnlEnvironment::bFindLibPath(kstrToFill))
        return kCIDLib::False;

    strToFill = kstrToFill.pszValue();
    return kCIDLib::True;
}


tCIDLib::TBoolean TProcEnvironment::bFindTempPath(TString& strToFill)
{
    TKrnlString kstrToFill;
    if (!TKrnlEnvironment::bFindTempPath(kstrToFill))
        return kCIDLib::False;

    strToFill = kstrToFill.pszValue();
    return kCIDLib::True;
}


tCIDLib::TCard4 TProcEnvironment::c4Entries()
{
    return TKrnlEnvironment::c4Entries();
}


tCIDLib::TCard4 TProcEnvironment::c4CharsInValue(const TStringView& strvKey)
{
    tCIDLib::TCard4 c4Chars;
    if (!TKrnlEnvironment::bCharsInValue(strvKey.pszBuffer(), c4Chars))
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
            , TString(strvKey)
        );
    }
    return c4Chars;
}


TString TProcEnvironment::strFind(const TStringView& strvKey)
{
    TProcEnvLocker lockEnv;

    const tCIDLib::TCh* pszValue = TKrnlEnvironment::pszFind(strvKey.pszBuffer());
    if (!pszValue)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEnv_KeyNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TString(strvKey)
        );
    }
    return TString(pszValue);
}


tCIDLib::TVoid
TProcEnvironment::Update(   const   TStringView&    strvKey
                            , const TStringView&    strvNewValue)
{
    if (!TKrnlEnvironment::bUpdate(strvKey.pszBuffer(), strvNewValue.pszBuffer()))
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
            , TString(strvKey)
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
TEnvironment& TEnvironment::Nul_TEnvironment() noexcept
{
    static TEnvironment envNull;
    return envNull;
}


// ---------------------------------------------------------------------------
//  TEnvironment: Constructors and Destructor
// ---------------------------------------------------------------------------

// Make sure key ops is in non-case sensitive mode
TEnvironment::TEnvironment(const tCIDLib::TBoolean bInherit) :

    m_pcolEnv(nullptr)
{
    //
    //  If the bInherit parm is set, then we need to query a snapshot of
    //  the environment and load ourself up with the same info.
    //
    //  If not inheriting, we can be efficient and only fault our list in
    //  if it is ever actually needed.
    //
    if (bInherit)
    {
        FaultInList();

        // Load up a list of key value pair objects from the kernel
        TKrnlEnvironment::TElem* pelemList = nullptr;
        TArrayJanitor<TKrnlEnvironment::TElem> janList(pelemList);

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

            // Won't happen but makes analyzer happy
            return;
        }

        if (c4Count)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                m_pcolEnv->objPlace
                (
                    pelemList[c4Index].m_pszKey, pelemList[c4Index].m_pszValue
                );
            }
        }
    }
}

TEnvironment::TEnvironment(const TEnvironment& envSrc) :

    m_pcolEnv(nullptr)
{
    // If the source has a list, make a copy
    if (envSrc.m_pcolEnv)
        m_pcolEnv = new tCIDLib::TKVHashSet(*envSrc.m_pcolEnv);
}


// We just give away a null pointer and take his, which may also be null
TEnvironment::TEnvironment(TEnvironment&& envSrc) :

    m_pcolEnv(nullptr)
{
    operator=(tCIDLib::ForceMove(envSrc));
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
    {
        // If he has a list and we don't, then fault ours in
        if (!m_pcolEnv && envSrc.m_pcolEnv)
            FaultInList();

        //
        //  If he has a list, then we will as well at this point, so copy
        //  his into ours. Else, if we have one, then just clear it out.
        //
         if (envSrc.m_pcolEnv)
            *m_pcolEnv = *envSrc.m_pcolEnv;
        else if (m_pcolEnv)
            m_pcolEnv->RemoveAll();
    }
    return *this;
}


// We just swap our lists, either of which may be null
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
TEnvironment::Add(const TStringView& strvKey, const TStringView& strvNewValue)
{
    if (!m_pcolEnv)
        FaultInList();

    m_pcolEnv->objPlace(strvKey, strvNewValue);
}


tCIDLib::TBoolean
TEnvironment::bAddIfNew(const TString& strKey, const TStringView& strvValue)
{
    if (!m_pcolEnv)
        FaultInList();

    TKeyValuePair* pkvalTmp = m_pcolEnv->pobjFindByKey(strKey);
    if (!pkvalTmp)
    {
        // It's not there currently, so add it
        m_pcolEnv->objPlace(strKey, strvValue);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TEnvironment::bAddOrUpdate(const TString& strKey, const TStringView& strvNewValue)
{
    if (!m_pcolEnv)
        FaultInList();

    TKeyValuePair* pkvalTmp = m_pcolEnv->pobjFindByKey(strKey);
    if (pkvalTmp)
    {
        pkvalTmp->strValue(strvNewValue);
        return kCIDLib::False;
    }

    m_pcolEnv->objPlace(strKey, strvNewValue);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TEnvironment::bFind(const TString& strKey, TString& strToFill) const
{
    if (!m_pcolEnv)
        return kCIDLib::False;

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
    if (!m_pcolEnv)
        return kCIDLib::False;

    return (m_pcolEnv->pobjFindByKey(strKey) != nullptr);
}


tCIDLib::TCard4 TEnvironment::c4Entries() const
{
    if (!m_pcolEnv)
        return 0;

    return m_pcolEnv->c4ElemCount();
}


tCIDLib::TCard4 TEnvironment::chCharsInValue(const TString& strKey)
{
    if (!m_pcolEnv)
        FaultInList();

    const TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    return kvalFind.strValue().c4Length();
}


TEnvironment::TCursor TEnvironment::cursThis() const
{
    if (!m_pcolEnv)
        FaultInList();

    return TCursor(m_pcolEnv);
}


tCIDLib::TVoid TEnvironment::RemoveAll()
{
    if (m_pcolEnv)
        m_pcolEnv->RemoveAll();
}


const TString& TEnvironment::strFind(const TString& strKey) const
{
    if (!m_pcolEnv)
        FaultInList();

    const TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    return kvalFind.strValue();
}


tCIDLib::TVoid TEnvironment::Update(const   TString&        strKey
                                    , const TStringView&    strvNewValue)
{
    if (!m_pcolEnv)
        FaultInList();

    TKeyValuePair& kvalFind = m_pcolEnv->objFindByKey(strKey);
    kvalFind.strValue(strvNewValue);
}


// ---------------------------------------------------------------------------
//  TEnvironment: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TEnvironment::FaultInList() const
{
    // Won't ever actually return false, just to make the analyzer happy
    if (!bCIDPreCond(m_pcolEnv == nullptr))
        return;

    if (m_pcolEnv)
    {
        // If this happens in release mode, be tolerant and just flush the existing lists
        m_pcolEnv->RemoveAll();
    }
     else
    {
        m_pcolEnv = new tCIDLib::TKVHashSet
        (
            CIDLib_Environment::c4Modulus
            , TStringKeyOps(kCIDLib::False)
            , strGetKey
            , tCIDLib::EMTStates::Unsafe
        );
    }
}
