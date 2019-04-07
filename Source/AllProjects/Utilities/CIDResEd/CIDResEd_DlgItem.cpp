//
// FILE NAME: CIDResEd_DlgItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  This file implements the dialog item class.
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdDlgItem,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TResEdDlgItem
//  PREFIX: redit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdDlgItem: Public, static methods
// ---------------------------------------------------------------------------

// Sort by name comparator
tCIDLib::ESortComps
TResEdDlgItem::eComp(const  TResEdDlgItem&  redit1
                    , const TResEdDlgItem&  redit2)
{
    return redit1.m_strSymName.eCompare(redit2.m_strSymName);
}


//
//  The code to initialize a single control. To avoid replicating this multiple
//  times.
//
tCIDLib::TVoid
TResEdDlgItem::InitCtrl(const   TResEdDlgItem&          reditInit
                        , const tCIDCtrls::EWndThemes   eTheme
                        , const TWindow&                wndParent
                        , const TWindow* const          pwndAfter)
{
    // Translate the type name from the resource desc to a class name
    TString strWndClass = facCIDCtrls().strXlatDlgClass(reditInit.strType());

    // Dynamically create a GUI widget of this type
    TCtrlWnd* pwndNew = ::pobjMakeNewOfClass<TCtrlWnd>(strWndClass);

    //
    //  Convert our dialog item into the type used by the controls/resource file
    //  and initialize the new guy.
    //
    TDlgItem dlgiNew;
    reditInit.ToDlgItem(dlgiNew);
    pwndNew->InitFromDesc(wndParent, dlgiNew, eTheme);

    // Set the dialog item's unique window name on it
    pwndNew->strName(reditInit.strWndName());

    // If it has the disabled flag, then disable it
    if (tCIDLib::bAllBitsOn(dlgiNew.eFlags(), tCIDCtrls::EDlgItemFlags::Disabled))
        pwndNew->SetEnable(kCIDLib::False);

    // Store the edit id as the window's user id for later matching them back up
    pwndNew->c4UserId(reditInit.c4EditId());

    //
    //  Move it to its appropriate index. If the the 'after' window is null, just
    //  put it at the top. Else put it after the indicated window.
    //
    if (!pwndAfter)
        pwndNew->ToTop();
    else
        pwndNew->SetZOrder(*pwndAfter);
}




// ---------------------------------------------------------------------------
//  TResEdDlgItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdDlgItem::TResEdDlgItem() :

    m_areaItem(0, 0, 64, 32)
    , m_c4EditId(facCIDResEd.c4NextEditId())
    , m_eAnchor(tCIDCtrls::EEdgeAnchors::None)
    , m_eFlags(tCIDCtrls::EDlgItemFlags::None)
    , m_objaTextSyms(tCIDResEd::ETextSyms::Count)
    , m_ridThis(0)
    , m_strWndName(L"ResEdWndName_")
{
    m_strWndName.AppendFormatted(m_c4EditId);
}

TResEdDlgItem::TResEdDlgItem(const  TString&        strType
                            , const TArea&          areaPos
                            , const tCIDLib::TResId ridThis) :
    m_areaItem(areaPos)
    , m_c4EditId(facCIDResEd.c4NextEditId())
    , m_eAnchor(tCIDCtrls::EEdgeAnchors::None)
    , m_eFlags(tCIDCtrls::EDlgItemFlags::None)
    , m_objaTextSyms(tCIDResEd::ETextSyms::Count)
    , m_ridThis(ridThis)
    , m_strType(strType)
    , m_strWndName(L"ResEdWndName_")
{
    m_strWndName.AppendFormatted(m_c4EditId);
}

TResEdDlgItem::TResEdDlgItem(const TString& strType) :

    m_areaItem(0, 0, 64, 32)
    , m_c4EditId(facCIDResEd.c4NextEditId())
    , m_eAnchor(tCIDCtrls::EEdgeAnchors::None)
    , m_eFlags(tCIDCtrls::EDlgItemFlags::None)
    , m_objaTextSyms(tCIDResEd::ETextSyms::Count)
    , m_ridThis(0)
    , m_strType(strType)
    , m_strWndName(L"ResEdWndName_")
{
    m_strWndName.AppendFormatted(m_c4EditId);
}

