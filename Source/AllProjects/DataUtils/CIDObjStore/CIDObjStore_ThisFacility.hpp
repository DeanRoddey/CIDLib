//
// FILE NAME: CIDObjStore_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the facility class of the object store engine.
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
//   CLASS: TFacCIDObjStore
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDOBJSTOREEXP TFacCIDObjStore : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDObjStore();

        TFacCIDObjStore(const TFacCIDObjStore&) = delete;

        ~TFacCIDObjStore();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDObjStore& operator=(const TFacCIDObjStore&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDObjStore,TFacility)
};

#pragma CIDLIB_POPPACK

