//
// FILE NAME: CIDDocComp_Pages.hpp
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
//  This file defines the classes that represent the class hierarchy we use to
//  hold the parsed data until we can spit it out.
//
//  The (containment) hierarhcy is:
//
//      Topic
//          Pages
//          Sub-Topics
//
//  So it's a recursive scheme that starts with a faux root topic, and then each topic
//  holds any pages at that level and any sub-topics, and those sub-topics in turn do
//  the same. Each entry contains the link text to be used in the left hand links
//  section, the file (for pages) or sub-dir (for sub-topics) name for that entry.
//
//  We also support 'alias' page links, sort of naturally here. For instance, the
//  same driver (and hence its doc content) is used for multiple devices. So we will
//  often reference the same page file from multiple page entries in a given topic. So we
//  keep a list of pages compiled, in a keyed hash set, keyed by the 'path' (the
//  ones in the form /Reference/Drivers/Security/DSC-IT100. These are unique for
//  every page. That way we can quickly see if we have already found, compiled and
//  stored away any given file. If not done yet, we do it.
//
//  Each topic (page) entry will be updated with the path of the file it references,
//  which will be used later to generate the link. Actually we store the path, and
//  then the parent path and name separately, all of which are useful to have for
//  fast access.
//
//  When generating any alternative indices, we have the info we need to generate links,
//  and we can quickly get to the actual pages to get other information out we require.
//
//
//  We have a simple hierarchy of pages, since there are some specialized ones.
//  The inheritance hierarchy is:
//
//      TBasePage
//          THelpPage       - A standard help page, for general purpose help text.
//          TCMLClassPage   - Holds a class, and it's methods, nested types, and
//                              literals.
//          TDevClassPage   - Holds the info for a V2 device class
//          TDevDriverPage  - Holds the info for a device driver
//
//
//  In a lot of places we just need to hold general help content, in various sections
//  of any of the page types. Our DTD defines the general help content tags we support.
//  So we have a THelpNode that we use to store that sort of content anywhere we need
//  to. It knows how to parse that content out and format it back out again as the final
//  HTML content. So that makes a lot of things much easier.
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
//
//  The base help page class, so that we can store them heterogenously and store
//  common stuff in one place.
// ---------------------------------------------------------------------------
class TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bProcessHelpText
        (
            const   TXMLTreeElement&        xtnodeText
            ,       THelpNode&              hnToFill
        );

        static tCIDLib::TBoolean bProcessHelpText
        (
            const   TXMLTreeElement&        xtnodePar
            , const TString&                strName
            ,       THelpNode&              hnToFill
            , const tCIDLib::TBoolean       bOptional
        );

        static tCIDLib::TBoolean bProcessTableRow
        (
            const   TXMLTreeElement&        xtnodeRow
            ,       THelpNode&              hnToFill
        );

        static tCIDLib::TBoolean bProcessTable
        (
            const   TXMLTreeElement&        xtnodeTbl
            ,       THelpNode&              hnToFill
        );

        static tCIDLib::TVoid OutputNodes
        (
                    TTextOutStream&         strmTar
            , const THelpNode&              hnText
            , const TString&                strKey
        );

        static tCIDLib::TVoid OutputTableRow
        (
                    TTextOutStream&         strmTar
            , const THelpNode&              hnRow
            , const TString&                strKey
        );

        static tCIDLib::TVoid OutputTable
        (
                    TTextOutStream&         strmTar
            , const THelpNode&              hnRow
            , const TString&                strKey
        );

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

        static const TString& strKey
        (
            const   TBasePage&              pgSrc
        );



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBasePage();

        TBasePage(const TBasePage&) = delete;

        virtual ~TBasePage();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bParseXML
        (
            const   TXMLTreeElement&        xtnodeRoot
            , const TString&                strName
            , const TTopic&                 topicPar
        );

        virtual tCIDLib::TVoid GenerateOutput
        (
                    TTextOutStream&         strmTar
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public class members
        //
        //  m_strFileName
        //      The name of the file from which this content came. It's just the
        //      base name, which is also included at the end of Path.
        //
        //  m_strPath
        //      The key for this page, which is a /x/y type path, that reflects
        //      the place in the hierarchy of this page and acts as a unique
        //      key. The values are the directory names along the way (relative to
        //      to the top level help directory) and the file name at the end.
        //
        //  m_strTitle
        //      The title text of the page, from the XML, used to generate the link
        //      text. In the case of aliases it's the alias text to be used to generate
        //      the link.
        // -------------------------------------------------------------------
        TString             m_strFileName;
        TString             m_strPath;
        TString             m_strTitle;
};



