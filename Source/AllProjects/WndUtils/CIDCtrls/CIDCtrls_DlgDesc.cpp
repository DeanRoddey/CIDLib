//
// FILE NAME: CIDCtrls_DlgDesc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/26/2000
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
//  This file implements the TDlgDesc class.
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
#include    "CIDCtrls_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDlgItem,TObject)
RTTIDecls(TDlgDesc,TObject)


namespace CIDCtrls_DlgDesc
{
    // -----------------------------------------------------------------------
    //  Local const data
    //
    //  c2FmtVersion
    //      This is the current version of the format. It is currently just 1,
    //      but will be used in the future to handle format expansion.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//  CLASS: TDlgItem
// PREFIX: dlgi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDlgItem: Public, static methods
// ---------------------------------------------------------------------------

//
//  A helper to search the list of hint strings to see if a given one is there. We
//  don't use a hash set since we can't depend on the case being correct. These
//  values come from the user and CIDRC files. So we just do a search. There are never
//  many of them.
//
tCIDLib::TBoolean
TDlgItem::bFindHint(const   tCIDCtrls::THintsList&  colHints
                    , const TString&                strToFind
                    ,       tCIDLib::TCard4&        c4At)
{
    const tCIDLib::TCard4 c4Count = colHints.c4ElemCount();
    c4At = 0;
    while (c4At < c4Count)
    {
        if (colHints[c4At].bCompareI(strToFind))
            return kCIDLib::True;
        c4At++;
    }
    return kCIDLib::False;
}


//
//  A helper to parse out a an image of the facility based resource type. So it will be
//  in the form:
//
//  facname, id, type
//
//  Facility is the name of the facility to load it from, which must be one that is already
//  loaded into the containing process. Id is the resoure id, and type is JPEG, PNG, or BITMAP.
//
tCIDLib::TBoolean
TDlgItem::bParseResImage(const  TString&                strText
                        ,       tCIDLib::TResId&        ridImg
                        ,       TString&                strFacName
                        ,       tCIDImage::EImgTypes&   eType)
{
    TStringTokenizer stokHints(&strText, L" \t");

    TString strId;
    TString strType;
    if (!stokHints.bGetNextToken(strFacName)
    ||  !stokHints.bGetNextToken(strId)
    ||  !stokHints.bGetNextToken(strType))
    {
        return kCIDLib::False;
    }

    // Convert the id
    if (!strId.bToCard4(ridImg, tCIDLib::ERadices::Auto))
        return kCIDLib::False;

    // Convert the type
    if (strType.bCompareI(L"BITMAP"))
        eType = tCIDImage::EImgTypes::Bitmap;
    else if (strType.bCompareI(L"JPEG"))
        eType = tCIDImage::EImgTypes::JPEG;
    else if (strType.bCompareI(L"PNG"))
        eType = tCIDImage::EImgTypes::PNG;
    else
        return kCIDLib::False;

    return kCIDLib::True;
}


// A helper to parse out the hints string into a list of tokens
tCIDLib::TVoid
TDlgItem::ParseHints(const TString& strText, tCIDLib::TStrList& colToFill)
{
    colToFill.RemoveAll();

    TStringTokenizer stokHints(&strText, L" \t");
    TString strToken;
    while (stokHints.bGetNextToken(strToken))
        colToFill.objAdd(strToken);

    // Sort the list if we got any
    if (!colToFill.bIsEmpty())
    {
        colToFill.Sort
        (
            [](const TString& str1, const TString& str2)
            {
                return str1.eCompareI(str2);
            }
        );
    }
}


//
//  A helper to parse out the app image attribte. It can be either a default size
//  or prefixed by a size indicator.
//
tCIDCtrls::EAppImgSz
TDlgItem::eParseAppImage(const TString& strText, TString& strImgName)
{
    tCIDCtrls::EAppImgSz eSz = tCIDCtrls::EAppImgSz::Default;

    if (strText.bStartsWithI(L"Sml:"))
    {
        strImgName.CopyInSubStr(strText, 4);
        eSz = tCIDCtrls::EAppImgSz::Small;
    }
     else if (strText.bStartsWithI(L"Med:"))
    {
        eSz = tCIDCtrls::EAppImgSz::Medium;
        strImgName.CopyInSubStr(strText, 4);
    }
     else if (strText.bStartsWithI(L"Lrg:"))
    {
        eSz = tCIDCtrls::EAppImgSz::Large;
        strImgName.CopyInSubStr(strText, 4);
    }
     else
    {
        strImgName = strText;
    }
    return eSz;
}



// ---------------------------------------------------------------------------
//  TDlgItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TDlgItem::TDlgItem() :

