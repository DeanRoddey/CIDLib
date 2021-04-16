//
// FILE NAME: CIDRTP_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  This is the header file for the CIDRTP_ThisFacility.cpp file. This
//  file implements the facility class for this facility.
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
//  CLASS: TFacCIDRTP
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDRTPEXP TFacCIDRTP : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDRTP();

        TFacCIDRTP(const TFacCIDRTP&) = delete;
        TFacCIDRTP(TFacCIDRTP&&) = delete;

        ~TFacCIDRTP();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDRTP& operator=(const TFacCIDRTP&) = delete;
        TFacCIDRTP& operator=(TFacCIDRTP&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GenSyncSrcId();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      To synchronize access to our random number generator.
        //
        //  m_randGenId
        //      A random number generated to generate random 32 bit ids for our
        //      RTP sessions.
        // -------------------------------------------------------------------
        TMutex      m_mtxSync;
        TRandomNum  m_randGenId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDRTP,TFacility)
};

#pragma CIDLIB_POPPACK

