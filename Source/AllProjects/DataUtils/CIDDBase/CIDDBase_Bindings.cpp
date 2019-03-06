//
// FILE NAME: CIDDBase_Bindings.cpp
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
//  This file implements a family of binding description objects
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
RTTIDecls(TDBBinding,TObject)
RTTIDecls(TDBBoolBinding,TDBBinding)
RTTIDecls(TDBCard1Binding,TDBBinding)
RTTIDecls(TDBCard2Binding,TDBBinding)
RTTIDecls(TDBCard4Binding,TDBBinding)
RTTIDecls(TDBCard8Binding,TDBBinding)
RTTIDecls(TDBDateBinding,TDBBinding)
RTTIDecls(TDBInt1Binding,TDBBinding)
RTTIDecls(TDBInt2Binding,TDBBinding)
RTTIDecls(TDBInt4Binding,TDBBinding)
RTTIDecls(TDBInt8Binding,TDBBinding)
RTTIDecls(TDBTimeBinding,TDBBinding)
RTTIDecls(TDBStrBinding,TDBBinding)


// ---------------------------------------------------------------------------
//  CLASS: TDBBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBBinding: Destructor
// ---------------------------------------------------------------------------
TDBBinding::~TDBBinding()
{
}


// ---------------------------------------------------------------------------
//  TDBBinding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBBinding::bIsNull() const
{
    return (m_iIndicator == SQL_NULL_DATA);
}


tCIDDBase::ESQLTypes TDBBinding::eType() const
{
    return m_eType;
}


const TString& TDBBinding::strName() const
{
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TDBBinding: Hidden constructors and operators
// ---------------------------------------------------------------------------
TDBBinding::TDBBinding( const   TString&                strName
                        , const tCIDDBase::ESQLTypes    eType
                        , const tCIDDBase::EParmDirs    eDir) :
    m_eDir(eDir)
    , m_eType(eType)
    , m_iIndicator(0)
    , m_strName(strName)
{
}

TDBBinding::TDBBinding(const TDBBinding& dbbindToCopy) :

    m_eDir(dbbindToCopy.m_eDir)
    , m_iIndicator(dbbindToCopy.m_iIndicator)
    , m_eType(dbbindToCopy.m_eType)
    , m_strName(dbbindToCopy.m_strName)
{
}

tCIDLib::TVoid TDBBinding::operator=(const TDBBinding& dbbindToAssign)
{
    if (this != &dbbindToAssign)
    {
        m_eDir      = dbbindToAssign.m_eDir;
        m_eType     = dbbindToAssign.m_eType;
        m_iIndicator= dbbindToAssign.m_iIndicator;
        m_strName   = dbbindToAssign.m_strName;
    }
}


// ---------------------------------------------------------------------------
//  TDBBinding: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBBinding::CheckBindResult(        TDBStatement&   dbstmtSrc
                            , const tCIDLib::TSInt  sToCheck)
{
    if ((sToCheck != SQL_SUCCESS_WITH_INFO) && (sToCheck != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
          SQL_HANDLE_STMT
          , dbstmtSrc.hStatement()
          , sToCheck
          , strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_BindFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , dbstmtSrc.strName()
            , strInfo
        );
    }
}


// Return the value in the indicator
tCIDDBase::TSQLIndicator TDBBinding::iIndicator() const
{
    return m_iIndicator;
}


// Give the derived classes access to the indicator for the bidning operationg
const tCIDDBase::TSQLIndicator* TDBBinding::piIndicator() const
{
    return &m_iIndicator;
}

tCIDDBase::TSQLIndicator* TDBBinding::piIndicator()
{
    return &m_iIndicator;
}





// ---------------------------------------------------------------------------
//  CLASS: TDBBoolBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBBoolBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBBoolBinding::TDBBoolBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c1Storage(0)
{
}

TDBBoolBinding::~TDBBoolBinding()
{
}


// ---------------------------------------------------------------------------
//  TDBBoolBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBBoolBinding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_UTINYINT
        , &m_c1Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBBoolBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_c1Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBBoolBinding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBBoolBinding::bValue() const
{
    return (m_c1Storage != 0);
}





// ---------------------------------------------------------------------------
//  CLASS: TDBCard1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard1Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard1Binding::TDBCard1Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c1Storage(0)
{
}

TDBCard1Binding::~TDBCard1Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard1Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_UTINYINT
        , &m_c1Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBCard1Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_c1Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBCard1Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1 TDBCard1Binding::c1Value() const
{
    return m_c1Storage;
}





// ---------------------------------------------------------------------------
//  CLASS: TDBCard2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard2Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard2Binding::TDBCard2Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c2Storage(0)
{
}

TDBCard2Binding::~TDBCard2Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard2Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_USHORT
        , &m_c2Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBCard2Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_c2Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBCard2Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TDBCard2Binding::c2Value() const
{
    return m_c2Storage;
}





// ---------------------------------------------------------------------------
//  CLASS: TDBCard4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard4Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard4Binding::TDBCard4Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c4Storage(0)
{
}

TDBCard4Binding::~TDBCard4Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard4Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_ULONG
        , &m_c4Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBCard4Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_c4Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TDBCard4Binding::c4Value() const
{
    return m_c4Storage;
}






// ---------------------------------------------------------------------------
//  CLASS: TDBCard8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard8Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard8Binding::TDBCard8Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c8Storage(0)
{
}

TDBCard8Binding::~TDBCard8Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard8Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_UBIGINT
        , &m_c8Storage
        , sizeof(m_c8Storage)
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBCard8Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_c8Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBCard8Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TDBCard8Binding::c8Value() const
{
    return m_c8Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBDateBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBDateBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBDateBinding::TDBDateBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(0)
{
    // Allocate the buffer
    m_pStorage = new DATE_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(DATE_STRUCT));
}

