//
// FILE NAME: CIDMacroEng_NamedItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
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
//  This is the header file for the CIDMacroEng_NamedItem.cpp file, which
//  implements the TMEngNamedItem class. A considerable number of items
//  in the engine share some common base semantics, such as being in a name
//  space, having a name within that namespace, having a full name based on
//  the NS name and name, and being id stamped so that they have a unique
//  id within their containing entity.
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
//  CLASS: TExprNamedItem
// PREFIX: meni
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngNamedItem : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TMEngNamedItem();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TMEngNamedItem&         meniToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMEngNamedItem&         meniToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2Id() const;

        tCIDLib::TCard2 c2Id
        (
            const   tCIDLib::TCard2         c2ToSet
        );

        const TString& strName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TMEngNamedItem();

        TMEngNamedItem
        (
            const   TString&                strName
        );

        TMEngNamedItem
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
        );

        TMEngNamedItem
        (
            const   tCIDLib::TCard2         c2Id
        );

        TMEngNamedItem
        (
            const   TMEngNamedItem&         meniToCopy
        );

        tCIDLib::TVoid operator=
        (
            const   TMEngNamedItem&         meniToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ChangeName
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2Id
        //      When we are added to our containing entity, it will stamp us
        //      with an id that is unique to it. This stamp is used for
        //      name resolution, since the ids are what are stored in op codes
        //      not the names.
        //
        //  m_pstrName
        //      The name of this item. It must be unique within it's
        //      containing entity. Some items don't have names, so we use a
        //      pointer and only allocate the name if it's used.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2Id;
        TString*            m_pstrName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngNamedItem,TObject)
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TMEngNamedItem: Public operators
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
TMEngNamedItem::operator!=(const TMEngNamedItem& meniToComp) const
{
    // Just call the equality and negate the result
    return !operator==(meniToComp);
}

// ---------------------------------------------------------------------------
//  TMEngNamedItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard2 TMEngNamedItem::c2Id() const
{
    return m_c2Id;
}


