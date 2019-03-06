//
// FILE NAME: CIDMetaExtr_Sony777DVDID.hpp
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
//  DVD disc, in the form used by Sony in their 777 changer, and also used
//  by the online metadata source AMG.
//
//  The returned ID is a string of formatted hex numbers.
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
//  CLASS: TCIDSony777DVDID
// PREFIX: dvdid
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDSony777DVDID : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSony777DVDID();

        ~TCIDSony777DVDID();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenSony777DVDID
        (
            const   TString&                strDVDPath
            ,       TString&                strFmtID
        );


    private :
        // -------------------------------------------------------------------
        //  Unimpleemnted
        // -------------------------------------------------------------------
        TCIDSony777DVDID(const TCIDSony777DVDID&);
        tCIDLib::TVoid operator=(const TCIDSony777DVDID&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenChapterLen
        (
            const   tCIDLib::TCard1* const  pc1Buf
            , const tCIDLib::TCard4         c4TblOfs
            , const tCIDLib::TCard4         c4PTTCnt
            ,       TString&                strToFill
        )   const;

        tCIDLib::TCard1* pc1ReadDVDFile
        (
            const   TString&                strFilePath
            ,       tCIDLib::TCard4&        c4Size
        )   const;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDSony777DVDID,TObject)
};

#pragma CIDLIB_POPPACK




