//
// FILE NAME: CIDResEd_MenuData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  This is the header for the CIDResEd_MenuData.cpp file, which implements
//  a set of classes that handle menu resources. The main resource class
//  holds a list of these for all of the defined menus.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TResEdMenuDesc;

// ---------------------------------------------------------------------------
//   CLASS: TResEdMenuItem
//  PREFIX: remeni
// ---------------------------------------------------------------------------
class TResEdMenuItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TResEdMenuItem&          redit1
            , const TResEdMenuItem&          redit2
        );


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdMenuItem();

        TResEdMenuItem
        (
            const   tCIDCtrls::EMenuItemTypes eType
            , const TString&                strName
            , const tCIDLib::TCard4         c4Id
            , const TString&                strTextSym
        );

        TResEdMenuItem
        (
            const   TResEdMenuItem&         remeniToCopy
        );

        ~TResEdMenuItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdMenuItem& operator=
        (
            const   TResEdMenuItem&         remeniToCopy
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdMenuItem&         remeniToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdMenuItem&         remeniToComp
        )   const;



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDCtrls::EMenuItemTypes eType() const;

        tCIDCtrls::EMenuItemTypes eType
        (
            const   tCIDCtrls::EMenuItemTypes eType
        );

        const TResEdMenuDesc& remendSubMenu() const;

        TResEdMenuDesc& remendSubMenu();

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );

        const TString& strTextSym() const;

        const TString& strTextSym
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid WriteTo
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Level
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The menu id of this menu item
        //
        //  m_premendSubMenu
        //      If this is a submenu type, then this is a pointer to the
        //      nested submenu. It has to be a pointer because it's a
        //      recursive scenario.
        //
        //  m_eType
        //      The type of menu item (text, sub-menu, etc...)
        //
        //  m_strTextSym
        //      The symbolic id of the text of this menu item. If none, then
        //      it's empty.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Id;
        TResEdMenuDesc*         m_premendSubMenu;
        tCIDCtrls::EMenuItemTypes m_eType;
        TString                 m_strTextSym;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdMenuItem,TObject)
};


// Define a list of menu items
namespace tCIDResEd
{
    typedef TVector<TResEdMenuItem> TMenuItemList;
}


// ---------------------------------------------------------------------------
//   CLASS: TResEdMenuDesc
//  PREFIX: remend
// ---------------------------------------------------------------------------
class TResEdMenuDesc : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TResEdMenuDesc&         remend1
            , const TResEdMenuDesc&         remend2
        );


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdMenuDesc();

        TResEdMenuDesc
        (
            const   TResEdMenuDesc&         remendToCopy
        );

        ~TResEdMenuDesc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdMenuDesc& operator=
        (
            const   TResEdMenuDesc&         remendToCopy
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdMenuDesc&         remendToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdMenuDesc&         remendToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid ParseFrom
        (
                    TResEdSpooler&          spoolSrc
            , const tCIDLib::TCard4         c4Level
        );

        const TResEdMenuItem& remeniAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TResEdMenuItem& remeniAt
        (
            const   tCIDLib::TCard4         c4At
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strName
        );

        tCIDLib::TVoid WriteTo
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Level
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The menu id of the main menu. For submenus, this is in the
        //      menu item that contains it.
        //
        //  m_colItems
        //      This is the list of menu items that make up this menu
        //
        //  m_strName
        //      The symbolic name of the main menu. For submenus, this is in
        //      the menu item that contains it.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4Id;
        tCIDResEd::TMenuItemList    m_colItems;
        TString                     m_strName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdMenuDesc,TObject)
};


// Define a list of menus
namespace tCIDResEd
{
    typedef TVector<TResEdMenuDesc> TMenuList;
}

