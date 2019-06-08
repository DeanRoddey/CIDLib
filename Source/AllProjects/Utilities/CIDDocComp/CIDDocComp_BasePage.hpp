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

class TTopic;

// ---------------------------------------------------------------------------
//   CLASS: TBasePage
//  PREFIX: pg
// ---------------------------------------------------------------------------
class TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid QueryElemText
        (
            const   TXMLTreeElement&        xtnodePar
            ,       TString&                strToFill
        );

        static tCIDLib::TVoid QueryElemText
        (
            const   TXMLTreeElement&        xtnodePar
            , const TString&                strChildName
            ,       TString&                strToFill
        );

        static tCIDLib::TVoid QueryElemTextAt
        (
            const   TXMLTreeElement&        xtnodePar
            , const tCIDLib::TCard4         c4At
            ,       TString&                strToFill
        );

        static const TString& strKey(const TBasePage& pgSrc)
        {
            return pgSrc.m_strPagePath;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBasePage(const TBasePage&) = delete;

        virtual ~TBasePage();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBasePage& operator=(const TBasePage&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsThisName(const TString& strName) const
        {
            return strName.bCompareI(m_strFileName);
        }

        tCIDLib::TVoid ParseFile
        (
                    TTopic&                 topicParent
        );

        tCIDLib::TBoolean bVirtual() const
        {
            return m_bVirtual;
        }

        tCIDLib::TVoid GenerateOutput
        (
            const   TString&                strParPath

        )   const;

        tCIDLib::TVoid GenerateLink
        (
                    TTextOutStream&         strmTar
        )   const;

        const TString& strPagePath() const
        {
            return m_strPagePath;
        }

        const TString& strPageName() const
        {
            return m_strFileName;
        }


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TBasePage
        (
            const   TString&                strExtTitle
            , const TString&                strParSrcDir
            , const TString&                strParTopic
            , const TString&                strFileName
            , const tCIDDocComp::EPageTypes eType
            , const tCIDLib::TBoolean       bVirtual = kCIDLib::False
        );


    private :
        // -------------------------------------------------------------------
        //  Private, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Parse
        (
                    TTopic&                 topicParent
            , const TXMLTreeElement&        xtnodeRoot
        ) = 0;

        virtual tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const = 0;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bVirtual
        //      Some pages are virtual, so they are added to the list and filled in
        //      programmatically. This flag being set means there is no real file
        //      to try to parse.
        //
        //  m_colKeywords
        //      Any keywords defined for this page (used for glossary generation.)
        //      Most pages have them but they are optional.
        //
        //  m_eType
        //      The type of page, which is directly translated from the PageType
        //      attribute of the topic entry for the page.
        //
        //  m_strFileExt
        //      The extension for this file, which controls whate type of derived
        //      page class this will be (the owning topic will create us based on
        //      this.
        //
        //  m_strExtTitle
        //      This is our external title. This is what is set in the topic file
        //      that references us, so this is used in the left hand panel as the
        //      text for the link.
        //
        //  m_strFileName
        //      The base part of the file name, no extension or path, for this page.
        //
        //  m_strPagePath
        //      The help topic type path to this page, which is our parent's topic
        //      path, plus our base file name. This is our unique identifer for dups
        //      checks, unique topics lists, and such.
        //
        //  m_strParSrcDir
        //      The file system directory for our parent topic's content, which is
        //      where our source file should be found.
        //
        //  m_strParTopic
        //      Our parent's topic path.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bVirtual;
        tCIDLib::TStrList       m_colKeywords;
        tCIDDocComp::EPageTypes m_eType;
        TString                 m_strExtTitle;
        TString                 m_strFileName;
        TString                 m_strPagePath;
        TString                 m_strParSrcDir;
        TString                 m_strParTopic;
};