TResEdDlgItem::TResEdDlgItem(const TResEdDlgItem& reditSrc) :

    m_areaItem(reditSrc.m_areaItem)
    , m_c4EditId(reditSrc.m_c4EditId)
    , m_eAnchor(reditSrc.m_eAnchor)
    , m_eFlags(reditSrc.m_eFlags)
    , m_colHints(reditSrc.m_colHints)
    , m_objaTextSyms(reditSrc.m_objaTextSyms)
    , m_ridThis(reditSrc.m_ridThis)
    , m_strImage(reditSrc.m_strImage)
    , m_strSymName(reditSrc.m_strSymName)
    , m_strType(reditSrc.m_strType)
    , m_strWndName(reditSrc.m_strWndName)
{
}

TResEdDlgItem::~TResEdDlgItem()
{
}


// ---------------------------------------------------------------------------
//  TResEdDlgItem: Public operators
// ---------------------------------------------------------------------------

TResEdDlgItem& TResEdDlgItem::operator=(const TResEdDlgItem& reditSrc)
{
    if (this != &reditSrc)
    {
        m_areaItem      = reditSrc.m_areaItem;
        m_c4EditId      = reditSrc.m_c4EditId;
        m_eAnchor       = reditSrc.m_eAnchor;
        m_eFlags        = reditSrc.m_eFlags;
        m_objaTextSyms  = reditSrc.m_objaTextSyms;
        m_colHints      = reditSrc.m_colHints;
        m_ridThis       = reditSrc.m_ridThis;
        m_strImage      = reditSrc.m_strImage;
        m_strSymName    = reditSrc.m_strSymName;
        m_strType       = reditSrc.m_strType;
        m_strWndName    = reditSrc.m_strWndName;
    }
    return *this;
}

