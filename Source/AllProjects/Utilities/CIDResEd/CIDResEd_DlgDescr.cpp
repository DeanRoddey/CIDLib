//
// FILE NAME: CIDResEd_DlgDescr.cpp
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
//  This file implements the classes related to the dialog resource data.
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
RTTIDecls(TResEdDlgDesc,TObject)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDResEd_ResData
{
    // -----------------------------------------------------------------------
    //  Some strings that we look for during parsing
    // -----------------------------------------------------------------------
    const TString   strKW_DIALOGEnd(L"END DIALOG");

    const TString   strKW_INFO(L"INFO=");
    const TString   strKW_INFOEnd(L"END INFO");
}




// ---------------------------------------------------------------------------
//   CLASS: TResEdDlgDesc
//  PREFIX: redd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdDlgDesc: Public, static methods
// ---------------------------------------------------------------------------

// Sort by name comparator
tCIDLib::ESortComps
TResEdDlgDesc::eComp(const  TResEdDlgDesc&  redd1
                    , const TResEdDlgDesc&  redd2)
{
    return redd1.m_strName.eCompare(redd2.m_strName);
}


// ---------------------------------------------------------------------------
//  TResEdDlgDesc: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdDlgDesc::TResEdDlgDesc() :

    m_areaDlg(0, 0, 120, 64)
    , m_c4EditId(facCIDResEd.c4NextEditId())
    , m_eFlags(tCIDCtrls::EDlgFlags::None)
    , m_eTheme(tCIDCtrls::EWndThemes::DialogBox)
    , m_ridDlg(kCIDLib::c4MaxCard)
{
}

TResEdDlgDesc::TResEdDlgDesc(const  TString&        strName
                            , const tCIDLib::TResId ridDlg) :

    m_areaDlg(0, 0, 120, 64)
    , m_c4EditId(facCIDResEd.c4NextEditId())
    , m_eFlags(tCIDCtrls::EDlgFlags::None)
    , m_eTheme(tCIDCtrls::EWndThemes::DialogBox)
    , m_ridDlg(ridDlg)
    , m_strName(strName)

{
}

TResEdDlgDesc::TResEdDlgDesc(const TResEdDlgDesc& reddSrc) :

    m_areaDlg(reddSrc.m_areaDlg)
    , m_c4EditId(reddSrc.m_c4EditId)
    , m_colList(reddSrc.m_colList)
    , m_eFlags(reddSrc.m_eFlags)
    , m_eTheme(reddSrc.m_eTheme)
    , m_ridDlg(reddSrc.m_ridDlg)
    , m_strName(reddSrc.m_strName)
    , m_tsymText(reddSrc.m_tsymText)
{
}

TResEdDlgDesc::~TResEdDlgDesc()
{
}


// ---------------------------------------------------------------------------
//  TResEdDlgDesc: Public operators
// ---------------------------------------------------------------------------
TResEdDlgDesc& TResEdDlgDesc::operator=(const TResEdDlgDesc& reddSrc)
{
    if (this != &reddSrc)
    {
        m_areaDlg   = reddSrc.m_areaDlg;
        m_c4EditId  = reddSrc.m_c4EditId;
        m_colList   = reddSrc.m_colList;
        m_eFlags    = reddSrc.m_eFlags;
        m_eTheme    = reddSrc.m_eTheme;
        m_ridDlg    = reddSrc.m_ridDlg;
        m_strName   = reddSrc.m_strName;
        m_tsymText  = reddSrc.m_tsymText;
    }
    return *this;
}

