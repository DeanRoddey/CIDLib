//
// FILE NAME: CIDWUtils_AttrEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/23/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the attribute editing control window
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TAttrEditInfo,TCtrlNotify)
AdvRTTIDecls(TAttrEditWnd, TWindow)



// ---------------------------------------------------------------------------
//  CLASS: TMCListAttrOps
// PREFIX: mclops
// ---------------------------------------------------------------------------
class TMCListAttrOps : public TMCListOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TMCListAttrOps(const tCIDMData::TAttrList* const pcolList) :

            m_pcolList(pcolList)
            , m_strDropDown(L"v")
            , m_strVisSel(L"...")
        {
        }

        TMCListAttrOps(const TMCListAttrOps&) = delete;

        ~TMCListAttrOps() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMCListAttrOps& operator=(const TMCListAttrOps&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColCount() const override
        {
            return 4;
        }

        tCIDLib::TCard4 c4RowCount() const override
        {
            return m_pcolList->c4ElemCount();
        }

        tCIDLib::TCard4 c4RowId(const tCIDLib::TCard4 c4RowIndex) const override
        {
            return c4RowIndex;
        }

        const TString& strColumnText
        (
            const   tCIDLib::TCard4     c4RowIndex
            , const tCIDLib::TCard4     c4ColIndex
        )   const override;


        const tCIDMData::TAttrList* const   m_pcolList;
        const TString                       m_strDropDown;
        const TString                       m_strVisSel;
        mutable TString                     m_strTmpFmt;
};


const TString&
TMCListAttrOps::strColumnText(  const   tCIDLib::TCard4     c4RowIndex
                                , const tCIDLib::TCard4     c4ColIndex) const
{
    const TAttrData& adatCur = m_pcolList->objAt(c4RowIndex);

    switch(c4ColIndex)
    {
        case 1 :
            return adatCur.strName();

        case 2 :
            // Ask the attribute to format itself and return that
            adatCur.FormatToText(m_strTmpFmt);
            return m_strTmpFmt;

        case 3 :
            if (!adatCur.bVisEdit())
                return TString::strEmpty();

            if ((adatCur.eType() == tCIDMData::EAttrTypes::Bool)
            ||  ((adatCur.eType() == tCIDMData::EAttrTypes::String)
            &&   adatCur.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref)))
            {
                return m_strDropDown;
            }
             else
            {
                return m_strVisSel;
            }

        default :
            break;
    };
    return TString::strEmpty();
}




// ---------------------------------------------------------------------------
//   CLASS: TAttrEditInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAttrEditInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TAttrEditInfo::TAttrEditInfo(const  TAttrData&          adatNew
                            , const TAttrData&          adatOld
                            , const tCIDLib::TCard8     c8UserId
                            , const TWindow&            wndSrc
                            , const tCIDLib::TBoolean   bFromUser) :

    TCtrlNotify(wndSrc)
    , m_adatNew(adatNew)
    , m_adatOld(adatOld)
    , m_bFromUser(bFromUser)
    , m_c8UserId(c8UserId)
{
}

TAttrEditInfo::~TAttrEditInfo()
{
}


// ---------------------------------------------------------------------------
//  TAttrEditInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TAttrData& TAttrEditInfo::adatNew() const
{
    return m_adatNew;
}

const TAttrData& TAttrEditInfo::adatOld() const
{
    return m_adatOld;
}

tCIDLib::TBoolean TAttrEditInfo::bFromUser() const
{
    return m_bFromUser;
}

tCIDLib::TCard8 TAttrEditInfo::c8UserId() const
{
    return m_c8UserId;
}





// ---------------------------------------------------------------------------
//  CLASS: TAttrEditWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAttrEditWnd: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TAttrEditWnd::nidChange(L"AttrEditChangeEvent");



// ---------------------------------------------------------------------------
// TAttrEditWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TAttrEditWnd::TAttrEditWnd() :

    m_pipeiToUse(nullptr)
{
}

TAttrEditWnd::~TAttrEditWnd()
{
}


// ---------------------------------------------------------------------------
//  TAttrEditWnd: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this from the parent class, since we need to do our own stuff.
//
tCIDLib::TVoid
TAttrEditWnd::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Group);

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles = tCIDLib::eOREnumBits(eExStyles, tCIDCtrls::EExWndStyles::SunkenEdge);
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Border);
    }

    CreateAttrEd
    (
        wndParent, dlgiSrc.ridChild(), dlgiSrc.areaPos(), eStyles, eExStyles
    );
}


tCIDLib::TVoid TAttrEditWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    //
    //  We don't support all of the hints that our parent does, so we don't call our
    //  parent class.
    //
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
}


// ---------------------------------------------------------------------------
//  TAttrEditWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Provide read only access to attributes by id. We just call a non-const private
//  helper that does this.
//
const TAttrData& TAttrEditWnd::adatFindById(const TString& strId) const
{
    return const_cast<TAttrEditWnd*>(this)->adatById(strId);
}


// Just check to see if the id exists
tCIDLib::TBoolean TAttrEditWnd::bAttrExists(const TString& strId)
{
    const tCIDLib::TCard4 c4Count = m_colVals.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        if (m_colVals[c4Index].strId().bCompareI(strId))
            break;
        c4Index++;
    }
    return (c4Index < c4Count);
}


//
//  Modify some aspects of an attribute after the fact. This is sometimes required
//  if one attribute's possible values or editing type changes depending on the value
//  of another attribute.
//
tCIDLib::TVoid
TAttrEditWnd::ChangeAttr(const  TString&                strIdToChange
                        , const TString&                strName
                        , const TString&                strLimits
                        , const tCIDMData::EAttrEdTypes eEdType)
{
    TAttrData& adatTar = adatById(strIdToChange);

    // Find it in the list box before we potentially change the name!
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);

    // Now udpate the data
    adatTar.strName(strName);
    adatTar.strLimits(strLimits);

    // We need to visually update the name
    SetColText(c4LBIndex, 1, adatTar.strName());

    //
    //  If the edit type is different, update that. It will require updating the
    //  third column, so we call the other method that does that.
    //
    if (eEdType != adatTar.eEditType())
        ChangeEditType(strIdToChange, eEdType);
}

