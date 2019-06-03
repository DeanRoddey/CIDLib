//
// FILE NAME: CIDDocComp_BasePage.hpp
//
// AUTHOR: Dean Roddey
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
//
// DESCRIPTION:
//
//  WE have a number of different specialized help pages (general, class, facility
//  and so forth.) They all have some common info and we need to deal with them
//  generically, so this class provides a base class for them all to derive from.
//  THis info comes from the topic file, so we just are doing basic setup of the
//  pages during the initial pass through all the topics to build the tree.
//
//  The file extension tells the parsing code (in the topic class) what type of
//  file to create. They all provide a constructor that takes the basic info we
//  get from the topic file, and they in turn call our one available protected
//  ctor and we store that core info.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once

class TParseStackJan;


// -----------------------------------------------------------------------------
//   CLASS: TParseCtx
//  PREFIX: ctx
// -----------------------------------------------------------------------------
class TParseCtx
{
    public :
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
        //  Constructors
        // -------------------------------------------------------------------
        TParseCtx();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddErrorMsg
        (
            const   TString&                strMsg
            , const MFormattable&           mfmtblToken1 = MFormattable::Nul_MFormattable()
            , const MFormattable&           mfmtblToken2 = MFormattable::Nul_MFormattable()
        );

        tCIDLib::TBoolean bGotErrors() const
        {
            return m_colErrs.c4ElemCount() != 0;
        }

        tCIDLib::TBoolean bParseXML
        (
            const   TString&                strFile
        );

        tCIDLib::TBoolean bVerbose() const
        {
            return m_bVerbose;
        }

        tCIDLib::TVoid LoadDTD
        (
            const   TString&                strSrcPath
        );

        const TXMLTreeElement& xtnodeRoot() const
        {
            return m_xtprsToUse.xtdocThis().xtnodeRoot();
        }

        tCIDLib::TVoid ShowErrors
        (
                    TTextOutStream&         strmOut
        );


    protected :
        // -------------------------------------------------------------------
        //  The janitor below has to be our friend
        // -------------------------------------------------------------------
        friend class TParseStackJan;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TErrList  = TVector<TKeyValuePair>;
        using TSrcStack = TStack<TString>;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bVerbose
        //      Defaults to false, can be set via the /Verbose command line option.
        //      That will cause it to dump out diagnostic stuff to help figure out
        //      issues in the help content.
        //
        //  m_colErrs
        //      The parsing code calls into here to store errors during the parsing
        //      process.
        //
        //  m_colSrcStack
        //      A stack that each topic and page pushes itself onto for the duration
        //      of parsing that page, so that we always know the context of an error
        //      message being added to our list.
        //
        //  m_xtprsToUse
        //      We keep a single parser around and reuse it
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bVerbose;
        TErrList            m_colErrs;
        TSrcStack           m_colSrcStack;
        TXMLTreeParser      m_xtprsToUse;
};


// -----------------------------------------------------------------------------
//   CLASS: TParseStackJan
//  PREFIX: jan
// -----------------------------------------------------------------------------
class TParseStackJan
{
    public :
        TParseStackJan() = delete;

        TParseStackJan(TParseCtx* pctxTar, const TString& strSrc) :

            m_pctxTar(pctxTar)
        {
            pctxTar->m_colSrcStack.objPush(strSrc);
        }

        TParseStackJan(const TParseStackJan&) = delete;

        ~TParseStackJan()
        {
            m_pctxTar->m_colSrcStack.TrashTop();
        }

    private :
        TParseCtx*  m_pctxTar;
};
