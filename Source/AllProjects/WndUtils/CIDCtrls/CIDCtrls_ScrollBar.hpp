//
// FILE NAME: CIDCtrls_ScrollBar.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/2017
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
//  Implements a wrapper around the standard scrollbar control. These are often more
//  convenient to use than the ones that are built into windows, because those cannot
//  be kept visible when there's no scrollable content, so yo are constantly having to
//  adjust the size of your content to account for that. And of course these can be put
//  whereever you want them.
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
//   CLASS: TSBChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSBChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSBChangeInfo() = delete;

        TSBChangeInfo
        (
            const   tCIDLib::TCard4         c4Position
            , const TWindow&                wndSrc
        );

        TSBChangeInfo
        (
            const   TSBChangeInfo&          wnotSrc
        );

        ~TSBChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSBChangeInfo& operator=
        (
            const   TSBChangeInfo&          wnotSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Position() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Position
        //      The new scroll position.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Position;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSBChangeInfo,TCtrlNotify)
        DefPolyDup(TSBChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TScrollBar
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TScrollBar : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TScrollBar();

        TScrollBar(const TScrollBar&) = delete;

        ~TScrollBar();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TScrollBar& operator=(const TScrollBar&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurPos() const;

        tCIDLib::TVoid CreateScrollBar
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDLib::TBoolean       bHorizontal
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ESBStyles    eScrollStyles = tCIDCtrls::ESBStyles::None
        );

        tCIDLib::TVoid Setup
        (
            const   tCIDLib::TCard4     c4VisUnits
            , const tCIDLib::TCard4     c4AvailUnits
            , const tCIDLib::TCard4     c4InitPos
        );

        tCIDLib::TVoid Setup
        (
            const   tCIDLib::TCard4     c4VisUnits
            , const tCIDLib::TCard4     c4AvailUnits
        );

        tCIDLib::TVoid SetPosition
        (
            const   tCIDLib::TCard4     c4ToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TSBChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TSBChangeInfo>(pTarget, TScrollBar::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TSBChangeInfo>
            (
                pTarget, TScrollBar::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }



    protected :
        // -------------------------------------------------------------------
        //  Protecte, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bHorizontal
        //      Indicates if we are horizontal or vertical style
        //
        //  m_eScrollStyles
        //      Our own scroll bar specific styles
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bHorizontal;
        tCIDCtrls::ESBStyles    m_eScrollStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TScrollBar, TStdCtrlWnd)
};


#pragma CIDLIB_POPPACK