tCIDLib::TBoolean
TResEdDlgDesc::operator==(const TResEdDlgDesc& reddSrc) const
{
    if ((m_areaDlg  != reddSrc.m_areaDlg)
    ||  (m_ridDlg   != reddSrc.m_ridDlg)
    ||  (m_strName  != reddSrc.m_strName)
    ||  (m_tsymText != reddSrc.m_tsymText)
    ||  (m_eFlags   != reddSrc.m_eFlags)
    ||  (m_eTheme   != reddSrc.m_eTheme)
    ||  (m_colList  != reddSrc.m_colList))
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean
TResEdDlgDesc::operator!=(const TResEdDlgDesc& reddToComp) const
{
    return !operator==(reddToComp);
}


// ---------------------------------------------------------------------------
//  TResEdDlgDesc: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the area. It should be in dialog units!
const TArea& TResEdDlgDesc::areaDlg() const
{
    return m_areaDlg;
}

const TArea& TResEdDlgDesc::areaDlg(const TArea& areaToSet)
{
    m_areaDlg = areaToSet;
    return m_areaDlg;
}


// Return an area, in dialog units, that contains all of the children
TArea TResEdDlgDesc::areaHull() const
{
    TArea areaRet;
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    if (!c4Count)
        return areaRet;

    // Start with the 0th item's area
    areaRet = m_colList[0].areaItem();

    // Now add the rest
    TArea areaTmp;
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4Count; c4Index++)
        areaRet |= m_colList[c4Index].areaItem();
    return areaRet;
}


// Move the indicated item forward or back
tCIDLib::TVoid
TResEdDlgDesc::AdjustZOrder(const   TResEdDlgItem&      reditTar
                            , const tCIDLib::TBoolean   bForward)
{
    const tCIDLib::TCard4 c4At = c4FindIndex(reditTar);
    if (bForward && (c4At < m_colList.c4ElemCount() - 1))
        m_colList.SwapItems(c4At, c4At + 1);
    else if (!bForward && c4At)
        m_colList.SwapItems(c4At, c4At - 1);
}


//
//  See if any control has the indicated resource id, ir of it's zero which we allow
//  multiple controls to have.
//
tCIDLib::TBoolean
TResEdDlgDesc::bResIdUsed(const tCIDLib::TResId ridToFind) const
{
    if (!ridToFind)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_colList[c4Index];
        if (reditCur.ridThis() == ridToFind)
            return kCIDLib::True;
    }
    return kCIDLib::False;;
}


//
//  Adds a new dialog item to the end of our list, and return its index, which is
//  the count before it's added.
//
tCIDLib::TCard4 TResEdDlgDesc::c4AddNew(const TResEdDlgItem& reditToAdd)
{
    const tCIDLib::TCard4 c4Ret = m_colList.c4ElemCount();
    m_colList.objAdd(reditToAdd);
    return c4Ret;
}


// Return the number of child controls in our list
tCIDLib::TCard4 TResEdDlgDesc::c4ChildCount() const
{
    return m_colList.c4ElemCount();
}


tCIDLib::TCard4 TResEdDlgDesc::c4EditId() const
{
    return m_c4EditId;
}


//
//  We will remove all of the items in the passed list. The list is a non-
//  adopting ref collections, which is used in various places to maintain
//  a list of pointers to our dialog items. So we can delete them without
//  worrying about the pointers in the passed list. They will be tossed
//  upon return.
//
tCIDLib::TVoid
TResEdDlgDesc::DeleteItems(const tCIDResEd::TDlgItemRefList& colList)
{
    //
    //  For each one in the incoming list, we have to do a search to find the
    //  item in our list.
    //
    const tCIDLib::TCard4 c4InCount = colList.c4ElemCount();
    tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();

    for (tCIDLib::TCard4 c4InIndex = 0; c4InIndex < c4InCount; c4InIndex++)
    {
        const TResEdDlgItem* preditCur = colList[c4InIndex];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (&m_colList[c4Index] == preditCur)
            {
                m_colList.RemoveAt(c4Index);

                CIDAssert(c4Count != 0, L"Item count underflow");
                c4Count--;
                break;
            }
        }
    }
}


tCIDCtrls::EDlgFlags TResEdDlgDesc::eFlags() const
{
    return m_eFlags;
}


// Get the theme
tCIDCtrls::EWndThemes TResEdDlgDesc::eTheme() const
{
    return m_eTheme;
}


