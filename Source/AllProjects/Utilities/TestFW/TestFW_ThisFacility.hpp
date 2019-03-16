//
// FILE NAME: TestFW_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
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
//  This is the header for the facility class. The main module just starts
//  up the program on our eMainThread() method. We provide the main program
//  functinality here.
//
// CAVEATS/GOTCHAS:
//
//  1)  This guy is globally allocated, so it cannot allocate any non-trivial
//      members. It should fault them in after construction.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TFacTestFW
// PREFIX: fac
// ---------------------------------------------------------------------------
class TFacTestFW : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TFacTestFW();

        TFacTestFW(const TFacTestFW&) = delete;

        ~TFacTestFW();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacTestFW& operator=(const TFacTestFW&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        TTextOutStream& strmOut();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckFmtType
        (
            const   TString&                strToCheck
            ,       TTFWHTMLFormatter**     pptfwfmToFill
        )   const;

        tCIDLib::TBoolean bParseCfgFile();

        tCIDLib::TBoolean bParseLine
        (
                    TTextInStream&          strmSrc
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bEOFOK
        );

        tCIDLib::TBoolean bParseParms();

        tCIDLib::TCard4 c4FindGroup
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TCard4 c4FindTestPrg
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid CheckLine
        (
                    TTextInStream&          strmSrc
            ,       TString&                strTmp
            , const TString&                strExpected
        );

        tCIDLib::TVoid CheckLine
        (
            const   TString&                strTmp
            , const TString&                strExpected
        );

        tCIDLib::TVoid CheckLinePref
        (
                    TTextInStream&          strmSrc
            ,       TString&                strTmp
            , const TString&                strPref
        );

        tCIDLib::TVoid GenerateResults();

        tCIDLib::TVoid InvokeGroups();

        tCIDLib::TVoid ParseGroups
        (
                    TTextInStream&          strmSrc
        );

        tCIDLib::TVoid ParseTestPrgs
        (
                    TTextInStream&          strmSrc
        );

        tCIDLib::TVoid ShowBlurb();



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4LineNum
        //      Used during file parsing. The bParseLine() method wil keep
        //      it up to date.
        //
        //  m_c4MaxLevel
        //      The maximum test level we'll run. It's set via the /MaxLevel=
        //      command line parm. If not set, it defaults to 10, which means
        //      do't do any level based filtering.
        //
        //  m_c4NextGrouppId
        //  m_c4NextTestPrgId
        //      We assign a sequential id to each group and test program
        //      that's added. This is used when we generate links to the
        //      detailed results pages.
        //
        //  m_colGroupList
        //      The list of groups we parsed out of the config file.
        //
        //  m_colGroupsToDo
        //      This is filled in with the list of groups that we got on
        //      the /Groups= command line parm.
        //
        //  m_colTestPrgList
        //      The list of test programs we parsed out of the config file.
        //
        //  m_eVerbosity
        //      The verbosity level. Defaults to low unless overridden by the
        //      /Verbose= parameter.
        //
        //  m_fcolStats
        //      This is filled in with overall statistics before the output
        //      formatting.
        //
        //  m_pathTestBaseDir
        //      Some tests want to access local test files, in their source
        //      directory. So they indicate a working directory that we
        //      should start them on. If it's relative, then it is relative
        //      to the path where the test config file was found. So we
        //      break out the base path of the config file and store it
        //      here to avoid having to pull it out over and over.
        //
        //  m_pstrmErr
        //  m_pstrmOut
        //      The output and error streams that we output any progress
        //      and error messsages to. We set these via TSysInfo, so that
        //      we pick up any redirections.
        //
        //  m_strCfgFile
        //      The path to the configuration file that we will parse to see
        //      what tests we should invoke.
        //
        //  m_strOutFmt
        //      The output format indicated by the user on the command line.
        //      If not indicated, we default to HTML.
        //
        //  m_strResPath
        //      The path where the results will be written. If not set, it
        //      will default to a TestResults directory under the CIDBuild
        //      output directory.
        //
        //  m_strTestCfgName
        //      This is the text after the opening tests block (Tests=), which
        //      is used as the overall test configuration name. It's just
        //      passed to the formatter to be formatted into the header of
        //      main summary page.
        //
        //  m_strTestRoot
        //      This is the value set by the /TestRoot= parameter. They can
        //      use a token <Root> in the config file as the first part of
        //      the test program path. We replace that token with this value.
        //      If not set, it will default to the CIDBuild output directory.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4LineNum;
        tCIDLib::TCard4         m_c4MaxLevel;
        tCIDLib::TCard4         m_c4NextGroupId;
        tCIDLib::TCard4         m_c4NextTestPrgId;
        tTestFW::TGroupList     m_colGroupList;
        tTestFW::TNameList      m_colGroupsToDo;
        tTestFW::TTestPrgList   m_colTestPrgList;
        tTestFWLib::EVerbosity  m_eVerbosity;
        tTestFW::TStatList      m_fcolStats;
        TPathStr                m_pathTestBaseDir;
        TTextOutStream*         m_pstrmErr;
        TTextOutStream*         m_pstrmOut;
        TString                 m_strCfgFile;
        TString                 m_strOutFmt;
        TString                 m_strResPath;
        TString                 m_strTestCfgName;
        TString                 m_strTestRoot;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacTestFW,TFacility)
};

