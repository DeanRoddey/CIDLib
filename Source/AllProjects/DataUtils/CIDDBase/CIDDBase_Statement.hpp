//
// FILE NAME: CIDDBase_Statement.hpp
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
//  This is the header file for the CIDDBase_Statement.Cpp file. This file
//  implements a database query statement.
//
//  The statement requires a connection (actually the connection handle) which
//  it will keep around (it's a counted object) until it is destructed so that
//  the connection can't go away on us.
//
//  Though there are no virtual methods, this is just a base class so the ctors
//  are protected. It handles the grunt work but we may want to have multiple
//  specialized types of statements.
//
//  One you have done a successful query, you can bFetch() to load each successive
//  row into the column bindings. Do this until bFetch() returns false to see them
//  all.
//
//
//  As mentioned you actually used derived statement classes to do the queries.
//  Those classes, after a successful query, should call DoColBindings() to set
//  up the binding objects that have been set on this statement with the columns
//  in the query results.
//
//  They should call DoParmBindings() before the query, though as mentioned in the
//  gotchas below, parameter bindings aren't yet actually supported.
//
// CAVEATS/GOTCHAS:
//
//  1)  Though the interfaces are in place to do parameter bindings, they are not
//      yet implemented. Only column result bindings are actually supported at
//      this time.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TDBStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBStatement : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TDBStatement() = delete;

        TDBStatement(const TDBStatement&) = delete;

        ~TDBStatement();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBStatement& operator=(const TDBStatement&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddColBinding
        (
            const   TString&                strName
            , const tCIDDBase::ECppTypes    eCppType
        );

        tCIDLib::TVoid AddColDateBinding
        (
            const   TString&                strName
            , const TString&                strFormat
        );

        tCIDLib::TVoid AddColStrBinding
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4MaxChars
        );

        tCIDLib::TVoid AddParmBinding
        (
            const   TString&                strName
            , const tCIDDBase::ECppTypes    eCppType
            , const tCIDDBase::ESQLTypes    eSQLType
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        tCIDLib::TBoolean bColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TBoolean bFetch();

        tCIDLib::TBoolean bIsColNull
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TCard1 c1ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TCard2 c2ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TCard4 c4ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TCard8 c8ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TInt1 i1ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TInt2 i2ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TInt4 i4ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TInt8 i8ColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDDBase::TStmtHandle* pStatement() const
        {
            return m_pStatement;
        }

        tCIDLib::TVoid Reset();

        const TString& strColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        const TString& strName() const;

        TTime tmDateColVal
        (
            const   tCIDLib::TCard4         c4Index
        );

        TTime tmTimeColVal
        (
            const   tCIDLib::TCard4         c4Index
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TDBStatement
        (
            const   TDBConnection&          dbconnSrc
            , const TString&                strName
        );

        TDBStatement
        (
            const   tCIDDBase::THConn&      hConnection
            , const TString&                strName
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoColBindings();

        tCIDLib::TVoid DoParmBindings();


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TBindingList = TRefVector<TDBBinding>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDoPlatFetch();

        tCIDLib::TVoid ClearCursor();

        tCIDLib::TVoid ClearHandleColBindings();

        tCIDLib::TVoid ClearHandleParmBindings();

        tCIDLib::TVoid CreateStatement
        (
            const   tCIDDBase::TConnHandle* pConnection
        );

        tCIDLib::TVoid DestroyStatement
        (
            const   tCIDDBase::TConnHandle* pConnection
        );

        [[nodiscard]] TDBBinding* pdbbindMakeNew
        (
            const   TString&                strName
            , const tCIDDBase::ECppTypes    eCppType
            , const tCIDDBase::ESQLTypes    eSQLType
            , const tCIDDBase::EParmDirs    eDir
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colColBindings
        //  m_colParmBindings
        //      The bindings currently associated with this statement. We have
        //      one list of parameter and one list of column bindings.
        //
        //  m_hConnection
        //      We have to insure the connection that created us cannot go away until
        //      we are done, so we keep a (ref counted) copy of it.
        //
        //  m_pStatement
        //      The opaque 'handle' to a statement that each platform driver defines
        //      for its own needs.
        //
        //  m_strName
        //      Statement name, for error reporting purposes.
        // -------------------------------------------------------------------
        TBindingList                m_colColBindings;
        TBindingList                m_colParmBindings;
        tCIDDBase::THConn           m_hConnection;
        tCIDDBase::TStmtHandle*     m_pStatement;
        TString                     m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBStatement,TObject)
};

#pragma CIDLIB_POPPACK