tCIDLib::TBoolean
TResEdDlgItem::operator==(const TResEdDlgItem& reditSrc) const
{
    if ((m_areaItem     != reditSrc.m_areaItem)
    ||  (m_c4EditId     != reditSrc.m_c4EditId)
    ||  (m_eAnchor      != reditSrc.m_eAnchor)
    ||  (m_eFlags       != reditSrc.m_eFlags)
    ||  (m_ridThis      != reditSrc.m_ridThis)
    ||  (m_strImage     != reditSrc.m_strImage)
    ||  (m_strSymName   != reditSrc.m_strSymName)
    ||  (m_strType      != reditSrc.m_strType)
    ||  (m_colHints     != reditSrc.m_colHints))
    {
        return kCIDLib::False;
    }

    tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms::Min;
    while (eSym < tCIDResEd::ETextSyms::Count)
    {
        if (m_objaTextSyms[eSym] != reditSrc.m_objaTextSyms[eSym])
            return kCIDLib::False;
        eSym++;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean TResEdDlgItem::operator!=(const TResEdDlgItem& reditToComp) const
{
    return !operator==(reditToComp);
}



// ---------------------------------------------------------------------------
//  TResEdDlgItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the item's area
const TArea& TResEdDlgItem::areaItem() const
{
    return m_areaItem;
}

const TArea& TResEdDlgItem::areaItem(const TArea& areaToSet)
{
    m_areaItem = areaToSet;
    return m_areaItem;
}


// See if we have the indicted hint in our hint list. If so, return the index
tCIDLib::TBoolean
TResEdDlgItem::bHaveHint(const TString& strToCheck, tCIDLib::TCard4& c4At)
{
    return TDlgItem::bFindHint(m_colHints, strToCheck, c4At);
}


//
//  Provide access to our edit id. We need a setter because if we cut and paste, the
//  newly pasted ones need new edit ids. We also have to update the window name when
//  the id changes since the id is part of it.
//
tCIDLib::TCard4 TResEdDlgItem::c4EditId() const
{
    return m_c4EditId;
}

tCIDLib::TCard4 TResEdDlgItem::c4EditId(const tCIDLib::TCard4 c4ToSet)
{
    m_c4EditId = c4ToSet;

    m_strWndName = L"ResEdWndName_";
    m_strWndName.AppendFormatted(m_c4EditId);

    return m_c4EditId;
}


// Get or set our dialog item flags
tCIDCtrls::EDlgItemFlags TResEdDlgItem::eFlags() const
{
    return m_eFlags;
}

tCIDCtrls::EDlgItemFlags
TResEdDlgItem::eFlags(const tCIDCtrls::EDlgItemFlags eToSet)
{
    m_eFlags = eToSet;
    return m_eFlags;
}


//
//  Parse ourself form the passed spooler. The caller saw the start line,
//  so he tells us what the end line for our block should be.
//
tCIDLib::TVoid
TResEdDlgItem::ParseFrom(       TResEdSpooler&  spoolSrc
                        , const TString&        strType
                        , const TString&        strEndText)
{
    // Make sure we are reset back to defaults, and get a new edit id.
    Reset();

    // Store the incoming type
    m_strType = strType;

    //
    //  Set up the list of prefixes we need to handle. We make it static so
    //  we don't need to load it again and again.
    //
    //
    //  KEEP THE TEXT symbols contiguous, since we depend on that below! AND THEY
    //  must be in the same order as the ETextSyms enum!
    //
    static tCIDLib::TStrList colPrefs;
    if (colPrefs.bIsEmpty())
    {
        colPrefs.objAdd(L"SYMBOL=");
        colPrefs.objAdd(L"AREA=");
        colPrefs.objAdd(L"HINTS=");
        colPrefs.objAdd(L"CUESYM=");
        colPrefs.objAdd(L"FLYOVERSYM=");
        colPrefs.objAdd(L"TEXTSYM=");
        colPrefs.objAdd(L"FLAGS=");
        colPrefs.objAdd(L"IMAGE=");
        colPrefs.objAdd(L"ANCHOR=");
    }

    // Process the lines until we hit the end of the block
    tCIDLib::TStrList   colStrs;
    tCIDLib::TCard4     c4Id;
    TString             strInBuf;
    TString             strTmp;
    while (kCIDLib::True)
    {
        const tCIDLib::TCard4 c4Ind = spoolSrc.c4CheckPrefix
        (
            colPrefs, strEndText, strInBuf
        );

        // If the end string, then we are done
        if (c4Ind == kCIDLib::c4MaxCard)
        {
            break;
        }
         else if (c4Ind == 0)
        {
            // It's the symbolic name and id
            spoolSrc.ParseNameVal(strInBuf, c4Id);
            m_strSymName = strInBuf;
            m_ridThis = c4Id;
        }
         else if (c4Ind == 1)
        {
            // It's the control area, so parse it out
            if (!m_areaItem.bParseFromText(strInBuf, tCIDLib::ERadices::Dec))
            {
                facCIDResEd.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kResEdErrs::errcSrcFile_BadArea
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(spoolSrc.c4CurLine())
                    , spoolSrc.strSrcFile()
                );
            }
        }
         else if (c4Ind == 2)
        {
            // Parse out the hint data
            TDlgItem::ParseHints(strInBuf, m_colHints);
        }
         else if ((c4Ind >= 3) && (c4Ind <= 5))
        {
            //
            //  It's one of text symbols. We keep them contiguous, and in the same
            //  order as the enum, so we can convert from index to enum.
            //
            tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms(c4Ind - 3);
            CIDAssert(eSym < tCIDResEd::ETextSyms::Count, L"Invalid text symbol calculated");
            if (strInBuf[0] == kCIDLib::chOpenBracket)
                m_objaTextSyms[eSym].Set(strInBuf, tCIDResEd::EMsgTypes::Common);
            else
                m_objaTextSyms[eSym].Set(strInBuf, tCIDResEd::EMsgTypes::Message);
        }
         else if (c4Ind == 6)
        {
            // Break out the comma separated values. Each one sets a flag
            m_eFlags = tCIDCtrls::EDlgItemFlags::None;

            tCIDLib::TCard4 c4ErrInd;
            colStrs.RemoveAll();
            if (TStringTokenizer::bParseCSVLine(strInBuf, colStrs, c4ErrInd))
            {
                const tCIDLib::TCard4 c4FlagCnt = colStrs.c4ElemCount();
                for (tCIDLib::TCard4 c4FlagInd = 0; c4FlagInd < c4FlagCnt; c4FlagInd++)
                {
                    if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgItemFlag_InitFocus))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgItemFlags::InitFocus);
                    else if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgItemFlag_Disabled))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgItemFlags::Disabled);
                }
            }
        }
         else if (c4Ind == 7)
        {
            m_strImage = strInBuf;
        }
         else if (c4Ind == 8)
        {
            m_eAnchor = tCIDCtrls::eXlatEEdgeAnchors(strInBuf);
        }
         else
        {
            // Shouldn't get here but check if debugging
            CIDAssert2(L"Failed to catch prefix failure");
        }
    }
}


