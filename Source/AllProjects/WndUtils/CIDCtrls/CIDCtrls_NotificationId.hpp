//
// FILE NAME: CIDCtrls_NotificationId.hpp
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
        TNotificationId
        (
            const   TString&                strId
        );

        TNotificationId
        (
            const   tCIDLib::TCh* const     pszId
        );

        TNotificationId
        (
            const   TNotificationId&        nidToCopy
        );

        ~TNotificationId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNotificationId& operator=
        (
            const   TNotificationId&        nidToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TNotificationId&        nidToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TNotificationId&        nidToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::THashVal hshId() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TNotificationId();

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



