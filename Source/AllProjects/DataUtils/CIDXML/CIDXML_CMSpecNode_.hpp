//
// FILE NAME: CIDXML_CMSpecNode_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/20/1999
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
//  This file defines the TXMLCMSpecNode class, which is a little tree node
//  class used internally within the TXMLChildModel class to store its
//  representation of the regular expression tree of its content model.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLCMSpecNode
// PREFIX: xcsn
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLCMSpecNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLCMSpecNode() = delete;

        TXMLCMSpecNode( const   tCIDXML::ECMNodeTypes   eNodeType
                        ,       TXMLCMSpecNode* const   pxcsnLeft = nullptr
                        ,       TXMLCMSpecNode* const   pxcsnRight = nullptr) :

            m_c4ElemId(kCIDXML::c4InvalidElemId)
            , m_eNodeType(eNodeType)
            , m_pxcsnLeft(pxcsnLeft)
            , m_pxcsnRight(pxcsnRight)
        {
        }

        TXMLCMSpecNode(const tCIDLib::TCard4 c4ElemLeafId) :

            m_c4ElemId(c4ElemLeafId)
            , m_eNodeType(tCIDXML::ECMNodeTypes::Leaf)
            , m_pxcsnLeft(nullptr)
            , m_pxcsnRight(nullptr)
        {
        }

        TXMLCMSpecNode(const TXMLCMSpecNode&) = delete;
        TXMLCMSpecNode(TXMLCMSpecNode&&) = delete;

        virtual ~TXMLCMSpecNode()
        {
            // Clean up our children, which clean up their children, etc...
            delete m_pxcsnLeft;
            delete m_pxcsnRight;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLCMSpecNode& operator=(const TXMLCMSpecNode&) = delete;
        TXMLCMSpecNode& operator=(TXMLCMSpecNode&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ElemId() const
        {
            return m_c4ElemId;
        }

        tCIDLib::TCard4 c4ElemId(const tCIDLib::TCard4 c4ElemId)
        {
            m_c4ElemId = c4ElemId;
            return m_c4ElemId;
        }

        tCIDXML::ECMNodeTypes eNodeType() const
        {
            return m_eNodeType;
        }

        TXMLCMSpecNode* pxcsnLeft()
        {
            return m_pxcsnLeft;
        }

        TXMLCMSpecNode* pxcsnLeft(TXMLCMSpecNode* const   pxcsnToSet)
        {
            m_pxcsnLeft = pxcsnToSet;
            return m_pxcsnLeft;
        }

        const TXMLCMSpecNode* pxcsnLeft() const
        {
            return m_pxcsnLeft;
        }

        TXMLCMSpecNode* pxcsnRight()
        {
            return m_pxcsnRight;
        }

        TXMLCMSpecNode* pxcsnRight(TXMLCMSpecNode* const pxcsnToSet)
        {
            m_pxcsnRight = pxcsnToSet;
            return m_pxcsnRight;
        }

        const TXMLCMSpecNode* pxcsnRight() const
        {
            return m_pxcsnRight;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ElemId
        //      This is the element pool id of the element, if this is a leaf
        //      node. Else its value means nothing.
        //
        //  m_eNodeType
        //      This is the type of this node. Its passed in by the derived
        //      class when they call our constructor.
        //
        //  m_pxcsnLeft
        //  m_pxcsRight
        //      These are the optional left and right child nodes.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ElemId;
        tCIDXML::ECMNodeTypes   m_eNodeType;
        TXMLCMSpecNode*         m_pxcsnLeft;
        TXMLCMSpecNode*         m_pxcsnRight;
};

#pragma CIDLIB_POPPACK


