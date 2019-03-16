//
// FILE NAME: CIDDBase_Connection.hpp
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
//  This is the header file for the CIDDBase_Connection.Cpp file. This
//  file implements a database connection.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TODBCConnHandle
//  PREFIX: odbch
//
//  We need a small class that will be used to reference count connection
//  handles. A connection handle cannot be destroyed until all the statements
//  that use it are destroyed. Since we cannot enforce order of destruction
//  of objects. we just internally reference count the connection handle, so
//  that the connection object can be destroyed at any time after the desired
//  statements have been created.
//
//  We use thse via a counted pointer object to get the reference counting.
// ---------------------------------------------------------------------------
class CIDDBASEEXP TODBCConnHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TODBCConnHandle();

        TODBCConnHandle
        (
            const   tCIDDBase::THandle      hConn
            , const TString&                strSrcName
        );

        TODBCConnHandle(const TODBCConnHandle&) = delete;

        ~TODBCConnHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TODBCConnHandle& operator=(const TODBCConnHandle&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDDBase::THandle hConn() const;

        const TString& strDataSrc() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hConn
        //      The handle to the connection. It's opaque to the outside
        //      world.
        //
        //  m_strDataSrc
        //      We store the data source name provided in the connect, if
        //      successful, for later error reporting purposes.
        // -------------------------------------------------------------------
        tCIDDBase::THandle  m_hConn;
        TString             m_strDataSrc;
};

namespace tCIDDBase
{
    typedef TCntPtr<TODBCConnHandle>    TODBCHandleRef;
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
            , const tCIDLib::TBoolean       bUseWinAuth
        );

        tCIDLib::TVoid ConnectStr
        (
            const   TString&                strConnName
            , const TString&                strInConnStr
            ,       TString&                strOutConnStr
        );

        tCIDLib::TVoid Disconnect();

        const tCIDDBase::TODBCHandleRef& cptrHandle() const;

        tCIDDBase::TODBCHandleRef& cptrHandle();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckAlreadyConnected();

        tCIDLib::TVoid CheckNotConnected();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrHandle
        //      The handle to the connection. It's a counted pointer to a
        //      connection handle object
        // -------------------------------------------------------------------
        tCIDDBase::TODBCHandleRef   m_cptrHandle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBConnection,TObject)
};

#pragma CIDLIB_POPPACK