tCIDLib::TVoid
TAttrEditWnd::ChangeEditType(const  TString&                strIdToChange
                            , const tCIDMData::EAttrEdTypes eToSet)
{
    TAttrData& adatTar = adatById(strIdToChange);

    if (eToSet != adatTar.eEditType())
    {
        adatTar.eEditType(eToSet);

        // Update the third column
        TString strNewText;
        if ((eToSet == tCIDMData::EAttrEdTypes::None)
        ||  (eToSet == tCIDMData::EAttrEdTypes::InPlace))
        {
            strNewText = TString::strEmpty();
        }
         else
        {
            if ((adatTar.eType() == tCIDMData::EAttrTypes::Bool)
            ||  ((adatTar.eType() == tCIDMData::EAttrTypes::String)
            &&   adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref)))
            {
                // These provide a dropdown menu
                strNewText = L"v";
            }
             else
            {
                // Else invoke a visual editor
                strNewText = L"...";
            }
        }

        const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
        SetColText(c4LBIndex, 3, strNewText);

        //
        //  In case we moved from read only to editable (or vice versa) redraw
        //  this one.
        //
        TArea areaUpdate;
        QueryRowArea(c4LBIndex, areaUpdate);
        Redraw(areaUpdate, tCIDCtrls::ERedrawFlags::Full);
    }
}

tCIDLib::TVoid
TAttrEditWnd::ChangeAttrLimits( const   TString&    strIdToChange
                                , const TString&    strLimits)
{
    TAttrData& adatTar = adatById(strIdToChange);

    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.strLimits(strLimits);

    //
    //  Make sure that the current value of this attribute is one of the indicated
    //  values. If not, then we need to update it. It will return the first value as
    //  the canonical value if the check fails.
    //
    TString strCanonVal;
    if (!MIPEIntf::bTestEnumVal(adatTar, strCanonVal))
    {
        // We need to report this change, so save the original for use below
        const TAttrData adatOld = adatTar;

        // Now update the value and the displayed value
        adatTar.SetString(strCanonVal);
        SetColText(c4LBIndex, 2, strCanonVal);

        // And send a change report
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
    }

    //
    //  If this is a string type, and the new limits are not an enum, then make sure
    //  that the third column's visual edit indicator is removed.
    //
    if ((adatTar.eType() == tCIDMData::EAttrTypes::String)
    &&  !adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref))
    {
        SetColText(c4LBIndex, 3, TString::strEmpty());
    }
}


tCIDLib::TVoid
TAttrEditWnd::ChangeAttrType(const  TString&                strIdToChange
                            , const tCIDMData::EAttrTypes   eType)
{
    TAttrData& adatTar = adatById(strIdToChange);
    adatTar.SetType(eType);
}


tCIDLib::TVoid
TAttrEditWnd::ChangeAttrUserData(const  TString&    strIdToChange
                                , const TString&    strToSet)
{
    TAttrData& adatTar = adatById(strIdToChange);
    adatTar.strUserData(strToSet);
}


//
//  WE just call our parent to create a list view window with the appropriate styles
//  for our needs. Created() will do the rest of the setup.
//
tCIDLib::TVoid
TAttrEditWnd::CreateAttrEd( const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    //
    //  Set up our columns. The first and last have no text
    tCIDLib::TStrList colTitles(4);
    colTitles.objAdd(L"");
    colTitles.objAdd(L"Name");
    colTitles.objAdd(L"Value");
    colTitles.objAdd(L"");

    CreateMCLB
    (
        wndParent
        , widThis
        , areaInit
        , colTitles
        , 0
        , tCIDCtrls::EWndStyles(eStyles)
        , tCIDCtrls::EMCLBStyles
          (
            tCIDCtrls::EMCLBStyles::FullRowSel
          )
        , eExStyles
    );
}


//
//  We get a list of key/values objects, which we load up. They key is the name of
//  the value. Value 1 is the value. Value 2 is a type, and value 3 is optionally some
//  value range info.
//
tCIDLib::TVoid
TAttrEditWnd::LoadValues(const  tCIDMData::TAttrList&   colToLoad
                        , const tCIDLib::TCard8         c8UserId)
{
    // Store the user id for later use
    m_c8UserId = c8UserId;

    // Disable redraws until we are done
    TWndPaintJanitor janPaint(this);

    // Make sure all existing values are unloaded
    RemoveAll();

    TMCListAttrOps mclopsLoad(&colToLoad);
    LoadList(mclopsLoad);

    //
    //  It worked, so save the list for our use, and make another copy for checking
    //  for changes.
    //
    m_colVals = colToLoad;
    m_colOrgVals = colToLoad;
}


//
//  In this case, unlike LoadValues above, we get a list of attributes, but we expect
//  to get the same attributes as we already have, and so we only have to update the
//  values, if they have changed, not reload the whole thing.
//
//  NOTE that we assume that only the values have changed, so that's the only thing we
//  visually update.
//
tCIDLib::TVoid TAttrEditWnd::RefreshValues(const  tCIDMData::TAttrList& colToLoad)
{
    // Do a quick check that it has the same count as what we already have
    const tCIDLib::TCard4 c4Count = colToLoad.c4ElemCount();
    if (c4Count != m_colVals.c4ElemCount())
    {
        facCIDWUtils().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kWUtilErrs::errcAEd_BadRefreshList
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Loop through them and update them
    TString strFmtVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TAttrData& adatSrc = colToLoad[c4Index];
        TAttrData& adatTar = m_colVals[c4Index];

        // Make sure we really have the same attributes
        if (adatSrc.strId() != adatTar.strId())
        {
            facCIDWUtils().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kWUtilErrs::errcAEd_BadRefreshItem
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TCardinal(c4Index)
                , adatSrc.strId()
                , adatTar.strId()
            );
        }

        // If not a separator line, handle this one
        if (adatTar.eType() != tCIDMData::EAttrTypes::Separator)
        {
            // Remember if the values are different, then copy it over
            const tCIDLib::TBoolean bSame = adatSrc.bSameValue(adatTar);
            adatTar = adatSrc;

            // If the value was different, update it
            if (!bSame)
            {
                adatTar.FormatToText(strFmtVal);
                SetColText(c4Index, 2, strFmtVal);
            }
        }
    }

    // Redraw the visible items in case
    RedrawVisible();
}


//
//  Let's the containing app install an in place editor window on us, which we use to
//  interact with the app when in place editing occurs. We don't own them so we just
//  overwrite any previous one.
//
tCIDLib::TVoid TAttrEditWnd::SetIPEHandler(MIPEIntf* const pipeiToUse)
{
    m_pipeiToUse = pipeiToUse;
}


