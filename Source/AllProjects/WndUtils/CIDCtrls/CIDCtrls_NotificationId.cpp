//
// FILE NAME: CIDCtrls_NotificationId.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TNotificationId class, which identifies a
//  notification to the receiver. Its also used internally to keep a list of
//  what notifications the outside world wants to know about.
//
//  This was moved over from the old CIDWnd when we moved to wrapping standard
//  controls instead of doing our own.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TNotificationId,TObject)


namespace CIDCtrls_NotificationId
{
    // -----------------------------------------------------------------------
    //  Local const data
    // -----------------------------------------------------------------------
    const tCIDLib::THashVal hshModulus = 109;
}



// ---------------------------------------------------------------------------
//  CLASS: TNotificationId
// PREFIX: nid
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNotificationId: Constructors and destructor
// ---------------------------------------------------------------------------
TNotificationId::TNotificationId(const TString& strId)
{
    if (strId.c4Length() > kCIDCtrls::c4MaxNotIdLen)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNot_BadIdLen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , strId
            , TCardinal(kCIDCtrls::c4MaxNotIdLen)
        );
    }

    strId.ToZStr(m_szId, kCIDCtrls::c4MaxNotIdLen);
    m_hshId = strId.hshCalcHash(CIDCtrls_NotificationId::hshModulus);
}

TNotificationId::TNotificationId(const TNotificationId& nidToCopy) :

    m_hshId(nidToCopy.m_hshId)
{
    // Just do a raw copy over, since we know the size
    TRawMem::CopyMemBuf(m_szId, nidToCopy.m_szId, sizeof(m_szId));
}

TNotificationId::TNotificationId(const tCIDLib::TCh* const pszId)
{
    if (TRawStr::c4StrLen(pszId) > kCIDCtrls::c4MaxNotIdLen)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcNot_BadIdLen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(pszId)
            , TCardinal(kCIDCtrls::c4MaxNotIdLen)
        );
    }

    TRawStr::CopyStr(m_szId, pszId);
    m_hshId = TRawStr::hshHashStr(pszId, CIDCtrls_NotificationId::hshModulus);
    if (!m_hshId)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}

TNotificationId::~TNotificationId()
{
}


// ---------------------------------------------------------------------------
//  TNotificationId: Public operators
// ---------------------------------------------------------------------------
TNotificationId& TNotificationId::operator=(const TNotificationId& nidToAssign)
{
    if (this == &nidToAssign)
        return *this;

    // Just do a raw copy over, since we know the size
    TRawMem::CopyMemBuf(m_szId, nidToAssign.m_szId, sizeof(m_szId));
    m_hshId = nidToAssign.m_hshId;

    return *this;
}

tCIDLib::TBoolean
TNotificationId::operator==(const TNotificationId& nidToCompare) const
{
    if (this == &nidToCompare)
        return kCIDLib::True;

    if (m_hshId != nidToCompare.m_hshId)
        return kCIDLib::False;

    if (!TRawStr::bCompareStr(nidToCompare.m_szId, m_szId))
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TNotificationId::operator!=(const TNotificationId& nidToCompare) const
{
    return !operator==(nidToCompare);
}


// ---------------------------------------------------------------------------
//  TNotificationId: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::THashVal TNotificationId::hshId() const
{
    return m_hshId;
}


// ---------------------------------------------------------------------------
//  TNotificationId: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TNotificationId::FormatTo(TTextOutStream& strmDest) const
{
    strmDest << m_szId;
}

