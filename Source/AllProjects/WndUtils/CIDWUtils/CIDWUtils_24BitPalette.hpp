//
// FILE NAME: CIDWUtils_24BitPalette.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  This class implements a window that displays a 24 bit palette bitmap and
//  allows the user to click on that bitmap to set a color. It keeps up with
//  the pixel clicked on and puts a hotspot cursor on that spot. It also
//  sends events about changes in the selected color.
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
//   CLASS: TPalChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDWUTILSEXP T24BPalChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        T24BPalChangeInfo() = delete;

        T24BPalChangeInfo
        (
            const   TRGBClr&                rgbColor
            , const TWindow&                wndSrc
        );

        T24BPalChangeInfo
        (
            const   T24BPalChangeInfo&      wnotToCopy
        );

        ~T24BPalChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T24BPalChangeInfo& operator=
        (
            const   T24BPalChangeInfo&      wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TRGBClr& rgbNewColor() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_rgbColor
        //      The new color that was selected
        // -------------------------------------------------------------------
        TRGBClr m_rgbColor;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(T24BPalChangeInfo,TCtrlNotify)
        DefPolyDup(T24BPalChangeInfo)
};


// ---------------------------------------------------------------------------
//   CLASS: T24BitPalette
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP T24BitPalette : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        T24BitPalette();

        T24BitPalette(const T24BitPalette&) = delete;

        T24BitPalette
        (
            const   TRGBClr&                rgbInit
        );

        ~T24BitPalette();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T24BitPalette& operator=(const T24BitPalette&) = delete;


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


        // -------------------------------------------------------------------
        //  We provide a templatized method to register typesafe handlers
        //  for events.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(T24BPalChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to the source palette
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, T24BPalChangeInfo>
                (
                    pTarget, T24BitPalette::nidChange
                )
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, T24BPalChangeInfo>
            (
                pTarget, T24BitPalette::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::EWndStyles   eWndStyles
            , const TArea&                  areaInit
            , const tCIDCtrls::TWndId       widToUse
        );

        TRGBClr rgbCurrent() const;

        tCIDLib::TVoid SetColor
        (
            const   TRGBClr&                rgbToSet
        );


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
        )   override;

        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bCreated() override;

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

        tCIDLib::TVoid LosingFocus() override;


    private :
        // -------------------------------------------------------------------
        //  Private types
        // -------------------------------------------------------------------
        enum class ETrackModes
        {
            None
            , Palette
            , Value
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CalcAreas
        (
            const   TSize&                  szNew
        );

        tCIDLib::TVoid ColorToPoint
        (
            const   THSVClr&                hsvSrc
            ,       TPoint&                 pntTarget
        )   const;

        tCIDLib::TVoid DrawSelector
        (
                    TGraphDrawDev&          gdevToUse
        );

        tCIDLib::TVoid DrawValueBar
        (
                    TGraphDrawDev&          gdevToUse
        );

        tCIDLib::TVoid DrawValueBarPtr
        (
                    TGraphDrawDev&          gdevToUse
            , const THSVClr&                hsvToUse
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TFloat4 f4ValueFromPoint
        (
            const   TPoint&                 pntAt
        )   const;

        TPoint pntSelector() const;

        tCIDLib::TVoid PointToColor
        (
            const   TPoint&                 pntSrc
            ,       THSVClr&                hsvTarget
        )   const;

        tCIDLib::TVoid RemoveSelector
        (
                    TGraphDrawDev&          gdevToUse
        );

        tCIDLib::TVoid SendChangeNotification
        (
            const   TRGBClr&                rgbNew
        );

        tCIDLib::TVoid SetFromColor
        (
            const   THSVClr&                hsvToSet
        );

        tCIDLib::TVoid SetFromPoint
        (
            const   TPoint&                 pntAt
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaPal
        //  m_areaValueBar
        //      The area of the palette and the area of the value bar (the
        //      area inside their sunken borders.)
        //
        //  m_bmpPal
        //      The palette bitmap that we display.
        //
        //  m_bmpValSel
        //      The small triangle image that we use as the indicator.
        //
        //  m_eTracking
        //      Used to keep up with whether we are tracking the mouse or not.
        //
        //  m_hsvCur
        //      This is our current color value, which is represented in HSV
        //      format because that maps directly to what on the screen. The
        //      horizontal axis of the bitmap maps to the hue (0 to 360.0),
        //      the vertical to the saturation (0 to 1.0), and the value bar
        //      to the value (0 to 1.0).
        //
        //  m_hsvTracking
        //      This is the color that we use during mouse tracking, so that
        //      we don't affect the original until we are sure that the mouse
        //      is going to be dropped on a legal spot.
        //
        //  m_szValSel
        //      To avoid having to get it over and over again, we store the
        //      size of the value selector icon.
        // -------------------------------------------------------------------
        TArea       m_areaPal;
        TArea       m_areaValueBar;
        TBitmap     m_bmpPal;
        TBitmap     m_bmpValSel;
        ETrackModes m_eTracking;
        THSVClr     m_hsvCur;
        THSVClr     m_hsvTracking;
        TSize       m_szValSel;

        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(T24BitPalette,TCtrlWnd)
};

#pragma CIDLIB_POPPACK


