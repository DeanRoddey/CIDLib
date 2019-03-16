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
//  This is the header file for the CIDDBase_TransJanitor.Cpp file. This
//  file implements a transaction janitor that can be drive by C++ scopes.
//  The ctor invokes a transaction and, unless it is told otherwise by a
//  call to it's commit method, will roll back the transaction at the end
//  of the C++ scope it is in.
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
                    TDBConnection* const    pdbconToUse
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
        //  Private data types
        //
        //  m_bCommited
        //      The Commit method has been called, and we've commited, and
        //      will invoke a rollback on destruction.
        //
        //  m_pdbconToUse
        //      The database connection to use for our statement invocations.
        //      We don't adopt, just use. But it means that we have to destruct
        //      before the connection object does.
        //
        //  m_strName
        //      A name given in the ctor that we can use in error messages.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bCommited;
        TDBConnection*      m_pdbconToUse;
        TString             m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBTransJan,TObject)
};


#pragma CIDLIB_POPPACK


