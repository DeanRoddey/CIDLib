//
// FILE NAME: CIDCtrls_AccelTable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2015
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
//  This class implements an accelerator table. They can be built manually, or
//  from an existing menu (in which case we get any of the hot keyed menu items
//  as accelerators.
//
//  When creating them manully you pass in a collection of strings that are
//  formatted in the same way that menu shortcut keys would be, e.g:
//
//      Ctrl-B
//      Shift-PageDn
//      F5
//
//  And so forth. This way we can use the same code to create from a menu as from
//  a manual list.
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
//  CLASS: TAccelTable
// PREFIX: accel
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TAccelTable : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAccelTable();

        ~TAccelTable();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   TWindow&                wndTar
            , const tCIDLib::TVoid* const   pMsgData
        )   const;

        tCIDLib::TVoid Close();

        tCIDLib::TVoid Create
        (
            const   tCIDLib::TStrList&      colKeys
            , const tCIDLib::TCardList&     fcolCmds
        );

        tCIDLib::TVoid Create
        (
            const   TMenu&                  menuSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TAccelTable(const TAccelTable&);
        tCIDLib::TVoid operator=(const TAccelTable&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_accelThis
        //      The handle to our accelerator table. If not valid, then the
        //      table hasn't been created.
        // -------------------------------------------------------------------
        tCIDCtrls::TAccelHandle m_haccelThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAccelTable, TObject)
};

#pragma CIDLIB_POPPACK


