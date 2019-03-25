//
// FILE NAME: CIDXML_ContextStack.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
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
//  This is the header file for the CIDXML_ContextStack.Cpp file, which
//  implements the TXMLContextStack class. This class provides the mechanism
//  by which the parser core keeps up with what the current element is,
//  what children all the elements along the current element hierarchy have,
//  and what the namespace mappings are along the element hierarchy.
//
//  Basically, instead of keeping this context information in the call stack,
//  which would be difficult because of the need to search back up it for
//  namespace mapping info, its kept in an auxillary stack. This allows the
//  top level content parsing to be all flat.
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
//  CLASS: TXMLContextStack
// PREFIX: xcs
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLContextStack : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLContextStack();

        TXMLContextStack(const TXMLContextStack&) = delete;

        ~TXMLContextStack();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLContextStack& operator=(const TXMLContextStack&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddAsChild
        (
            const   tCIDLib::TCard4         c4ChildId
        );

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bPopLevel();

        const tCIDLib::TCard4* pc4GetTopContent
        (
                    tCIDLib::TCard4&        c4Count
        );

        tCIDXML::EElemTextTypes eTextType() const;

        const TXMLElemDecl& xdeclTopElem
        (
                    tCIDLib::TCard4&        c4SpoolerId
        )   const;

        tCIDLib::TVoid PushNewLevel
        (
                    TXMLElemDecl* const     pxdeclNew
            , const tCIDLib::TCard4         c4SpoolerId
        );

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Class specific data types
        //
        //  TLevel
        //      This is the main structure and it represents a new level in
        //      the element hierarchy. It represents the element at that
        //      level, and holds info about the children of that element and
        //      the namespace map info for that element.
        //
        //      The element decl is not owned by us, we just reference it.
        //      Its owned by the element pool in the validator.
        //
        //      There is an expandable list of ids for the children of this
        //      element. It is given an initial allocation, but it will be
        //      grown as required.
        //
        //      There is also an expandable list for namespace mapping. At
        //      eat level, an element's xmlns attributes can add namespace
        //      mappings. It works like the child array, but its array holds
        //      a small structure that represents a prefix to URI mapping.
        // -------------------------------------------------------------------
        struct TNSMapItem
        {
            tCIDLib::TCard4     c4URIId;
            tCIDLib::TCard4     c4PrefixId;
        };

        struct TLevel
        {
            TXMLElemDecl*       pxdeclLevel;
            tCIDLib::TCard4     c4SpoolerId;

            tCIDLib::TCard4*    pc4ChildIds;
            tCIDLib::TCard4     c4ChildCount;
            tCIDLib::TCard4     c4ChildMax;

            TNSMapItem*         pMapItems;
            tCIDLib::TCard4     c4MapCount;
            tCIDLib::TCard4     c4MapMax;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CleanUp();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4LevelTop
        //      The current count of levels in the m_apLevels array. This is
        //      how many of the entries are used, not the current max it
        //      can hold. Its treated like a stack top, since that's the
        //      semantics of this class.
        //
        //  m_c4LevelMax
        //      This is the current allocation size of the m_apLevels array.
        //      When (m_c4LevelCount == m_c4LevelMax) its time to expand it.
        //
        //  m_eTextType
        //      This is kept up to date to represent the ability of the
        //      current top of stack element to accept text content.
        //
        //  m_pLevels
        //      This is the expandable array of level structures. This grows
        //      to the maximum nesting depth of the source data. Its given
        //      a small initial allocation which will serve 90% of documents.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4LevelTop;
        tCIDLib::TCard4         m_c4LevelMax;
        tCIDXML::EElemTextTypes m_eTextType;
        TLevel*                 m_pLevels;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLContextStack,TObject)
};

#pragma CIDLIB_POPPACK

