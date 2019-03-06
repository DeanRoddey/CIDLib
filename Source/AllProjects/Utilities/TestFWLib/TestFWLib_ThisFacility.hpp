//
// FILE NAME: TestFWLib_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the TestFWLib_ThisFacility.cpp file. This
//  file implements the facility class for the test framework library.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TFacTestFWLib
// PREFIX: fac
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TFacTestFWLib : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacTestFWLib();

        TFacTestFWLib(const TFacTestFWLib&) = delete;

        ~TFacTestFWLib();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacTestFWLib& operator=(const TFacTestFWLib&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strPhaseText
        (
            const   tTestFWLib::EPhases     eToXlat
        );

        const TString& strResText
        (
            const   tTestFWLib::ETestRes    eToXlat
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strUnknown
        //      This is set to a value that we return if we get an unknown
        //      enumeration value to provide the text for.
        // -------------------------------------------------------------------
        TString  m_strUnknown;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacTestFWLib,TFacility)
};

#pragma CIDLIB_POPPACK