//
//  This is called to load up (in the form required by the attribute editing window)
//  all of our editable attributes. We just add them to the list, to whatever is
//  already there.
//
tCIDLib::TVoid
TResEdDlgItem::QueryAttrs(          tCIDMData::TAttrList&   colTar
                            ,       TAttrData&              adatTmp
                            , const TCtrlWnd&               wndSrc
                            , const TResEdDlgDesc&          reddParent) const
{
    adatTmp.Set(L"Settings:", TString::strEmpty(), tCIDMData::EAttrTypes::Separator);
    colTar.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_Symbol
        , kCIDResEd::strAttr_Symbol
        , tCIDMData::EAttrTypes::String
    );
    adatTmp.SetString(m_strSymName);
    colTar.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_ResId
        , kCIDResEd::strAttr_ResId
        , L"1,65535"
        , tCIDMData::EAttrTypes::Card
    );
    adatTmp.SetCard(tCIDLib::TCard4(m_ridThis));
    colTar.objAdd(adatTmp);

    // Limit it to be within the dialog's area
    TString strLim(kCIDMData::strAttrLim_AreaPref);
    reddParent.areaDlg().FormatToText(strLim, kCIDLib::chComma, kCIDLib::True);
    adatTmp.Set
    (
        kCIDResEd::strAttr_Area
        , kCIDResEd::strAttr_Area
        , strLim
        , tCIDMData::EAttrTypes::Area
    );
    adatTmp.SetArea(m_areaItem);
    colTar.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_AppImage
        , kCIDResEd::strAttr_AppImage
        , tCIDMData::EAttrTypes::AppImage
    );
    adatTmp.SetString(m_strImage);
    colTar.objAdd(adatTmp);

    //
    //  Do the anchor type. We have to get the current value to a string in order to
    //  pass in the value. And we have to build up the enumerated limit value as well.
    //
    {
        strLim = kCIDMData::strAttrLim_EnumPref;
        tCIDCtrls::EEdgeAnchors eAnchor = tCIDCtrls::EEdgeAnchors::Min;
        while(eAnchor < tCIDCtrls::EEdgeAnchors::Count)
        {
            if (eAnchor > tCIDCtrls::EEdgeAnchors::Min)
                strLim.Append(kCIDLib::chComma);
            strLim.Append(tCIDCtrls::strLoadEEdgeAnchors(eAnchor));
            eAnchor++;
        }
        adatTmp.Set
        (
            kCIDResEd::strAttr_Anchor
            , kCIDResEd::strAttr_Anchor
            , strLim
            , tCIDMData::EAttrTypes::String
        );
        adatTmp.SetString(tCIDCtrls::strLoadEEdgeAnchors(m_eAnchor));
        colTar.objAdd(adatTmp);
    }


    //
    //  These are all text symbols. They have to come from the messages type
    //  loadable text section. We set a sepecial loadable text type on them to
    //  make it easy to handle them all without having to check for the keys.
    //
    adatTmp.Set(L"Control Text:", TString::strEmpty(), tCIDMData::EAttrTypes::Separator);
    colTar.objAdd(adatTmp);

    TString strVal;
    tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms::Min;
    while (eSym < tCIDResEd::ETextSyms::Count)
    {
        adatTmp.Set
        (
            tCIDResEd::strXlatETextSyms(eSym)
            , tCIDResEd::strXlatETextSyms(eSym)
            , tCIDMData::EAttrTypes::String
            , tCIDMData::EAttrEdTypes::Both
        );
        adatTmp.SetString(m_objaTextSyms[eSym].strSymName());
        adatTmp.SetSpecType(kCIDResEd::strSpecType_LoadText);
        adatTmp.bAlwaysReport(kCIDLib::True);
        colTar.objAdd(adatTmp);

        eSym++;
    }

    //
    //  Load up the dialog item flags. As with hints, we add each flag as a boolean
    //  attribute, with the value being whether it's currently set on this item.
    //  And also similarly we give it a special type to make them easy to process
    //  upon a change.
    //
    {
        // Add the separator
        adatTmp.Set
        (
            L"Flags:", TString::strEmpty(), tCIDMData::EAttrTypes::Separator
        );
        colTar.objAdd(adatTmp);

        adatTmp.Set
        (
            kCIDCtrls::strDlgItemFlag_InitFocus
            , kCIDCtrls::strDlgItemFlag_InitFocus
            , tCIDMData::EAttrTypes::Bool
        );
        adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags,  tCIDCtrls::EDlgItemFlags::InitFocus));
        adatTmp.SetSpecType(kCIDResEd::strSpecType_DlgItemFlag);
        colTar.objAdd(adatTmp);

        adatTmp.Set
        (
            kCIDCtrls::strDlgItemFlag_Disabled
            , kCIDCtrls::strDlgItemFlag_Disabled
            , tCIDMData::EAttrTypes::Bool
        );
        adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgItemFlags::Disabled));
        adatTmp.SetSpecType(kCIDResEd::strSpecType_DlgItemFlag);
        colTar.objAdd(adatTmp);
    }


    //
    //  Query the hints that this control supports. We add each one as a boolean
    //  attributes, with the value being whether it's present in this dialog item's
    //  hint list. We given the a special type so that we can easily process them
    //  all when we receive a change.
    //
    tCIDLib::TStrList colAllHints;
    wndSrc.QueryHints(colAllHints);

    const tCIDLib::TCard4 c4HintCnt = colAllHints.c4ElemCount();
    if (c4HintCnt)
    {
        tCIDLib::TBoolean bVal;
        tCIDLib::TCard4 c4At;

        // Add the separator for the hints
        adatTmp.Set(L"Hints:", TString::strEmpty(), tCIDMData::EAttrTypes::Separator);
        colTar.objAdd(adatTmp);

        //
        //  And now add all of the hints that are supported, setting their value
        //  based on whether they are in the current list or not.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4HintCnt; c4Index++)
        {
            bVal = TDlgItem::bFindHint(m_colHints, colAllHints[c4Index], c4At);
            adatTmp.Set
            (
                colAllHints[c4Index], colAllHints[c4Index], tCIDMData::EAttrTypes::Bool
            );
            adatTmp.SetBool(bVal);
            adatTmp.SetSpecType(kCIDResEd::strSpecType_Hint);
            colTar.objAdd(adatTmp);
        }
    }
}


