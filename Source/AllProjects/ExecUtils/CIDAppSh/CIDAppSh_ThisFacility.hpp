//
// FILE NAME: CIDAppSh_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
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
//  This is the header for the facility class of the app shell.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDAppSh
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDAPPSHEXP TFacCIDAppSh : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDAppSh();

        ~TFacCIDAppSh();


    private :
        // -------------------------------------------------------------------
        //  Unimplements
        // -------------------------------------------------------------------
        TFacCIDAppSh(const TFacCIDAppSh&);
        tCIDLib::TVoid operator=(const TFacCIDAppSh&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDAppSh,TFacility)
};

