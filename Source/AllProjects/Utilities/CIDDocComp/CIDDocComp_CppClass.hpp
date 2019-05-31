//
// FILE NAME: CIDDocComp_CppClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/30/2019
//
// COPYRIGHT: 2015..2015, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This is a specialized page derivative for C++ classes. It's a fairly complex
//  bunch of data, but the biggest parts are just replications of the same info
//  for public, protected, and private content. For each of those visibility
//  scopes we need nested enums, nested fundamental types, aliases, data members,
//  and methods.
//
//  For methods we don't have separate sections for static, virtual, non-virtual
//  and so forth. Those are just attributes of the methods. When it's type do do
//  the output, we find the methods of the various types as required.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TCppClassPage
//  PREFIX: pg
// ---------------------------------------------------------------------------
class TCppClassPage : public TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCppClassPage() = delete;

        TCppClassPage
        (
            const   TString&                strTitle
            , const TString&                strParSrcDir
            , const TString&                strParTopic
            , const TString&                strFileName
        );

        ~TCppClassPage() = default;


    private :
        // -------------------------------------------------------------------
        //  Private, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParse
        (
                    TTopic&                     topicParent
            , const TXMLTreeElement&            xtnodeRoot
        )   override;

        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const override;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDuplicable
        //  m_bFormattable
        //  m_bStreamable
        //      These are flags that indicate if this class supports any of the
        //      big three mixin interfaces.
        //
        //  m_strClass
        //  m_strParClass
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDuplicable;
        tCIDLib::TBoolean   m_bFormattable;
        tCIDLib::TBoolean   m_bStreamable;
        TString             m_strClass;
        TString             m_strParClass;
};
