//
// FILE NAME: CIDDBase_DirectStatement.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDDBase_DirectStatement.Cpp file. This
//  file implements a derivative of the base statement class, which in this
//  case allows for direct execution.
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
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBDirStatement,TDBStatement)
};

#pragma CIDLIB_POPPACK



