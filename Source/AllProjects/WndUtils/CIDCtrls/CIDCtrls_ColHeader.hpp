//
// FILE NAME: CIDCtrls_ColHeader.hpp
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
//  A control that is used by other controls to provide column titles, and to manage the
//  width of those columns. It's just a row of cells, with divider lines. Each cell can
//  have a bitmap, a textual title, or both.
//
//  We let the user drag the divider lines and we send column size change messages to the
//  parent (whoever is registered for events, but mostly it'll be the containing window.)
//  The drag is limited such that no columns can get too small. We don't bother with the
//  complexity of min/max sizes of columns. Let the user do what he wants just don't get
//  the columns so small that they might cause drawing issues.
//
//  We provide virtuals for drawing of the individual columns, so a derivative can be
//  created to provide for specialized headers if needed. But generally it will be
//  flexible enough for most needs.
//
//  We have a small class that defines the info we store per-column, and keep an array of
//  those to represent the columns. The client code indicates how many columns we have and
//  sets the attributes of those columns.
//
//  We have a display style that controls the layout of the header cells.
//
//      BitmapOnly  - Just displays bitmap, the justification controls how it's placed
//      TextOnly    - Just displays text, the justification controls how it's placed
//      CenterText  - Both are displayed side by side, centered, with the text first
//      CenterBmp   - Both are displayed side by side, centered, with bitmap first
//      BmpRight    - Bitmap is drawn to the right, and the text gets the rest of the area
//                      and is justified base don the justification setting.
//      BmpLeft     - Bitmap is drawn to the left, and the text gets the rest of the area
//                      and is justified base don the justification setting.
//
//  If the bitmap is not set, then the display format will be forced to TextOnly. The text
//  can be empty, but it makes not sense for it to be.
//
//  These never scroll, they just clip if cummulative width of the columns pushes the
//  content off the right side. So it's fairly simple in that respect compared to some
//  things.
//
//  We store a width for each one. The vertical is driven by the window height, so we don't
//  need to keep that, and we just add up the widths to get the x offset of any given one.
//
//  They can pass max card for the width and we'll calculate an appropriate width based
//  on the content.
//
//  We draw a separator border along our bottom. There isn't one at the top, since this
//  control is always top justified in some parent window. If it has a border our having
//  one also wouldn't look good.
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
//  CLASS: TColHeaderInfo
// PREFIX: colhi
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TColHeaderInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TColHeaderInfo();

        TColHeaderInfo
        (
            const   TString&                strTitle
            , const tCIDLib::TCard4         c4Width
            , const tCIDLib::EHJustify      eJustify
        );

        TColHeaderInfo
        (
            const   TBitmap&                bmpTitle
            , const tCIDLib::TCard4         c4Width
            , const tCIDLib::EHJustify      eJustify
        );

        TColHeaderInfo
        (
            const   TString&                strTitle
            , const TBitmap&                bmpTitle
            , const tCIDCtrls::ECHdrTypes   eType
            , const tCIDLib::TCard4         c4Width
            , const tCIDLib::EHJustify      eJustify
        );

        TColHeaderInfo
        (
            const   TColHeaderInfo&         colhiSrc
        );

        ~TColHeaderInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColHeaderInfo& operator=
        (
            const   TColHeaderInfo&         cohiSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBold() const;

        tCIDLib::TBoolean bBold
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetColors
        (
            const   TRGBClr&                rgbBgnClr
            , const TRGBClr&                rgbTextClr
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TColHeader;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bBgnOver
        //      Indicates if the bgn color has been overridden. If not, then the colors of
        //      the header window are used.
        //
        //  m_bBold
        //      Controls whether the text should be displayed bold or normal. Defaults to
        //      normal.
        //
        //  m_bTextOver
        //      Indicates if the text color has been overridden. If not, then the color of
        //      the header window is used.
        //
        //  m_bmpTitle
        //      If we are to display a bitmap, this is it.
        //
        //  m_c4Width
        //      The width of this slot. We don't keep the horizontal start/end offsets,
        //      we just calculate them as required.
        //
        //  m_eType
        //      This indicates what type of display we are to do, text only, text left
        //      and image right, or image left and text right.
        //
        //  m_eJustify
        //      Controls how our text or bitmap is justified.
        //
        //  m_rgbBgn1
        //  m_rgbBgn2
        //  m_rgbText
        //      These are set from standard window colors, but can be updated by the
        //      client code to create specialized header displays. If not set explicitly
        //      the defaults are used for this slot.
        //
        //  m_strTitle
        //      A the title text if we are to display any
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bBgnOver;
        tCIDLib::TBoolean       m_bBold;
        tCIDLib::TBoolean       m_bTextOver;
        TBitmap                 m_bmpTitle;
        tCIDLib::TCard4         m_c4Width;
        tCIDCtrls::ECHdrTypes   m_eType;
        tCIDLib::EHJustify      m_eJustify;
        TRGBClr                 m_rgbBgn1;
        TRGBClr                 m_rgbBgn2;
        TRGBClr                 m_rgbText;
        TString                 m_strTitle;

};


// ---------------------------------------------------------------------------
//   CLASS: TColHdrChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TColHdrChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TColHdrChangeInfo() = delete;

        TColHdrChangeInfo
        (
            const   tCIDCtrls::ECHdrEvents  eEvent
            , const tCIDLib::TCard4         c4ColIndex
            , const TWindow&                wndSrc
        );

        TColHdrChangeInfo
        (
            const   tCIDLib::TCard4         c4ColIndex
            , const tCIDLib::TCard4         c4OldWidth
            , const tCIDLib::TCard4         c4NewWidth
            , const TWindow&                wndSrc
        );

        TColHdrChangeInfo
        (
            const   TColHdrChangeInfo&        wnotToCopy
        );

        ~TColHdrChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColHdrChangeInfo& operator=
        (
            const   TColHdrChangeInfo&        wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColIndex() const
        {
            return m_c4ColIndex;
        }

        tCIDLib::TCard4 c4OldWidth() const
        {
            return m_c4OldWidth;
        }

        tCIDLib::TCard4 c4NewWidth() const
        {
            return m_c4NewWidth;
        }

        tCIDCtrls::ECHdrEvents eEvent() const
        {
            return m_eEvent;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ColIndex
        //      If the event is about a single column, this is the index of that column.
        //      Else it is not used and set to max card.
        //
        //  m_c4NewWidth
        //  m_c4OldWidth
        //      If it's a drag operation, then the column index indicates the one whose
        //      right edge is being dragged. And these contain the previous and new widths
        //      for that column.
        //
        //  m_eEvent
        //      The specific event that occurred.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ColIndex;
        tCIDLib::TCard4         m_c4NewWidth;
        tCIDLib::TCard4         m_c4OldWidth;
        tCIDCtrls::ECHdrEvents  m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TColHdrChangeInfo,TCtrlNotify)
        DefPolyDup(TColHdrChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TColHeader
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TColHeader : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public data types
        // -------------------------------------------------------------------
        typedef TVector<TColHeaderInfo>     TColList;


        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChangeId;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TColHeader();

        TColHeader(const TColHeader&) = delete;

        ~TColHeader();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColHeader& operator=(const TColHeader&) = delete;


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

        tCIDLib::TVoid SetBgnColor
        (
            const   TRGBClr&                rgbToSet
            , const tCIDLib::TBoolean       bOver = kCIDLib::True
            , const tCIDLib::TBoolean       bRedraw = kCIDLib::False
        )   override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColCount() const;

        tCIDLib::TCard4 c4FullWidth() const;

        tCIDLib::TCard4 c4QueryWidths
        (
                    tCIDLib::TCardList&     fcolToFill
        );

        const TColHeaderInfo& colhiAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid CreateColHeader
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ECHdrStyles  eCHdrStyles = tCIDCtrls::ECHdrStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid SetColHdrWidth
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetColumns
        (
            const   TColList&               colToSet
        );

        tCIDLib::TVoid SetDefTextColor
        (
            const   TRGBClr&                rgbToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration of event
        //  handlers. Each derived types has to provide its own notification id or they
        //  would conflict, so the derived class provides us with one to use.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TColHdrChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TColHdrChangeInfo>(pTarget, nidChangeId)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TColHdrChangeInfo>
            (
                pTarget, nidChangeId, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUs
        )   override;

        tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bControlDown
            , const tCIDLib::TBoolean       bShiftDown
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        enum class ETrackStates
        {
            Idle
            , Click
            , Drag
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EWndAreas eSlotHitTest
        (
            const   TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4Index
        );

        tCIDLib::TInt4 i4SlotXOffset
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TInt4 i4SlotXOffset
        (
            const   tCIDLib::TCard4         c4Index
            ,       tCIDLib::TInt4&         i4SlotEndX
            ,       tCIDLib::TInt4&         i4SlotsEndX
        )   const;

        tCIDLib::TVoid SetFillColors
        (
            const   TRGBClr&                rgbBase
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4TrackIndex
        //      If we are tracking, this is the slot we are tracking.
        //
        //  m_colList
        //      Our list of column info objects.
        //
        //  m_eCHdrStyles
        //      Our column header specific styles.
        //
        //  m_eTrack
        //      Indicates if we are tracking and, if so, what type of event we are tracking.
        //
        //  m_rgbBgn1
        //  m_rgbBgn2
        //      We do a gradient fill on the window area. Individual headers can do their
        //      own colors. This is set as the initial values to be used if a specific color
        //      hasn't been used, so that the default is that they match the overall window
        //      colors. We base these on the bgn color set on us.
        //
        //  m_rgbText
        //      The text color to use for columns where the text color hasn't been explicitly
        //      set. We default it to a very dark gray, but not black. That minimizes the
        //      artifacts when moving the text around.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4TrackIndex;
        TColList                m_colList;
        tCIDCtrls::ECHdrStyles  m_eCHdrStyles;
        ETrackStates            m_eTrack;
        TRGBClr                 m_rgbBgn1;
        TRGBClr                 m_rgbBgn2;
        TRGBClr                 m_rgbText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TColHeader, TCtrlWnd)
};


#pragma CIDLIB_POPPACK

