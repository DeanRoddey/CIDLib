//
// FILE NAME: CIDOrbUC_CoreAdminBaseImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides a base implementation of the core server admin IDL
//  interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDCoreAdminBaseImpl,TCQCSrvAdminServerBase)



// ---------------------------------------------------------------------------
//   CLASS: TCIDCoreAdminBaseImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDCoreAdminBaseImpl: Destructor
// ---------------------------------------------------------------------------
TCIDCoreAdminBaseImpl::~TCIDCoreAdminBaseImpl()
{
}


// ---------------------------------------------------------------------------
//  TCIDCoreAdminBaseImpl: Public, inherited methods
// ---------------------------------------------------------------------------

// We handle this on behalf of all dervatives
tCIDLib::TBoolean TCIDCoreAdminBaseImpl::
bPollValuesInScope( const   TString&                        strParScope
                    , const tCIDLib::TBoolean               bDirectOnly
                    , const tCIDLib::TCard8                 c8QueryStamp
                    ,       tCIDLib::TCard8&                c8PollStamp
                    ,       TFundVector<tCIDLib::TCard4>&   fcolIds
                    ,       TFundVector<tCIDLib::TCard8>&   fcolValues)
{
    // Just forward it to the stats cache
    return TStatsCache::bPollValuesInScope
    (
        strParScope, bDirectOnly, c8QueryStamp, c8PollStamp, fcolIds, fcolValues
    );
}


// We handle this one on behalf of all derivatives
tCIDLib::TVoid
TCIDCoreAdminBaseImpl::QueryBasicInfo(  TString&                strHostName
                                        , TString&              strProcName
                                        , tCIDLib::TCard4&      c4CPUCount
                                        , TString&              strOSInfo
                                        , tCIDLib::TCard8&      c8StartTime)
{
    strProcName = m_strProcName;
    strHostName = facCIDSock().strIPHostName();
    c4CPUCount = TSysInfo::c4CPUCount();
    c8StartTime = m_enctStartedAt;
    TSysInfo::QueryOSInfoStr(strOSInfo);
}


// We handle this on behalf of all derivatives
tCIDLib::TCard4
TCIDCoreAdminBaseImpl::c4QueryStats(const   TString&                strParScope
                                    ,       TVector<TStatsCacheItemInfo>& colToFill
                                    , const tCIDLib::TBoolean       bDirectOnly
                                    ,       tCIDLib::TCard8&        c8Stamp)
{
    // Just forward it to the stats cache
    return TStatsCache::c4QueryValuesInScope
    (
        strParScope, colToFill, bDirectOnly, c8Stamp
    );
}


// We handle this on behalf of all derivatives
tCIDLib::TVoid
TCIDCoreAdminBaseImpl::SetModLogLevel(  const   TString&    strModName
                                        , const TString&    strLevel)
{
    // Check the level for validity
    TString strTmp(TStrCat(L"ESev_", strLevel));
    const tCIDLib::ESeverities eSev = tCIDLib::eXlatESeverities(strTmp);

    // It wasn't a valid level
    if (eSev == tCIDLib::ESeverities::Count)
    {
        // We can throw a generic error here
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strLevel
            , TString(L"tCIDLib::ESeverities")
        );
    }


    TFacility* pfacTar = TFacility::pfacFromName(strModName);
    if (pfacTar)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantFindFac
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strModName
        );
    }
     else
    {
        pfacTar->SetLogThreshold(eSev);
    }
}


// We handle this on behalf of all derivatives
tCIDLib::TVoid
TCIDCoreAdminBaseImpl::SetModLogTrace(  const   TString&            strModName
                                        , const tCIDLib::TBoolean   bToSet)
{
    TFacility* pfacTar = TFacility::pfacFromName(strModName);
    if (pfacTar)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantFindFac
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strModName
        );
    }
     else
    {
        pfacTar->SetLogTrace(bToSet);
    }
}


// ---------------------------------------------------------------------------
//  TCIDCoreAdminBaseImpl: Hidden Constructors
// ---------------------------------------------------------------------------
TCIDCoreAdminBaseImpl::TCIDCoreAdminBaseImpl(const TString& strProcName) :

    m_enctStartedAt(TTime::enctNow())
    , m_strProcName(strProcName)
{
}

