//
// FILE NAME: CIDDBase_Connection.cpp
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
//  This file implements DB connection class and the underlying fundamental
//  connection handle class.
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
//  CLASS: TDBConnection
// PREFIX: dbcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBConnection: Constructors and Destructor.
// ---------------------------------------------------------------------------

//
//  When we construct, we need to set a default connection handle on our counted handle
//  member. So it will be set but not valid at this point.
//
TDBConnection::TDBConnection() :

    m_hConnection(new TODBCConnHandle)
{
}


// If we are the last reference to the connection handle, it will be dropped
TDBConnection::~TDBConnection()
{
}


// ---------------------------------------------------------------------------
//  TDBConnection: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBConnection::bIsConnected() const
{
    // Should always be set, but just in case
    if (!m_hConnection)
        return kCIDLib::False;
    return m_hConnection->bIsValid();
}


//
//  This one we can handle here and just call the other version. This will use OS
//  authorization, so we just pass along empty name/password info.
//
tCIDLib::TVoid TDBConnection::Connect(const TString& strSrcName)
{
    // Just call the other version with dummy user and password
    Connect(strSrcName, TString::strEmpty(), TString::strEmpty(), kCIDLib::True);
}


//
//  Just drop our current handle reference. This won't actually cause the
//  connection to close until all statement objects have been destroyed, but
//  it'll make this object looked disconnected so that it can be reused.
//
tCIDLib::TVoid TDBConnection::Disconnect()
{
    m_hConnection.DropRef();
}


// Provide access to the ref countedd connection handle
const tCIDDBase::THConn& TDBConnection::hConnection() const
{
    return m_hConnection;
}

tCIDDBase::THConn& TDBConnection::hConnection()
{
    return m_hConnection;
}


// ---------------------------------------------------------------------------
//  TDBConnection: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Throws if this object is already connected
tCIDLib::TVoid TDBConnection::CheckAlreadyConnected()
{
    if (m_hConnection && m_hConnection->bIsValid())
    {
        facCIDDBase().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcConn_AlreadyConnected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , m_hConnection->strDataSrc()
        );
    }
}


// Throws if this object is not connected
tCIDLib::TVoid TDBConnection::CheckNotConnected()
{
    if (!m_hConnection || !m_hConnection->bIsValid())
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


