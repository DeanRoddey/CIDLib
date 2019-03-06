//
// FILE NAME: CIDDBase_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDDBase_ThisFacility.Cpp file. This
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
//  CLASS: TFacCIDDBase
// PREFIX: fac
// ---------------------------------------------------------------------------
class  CIDDBASEEXP TFacCIDDBase : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDDBase();

        TFacCIDDBase(const TFacCIDDBase&) = delete;

        ~TFacCIDDBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDDBase& operator=(const TFacCIDDBase&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDDBase::THandle hEnv() const;

        tCIDLib::TVoid Initialize();

        tCIDLib::TVoid QuerySources
        (
                    tCIDLib::TKVPCollect&   colToFill
            , const tCIDDBase::ESrcTypes    eType
        );

        tCIDLib::TVoid Terminate();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDBase,TFacility)
};

#pragma CIDLIB_POPPACK


