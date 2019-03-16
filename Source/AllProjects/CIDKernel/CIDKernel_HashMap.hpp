//
// FILE NAME: CIDKernel_HashMap.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_HashMap.Cpp file. This file
//  implements a very simple hash map for use within the kernel. Its used
//  in a couple of places like storage of the environment values and such.
//
//  Its very simplistic and assumes that its key and data are both raw strings.
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
//   CLASS: TKrnlHashMap
//  PREFIX: khshm
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlHashMap
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlHashMap() = delete;

        TKrnlHashMap(const TKrnlHashMap&) = delete;

        TKrnlHashMap
        (
            const   tCIDLib::TCard4         c4Modulus
        );

        ~TKrnlHashMap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlHashMap& operator=(const TKrnlHashMap&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAdd
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszValue
        );

        tCIDLib::TBoolean bCurKey
        (
            const   tCIDLib::TCh*&          pszToFill
        )   const;

        tCIDLib::TBoolean bCurKeyAndValue
        (
            const   tCIDLib::TCh*&          pszKeyToFill
            , const tCIDLib::TCh*&          pszValToFill
        )   const;

        tCIDLib::TBoolean bCurValue
        (
            const   tCIDLib::TCh*&          pszToFill
        )   const;

        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bRemoveKey
        (
            const   tCIDLib::TCh* const     pszKey
        );

        tCIDLib::TBoolean bResetIter();

        tCIDLib::TBoolean bUpdateKey
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszNewValue
        );

        tCIDLib::TCard4 c4ElemCount() const;

        const tCIDLib::TCh* pszValueForKey
        (
            const   tCIDLib::TCh* const     pszKey
        );

        tCIDLib::TVoid RemoveAll();


    private :
        // -------------------------------------------------------------------
        //  Private, nested types
        // -------------------------------------------------------------------
        struct TNode
        {
            tCIDLib::TCh*   pszKey;
            tCIDLib::TCh*   pszValue;

            TNode*          pnodeNext;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TNode* pnodeFind
        (
            const  tCIDLib::TCh* const      pszKeyToFind
            ,       tCIDLib::THashVal&      hshElem
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apnodeTable
        //      This is list of node headers, each one of which contains
        //      the nodes which hashed to that value.
        //
        //  m_c4CurList
        //      This is the current list that the iterator is pointing to.
        //
        //  m_c4ElemCount
        //      The number of elements in the map.
        //
        //  m_c4Modulus
        //      THis is the modulus divisor for the hashing of keys. There
        //      are this many entries in the m_apnodeTable array.
        //
        //  m_pnodeCur
        //      This is the current node with the current list that the
        //      iterator is pointing to.
        // -------------------------------------------------------------------
        TNode**             m_apnodeTable;
        tCIDLib::TCard4     m_c4CurList;
        tCIDLib::TCard4     m_c4ElemCount;
        tCIDLib::TCard4     m_c4Modulus;
        TNode*              m_pnodeCur;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TKrnlHashMap::c4ElemCount() const
{
    return m_c4ElemCount;
}