//
//  Load up any editable attributes that we have, for putting into the attribute
//  editor window.
//
tCIDLib::TVoid
TResEdDlgDesc::QueryAttrs(  tCIDMData::TAttrList&   colAttrs
                            , TAttrData&            adatTmp) const
{
    adatTmp.Set(L"Dialog Settings:", L"", tCIDMData::EAttrTypes::Separator);
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_Symbol
        , kCIDResEd::strAttr_Symbol
        , tCIDMData::EAttrTypes::String
    );
    adatTmp.SetString(m_strName);
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_ResId
        , kCIDResEd::strAttr_ResId
        , L"1,65535"
        , tCIDMData::EAttrTypes::Card
    );
    adatTmp.SetCard(tCIDLib::TCard4(m_ridDlg));
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_Title
        ,  kCIDResEd::strAttr_Title
        , tCIDMData::EAttrTypes::String
        , tCIDMData::EAttrEdTypes::Both
    );
    adatTmp.SetString(m_tsymText.strSymName());
    adatTmp.SetSpecType(kCIDResEd::strSpecType_LoadText);
    adatTmp.bAlwaysReport(kCIDLib::True);
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_Area
        , kCIDResEd::strAttr_Area
        , tCIDMData::EAttrTypes::Area
    );
    adatTmp.SetArea(m_areaDlg);
    colAttrs.objAdd(adatTmp);


    TString strTheme = tCIDCtrls::strLoadEWndThemes(m_eTheme);
    adatTmp.Set
    (
        kCIDResEd::strAttr_Theme
        , kCIDResEd::strAttr_Theme
        , L"Enum:MainWnd, DialogBox"
        , tCIDMData::EAttrTypes::String
        , tCIDMData::EAttrEdTypes::Visual
    );
    adatTmp.SetString(strTheme);
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_ScreenOrigin
        , kCIDResEd::strAttr_ScreenOrigin
        , tCIDMData::EAttrTypes::Bool
    );
    adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::ScreenOrigin));
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_Sizeable
        , kCIDResEd::strAttr_Sizeable
        , tCIDMData::EAttrTypes::Bool
    );
    adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::Sizeable));
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_SetFgn
        , kCIDResEd::strAttr_SetFgn
        , tCIDMData::EAttrTypes::Bool
    );
    adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::SetFgn));
    colAttrs.objAdd(adatTmp);

    adatTmp.Set
    (
        kCIDResEd::strAttr_UseOrigin
        , kCIDResEd::strAttr_UseOrigin
        , tCIDMData::EAttrTypes::Bool
    );
    adatTmp.SetBool(tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::UseOrigin));
    colAttrs.objAdd(adatTmp);
}


//
//  See if we have an item with the indicated resource id, edit id, or name. If not
//  return a null pointer.
//
const TResEdDlgItem*
TResEdDlgDesc::preditByEditId(const tCIDLib::TCard4 c4ToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_colList[c4Index];
        if (reditCur.c4EditId() == c4ToFind)
            return &reditCur;
    }
    return nullptr;
}

const TResEdDlgItem*
TResEdDlgDesc::preditBySymName(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_colList[c4Index];
        if (reditCur.strSymName() == strToFind)
            return &reditCur;
    }
    return nullptr;
}


const TResEdDlgItem*
TResEdDlgDesc::preditByWndName(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_colList[c4Index];
        if (reditCur.strWndName() == strToFind)
            return &reditCur;
    }
    return nullptr;
}

