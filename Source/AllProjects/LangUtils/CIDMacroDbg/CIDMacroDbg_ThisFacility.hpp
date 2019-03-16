//
// FILE NAME: CIDMacroDbg_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
//  This is the header for the facility class of this facility.
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
//   CLASS: TFacCIDMacroDbg
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TFacCIDMacroDbg : public TGUIFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDMacroDbg();

        TFacCIDMacroDbg(const TFacCIDMacroDbg&) = delete;

        ~TFacCIDMacroDbg();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TFacCIDMacroDbg& operator=(const TFacCIDMacroDbg&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDMacroDbg,TGUIFacility)
};

#pragma CIDLIB_POPPACK


