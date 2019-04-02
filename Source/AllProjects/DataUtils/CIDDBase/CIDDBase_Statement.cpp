//
// FILE NAME: CIDDBase_Statement.cpp
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
//  This file implements the query statement class.
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
#include    "CIDDBase_Win32.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDBStatement,TObject)


//
//  A templatized method to handle the casting of a binding object to it's
//  actual type and throwing it if it not a valid cast.
//
template <class T> T*
pdbbindCheckType(TDBBinding* pdbbindTest, const TString& strStmtName)
{
    // See if it's of the right type. If not, throw
    T* pdbbindRet = dynamic_cast<T*>(pdbbindTest);
    if (!pdbbindRet)
    {
        facCIDDBase().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_BadColBindAccess
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , pdbbindTest->strName()
            , strStmtName
            , T::clsThis()
            , pdbbindTest->clsIsA()
        );
    }
    return pdbbindRet;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBStatement: Destructor
// ---------------------------------------------------------------------------

//
//  This will also deref our connection handle when we exit this scope. If we are
//  the last one holding onto it, it will be cleaned up.
//
TDBStatement::~TDBStatement()
{
    // Make sure the statement is cleaned up
    DestroyStatement(m_hConnection->pConnection());
}


// ---------------------------------------------------------------------------
//  TDBStatement: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBStatement::AddColBinding(const   TString&                strName
                            , const tCIDDBase::ECppTypes    eCppType)
{
    //
    //  Just call a common private that we share with the parm binding
    //  stuff, since they are pretty similar. It will return us a constructed
    //  binding object, and we can add it to our list. In our case, we set
    //  the direction to In and the SQLType to None, but they don't matter
    //  for a column binding.
    //
    TDBBinding* pdbbindNew = pdbbindMakeNew
    (
        strName, eCppType, tCIDDBase::ESQLTypes::None, tCIDDBase::EParmDirs::In
    );
    m_colColBindings.Add(pdbbindNew);
}


tCIDLib::TVoid
TDBStatement::AddColDateBinding(const   TString&    strName
                                , const TString&    strFmt)
{
    //
    //  This one we'll do ourself, since we take a format parameter. They
    //  can use the generic add above as well, but will get the default
    //  format.
    //
    m_colColBindings.Add
    (
        new TDBDateBinding
        (
            strName, strFmt, tCIDDBase::ESQLTypes::None, tCIDDBase::EParmDirs::In
        )
    );
}


tCIDLib::TVoid
TDBStatement::AddColStrBinding( const   TString&        strName
                                , const tCIDLib::TCard4 c4Max)
{
    //
    //  This one we'll do ourself, since it needs a max char parm. In our
    //  case, we set the direction to In and the SQLType to None, but they
    //  don't matter for a column binding.
    //
    m_colColBindings.Add
    (
        new TDBStrBinding
        (
            strName, c4Max, tCIDDBase::ESQLTypes::None, tCIDDBase::EParmDirs::In
        )
    );
}


tCIDLib::TVoid
TDBStatement::AddParmBinding(const  TString&                strName
                            , const tCIDDBase::ECppTypes    eCppType
                            , const tCIDDBase::ESQLTypes    eSQLType
                            , const tCIDDBase::EParmDirs    eDir)
{
    // <TBD>
}


tCIDLib::TBoolean TDBStatement::bColVal(const tCIDLib::TCard4 c4Index)
{
    TDBBoolBinding* pdbbindRet = pdbbindCheckType<TDBBoolBinding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->bValue();
}


tCIDLib::TBoolean TDBStatement::bFetch()
{
    // Do the platform specific fetch. If no rows, return false now
    if (!bDoPlatFetch())
        return kCIDLib::False;

    // Give each binding a chance to swizzle data
    const tCIDLib::TCard4 c4BindCnt = m_colColBindings.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BindCnt; c4Index++)
        m_colColBindings[c4Index]->PostFetch();

    return kCIDLib::True;
}


//
//  Returns whether the last value fetched for the indicated column was
//  null or not.
//
tCIDLib::TBoolean TDBStatement::bIsColNull(const tCIDLib::TCard4 c4Index)
{
    return m_colColBindings[c4Index]->bIsNull();
}


tCIDLib::TCard1 TDBStatement::c1ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBCard1Binding* pdbbindRet = pdbbindCheckType<TDBCard1Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->c1Value();
}


tCIDLib::TCard2 TDBStatement::c2ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBCard2Binding* pdbbindRet = pdbbindCheckType<TDBCard2Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->c2Value();
}


tCIDLib::TCard4 TDBStatement::c4ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBCard4Binding* pdbbindRet = pdbbindCheckType<TDBCard4Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->c4Value();
}


tCIDLib::TCard8 TDBStatement::c8ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBCard8Binding* pdbbindRet = pdbbindCheckType<TDBCard8Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->c8Value();
}


tCIDLib::TInt1 TDBStatement::i1ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBInt1Binding* pdbbindRet = pdbbindCheckType<TDBInt1Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->i1Value();
}


tCIDLib::TInt2 TDBStatement::i2ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBInt2Binding* pdbbindRet = pdbbindCheckType<TDBInt2Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->i2Value();
}


