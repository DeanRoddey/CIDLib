//
// FILE NAME: CIDMath_FValueMap2.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDMath_FValue2.cpp file, which implements
//  a couple of useful derivatives of the TFValueMap class.
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
//  CLASS: TFLogMap
// PREFIX: vmap
// ---------------------------------------------------------------------------
class CIDMATHEXP TFLogMap : public TFValueMap
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TFLogMap
        (
            const   tCIDMath::ELogTypes     eLogType = tCIDMath::ELogTypes::Natural
        );

        TFLogMap(const TFLogMap&) = default;

        ~TFLogMap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFLogMap& operator=(const TFLogMap&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TFloat8 f8MapValue
        (
            const   tCIDLib::TFloat8        f8ToMap
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eLogType
        //      Indicates which type of logarithmic mapping to do.
        // -------------------------------------------------------------------
        tCIDMath::ELogTypes m_eLogType;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFLogMap,TFValueMap)
};

#pragma CIDLIB_POPPACK

