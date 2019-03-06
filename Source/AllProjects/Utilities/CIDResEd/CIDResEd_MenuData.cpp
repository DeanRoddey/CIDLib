//
// FILE NAME: CIDResEd_MenuData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the classes related to the menu resource data.
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
RTTIDecls(TResEdMenuItem,TObject)
RTTIDecls(TResEdMenuDesc,TObject)




// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDResEd_MenuData
{
    // -----------------------------------------------------------------------
    //  Some strings that we look for during parsing
    // -----------------------------------------------------------------------
    const TString   strKW_ITEM(L"ITEM=");
    const TString   strKW_ITEMEnd(L"END ITEM");

    const TString   strKW_MENUEnd(L"END MENU");

    const TString   strKW_SUBMENU(L"SUBMENU=");
    const TString   strKW_SUBMENUEnd(L"END SUBMENU");

    const TString   strKW_SEPARATOR(L"SEPARATOR");
    const TString   strKW_SYMBOL(L"SYMBOL=");
    const TString   strKW_TEXTSYM(L"TEXTSYM=");
}



// ---------------------------------------------------------------------------
//   CLASS: TResEdMenuItem
//  PREFIX: remeni
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdMenuItem: Public, static methods
// ---------------------------------------------------------------------------

// Sort by name comparator
tCIDLib::ESortComps
TResEdMenuItem::eComp(  const   TResEdMenuItem&  remeni1
                        , const TResEdMenuItem&  remeni2)
{
    return remeni1.m_strName.eCompare(remeni2.m_strName);
}


// ---------------------------------------------------------------------------
//  TResEdMenuItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdMenuItem::TResEdMenuItem() :

    m_c4Id(0)
    , m_premendSubMenu(nullptr)
    , m_eType(tCIDCtrls::EMenuItemTypes::ActionItem)
{
}

TResEdMenuItem::TResEdMenuItem(const TResEdMenuItem& remeniToCopy) :

    m_c4Id(remeniToCopy.m_c4Id)
    , m_premendSubMenu(nullptr)
    , m_eType(remeniToCopy.m_eType)
    , m_strName(remeniToCopy.m_strName)
    , m_strTextSym(remeniToCopy.m_strTextSym)
{
    // If there's a submenu, then replicate it
    if (remeniToCopy.m_premendSubMenu)
        m_premendSubMenu = new TResEdMenuDesc(*remeniToCopy.m_premendSubMenu);
}

TResEdMenuItem::TResEdMenuItem( const   tCIDCtrls::EMenuItemTypes eType
                                , const TString&                strName
                                , const tCIDLib::TCard4         c4Id
                                , const TString&                strTextSym) :
    m_c4Id(c4Id)
    , m_premendSubMenu(nullptr)
    , m_eType(eType)
    , m_strName(strName)
    , m_strTextSym(strTextSym)
{
    // If submenu type, then create the submenu for later filling in
    if (eType == tCIDCtrls::EMenuItemTypes::SubMenu)
        m_premendSubMenu = new TResEdMenuDesc;
}

TResEdMenuItem::~TResEdMenuItem()
{
    delete m_premendSubMenu;
    m_premendSubMenu = nullptr;
}


// ---------------------------------------------------------------------------
//  TResEdMenuItem: Public operators
// ---------------------------------------------------------------------------

TResEdMenuItem& TResEdMenuItem::operator=(const TResEdMenuItem& remeniToAssign)
{
    if (this != &remeniToAssign)
    {
        m_c4Id       = remeniToAssign.m_c4Id;
        m_eType      = remeniToAssign.m_eType;
        m_strName    = remeniToAssign.m_strName;
        m_strTextSym = remeniToAssign.m_strTextSym;

        //
        //  If there's a submenu, then replicate it. If there isn't, get rid
        //  of ours if we have one.
        //
        if (remeniToAssign.m_premendSubMenu)
        {
            // The source must have been a submenu type
            CIDAssert
            (
                m_eType != tCIDCtrls::EMenuItemTypes::SubMenu
                , L"Non-submenu item type had a submenu"
            );

            // If we don't have one yet, create it
            if (!m_premendSubMenu)
                m_premendSubMenu = new TResEdMenuDesc(*remeniToAssign.m_premendSubMenu);
            else
                *m_premendSubMenu =  *remeniToAssign.m_premendSubMenu;
        }
         else
        {
            // The source cannot have been a submenu type
            CIDAssert
            (
                m_eType == tCIDCtrls::EMenuItemTypes::SubMenu
                , L"Submenu item type had no submenu"
            );

            delete m_premendSubMenu;
            m_premendSubMenu = 0;
        }
    }
    return *this;
}

