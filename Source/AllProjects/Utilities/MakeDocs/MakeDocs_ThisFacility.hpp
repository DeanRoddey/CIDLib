//
// FILE NAME: MakeDocs_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This is the header for the MakeDocs_ThisFacility.cpp module. This module
//  implements the facility class for this program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TFacMakeDocs
// PREFIX: fac
// ---------------------------------------------------------------------------
class TFacMakeDocs : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TFacMakeDocs();

        ~TFacMakeDocs();


        // -------------------------------------------------------------------
        //  Public, non-static methods
        // -------------------------------------------------------------------

    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacMakeDocs, TFacility)
};