// Get or set the srource id
tCIDCtrls::TWndId TResEdDlgItem::ridThis() const
{
    return m_ridThis;
}

tCIDCtrls::TWndId TResEdDlgItem::ridThis(const tCIDCtrls::TWndId widToSet)
{
    m_ridThis = widToSet;
    return m_ridThis;
}


// Reset ourself back to defaults
tCIDLib::TVoid TResEdDlgItem::Reset()
{
    m_eAnchor = tCIDCtrls::EEdgeAnchors::None;
    m_eFlags = tCIDCtrls::EDlgItemFlags::None;
    m_ridThis = 0;

    m_areaItem.Set(0, 0, 64, 32);
    m_colHints.RemoveAll();
    m_strImage.Clear();
    m_strSymName.Clear();
    m_strType.Clear();

    // Make sure we can't match the object we previous were
    m_c4EditId = facCIDResEd.c4NextEditId();

    // Update the unique window name as well
    m_strWndName = L"ResEdWndName_";
    m_strWndName.AppendFormatted(m_c4EditId);
}


// Reset one of the text symbols
tCIDLib::TVoid TResEdDlgItem::ResetTextSym(const tCIDResEd::ETextSyms eSym)
{
    m_objaTextSyms[eSym].Reset();
}


//
//  Get or set the symbolic name of this item. It can be empty if they never set
//  one on this item!
//
const TString& TResEdDlgItem::strSymName() const
{
    return m_strSymName;
}

