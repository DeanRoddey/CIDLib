//
// FILE NAME: CIDDBase_Bindings_Win32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/01/2019
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
//  This file implements the Win32 specific versions of all of the database binding
//  classes.
//
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


// We don't need to use this guy for Windows so just define as an empty struct
struct tCIDDBase::TBindingInfo
{
};


// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------
tCIDLib::TVoid
CheckBindResult(        TDBStatement&   dbstmtSrc
                , const TString&        strBindingName
                , const SQLRETURN       ToCheck)
{
    if ((ToCheck != SQL_SUCCESS_WITH_INFO) && (ToCheck != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, dbstmtSrc.pStatement()->hStatement, ToCheck, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_BindFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strBindingName
            , dbstmtSrc.strName()
            , strInfo
        );
    }
}



// ---------------------------------------------------------------------------
//  TDBBinding: protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The platform independent code calls us here to let us allocate and free our per
//  platform info structure. In our case we don't use it, but we still have to go
//  through the motions.
//
tCIDDBase::TBindingInfo* TDBBinding::pAllocBindingInfo()
{
    return new tCIDDBase::TBindingInfo;
}

tCIDLib::TVoid TDBBinding::FreeBindingInfo(tCIDDBase::TBindingInfo* pToFree)
{
    delete pToFree;
}



// ---------------------------------------------------------------------------
//  TDBBoolBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBBoolBinding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_UTINYINT
        , &m_c1Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(), RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBCard1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard1Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_UTINYINT
        , &m_c1Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(), RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBCard2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard2Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_USHORT
        , &m_c2Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard4Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_ULONG
        , &m_c4Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBCard8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBCard8Binding::BindCol(       TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_UBIGINT
        , &m_c8Storage
        , sizeof(m_c8Storage)
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}



// ---------------------------------------------------------------------------
//  TDBDateBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBDateBinding::BindCol(TDBStatement& dbstmtSrc, const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_TYPE_DATE
        , m_pStorage
        , sizeof(DATE_STRUCT)
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


TTime TDBDateBinding::tmValue() const
{
    // Get the value into a time object
    const DATE_STRUCT* pDate = reinterpret_cast<const DATE_STRUCT*>(m_pStorage);
    TTime tmRet;
    tmRet.FromDetails(pDate->year, tCIDLib::EMonths(pDate->month - 1), pDate->day);
    if (!m_strDefFormat.bIsEmpty())
        tmRet.strDefaultFormat(m_strDefFormat);
    return tmRet;
}


// ---------------------------------------------------------------------------
//  TDBDateBinding: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBDateBinding::DeleteStorage()
{
    delete [] m_pStorage;
    m_pStorage = nullptr;
}

tCIDLib::TVoid TDBDateBinding::InitStorage()
{
    m_pStorage = new DATE_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, kCIDLib::c1MinCard, sizeof(DATE_STRUCT));
}

tCIDLib::TVoid TDBDateBinding::NullStorage()
{
    TRawMem::SetMemBuf(m_pStorage, kCIDLib::c1MinCard, sizeof(DATE_STRUCT));
}



// ---------------------------------------------------------------------------
//  TDBInt1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt1Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_TINYINT
        , &m_i1Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}



// ---------------------------------------------------------------------------
//  TDBInt2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt2Binding::BindCol(       TDBStatement&    dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_SHORT
        , &m_i2Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}



// ---------------------------------------------------------------------------
//  TDBInt4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt4Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_LONG
        , &m_i4Storage
        , 0
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}



// ---------------------------------------------------------------------------
//  TDBInt8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBInt8Binding::BindCol(        TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_SBIGINT
        , &m_i8Storage
        , sizeof(m_i8Storage)
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}



// ---------------------------------------------------------------------------
//  TDBStrBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBStrBinding::BindCol(         TDBStatement&   dbstmtSrc
                        , const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_WCHAR
        , m_pStorage
        , (m_c4MaxChars + 1) * sizeof(tCIDLib::TCh)
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBStrBinding: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBStrBinding::DeleteStorage()
{
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pStorage);
}

tCIDLib::TVoid TDBStrBinding::NullStorage()
{
    *reinterpret_cast<tCIDLib::TCh*>(m_pStorage) = kCIDLib::chNull;
}

tCIDLib::TVoid TDBStrBinding::InitStorage()
{
    // It's just wide chars for us, an extra for a null just in case
    m_pStorage = new tCIDLib::TCh[m_c4MaxChars + 1];
}

tCIDLib::TVoid TDBStrBinding::StorageToStr()
{
    m_strValue = reinterpret_cast<tCIDLib::TCh*>(m_pStorage);
    m_strValue.StripWhitespace();
}



// ---------------------------------------------------------------------------
//  TDBTimeBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDBTimeBinding::BindCol(TDBStatement& dbstmtSrc, const tCIDLib::TCard2 c2ParmInd)
{
    SQLLEN Indicator = 0;
    const SQLRETURN RetVal = ::SQLBindCol
    (
        dbstmtSrc.pStatement()->hStatement
        , c2ParmInd
        , SQL_C_TYPE_TIME
        , m_pStorage
        , sizeof(TIME_STRUCT)
        , &Indicator
    );
    CheckBindResult(dbstmtSrc, strName(),  RetVal);
    SetNull(Indicator == SQL_NULL_DATA);
}


// ---------------------------------------------------------------------------
//  TDBTimeBinding: Public, non-virtual methods
// ---------------------------------------------------------------------------
TTime TDBTimeBinding::tmValue() const
{
    // Get the buffer value into the time object
    const TIME_STRUCT* pTime = reinterpret_cast<const TIME_STRUCT*>(m_pStorage);
    TTime tmRet;
    tmRet.FromTimeDetails(pTime->hour, pTime->minute, pTime->second, 0);
    if (!m_strDefFormat.bIsEmpty())
        tmRet.strDefaultFormat(m_strDefFormat);
    return tmRet;
}


// ---------------------------------------------------------------------------
//  TDBTimeBinding: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBTimeBinding::DeleteStorage()
{
    delete [] m_pStorage;
    m_pStorage = nullptr;
}

tCIDLib::TVoid TDBTimeBinding::InitStorage()
{
    m_pStorage = new TIME_STRUCT;
    TRawMem::SetMemBuf(m_pStorage, kCIDLib::c1MinCard, sizeof(TIME_STRUCT));
}

tCIDLib::TVoid TDBTimeBinding::NullStorage()
{
    TRawMem::SetMemBuf(m_pStorage, kCIDLib::c1MinCard, sizeof(TIME_STRUCT));
}

