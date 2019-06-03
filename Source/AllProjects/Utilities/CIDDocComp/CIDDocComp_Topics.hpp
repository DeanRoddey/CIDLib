//
// FILE NAME: CIDDocComp_Topics.hpp
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
//  This file defines the topic class, which is a non-terminal node in our tree of
//  documentation content. It contains information about itself and about the child
//  sub-topics and help pages directly under it.
//
//  We parse out all of the info first and do a basic setup of the child page and
//  topic objects, basically storing some path and title info that is defined at the
//  top level. So we call SetBaseInfo() to set up the basic info, then put them in
//  our child lists.
//
//  Once the whole tree is processed, the top level code will go back and start
//  parsing the actual help files once it knows that all of them were found and
//  what the whole layout is.
//
//  In order to just reuse the same parser over and over, we parse all our own stuff
//  first, then go back and call the parse method on child topics, and they do the
//  same. So we can just pass a parser down through the recursion.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TTopic
//  PREFIX: topic
// ---------------------------------------------------------------------------
class TTopic
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TTopicPtr = TCntPtr<TTopic>;
        using TSubTopicList = TVector<TTopicPtr>;
        using TPagePtr = TCntPtr<TBasePage>;
        using TPageList = TVector<TPagePtr>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTopic() = delete;

        TTopic
        (
            const   TString&                strTitle
            , const TString&                strParTopicPath
            , const TString&                strTopicName
            , const TString&                strSrcPath
        );

        TTopic(const TTopic&) = delete;

        ~TTopic();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsThisName(const TString& strDirName) const
        {
            return strDirName.bCompareI(m_strTopicName);
        }

        tCIDLib::TVoid Parse
        (
                    TParseCtx&              ctxToUse
        );

        TPagePtr cptrFindPage
        (
            const   TString&                strFileName
        )   const;

        TTopicPtr cptrFindTopic
        (
            const   TString&                strDirName
        )   const;

        TPagePtr cptrTopicPage() const;

        tCIDLib::TVoid GenerateOutput
        (
            const   TString&                strOutPath
            , const TTopicPtr&              cptrParent
            , const TTopicPtr&              cptrUs
        )   const;

        tCIDLib::TVoid GenerateTopicLink
        (
                    TTextOutStream&         strmOut
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenerateTopicIndex
        (
                    TTextOutStream&         strmOutFl
            , const TTopicPtr&              cptrParent
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colPages
        //      The pages that are defined at this topic level.
        //
        //  m_colSubTopics
        //      These are the recursive child topics under this one. We parse this from
        //      the topic file.
        //
        //  m_pathSource
        //      The file system path to our topic directory. This is set on us before
        //      parsing obviously.
        //
        //  m_strParTopicPath
        //      This is our parent topic path. We keep this separately because the
        //      javascript stuff works in terms of paths and pages, so every time we
        //      need to generate an HTML link for our topic, we'd have to break this
        //      out anyway.
        //
        //  m_strTitle
        //      The title of this topic, used in the topic list on the left. This is
        //      set on us before we are parsed (from the parent topic's child topic
        //      list.)
        //
        //  m_strTopicName
        //      Our topic name, i.e. the unique name under our parent topic. It will
        //      also be the last part of m_strTopicPath which is our full path.
        //
        //  m_strTopicPath
        //      The full topic path for this topic. This is set on us before parsing.
        //
        //  m_strTopicPage
        //      The name (just the base name) of the .cidhpage file that is the main page
        //      for this topic. This one is put at the top. If this is empty, we just take
        //      the 0th file in the page list. We parse this from the topic file.
        // -------------------------------------------------------------------
        TPageList       m_colPages;
        TSubTopicList   m_colSubTopics;
        TPathStr        m_pathSource;
        TString         m_strParTopicPath;
        TString         m_strTitle;
        TString         m_strTopicName;
        TString         m_strTopicPath;
        TString         m_strTopicPage;
};