const TString& TResEdDlgItem::strSymName(const TString& strToSet)
{
    m_strSymName = strToSet;
    return m_strSymName;
}


// Get the class type
const TString& TResEdDlgItem::strType() const
{
    return m_strType;
}


// Get the unique edit time window name of this item
const TString& TResEdDlgItem::strWndName() const
{
    return m_strWndName;
}


//
//  When we get back one of the attributes, which we gave out in QueryAttribs above,
//  we will figure out which one and store the value.
//
//  Actually we will also get any presentation parameters that our associated control
//  added to the list. Those are easy, we just add/update the value, or remove it
//  if appropriate.
//
//  NOTE that the keys are upper cased versions of the values we passed in as the
//  attribute names when we loaded them in QueryAttrs.
//
tCIDLib::TVoid
TResEdDlgItem::SetAttr(TWindow& wndOwner, const TAttrData& adatNew)
{
    const TString& strId = adatNew.strId();

    if (strId.bCompareI(kCIDResEd::strAttr_Anchor))
    {
        //
        //  Has to be one of the valid anchor types, and this insures it is in the
        //  canonical case as well.
        //
        m_eAnchor = tCIDCtrls::eXlatEEdgeAnchors(adatNew.strValue());
    }
     else if (adatNew.eType() == tCIDMData::EAttrTypes::AppImage)
    {
        m_strImage = adatNew.strValue();
    }
     else if (adatNew.eType() == tCIDMData::EAttrTypes::Area)
    {
        m_areaItem = adatNew.areaVal();
    }
     else if (adatNew.strSpecType() == kCIDResEd::strSpecType_DlgItemFlag)
    {
        // It's a dialog item flag
        tCIDCtrls::EDlgItemFlags eSrcFlag = tCIDCtrls::EDlgItemFlags::None;
        if (adatNew.strId() == kCIDCtrls::strDlgItemFlag_InitFocus)
            eSrcFlag = tCIDCtrls::EDlgItemFlags::InitFocus;
        else if (adatNew.strId() == kCIDCtrls::strDlgItemFlag_Disabled)
            eSrcFlag = tCIDCtrls::EDlgItemFlags::Disabled;

        if (eSrcFlag == tCIDCtrls::EDlgItemFlags::None)
        {
            TString strMsg(adatNew.strId());
            strMsg.Append(L" is not a known dialog item flag");

            TErrBox msgbErr(L"Dialog Item Attrs", strMsg);
            msgbErr.ShowIt(wndOwner);
            return;
        }
         else
        {
            if (adatNew.bVal())
                m_eFlags = tCIDLib::eOREnumBits(m_eFlags, eSrcFlag);
            else
                m_eFlags = tCIDLib::eClearEnumBits(m_eFlags, eSrcFlag);
        }
    }
     else if (adatNew.strSpecType() == kCIDResEd::strSpecType_LoadText)
    {
        // We use a helper in the main window for this
        tCIDResEd::ETextSyms eSym = tCIDResEd::eXlatETextSyms(adatNew.strId());
        if (eSym == tCIDResEd::ETextSyms::Count)
        {
            TErrBox msgbErr
            (
                L"Dialog Item Attrs", L"Unknown dlg item text symbol"
            );
            msgbErr.ShowIt(wndOwner);
            return;
        }

        // Call a helper for this
        if (!facCIDResEd.wndMain().bFindTextSym(adatNew.strValue(), m_objaTextSyms[eSym]))
        {
            TString strMsg(adatNew.strValue());
            strMsg.Append(L" is not a known text symbol");
            TErrBox msgbErr(L"Dialog Item Attrs", strMsg);
            msgbErr.ShowIt(wndOwner);
            return;
        }
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_ResId))
    {
        m_ridThis = adatNew.c4Val();
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_Symbol))
    {
        m_strSymName = adatNew.strValue();
    }
     else if (adatNew.strSpecType() == kCIDResEd::strSpecType_Hint)
    {
        //
        //  It's one of the hints. So, depending on the value, we either remove
        //  this existing hint or add it.
        //
        tCIDLib::TCard4 c4At;
        const tCIDLib::TBoolean bHave = bHaveHint(adatNew.strId(), c4At);
        if (adatNew.bVal())
        {
            if (!bHave)
                m_colHints.objAdd(adatNew.strId());
        }
         else
        {
            if (bHave)
                m_colHints.RemoveAt(c4At);
        }
    }
}