//
//  Parse our contents in from a dialog description in the CIDRC file
//
tCIDLib::TVoid
TResEdDlgDesc::ParseFrom(TResEdSpooler& spoolSrc)
{
    // Make sure we are emptied out
    Reset();

    // The first thing we have to see is the INFO block
    TString strInBuf;
    spoolSrc.CheckNext(CIDResEd_ResData::strKW_INFO);

    tCIDLib::TStrList colPrefs;
    colPrefs.objAdd(L"AREA=");
    colPrefs.objAdd(L"THEME=");
    colPrefs.objAdd(L"SYMBOL=");
    colPrefs.objAdd(L"TEXTSYM=");
    colPrefs.objAdd(L"FLAGS=");

    // Process the lines until we hit the end of the block
    tCIDLib::TCard4 c4Id;
    TString         strTmp;
    while (kCIDLib::True)
    {
        const tCIDLib::TCard4 c4Ind = spoolSrc.c4CheckPrefix
        (
            colPrefs, CIDResEd_ResData::strKW_INFOEnd, strInBuf
        );

        // If the end string, then we are done
        if (c4Ind == kCIDLib::c4MaxCard)
        {
            break;
        }
         else if (c4Ind == 0)
        {
            // It's the dialog area, so parse out the area values
            if (!m_areaDlg.bParseFromText(strInBuf, tCIDLib::ERadices::Dec))
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
         else if (c4Ind == 1)
        {
            if (strInBuf.bCompareI(L"MainWnd"))
                m_eTheme = tCIDCtrls::EWndThemes::MainWnd;
            else if (strInBuf.bCompareI(L"DialogBox"))
                m_eTheme = tCIDCtrls::EWndThemes::DialogBox;
            else
                m_eTheme = tCIDCtrls::EWndThemes::None;
        }
         else if (c4Ind == 2)
        {
            // It's the symbolic name and id
            spoolSrc.ParseNameVal(strInBuf, c4Id);
            m_strName = strInBuf;
            m_ridDlg = c4Id;
        }
         else if (c4Ind == 3)
        {
            // It's the text symbol for the title bar text
            if (strInBuf[0] == kCIDLib::chOpenBracket)
                m_tsymText.Set(strInBuf, tCIDResEd::EMsgTypes::Common);
            else
                m_tsymText.Set(strInBuf, tCIDResEd::EMsgTypes::Message);
        }
         else if (c4Ind == 4)
        {
            // Break out the comma separated values. Each one sets a flag
            m_eFlags = tCIDCtrls::EDlgFlags::None;

            tCIDLib::TStrList   colStrs;
            TStringTokenizer    stokFlags(&strInBuf, kCIDLib::szWhitespace);
            if (stokFlags.c4BreakApart(colStrs))
            {
                const tCIDLib::TCard4 c4FlagCnt = colStrs.c4ElemCount();
                for (tCIDLib::TCard4 c4FlagInd = 0; c4FlagInd < c4FlagCnt; c4FlagInd++)
                {
                    if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgFlag_Sizeable))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::Sizeable);
                    else if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgFlag_UseOrigin))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::UseOrigin);
                    else if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgFlag_ScreenOrigin))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::ScreenOrigin);
                    else if (colStrs[c4FlagInd].bCompareI(kCIDCtrls::strDlgFlag_SetFgn))
                        m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::SetFgn);
                }
            }
        }
         else
        {
            // Shouldn't get here but check if debugging
            CIDAssert2(L"Failed to catch prefix failure");
        }
    }

    //
    //  Now we process control blocks until we hit the end of the dialog
    //  definition.
    //
    while (kCIDLib::True)
    {
        spoolSrc.bReadLine(strInBuf);

        // Break out on the end of dialog
        if (strInBuf == CIDResEd_ResData::strKW_DIALOGEnd)
            break;

        //
        //  It has to be a control type. It must end with an equal sign,
        //  so check that and then remove it. That becomes the symbolic
        //  class type.
        //
        if (strInBuf.chLast() != kCIDLib::chEquals)
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_Expected
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"control type")
                , TCardinal(spoolSrc.c4CurLine())
                , spoolSrc.strSrcFile()
            );
        }
        strInBuf.DeleteLast();

        // Build up what the end text will be
        strTmp = L"END ";
        strTmp.Append(strInBuf);

        //
        //  Now create a new dialog item and ask it to parse itself. It
        //  will eat input up to its end of block line.
        //
        TResEdDlgItem reditNew;
        reditNew.ParseFrom(spoolSrc, strInBuf, strTmp);

        // It worked, so add it to our list of controls
        m_colList.objAdd(reditNew);
    }
}


// Proivde direct access to the dialog items
const TResEdDlgItem& TResEdDlgDesc::reditAt(const tCIDLib::TCard4 c4At) const
{
    return m_colList[c4At];
}

TResEdDlgItem& TResEdDlgDesc::reditAt(const tCIDLib::TCard4 c4At)
{
    return m_colList[c4At];
}


// Get or set the resource id
tCIDLib::TResId TResEdDlgDesc::ridDlg() const
{
    return m_ridDlg;
}