tCIDLib::TInt4 TDBStatement::i4ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBInt4Binding* pdbbindRet = pdbbindCheckType<TDBInt4Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->i4Value();
}


tCIDLib::TInt8 TDBStatement::i8ColVal(const tCIDLib::TCard4 c4Index)
{
    TDBInt8Binding* pdbbindRet = pdbbindCheckType<TDBInt8Binding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->i8Value();
}


tCIDLib::TVoid TDBStatement::Reset()
{
    // Clear our lists first
    m_colParmBindings.RemoveAll();
    m_colColBindings.RemoveAll();

    // And clear the handle's lists
    ClearHandleParmBindings();
    ClearHandleColBindings();

    // And clean up the cursor if any
    ClearCursor();
}


const TString& TDBStatement::strColVal(const tCIDLib::TCard4 c4Index)
{
    TDBStrBinding* pdbbindRet = pdbbindCheckType<TDBStrBinding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->strValue();
}


const TString& TDBStatement::strName() const
{
    return m_strName;
}


TTime TDBStatement::tmDateColVal(const tCIDLib::TCard4 c4Index)
{
    TDBDateBinding* pdbbindRet = pdbbindCheckType<TDBDateBinding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->tmValue();
}


TTime TDBStatement::tmTimeColVal(const tCIDLib::TCard4 c4Index)
{
    TDBTimeBinding* pdbbindRet = pdbbindCheckType<TDBTimeBinding>
    (
        m_colColBindings[c4Index], m_strName
    );
    return pdbbindRet->tmValue();
}


// ---------------------------------------------------------------------------
//  TDBStatement: Hidden Constructors
// ---------------------------------------------------------------------------
TDBStatement::TDBStatement( const   TDBConnection&  dbconTarget
                            , const TString&        strName) :

    m_colColBindings(tCIDLib::EAdoptOpts::Adopt)
    , m_colParmBindings(tCIDLib::EAdoptOpts::Adopt)
    , m_pStatement(nullptr)
    , m_strName(strName)
{
    // Let the platform create the per-platform statement
    CreateStatement(dbconTarget.hConnection()->pConnection());
    CIDAssert(m_pStatement != nullptr, L"The per-platform DB statement handle wasn't set");

    // It worked, so ref count the connection handle
    m_hConnection = dbconTarget.hConnection();
}

TDBStatement::TDBStatement( const   tCIDDBase::THConn&  hConnection
                            , const TString&            strName) :

    m_colColBindings(tCIDLib::EAdoptOpts::Adopt)
    , m_colParmBindings(tCIDLib::EAdoptOpts::Adopt)
    , m_pStatement(nullptr)
    , m_strName(strName)
{
    // Let the platform create the per-platform statement
    CreateStatement(hConnection->pConnection());
    CIDAssert(m_pStatement != nullptr, L"The per-platform DB statement handle wasn't set");

    // It worked, so ref count the connection handle
    m_hConnection = hConnection;
}


// ---------------------------------------------------------------------------
//  TDBStatement: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBStatement::DoColBindings()
{
    //
    //  Remove any current bindings form the handle, then run through our
    //  list and set up bindings for any column bindings we have.
    //
    ClearHandleColBindings();

    const tCIDLib::TCard4 c4Count = m_colColBindings.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // NOTE: ODBC bindings are 1 based, so we add one to our index!
        TDBBinding* pdbbindCur = m_colColBindings[c4Index];
        pdbbindCur->BindCol(*this, tCIDLib::TCard2(c4Index + 1));
    }
}


tCIDLib::TVoid TDBStatement::DoParmBindings()
{
}


// ---------------------------------------------------------------------------
//  TDBStatement: Private, non-virtual methods
// ---------------------------------------------------------------------------
TDBBinding*
TDBStatement::pdbbindMakeNew(const  TString&                strName
                            , const tCIDDBase::ECppTypes    eCppType
                            , const tCIDDBase::ESQLTypes    eSQLType
                            , const tCIDDBase::EParmDirs    eDir)
{
    //
    //  Create an object of the correct CPP type, which provides the
    //  storage for the binding.
    //
    TDBBinding* pdbbindRet = 0;
    switch(eCppType)
    {
        case tCIDDBase::ECppTypes::Boolean :
            pdbbindRet = new TDBBoolBinding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Card1 :
            pdbbindRet = new TDBCard1Binding(strName, eSQLType, eDir);

            break;

        case tCIDDBase::ECppTypes::Card2 :
            pdbbindRet = new TDBCard2Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Card4 :
            pdbbindRet = new TDBCard4Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Card8 :
            pdbbindRet = new TDBCard8Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Date :
            pdbbindRet = new TDBDateBinding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Int1 :
            pdbbindRet = new TDBInt1Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Int2 :
            pdbbindRet = new TDBInt2Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Int4 :
            pdbbindRet = new TDBInt4Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Int8 :
            pdbbindRet = new TDBInt8Binding(strName, eSQLType, eDir);
            break;

        case tCIDDBase::ECppTypes::Time :
            pdbbindRet = new TDBTimeBinding(strName, eSQLType, eDir);
            break;

        default :
            // <TBD> Throw here
            break;
    };

    return pdbbindRet;
}