// Update the origin or size of this dialog item, in dialog units
tCIDLib::TVoid
TResEdDlgItem::SetOrg(const tCIDLib::TInt4 i4X, const tCIDLib::TInt4 i4Y)
{
    m_areaItem.SetOrg(i4X, i4Y);
}


tCIDLib::TVoid
TResEdDlgItem::SetSize(const tCIDLib::TCard4 c4CX, const tCIDLib::TCard4 c4CY)
{
    m_areaItem.SetSize(c4CX, c4CY);
}


// Get or set the symbolic text info that provides our flyover text
const TTextSym& TResEdDlgItem::tsymFor(const tCIDResEd::ETextSyms eSym) const
{
    return m_objaTextSyms[eSym];
}


//
//  The dialog system itself uses a different type of dialog item class. We can't
//  use theirs for various reasons. They are designed for the needs of runtime,
//  not for editing. But, in order generically initialize all the various types
//  of controls, we have to have one of these. So we provide a method to fill in
//  one of those from our format.
//
tCIDLib::TVoid TResEdDlgItem::ToDlgItem(TDlgItem& dlgiToFill) const
{
    //
    //  The area needs to be in window units, since the lower level dialog items
    //  are purely just for creating the windows.
    //
    TArea areaReal(m_areaItem);
    TGUIFacility::AdjustDlgUnits(areaReal);

    TDlgItem dlgiNew
    (
        m_ridThis
        , facCIDResEd.wndMain().strLoadResText(m_objaTextSyms[tCIDResEd::ETextSyms::Text])
        , facCIDResEd.wndMain().strLoadResText(m_objaTextSyms[tCIDResEd::ETextSyms::Cue])
        , facCIDResEd.wndMain().strLoadResText(m_objaTextSyms[tCIDResEd::ETextSyms::Flyover])
        , areaReal
        , m_strType
        , m_colHints
        , m_strImage
        , m_eAnchor
    );

    // Now assign our new one to the caller's copy
    dlgiToFill = dlgiNew;
}


