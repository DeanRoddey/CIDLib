//
// FILE NAME: CIDDBase_Connection.hpp
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
//  This is the header file for the CIDDBase_Connection.Cpp file, which represents
//  a connection to a particular database and is needed to do operations against
//  that database.
//
//  We need a small class to represent a database connection. We have a database
//  connection object as well, but this is a wrapper around the platform specific
//  information necessary to represent a connection.
//
//  Below we define a counted pointer to one of these and that becomes our fundamental
//  database connection handle. Anything that needs to keep around the connection
//  that was used to create it (so that it can complete/undo something later) will
//  make a copy of this reference counted handle to make sure the connection stays
//  alive until its not needed anymore.
//
//  So the actual connection object can be discarded after any required statements have
//  been set up. They will keep the connection alive until its no longer needed. Other
//  per-platform code can look at the internal definition of the connection handle info
//  as required.
//
//  We can connect via an ODBC data source or a connection string, and have separate
//  connection methods for the two schemes. We can use database security or local OS
//  security. IF local OS security then the username/password is ignored and can be
//  blank.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TDBConnection;

// ---------------------------------------------------------------------------
//   CLASS: TODBCConnHandle
//  PREFIX: odbch
// ---------------------------------------------------------------------------
class CIDDBASEEXP TODBCConnHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TODBCConnHandle();

        TODBCConnHandle(const TODBCConnHandle&) = delete;

        ~TODBCConnHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TODBCConnHandle& operator=(const TODBCConnHandle&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        const tCIDDBase::TConnHandle* pConnection() const
        {
            return m_pConn;
        }

        const TString& strDataSrc() const
        {
            return m_strDataSrc;
        }


    protected :
        // -------------------------------------------------------------------
        //  The connection class is our friend
        // -------------------------------------------------------------------
        friend class TDBConnection;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pConn
        //      The opaque per-platform structure for keeping track of a connection.
        //      The connection class below will create one of these and will set our
        //      members.
        //
        //  m_strDataSrc
        //      We store the data source name provided in the connect, if successful,
        //      for later error reporting purposes. If it was via connection string this
        //      is the resulting output connection string.
        // -------------------------------------------------------------------
        tCIDDBase::TConnHandle* m_pConn;
        TString                 m_strDataSrc;
};

namespace tCIDDBase
{
    using THConn = TCntPtr<TODBCConnHandle>;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBConnection
// PREFIX: dbcon
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBConnection : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TDBConnection();

        TDBConnection(const TDBConnection&) = delete;

        ~TDBConnection();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBConnection& operator=(const TDBConnection&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConnected() const;

        tCIDLib::TVoid Connect
        (
            const   TString&                strSrcName
        );

        tCIDLib::TVoid Connect
        (
            const   TString&                strSrcName
            , const TString&                strUserName
            , const TString&                strPassword
            , const tCIDLib::TBoolean       bUseOSAuth
        );

        tCIDLib::TVoid ConnectStr
        (
            const   TString&                strConnName
            , const TString&                strInConnStr
            ,       TString&                strOutConnStr
        );

        tCIDLib::TVoid Disconnect();

        const tCIDDBase::THConn& hConnection() const;

        tCIDDBase::THConn& hConnection();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckAlreadyConnected();

        tCIDLib::TVoid CheckNotConnected();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hConnection
        //      The handle to the connection. It's a counted pointer to a connection
        //      handle object that represents this connection. If we have never been
        //      connected it will show up as not valid.
        // -------------------------------------------------------------------
        tCIDDBase::THConn   m_hConnection;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBConnection,TObject)
};

#pragma CIDLIB_POPPACK