    m_areaPos(10, 10, 10, 10)
    , m_eAnchor(tCIDCtrls::EEdgeAnchors::None)
    , m_eFlags(tCIDCtrls::EDlgItemFlags::None)
    , m_ridThis(kCIDLib::c4MaxCard)
{
}

TDlgItem::TDlgItem( const   tCIDLib::TResId         ridChild
                    , const TString&                strText
                    , const TString&                strCueText
                    , const TString&                strFlyText
                    , const TArea&                  areaPos
                    , const TString&                strType
                    , const tCIDCtrls::THintsList   colHints
                    , const TString&                strImage
                    , const tCIDCtrls::EEdgeAnchors eAnchor) :

    m_areaPos(areaPos)
    , m_colHints(colHints)
    , m_eAnchor(eAnchor)
    , m_eFlags(tCIDCtrls::EDlgItemFlags::None)
    , m_ridThis(ridChild)
    , m_strCueText(strCueText)
    , m_strFlyoverText(strFlyText)
    , m_strImage(strImage)
    , m_strText(strText)
    , m_strType(strType)
{
}

TDlgItem::TDlgItem(const TDlgItem& dlgiSrc) :

    m_areaPos(dlgiSrc.m_areaPos)
    , m_colHints(dlgiSrc.m_colHints)
    , m_eAnchor(dlgiSrc.m_eAnchor)
    , m_eFlags(dlgiSrc.m_eFlags)
    , m_ridThis(dlgiSrc.m_ridThis)
    , m_strCppType(dlgiSrc.m_strCppType)
    , m_strCueText(dlgiSrc.m_strCueText)
    , m_strFlyoverText(dlgiSrc.m_strFlyoverText)
    , m_strImage(dlgiSrc.m_strImage)
    , m_strText(dlgiSrc.m_strText)
    , m_strType(dlgiSrc.m_strType)
{
}

TDlgItem::TDlgItem( const   tCIDCtrls::TRawDlgItem& rdlgiSrc
                    , const TFacility&              facTextSrc)
{
    // We already have a method to do this so call that to load up
    FromRaw(rdlgiSrc, facTextSrc);
}

TDlgItem::~TDlgItem()
{
}


// ---------------------------------------------------------------------------
//  TDlgItem: Public operators
// ---------------------------------------------------------------------------
TDlgItem& TDlgItem::operator=(const TDlgItem& dlgiSrc)
{
    if (this != &dlgiSrc)
    {
        m_areaPos       = dlgiSrc.m_areaPos;
        m_colHints      = dlgiSrc.m_colHints;
        m_eAnchor       = dlgiSrc.m_eAnchor;
        m_eFlags        = dlgiSrc.m_eFlags;
        m_ridThis       = dlgiSrc.m_ridThis;
        m_strCppType    = dlgiSrc.m_strCppType;
        m_strCueText    = dlgiSrc.m_strCueText;
        m_strFlyoverText= dlgiSrc.m_strFlyoverText;
        m_strImage      = dlgiSrc.m_strImage;
        m_strText       = dlgiSrc.m_strText;
        m_strType       = dlgiSrc.m_strType;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TDlgItem: Public, non-virtual
// ---------------------------------------------------------------------------

// Get/set the position of this control
const TArea& TDlgItem::areaPos() const
{
    return m_areaPos;
}

const TArea& TDlgItem::areaPos(const TArea& areaToSet)
{
    m_areaPos = areaToSet;
    return m_areaPos;
}


// Add a hint to to our hint list if not already there
tCIDLib::TVoid TDlgItem::AddHint(const TString& strToAdd)
{
    tCIDLib::TCard4 c4At;
    if (!bFindHint(m_colHints, strToAdd, c4At))
        m_colHints.objAdd(strToAdd);
}



//
//  See if we have a particular hint. We work in terms of the generic static version
//  of this functionality, but this is more convenient for the use of the controls
//  during init.
//
tCIDLib::TBoolean TDlgItem::bHasHint(const TString& strToCheck) const
{
    tCIDLib::TCard4 c4At;
    return bFindHint(m_colHints, strToCheck, c4At);
}


// Get/set our edge anchor setting
tCIDCtrls::EEdgeAnchors TDlgItem::eAnchor() const
{
    return m_eAnchor;
}

tCIDCtrls::EEdgeAnchors TDlgItem::eAnchor(const tCIDCtrls::EEdgeAnchors eToSet)
{
    m_eAnchor = eToSet;
    return m_eAnchor;
}


// Return our flags
tCIDCtrls::EDlgItemFlags TDlgItem::eFlags() const
{
    return m_eFlags;
}


//
//  Take a raw dialog item definition, from the binary CIDRES file and parse it
//  into our, more digestable, format.
//
tCIDLib::TVoid TDlgItem::FromRaw(const  tCIDCtrls::TRawDlgItem& rdlgiSrc
                                , const TFacility&              facTextSrc)
{
    //
    //  Load the message text if any was defined. It might be one of the
    //  special ones.
    //
    if (rdlgiSrc.midText != kCIDLib::c4MaxCard)
    {
        if (rdlgiSrc.midText & 0x80000000)
            facCIDCtrls().bLoadSpecialText(rdlgiSrc.midText & 0x7FFFFFFF, m_strText);
         else
            m_strText.LoadFromMsg(rdlgiSrc.midText, facTextSrc);
    }

    // The cue text
    if (rdlgiSrc.midCue != kCIDLib::c4MaxCard)
        m_strCueText.LoadFromMsg(rdlgiSrc.midCue, facTextSrc);

    // And the fly over text
    if (rdlgiSrc.midFlyOver != kCIDLib::c4MaxCard)
        m_strFlyoverText.LoadFromMsg(rdlgiSrc.midFlyOver, facTextSrc);
    else
        m_strFlyoverText.Clear();

    m_eAnchor   = rdlgiSrc.eAnchor;
    m_eFlags    = rdlgiSrc.eFlags;
    m_ridThis   = rdlgiSrc.ridThis;

    // Do any trailing text strings. First is the hints string.
    TString strHints = (const tCIDLib::TCh*)
    (
        ((tCIDLib::TCard1*)&rdlgiSrc) + sizeof(rdlgiSrc)
    );
    ParseHints(strHints, m_colHints);

    // The dialog item type string
    m_strType = reinterpret_cast<const tCIDLib::TCh*>
    (
        ((tCIDLib::TCard1*)&rdlgiSrc)
        + (sizeof(rdlgiSrc) + ((rdlgiSrc.c4HintsLen + 1) * kCIDLib::c4CharBytes))
    );

    // The app image name string
    m_strImage = reinterpret_cast<const tCIDLib::TCh*>
    (
        ((tCIDLib::TCard1*)&rdlgiSrc)
        + sizeof(rdlgiSrc)
        + ((rdlgiSrc.c4HintsLen + 1) * kCIDLib::c4CharBytes)
        + ((rdlgiSrc.c4ItemTypeLen + 1) * kCIDLib::c4CharBytes)
    );

    //
    //  Get the area and convert to window units. These objects are just to be
    //  used to create the dialog items, which need window units, so best to do it
    //  here and not burden them with it
    //
    m_areaPos.Set(rdlgiSrc.i4X, rdlgiSrc.i4Y, rdlgiSrc.c4CX, rdlgiSrc.c4CY);
    TGUIFacility::AdjustDlgUnits(m_areaPos);

    // Clear any previously set CPP type
    m_strCppType.Clear();
}


tCIDLib::TResId TDlgItem::ridChild() const
{
    return m_ridThis;
}

// Get or set the C++ type
const TString& TDlgItem::strCppType() const
{
    return m_strCppType;
}

const TString& TDlgItem::strCppType(const TString& strToSet)
{
    m_strCppType = strToSet;
    return m_strCppType;
}


// Get the cue text
const TString& TDlgItem::strCueText() const
{
    return m_strCueText;
}


// Get the flyover text
const TString& TDlgItem::strFlyoverText() const
{
    return m_strFlyoverText;
}


const tCIDLib::TStrList& TDlgItem::colHints() const
{
    return m_colHints;
}


const TString& TDlgItem::strImage() const
{
    return m_strImage;
}


const TString& TDlgItem::strText() const
{
    return m_strText;
}


const TString& TDlgItem::strType() const
{
    return m_strType;
}






// ---------------------------------------------------------------------------
//  CLASS: TDlgDesc
// PREFIX: dlgd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDlgDesc: Constructors and Destructor
// ---------------------------------------------------------------------------
TDlgDesc::TDlgDesc() :

    m_eFlags(tCIDCtrls::EDlgFlags::None)
    , m_eTheme(tCIDCtrls::EWndThemes::None)
    , m_pcolItems(0)
    , m_ridThis(0)
{
    m_pcolItems = new TVector<TDlgItem>;
}

TDlgDesc::TDlgDesc( const   TString&                strTitle
                    , const tCIDCtrls::EWndThemes   eTheme
                    , const TArea&                  areaPos) :

    m_areaPos(areaPos)
    , m_eFlags(tCIDCtrls::EDlgFlags::None)
    , m_eTheme(eTheme)
    , m_pcolItems(0)
    , m_ridThis(0)
    , m_strTitle(strTitle)
{
    m_pcolItems = new TVector<TDlgItem>;
}

TDlgDesc::TDlgDesc(const TDlgDesc& dlgdSrc) :

    m_areaPos(dlgdSrc.m_areaPos)
    , m_eFlags(dlgdSrc.m_eFlags)
    , m_eTheme(dlgdSrc.m_eTheme)
    , m_pcolItems(0)
    , m_ridThis(dlgdSrc.m_ridThis)
    , m_strTitle(dlgdSrc.m_strTitle)
{
    // Dup the list of items
    m_pcolItems = new TVector<TDlgItem>(*dlgdSrc.m_pcolItems);
}

TDlgDesc::TDlgDesc( const   tCIDCtrls::TRawDlgDesc& rdlgdSrc
                    , const TFacility&              facTextSrc) :

    m_eFlags(rdlgdSrc.eFlags)
    , m_eTheme(rdlgdSrc.eTheme)
    , m_pcolItems(0)
    , m_ridThis(rdlgdSrc.ridDlg)
{
    // Convert the area to window units
    m_areaPos.Set(rdlgdSrc.i4X, rdlgdSrc.i4Y, rdlgdSrc.c4CX, rdlgdSrc.c4CY);
    TGUIFacility::AdjustDlgUnits(m_areaPos);

    //
    //  Load the message text if any was defined. It might be one of the
    //  special ones.
    //
    if (rdlgdSrc.midTitle != kCIDLib::c4MaxCard)
    {
        if (rdlgdSrc.midTitle & 0x80000000)
            facCIDCtrls().bLoadSpecialText(rdlgdSrc.midTitle & 0x7FFFFFFF, m_strTitle);
         else
            m_strTitle.LoadFromMsg(rdlgdSrc.midTitle, facTextSrc);
    }

    // Create our child collection and then the children
    m_pcolItems = new TVector<TDlgItem>;
    ChildrenFromRaw(rdlgdSrc, facTextSrc);
}

TDlgDesc::~TDlgDesc()
{
    delete m_pcolItems;
}


// ---------------------------------------------------------------------------
//  TDlgDesc: Public operators
// ---------------------------------------------------------------------------
TDlgDesc& TDlgDesc::operator=(const TDlgDesc& dlgdSrc)
{
    if (this != &dlgdSrc)
    {
        m_areaPos       = dlgdSrc.m_areaPos;
        m_eFlags        = dlgdSrc.m_eFlags;
        m_eTheme        = dlgdSrc.m_eTheme;
        *m_pcolItems    = *dlgdSrc.m_pcolItems;
        m_ridThis       = dlgdSrc.m_ridThis;
        m_strTitle      = dlgdSrc.m_strTitle;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TDlgDesc: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDlgDesc::AddItem(const TDlgItem& dlgiToAdd)
{
    m_pcolItems->objAdd(dlgiToAdd);
}


// Get/set the position
const TArea& TDlgDesc::areaPos() const
{
    return m_areaPos;
}

const TArea& TDlgDesc::areaPos(const TArea& areaToSet)
{
    m_areaPos = areaToSet;
    return m_areaPos;
}


// Get a const cursor to iterate over the items
TDlgDesc::TCursor TDlgDesc::cursItems() const
{
    return TCursor(m_pcolItems);
}


// Find a dialog item by its window (resource) id
TDlgItem& TDlgDesc::dlgiFindById(const tCIDLib::TResId ridToFind)
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToFind)
            return dlgiCur;
    }

    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcDlgD_ItemIdNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , TCardinal(ridToFind)
        , TCardinal(m_ridThis)
    );

    // Make the compiler happy
    return m_pcolItems->objAt(0);
}

const TDlgItem& TDlgDesc::dlgiFindById(const tCIDLib::TResId ridToFind) const
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToFind)
            return dlgiCur;
    }

    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcDlgD_ItemIdNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , TCardinal(ridToFind)
        , TCardinal(m_ridThis)
    );

    // Make the compiler happy
    return m_pcolItems->objAt(0);
}


