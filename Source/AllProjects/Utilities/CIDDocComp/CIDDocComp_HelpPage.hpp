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
//  This file defines a derivativ the base page class. In this case it's for the
//  most common type which is a non-specialized, general help page that contains
//  content defined by the HelpText element of the DTD. So basically just general
//  markup. All we need is a THelpNode member, which we let parse itself out of
//  our provided XML content.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: THelpPage
//  PREFIX: pg
// ---------------------------------------------------------------------------
class THelpPage : public TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        THelpPage() = delete;

        THelpPage
        (
            const   TString&                strTitle
            , const TString&                strParSrcDir
            , const TString&                strParTopic
            , const TString&                strFileName
            , const TString&                strFileExt
            , const tCIDLib::TBoolean       bVirtual
        );

        ~THelpPage();


    private :
        // -------------------------------------------------------------------
        //  private, inherited methods
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
        //  m_hnContent
        //      For these types of pages, it's all just general help text markup.
        // -------------------------------------------------------------------
        THelpNode   m_hnContent;
};