// Write out our dialog item record to the target stream
tCIDLib::TVoid TResEdDlgItem::WriteTo(TTextOutStream& strmTar) const
{
    strmTar << L"    " << m_strType << L"=\n"
            << L"        AREA=" << m_areaItem << kCIDLib::NewLn;

    //
    //  It doesn't have to have a symbol. Only ones that code needs to interact with
    //  need one so that they can be found. Decorative controls usually don't.
    //
    if (!m_strSymName.bIsEmpty())
    {
        strmTar << L"        SYMBOL=" << m_strSymName
                << L", " << m_ridThis << kCIDLib::NewLn;
    }

    // Do all of the valid text symbols
    tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms::Min;
    while (eSym < tCIDResEd::ETextSyms::Count)
    {
        if (m_objaTextSyms[eSym].bIsValid())
        {
            const tCIDLib::TCh* pszPref = nullptr;
            switch(eSym)
            {
                case tCIDResEd::ETextSyms::Cue:
                    pszPref = L"CUESYM=";
                    break;

                case tCIDResEd::ETextSyms::Flyover:
                    pszPref = L"FLYOVERSYM=";
                    break;

                case tCIDResEd::ETextSyms::Text :
                    pszPref = L"TEXTSYM=";
                    break;

                default :
                    CIDAssert2(L"Unknown dialog item text symbol");
                    break;
            };

            strmTar << L"        " << pszPref
                    << m_objaTextSyms[eSym].strSymName() << kCIDLib::NewLn;
        }
        eSym++;
    }

    // If any hints, then write them out
    const tCIDLib::TCard4 c4HintCnt = m_colHints.c4ElemCount();
    if (c4HintCnt)
    {
        strmTar << L"        HINTS=";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4HintCnt; c4Index++)
            strmTar << m_colHints[c4Index] << L" ";
        strmTar << kCIDLib::NewLn;
    }

    // If any flags, write them out
    if (m_eFlags != tCIDCtrls::EDlgItemFlags::None)
    {
        strmTar << L"        FLAGS=";
        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgItemFlags::InitFocus))
            strmTar << L"InitFocus ";

        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgItemFlags::Disabled))
            strmTar << L"Disabled ";

        strmTar << kCIDLib::NewLn;
    }

    // If an image, write it
    if (!m_strImage.bIsEmpty())
        strmTar << L"        IMAGE=" << m_strImage << kCIDLib::NewLn;

    if (m_eAnchor != tCIDCtrls::EEdgeAnchors::None)
    {
        strmTar << L"        ANCHOR=" << tCIDCtrls::strLoadEEdgeAnchors(m_eAnchor)
                << kCIDLib::NewLn;
    }

    strmTar << L"    END " << m_strType << kCIDLib::DNewLn;
}


// ---------------------------------------------------------------------------
//  TResEdDlgItem: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  Note that we don't do any streaming format versioning here, since we don't
//  stream for persistence. It's only for clipboard purposes. The stuff that gets
//  streamed is the real dialog item objects (from CIDCtrls) that we convert over
//  to in order to create the binary resource files.
//
tCIDLib::TVoid TResEdDlgItem::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Our stuff should start with a frame marker
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    strmToReadFrom  >> m_areaItem
                    >> m_ridThis
                    >> m_strSymName
                    >> m_strType
                    >> m_colHints
                    >> m_strImage
                    >> m_eAnchor;

    tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms::Min;
    while (eSym < tCIDResEd::ETextSyms::Count)
        strmToReadFrom >> m_objaTextSyms[eSym++];

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    // Give ourself a new edit id since we are not the original object anymore
    m_c4EditId = facCIDResEd.c4NextEditId();

    // And update the unique window name
    m_strWndName = L"ResEdWndName_";
    m_strWndName.AppendFormatted(m_c4EditId);
}

tCIDLib::TVoid TResEdDlgItem::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << m_areaItem
                    << m_ridThis
                    << m_strSymName
                    << m_strType
                    << m_colHints
                    << m_strImage
                    << m_eAnchor;

    tCIDResEd::ETextSyms eSym = tCIDResEd::ETextSyms::Min;
    while (eSym < tCIDResEd::ETextSyms::Count)
        strmToWriteTo << m_objaTextSyms[eSym++];

    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;
}