tCIDLib::TBoolean
TResEdMenuItem::operator==(const TResEdMenuItem& remeniToComp) const
{
    if ((m_c4Id       != remeniToComp.m_c4Id)
    ||  (m_eType      != remeniToComp.m_eType)
    ||  (m_strName    != remeniToComp.m_strName)
    ||  (m_strTextSym != remeniToComp.m_strTextSym))
    {
        return kCIDLib::False;
    }

    // If we both have no submenu, we are equal
    if (!m_premendSubMenu && !remeniToComp.m_premendSubMenu)
        return kCIDLib::True;

    // If we both have a submenu, then recurse and return that
    if (m_premendSubMenu && remeniToComp.m_premendSubMenu)
        return (*m_premendSubMenu == *remeniToComp.m_premendSubMenu);

    // Else we have to be different
    return kCIDLib::False;
}

tCIDLib::TBoolean
TResEdMenuItem::operator!=(const TResEdMenuItem& remeniToComp) const
{
    return !operator==(remeniToComp);
}



// ---------------------------------------------------------------------------
//  TResEdMenuItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TResEdMenuItem::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TResEdMenuItem::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


// Get or set the type of this menu item
tCIDCtrls::EMenuItemTypes TResEdMenuItem::eType() const
{
    return m_eType;
}

tCIDCtrls::EMenuItemTypes
TResEdMenuItem::eType(const tCIDCtrls::EMenuItemTypes eType)
{
    m_eType = eType;

    //
    //  If not a submenu, remove any submenu we might have. If a submenu,
    //  then create an empty one if we don't have one.
    //
    if (m_premendSubMenu && (m_eType != tCIDCtrls::EMenuItemTypes::SubMenu))
    {
        delete m_premendSubMenu;
        m_premendSubMenu = 0;
    }
     else if (!m_premendSubMenu && (m_eType == tCIDCtrls::EMenuItemTypes::SubMenu))
    {
        m_premendSubMenu = new TResEdMenuDesc;
    }
    return m_eType;
}


// Provide access to the submenu contents if we are a submenu type
const TResEdMenuDesc& TResEdMenuItem::remendSubMenu() const
{
    CIDAssert(m_premendSubMenu != 0, L"There is no sub-menu to return");
    return *m_premendSubMenu;
}

TResEdMenuDesc& TResEdMenuItem::remendSubMenu()
{
    CIDAssert(m_premendSubMenu != 0, L"There is no sub-menu to return");
    return *m_premendSubMenu;
}


// Get or set the symbolic name for this item
const TString& TResEdMenuItem::strName() const
{
    return m_strName;
}

const TString& TResEdMenuItem::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


// Get or set the symbolic name for the text of this item
const TString& TResEdMenuItem::strTextSym() const
{
    return m_strTextSym;
}

const TString& TResEdMenuItem::strTextSym(const TString& strToSet)
{
    m_strTextSym = strToSet;
    return m_strTextSym;
}


