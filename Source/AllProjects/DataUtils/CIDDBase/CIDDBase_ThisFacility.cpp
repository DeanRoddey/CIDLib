//
// FILE NAME: CIDDBase_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
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
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDDBase,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDDBase_ThisFacility
{
    // -----------------------------------------------------------------------
    //  We have to initialize the database support, and these values are used
    //  to know if we've initialized and to store the init info.
    // -----------------------------------------------------------------------
    volatile tCIDLib::TBoolean  bInitDone;
    SQLHANDLE                   hSQLEnv;
}


// ---------------------------------------------------------------------------
//  An internal method for translating errors
// ---------------------------------------------------------------------------
TKrnlError
kerrcXlatDBError(const  SQLSMALLINT     iType
                ,       SQLHANDLE       hToCheck
                , const SQLRETURN       sDBErr
                ,       TString&        strInfo)
{
    // Default is an unknown error
    TKrnlError kerrRet;

    if (sDBErr == SQL_ERROR)
    {
        // We get get further info
        const SQLSMALLINT iMaxMsgChars = 1024;
        tCIDLib::TCh achMsg[iMaxMsgChars + 1];
        achMsg[0] = kCIDLib::chNull;

        const tCIDLib::TCard4 c4MaxStateChars = 6;
        tCIDLib::TCh achState[c4MaxStateChars + 1];

        SQLINTEGER sNativeErr;
        SQLSMALLINT iActualChars;
        ::SQLGetDiagRec
        (
            iType
            , hToCheck
            , 1
            , achState
            , &sNativeErr
            , achMsg
            , iMaxMsgChars
            , &iActualChars
        );

        strInfo = achMsg;
        kerrRet.Set(kKrnlErrs::errcHostErr, sNativeErr);
    }
     else
    {
        // Note the general error, so handle the specific ones
        switch(sDBErr)
        {
            case SQL_INVALID_HANDLE :
                kerrRet.errcId(kKrnlErrs::errcGen_InvalidHandle);
                break;

            default :
                kerrRet.errcId(kKrnlErrs::errcUnknown);
                break;
        };
    }
    return kerrRet;
}





// ---------------------------------------------------------------------------
//   CLASS: TFacCIDDBase
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDDBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDDBase::TFacCIDDBase() :

    TFacility
    (
        L"CIDDBase"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDDBase::~TFacCIDDBase()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDDBase::THandle TFacCIDDBase::hEnv() const
{
    return CIDDBase_ThisFacility::hSQLEnv;
}


tCIDLib::TVoid TFacCIDDBase::Initialize()
{
    if (!CIDDBase_ThisFacility::bInitDone)
    {
        TBaseLock lockInit;
        if (!CIDDBase_ThisFacility::bInitDone)
        {
            SQLRETURN ResVal = ::SQLAllocHandle
            (
                SQL_HANDLE_ENV, SQL_NULL_HANDLE, &CIDDBase_ThisFacility::hSQLEnv
            );

            if ((ResVal != SQL_SUCCESS_WITH_INFO) && (ResVal != SQL_SUCCESS))
            {
                //
                //  DON'T call the error translator function here, because
                //  we never got the environment set up.
                //
                facCIDDBase().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kDBErrs::errcInit_SQLEnvInitFailed
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::InitFailed
                );
            }

            // Notify ODBC that this is an ODBC 3.0 app
            ResVal = ::SQLSetEnvAttr
            (
                CIDDBase_ThisFacility::hSQLEnv
                , SQL_ATTR_ODBC_VERSION
                , (SQLPOINTER) SQL_OV_ODBC3
                , SQL_IS_INTEGER
            );

            if ((ResVal != SQL_SUCCESS_WITH_INFO) && (ResVal != SQL_SUCCESS))
            {
                facCIDDBase().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kDBErrs::errcInit_SQLEnvInitFailed
                    , L"Failed to set ODBC version"
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::InitFailed
                );
            }

            // Mark us initialized now
            CIDDBase_ThisFacility::bInitDone = kCIDLib::True;
        }
    }
}


tCIDLib::TVoid
TFacCIDDBase::QuerySources(         tCIDLib::TKVPCollect&   colToFill
                            , const tCIDDBase::ESrcTypes    eType)
{
    const SQLSMALLINT iMaxNameChars = 1024;
    tCIDLib::TCh achName[iMaxNameChars + 1];

    const SQLSMALLINT iMaxDescChars = 1024;
    tCIDLib::TCh achDesc[iMaxDescChars + 1];

    tCIDLib::TBoolean bDone = kCIDLib::False;
    SQLUSMALLINT iDir;

    if (eType == tCIDDBase::ESrcTypes::System)
        iDir = SQL_FETCH_FIRST_SYSTEM;
    else if (eType == tCIDDBase::ESrcTypes::User)
        iDir = SQL_FETCH_FIRST_USER;
    else
        iDir = SQL_FETCH_FIRST;

    SQLSMALLINT iActNameChars;
    SQLSMALLINT iActDescChars;

    TKeyValuePair kvalCur;
    while (!bDone)
    {
        const SQLRETURN RetVal = ::SQLDataSources
        (
            CIDDBase_ThisFacility::hSQLEnv
            , iDir
            , achName
            , iMaxNameChars
            , &iActNameChars
            , achDesc
            , iMaxDescChars
            , &iActDescChars
        );

        if ((RetVal != SQL_SUCCESS_WITH_INFO)
        &&  (RetVal != SQL_SUCCESS)
        &&  (RetVal != SQL_NO_DATA))
        {
            TString strInfo;
            TKrnlError kerrThrow = kerrcXlatDBError
            (
              SQL_HANDLE_ENV
              , CIDDBase_ThisFacility::hSQLEnv
              , RetVal
              , strInfo
            );

            facCIDDBase().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kDBErrs::errcConn_QuerySrcs
                , kerrThrow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::InitFailed
                , strInfo
            );
        }

        // If no data, we are done
        if (RetVal == SQL_NO_DATA)
            break;

        // Fill one in
        kvalCur.Set(achName, achDesc);
        colToFill.objAdd(kvalCur);

        // Now start doing next queries
        iDir = SQL_FETCH_NEXT;
    }
}


tCIDLib::TVoid TFacCIDDBase::Terminate()
{
    if (CIDDBase_ThisFacility::bInitDone)
    {
        TBaseLock lockInit;
        if (CIDDBase_ThisFacility::bInitDone)
        {
            ::SQLFreeHandle(SQL_HANDLE_ENV, CIDDBase_ThisFacility::hSQLEnv);
            CIDDBase_ThisFacility::bInitDone = kCIDLib::False;
        }
    }
}


