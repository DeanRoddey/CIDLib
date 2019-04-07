//
// FILE NAME: CIDCtrls_List.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
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
//  This is a common base class for list oriented controls, such as list boxes,
//  combo boxes, and spin boxes. It just lets them share some common event
//  notification code.
//
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
//   CLASS: TListChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TListChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TListChangeInfo() = delete;

        TListChangeInfo
        (
            const   tCIDCtrls::EListEvents  eEvent
            , const tCIDLib::TCard4         c4NewIndex
            , const tCIDLib::TCard4         c4NewId
            , const tCIDLib::TCard4         c4ColIndex
            , const TWindow&                wndSrc
        );

        TListChangeInfo
        (
            const   TListChangeInfo&        wnotToCopy
        );

        ~TListChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TListChangeInfo& operator=
        (
            const   TListChangeInfo&        wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColIndex() const;

        tCIDLib::TCard4 c4Index() const;

        tCIDLib::TCard4 c4Id() const;

        tCIDCtrls::EListEvents eEvent() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index
        //      If the event is a single item (select, invoke), this is the new
        //      index. For single selection lists it's the new selection, else
        //      it's the caret one. For a clear event, it's meaningless.
        //
        //  m_c4ColInd
        //      In those cases where there are columns, this can indicate the
        //      index of the column affected.
        //
        //  m_c4Id
        //      The id of the new item, when it's a single item event. It's only
        //      available from derivatives that provide per-item ids.
        //
        //  m_eEvent
        //      The specific event that occurred.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ColInd;
        tCIDLib::TCard4         m_c4Index;
        tCIDLib::TCard4         m_c4Id;
        tCIDCtrls::EListEvents  m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TListChangeInfo,TCtrlNotify)
        DefPolyDup(TListChangeInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TListWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TListWnd : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TListWnd() = delete;

        TListWnd(const TListWnd&) = delete;

        ~TListWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TListWnd& operator=(const TListWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsMultiSel() const;

        virtual tCIDLib::TBoolean bSelectByText
        (
            const   TString&                strToSel
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
        ) = 0;

        virtual tCIDLib::TCard4 c4CurItem() const = 0;

        virtual tCIDLib::TCard4 c4FindByText
        (
            const   TString&                strToFind
        )   const = 0;

        virtual tCIDLib::TCard4 c4IndexToId
        (
            const   tCIDLib::TCard4         c4Index
        )   const = 0;

        virtual tCIDLib::TCard4 c4ItemCount() const = 0;

        virtual tCIDLib::TCard4 c4StartsWith
        (
            const   TString&                strToFind
        )   const = 0;

        virtual tCIDLib::TVoid RemoveAll
        (
            const   tCIDLib::TBoolean       bNoEvent
        ) = 0;

        virtual tCIDLib::TVoid RemoveAt
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bSelectAnother
        ) = 0;

        virtual tCIDLib::TVoid SelectByIndex
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurItemId() const;

        tCIDLib::TCard4 c4SelectByText
        (
            const   TString&                strToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
        );

        tCIDLib::TVoid RemoveCurrent
        (
            const tCIDLib::TBoolean         bSelectAnother = kCIDLib::True
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration of event
        //  handlers. Each derived types has to provide its own notification id or they
        //  would conflict, so the derived class provides us with one to use.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TListChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TListChangeInfo>(pTarget, m_nidChangeIdToUse)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TListChangeInfo>
            (
                pTarget, m_nidChangeIdToUse, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }



    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TListWnd
        (
            const   TString&                strNotificationId
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual
        // -------------------------------------------------------------------
        tCIDLib::TVoid SendListEvent
        (
            const   tCIDCtrls::EListEvents  eEvent
            , const tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TCard4         c4ColIndex = kCIDLib::c4MaxCard
            , const tCIDLib::TBoolean       bSync = kCIDLib::True
        );


    private :
        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_nidChangeIdToUse
        //      The derived class provides us with a notification id to use when our
        //      notification registration method is called.
        // -------------------------------------------------------------------
        TString     m_nidChangeIdToUse;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TListWnd, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK



