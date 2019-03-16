//
// FILE NAME: CIDResEd_CBData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2010
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
//  This is the header for the CIDResEd_CBData.cpp file, which implements
//  some simple classes that let us put various types of info on the
//  clipboard for copy/paste purposes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResEdCBItems
//  PREFIX: cbi
//
//  Copy and paste one or more dialog items.
// ---------------------------------------------------------------------------
class TResEdCBItems : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdCBItems();

        TResEdCBItems
        (
            const   tCIDResEd::TDlgItemRefList& colItems
        );

        ~TResEdCBItems();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const tCIDResEd::TDlgItemList& colItems() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colItems
        //      We get a by ref list of the selected items. We have to make
        //      actual copies of them because we have to keep the original
        //      values.
        // -------------------------------------------------------------------
        tCIDResEd::TDlgItemList m_colItems;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdCBItems,TObject)
};