TDBDateBinding::TDBDateBinding( const   TString&                strName
                                , const TString&                strDefFormat
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(0)
{
    // Allocate the buffer
    m_pStorage = new DATE_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(DATE_STRUCT));

    // Set the given default format on the time object
    m_tmValue.strDefaultFormat(strDefFormat);
}

TDBDateBinding::~TDBDateBinding()
{
    // Clean up the storage
    delete [] m_pStorage;
}


// ---------------------------------------------------------------------------
//  TDBDateBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBDateBinding::BindCol(TDBStatement& dbstmtSrc, const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_TYPE_DATE
        , m_pStorage
        , sizeof(DATE_STRUCT)
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBDateBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(DATE_STRUCT));
}



// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TTime& TDBDateBinding::tmValue() const
{
    // Get the buffer value into the time object
    const DATE_STRUCT* pDate = reinterpret_cast<const DATE_STRUCT*>(m_pStorage);

    m_tmValue.FromDetails
    (
        pDate->year
        , tCIDLib::EMonths(pDate->month - 1)
        , pDate->day
    );
    return m_tmValue;
}





// ---------------------------------------------------------------------------
//  CLASS: TDBInt1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt1Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt1Binding::TDBInt1Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i1Storage(0)
{
}

TDBInt1Binding::~TDBInt1Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt1Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_TINYINT
        , &m_i1Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBInt1Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_i1Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBInt1Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt1 TDBInt1Binding::i1Value() const
{
    return m_i1Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBInt2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt2Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt2Binding::TDBInt2Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i2Storage(0)
{
}

TDBInt2Binding::~TDBInt2Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt2Binding::BindCol(       TDBStatement&    dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_SHORT
        , &m_i2Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBInt2Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_i2Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt2Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt2 TDBInt2Binding::i2Value() const
{
    return m_i2Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBInt4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt4Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt4Binding::TDBInt4Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i4Storage(0)
{
}

TDBInt4Binding::~TDBInt4Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt4Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_LONG
        , &m_i4Storage
        , 0
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBInt4Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_i4Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TDBInt4Binding::i4Value() const
{
    return m_i4Storage;
}





// ---------------------------------------------------------------------------
//  CLASS: TDBInt8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt8Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt8Binding::TDBInt8Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i8Storage(0)
{
}

TDBInt8Binding::~TDBInt8Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt8Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_SBIGINT
        , &m_i8Storage
        , sizeof(m_i8Storage)
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBInt8Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        m_i8Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt8Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt8 TDBInt8Binding::i8Value() const
{
    return m_i8Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBTimeBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBTimeBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBTimeBinding::TDBTimeBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(0)
{
    // Allocate the buffer
    m_pStorage = new TIME_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(TIME_STRUCT));
}

TDBTimeBinding::TDBTimeBinding( const   TString&                strName
                                , const TString&                strDefFormat
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(0)
{
    // Allocate the buffer
    m_pStorage = new TIME_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(TIME_STRUCT));

    // Set the given default format on the time object
    m_tmValue.strDefaultFormat(strDefFormat);
}

TDBTimeBinding::~TDBTimeBinding()
{
    // Clean up the storage
    delete [] m_pStorage;
}


// ---------------------------------------------------------------------------
//  TDBTimeBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBTimeBinding::BindCol(TDBStatement& dbstmtSrc, const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_TYPE_TIME
        , m_pStorage
        , sizeof(TIME_STRUCT)
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBTimeBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (iIndicator() == SQL_NULL_DATA)
        TRawMem::SetMemBuf(m_pStorage, tCIDLib::TCard1(0), sizeof(TIME_STRUCT));
}


// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TTime& TDBTimeBinding::tmValue() const
{
    // Get the buffer value into the time object
    const TIME_STRUCT* pTime = reinterpret_cast<const TIME_STRUCT*>(m_pStorage);
    m_tmValue.FromTimeDetails(pTime->hour, pTime->minute, pTime->second, 0);
    return m_tmValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBStrBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBStrBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBStrBinding::TDBStrBinding(const  TString&                strName
                            , const tCIDLib::TCard4         c4MaxChars
                            , const tCIDDBase::ESQLTypes    eType
                            , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c4MaxChars(c4MaxChars)
    , m_pchStorage(0)
{
    // Make sure the max chars isn't zero
    if (!m_c4MaxChars)
        m_c4MaxChars = 8;

    // Allocate the buffer
    m_pchStorage = new tCIDLib::TCh[c4MaxChars + 1];
    m_pchStorage[0] = kCIDLib::chNull;
}

TDBStrBinding::~TDBStrBinding()
{
    // Clean up the storage
    delete [] m_pchStorage;
}


// ---------------------------------------------------------------------------
//  TDBStrBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBStrBinding::BindCol(         TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.hStatement()
        , c2ParmInd
        , SQL_C_WCHAR
        , m_pchStorage
        , (m_c4MaxChars + 1) * sizeof(tCIDLib::TCh)
        , piIndicator()
    );
    CheckBindResult(dbstmtSrc, RetVal);
}


tCIDLib::TVoid TDBStrBinding::PostFetch()
{
    // Store out data to the string member, or clear if it null data
    if (iIndicator() == SQL_NULL_DATA)
    {
        m_strValue.Clear();
    }
     else
    {
        // Get the buffer value into the string
        m_strValue = m_pchStorage;
        m_strValue.StripWhitespace();
    }
}



// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TDBStrBinding::strValue() const
{
    return m_strValue;
}


