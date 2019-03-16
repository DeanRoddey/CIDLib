//
// FILE NAME: CIDMData_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
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
//  This is the header for the CIDMData_ThisFacility.cpp file. This file
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
//  CLASS: TFacCIDMData
// PREFIX: fac
//
//  This is the facility class for this facility.
// ---------------------------------------------------------------------------
class CIDMDATAEXP TFacCIDMData : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDMData();

        TFacCIDMData(const TFacCIDMData&) = delete;

        ~TFacCIDMData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDMData& operator=(const TFacCIDMData&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid RemoveAttr
        (
                    tCIDMData::TAttrList&   colList
            , const TString&                strId
        );

        const TString& strBoolAttr
        (
            const   tCIDLib::TBoolean       bValue
        )   const;

        tCIDLib::TVoid SetAttrEditType
        (
                    tCIDMData::TAttrList&   colList
            , const TString&                strId
            , const tCIDMData::EAttrEdTypes eToSet
        )   const;

        tCIDLib::TVoid SetAttrLimits
        (
                    tCIDMData::TAttrList&   colList
            , const TString&                strId
            , const TString&                strToSet
        )   const;

        tCIDLib::TVoid SetAttrName
        (
                    tCIDMData::TAttrList&   colList
            , const TString&                strId
            , const TString&                strToSet
        )   const;



    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDMData,TFacility)
};

#pragma CIDLIB_POPPACK

