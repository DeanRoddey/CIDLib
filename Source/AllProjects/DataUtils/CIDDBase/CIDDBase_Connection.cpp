//
// FILE NAME: CIDDBase_Connection.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This file implements DB connection class.
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
RTTIDecls(TDBConnection,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TODBCConnHandle
//  PREFIX: odbch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TODBCConnHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TODBCConnHandle::TODBCConnHandle() :

    m_hConn(SQL_NULL_HDBC)
{
}

TODBCConnHandle::TODBCConnHandle(const  tCIDDBase::THandle  hConn
                                , const TString&            strSrcName) :
    m_hConn(hConn)
    , m_strDataSrc(strSrcName)
{
}

TODBCConnHandle::~TODBCConnHandle()
{
    // If the connection is open, then close it
    if (m_hConn != SQL_NULL_HDBC)
    {
        try
        {
            ::SQLDisconnect(m_hConn);
        }

        catch(...)
        {
        }

        try
        {
            ::SQLFreeHandle(SQL_HANDLE_DBC, m_hConn);
        }

        catch(...)
        {
        }
    }
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDDBase::THandle TODBCConnHandle::hConn() const
{
    return m_hConn;
}

const TString& TODBCConnHandle::strDataSrc() const
{
    return m_strDataSrc;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBConnection
// PREFIX: dbcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBConnection: Constructors and Destructor.
// ---------------------------------------------------------------------------

//
//  When we construct, we need to set a default connection handle on our
//  counted pointer.
//
TDBConnection::TDBConnection() :

    m_cptrHandle(new TODBCConnHandle)
{
}

TDBConnection::~TDBConnection()
{
}


// ---------------------------------------------------------------------------
//  TDBConnection: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBConnection::bIsConnected() const
{
    // If we've not stored a connection handle yet, then obviously not
    const TODBCConnHandle* podbchTest = m_cptrHandle.pobjData();
    if (!podbchTest)
        return kCIDLib::False;
    return (podbchTest->hConn() != SQL_NULL_HDBC);
}


//
//  Connects using either Windows authorization or SQL authorization. In the
//  former case, the user name and password strings will be ignored.
//
tCIDLib::TVoid TDBConnection::Connect(const TString& strSrcName)
{
    // Just call the other version with dummy user and password
    Connect(strSrcName, TString::strEmpty(), TString::strEmpty(), kCIDLib::True);
}

tCIDLib::TVoid
TDBConnection::Connect( const   TString&            strSrcName
                        , const TString&            strUserName
                        , const TString&            strPassword
                        , const tCIDLib::TBoolean   bUseWinAuth)
{
    // Make sure not already connected
    CheckAlreadyConnected();

    // Create the connection handle
    tCIDDBase::THandle hConn;
    SQLRETURN RetVal;
    RetVal = ::SQLAllocHandle(SQL_HANDLE_DBC, facCIDDBase().hEnv(), &hConn);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_ENV, facCIDDBase().hEnv(), RetVal, strInfo
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

    // Use Windows security when we connect if asked to
    if (bUseWinAuth)
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
    RetVal = ::SQLConnect
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
    m_cptrHandle.SetPointer(new TODBCConnHandle(hConn, strSrcName));
}


// Connect through a connection string
tCIDLib::TVoid
TDBConnection::ConnectStr(  const   TString&    strConnName
                            , const TString&    strInConnStr
                            ,       TString&    strOutConnStr)
{
    // Make sure not already connected
    CheckAlreadyConnected();

    // Create the connection handle
    tCIDDBase::THandle hConn;
    SQLRETURN RetVal;
    RetVal = ::SQLAllocHandle(SQL_HANDLE_DBC, facCIDDBase().hEnv(), &hConn);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_ENV, facCIDDBase().hEnv(), RetVal, strInfo
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
    RetVal = ::SQLDriverConnect
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
    m_cptrHandle.SetPointer(new TODBCConnHandle(hConn, strConnName));
}



//
//  Just drop our current handle reference. This won't actually cause the
//  connection to close until any statement objects have been destroyed, but
//  it'll make this object looked disconnected so that it can be reused.
//
tCIDLib::TVoid TDBConnection::Disconnect()
{
    m_cptrHandle.DropRef();
}


//
//  Provide access to the connection handle, by way of the counted pointer
//  object that wraps it.
//
const tCIDDBase::TODBCHandleRef& TDBConnection::cptrHandle() const
{
    return m_cptrHandle;
}

tCIDDBase::TODBCHandleRef& TDBConnection::cptrHandle()
{
    return m_cptrHandle;
}


// ---------------------------------------------------------------------------
//  TDBConnection: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Throws if this object is already connected
tCIDLib::TVoid TDBConnection::CheckAlreadyConnected()
{
    TODBCConnHandle* podbchTest = m_cptrHandle.pobjData();
    if (podbchTest && (podbchTest->hConn() != SQL_NULL_HDBC))
    {
        facCIDDBase().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_AlreadyConnected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , m_cptrHandle->strDataSrc()
        );
    }
}


// Throws if this object is not connected
tCIDLib::TVoid TDBConnection::CheckNotConnected()
{
    TODBCConnHandle* podbchTest = m_cptrHandle.pobjData();
    if (podbchTest && (podbchTest->hConn() == SQL_NULL_HDBC))
    {
        facCIDDBase().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_NotConnected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }
}