//
//  Sometimes the application needs to update attributes directly. It may directly
//  manipulate the object in some way, or perhaps one attribute changing affects
//  another attributes limits and so forth. It also will be used in any undo system to
//  put a previous value back in a convenient way.
//
//  We find the attribute and update its value. Optionally, we report it as though the
//  user reported it (for those scenarios where the application hasn't already done
//  whatever is being set.)
//
//  If we report, we indicate it's not from the user, to help the containing application
//  avoid circular changes.
//
tCIDLib::TVoid
TAttrEditWnd::UpdateAttr(const TAttrData& adatToSet, const tCIDLib::TBoolean bReport)
{
    // Find the current value of this attribute
    TAttrData& adatTar = adatById(adatToSet.strId());

    // And set to to the new value
    adatTar = adatToSet;

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatToSet, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateArea(const  TString&            strId
                        , const TArea&              areaToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetArea(areaToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateBool(const  TString&            strId
                        , const tCIDLib::TBoolean   bToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetBool(bToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateCard(const  TString&            strId
                        , const tCIDLib::TCard4     c4ToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetCard(c4ToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateCardRange(  const   TString&            strId
                                , const tCIDLib::TCard4     c4MinToSet
                                , const tCIDLib::TCard4     c4MaxToSet
                                , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetCardRange(c4MinToSet, c4MaxToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateCard64( const   TString&            strId
                            , const tCIDLib::TCard8     c8ToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetCard64(c8ToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateChar(const  TString&            strId
                        , const tCIDLib::TCh        chToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetChar(chToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateFloat(  const   TString&            strId
                            , const tCIDLib::TFloat8    f8ToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetFloat(f8ToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateInt(const   TString&            strId
                        , const tCIDLib::TInt4      i4ToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetInt(i4ToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateIntRange(const  TString&            strId
                            , const tCIDLib::TInt4      i4MinToSet
                            , const tCIDLib::TInt4      i4MaxToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetIntRange(i4MinToSet, i4MaxToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateMemBuf( const   TString&            strId
                            , const tCIDLib::TCard4     c4Bytes
                            , const TMemBuf&            mbufToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetMemBuf(mbufToSet, c4Bytes);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdatePoint(  const   TString&            strId
                            , const TPoint&             pntToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetPoint(pntToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateRGBColor(const  TString&            strId
                            , const TRGBClr&            clrToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetRGBColor(clrToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateSize(const  TString&            strId
                        , const TSize&              szToSet
                        , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetSize(szToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateString( const   TString&            strId
                            , const TString&            strToSet
                            , const tCIDLib::TBoolean   bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetString(strToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}

tCIDLib::TVoid
TAttrEditWnd::UpdateTime(const  TString&                strId
                        , const tCIDLib::TEncodedTime   enctToSet
                        , const tCIDLib::TBoolean       bReport)
{
    TAttrData& adatTar = adatById(strId);
    const TAttrData adatOld = adatTar;
    adatTar.SetTime(enctToSet);

    // Force it to update the value column
    const tCIDLib::TCard4 c4LBIndex = c4TextToIndex(adatTar.strName(), 1);
    adatTar.FormatToText(m_strTmpFmt);
    SetColText(c4LBIndex, 2, m_strTmpFmt);

    // If asked, report it as a change
    if (bReport)
        SendEditEvent(adatTar, adatOld, kCIDLib::False);
}



// ---------------------------------------------------------------------------
//  TAttrEditWnd: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to edit a value when the user clicks on it. We provide editing
//  for the built in types. Derived classes can override this to handle his own
//  types.
//
//  We get the area of the cell clicked, which is where we invoke the inline
//  edit, in screen coordinates.
//
tCIDLib::TBoolean
TAttrEditWnd::bEditValue(       TAttrData&      adatEdit
                        , const TArea&          areaCell
                        , const tCIDLib::TCard8 c8UserId)
{
    switch(adatEdit.eType())
    {
        case tCIDMData::EAttrTypes::Area :
        case tCIDMData::EAttrTypes::Bool :
        case tCIDMData::EAttrTypes::Card :
        case tCIDMData::EAttrTypes::Card64 :
        case tCIDMData::EAttrTypes::CardRange :
        case tCIDMData::EAttrTypes::Char :
        case tCIDMData::EAttrTypes::File :
        case tCIDMData::EAttrTypes::Float :
        case tCIDMData::EAttrTypes::Int :
        case tCIDMData::EAttrTypes::IntRange :
        case tCIDMData::EAttrTypes::Point :
        case tCIDMData::EAttrTypes::RGBColor :
        case tCIDMData::EAttrTypes::Size :
        case tCIDMData::EAttrTypes::String :
        case tCIDMData::EAttrTypes::Time :
            //
            //  These are all just in place edit, though boolean and enumerated
            //  string types (with reasonable numbers of values) and time values
            //  are special cased via a menu or dialog.
            //
            bInPlaceEdit(adatEdit, areaCell);
            break;

        case tCIDMData::EAttrTypes::Separator :
            // These are non-data items, so do nothing
            break;

        default :
            return kCIDLib::False;
    };

    // Indicate we handled it
    return kCIDLib::True;
}


//
//  This is called when the user clicks the edit column on an attribute that supports
//  supports visual value selection, or invokes editing via keyboard and the
//  attribute only supports visual editing. We handle the built in types that support
//  it. Derived classes should override this, call us first and, if we return false
//  to indicate we didn't handle it, they should check for any types that they
//  handle.
//
//  The caller provides us with the area of the value cell in screen coordinates.
//  The later is for when we want to use a drop down menu instead of a dialog.
//
tCIDLib::TBoolean
TAttrEditWnd::bVisEdit(         TAttrData&      adatEdit
                        , const TArea&          areaCellScr
                        , const tCIDLib::TCard8 c8UserId)
{
    if (adatEdit.eType() == tCIDMData::EAttrTypes::Bool)
    {
        // Do a little drop down menu below this item
        TPopupMenu menuSelect(L"bool attr value");
        menuSelect.Create();
        menuSelect.AddActionItem(kCIDLib::c4MaxCard, L"False", L"False", 1000);
        menuSelect.AddActionItem(kCIDLib::c4MaxCard, L"True", L"True", 1001);
        const tCIDLib::TCard4 c4Sel = menuSelect.c4Process
        (
            *this
            , areaCellScr.pntLL()
            , tCIDLib::EVJustify::Top
            , tCIDLib::EHJustify::Left
        );

        if (c4Sel)
            adatEdit.SetBool(c4Sel == 1001);
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::File)
    {
        //
        //  We invoke the file open dialog. If there's a FileSel: limit, we
        //  set up the indicated file types, else we set up a default *.* type
        //  selection.
        //
        tCIDLib::TKVPList colTypes;
        if (adatEdit.strLimits().bStartsWith(kCIDMData::strAttrLim_FileSelPref))
        {
            TString strDescr = adatEdit.strLimits();
            strDescr.Cut(0, kCIDMData::strAttrLim_FileSelPref.c4Length());
            TString strTypeList;
            strDescr.bSplit(strTypeList, kCIDLib::chColon, kCIDLib::False);
            strDescr.StripWhitespace();

            TStringTokenizer stokTypes(&strTypeList, L", ");
            tCIDLib::TStrList colTypeList;
            const tCIDLib::TCard4 c4Cnt = stokTypes.c4BreakApart(colTypeList);

            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Cnt; c4Index++)
            {
                colTypeList[c4Index].StripWhitespace();
                colTypes.objAdd(TKeyValuePair(strDescr, colTypeList[c4Index]));
            }
        }

        if (colTypes.bIsEmpty())
            colTypes.objAdd(TKeyValuePair(L"All Files", L"*.*"));

        tCIDLib::TStrList colSelected;
        const tCIDLib::TBoolean bRes = facCIDCtrls().bOpenFileDlg
        (
            *this
            , L"Select a File"
            , adatEdit.strValue()
            , colSelected
            , colTypes
            , tCIDLib::eOREnumBits
              (
                tCIDCtrls::EFOpenOpts::FileMustExist
                , tCIDCtrls::EFOpenOpts::FileSystemOnly
                , tCIDCtrls::EFOpenOpts::StrictTypes
              )
        );

        // If a selection, put it back into the attribute
        if (bRes)
            adatEdit.SetString(colSelected[0]);
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::MLString)
    {
        //
        //  Just pop up a little multi-line editor based dialog to let them
        //  edit the text.
        //
        TString strTitle(TStrCat(L"Edit Attribute: ", adatEdit.strName()));
        TString strNew;
        if (facCIDWUtils().bEditText(*this, strTitle, adatEdit.strValue(), strNew, kCIDLib::True))
            adatEdit.SetString(strNew);
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::RGBColor)
    {
        TRGBClr rgbEdit = adatEdit.clrVal();
        if (facCIDWUtils().bSelectColor(*this, rgbEdit))
        {
            //
            //  Format it back out to the attribute. Search teh original value and
            //  see if there are any X's in it. If so, use hex radix, else decimal.
            //
            tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
            if (adatEdit.strValue().bContainsChar(L'x'))
                eRadix = tCIDLib::ERadices::Hex;
            adatEdit.SetRGBColor(rgbEdit);
        }
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::String)
    {
        //
        //  If enumerated let them select via a menu. Else just pop up a little editor to
        //  let them enter the value. It's a single line value in this case.
        //
        if (adatEdit.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref))
        {
            //
            //  It's a an enumerated string type, so we can process it here. All that
            //  matters is if its one of the enumerated values.
            //
            TString strEnum = adatEdit.strLimits();
            strEnum.Cut(0, kCIDMData::strAttrLim_EnumPref.c4Length());

            TStringTokenizer stokVals(&strEnum, L",");
            tCIDLib::TStrList colVals;
            TString strToken;
            while (stokVals.bGetNextToken(strToken))
            {
                strToken.StripWhitespace();
                colVals.objAdd(strToken);
            }

            tCIDLib::TCard4 c4Count = colVals.c4ElemCount();
            if (c4Count)
            {
                TPopupMenu menuSelect(L"enum attr value");
                menuSelect.Create();

                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    menuSelect.AddActionItem
                    (
                        kCIDLib::c4MaxCard, L"", colVals[c4Index], c4Index + 1
                    );
                }

                const tCIDLib::TCard4 c4Sel = menuSelect.c4Process
                (
                    *this
                    , areaCellScr.pntLL()
                    , tCIDLib::EVJustify::Top
                    , tCIDLib::EHJustify::Left
                );

                // If a selection. Sub one to get the index
                if (c4Sel)
                    adatEdit.SetString(colVals[c4Sel - 1]);
            }
        }
         else
        {
            TString strTitle(TStrCat(L"Edit Attribute: ", adatEdit.strName()));
            TString strNew;

            // Call the appropriate editor based on single or multi-line limit
            if (adatEdit.strLimits().bCompareI(kCIDMData::strAttrLim_SingleLine)
            ||  adatEdit.strLimits().bCompareI(kCIDMData::strAttrLim_SingleReq))
            {
                if (facCIDWUtils().bGetText(*this, strTitle, adatEdit.strValue(), strNew))
                    adatEdit.SetString(strNew);
            }
             else
            {
                if (facCIDWUtils().bEditText(*this, strTitle, adatEdit.strValue(), strNew, kCIDLib::True))
                    adatEdit.SetString(strNew);
            }
        }
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::Separator)
    {
        // These are non-data items, so do nothing
    }
     else if (adatEdit.eType() == tCIDMData::EAttrTypes::Time)
    {
        //
        //  We provide a dialog box that lets them set the time via a set of combo
        //  boxes where they set hours, minutes, and seconds.
        //
        TString strTitle(TStrCat(L"Edit Attribute: ", adatEdit.strName()));
        tCIDLib::TEncodedTime enctEdit = adatEdit.enctVal();
        if (facCIDWUtils().bEditTime(*this, strTitle, enctEdit))
            adatEdit.SetTime(enctEdit);
    }
     else
    {
        // Indicate we didn't handle this one
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TAttrEditWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We insert our columns with header text.
//
tCIDLib::TBoolean TAttrEditWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    TArea areaUs(areaWndSize());
    SetColOpts(0, 0, tCIDLib::EHJustify::Left);
    SetColOpts(1, 100, tCIDLib::EHJustify::Right, 32);
    SetColOpts(2, 120, tCIDLib::EHJustify::Left, 64);
    SetColOpts(3, 24, tCIDLib::EHJustify::Center, 24);

    //
    //  Set custom draw options. These are the phases that we need to be called
    //  back on.
    //
    SetCustomDrawOpts
    (
        tCIDLib::eOREnumBits
        (
            tCIDCtrls::EMCLBCustOpts::PreRow
            , tCIDCtrls::EMCLBCustOpts::PostDraw
            , tCIDCtrls::EMCLBCustOpts::PostRow
            , tCIDCtrls::EMCLBCustOpts::PreCol
        )
    );

    // We need to see arrow keys
    SetWantInput(tCIDCtrls::EWantInputs::Arrows);

    // Let creation continue
    return kCIDLib::True;
}


//
//  We need to process various key combinations to provide hot keys for doing various
//  editing operations.
//
tCIDLib::TBoolean
TAttrEditWnd::bExtKey(  const   tCIDCtrls::EExtKeys eKey
                        , const tCIDLib::TBoolean   bAlt
                        , const tCIDLib::TBoolean   bCtrl
                        , const tCIDLib::TBoolean   bShift
                        , const TWindow* const      pwndChild)
{
    // No alt combos are used by us
    if (bAlt)
        return kCIDLib::False;


    // See if we have a current selection. If not, do nothing
    const tCIDLib::TCard4 c4Id = c4CurItemId();
    if (c4Id == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    // We do so get the attribute
    TAttrData& adatTar = m_colVals[c4Id];

    // If it's read only, don't allow it to be changed
    if (adatTar.bReadOnly())
        return kCIDLib::False;

    // Save a copy of the original value
    const TAttrData adatOrg = adatTar;

    if (!bShift && !bCtrl)
    {
        // We do F2 and left/right arrows here
        if (eKey == tCIDCtrls::EExtKeys::F2)
        {
            //
            //  Let's see if we can invoke inplace editing, or have to invoke the
            //  interactive type editor, favoring inplace. Though they guys don't
            //  directly use our attribute object, we are looking into the list so
            //  we will still see the changes below if any are made.
            //
            const tCIDLib::TCard4 c4RowInd = c4IdToIndex(c4Id);
            if ((adatTar.eEditType() == tCIDMData::EAttrEdTypes::InPlace)
            ||  (adatTar.eEditType() == tCIDMData::EAttrEdTypes::Both))
            {
                EditValue(c4RowInd);
            }
             else if (adatTar.eEditType() == tCIDMData::EAttrEdTypes::Visual)
            {
                VisEdit(c4RowInd);
            }
        }
         else if ((eKey == tCIDCtrls::EExtKeys::Left)
              ||  (eKey == tCIDCtrls::EExtKeys::Right))
        {
            // Pass true for previous, false for next
            DoPrevNextKey(adatTar, eKey == tCIDCtrls::EExtKeys::Left);
        }
         else
        {
            return kCIDLib::False;
        }
    }
     else if (bShift && bCtrl)
    {
        // We do Ctrl-Shift-arrow keys
        if ((eKey == tCIDCtrls::EExtKeys::Down)
        ||  (eKey == tCIDCtrls::EExtKeys::Left)
        ||  (eKey == tCIDCtrls::EExtKeys::Right)
        ||  (eKey == tCIDCtrls::EExtKeys::Up))
        {
            DoIncDecXYKey(adatTar, eKey);
        }
         else
        {
            return kCIDLib::False;
        }
    }
     else if (!bShift && bCtrl)
    {
        // We do Ctrl-arrow keys
        if ((eKey == tCIDCtrls::EExtKeys::Down)
        ||  (eKey == tCIDCtrls::EExtKeys::Left)
        ||  (eKey == tCIDCtrls::EExtKeys::Right)
        ||  (eKey == tCIDCtrls::EExtKeys::Up))
        {
            DoIncDecCXYKey(adatTar, eKey);
        }
         else
        {
            return kCIDLib::False;
        }
    }
     else
    {
        return kCIDLib::False;
    }


    // If we set a value, we need to update
    if (adatTar != adatOrg)
    {
        // We need to set the new value into the value column of the target row
        TString strFmtVal;
        adatTar.FormatToText(strFmtVal);
        SetColText(c4IdToIndex(c4Id), 2, strFmtVal);

        // Report this change, as a change from the user
        SendEditEvent(adatTar, adatOrg, kCIDLib::True);
    }

    return kCIDLib::True;
}



//
//  We override this to handle in-place editing. If it's the left button and
//  the row clicked is the same as currently selected, and the column is one we
//  allow editing on, then we do the IPE.
//
tCIDLib::TVoid
TAttrEditWnd::CellClicked(  const   tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bLeftButton)
{
    if (bLeftButton)
    {
        if (c4CurItem() == c4Row)
        {
            //
            //  We only care about columsn 2 or 3. 2 is the actual value, which
            //  we do an inplace edit on.
            //
            if (c4Column == 2)
                EditValue(c4Row);
            else if (c4Column == 3)
                VisEdit(c4Row);
        }
    }
}


//
//  Handle custom drawing. We use it to draw a grid around our cells, and to bold
//  the font in the name column.
//
tCIDCtrls::EMCLBCustRets
TAttrEditWnd::eCustomDraw(          TGraphDrawDev&      gdevTar
                            , const tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bPost
                            , const TArea&              areaAt
                            ,       TRGBClr&            rgbBgn
                            ,       TRGBClr&            rgbText)
{
    tCIDCtrls::EMCLBCustRets eRet = tCIDCtrls::EMCLBCustRets::None;
    if (bPost)
    {
        if ((c4Row == kCIDLib::c4MaxCard) && (c4Column == kCIDLib::c4MaxCard))
        {
            //
            //  Its the final post draw, so draw our vertical separators. We get the
            //  area of the 2nd and 3rd cells, and those provide the right sides
            //  of the lines. And we get the area of the last row, and that provides
            //  the bottom of the line. If it's off the end of the window, we just
            //  let windows clip it.
            //
            const tCIDLib::TCard4 c4Count = c4ItemCount();
            if (c4Count)
            {
                TArea areaName;
                TArea areaVal;
                QueryColArea(0, 1, areaName);
                QueryColArea(0, 2, areaVal);

                TArea areaLast;
                QueryRowArea(c4Count - 1, areaLast);

                TPoint pntTop(areaName.i4Right(), areaName.i4Y());
                TPoint pntBottom(pntTop.i4X(), areaLast.i4Bottom());
                gdevTar.DrawLine(pntTop, pntBottom, facCIDGraphDev().rgbPaleGrey);

                pntTop.i4X(areaVal.i4Right());
                pntBottom.i4X(pntTop.i4X());
                gdevTar.DrawLine(pntTop, pntBottom, facCIDGraphDev().rgbPaleGrey);
            }
        }
         else if ((c4Row != kCIDLib::c4MaxCard) && (c4Column == kCIDLib::c4MaxCard))
        {
            // It's post row, so we draw a line under our row
            TArea areaRow;
            QueryRowArea(c4Row, areaRow);
            gdevTar.DrawLine(areaRow.pntLL(), areaRow.pntLR(), facCIDGraphDev().rgbPaleGrey);
        }
    }
     else
    {
        if ((c4Row < m_colVals.c4ElemCount()) && (c4Column == kCIDLib::c4MaxCard))
        {
            //
            //  It's pre-row. If this one is a separator, we want to color fill it.
            //  If it's a disbled value, then we set the text color to a pale
            //  grey to indicate it's not used.
            //
            if (m_colVals[c4Row].eType() == tCIDMData::EAttrTypes::Separator)
            {
                rgbBgn = facCIDGraphDev().rgbPaleGrey;
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewBgnClr);
            }
             else if (m_colVals[c4Row].bReadOnly())
            {
                rgbText = facCIDGraphDev().rgbPaleGrey;
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewTextClr);
            }
        }
         else
        {
            // For columns 1 dn 3, set bold font, else for a valid column set normal
            if ((c4Column == 1) || (c4Column == 3))
            {
                gdevTar.hfontSetFont(TGUIFacility::gfontDefBold());
                eRet = tCIDCtrls::EMCLBCustRets::NewFont;
            }
             else if (c4Column != kCIDLib::c4MaxCard)
            {
                gdevTar.hfontSetFont(TGUIFacility::gfontDefNorm());
                eRet = tCIDCtrls::EMCLBCustRets::NewFont;
            }
        }
    }
    return eRet;
}


// ---------------------------------------------------------------------------
//  TAttrEditWnd: Private, non-virtaul methods
// ---------------------------------------------------------------------------

// Provide access to attributes by id
TAttrData& TAttrEditWnd::adatById(const TString& strId)
{
    // Find the attribute in our attribute list
    const tCIDLib::TCard4 c4Count = m_colVals.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        if (m_colVals[c4Index].strId().bCompareI(strId))
            break;
        c4Index++;
    }

    if (c4Index >= c4Count)
    {
        facCIDWUtils().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kWUtilErrs::errcAEd_IdNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strId
        );
    }
    return m_colVals[c4Index];
}


//
//  Where possible EditValue() will just present a simple drop down to set a value,
//  and some application specific types (processed via a virtual callback) will
//
tCIDLib::TBoolean
TAttrEditWnd::bInPlaceEdit(TAttrData& adatEdit, const TArea& areaCell)
{
    //
    //  Create an inplace editor object and invoke it. All validation will be done on
    //  the value before it returns. It will use the passed IPE interface object to
    //  validate the entered value and, if valid, to pass it to the client code for
    //  storage. We set the current row on the IPE object first, so that it's available
    //  in the callbacks.
    //
    m_pipeiToUse->c4IPERow(c4CurItem());
    TInPlaceEdit ipeVal;
    return ipeVal.bProcess(*this, areaCell, adatEdit, *m_pipeiToUse, m_c8UserId);
}



//
//  This is called when we get a ctrl-shifted arrow key. We use those combos to adjust
//  the size of the selected widget in the indicated direction.
//
tCIDLib::TVoid
TAttrEditWnd::DoIncDecCXYKey(TAttrData& adatTar, const tCIDCtrls::EExtKeys eKey)
{
    // It can be an area or a size, so process based on the attribute type.
    if ((adatTar.eType() == tCIDMData::EAttrTypes::Size)
    ||  (adatTar.eType() == tCIDMData::EAttrTypes::Area))
    {
        // Get the size we will adjust
        TSize szAdj;
        if (adatTar.eType() == tCIDMData::EAttrTypes::Size)
            szAdj = adatTar.szVal();
        else
            szAdj = adatTar.areaVal().szArea();

        if (eKey == tCIDCtrls::EExtKeys::Up)
        {
            szAdj.Adjust(0, -1);
        }
         else if (eKey == tCIDCtrls::EExtKeys::Left)
        {
           if (szAdj.c4Width())
                szAdj.Adjust(-1, 0);
        }
         else if (eKey == tCIDCtrls::EExtKeys::Down)
        {
            if (szAdj.c4Height())
                szAdj.Adjust(0, 1);
        }
         else if (eKey == tCIDCtrls::EExtKeys::Right)
        {
            szAdj.Adjust(1, 0);
        }

        // Now, based on the type, get any limits and check the new value
        if (adatTar.eType() == tCIDMData::EAttrTypes::Size)
        {
            if (adatTar.strLimits().bStartsWith(kCIDMData::strAttrLim_SizePref))
            {
                TSize szMin;
                TSize szMax(0x4000, 0x4000);
                TString strErrMsg;
                if (!TAttrData::bParseSizeLim(adatTar.strLimits(), szMin, szMax, strErrMsg))
                    return;

                if ((szAdj.c4Width() < szMin.c4Width())
                ||  (szAdj.c4Height() < szMin.c4Height())
                ||  (szAdj.c4Width() > szMax.c4Width())
                ||  (szAdj.c4Height() > szMax.c4Height()))
                {
                    return;
                }
            }
             else if (!adatTar.strLimits().bIsEmpty())
            {
                facCIDWUtils().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kWUtilErrs::errcAEd_BadLimType
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::TypeMatch
                    , adatTar.strId()
                );
                return;
            }
            adatTar.SetSize(szAdj);
        }
         else
        {
            // Don't allow them to be less than 2 pixels
            if ((szAdj.c4Width() < 2) || (szAdj.c4Height() < 2))
                return;

            TArea areaNew = adatTar.areaVal();
            areaNew.SetSize(szAdj);

            if (adatTar.strLimits().bStartsWith(kCIDMData::strAttrLim_AreaPref))
            {
                TArea areaLims;
                TString strErrMsg;
                if (!TAttrData::bParseAreaLim(adatTar.strLimits(), areaLims, strErrMsg))
                    return;

                if (!areaLims.bContainsArea(areaNew))
                    return;
            }
             else if (!adatTar.strLimits().bIsEmpty())
            {
                facCIDWUtils().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kWUtilErrs::errcAEd_BadLimType
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::TypeMatch
                    , adatTar.strId()
                );
                return;
            }
            adatTar.SetArea(areaNew);
        }
    }
}



//
//  This is called when we get an alt-shifted arrow key. We use those combos to move
//  the origin of the selected widget in the indicated direction.
//
tCIDLib::TVoid
TAttrEditWnd::DoIncDecXYKey(TAttrData& adatTar, const tCIDCtrls::EExtKeys eKey)
{
    // It can be a point or an area
    if ((adatTar.eType() == tCIDMData::EAttrTypes::Point)
    ||  (adatTar.eType() == tCIDMData::EAttrTypes::Area))
    {
        // Get the point we will adjust
        TPoint pntAdj;
        if (adatTar.eType() == tCIDMData::EAttrTypes::Point)
            pntAdj = adatTar.pntVal();
        else
            pntAdj = adatTar.areaVal().pntOrg();

        if (eKey == tCIDCtrls::EExtKeys::Left)
        {
            pntAdj.Adjust(-1, 0);
        }
         else if (eKey == tCIDCtrls::EExtKeys::Up)
        {
            pntAdj.Adjust(0, -1);
        }
         else if (eKey == tCIDCtrls::EExtKeys::Right)
        {
            pntAdj.Adjust(1, 0);
        }
        else if (eKey == tCIDCtrls::EExtKeys::Down)
        {
            pntAdj.Adjust(0, 1);
        }

        //
        //  Check it against any limits. Areas could have either an area or a size
        //  limit. Points can have an area limit. However, in this case we have not
        //  changed the size (if it's an area) so we only need to check for the
        //  area limit in either case.
        //
        tCIDLib::TBoolean bHaveLims = kCIDLib::False;
        TArea areaLims;
        if (adatTar.strLimits().bStartsWith(kCIDMData::strAttrLim_AreaPref))
        {
            TString strErrMsg;
            if (!TAttrData::bParseAreaLim(adatTar.strLimits(), areaLims, strErrMsg))
                return;
            bHaveLims = kCIDLib::True;
        }
         else if (!adatTar.strLimits().bIsEmpty())
        {
            facCIDWUtils().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kWUtilErrs::errcAEd_BadLimType
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::TypeMatch
                , adatTar.strId()
            );
            return;
        }

        if (adatTar.eType() == tCIDMData::EAttrTypes::Point)
        {
            if (bHaveLims && !areaLims.bContainsPoint(pntAdj))
                return;
            adatTar.SetPoint(pntAdj);
        }
         else
        {
            TArea areaNew = adatTar.areaVal();
            areaNew.SetOrg(pntAdj);

            if (bHaveLims && !areaLims.bContainsArea(areaNew))
                return;
            adatTar.SetArea(areaNew);
        }
    }
}


//
//  Called when we get an unsifted left/right arrow. If we are on a value that has
//  some known sequence of values, we can inc/dec the value. We update the attribute
//  value if we can.
//
tCIDLib::TVoid
TAttrEditWnd::DoPrevNextKey(TAttrData& adatTar, const tCIDLib::TBoolean bPrevious)
{
    //
    //  If it's a boolean or an enumerated string, we will move to the next or
    //  previous value.
    //
    const TString strOrgVal = adatTar.strValue();
    if (adatTar.eType() == tCIDMData::EAttrTypes::Bool)
    {
        // The direction doesn't matter, just flip to the other
        adatTar.SetBool(!adatTar.bVal());
    }
     else if ((adatTar.eType() == tCIDMData::EAttrTypes::Card)
          ||  (adatTar.eType() == tCIDMData::EAttrTypes::Card64))
    {
        const tCIDLib::TBoolean b32(adatTar.eType() == tCIDMData::EAttrTypes::Card);

        // Get the limits, if any
        tCIDLib::TCard8 c8Min = 0;
        tCIDLib::TCard8 c8Max = b32 ? kCIDLib::c4MaxCard : kCIDLib::c8MaxCard;
        if (adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_RangePref))
        {
            TString strMin = adatTar.strLimits();
            strMin.Cut(0, kCIDMData::strAttrLim_RangePref.c4Length());
            TString strMax;
            if (strMin.bSplit(strMax, kCIDLib::chComma))
            {
                strMax.StripWhitespace();
                strMin.StripWhitespace();
                c8Min = strMin.c8Val();
                c8Max = strMax.c8Val();
            }
        }

        // Now move the value up or down if not at the range limit
        tCIDLib::TCard8 c8NewVal = b32 ? adatTar.c4Val() : adatTar.c8Val();
        const tCIDLib::TCard8 c8OrgVal = c8NewVal;
        if (bPrevious)
        {
            if (c8NewVal > c8Min)
                c8NewVal--;
        }
         else
        {
            if (c8NewVal < c8Max)
                c8NewVal++;
        }

        if (c8NewVal != c8OrgVal)
        {
            if (b32)
                adatTar.SetCard(tCIDLib::TCard4(c8NewVal));
            else
                adatTar.SetCard64(c8NewVal);
        }
    }
     else if (adatTar.eType() == tCIDMData::EAttrTypes::Char)
    {
        // If there's a character range limit, we move through those
        if (adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_CharPref))
        {
            TString strTmpLim(adatTar.strLimits());
            strTmpLim.Cut(0, kCIDMData::strAttrLim_CharPref.c4Length());

            //
            //  Find the current value within the limit range. If not in the
            //  range, go to the first one.
            //
            tCIDLib::TCard4 c4LimInd = 0;
            if (!strTmpLim.bFirstOccurrence(adatTar.chVal(), c4LimInd))
                c4LimInd = 0;

            if (bPrevious)
            {
                if (c4LimInd)
                    c4LimInd++;
                else
                    c4LimInd = strTmpLim.c4Length() - 1;
            }
             else
            {
                if (c4LimInd + 1 < strTmpLim.c4Length())
                    c4LimInd++;
                else
                    c4LimInd = 0;
            }

            if (strTmpLim[c4LimInd] != adatTar.chVal())
                adatTar.SetChar(strTmpLim[c4LimInd]);
        }
    }
     else if (adatTar.eType() == tCIDMData::EAttrTypes::Int)
    {
        // Get the limits, if any
        tCIDLib::TInt4 i4Min = kCIDLib::i4MinInt;
        tCIDLib::TInt4 i4Max = kCIDLib::i4MaxInt;
        if (!adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_RangePref))
        {
            TString strMin = adatTar.strLimits();
            strMin.Cut(0, kCIDMData::strAttrLim_RangePref.c4Length());
            TString strMax;
            if (strMin.bSplit(strMax, kCIDLib::chComma))
            {
                strMax.StripWhitespace();
                strMin.StripWhitespace();
                i4Min = strMin.i4Val();
                i4Max = strMax.i4Val();
            }
        }

        // Now move the value up or down if not at the range limit
        tCIDLib::TInt4 i4NewVal = adatTar.i4Val();
        const tCIDLib::TInt4 i4OrgVal = i4NewVal;
        if (bPrevious)
        {
            if (i4NewVal > i4Min)
                i4NewVal--;
        }
         else
        {
            if (i4NewVal < i4Max)
                i4NewVal++;
        }

        if (i4NewVal != i4OrgVal)
            adatTar.SetInt(i4NewVal);
    }
     else if ((adatTar.eType() == tCIDMData::EAttrTypes::String)
          &&  adatTar.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref))
    {
        // Pull all the values out into a vector
        TString strEnum = adatTar.strLimits();
        strEnum.Cut(0, kCIDMData::strAttrLim_EnumPref.c4Length());

        TStringTokenizer stokVals(&strEnum, L",");
        TString strToken;
        tCIDLib::TStrList colVals;
        tCIDLib::TCard4 c4NewInd = 0;
        while (stokVals.bGetNextToken(strToken))
        {
            strToken.StripWhitespace();
            if (strToken.bCompareI(strOrgVal))
                c4NewInd = colVals.c4ElemCount();
            colVals.objAdd(strToken);
        }

        if (!colVals.bIsEmpty())
        {
            // Adjust the index according to direction
            const tCIDLib::TCard4 c4OrgInd = c4NewInd;
            if (bPrevious)
            {
                if (c4NewInd > 0)
                    c4NewInd--;
                else
                    c4NewInd = colVals.c4ElemCount() - 1;
            }
             else
            {
                c4NewInd++;
                if (c4NewInd >= colVals.c4ElemCount())
                    c4NewInd = 0;
            }

            if (c4OrgInd != c4NewInd)
                adatTar.SetString(colVals[c4NewInd]);
        }
    }
}



//
//  This is called when the user clicks on column 2 of an item. This is the value of
//  the item. We look at the type of that item and do the appropriate thing to let the
//  user edit it.
//
//  As much as possible, we do in-place editing, but in some cases we might have to do
//  a dialog or something.
//
tCIDLib::TVoid TAttrEditWnd::EditValue(const tCIDLib::TCard4 c4Item)
{
    // Get the original index of the item
    tCIDLib::TCard4 c4Id = c4IndexToId(c4Item);

    // Now we can get the original information
    TAttrData& adatTar = m_colVals[c4Id];

    // If this one doesn't support inline editing, do nothing
    if (!adatTar.bInPlaceEdit())
        return;

    //  Get the area cell, which is where inplace editing will happen
    TArea areaCell;
    QueryColArea(c4Item, 2, areaCell);

    // Save the original value for reporting and to know if we changed it
    const TAttrData adatOrg = adatTar;

    //
    //  If no one handled it, complain and give up. If it was handled, and the
    //  the value changed, then handle it.
    //
    if (!bEditValue(adatTar, areaCell, m_c8UserId))
    {
        TErrBox msgbErr
        (
            L"Attribute Editor"
            , TStrCat
              (
                L"No editor is available for data type: "
                , tCIDMData::strXlatEAttrTypes(adatTar.eType())
              )
        );
        msgbErr.ShowIt(*this);
    }
     else if (!adatTar.bSameValue(adatOrg))
    {
        // Update the column cell with the new value and send a change notification.
        TString strFmtVal;
        adatTar.FormatToText(strFmtVal);
        SetColText(c4Item, 2, strFmtVal);

        // Report the change as from the user
        SendEditEvent(adatTar, adatOrg, kCIDLib::True);
    }
}



//
//  A helper for derived classes to call when they want to send out a list event. These
//  are all done asynchronously, since they are just reporting a change in data, and the
//  caller might respond with changing some other attribute so we want to get back out of
//  here before that happens.
//
tCIDLib::TVoid
TAttrEditWnd::SendEditEvent(const   TAttrData&          adatSrc
                            , const TAttrData&          adatOrg
                            , const tCIDLib::TBoolean   bFromUser)
{
    SendAsyncNotify
    (
        new TAttrEditInfo(adatSrc, adatOrg, m_c8UserId, *this, bFromUser)
        , TAttrEditWnd::nidChange
    );
}



//
//  This is called when the user clicks on column 3 of an item. If it's an attr
//  that supported visual editing, we invoke the visual editing callback.
//
tCIDLib::TVoid TAttrEditWnd::VisEdit(const tCIDLib::TCard4 c4Item)
{
    // Get the original index of the item
    const tCIDLib::TCard4 c4OrgIndex = c4IndexToId(c4Item);

    // Shouldn't be invalid, but be sure
    if (c4Item >= m_colVals.c4ElemCount())
    {
        // <TBD> log a msg
        return;
    }

    // Now we can get the original information
    TAttrData& adatTar = m_colVals[c4Item];

    // If it doesn't support visual editing, do nothing
    if (!adatTar.bVisEdit())
        return;

    // Get the area cell, which is where inplace editing will happen
    TArea areaCell;
    QueryColArea(c4Item, 2, areaCell);
    TArea areaCellScr;
    ToScreenCoordinates(areaCell, areaCellScr);

    // Save the original value for reporting and to know if we changed it
    const TAttrData adatOrg = adatTar;

    //
    //  If not one handled it, complain and give up. If it was handled, and the
    //  the value changed, then handle it.
    //
    if (!bVisEdit(adatTar, areaCellScr, m_c8UserId))
    {
        TErrBox msgbErr
        (
            L"Attribute Editor"
            , TStrCat
              (
                L"No visual editor is available for data type: "
                , tCIDMData::strXlatEAttrTypes(adatTar.eType())
              )
        );
        msgbErr.ShowIt(*this);
    }
     else
    {
        //
        //  If the value changed, or this guy is marked as always report, then let's
        //  send an event notification. This one is obviously from the user.
        //
        if (!adatTar.bSameValue(adatOrg) || adatTar.bAlwaysReport())
        {
            TString strFmtVal;
            adatTar.FormatToText(strFmtVal);

            SetColText(c4Item, 2, strFmtVal);
            SendEditEvent(adatTar, adatOrg, kCIDLib::True);
        }
    }
}