// Format out contents out to the target output file
tCIDLib::TVoid
TResEdMenuItem::WriteTo(        TTextOutStream& strmTar
                        , const tCIDLib::TCard4 c4Level) const
{
    const TTextOutStream::Spaces Space(c4Level * 4);
    const TTextOutStream::Spaces Space2((c4Level + 1) * 4);

    if (m_eType == tCIDCtrls::EMenuItemTypes::Decoration)
    {
        strmTar << Space << L"SEPARATOR\n\n";
    }
     else
    {
        strmTar << Space << L"ITEM=\n"
                << Space2 << L"SYMBOL=" << m_strName << L", " << m_c4Id
                << kCIDLib::NewLn;
        if (!m_strTextSym.bIsEmpty())
            strmTar << Space2 << L"TEXTSYM=" << m_strTextSym << kCIDLib::NewLn;

        strmTar << Space << L"END ITEM\n\n";
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TResEdMenuDesc
//  PREFIX: remend
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdMenuItem: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TResEdMenuDesc::eComp(const TResEdMenuDesc& remend1, const TResEdMenuDesc& remend2)
{
    return remend1.m_strName.eCompare(remend2.m_strName);
}


// ---------------------------------------------------------------------------
//  TResEdMenuItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdMenuDesc::TResEdMenuDesc() :

    m_c4Id(kCIDLib::c4MaxCard)
{
}

TResEdMenuDesc::TResEdMenuDesc(const TResEdMenuDesc& remendToCopy) :

    m_c4Id(remendToCopy.m_c4Id)
    , m_colItems(remendToCopy.m_colItems)
    , m_strName(remendToCopy.m_strName)
{
}

TResEdMenuDesc::~TResEdMenuDesc()
{
}


// ---------------------------------------------------------------------------
//  TResEdMenuDesc: Public operators
// ---------------------------------------------------------------------------
TResEdMenuDesc& TResEdMenuDesc::operator=(const TResEdMenuDesc& remendToAssign)
{
    if (this != &remendToAssign)
    {
        m_c4Id     = remendToAssign.m_c4Id;
        m_colItems = remendToAssign.m_colItems;
        m_strName  = remendToAssign.m_strName;
    }
    return *this;
}


tCIDLib::TBoolean
TResEdMenuDesc::operator==(const TResEdMenuDesc& remendToComp) const
{
    return
    (
        (m_c4Id == remendToComp.m_c4Id)
        && (m_strName == remendToComp.m_strName)
        && (m_colItems == remendToComp.m_colItems)
    );
}


tCIDLib::TBoolean
TResEdMenuDesc::operator!=(const TResEdMenuDesc& remeniToComp) const
{
    return !operator==(remeniToComp);
}


// ---------------------------------------------------------------------------
//  TResEdMenuDesc: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the top level menu id
tCIDLib::TCard4 TResEdMenuDesc::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TResEdMenuDesc::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


//
//  Called to parse out a menu block. This can be recursive where sub-menus
//  are involved.
//
tCIDLib::TVoid
TResEdMenuDesc::ParseFrom(TResEdSpooler& spoolSrc, const tCIDLib::TCard4 c4Level)
{
    tCIDLib::TCard4 c4Id;
    TString         strInBuf;
    TString         strTmp;

    //
    //  If this is the outermost menu, level 0, then we have to deal with
    //  the symbol for the menu itself. After that, the symbol info for
    //  submenus are in the menu items that contain them.
    //
    if (!c4Level)
    {
        spoolSrc.bCheckPrefix(CIDResEd_MenuData::strKW_SYMBOL, strInBuf, kCIDLib::True);
        spoolSrc.ParseNameVal(strInBuf, c4Id);
        m_strName = strInBuf;
        m_c4Id = c4Id;
    }

    while(kCIDLib::True)
    {
        spoolSrc.bReadLine(strInBuf);

        //
        //  If we hit the end of the main menu block, we are done.
        //
        if (strInBuf == CIDResEd_MenuData::strKW_MENUEnd)
        {
            //
            //  We should only see this when we are back to the main menu
            //  level.
            //
            if (c4Level)
            {
                facCIDResEd.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kResEdErrs::errcSrcFile_NestedEndMenu
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(spoolSrc.c4CurLine())
                    , spoolSrc.strSrcFile()
                );
            }
            break;
        }
         else if (strInBuf == CIDResEd_MenuData::strKW_SUBMENUEnd)
        {
            // We should only see this when in a nested submenu
            if (!c4Level)
            {
                facCIDResEd.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kResEdErrs::errcSrcFile_BadEndSubMenu
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(spoolSrc.c4CurLine())
                    , spoolSrc.strSrcFile()
                );
            }
            break;
        }

        if (strInBuf == CIDResEd_MenuData::strKW_SEPARATOR)
        {
            // Add a separator item to our item list
            TResEdMenuItem remitNew
            (
                tCIDCtrls::EMenuItemTypes::Decoration
                , TString::strEmpty()
                , kCIDLib::c4MaxCard
                , TString::strEmpty()
            );
            m_colItems.objAdd(remitNew);
        }
         else if (strInBuf == CIDResEd_MenuData::strKW_SUBMENU)
        {
            // We should see the symbol,id and the text id
            spoolSrc.bCheckPrefix(CIDResEd_MenuData::strKW_SYMBOL, strInBuf, kCIDLib::True);
            spoolSrc.ParseNameVal(strInBuf, c4Id);
            spoolSrc.bCheckPrefix(CIDResEd_MenuData::strKW_TEXTSYM, strTmp, kCIDLib::True);

            // Create a temp new menu item with this info
            TResEdMenuItem remitNew(tCIDCtrls::EMenuItemTypes::SubMenu, strInBuf, c4Id, strTmp);

            // And now recurse on its nested menu
            remitNew.remendSubMenu().ParseFrom(spoolSrc, c4Level + 1);

            // Add it to the list
            m_colItems.objAdd(remitNew);
        }
         else if (strInBuf == CIDResEd_MenuData::strKW_ITEM)
        {
            // We should see the symbol,id and the text id
            spoolSrc.bCheckPrefix(CIDResEd_MenuData::strKW_SYMBOL, strInBuf, kCIDLib::True);
            spoolSrc.ParseNameVal(strInBuf, c4Id);
            spoolSrc.bCheckPrefix(CIDResEd_MenuData::strKW_TEXTSYM, strTmp, kCIDLib::True);

            // Create a standard item and add it to our list
            TResEdMenuItem remitNew(tCIDCtrls::EMenuItemTypes::ActionItem, strInBuf, c4Id, strTmp);
            m_colItems.objAdd(remitNew);

            // And we should see an end item
            spoolSrc.CheckNext(CIDResEd_MenuData::strKW_ITEMEnd);
        }
         else
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_Expected
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"SUBMENU=, ITEM=, SEPARATOR, or END MENU")
                , TCardinal(spoolSrc.c4CurLine())
                , spoolSrc.strSrcFile()
            );
        }
    }
}


