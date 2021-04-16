//
// FILE NAME: CIDWUtils_TextOut.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/2015
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
//  This is a commonly used window that supports a LIFO type of text output window
//  where lines of text can be output until a max line count is reached then the
//  oldest lines are thrown away to make room for new ones at the other end. It outputs
//  at the bottom and scrolls upwards.
//
//  We have a main window, with a vertical scroll bar to the right, and a display area in
//  the remaining area (with a small margin to keep the text off the edges.) The display
//  area just shows the messages that fall into the current 'view port', the part of the
//  virtual display area that falls into the display area given the scroll position.
//
//  We don't try to be fancy. We keep around an area that represents the total size of the
//  virtual content (sized to fit the horizontal size of the viewing area.) Painting just
//  requires that we calculate the lines that intersect the display view port. We never have
//  to update the scroll bar, the user only does that, and we just update our virtual content
//  area's y origin to reflect that. We initially put the scroll bar at the bottom, since we
//  draw from the bottom. The scroll bar is always set up to reflect the full virtual content
//  area.
//
//  New lines being inserted just requires that we move any existing content up and put a new
//  one in at the bottom. We just move the screen content up by a line and invalidate the
//  area of the last line. If it's visible it will display. We don't have to adjust the scroll
//  bar even here. It's always reflecting the position of the virtual content. We are just
//  scrolling the lines up through that virtual content area.
//
//
//  We also have a fancier one that is used for things like chat output text display. It's
//  non-scrollable and just maintains a bitmap image the size of itself into which it will
//  scroll in text from the bottom. Each one can be given a different color. It doesn't
//  remember anything since it only displays the text graphically.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTODispWnd;

// ---------------------------------------------------------------------------
//   CLASS: TTextOutWnd
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TTextOutWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextOutWnd
        (
            const   tCIDLib::TCard4         c4MaxStrs = 1024
        );

        TTextOutWnd(const TTextOutWnd&) = delete;
        TTextOutWnd(TTextOutWnd&&) = delete;

        ~TTextOutWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextOutWnd& operator=(const TTextOutWnd&) = delete;
        TTextOutWnd& operator=(TTextOutWnd&&) = delete;


                // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddString
        (
            const   TString&                strToAdd
        );

        tCIDLib::TVoid AddStrings
        (
            const   tCIDLib::TStrList&      colToAdd
        );

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid CreateTextOut
        (
            const   TWindow&                wndParent
            , const TArea&                  areaInit
            , const tCIDCtrls::TWndId       widThis
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDWUtils::ETOutStyles eTOStyles = tCIDWUtils::ETOutStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid QueryText
        (
                    TString&                strToLoad
        )   const;

        const TRGBClr& rgbCurBgn() const;

        const TRGBClr& rgbCurText() const;

        tCIDLib::TVoid SetLineColors
        (
            const   TRGBClr&                rgbBgn
            , const TRGBClr&                rgbText
        );


    protected :
        // -------------------------------------------------------------------
        //  A small class we use to hold the text lines and the colors assigned to
        //  each one.
        // -------------------------------------------------------------------
        class TTOLine
        {
            public :
                TTOLine();
                TTOLine
                (
                    const   TString&    strText
                    , const TRGBClr&    rgbBgn
                    , const TRGBClr&    rgbText
                );
                TTOLine(const TTOLine& tolSrc) = default;
                ~TTOLine();

                TTOLine& operator=(const TTOLine&) = default;

                tCIDLib::TVoid Reset
                (
                    const   TRGBClr&    rgbBgn
                    , const TRGBClr&    rgbText
                );

                TRGBClr m_rgbBgn;
                TRGBClr m_rgbText;
                TString m_strText;
        };


        // -------------------------------------------------------------------
        //  Protected, inheritd methods
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

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        )   override;

        tCIDLib::TVoid ScrollEvent
        (
            const   tCIDCtrls::EScrollEvs   eEvent
            , const tCIDLib::TBoolean       bHorizontal
        )   override;

        tCIDLib::TVoid ScrollDrag
        (
            const   tCIDLib::TBoolean       bHorizontal
            , const tCIDLib::TCard4         c4Position
            , const tCIDLib::TBoolean       bFinal
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetDisplayArea();

        tCIDLib::TVoid SetVContArea();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaDisplay
        //      Our display area, which is our client area minus some margin.
        //
        //  m_areaVCont
        //      This is the virtual content area. It's calculated to fit the max lines
        //      vertically and the width of the display area horizontally. It's y offset
        //      is adjusted to reflect the scroll bar position, i.e. it will be:
        //
        //          (scrollpos * lineheight * -1)
        //
        //      So it moves 'up above' our display area as the scroll position gets larger.
        //
        //  m_c4LineHeight
        //      The height of a single line, including spacing. This based on our font.
        //
        //  m_colBuf
        //      A circular buffer we use to manage the list of strings. To make it even
        //      easier, we pre-fill it with empty strings (bgn color sames as ours), so that
        //      it's always at the max line count. This makes it straightforward to translate
        //      from lines in the buffer to positions within the virtual content area.
        //
        //  m_eTOStyles
        //      Our type specific window styles
        //
        //  m_rgbCurBgn
        //      The last colors we were given, which is applied to any new text lines
        //      added. We default them to our own colors, in which case they hav no effect.
        // -------------------------------------------------------------------
        TArea                   m_areaDisplay;
        TArea                   m_areaVCont;
        tCIDLib::TCard4         m_c4LineHeight;
        tCIDWUtils::ETOutStyles m_eTOStyles;
        TCircularBuf<TTOLine>   m_colBuf;
        TRGBClr                 m_rgbCurBgn;
        TRGBClr                 m_rgbCurText;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextOutWnd, TCtrlWnd)
};



// ---------------------------------------------------------------------------
//   CLASS: TTextOutWnd2
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TTextOutWnd2 : public TCtrlWnd
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextOutWnd2();

        TTextOutWnd2(const TTextOutWnd2&) = delete;
        TTextOutWnd2(TTextOutWnd2&&) = delete;

        ~TTextOutWnd2();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextOutWnd2& operator=(const TTextOutWnd2&) = delete;
        TTextOutWnd2& operator=(TTextOutWnd2&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendText
        (
            const   TString&                strToAppend
            , const TRGBClr&                rgbColor
        );

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid CreateTextOutWnd
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widToUse
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eWndStyles
            , const tCIDCtrls::EExWndStyles eExWndStyles
        );

        tCIDLib::TVoid SetLineSpacing
        (
            const   tCIDLib::TCard4         c4ToSet
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

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoSizeCalcs();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaZero
        //      The area of the image, zero origined, i.e. for drawing into it.
        //
        //  m_areaImg
        //      The area of the bitmap, as displayed in our window, for blitting
        //      from it to the window.
        //
        //  m_bmpImg
        //      This is the backing image we use to display the text, which is
        //      all just done graphically. We size it up to fit our own size.
        //
        //  m_c4LineSpacing
        //      The spacing between sequential output text chunks.
        // -------------------------------------------------------------------
        TArea           m_areaZero;
        TArea           m_areaImg;
        TBitmap         m_bmpImg;
        tCIDLib::TCard4 m_c4LineSpacing;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextOutWnd2,TCtrlWnd)
};

#pragma CIDLIB_POPPACK



