//
// FILE NAME: CIDLib_CollectCursor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/23/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the bit of out of line stuff that collection cursors
//  have. The most base collection cursor class is non-templatized, so we can
//  get some stuff out of the header.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCursorBase,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCursorBase
//  PREFIX: curs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCursorBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TCursorBase::~TCursorBase()
{
}


// ---------------------------------------------------------------------------
//  TCursorBase: Hidden constructors and operators
// ---------------------------------------------------------------------------
TCursorBase::TCursorBase() :

    m_c4SerialNum(0)
{
}

TCursorBase::TCursorBase(const tCIDLib::TCard4 c4SerialNum) :

    m_c4SerialNum(c4SerialNum)
{
}

TCursorBase::TCursorBase(const TCursorBase& cursSrc) :

    m_c4SerialNum(cursSrc.m_c4SerialNum)
{
}

TCursorBase& TCursorBase::operator=(const TCursorBase& cursSrc)
{
    if (&cursSrc != this)
        m_c4SerialNum   = cursSrc.m_c4SerialNum;
    return *this;
}



// ---------------------------------------------------------------------------
//  TCursorBase: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCursorBase::operator==(const TCursorBase& cursSrc) const
{
    if (&cursSrc == this)
        return kCIDLib::True;
    return (m_c4SerialNum == cursSrc.m_c4SerialNum);
}

tCIDLib::TBoolean TCursorBase::operator!=(const TCursorBase& cursSrc) const
{
    return !TCursorBase::operator==(cursSrc);
}


// ---------------------------------------------------------------------------
//  TCursorBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCursorBase::CheckSerialNum(const   tCIDLib::TCard4     c4ToCheck
                            , const tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    if (c4ToCheck != m_c4SerialNum)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_StaleCursor
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , clsIsA()
        );
    }
}


tCIDLib::TVoid
TCursorBase::CheckValid(const   tCIDLib::TVoid* const pcursTest
                        , const tCIDLib::TCh* const     pszFile
                        , const tCIDLib::TCard4         c4Line) const
{
    if (pcursTest == nullptr)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_CursorNotValid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutOfSync
            , clsIsA()
        );
    }
}

tCIDLib::TVoid
TCursorBase::CheckValid(const   tCIDLib::TBoolean       bState
                        , const tCIDLib::TCh* const     pszFile
                        , const tCIDLib::TCard4         c4Line) const
{
    if (!bState)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_CursorNotValid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutOfSync
            , clsIsA()
        );
    }
}

tCIDLib::TCard4 TCursorBase::c4SerialNum(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SerialNum = c4ToSet;
    return c4ToSet;
}


tCIDLib::TVoid
TCursorBase::ThrowNotInitialized(const  tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NotReady
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppError
        , clsIsA()
    );
}
