//
// FILE NAME: CIDXML_FileEntitySrc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_FileEntitySrc.Cpp file, which
//  implements the TFileEntitySrc class. This is a derivative of the base
//  entity source class which provides support for file based sources.
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
//  CLASS: TFileEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------
class CIDXMLEXP TFileEntitySrc : public TXMLEntitySrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileEntitySrc
        (
            const   TString&                strSystemId
        );

        TFileEntitySrc
        (
            const   TString&                strParentSysId
            , const TString&                strSystemId
        );

        TFileEntitySrc
        (
            const   TString&                strParentSysId
            , const TString&                strSystemId
            , const TString&                strPublicId
        );

        ~TFileEntitySrc();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TBinInStream* pstrmMakeNew() const;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFileEntitySrc,TXMLEntitySrc)
        DefPolyDup(TFileEntitySrc)
};

#pragma CIDLIB_POPPACK
