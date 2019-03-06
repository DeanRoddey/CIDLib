//
// FILE NAME: CIDResEd_NewDlgDlg.cpp
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
//  This module implements the about dialog class for the dialog editor.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdTextDlg, TDlgBox)



// ---------------------------------------------------------------------------
//  CLASS: TResEdTextDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdTextDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdTextDlg::TResEdTextDlg() :

    m_bSelMode(kCIDLib::True)
    , m_colTypes()
    , m_eSelType(tCIDResEd::EMsgTypes::Count)
    , m_midSel(0)
    , m_pwndAddButton(nullptr)
    , m_pwndDelButton(nullptr)
    , m_pwndList(nullptr)
    , m_pwndTypes(nullptr)
{
}

TResEdTextDlg::~TResEdTextDlg()
{
    // Destroy ourself
    Destroy();
}


// ---------------------------------------------------------------------------
//  TResEdTextDlg: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  If not on common text, and it's not the id, then we allow it. There's no purpose
//  to edit the id since they are automatically assigned. We don't set the value
//  here, we just let the MC list box just load up the current column contents to
//  edit. That's fine as long as the attribute type is string, which it is in this
//  case since it's text we are editing.
//
tCIDLib::TBoolean
TResEdTextDlg::bIPETestCanEdit( const   tCIDCtrls::TWndId
                                , const tCIDLib::TCard4     c4ColInd
                                ,       TAttrData&          adatFill
                                ,       tCIDLib::TBoolean&  bValueSet)
{
    if ((tCIDResEd::EMsgTypes(m_pwndTypes->c4CurItem()) != tCIDResEd::EMsgTypes::Common)
    &&  (c4ColInd != 1))
    {
        if (c4ColInd == 0)
        {
            adatFill.Set(L"Symbol", L"Symbol", tCIDMData::EAttrTypes::String);
        }
         else if (c4ColInd == 2)
        {
            adatFill.Set(L"Text", L"Text", tCIDMData::EAttrTypes::String);
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TResEdTextDlg::bIPEValidate(const   TString&            strSrc
                            ,       TAttrData&          adatTar
                            , const TString&            strNewVal
                            ,       TString&            strErrMsg
                            ,       tCIDLib::TCard8&    c8UserId) const
{
    //
    //  Call the base mixin class to do basic validation. If that fails, then
    //  no need to look further.
    //
    if (!MIPEIntf::bIPEValidate(strSrc, adatTar, strNewVal, strErrMsg, c8UserId))
        return kCIDLib::False;

    const tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes(m_pwndTypes->c4CurItem());
    const TResEdMsgList& remilType = m_colTypes[eType];
    const TResEdMsgItem& remitEd = remilType.remitById(m_pwndList->c4IndexToId(c4IPERow()));

    if (adatTar.strId() == L"Symbol")
    {
        // If the symbol isn't the same but matches something, it's taken
        if ((remitEd.strName() != strNewVal) && remilType.premitByName(strNewVal))
        {
            strErrMsg = L"This symbolic name is already taken";
            return kCIDLib::False;
        }
        adatTar.SetString(strNewVal);
    }
     else if (adatTar.strId() == L"Text")
    {
        // The text, we don't care what it is, though it can't be empty
        if (strNewVal.bIsEmpty())
        {
            strErrMsg = L"The text cannot be empty";
            return kCIDLib::False;
        }
        adatTar.SetString(strNewVal);
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TResEdTextDlg::IPEValChanged(const  tCIDCtrls::TWndId   widSrc
                            , const TString&            strSrc
                            , const TAttrData&          adatNew
                            , const TAttrData&          adatOld
                            , const tCIDLib::TCard8     )
{
    // Look up the msg item
    const tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes(m_pwndTypes->c4CurItem());
    TResEdMsgList& remilType = m_colTypes[eType];

    //
    //  Based on the selected list row, get the msg id, and use that to find the
    //  index without our msg list.
    //
    const tCIDLib::TCard4 c4MsgId = m_pwndList->c4IndexToId(c4IPERow());
    const tCIDLib::TCard4 c4ListInd = remilType.c4FindById(c4MsgId);

    // Get a copy of the item we can modify
    TResEdMsgItem remitEd = remilType.remitAt(c4ListInd);

    if (adatNew.strId() == L"Symbol")
    {
        remitEd.strName(adatNew.strValue());
    }
     else if (adatNew.strId() == L"Text")
    {
        remitEd.strMsgText(adatNew.strValue());
    }
     else
    {
        CIDAssert2(L"Invalid attribute key");
    }

    remilType.c4UpdateAt(c4ListInd, remitEd);
}


// ---------------------------------------------------------------------------
//  TResEdTextDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TResEdTextDlg::bRunDlg( TWindow&                    wndOwner
                        , tCIDResEd::EMsgTypes&     eIOType
                        , tCIDLib::TMsgId&          midIO
                        , TString&                  strSymbol
                        , const tCIDLib::TBoolean   bSelMode)
{
    // Save the incoming stuff for later use.
    m_bSelMode = bSelMode;
    m_eSelType = eIOType;
    m_midSel = midIO;

    if (m_eSelType >= tCIDResEd::EMsgTypes::Count)
        m_eSelType = tCIDResEd::EMsgTypes::Message;

    //
    //  We make copies of all of the msg lists. Though we will only allow two of
    //  them to be edited.
    //
    tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes::Min;
    while (eType < tCIDResEd::EMsgTypes::Count)
    {
        m_colTypes.objAdd(facCIDResEd.wndMain().remilByType(eType));
        eType++;
    }

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDResEd, kCIDResEd::ridDlg_EditText
    );

    if (c4Res && (c4Res != kCIDLib::c4MaxCard))
    {
        eIOType = m_eSelType;
        midIO = m_midSel;
        strSymbol = m_strSymbol;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TResEdTextDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TResEdTextDlg::bCreated()
{
    // Return our parent's return, which indicates if he set the focus or not
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Get our typed pointers to important controls
    CastChildWnd(*this, kCIDResEd::ridDlg_EditText_Add, m_pwndAddButton);
    CastChildWnd(*this, kCIDResEd::ridDlg_EditText_Delete, m_pwndDelButton);
    CastChildWnd(*this, kCIDResEd::ridDlg_EditText_List, m_pwndList);
    CastChildWnd(*this, kCIDResEd::ridDlg_EditText_MsgTypes, m_pwndTypes);
    TPushButton* pwndSave = pwndChildAs<TPushButton>(kCIDResEd::ridDlg_NewDlg_Save);
    TPushButton* pwndCancel = pwndChildAs<TPushButton>(kCIDResEd::ridDlg_NewDlg_Cancel);

    // Set our column names
    tCIDLib::TStrList colColTitles(3);
    colColTitles.objAdd(L"Symbol");
    colColTitles.objAdd(L"Id");
    colColTitles.objAdd(L"Text");
    m_pwndList->SetColumns(colColTitles);

    // Set the first two column widths. The last will auto-size
    m_pwndList->SetColOpts(0, TGUIFacility::c4AdjustHDlgUnit(100));
    m_pwndList->SetColOpts(1, TGUIFacility::c4AdjustHDlgUnit(28));

    //
    //  Load up the message types. We put them in the order of the message types enum,
    //  and this list is not sorted, so the selection index should map back to the
    //  message type.
    //
    TVector<TString, tCIDResEd::EMsgTypes> colTypes;
    tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes::Min;
    while (eType < tCIDResEd::EMsgTypes::Count)
    {
        colTypes.objAdd(tCIDResEd::strLoadEMsgTypes(eType));
        eType++;
    }
    m_pwndTypes->LoadList(colTypes);

    // Install event handlers for what we need
    m_pwndAddButton->pnothRegisterHandler(this, &TResEdTextDlg::eClickHandler);
    m_pwndDelButton->pnothRegisterHandler(this, &TResEdTextDlg::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TResEdTextDlg::eListHandler);
    m_pwndTypes->pnothRegisterHandler(this, &TResEdTextDlg::eListHandler);
    pwndCancel->pnothRegisterHandler(this, &TResEdTextDlg::eClickHandler);
    pwndSave->pnothRegisterHandler(this, &TResEdTextDlg::eClickHandler);

    // If in select mode, set the save button to Select
    if (m_bSelMode)
        pwndSave->strWndText(L"Select");

    //
    //  Select the type of the incoming symbol. Force the sending of a event even if
    //  the selection didn't change, to make sure the msg list gets loaded.
    //
    m_pwndTypes->c4SelectByText(tCIDResEd::strLoadEMsgTypes(m_eSelType), kCIDLib::True);

    //
    //  If the the message id is set, then select that. Force a selection so that
    //  we get an event even if the selection doesn't change, to insure that the
    //  current msg editing controls get updated.
    //
    if (m_midSel)
    {
        const tCIDLib::TCard4 c4MsgInd = m_pwndList->c4SelectById
        (
            m_midSel, kCIDLib::False, kCIDLib::True
        );

        if ((c4MsgInd == kCIDLib::c4MaxCard) && m_bSelMode)
        {
            //
            //  If in selection mode, and the incoming message id isn't found,
            //  then warn the user and reset the incoming values. so he has to
            //  select a new one.
            //
            TErrBox msgbNoFnd(L"The incoming msg id was not found");
            msgbNoFnd.ShowIt(*this);

            m_eSelType = tCIDResEd::EMsgTypes::Count;
            m_midSel = kCIDLib::c4MaxCard;
            m_strSymbol.Clear();
        }
    }

    // Install us as the auto-in place editor on the list window
    m_pwndList->SetAutoIPE(this);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TResEdTextDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TResEdTextDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDResEd::ridDlg_EditText_Add)
    {
        //
        //  We just put one in the list with default values, and the user can
        //  in place edit it as desired.
        //
        tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes(m_pwndTypes->c4CurItem());
        TResEdMsgList& remilLoad = m_colTypes[eType];

        const tCIDLib::TCard4 c4NewId = remilLoad.c4NextId();
        TString strNewName;
        if (eType == tCIDResEd::EMsgTypes::Message)
            strNewName = L"midDlg_Id";
        else
            strNewName = L"errcId_Id";
        strNewName.AppendFormatted(c4NewId);

        if (remilLoad.premitByName(strNewName))
        {
            TErrBox msgbErr(L"Default name already exists");
            msgbErr.ShowIt(*this);
        }
         else
        {
            remilLoad.c4AddNew
            (
                c4NewId, strNewName, L"Some text"
            );

            const TResEdMsgItem* premitNew = remilLoad.premitById(c4NewId);

            TMCListMsgOps mclopsAdd(premitNew);
            m_pwndList->LoadList(mclopsAdd, kCIDLib::True);
            m_pwndList->c4SelectById(c4NewId);
        }
    }
     else if (wnotEvent.widSource() == kCIDResEd::ridDlg_EditText_Cancel)
    {
        EndDlg(0);
    }
     else if (wnotEvent.widSource() == kCIDResEd::ridDlg_EditText_Save)
    {
        // Put back any changes
        facCIDResEd.wndMain().StoreMsgText
        (
            m_colTypes[tCIDResEd::EMsgTypes::Error]
            , m_colTypes[tCIDResEd::EMsgTypes::Message]
        );

        // Get the message type selected and the id of the selected msg
        m_eSelType = tCIDResEd::EMsgTypes(m_pwndTypes->c4CurItem());
        CIDAssert(m_eSelType < tCIDResEd::EMsgTypes::Count, L"Invalid msg type from combo");

        const tCIDLib::TCard4 c4ListInd = m_pwndList->c4CurItem();
        if (c4ListInd == kCIDLib::c4MaxCard)
            m_midSel = kCIDLib::c4MaxCard;
        else
            m_midSel = m_pwndList->c4IndexToId(c4ListInd);

        // If in selection mode, it can't be errors or no message
        if (m_bSelMode
        &&  ((m_eSelType == tCIDResEd::EMsgTypes::Error)
        ||   (m_midSel == kCIDLib::c4MaxCard)))
        {
            TErrBox msgbErr(L"A non-error message must be selected");
            msgbErr.ShowIt(*this);
        }
         else
        {
            // Build up the symbol name for return
            if (m_bSelMode)
            {
                const TResEdMsgItem& remitCur = m_colTypes[m_eSelType].remitById(m_midSel);

                m_strSymbol = remitCur.strName();

                if (m_eSelType == tCIDResEd::EMsgTypes::Common)
                {
                    // Cut off the prefix and put brackets around it
                    m_strSymbol.Cut(0, 8);
                    m_strSymbol.Insert(L'[', 0);
                    m_strSymbol.Append(L']');
                }
            }

            EndDlg(1);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//  We handle events here from the types combo box and the text list.
//
tCIDCtrls::EEvResponses TResEdTextDlg::eListHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDResEd::ridDlg_EditText_MsgTypes)
    {
        if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
        {
            // When the selection in the message types changes, reload ourlist
            const tCIDResEd::EMsgTypes eType = tCIDResEd::EMsgTypes(wnotEvent.c4Index());

            const TResEdMsgList& remilLoad = m_colTypes[eType];
            TMCListMsgOps mclopsLoad(&remilLoad);
            m_pwndList->LoadList(mclopsLoad);
        }
    }
     else if (wnotEvent.widSource() == kCIDResEd::ridDlg_EditText_List)
    {
        //
        //  Enable the delete button if there's a selection, else disable. If
        //  we are on the common text type, both add and delete were disabled when
        //  it was selected and we don't change that.
        //
        if (tCIDResEd::EMsgTypes(wnotEvent.c4Index()) != tCIDResEd::EMsgTypes::Common)
        {
            if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Cleared)
                m_pwndDelButton->SetEnable(kCIDLib::False);
            else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
                m_pwndDelButton->SetEnable(kCIDLib::True);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