// ---------------------------------------------------------------------------
//   CLASS: THelpPage
//  PREFIX: pg
//
//  This is a standard help page, i.e. not one of the special cases. So it just
//  holds HTML node data basically, for the text of that topic page. So we don't
//  need anything data-wise beyond the parent class, but we have to have our own
//  parsing because we have to find the right XML node to parse the HTML node
//  data from. We use the standard help node class to store the node content.
// ---------------------------------------------------------------------------
class THelpPage : public TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THelpPage();

        THelpPage(const THelpPage&) = delete;

        ~THelpPage();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseXML
        (
            const   TXMLTreeElement&        xtnodeRoot
            , const TString&                strName
            , const TTopic&                 topicPar
        )   override;

        tCIDLib::TVoid GenerateOutput
        (
                    TTextOutStream&         strmTar
        )   const override;


        // -------------------------------------------------------------------
        //  Public class members
        //
        //  m_hnText
        //      This type of page is just a big chunk of general markup, so we can hold
        //      it all in just a single help node.
        // -------------------------------------------------------------------
        THelpNode   m_hnText;
};




// ---------------------------------------------------------------------------
//   CLASS: TPageInfo
//  PREFIX: pinfo
//
//  This holds the info we parse out of the page entries of each topic XML file. This is
//  used to keep the info we require to generate the left side page links for that topic.
// ---------------------------------------------------------------------------
class TPageInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPageInfo();

        TPageInfo(const TPageInfo&) = default;

        ~TPageInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Complete
        (
            const   TString&                strParPath
        );

        tCIDLib::TVoid Reset();


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_bVirtual
        //      Some page links are virtual ones. They will be generated after the initial
        //      scan, but before the output generation. So this tells the initial scan to
        //      not try to actually parse this file.
        //
        //  m_ppgThis
        //      Once the page is parsed and stored away, we store a pointer to it so that
        //      we can later get from here to the page for index generation.
        //
        //  m_strExt
        //      The file extension of this file.
        //
        //  m_strExtra1
        //  m_strExtra2
        //      Some extra values that can be set in the topic file, for file type specific
        //      use.
        //
        //  m_strFile
        //      The base name of the file (at the same directory level) that represents
        //      the content of this page. This comes from the topic file XML.
        //
        //  m_strPath
        //      The 'help path' of the file that represents the content of this page. This
        //      allows us to quickly look up the file in the list of compiled files, since
        //      this is the key field for that collection.
        //
        //  m_strTitle
        //      This is the text to use for the link. This comes from the topic file XML.
        //
        //  m_strTopicPath
        //      This is the path of the topic in which the page is located, so it's
        //      the m_strPath minus the file name part.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bVirtual;
        TBasePage*          m_ppgThis;
        TString             m_strExt;
        TString             m_strExtra1;
        TString             m_strExtra2;
        TString             m_strFile;
        TString             m_strPath;
        TString             m_strTitle;
        TString             m_strTopicPath;
};



// ---------------------------------------------------------------------------
//   CLASS: TSubTopicInfo
//  PREFIX: stinfo
//
//  This holds the info we parse out of the topic entries of each topic XML file. We could
//  just put it directly into the topic objects. But, since we reuse the same XML parser
//  in a recursive way, we want to get all of the topic info out before we start parsing
//  the referenced files. So at each level we set up a list of these, and later use them
//  to add and recurse on the sub-topics.
//
//  We also keep an overall list of them so that we can generate a topic index at the
//  end.
// ---------------------------------------------------------------------------
class TSubTopicInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSubTopicInfo();

        TSubTopicInfo(const TSubTopicInfo&) = default;

        ~TSubTopicInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Complete
        (
            const   TString&                strParPath
        );


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_strPath
        //      The 'help path' of the directory that holds this topic. This allows us
        //      to quickly look up the topic.
        //
        //  m_strSubDir
        //      The base name of the subdirectory that this sub-topic is in, relative
        //      to the current directory being processed.
        //
        //  m_strTitle
        //      This is the text to use for the link. This comes from the topic file XML.
        // -------------------------------------------------------------------
        TString     m_strPath;
        TString     m_strSubDir;
        TString     m_strTitle;
};