// Provide access to our flags
tCIDCtrls::EDlgFlags TDlgDesc::eFlags() const
{
    return m_eFlags;
}


// Get/set our theme
tCIDCtrls::EWndThemes TDlgDesc::eTheme() const
{
    return m_eTheme;
}

tCIDCtrls::EWndThemes TDlgDesc::eTheme(const tCIDCtrls::EWndThemes eToSet)
{
    m_eTheme = eToSet;
    return m_eTheme;
}


// Read in a raw dialog decription (from the CIDRES file) and load up its contents
tCIDLib::TVoid TDlgDesc::FromRaw(const  tCIDCtrls::TRawDlgDesc& rdlgdSrc
                                , const TFacility&              facTextSrc)
{
    // Flush any existing children
    m_pcolItems->RemoveAll();

    //
    //  Load the message text if any was defined. It might be one of the
    //  special ones.
    //
    if (rdlgdSrc.midTitle != kCIDLib::c4MaxCard)
    {
        if (rdlgdSrc.midTitle & 0x80000000)
            facCIDCtrls().bLoadSpecialText(rdlgdSrc.midTitle & 0x7FFFFFFF, m_strTitle);
         else
            m_strTitle.LoadFromMsg(rdlgdSrc.midTitle, facTextSrc);
    }

    // Copy over our dialog specific stuff
    m_eFlags  = rdlgdSrc.eFlags;
    m_eTheme  = rdlgdSrc.eTheme;
    m_ridThis = rdlgdSrc.ridDlg;

    // Get the area info and convert to screen units
    m_areaPos.Set(rdlgdSrc.i4X, rdlgdSrc.i4Y, rdlgdSrc.c4CX, rdlgdSrc.c4CY);
    TGUIFacility::AdjustDlgUnits(m_areaPos);

    // And process the children, refilling our collection of child items
    ChildrenFromRaw(rdlgdSrc, facTextSrc);
}


