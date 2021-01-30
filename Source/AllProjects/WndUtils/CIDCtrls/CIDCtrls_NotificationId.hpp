//
// FILE NAME: CIDCtrls_NotificationId.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1997
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
//  This is the header for the CIDCtrls_NotificationId.cpp file. This file
//  implements the TNotificationId class, which is a way to indentify what
//  a notification represents.
//
//  This was moved over from the old CIDWnd when we moved away from our own
//  custom windows.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TNotificationId
// PREFIX: nid
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TNotificationId

    : public TObject, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TNotificationId() = delete;

        explicit TNotificationId
        (
            const   TString&                strId
        );

        explicit TNotificationId
        (
            const   tCIDLib::TCh* const     pszId
        );

        TNotificationId
        (
            const   TNotificationId&        nidSrc
        );

        TNotificationId(TNotificationId&&) = delete;

        ~TNotificationId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNotificationId& operator=
        (
            const   TNotificationId&        nidSrc
        );

        TNotificationId& operator=(const TNotificationId&&) = delete;

        tCIDLib::TBoolean operator==
        (
            const   TNotificationId&        nidComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TNotificationId&        nidComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::THashVal hshId() const
        {
            return m_hshId;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hshId
        //      The hash of the id, for fast comparisons when lists of
        //      notifications are scanned.
        //
        //  m_szId
        //      This is the text of the notification id
        // -------------------------------------------------------------------
        tCIDLib::THashVal       m_hshId;
        tCIDLib::TCh            m_szId[kCIDCtrls::c4MaxNotIdLen + 1];


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNotificationId,TObject)
        DefPolyDup(TNotificationId)
};

#pragma CIDLIB_POPPACK