// Access individual menu sub-items
const TResEdMenuItem&
TResEdMenuDesc::remeniAt(const tCIDLib::TCard4 c4At) const
{
    return m_colItems[c4At];
}

TResEdMenuItem& TResEdMenuDesc::remeniAt(const tCIDLib::TCard4 c4At)
{
    return m_colItems[c4At];
}


// Get or set the symbolic name of the top level menu
const TString& TResEdMenuDesc::strName() const
{
    return m_strName;
}

const TString& TResEdMenuDesc::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


//
//  Called to write out our contents to the output file. We get a level since
//  menus can be recursive. We use it to control indenting.
//
tCIDLib::TVoid
TResEdMenuDesc::WriteTo(        TTextOutStream& strmTar
                        , const tCIDLib::TCard4 c4Level) const
{
    const TTextOutStream::Spaces Space((c4Level + 1) * 4);
    const TTextOutStream::Spaces Space2((c4Level + 2) * 4);

    if (!c4Level)
    {
        //
        //  In this case, the overall menu symbol and id are stored
        //  at our level.
        //
        strmTar << L"MENU=\n"
                << Space << L"SYMBOL=" << m_strName << L", " << m_c4Id
                << kCIDLib::DNewLn;
    }

    // Format out all our items. If we hit a sub-menu, we recurse
    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdMenuItem& remeniCur = m_colItems[c4Index];

        if (remeniCur.eType() == tCIDCtrls::EMenuItemTypes::SubMenu)
        {
            //
            //  We have to recurse here, but we have to put out the
            //  symbol and text sym.
            //
            strmTar << Space << L"SUBMENU=\n"
                    << Space2 << L"SYMBOL="
                    << remeniCur.strName() << L", " << remeniCur.c4Id()
                    << kCIDLib::NewLn;

            if (!remeniCur.strTextSym().bIsEmpty())
            {
                strmTar << Space2 << L"TEXTSYM="
                        << remeniCur.strTextSym() << kCIDLib::NewLn;
            }
            strmTar << kCIDLib::NewLn;

            // Now recurse
            remeniCur.remendSubMenu().WriteTo(strmTar, c4Level + 1);

            // And close off the submenu
            strmTar << Space << L"END SUBMENU\n\n";
        }
         else
        {
            // Just ask it to write itself out at the next level of indent
            remeniCur.WriteTo(strmTar, c4Level + 1);
        }
    }

    // And close off the menu when we exit the main menu
    if (!c4Level)
        strmTar << L"END MENU\n\n";
}


