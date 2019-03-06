//
// FILE NAME: CIDJPEG_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDJPEG_ThisFacility.Cpp file. This
//  file implements the facility class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TFacCIDJPEG
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDJPEGEXP TFacCIDJPEG : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDJPEG();

        TFacCIDJPEG(const TFacCIDJPEG&) = delete;

        ~TFacCIDJPEG();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDJPEG& operator=(const TFacCIDJPEG&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDJPEG,TFacility)
};

#pragma CIDLIB_POPPACK



