//
// FILE NAME: CIDResEd_EditTextDlg.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/03/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This dialog is used to both edit our loadable text, and to select a particular
//  text. A particular loadable text consists of a message type, a text symbolic
//  name, and numeric id that the name maps to.
//
//  We implement the multi-column list box's auto-in-place editing mixin, so that
//  we can easily support in place editing of the message text values.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TResEdTextDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------
class TResEdTextDlg : public TDlgBox, public MMCLBIPEIntf
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TResEdTextDlg();

        ~TResEdTextDlg();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIPETestCanEdit
        (
            const   tCIDCtrls::TWndId       widSrc
            , const tCIDLib::TCard4         c4ColInd
            ,       TAttrData&              adatFill
            ,       tCIDLib::TBoolean&      bValueSet
        )   override;

        tCIDLib::TBoolean bIPEValidate
        (
            const   TString&                strSrc
            ,       TAttrData&              adatSrc
            , const TString&                strNewVal
            ,       TString&                strErrMsg
            ,       tCIDLib::TCard8&        c8UserId
        )   const override;

        tCIDLib::TVoid IPEValChanged
        (
            const   tCIDCtrls::TWndId       widSrc
            , const TString&                strSrc
            , const TAttrData&              adatNew
            , const TAttrData&              adatOld
            , const tCIDLib::TCard8         c8UserId
        )   override;



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
                    TWindow&                wndOwner
            ,       tCIDResEd::EMsgTypes&   eType
            ,       tCIDLib::TMsgId&        midMsg
            ,       TString&                strSymbol
            , const tCIDLib::TBoolean       bSelMode
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        typedef TVector <TResEdMsgList, tCIDResEd::EMsgTypes> TMTypeList;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSelMode
        //      We can be invoked in edit mode, or in selection mode. In selection
        //      mode the user can still edit, but can only select from common or
        //      message types, no errors.
        //
        //  m_colTypes
        //      We make a copy of each of the three sets of messages, so that we
        //      can store up changes and only put them back if the user commits.
        //
        //  m_eSelType
        //  m_midSel
        //      The type and id of the selected msg. If none was selected on save,
        //      or a cancel happens, the type will be set to the _Count value and
        //      msg id will be set to max card.
        //
        //  m_pcolCurList
        //      A pointer to the incoming list of exitsing dialogs, to store
        //      it until we can get the list loaded.
        //
        //  m_strSymbol
        //      The place to store symbol name of the selected msg until we can
        //      get it back to the caller's parms.
        //
        //  m_strState_XXX
        //      The names of some window states we set up.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSelMode;
        TMTypeList              m_colTypes;
        tCIDResEd::EMsgTypes    m_eSelType;
        tCIDLib::TMsgId         m_midSel;
        TPushButton*            m_pwndAddButton;
        TPushButton*            m_pwndDelButton;
        TMultiColListBox*       m_pwndList;
        TComboBox*              m_pwndTypes;
        TString                 m_strSymbol;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdTextDlg, TDlgBox)
};