const TDlgItem* TDlgDesc::pdlgiFindById(const tCIDLib::TResId ridToFind) const
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToFind)
            return &dlgiCur;
    }
    return nullptr;
}

TDlgItem* TDlgDesc::pdlgiFindById(const tCIDLib::TResId ridToFind)
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToFind)
            return &dlgiCur;
    }
    return nullptr;
}



tCIDLib::TResId TDlgDesc::ridThis() const
{
    return m_ridThis;
}


tCIDLib::TVoid TDlgDesc::RemoveItem(const tCIDLib::TResId ridToRemove)
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToRemove)
        {
            m_pcolItems->RemoveAt(c4Index);
            break;
        }
    }
}


const TString& TDlgDesc::strTitle() const
{
    return m_strTitle;
}


// To allow for dynamic placement of the dialog at runtime
tCIDLib::TVoid TDlgDesc::SetOrigin(const TPoint& pntToSet)
{
    m_areaPos.SetOrg(pntToSet);
}


tCIDLib::TVoid
TDlgDesc::SetCppType(const tCIDLib::TResId ridToSet, const TString& strType)
{
    const tCIDLib::TCard4 c4Count = m_pcolItems->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TDlgItem& dlgiCur = m_pcolItems->objAt(c4Index);
        if (dlgiCur.ridChild() == ridToSet)
        {
            dlgiCur.strCppType(strType);
            break;
        }
    }
}



