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
//  It's for use in those cases where, ultimately down there somewhere, you need
//  the raw string contents.
//
//  Since this guy just references whatever underlies it, we can freely copy and
//  move. The dtor does nothing. And we can use defaults for that reason.
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

        TStringView
        (
            const   tCIDLib::TCh* const     pszRaw
        );

        TStringView
        (
            const   TString&                strObj
        );

        TStringView
        (
            const   TString* const          pstrObj
        );

        TStringView(const TStringView&) = default;
        TStringView(TStringView&&) = default;;

        ~TStringView() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringView& operator=(const TStringView&) = default;
        TStringView& operator=(TStringView&&) = default;


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
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bContainsSubStr
        (
            const   tCIDLib::TCh* const     pszToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4Length() const noexcept;

        [[nodiscard]] const tCIDLib::TCh* pszBuffer() const noexcept;

        [[nodiscard]] const tCIDLib::TCh* pszBufferAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsRaw
        //      If true we have a raw pointer, else we have a string object.
        //
        //  m_c4RawLen
        //      If this is a raw string, this is the cached length. If m_bRawLen
        //      is true, we've already gotten it. Else it will be faulted in when
        //      needed.
        //
        //  m_pszRaw
        //      If m_bIsRaw, then this is the raw string pointer, else null.
        //
        //  m_pstrObj
        //      If not m_bIsRaw, then this is the string object
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bIsRaw = kCIDLib::False;
        mutable tCIDLib::TBoolean   m_bRawLen = kCIDLib::False;
        mutable tCIDLib::TCard4     m_c4RawLen = 0;
        const tCIDLib::TCh*         m_pszRaw = nullptr;
        const TString*              m_pstrObj = nullptr;
};

#pragma CIDLIB_POPPACK
