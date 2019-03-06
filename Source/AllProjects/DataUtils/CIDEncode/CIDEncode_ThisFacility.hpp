//
// FILE NAME: CIDEncode_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/26/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDEncode_ThisFacility.cpp file. This file
//  provides the facility class for this facility.
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
//  CLASS: TFacCIDEncode
// PREFIX: fac
//
//  This is the facility class for this facility.
// ---------------------------------------------------------------------------
class CIDENCODEEXP TFacCIDEncode : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDEncode();

        TFacCIDEncode(const TFacCIDEncode&) = delete;

        ~TFacCIDEncode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDEncode& operator=(const TFacCIDEncode&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMapping
        (
            const   TString&                strName
            , const TString&                strClass
        );

        tCIDLib::TBoolean bProbeForEncoding
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4Size
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bSupportsEncoding
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TVoid GetAllEncodingNames
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const;

        TTextConverter* ptcvtMakeNew
        (
            const   TString&                strName
        )   const;

        tCIDEncode::TTCvtPtr mptrMakeNew
        (
            const   TString&                strName
        )   const;

        TString strBaseNameFor
        (
            const   TString&                strName
        )   const;



    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TMapList = tCIDLib::TKVHashSet;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolMap
        //      This is the name to class name map, which maps publically
        //      understandable encoding names to the name of the class that
        //      provides it. The classes are all derivatives of the
        //      TTextConverter class.
        // -------------------------------------------------------------------
        TMapList*   m_pcolMap;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDEncode,TFacility)
};

#pragma CIDLIB_POPPACK

