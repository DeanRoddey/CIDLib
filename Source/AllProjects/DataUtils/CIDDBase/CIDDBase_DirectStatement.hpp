//
// FILE NAME: CIDDBase_DirectStatement.hpp
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
//  This is the header file for the CIDDBase_DirectStatement.Cpp file. This
//  file implements a derivative of the base statement class, which in this
//  case allows for direct execution of a SQL statement string.
//
//  This guy has some platform specific code to do the actual execution, but
//  the vast majority of the work is handled by the base statement class.
//
//  The base class reference counts the connection handle which it needs to
//  insure stays alive for the life of this statement. You can pass the actual
//  connection object but it just ref counts the connection's handle object.
//  The connection object itself can be discarded after setting up any needed
//  statements.
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
//  CLASS: TDBDirStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBDirStatement : public TDBStatement
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBDirStatement() = delete;

        TDBDirStatement
        (
            const   TDBConnection&          dbconTarget
            , const TString&                strName
        );

        TDBDirStatement
        (
            const   tCIDDBase::THConn&      hConnection
            , const TString&                strName
        );

        TDBDirStatement(const TDBDirStatement&) = delete;

        ~TDBDirStatement();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBDirStatement& operator=(const TDBDirStatement&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Execute
        (
            const   TString&                strStatement
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoPlatExecute
        (
            const   TString&                strStatement
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBDirStatement,TDBStatement)
};

#pragma CIDLIB_POPPACK



