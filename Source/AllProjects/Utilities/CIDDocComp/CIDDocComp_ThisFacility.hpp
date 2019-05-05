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
            return *m_pstrmOut;
        }

        TTextOutStream& strmErr()
        {
            return m_strmErr;
        }

        tCIDLib::TVoid ShowXMLParseErr
        (
            const   TString&                strPathSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CopyDir
        (
            const   TString&                strSrc
            , const TString&                strTar
        );

        tCIDLib::TVoid LoadDTD();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bVerbose
        //      Defaults to false, can be set via the /Verbose command line option.
        //      That will cause it to dump out diagnostic stuff to help figure out
        //      issues in the help content.
        //
        //  m_cptrRoot
        //      The top-most topic object, which isn't a real one, it just provides the
        //      root for the hierarchy to kick start it.
        //
        //  m_pstrmOut
        //      If in verbose mode this is set to the standard output stream. Else it's
        //      s set to a string based text string so that we just eat the output
        //      (though still have it if an error occurs, so we can dump it out.)
        //
        //  m_strSrcPath
        //  m_strTarPath
        //      The source and target paths that we figure out when we first start up and
        //      put here for further reference.
        //
        //  m_strmErr
        //      The output stream for errors, which will show up even if we are not
        //      in verbose mode (where m_pstrmOut is nullptr.)
        //
        //  m_xtprsToUse
        //      Our XML parser that we set up and reuse for all of the files.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bVerbose;
        TTopic::TTopicPtr   m_cptrRoot;
        TTextOutStream*     m_pstrmOut;
        TString             m_strSrcPath;
        TString             m_strTarPath;
        TTextFileOutStream  m_strmErr;
        TXMLTreeParser      m_xtprsToUse;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDocComp, TFacility)
};

