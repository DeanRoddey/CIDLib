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
        //  Public, static data members
        // -------------------------------------------------------------------
        static const TString    s_strExt_HelpPage;

        static const TString    s_strColSpan;
        static const TString    s_strExtra;
        static const TString    s_strExtra1;
        static const TString    s_strExtra2;
        static const TString    s_strFileExt;
        static const TString    s_strFileName;
        static const TString    s_strId;
        static const TString    s_strPageLink;
        static const TString    s_strPageMap;
        static const TString    s_strRef;
        static const TString    s_strRoot;
        static const TString    s_strSubDir;
        static const TString    s_strSubTopicMap;
        static const TString    s_strTitle;
        static const TString    s_strTopicPage;
        static const TString    s_strType;
        static const TString    s_strV2Compat;
        static const TString    s_strVirtual;


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

    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TPageList = TVector<TPageInfo>;
        using TTopicList = TVector<TSubTopicInfo>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIterateTopics
        (
            const   TString&                strSrcPath
            , const TString&                strDirName
            , const TString&                strHelpPath
            ,       TTopic&                 topicToFill
        );

        tCIDLib::TBoolean bParsePageFile
        (
            const   TString&                strSrc
            , const TPageInfo&              pinfoSrc
            ,       TBasePage&              pgTar
            ,       TTopic&                 topicPar
            , const tCIDLib::TBoolean       bFileSrc
        );

        tCIDLib::TVoid CopyDir
        (
            const   TString&                strSrc
            , const TString&                strTar
        );

        tCIDLib::TVoid LoadDTD();

        tCIDLib::TVoid OutputStdHPHeader
        (
                    TTextOutStream&         strmTar
            , const TString&                strTitle
        );

        TTopic& topicFindByPath
        (
            const   TString&                strToFind
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bVerbose
        //      Defaults to false, can be set via the /Verbose command line option.
        //      That will cause it to dump out diagnostic stuff to help figure out
        //      issues in the help content.
        //
        //  m_colPages
        //      We keep a list of all of the page info objects. The topics keep references
        //      to the entries here that were defined in their directories, and references
        //      are also in m_coldriverPages.
        //
        //  m_colTopics
        //      We keep a list of all of the topics, that we can use later to generate an
        //      overall topic index at the end.
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
        //  m_strSrcSrcPath
        //      This the top level source path (m_strSrcPath) plus the Src path under
        //      it. I.e. the directory where the actual help content is.
        //
        //  m_strmErr
        //      The output stream for errors, which will show up even if we are not
        //      in verbose mode (where m_pstrmOut is nullptr.)
        //
        //  m_topicRoot
        //      The top-most topic object, which isn't a real one, it just provides the
        //      root for the hierarchy to kick start it.
        //
        //  m_xtprsToUse
        //      Our XML parser that we set up and reuse for all of the files.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bVerbose;
        TPageList           m_colPages;
        TTopicList          m_colTopics;
        TTextOutStream*     m_pstrmOut;
        TString             m_strSrcPath;
        TString             m_strSrcSrcPath;
        TString             m_strTarPath;
        TTextFileOutStream  m_strmErr;
        TTopic              m_topicRoot;
        TXMLTreeParser      m_xtprsToUse;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDocComp, TFacility)
};

