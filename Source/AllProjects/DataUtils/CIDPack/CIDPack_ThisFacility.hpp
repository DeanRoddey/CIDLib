//
// FILE NAME: CIDPack_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/21/2015
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
//  This is the header for the CIDPack_ThisFacility.cpp file. This file
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

class TMessageDigest5;
class TZLibCompressor;


// ---------------------------------------------------------------------------
//  CLASS: TFacCIDPack
// PREFIX: fac
//
//  This is the facility class for this facility.
// ---------------------------------------------------------------------------
class CIDPACKEXP TFacCIDPack : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDPack();

        ~TFacCIDPack();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreatePackage
        (
            const   TString&                strTarFile
            , const TString&                strSrcPath
            , const tCIDLib::TBoolean       bOverwrite
            , const TFindBuf&               fndbTreeTop
            , const tCIDLib::TCard8         c8Version
            , const tCIDLib::TEncodedTime   enctStamp
            , const TString&                strNotes
            ,       TTextOutStream* const   pstrmStatus
            ,       tCIDLib::TCard4&        c4SoFar
            , const tCIDLib::TBoolean       bVerbose
        );

        tCIDLib::TVoid ExtractDetails
        (
            const   TString&                strSrcFile
            ,       tCIDLib::TCard8&        c8Version
            ,       tCIDLib::TEncodedTime&  enctStamp
            ,       TString&                strNotes
            ,       tCIDLib::TCard4&        c4TotalFiles
        );

        tCIDLib::TVoid ExtractPackage
        (
            const   TString&                strSrcFile
            , const TString&                strTarPath
            , const tCIDLib::TBoolean       bOverwrite
            ,       tCIDLib::TCard8&        c8Version
            ,       tCIDLib::TEncodedTime&  enctStamp
            ,       TString&                strNotes
            ,       tCIDLib::TCard4&        c4TotalFiles
            ,       TTextOutStream* const   pstrmStatus
            ,       tCIDLib::TCard4&        c4SoFar
            , const tCIDLib::TBoolean       bVerbose
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacCIDPack(const TFacCIDPack&);
        tCIDLib::TVoid operator=(const TFacCIDPack&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CountFiles
        (
            const   TFindBuf&               fndbTreeTop
            ,       tCIDLib::TCard4&        c4ToSet
        );

        tCIDLib::TVoid PackFile
        (
                    TBinOutStream&          strmTar
            , const TFindBuf&               fndbFile
            , const TString&                strSrcPath
            ,       TMessageDigest5&        mdigToUse
            ,       TZLibCompressor&        zlibToUse
            ,       TChunkedBinOutStream&   strmComp
            ,       TTextOutStream* const   pstrmStatus
            ,       tCIDLib::TCard4&        c4SoFar
            , const tCIDLib::TBoolean       bVerbose
        );

        tCIDLib::TVoid PackFiles
        (
                    TBinOutStream&          strmTar
            , const TFindBuf&               fndbCurDir
            , const TString&                strSrcPath
            ,       TMessageDigest5&        mdigToUse
            ,       TZLibCompressor&        zlibToUse
            ,       TChunkedBinOutStream&   strmComp
            ,       TTextOutStream* const   pstrmStatus
            ,       tCIDLib::TCard4&        c4SoFar
            , const tCIDLib::TBoolean       bVerbose
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDPack,TFacility)
};

#pragma CIDLIB_POPPACK


