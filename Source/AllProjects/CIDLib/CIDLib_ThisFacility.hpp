//
// FILE NAME: CIDLib_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDLib_ThisFacility.Cpp file. This
//  file implements the facility class for this facility. An object of this
//  class is created, named facCIDLib, and exported to the outside world to
//  give them access to this facility's global functionality and resources.
//
//  One thing we provide here is a means to create a new unique thread name.
//  Lots of code needs to create unique thread names, and so we provide it
//  here to avoid lots of ad hoc, redundant code. We just use a safe counter
//  to run up a number that we can put into each new one we generate.
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
//  CLASS: TFacCIDLib
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDLIBEXP TFacCIDLib : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDLib();

        TFacCIDLib(const TFacCIDLib&) = delete;

        ~TFacCIDLib();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDLib& operator=(const TFacCIDLib&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestLogFail(const tCIDLib::ELogFlags eFlag) const
        {
            return bCheckLogFlags(tCIDLib::ESeverities::Failed, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TBoolean bTestLogInfo(const tCIDLib::ELogFlags eFlag) const
        {
            return bCheckLogFlags(tCIDLib::ESeverities::Info, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TBoolean bTestLog( const    tCIDLib::ESeverities    eToCheck
                                    , const tCIDLib::ELogFlags      eFlag) const
        {
            return bCheckLogFlags(eToCheck, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TBoolean bTestLog( const   TError&             errToCheck
                                    , const tCIDLib::ELogFlags  eFlag) const
        {
            return bCheckLogFlags(errToCheck, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TBoolean bTestLogStatus(const tCIDLib::ELogFlags eFlag) const
        {
            return bCheckLogFlags(tCIDLib::ESeverities::Status, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TBoolean bTestLogWarn(const tCIDLib::ELogFlags eFlag) const
        {
            return bCheckLogFlags(tCIDLib::ESeverities::Warn, tCIDLib::TCard4(eFlag));
        }

        tCIDLib::TCard4 c4NextId();

        TString strNextThreadName
        (
            const   tCIDLib::TCh* const     pszBaseName
        );

        TString strNextThreadName
        (
            const   TString&                strBaseName
        );

        TString strUnknown() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_scntThread
        //      A safe counter that we'll use to generate new thread names.
        // -------------------------------------------------------------------
        TSafeCard4Counter   m_scntThread;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDLib,TFacility)
};

#pragma CIDLIB_POPPACK


