//
// FILE NAME: CIDMacroDbg_BuildResTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file defines a tab window derivative that is used to show the CML parser
//  output and to let the user double click on entries to go to that position. It
//  implements the CML parser's error output mixin and is set on the parser,
//  which spits out info directly to it. We implement a control notification
//  derivative for sending out double click notifications with the class path and
//  line/col info. The main dbugger window registers for them and passes them on
//  to the correct editor tab (loading new ones if needed.)
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
//   CLASS: TPrsErrPosInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TPrsErrPosInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPrsErrPosInfo() = delete;

        TPrsErrPosInfo
        (
            const   TWindow&                wndSrc
            , const TString&                strClassPath
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Col
        );

        TPrsErrPosInfo
        (
            const   TPrsErrPosInfo&         wnotSrc
        );

        ~TPrsErrPosInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPrsErrPosInfo& operator=
        (
            const   TPrsErrPosInfo&         wnotSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Col() const;

        tCIDLib::TCard4 c4Line() const;

        const TString& strClassPath() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Col
        //  m_c4Line
        //      The line/col where the error occured within the given class.
        //
        //  m_strClassPath
        //      The class path of the class where the error occurred.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Col;
        tCIDLib::TCard4         m_c4Line;
        TString                 m_strClassPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPrsErrPosInfo,TCtrlNotify)
        DefPolyDup(TPrsErrPosInfo)
};



// ---------------------------------------------------------------------------
//   CLASS: TBuildResTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TBuildResTab : public TTabWindow, public MMEngPrsErrHandler
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidErrInfo;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBuildResTab();

        TBuildResTab(const TBuildResTab&) = delete;

        ~TBuildResTab();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBuildResTab& operator=(const TBuildResTab&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseEvent
        (
            const   tCIDMacroEng::EPrsEvs   eEvent
            , const TString&                strText
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;

        tCIDLib::TVoid ParseException
        (
            const   TError&                 errCaught
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;

        tCIDLib::TVoid ParseException
        (
            const   tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateBldResTab
        (
                    TTabbedWnd&             wndParent
        );

        tCIDLib::TVoid Reset();


        // -------------------------------------------------------------------
        //  We provide a templatized method to register typesafe handlers
        //  for events.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const            pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TPrsErrPosInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            TCtrlWnd::CheckHandlerRegParms(this, pTarget);

            // A request object to us
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TPrsErrPosInfo>(pTarget, TBuildResTab::nidErrInfo)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TPrsErrPosInfo>
            (
                pTarget, TBuildResTab::nidErrInfo, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )  override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid CustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Id
            , const tCIDCtrls::ECustDrFlags eFlags
            , const TArea&                  areaTar
        )   override;

        tCIDLib::TVoid MeasureItem
        (
            const   tCIDCtrls::TWndId       widCtrl
            , const tCIDLib::TCard4         c4ItemId
            ,       tCIDLib::TCard4&        c4Height
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        class TErrInfo
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TErrInfo();

                TErrInfo
                (
                    const   TErrInfo&       erriSrc
                );

                ~TErrInfo();


                tCIDLib::TCard4         m_c4ColNum;
                tCIDLib::TCard4         m_c4LineNum;
                tCIDMacroEng::EPrsEvs   m_eEvent;
                TString                 m_strClassPath;
                TString                 m_strText;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eLBHandler
        (
                    TListChangeInfo&        wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colErrs
        //      We store the incoming event data here. They are stored in the order
        //      recieved and a formatted version is loaded into the list box.
        //
        //  m_pwndList
        //      The list window that the output is spit out to. It's not sorted
        //      so the indices map directly back to m_colErrs.
        //
        //  m_strFmt
        //      A temp string to format the values before we load them to the
        //      list.
        // -------------------------------------------------------------------
        TVector<TErrInfo>   m_colErrs;
        TListBox*           m_pwndList;
        TString             m_strFmt;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBuildResTab, TTabWindow)
};


#pragma CIDLIB_POPPACK


