//
// FILE NAME: CIDDBase_TransJanitor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/2003
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
//  This is the header file for the CIDDBase_TransJanitor.Cpp file. This file a
//  janitorclass  that can be used to do transactions on a C++ scoped basis.
//
//  The ctor begins a transaction. If Commit() is not called before the object goes
//  out of scope, it will do a rollback when its dtor is invoked. If the ctor cannot
//  being the transaction it throws. If the dtor cannot roll back it will log an
//  error.
//
//  We copy the (ref counted) connection handle so that it can't go away before
//  we do. Not that it likely could anyway given the scoped nature of this janitor
//  type class.
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
//  CLASS: TDBTransJan
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBTransJan : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBTransJan
        (
                    TDBConnection&          dbconnTar
            , const TString&                strName
        );

        TDBTransJan
        (
                    tCIDDBase::THConn&      hConnection
            , const TString&                strName
        );

        TDBTransJan(const TDBTransJan&) = delete;

        ~TDBTransJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBTransJan& operator=(const TDBTransJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Commit();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StartTrans();


        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_bCommited
        //      The Commit method has been called. So we have committed and will not
        //      do a rollback on dtor.
        //
        //  m_hConnection
        //      The database connection handle to use for our statement invocations.
        //      This is a counted pointer so it will stay alive long enough for us
        //      to do our thing at least.
        //
        //  m_strName
        //      A name given in the ctor that we can use in error messages.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bCommited;
        tCIDDBase::THConn   m_hConnection;
        TString             m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBTransJan,TObject)
};


#pragma CIDLIB_POPPACK


