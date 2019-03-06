//
// FILE NAME: CIDLib_Semaphore.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TSemaphore class, which is a standard
//  counting semaphore.
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
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TSemaphore,TObject)
RTTIDecls(TSemJanitor,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TSemaphore
//  PREFIX: sem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSemaphore: Constructors and Destructor
// ---------------------------------------------------------------------------
TSemaphore::TSemaphore( const   tCIDLib::TCard4 c4InitCount
                        , const tCIDLib::TCard4 c4MaxCount) :

    m_bNamed(kCIDLib::False)
    , m_ksemImpl(c4MaxCount)
{
    if (!m_ksemImpl.bCreate(c4InitCount))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSem_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
        );
    }
}

TSemaphore::TSemaphore( const   TResourceName&          rsnToUse
                        , const tCIDLib::TCard4         c4InitCount
                        , const tCIDLib::TCard4         c4MaxCount
                        , const tCIDLib::ECreateActs eAction) :

    m_bNamed(kCIDLib::True)
    , m_ksemImpl
      (
        rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Semaphore).pszBuffer()
        , c4MaxCount
      )
{
    tCIDLib::TBoolean bDummy;
    InitNamed(bDummy, c4InitCount, eAction);
}

TSemaphore::TSemaphore( const   TResourceName&          rsnToUse
                        ,       tCIDLib::TBoolean&      bCreated
                        , const tCIDLib::TCard4         c4InitCount
                        , const tCIDLib::TCard4         c4MaxCount
                        , const tCIDLib::ECreateActs eAction) :

    m_bNamed(kCIDLib::True)
    , m_ksemImpl
      (
        rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Semaphore).pszBuffer()
        , c4MaxCount
      )
{
    InitNamed(bCreated, c4InitCount, eAction);
}

TSemaphore::~TSemaphore()
{
    if (!m_ksemImpl.bClose())
    {
        //
        //  If we get an error, log it but don't throw out of the destructor
        //  so we use a warning message.
        //
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSem_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
        );
    }
}


// ---------------------------------------------------------------------------
//  TSemaphore: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSemaphore::bIsNamed() const
{
    return m_bNamed;
}


tCIDLib::TCard4 TSemaphore::c4MaxCount() const
{
    return m_ksemImpl.c4MaxCount();
}


tCIDLib::TVoid TSemaphore::Enter(const tCIDLib::TCard4 c4Timeout)
{
    if (!m_ksemImpl.bEnter(c4Timeout))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();

        // Handle timeout specially, otherwise generic enter error
        tCIDLib::TErrCode errcLog = kCIDErrs::errcSem_Enter;
        if (kerrRes.errcId() == kKrnlErrs::errcGen_Timeout)
            errcLog = kCIDErrs::errcSem_Timeout;

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcLog
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
        );
    }
}


tCIDLib::TVoid TSemaphore::Exit()
{
    if (!m_ksemImpl.bExit())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSem_Exit
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
        );
    }
}


const TResourceName& TSemaphore::rsnName() const
{
    return m_rsnThis;
}


// ---------------------------------------------------------------------------
//  TSemaphore: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSemaphore::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
                    << L", Max Count=" << m_ksemImpl.c4MaxCount();
}



// ---------------------------------------------------------------------------
//  TSemaphore: Private, non-virtual methods
// ---------------------------------------------------------------------------

// A common helper to avoid redundant code in some ctors
tCIDLib::TVoid
TSemaphore::InitNamed(          tCIDLib::TBoolean&      bCreated
                        , const tCIDLib::TCard4         c4InitCount
                        , const tCIDLib::ECreateActs eAction)
{
    if (eAction == tCIDLib::ECreateActs::CreateIfNew)
    {
        if (!m_ksemImpl.bCreate(c4InitCount))
        {
            const TKrnlError& kerrLast = TKrnlError::kerrLast();
            tCIDLib::TErrCode errcToThrow;
            if (kerrLast.errcId() == kKrnlErrs::errcGen_AlreadyExists)
                errcToThrow = kCIDErrs::errcSem_AlreadyExists;
            else
                errcToThrow = kCIDErrs::errcSem_Create;

            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcSem_Create
                , kerrLast
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
            );
        }

        // It's always created (or fails) in this case
        bCreated = kCIDLib::True;
    }
     else if (eAction == tCIDLib::ECreateActs::OpenOrCreate)
    {
        if (!m_ksemImpl.bOpenOrCreate(bCreated, c4InitCount))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcSem_OpenOrCreate
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
            );
        }
    }
     else if (eAction == tCIDLib::ECreateActs::OpenIfExists)
    {
        if (!m_ksemImpl.bOpen())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcSem_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
            );
        }

        // It's never created in this case
        bCreated = kCIDLib::False;
    }
     else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadCreateAct
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eAction))
            , TString(L"semaphore")
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Semaphore)
        );
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TSemJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSemJanitor: Constructors and Destructor.
// ---------------------------------------------------------------------------
TSemJanitor::TSemJanitor(       TSemaphore* const   psemToEnter
                        , const tCIDLib::TCard4     c4MilliSecs) :

    m_psemToEnter(0)
{
    m_psemToEnter->Enter(c4MilliSecs);

    // We successfull entered, so save the semaphore pointer
    m_psemToEnter = psemToEnter;
}

TSemJanitor::~TSemJanitor()
{
    // If it got entered, then exit
    if (m_psemToEnter)
        m_psemToEnter->Exit();
}

