//
// FILE NAME: CIDDocComp_HelpNode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/04/2015
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
//  Any place that we allow our general markup content, we use the THelpNode class. This
//  is a recursive containment class, that holds the XML nodes that we parse out of the
//  file. We use this stuff to generate HTML content.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: THelpNode
//  PREFIX: hn
// ---------------------------------------------------------------------------
class THelpNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THelpNode();

        THelpNode(const THelpNode&) = default;

        ~THelpNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THelpNode& operator=(const THelpNode&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeText
        );

        tCIDLib::TVoid ParseFromParent
        (
            const   TXMLTreeElement&        xtnodePar
            , const TString&                strName
            , const tCIDLib::TBoolean       bOptional
        );

        tCIDLib::TVoid OutputNodes
        (
                    TTextOutStream&         strmTar
        )   const;

        tCIDLib::TVoid SetToText
        (
            const   tCIDLib::TCh* const     pszToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ProcessTableRow
        (
            const   TXMLTreeElement&        xtnodeRow
        );

        tCIDLib::TVoid ProcessTable
        (
            const   TXMLTreeElement&        xtnodeTbl
        );

        tCIDLib::TVoid OutputTableRow
        (
                    TTextOutStream&         strmTar
        )   const;

        tCIDLib::TVoid OutputTable
        (
                    TTextOutStream&         strmTar
        )   const;


        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_c4Extra
        //      Just an extra value for general use in a node type specific way.
        //
        //  m_colNodes
        //      The nodes that make up our help text. It's a recursive structure so
        //      we have a list of ourself.
        //
        //  m_eType
        //      The type of node we are.
        //
        //  m_strID
        //  m_strRef
        //      Optional bits used for things like divs, spans, links, etc...
        //
        //  m_strText
        //      Holds the actual text content of a given node, where applicable.
        //
        //  m_strType
        //      Option info that is used per-node, to indicate variations that that
        //      type of node can support.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Extra;
        TVector<THelpNode>      m_colNodes;
        tCIDDocComp::EMUTypes   m_eType;
        TString                 m_strRef;
        TString                 m_strID;
        TString                 m_strText;
        TString                 m_strType;
};
