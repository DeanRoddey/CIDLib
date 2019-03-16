//
// FILE NAME: CIDDBase_Statement.hpp
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
//  This is the header file for the CIDDBase_Statement.Cpp file. This
//  file implements a query statement.
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
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TCard2 c2ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TCard4 c4ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TCard8 c8ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDDBase::THandle hStatement() const;

        tCIDLib::TInt1 i1ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TInt2 i2ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TInt4 i4ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TInt8 i8ColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        tCIDLib::TVoid Reset();

        const TString& strColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        const TString& strName() const;

        const TTime& tmDateColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );

        const TTime& tmTimeColVal
        (
            const   tCIDLib::TCard4         c4ParmInd
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructor
        // -------------------------------------------------------------------
        TDBStatement
        (
            const   TDBConnection&          dbconTarget
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
        typedef TRefVector<TDBBinding>   TBindingList;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ClearCursor();

        tCIDLib::TVoid ClearHandleColBindings();

        tCIDLib::TVoid ClearHandleParmBindings();

        TDBBinding* pdbbindMakeNew
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
        //  m_cptrConn
        //      We participate in referencing counting the connection handle
        //      that was used to create us. They cannot be destroyed until
        //      all statement objects that were created with it are destroyed.
        //      So when we are created, we ref count the handle, as does the
        //      connection object. When we all let it go, it's cleaned up.
        //
        //  m_hStatement
        //      The handle to the statement. It's opaque to the outside
        //      world.
        //
        //  m_strName
        //      Statement name, for error reporting purposes.
        // -------------------------------------------------------------------
        TBindingList                m_colColBindings;
        TBindingList                m_colParmBindings;
        tCIDDBase::TODBCHandleRef   m_cptrConn;
        tCIDDBase::THandle          m_hStatement;
        TString                     m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBStatement,TObject)
};

#pragma CIDLIB_POPPACK


