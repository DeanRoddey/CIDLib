//
// FILE NAME: CIDDocComp_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDDocComp_ThisFacility.cpp module. This module
//  implements the facility class for this program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TFacCIDDocComp
// PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDDocComp : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDDocComp();

        TFacCIDDocComp(const TFacCIDDocComp&) = delete;

        ~TFacCIDDocComp();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDDocComp& operator=(const TFacCIDDocComp&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        TTextOutStream& strmOut()
        {
            return TSysInfo::strmOut();
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CopyDir
        (
            const   TString&                strSrc
            , const TString&                strTar
        );

        tCIDLib::TVoid LoadDTD
        (
                    TParseCtx&              ctxToUse
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrRoot
        //      The top-most topic object, which isn't a real one, it just provides the
        //      root for the hierarchy to kick start it.
        //
        //  m_ctxParse
        //      Our parse contect object that we pass into the parse calls.
        //
        //  m_strSrcPath
        //  m_strTarPath
        //      The source and target paths that we figure out when we first start up and
        //      put here for further reference.
        // -------------------------------------------------------------------
        TTopic::TTopicPtr   m_cptrRoot;
        TParseCtx           m_ctxParse;
        TString             m_strSrcPath;
        TString             m_strTarPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDocComp, TFacility)
};

