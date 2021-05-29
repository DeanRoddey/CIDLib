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
tCIDLib::TBoolean TStringView::bContainsChar(const tCIDLib::TCh chToFind) const noexcept
{
    if (m_pszRaw)
        return TRawStr::pszFindChar(m_pszRaw, chToFind) != nullptr;

    return m_pstrObj->bContainsChar(chToFind);
}


tCIDLib::TBoolean TStringView::bContainsSubStr( const   TString&            strToFind
                                                , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    if (m_pszRaw)
        return TRawStr::pszFindSubStr(m_pszRaw, strToFind.pszBuffer(), 0, bCaseSensitive) != nullptr;
    return m_pstrObj->bContainsSubStr(strToFind, bCaseSensitive);
}

tCIDLib::TBoolean TStringView::bContainsSubStr( const   tCIDLib::TCh* const pszToFind
                                                , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    if (m_pszRaw)
        return TRawStr::pszFindSubStr(m_pszRaw, pszToFind, 0, bCaseSensitive) != nullptr;
    return m_pstrObj->bContainsSubStr(pszToFind, bCaseSensitive);
}


tCIDLib::TBoolean TStringView::bStartsWith( const   TString&            strToFind
                                            , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    if (m_pszRaw)
    {
        if (bCaseSensitive)
            return TRawStr::bCompareStrNI(strToFind.pszBuffer(), pszBuffer(), strToFind.c4Length());
        return TRawStr::bCompareStrN(strToFind.pszBuffer(), pszBuffer(), strToFind.c4Length());
    }

    if (bCaseSensitive)
        return m_pstrObj->bStartsWith(strToFind);
    return  m_pstrObj->bStartsWithI(strToFind);
}


tCIDLib::TVoid TStringView::CopyOutSubStr(          TString&            strTarget
                                            , const tCIDLib::TCard4     c4Start
                                            , const tCIDLib::TCard4     c4Len) const
{
    if (m_pstrObj)
    {
        m_pstrObj->CopyOutSubStr(strTarget, c4Start, c4Len);
    }
     else
    {
        if (c4Start > c4Length())
            ThrowIndexErr(CID_FILE, CID_LINE, c4Start);

        //
        //  If we would go beyond the end, then clip it back. We could pass max card
        //  for the real lenght, but we already did the work to get the length, and don't
        //  want to do it again.
        //
        tCIDLib::TCard4 c4RealLen = c4Len;
        if (c4Start + c4Len > c4Length())
            c4RealLen = c4Length() - c4Start;;

        strTarget.Clear();
        strTarget.AppendSubStr(m_pszRaw, c4Start, c4RealLen);
    }
}


const TString* TStringView::pstrObj() const
{
    if (m_pstrObj == nullptr)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrV_NoObject
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return m_pstrObj;
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

    if (m_pszRaw)
        return &m_pszRaw[c4At];
    return m_pstrObj->pszBufferAt(c4At);
}


// ---------------------------------------------------------------------------
//  TStringView: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TStringView::ThrowIndexErr(  const   tCIDLib::TCh* const     pszFile
                                            , const tCIDLib::TCard4         c4Line
                                            , const tCIDLib::TCard4         c4At) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcGen_IndexError
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Index
        , TCardinal(c4At)
        , TString(L"TStringView")
        , TCardinal(c4Length())
    );
}