// ---------------------------------------------------------------------------
//   CLASS: TTopic
//  PREFIX: topic
//
//  A topic is basically a directory in the tree hierarchy, which is about some particular
//  topic and contains help pages about that topic (and sub-directories which are sub-tops
//  of that topic.)
//
//  A faux one of these is created to start, and then the recursive parsing process is
//  kicked off and fills in a tree structure under the first one.
// ---------------------------------------------------------------------------
class TTopic
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TParsedPageList = TRefKeyedHashSet<TBasePage, TString, TStringKeyOps>;
        using TPageInfoList = TRefVector<TPageInfo>;
        using TChildTopicList = TRefVector<TTopic>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTopic();

        TTopic
        (
            const   TString&                strParDir
            , const TString&                strDir
            , const TString&                strTitle
        );

        TTopic(const TTopic&) = delete;

        ~TTopic();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TPageInfo& tpageTopicPage
        (
                    tCIDLib::TCard4&        c4At
        )   const;

        tCIDLib::TVoid GenerateOutput
        (
            const   TTopic&                 topicPar
            , const TString&                strTarDir
        )   const;

        tCIDLib::TVoid GenerateTopicIndex
        (
            const   TTopic&                 topicPar
            ,       TTextOutStream&         strmOutFl
        )   const;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_colChildTopicMap
        //      These are the recursive child topics under this one.
        //
        //  m_colPageMap
        //      This is the file info we parse from the topic file at this level. It tells
        //      us all of the page links we have to generate. And it references the help
        //      files at this level we need to find and compile. Some of the pages may be
        //      referenced multiple times, see m_colParsedPages.
        //
        //      This is a non-adopting by ref list. The page objects are actually stored
        //      in the overall main page list in the facility.
        //
        //  m_colParsedPages
        //      This is a hash set of the actual pages compiled for this topic. Since we
        //      sometimes have multiple page links pointing to the same page, we keep track of
        //      the actual pages separately. It is keyed on the pages 'path', which is stored
        //      in the various page map entries that reference it. If it's already been found
        //      and compiled, then we don't have to do it again.
        //
        //  m_strDirName
        //      The base name of this topic's directory, to make it easy to build the
        //      target output directories during output generation, so we don't have to
        //      parse it back out of the source path.
        //
        //  m_strSrcPath
        //      The actual file system path to the topic directory.
        //
        //  m_strTopicPath
        //      The topic path for this topic. Since this is a topic, this key is a
        //      non-leaf type node key. The components of the keys are the directories
        //      along the way, with / slashes, e.g. /Reference/Drivers.
        //
        //  m_strTitle
        //      The title of this topic. This comes from the CQCTopic.xml file in this
        //      topic's directory level, and is used to generate the link.
        //
        //  m_strTopicPage
        //      The name (just the base name) of the .cqchpage file that is the main page
        //      for this topic. This one is put at the top. If this is empty, we just take
        //      the 0th file in the page map. This comes from the CQCTopic.xml file.
        // -------------------------------------------------------------------
        TChildTopicList     m_colChildTopicMap;
        TPageInfoList       m_colPageMap;
        TParsedPageList     m_colParsedPages;
        TString             m_strDirName;
        TString             m_strSrcPath;
        TString             m_strTopicPath;
        TString             m_strTitle;
        TString             m_strTopicPage;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenPageLink
        (
            const   TTopic&                 topicPar
            , const TPageInfo&              pinfoSrc
            ,       TTextOutStream&         strmOut
        )   const;

        tCIDLib::TVoid GenTopicLink
        (
            const   TTopic&                 stinfoSrc
            ,       TTextOutStream&         strmOut
        )   const;
};

