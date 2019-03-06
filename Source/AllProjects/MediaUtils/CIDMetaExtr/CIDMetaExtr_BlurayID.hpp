//
// FILE NAME: CIDMetaExtr_BlurayID.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/29/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements an algorithm that will generate a unique id for a
//  blu-ray disc that's currently in some drive where we can read it. We get
//  the path to the root of the volume.
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
//  CLASS: TCIDBlurayID
// PREFIX: dvdid
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDBlurayID : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDBlurayID();

        ~TCIDBlurayID();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenBlurayID
        (
            const   TString&                strBlurayPath
            ,       tCIDLib::TCard8&        c8ID
            ,       TString&                strFmtID
        );


    private :
        // -------------------------------------------------------------------
        //  Unimpleemnted
        // -------------------------------------------------------------------
        TCIDBlurayID(const TCIDBlurayID&);
        tCIDLib::TVoid operator=(const TCIDBlurayID&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDBlurayID,TObject)
};

#pragma CIDLIB_POPPACK



