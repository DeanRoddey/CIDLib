//
// FILE NAME: TestFW_Formatter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2006
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
//  This is the header for the abstract base class that handles output
//  formatting. It get's passed all the test results info and is responsible
//  for spitting it out in some desired format, e.g. HTML, XML, etc...
//  There are derivatives of the base class for each supported format.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTFWFormatter
// PREFIX: tfwfm
// ---------------------------------------------------------------------------
class TTFWFormatter : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TTFWFormatter();

        TTFWFormatter(const TTFWFormatter&) = delete;

        ~TTFWFormatter();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid FormatResult
        (
            const   TString&                strOutPath
            , const TString&                strCfgFile
            , const TString&                strTestCfgName
            , const tCIDLib::TCard4         c4MaxLevel
            , const tTestFW::TStatList&     fcolStats
            , const tTestFW::TGroupList&    colGroupList
            , const tTestFW::TNameList&     colGroupsRun
        )   = 0;


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        const TTFWGroupInfo& tfwgiFindGrp
        (
            const   TString&                strToFind
            , const tTestFW::TGroupList&    colGroupList
        );


    private :
        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWFormatter,TObject)
};




// ---------------------------------------------------------------------------
//  CLASS: TTFWHTMLFormatter
// PREFIX: tfwfm
// ---------------------------------------------------------------------------
class TTFWHTMLFormatter : public TTFWFormatter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TTFWHTMLFormatter();

        TTFWHTMLFormatter(const TTFWHTMLFormatter&) = delete;

        ~TTFWHTMLFormatter();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatResult
        (
            const   TString&                strOutPath
            , const TString&                strCfgFile
            , const TString&                strTestCfgName
            , const tCIDLib::TCard4         c4MaxLevel
            , const tTestFW::TStatList&     fcolStats
            , const tTestFW::TGroupList&    colGroupList
            , const tTestFW::TNameList&     colGroupsRun
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strOutPath
        //      We store this so as to avoid having to pass it all the
        //      various file generating private methods.
        //
        //  m_strNewLine
        //  m_strTab
        //  m_strSpace
        //      In any text output we get in the case of errors, we have to
        //      replace newlines, tabs, and spaces with their HTML equiv.
        // -------------------------------------------------------------------
        TString m_strOutPath;
        TString m_strNewLine;
        TString m_strTab;
        TString m_strSpace;
        TString m_strErrEmph;
        TString m_strWarnEmph;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TString strEscape
        (
            const   TString&                strToEscape
        );

        tCIDLib::TVoid WriteFooter
        (
                    TTextOutStream&         strmTar
        );

        tCIDLib::TVoid WriteGroupResults
        (
            const   tTestFW::TGroupList&    colGroupList
        );

        tCIDLib::TVoid WriteHeader
        (
                    TTextOutStream&         strmTar
            , const TString&                strTitle
            , const TString&                strHeader
        );

        tCIDLib::TVoid WriteLinksTable
        (
                    TTextOutStream&         strmTar
            , const tTestFW::TGroupList&    colGroupList
        );

        tCIDLib::TVoid WriteOneGroupResult
        (
            const   TTFWGroupInfo&          tfwgiSrc
        );

        tCIDLib::TVoid WriteStats
        (
                    TTextOutStream&         strmTar
            , const tTestFW::TStatList      fcolStats
        );

        tCIDLib::TVoid WriteSTR
        (
                    TTextOutStream&         strmTar
            , const TString&                strStatName
            , const tCIDLib::TCard4         c4Value
            , const TString&                strEmphClass
            , const tCIDLib::TBoolean       bGrpHeader = kCIDLib::False
        );

        tCIDLib::TVoid WriteTestProgramResults
        (
            const   TTFWTestPrgInfo&        tfwtpiSrc
        );

        tCIDLib::TVoid WriteTestResult
        (
            const   TTFWTestPrgInfo&        tfwtpiSrc
            , const tCIDLib::TCard4         c4TestInd
        );


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWHTMLFormatter,TTFWFormatter)
};