// ---------------------------------------------------------------------------
//  TDlgItem: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDlgDesc::ChildrenFromRaw(  const   tCIDCtrls::TRawDlgDesc& rdlgdSrc
                            , const TFacility&              facTextSrc)
{
    //
    //  And lets create our children. Start off by getting a pointer to the
    //  first dialog item, which is immediately after the dialog description
    //  structure.
    //
    const tCIDCtrls::TRawDlgItem* prdlgiCur = reinterpret_cast<const tCIDCtrls::TRawDlgItem*>
    (
        ((tCIDLib::TCard1*)&rdlgdSrc) + sizeof(rdlgdSrc)
    );

    for (tCIDLib::TCard4 c4Index = 0; c4Index < rdlgdSrc.c4ChildCount; c4Index++)
    {
        // Add an item object, which constructs from the current item
        m_pcolItems->objAdd(TDlgItem(*prdlgiCur, facTextSrc));

        //
        //  Move up to the next item, being sure to account for any trailing
        //  text values.
        //
        prdlgiCur = tCIDLib::pOffsetPtr<const tCIDCtrls::TRawDlgItem>
        (
            prdlgiCur
            , sizeof(tCIDCtrls::TRawDlgItem)
              + ((prdlgiCur->c4HintsLen + 1) * kCIDLib::c4CharBytes)
              + ((prdlgiCur->c4ItemTypeLen + 1) * kCIDLib::c4CharBytes)
              + ((prdlgiCur->c4ImageLen + 1) * kCIDLib::c4CharBytes)
        );
    }
}


