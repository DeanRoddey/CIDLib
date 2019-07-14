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
        //  Public class members
        //
        //  These are just for short term use by the parsing code to avoid a lot
        //  of constant creation and destruction.
        // -------------------------------------------------------------------
        tCIDLib::TStrList       m_colTmp;
        TString                 m_strTmp1;
        TString                 m_strTmp2;


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
        tCIDLib::TVoid AddClass
        (
                    TCppClassPage* const    ppgToAdd
        );

        tCIDLib::TVoid AddErrorMsg
        (
            const   TString&                strMsg
            , const MFormattable&           mfmtblToken1 = MFormattable::Nul_MFormattable()
            , const MFormattable&           mfmtblToken2 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TBoolean bGotErrors() const
        {
            return m_colErrs.c4ElemCount() != 0;
        }

        tCIDLib::TBoolean bNoClassRefWarn() const
        {
            return m_bNoClassRefWarn;
        }

        tCIDLib::TBoolean bParseXML
        (
            const   TString&                strFile
        );

        tCIDLib::TBoolean bPostParse() const
        {
            return m_bPostParse;
        }

        tCIDLib::TBoolean bVerbose() const
        {
            return m_bVerbose;
        }

        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid FormatDeemphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText
        );

        tCIDLib::TVoid FormatDeemphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText1
            , const TString&                strText2
        );

        tCIDLib::TVoid FormatDeemphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText1
            , const TString&                strText2
            , const tCIDLib::TCh* const     pszText3
        );

        tCIDLib::TVoid FormatEmphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText
        );

        tCIDLib::TVoid FormatEmphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText1
            , const TString&                strText2
        );

        tCIDLib::TVoid FormatEmphText
        (
                    TTextOutStream&         strmTar
            , const TString&                strText1
            , const TString&                strText2
            , const tCIDLib::TCh* const     pszText3
        );

        tCIDLib::TVoid GenerateClassLink
        (
                    TTextOutStream&         strmTar
            , const TString&                strClass
        )   const;

        tCIDLib::TVoid GenerateFacLink
        (
                    TTextOutStream&         strmTar
            , const TString&                strFacName
        )   const;

        tCIDLib::TVoid LoadDTD
        (
            const   TString&                strSrcPath
        );

        const TCppClassPage* ppgFindByClass
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid PostParseProc();

        const TString& strCurClass() const;

        const TString& strCurClassPref() const;

        TTextOutStream& strmOut()
        {
            return TSysInfo::strmOut();
        }

        tCIDLib::TVoid ShowErrors
        (
                    TTextOutStream&         strmOut
        );

        const TXMLTreeElement& xtnodeRoot() const
        {
            return m_xtprsToUse.xtdocThis().xtnodeRoot();
        }



    protected :
        // -------------------------------------------------------------------
        //  The janitor below has to be our friend
        // -------------------------------------------------------------------
        friend class TCtxStackJan;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TClassNameList    = TRefVector<TCppClassPage>;
        using TErrList          = TVector<TKeyValuePair>;
        struct TSrcStackItem
        {
            tCIDDocComp::ESrcTypes  m_eType;
            TString                 m_strName;
            TString                 m_strPath;
            TString                 m_strExtra;
        };
        using TSrcStack         = TStack<TSrcStackItem>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseParams();

        tCIDLib::TVoid CopyDir
        (
            const   TString&                strSrc
            , const TString&                strTar
        );

        tCIDLib::TVoid LoadDTD();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNoClassRefWarn
        //      Sometimes we add class references before we have those classes yet
        //      defined. This is set via /NoClassRefWarn and will suppress those errors
        //      temporarily when needed.
        //
        //  m_bPostParse
        //      Once the parsing is done, PostParseProc() is called. We update some
        //      info and set this. We use this to insure some things are only called
        //      in the right phase.
        //
        //  m_bVerbose
        //      Defaults to false, can be set via the /Verbose command line option.
        //      That will cause it to dump out diagnostic stuff to help figure out
        //      issues in the help content.
        //
        //  m_colErrs
        //      The parsing code calls into here to store errors during the parsing
        //      process. Has to be mutable since we need to report errors that might
        //      occur in otherwise const methods.
        //
        //  m_colClassByName
        //      Each new class page is added to this non-adopting list, which will be
        //      sorted at the end. This provides a means to automatically resolve
        //      class references during output, so we don't have to hand create all
        //      those links. We provide a method to add classes, and one to find the
        //      the link info for a class.
        //
        //  m_colSrcStack
        //      During parsing and output generation, the context stack janitor
        //      below is used to keep up with the currently active topic or class
        //      or other page type.
        //
        //  m_cptrRoot
        //      The top-most topic object, which isn't a real one, it just provides the
        //      root for the hierarchy to kick start it.
        //
        //  m_strSrcPath
        //  m_strTarPath
        //      The source and target paths that we figure out when we first start up and
        //      put here for further reference.
        //
        //  m_xtprsToUse
        //      We keep a single parser around and reuse it
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNoClassRefWarn;
        tCIDLib::TBoolean   m_bPostParse;
        tCIDLib::TBoolean   m_bVerbose;
        TClassNameList      m_colClassByName;
        mutable TErrList    m_colErrs;
        TSrcStack           m_colSrcStack;
        TTopic::TTopicPtr   m_cptrRoot;
        TString             m_strSrcPath;
        TString             m_strTarPath;
        TXMLTreeParser      m_xtprsToUse;

        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDocComp, TFacility)
};


// -----------------------------------------------------------------------------
//   CLASS: TCtxStackJan
//  PREFIX: jan
// -----------------------------------------------------------------------------
class TCtxStackJan
{
    public :
        TCtxStackJan() = delete;

        TCtxStackJan(const TTopic& topicToPush);
        TCtxStackJan(const THelpPage& pgToPush);
        TCtxStackJan(const TCppClassPage& pgToPush);
        TCtxStackJan(const TNamespacePage& pgToPush);

        TCtxStackJan(const TCtxStackJan&) = delete;

        ~TCtxStackJan();
};
