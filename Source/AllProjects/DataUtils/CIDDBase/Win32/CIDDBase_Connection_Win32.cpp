//
// FILE NAME: CIDDBase_Connection_Win32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/01/2019
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
//  This file implements the Win32 specific versions of a database connection and
//  the main connection handle for a database connection/session.
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
#include    "CIDDBase_Win32.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TODBCConnHandle
//  PREFIX: odbch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TODBCConnHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TODBCConnHandle::TODBCConnHandle() :

    m_pConn(new tCIDDBase::TConnHandle)
{
    m_pConn->hConn = SQL_NULL_HANDLE;
}

TODBCConnHandle::~TODBCConnHandle()
{
    // Clean up any of our internal content
    if (m_pConn != nullptr)
    {
        if (m_pConn->hConn != SQL_NULL_HANDLE)
        {
            try
            {
                ::SQLDisconnect(m_pConn->hConn);
            }

            catch(...)
            {
            }

            try
            {
                ::SQLFreeHandle(SQL_HANDLE_DBC, m_pConn->hConn);
            }

            catch(...)
            {
            }

            m_pConn->hConn = SQL_NULL_HANDLE;
        }

        delete m_pConn;
        m_pConn = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TODBCConnHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TODBCConnHandle::bIsValid() const
{
    return (m_pConn && (m_pConn->hConn != SQL_NULL_HANDLE));
}



// ---------------------------------------------------------------------------
//   CLASS: TDBConnection
//  PREFIX: dbcon
// ---------------------------------------------------------------------------

// This one is a connection via data source
tCIDLib::TVoid
TDBConnection::Connect( const   TString&            strSrcName
                        , const TString&            strUserName
                        , const TString&            strPassword
                        , const tCIDLib::TBoolean   bUseOSAuth)
{
    // Make sure not already connected
    CheckAlreadyConnected();

    // Let's try to get a raw connection handle set up
    SQLHANDLE hConn = SQL_NULL_HANDLE;
    SQLRETURN RetVal;
    RetVal = ::SQLAllocHandle(SQL_HANDLE_DBC, facCIDDBase().hEnv().hEnv, &hConn);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_ENV, facCIDDBase().hEnv().hEnv, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_ConnFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , strSrcName
            , strInfo
        );
    }

    // Use OS security when we connect if asked to
    if (bUseOSAuth)
    {
        ::SQLSetConnectAttr
        (
            hConn
            , SQL_COPT_SS_INTEGRATED_SECURITY
            , (SQLPOINTER)SQL_IS_ON
            , SQL_IS_INTEGER
        );
    }

    // And try to connect now.
    RetVal = ::SQLConnectW
    (
        hConn
        , (SQLWCHAR*)strSrcName.pszBuffer()
        , (SQLSMALLINT)strSrcName.c4Length()
        , (SQLWCHAR*)strUserName.pszBuffer()
        , (SQLSMALLINT)strUserName.c4Length()
        , (SQLWCHAR*)strPassword.pszBuffer()
        , (SQLSMALLINT)strPassword.c4Length()
    );

    //
    //  If it fails, we just clean up the handle that we created and
    //  throw.
    //
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        // Translate the error before we close the handle
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_DBC, hConn, RetVal, strInfo
        );
        ::SQLFreeHandle(SQL_HANDLE_DBC, hConn);

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_ConnFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSrcName
            , strInfo
        );
    }

    //  And now set it as our new handle. This will let any previous handle
    //  go. We store the source name on the handle object for later use in
    //  any error reporting.
    //
    TODBCConnHandle* podbchNew = new TODBCConnHandle();
    podbchNew->m_pConn->hConn = hConn;
    podbchNew->m_strDataSrc = strSrcName;
    m_hConnection.SetPointer(podbchNew);
}


// Connect through a connection string
tCIDLib::TVoid
TDBConnection::ConnectStr(  const   TString&    strConnName
                            , const TString&    strInConnStr
                            ,       TString&    strOutConnStr)
{
    // Make sure not already connected
    CheckAlreadyConnected();

    // Create the raw connection handle
    SQLHANDLE hConn = SQL_NULL_HANDLE;
    SQLRETURN RetVal;
    RetVal = ::SQLAllocHandle(SQL_HANDLE_DBC, facCIDDBase().hEnv().hEnv, &hConn);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_ENV, facCIDDBase().hEnv().hEnv, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_ConnStrFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , strInfo
        );
    }

    // Try the connection string type connect
    tCIDLib::TCh achOutStr[2047];
    SQLSMALLINT OutLen;
    RetVal = ::SQLDriverConnectW
    (
        hConn
        , 0
        , (SQLWCHAR*)strInConnStr.pszBuffer()
        , (SQLSMALLINT)strInConnStr.c4Length()
        , achOutStr
        , 2047
        , &OutLen
        , SQL_DRIVER_NOPROMPT
    );

    //
    //  If it fails, we just clean up the handle that we created and
    //  throw.
    //
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        // Translate the error before we close the handle
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_DBC, hConn, RetVal, strInfo
        );
        ::SQLFreeHandle(SQL_HANDLE_DBC, hConn);

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_ConnStrFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strInfo
        );
    }

    // Give back the output connection string
    strOutConnStr = achOutStr;

    // And store the handle info
    TODBCConnHandle* podbchNew = new TODBCConnHandle();
    podbchNew->m_pConn->hConn = hConn;
    podbchNew->m_strDataSrc = strInConnStr;
    m_hConnection.SetPointer(podbchNew);
}