// Resets us back to defaults
tCIDLib::TVoid TResEdDlgDesc::Reset()
{
    m_areaDlg.Set(0, 0, 120, 64);
    m_eFlags = tCIDCtrls::EDlgFlags::None;
    m_colList.RemoveAll();
    m_eTheme = tCIDCtrls::EWndThemes::DialogBox;
    m_strName.Clear();
    m_tsymText.Reset();
    m_ridDlg = kCIDLib::c4MaxCard;

    // Give ourself a new edit time id
    m_c4EditId = facCIDResEd.c4NextEditId();
}


// Get or set the symbolic name of this dialog
const TString& TResEdDlgDesc::strName() const
{
    return m_strName;
}


//
//  When we get back one of the attributes, which we gave out in QueryAttrs above,
//  we will figure out which one and store the value.
//
//  NOTE that the keys are upper cased versions of the values we passed in as the
//  attribute names when we loaded them in QueryAttrs.
//
tCIDLib::TVoid
TResEdDlgDesc::SetAttr(TWindow& wndOwner, const TAttrData& adatNew)
{
    const TString& strId = adatNew.strId();

    if (strId.bCompareI(kCIDResEd::strAttr_Area))
    {
        //
        //  Parse out the new area. Make sure it's not smaller than our control
        //  children's content. If so, size it up to fit.
        //
        TArea areaNew = adatNew.areaVal();

        TArea areaChildren = areaHull();
        areaNew.TakeLarger(areaChildren);

        // Just in case there are no children and the area is empty, force it up
        if (areaNew.bIsEmpty())
            areaNew.SetSize(100, 64);

        m_areaDlg = areaNew;
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_ResId))
    {
        m_ridDlg = adatNew.c4Val();
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_ScreenOrigin))
    {
        if (adatNew.bVal())
            m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::ScreenOrigin);
        else
            m_eFlags = tCIDLib::eClearEnumBits(m_eFlags, tCIDCtrls::EDlgFlags::ScreenOrigin);
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_SetFgn))
    {
        if (adatNew.bVal())
            m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::SetFgn);
        else
            m_eFlags = tCIDLib::eClearEnumBits(m_eFlags, tCIDCtrls::EDlgFlags::SetFgn);
    }

     else if (strId.bCompareI(kCIDResEd::strAttr_Sizeable))
    {
        if (adatNew.bVal())
            m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::Sizeable);
        else
            m_eFlags = tCIDLib::eClearEnumBits(m_eFlags, tCIDCtrls::EDlgFlags::Sizeable);
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_Symbol))
    {
        m_strName = adatNew.strValue();
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_Theme))
    {
        m_eTheme = tCIDCtrls::eXlatEWndThemes(adatNew.strValue());
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_Title))
    {
        // Call a helper for this
        if (!facCIDResEd.wndMain().bFindTextSym(adatNew.strValue(), m_tsymText))
        {
            TString strMsg(adatNew.strValue());
            strMsg.Append(L" is not a known text symbol");

            TErrBox msgbErr(L"Bad text Symbol", strMsg);
            msgbErr.ShowIt(wndOwner);
            return;
        }
    }
     else if (strId.bCompareI(kCIDResEd::strAttr_UseOrigin))
    {
        if (adatNew.bVal())
            m_eFlags = tCIDLib::eOREnumBits(m_eFlags, tCIDCtrls::EDlgFlags::UseOrigin);
        else
            m_eFlags = tCIDLib::eClearEnumBits(m_eFlags, tCIDCtrls::EDlgFlags::UseOrigin);
    }
}


//
//  Set the indicated item to have the initial focus flag, and remove it from
//  any others.
//
tCIDLib::TVoid TResEdDlgDesc::SetInitFocus(const tCIDLib::TCard4 c4EditId)
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TResEdDlgItem& reditCur = m_colList[c4Index];

        tCIDCtrls::EDlgItemFlags eFlag = reditCur.eFlags();
        if (reditCur.c4EditId() == c4EditId)
            eFlag = tCIDLib::eOREnumBits(eFlag, tCIDCtrls::EDlgItemFlags::InitFocus);
        else
            eFlag = tCIDLib::eClearEnumBits(eFlag, tCIDCtrls::EDlgItemFlags::InitFocus);

        reditCur.eFlags(eFlag);
    }
}


