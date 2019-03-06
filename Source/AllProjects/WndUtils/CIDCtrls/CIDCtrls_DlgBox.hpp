//
// FILE NAME: CIDCtrls_DlgBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This implements our wrapper around the dialog box.
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
//  CLASS: TDlgBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TDlgBox : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static TRGBClr rgbDlgBgn
        (
            const   tCIDCtrls::EWndThemes   eTheme
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDlgBox();

        ~TDlgBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4RunDlg
        (
            const   TWindow&                wndOwner
            , const TDlgDesc&               dlgdCreate
        );

        tCIDLib::TCard4 c4RunDlg
        (
            const   TWindow&                wndOwner
            , const TGUIFacility&           facDlgSrc
            , const tCIDLib::TResId         ridDlg
            , const TPoint&                 pntOrg
        );

        tCIDLib::TCard4 c4RunDlg
        (
            const   TWindow&                wndOwner
            , const TGUIFacility&           facDlgSrc
            , const tCIDLib::TResId         ridDlg
        );

        tCIDLib::TVoid ForceToFront();

        tCIDLib::TVoid SetMinSize
        (
            const   TSize&                  szToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCancelRequest();


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

        tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TVoid WndTextPreQuery
        (
                    TString&                strToFill
        )   const override;

        tCIDLib::TVoid WndTextStore
        (
            const   TString&                strToStore
        )   override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddWndState
        (
            const   TWndState&              wstateToAdd
        );

        tCIDLib::TVoid ApplyWndState
        (
            const   TString&                strToSet
        );

        tCIDLib::TCard4 c4FindWndState
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid DelWindowState
        (
            const   TString&                strToRemove
        );

        tCIDLib::TVoid EndDlg
        (
            const   tCIDLib::TCard4         c4Code
        );

        tCIDLib::TVoid SetAutoSize
        (
            const   tCIDLib::TBoolean       bToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TDlgBox(const TDlgBox&);
        tCIDLib::TVoid operator=(const TDlgBox&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAutoSize
        //      The derived class can ask us to auto-resize children as our size changes
        //      based on the anchor settings they have.
        //
        //  m_colWndStates
        //      We support a list of named states that enable/disable menu items and
        //      child windows, set focus, etc.. The SetWindowState() method will set one.
        //
        //  m_pdlgdCreate
        //      A pointer to the incoming dialog description, so that we can have it
        //      available in the created callback, where we want to create our child controls.
        //
        //  m_szMinimum
        //      A minimum size we will allow ourselves to be sized to. We initialize it to
        //      the size required to hold the dialog content, but the client code can
        //      call SetMinSize to change it if they want.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bAutoSize;
        tCIDCtrls::TWndStateList    m_colWndStates;
        const TDlgDesc*             m_pdlgdCreate;
        TSize                       m_szMinimum;



        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDlgBox, TWindow)
};

#pragma CIDLIB_POPPACK


