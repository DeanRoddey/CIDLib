//
// FILE NAME: CIDLib_StringView.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 5/10/2021
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
//  This file implements the TStringView class. This is a light wrapper around
//  string objects or raw strings. Anything that just needs to look at the value
//  of a string can take one of these, and this avoids the need to provide two
//  different overloads.
//
//  Don't abuse it. It's just for things that process the contents of a string
//  and don't play any tricks. You can't modify the string, and you shouldn't
//  try to store any pointer to it, etc... It must be null terminated if it's
//  a raw string.
//
//  It just keeps a pointer to either a string object or a raw string. If it's
//  a string object, then it knows the length. If a raw string, it will get the
//  length if requested, and store it, so it only has to get it once.
//
//  That's as efficient as anything you'd do if you provided two separate
//  overloads typically. Obviously, if that call in turn calls other things and
//  needs to pass the string content along, that thing needs to take a string
//  view as well. So support for these needs to be implemented bottom up.
//
//  And also, obviously, if what you are calling really needs a string object,
//  because it's going to stream it out to something that expects a string
//  object or some such, then you need to take the actual string object.
//
//  It's for use in those cases where, ultimately down there somewhere, you just
//  need the raw string contents.
//
//  Since this guy just references whatever underlies it, we can freely copy and
//  move. The dtor does nothing. And we can use defaults for that reason.
//
//  We don't accept a null pointer. If you pass a null pointer, it will be
//  replaced by a pointer to kCIDLib::pszEmptyZStr or TString::strEmpty(). We
//  don't want to throw so that we constructor, assign in a constexpr way.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TStringView
// PREFIX: strv
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringView
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringView() = delete;

        constexpr TStringView(const tCIDLib::TCh* const pszRaw) :

            m_pszRaw(pszRaw)
        {
            if (pszRaw == nullptr)
                m_pszRaw = kCIDLib::pszEmptyZStr;
        }

        constexpr TStringView(const TString& strObj) :

            m_pstrObj(&strObj)
        {
            if (m_pstrObj == nullptr)
                m_pstrObj = &TString::strEmpty();
        }

        constexpr TStringView(const TString* const pstrObj) :

            m_pstrObj(pstrObj)
        {
            if (m_pstrObj == nullptr)
                m_pstrObj = &TString::strEmpty();
        }

        template <tCIDLib::TCard4 c4Sz> constexpr TStringView(const tCIDLib::TCh(&aChars)[c4Sz])
        {
            m_pszRaw = aChars;
        }

        TStringView(const TStringView&) = default;
        TStringView(TStringView&&) = default;

        ~TStringView() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringView& operator=(const TStringView&) = default;
        TStringView& operator=(TStringView&&) = default;

        [[nodiscard]] tCIDLib::TBoolean operator==(const TString& strSrc) const
        {
            if (m_pstrObj)
                return m_pstrObj->operator==(strSrc);

            if (bHaveLength() && (c4Length() != strSrc.c4Length()))
                return kCIDLib::False;

            return TRawStr::bCompareStr(m_pszRaw, strSrc.pszBuffer());
        }

        tCIDLib::TCh operator[](const tCIDLib::TCard4 c4At) const
        {
            if (m_pszRaw)
            {
                if (m_c4RawLen == kCIDLib::c4MajVersion)
                    m_c4RawLen = TRawStr::c4StrLen(m_pszRaw);

                if (c4At >= m_c4RawLen)
                    ThrowIndexErr(CID_FILE, CID_LINE, c4At);
            }
            return m_pstrObj->chAt(c4At);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TBoolean bContainsChar
        (
            const   tCIDLib::TCh            chToFind
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bContainsSubStr
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bContainsSubStr
        (
            const   tCIDLib::TCh* const     pszToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const noexcept;

        [[nodiscard]] constexpr tCIDLib::TBoolean bIsEmpty() const
        {
            if (m_pstrObj)
                return m_pstrObj->bIsEmpty();
            return m_pszRaw[0] == kCIDLib::chNull;
        }

        [[nodiscard]] tCIDLib::TBoolean bFirstOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bLastOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const noexcept;

        [[nodiscard]] constexpr tCIDLib::TBoolean bHaveLength() const
        {
            return (m_pstrObj != nullptr) || (m_c4RawLen != kCIDLib::c4MaxCard);
        }

        [[nodiscard]] constexpr tCIDLib::TBoolean bIsString() const
        {
            return m_pstrObj != nullptr;
        }

        [[nodiscard]] tCIDLib::TBoolean bStartsWith
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bStartsWith
        (
            const   tCIDLib::TCh* const     pszToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const noexcept;

        [[nodiscard]] constexpr tCIDLib::TCard4 c4Length() const noexcept
        {
            if (m_pszRaw)
            {
                if (m_c4RawLen == kCIDLib::c4MaxCard)
                    m_c4RawLen = TRawStr::c4StrLen(m_pszRaw);
                return m_c4RawLen;
            }
            return m_pstrObj->c4Length();
        }

        // Will not fault in raw strijng length if not gotten already. Returns max card if not
        [[nodiscard]] constexpr tCIDLib::TCard4 c4LengthOrNot() const noexcept
        {
            if (m_pszRaw)
                return m_c4RawLen;
            return m_pstrObj->c4Length();
        }

        // Null if the string is empty
        [[nodiscard]] constexpr tCIDLib::TCh chFirst() const
        {
            if (m_pstrObj)
                return m_pstrObj->chFirst();
            return m_pszRaw[0];
        }

        tCIDLib::TVoid CopyOutSubStr
        (
            COP     TString&                strTarget
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len
        )   const;

        [[nodiscard]] constexpr tCIDLib::THashVal hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
        {
            if (m_pszRaw)
                return TRawStr::hshHashStr(m_pszRaw, c4Modulus);
            return m_pstrObj->hshCalcHash(c4Modulus);
        }

        [[nodiscard]] constexpr const tCIDLib::TCh* pszBuffer() const noexcept
        {
            if (m_pszRaw)
                return m_pszRaw;
            return m_pstrObj->pszBuffer();
        }

        [[nodiscard]] const tCIDLib::TCh* pszBufferAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        [[nodiscard]] constexpr const tCIDLib::TCh* pszEnd() const noexcept
        {
            if (m_pszRaw)
            {
                return &m_pszRaw[c4Length()];
            }
            return m_pstrObj->pszEnd();
        }

        [[nodiscard]] const TString* pstrObj() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        [[noreturn]] tCIDLib::TVoid ThrowIndexErr
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4At
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4RawLen
        //      If this is a raw string, this is the cached length. It is initialized
        //      to max to indicate not gotten yet. We'll fault it in for raw strings
        //      if actually needed. This way we don't have to pay multiple times for
        //      returning the length of raw strings.
        //
        //  m_pszRaw
        //      If this is non-null, then this is wrapping a raw string.
        //
        //  m_pstrObj
        //      If this is non-null, this is wrapping a string object
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4     m_c4RawLen = kCIDLib::c4MaxCard;
        const tCIDLib::TCh*         m_pszRaw = nullptr;
        const TString*              m_pstrObj = nullptr;
};

#pragma CIDLIB_POPPACK
