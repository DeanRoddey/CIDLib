//
// FILE NAME: CIDWUtils_ImgSelDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/2006
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
//  This is the header for the CIDWUtils_ImgSelDlg.cpp file, which implements a
//  simple dialog that allows the user to select an image from a collection of image
//  objects. It provides next/previous, cancel and select buttons. The next/prev
//  buttons moves back and forth through the list, with the current one being shown
//  in a static image control.
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
//   CLASS: TImgSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TImgSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TImgSelDlg();

        TImgSelDlg(const TImgSelDlg&) = delete;
        TImgSelDlg(TImgSelDlg&&) = delete;

        ~TImgSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TImgSelDlg& operator=(const TImgSelDlg&) = delete;
        TImgSelDlg& operator=(TImgSelDlg&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TRefVector<TCIDImage>&  colImgs
            , const TString&                strTitle
            , COP   tCIDLib::TCard4&        c4SelIndex
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDLib::TVoid UpdateLegend();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SelIndex
        //      This is the index we move up and down as the user browses,
        //      and which is given back if they select one.
        //
        //  m_pcolList
        //      A pointer to the incoming collection of images to display.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we
        //      need to interact with on a typed basis.
        //
        //  m_strTitle
        //      The incoming title is put here until we can get it set.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                 m_c4SelIndex;
        const TRefVector<TCIDImage>*    m_pcolList;
        TPushButton*                    m_pwndCancel;
        TStaticText*                    m_pwndLegend;
        TStaticImg*                     m_pwndCurImage;
        TPushButton*                    m_pwndNext;
        TPushButton*                    m_pwndPrev;
        TPushButton*                    m_pwndSelect;
        TString                         m_strTitle;



        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TImgSelDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

