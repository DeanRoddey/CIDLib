//
// FILE NAME: CIDSIP_Transact.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 4/28/2013
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
//  This is the header for the CIDSIP_Transact.cpp file. This implements the
//  class that is used to track transactions between agents. A dialog will
//  manage a set of these, one for each transaction it has outstanding, which
//  can be with multiple other agents.
//
//  Info about timers and retransmission is managed here as well.
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
//   CLASS: TSIPTransact
//  PREFIX: siptact
// ---------------------------------------------------------------------------
class CIDSIPEXP TSIPTransact : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSIPTransact();

        TSIPTransact
        (
            const   TSIPTransact&           siptactSrc
        );

        ~TSIPTransact();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSIPTransact& operator=
        (
            const   TSIPTransact&           siptactSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strBranchID
        //      The branch id to use. This is generated based on other info.
        // -------------------------------------------------------------------
        TString     m_strBranchID;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSIPTransact, TObject)
};

#pragma CIDLIB_POPPACK