// Set the origin or size
tCIDLib::TVoid
TResEdDlgDesc::SetOrg(  const   tCIDLib::TInt4  i4X
                        , const tCIDLib::TInt4  i4Y)
{
    m_areaDlg.SetOrg(i4X, i4Y);
}

tCIDLib::TVoid
TResEdDlgDesc::SetSize( const   tCIDLib::TCard4 c4CX
                        , const tCIDLib::TCard4 c4CY)
{
    m_areaDlg.SetSize(c4CX, c4CY);
}


// Get or set the symbolic text info that provides our title text
const TTextSym& TResEdDlgDesc::tsymText() const
{
    return m_tsymText;
}


// Move the indicated dialog item in the z-order
tCIDLib::TVoid TResEdDlgDesc::ToBottom(const TResEdDlgItem& reditToZ)
{
    // Find the index of this guy
    tCIDLib::TCard4 c4Index = c4FindIndex(reditToZ);

    // Remove it from the list and insert it into the 0th position
    TResEdDlgItem reditTmp = m_colList.objAt(c4Index);
    m_colList.RemoveAt(c4Index);
    m_colList.InsertAt(reditTmp, 0);
}

tCIDLib::TVoid TResEdDlgDesc::ToTop(const TResEdDlgItem& reditToZ)
{
    // Find the index of this guy
    tCIDLib::TCard4 c4Index = c4FindIndex(reditToZ);

    // Remove it from the list and append it to the end
    TResEdDlgItem reditTmp = m_colList.objAt(c4Index);
    m_colList.RemoveAt(c4Index);
    m_colList.objAdd(reditTmp);
}



// Write our contents out to the target file
tCIDLib::TVoid TResEdDlgDesc::WriteTo(TTextOutStream& strmTar) const
{
    // Do the dialog header line and info block
    strmTar << L"DIALOG=\n"
               L"    INFO=\n"
               L"        SYMBOL=" << m_strName
            << L", " << m_ridDlg << kCIDLib::NewLn
            << L"        AREA=" << m_areaDlg << kCIDLib::NewLn;

    if (m_eTheme != tCIDCtrls::EWndThemes::None)
    {
        strmTar << L"        THEME=";

        if (m_eTheme == tCIDCtrls::EWndThemes::MainWnd)
            strmTar << L"MainWnd";
        else if (m_eTheme == tCIDCtrls::EWndThemes::DialogBox)
            strmTar << L"DialogBox";
        else
            strmTar << L"[Unknown]";

        strmTar << kCIDLib::NewLn;
    }

    if (m_tsymText.bIsValid())
        strmTar << L"        TEXTSYM=" << m_tsymText.strSymName() << kCIDLib::NewLn;

    if (m_eFlags != tCIDCtrls::EDlgFlags::None)
    {
        strmTar << L"        FLAGS=";
        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::Sizeable))
            strmTar << kCIDResEd::strAttr_Sizeable << L" ";

        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::UseOrigin))
            strmTar << kCIDResEd::strAttr_UseOrigin << L" ";

        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::ScreenOrigin))
            strmTar << kCIDResEd::strAttr_ScreenOrigin << L" ";

        if (tCIDLib::bAllBitsOn(m_eFlags, tCIDCtrls::EDlgFlags::SetFgn))
            strmTar << kCIDResEd::strAttr_SetFgn << L" ";

        strmTar << kCIDLib::NewLn;
    }

    // Now we can close off the info block
    strmTar << L"    END INFO\n\n";

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_colList[c4Index];
        reditCur.WriteTo(strmTar);
    }

    strmTar << L"END DIALOG\n\n";
}


// ---------------------------------------------------------------------------
//  TResEdDlgDesc: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TResEdDlgDesc::c4FindIndex(const TResEdDlgItem& reditToFind) const
{
    tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem* preditCur = &m_colList[c4Index];
        if (preditCur->ridThis() == reditToFind.ridThis())
            break;
    }

    if (c4Index == c4Count)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcInt_WIdNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(reditToFind.ridThis())
        );
    }
    return c4Index;
}


