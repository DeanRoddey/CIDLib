//
// FILE NAME: CIDResEd_NewDlgDlg.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/28/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This dialog use put up when the user asks to create a new dialog. We
//  need to get the name of the new one from him, and for conveneince we
//  want to show him the existing ones.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TResEdNewDlgDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------
class TResEdNewDlgDlg : public TDlgBox
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TResEdNewDlgDlg();

        ~TResEdNewDlgDlg();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
                    TWindow&                wndOwner
            , const tCIDResEd::TDlgDescList& colCurList
            ,       TString&                strToFill
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


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolCurList
        //      A pointer to the incoming list of exitsing dialogs, to store
        //      it until we can get the list loaded.
        //
        //  m_strNewName
        //      The place to store the new name until we can get it back to
        //      the caller.
        // -------------------------------------------------------------------
        const tCIDResEd::TDlgDescList*  m_pcolCurList;
        TListBox*                       m_pwndList;
        TEntryField*                    m_pwndName;
        TString                         m_strNewName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdNewDlgDlg, TDlgBox)
};


