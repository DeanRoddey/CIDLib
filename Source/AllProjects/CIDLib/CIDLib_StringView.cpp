//
// FILE NAME: CIDLib_StringView.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/10/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  Implements the out of line content of the string view class, which is mostly
//  just passing through calls to either a TString method or a TRawStr function.
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
//  CLASS: TStringView
// PREFIX: strv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringView: Public, non-virtual methods
// ---------------------------------------------------------------------------
TStringView::TStringView(const tCIDLib::TCh* const pszRaw) :

    m_bIsRaw(kCIDLib::True)
    , m_pszRaw(pszRaw)
{
    if (pszRaw == nullptr)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NullSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
}

TStringView::TStringView(const TString& strObj) :

    m_bIsRaw(kCIDLib::False)
    , m_pstrObj(&strObj)
{
    if (m_pstrObj == nullptr)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NullSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
}

TStringView::TStringView(const TString* const pstrObj) :

    m_bIsRaw(kCIDLib::False)
    , m_pstrObj(pstrObj)
{
    if (m_pstrObj == nullptr)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NullSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
}


// ---------------------------------------------------------------------------
//  TStringView: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStringView::bContainsChar(const tCIDLib::TCh chToFind) const noexcept
{
    if (m_bIsRaw)
        return TRawStr::pszFindChar(m_pszRaw, chToFind) != nullptr;

    return m_pstrObj->bContainsChar(chToFind);
}


tCIDLib::TBoolean TStringView::bContainsSubStr( const   TString&            strToFind
                                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    if (m_bIsRaw)
        return TRawStr::pszFindSubStr(m_pszRaw, strToFind.pszBuffer(), 0, bCaseSensitive) != nullptr;
    return m_pstrObj->bContainsSubStr(strToFind, bCaseSensitive);
}

tCIDLib::TBoolean TStringView::bContainsSubStr( const   tCIDLib::TCh* const pszToFind
                                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    if (m_bIsRaw)
        return TRawStr::pszFindSubStr(m_pszRaw, pszToFind, 0, bCaseSensitive) != nullptr;
    return m_pstrObj->bContainsSubStr(pszToFind, bCaseSensitive);
}


tCIDLib::TCard4 TStringView::c4Length() const noexcept
{
    if (m_bIsRaw)
    {
        if (!m_bRawLen)
            m_c4RawLen = TRawStr::c4StrLen(m_pszRaw);
        return m_c4RawLen;
    }
    return m_pstrObj->c4Length();
}


const tCIDLib::TCh* TStringView::pszBuffer() const noexcept
{
    if (m_bIsRaw)
        return m_pszRaw;
    return m_pstrObj->pszBuffer();
}


const tCIDLib::TCh* TStringView::pszBufferAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TString(L"TStringView")
            , TCardinal(c4Length())
        );
    }

    if (m_bIsRaw)
        return &m_pszRaw[c4At];
    return m_pstrObj->pszBufferAt(c4At);
}
