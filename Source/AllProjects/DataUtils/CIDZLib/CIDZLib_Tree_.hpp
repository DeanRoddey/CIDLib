//
// FILE NAME: CIDZLib_Tree.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/18/2004
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
//  This is the header file for the CIDZLib_Tree_.hpp file, which implements
//  the internal Huffman tree stuff used by the ZLib algorithm.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TStaticZTreeDescr;

// ---------------------------------------------------------------------------
//  CLASS: TZTree
// PREFIX: tree
// ---------------------------------------------------------------------------
class TZTree
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TZTree(const tCIDLib::TCard4 c4Size) :

            m_ac2D1(new tCIDLib::TCard2[c4Size])
            , m_ac2D2(new tCIDLib::TCard2[c4Size])
            , m_c4Size(c4Size)
        {
            Reset();
        }

        TZTree
        (
            const   tCIDLib::TCard4             c4Size
            , const tCIDZLib_::TTreeVals* const patvInit
        );

        TZTree(const TZTree&) = delete;

        ~TZTree()
        {
            delete [] m_ac2D1;
            delete [] m_ac2D2;
        }

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BumpFreq(const tCIDLib::TCard4 c4At)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D1[c4At]++;
        }

        tCIDLib::TCard2 c2Code(const tCIDLib::TCard4 c4At) const
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            return m_ac2D1[c4At];
        }

        tCIDLib::TCard2 c2Code( const   tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard2 c2ToSet)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D1[c4At] = c2ToSet;
            return c2ToSet;
        }

        tCIDLib::TCard2 c2Dad(const tCIDLib::TCard4 c4At) const
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            return m_ac2D2[c4At];
        }

        tCIDLib::TCard2 c2Dad( const    tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard2 c2ToSet)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D2[c4At] = c2ToSet;
            return c2ToSet;
        }

        tCIDLib::TCard2 c2Freq(const tCIDLib::TCard4 c4At) const
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            return m_ac2D1[c4At];
        }

        tCIDLib::TCard2 c2Freq( const   tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard2 c2ToSet)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D1[c4At] = c2ToSet;
            return c2ToSet;
        }

        tCIDLib::TCard2 c2Len(const tCIDLib::TCard4 c4At) const
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            return m_ac2D2[c4At];
        }

        tCIDLib::TCard2 c2Len(  const   tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard2 c2ToSet)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D2[c4At] = c2ToSet;
            return c2ToSet;
        }

        tCIDLib::TCard4 c4Build
        (
            const   TStaticZTreeDescr&      stdStatic
            ,       tCIDLib::TCard4&        c4OptLen
            ,       tCIDLib::TCard4&        c4StaticLen
        );

        tCIDLib::TCard4 c4CalcLength();

        tCIDLib::TCard4 c4Size() const;

        tCIDLib::TVoid IncFreq( const   tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard2 c2ToAdd)
        {
            #if CID_DEBUG_ON
            CheckIndex(c4At);
            #endif
            m_ac2D1[c4At] += c2ToAdd;
        }

        tCIDLib::TVoid Reset();



    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------

        tCIDLib::TVoid operator=(const TZTree&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIndex
        (
            const   tCIDLib::TCard4 c4ToCheck
        )   const;

        tCIDLib::TVoid GenBitLengths
        (
            const   TStaticZTreeDescr&      stdStatic
            ,       tCIDLib::TCard4&        c4OptLen
            ,       tCIDLib::TCard4&        c4StaticLen
            , const tCIDLib::TCard4* const  ac4Heap
            , const tCIDLib::TCard4         c4HeapCnt
            ,       tCIDLib::TCard4* const  ac4BitLens
            , const tCIDLib::TCard4         c4LenCount
            , const tCIDLib::TCard4         c4Largest
        );

        tCIDLib::TVoid GenCodes
        (
            const   tCIDLib::TCard4* const  ac4BitLens
            , const tCIDLib::TCard4         c4LenCount
            , const tCIDLib::TCard4         c4Largest
        );

        tCIDLib::TVoid ProcessHeap
        (
            const   tCIDLib::TCard4         c4Start
            ,       tCIDLib::TCard4* const  ac4Heap
            , const tCIDLib::TCard1* const  ac1Depth
            , const tCIDLib::TCard4         c4HeapCnt
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac2D1
        //  m_ac2D2
        //      These arrays provide the storage for the tree info.
        //      We use indexes instead of link pointers to create the
        //      tree structure.
        //
        //  m_c4Size
        //      The number of slots available for this tree, set in the
        //      ctor.
        // -------------------------------------------------------------------
        tCIDLib::TCard2*    m_ac2D1;
        tCIDLib::TCard2*    m_ac2D2;
        tCIDLib::TCard4     m_c4Size;
};


// ---------------------------------------------------------------------------
//  CLASS: TStaticZTreeDescr
// PREFIX: std
// ---------------------------------------------------------------------------
class TStaticZTreeDescr
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStaticZTreeDescr
        (
            const   tCIDLib::TCard4             c4MaxBitLen
            , const tCIDLib::TCard4             c4ExtraBase
            , const tCIDLib::TCard1* const      pc1ExtraBits
            , const tCIDLib::TCard4             c4ExtraBits
            , const tCIDZLib_::TTreeVals* const ptvInit
            , const tCIDLib::TCard4             c4InitCount
            , const tCIDLib::TCard4             c4MaxElems
        );

        TStaticZTreeDescr
        (
            const   tCIDLib::TCard4             c4MaxBitLen
            , const tCIDLib::TCard4             c4ExtraBase
            , const tCIDLib::TCard1* const      pc1ExtraBits
            , const tCIDLib::TCard4             c4ExtraBits
            , const tCIDLib::TCard4             c4MaxElems
        );

        TStaticZTreeDescr(const TStaticZTreeDescr&) = delete;

        ~TStaticZTreeDescr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStaticZTreeDescr& operator=(const TStaticZTreeDescr&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1 c1ExtraBitsAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4ExtraBase() const
        {
            return m_c4ExtraBase;
        }

        tCIDLib::TCard4 c4MaxBitLen() const
        {
            return m_c4MaxBitLen;
        }

        tCIDLib::TCard4 c4MaxElems() const
        {
            return m_c4MaxElems;
        }

        const TZTree* ptreeData() const
        {
            return m_ptreeData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ExtraBits
        //      The number of items in the extra bits array.
        //
        //  m_c4ExtraBase
        //      Base index for extra bits.
        //
        //  m_c4MaxBitLen
        //      The maximum bit length for the codes in this static
        //      tree.
        //
        //  m_c4MaxElems
        //      The maximum elements for the tree. This is set even if there
        //      is no actual static tree.
        //
        //  m_pc1ExtraBits
        //      Any extra bits. Can be null if none are needed.
        //
        //  m_ptreeData
        //      The tree data for this particular tree description. It may
        //      not be used in some cases, so it is a pointer.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ExtraBits;
        tCIDLib::TCard4     m_c4ExtraBase;
        tCIDLib::TCard4     m_c4MaxBitLen;
        tCIDLib::TCard4     m_c4MaxElems;
        tCIDLib::TCard1*    m_pc1ExtraBits;
        TZTree*             m_ptreeData;
};



// ---------------------------------------------------------------------------
//  CLASS: TZTreeDescr
// PREFIX: td
// ---------------------------------------------------------------------------
class TZTreeDescr
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TZTreeDescr(const   tCIDLib::TCard4             c4Size
                    , const TStaticZTreeDescr* const    pstdStatic) :

            m_c4MaxCode(0)
            , m_pstdStatic(pstdStatic)
            , m_treeData(c4Size)
        {
            Reset();
        }

        TZTreeDescr(const TZTreeDescr&) = delete;

        ~TZTreeDescr()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TZTreeDescr& operator=(const TZTreeDescr&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Build
        (
                    tCIDLib::TCard4&         c4OptLen
            ,       tCIDLib::TCard4&         c4StaticLen
        );

        tCIDLib::TCard4 c4MaxCode() const
        {
            return m_c4MaxCode;
        }

        const TStaticZTreeDescr& stdStatic() const
        {
            return *m_pstdStatic;
        }

        tCIDLib::TVoid Reset()
        {
            m_c4MaxCode = 0;
            m_treeData.Reset();
        }

        const TZTree& treeData() const
        {
            return m_treeData;
        }

        TZTree& treeData()
        {
            return m_treeData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxCode
        //      The largest code with a non-zero frequency.
        //
        //  m_stdStatic
        //      The tree description object for the equiv static tree.
        //
        //  m_treeData
        //      The tree data for this particular tree description.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4MaxCode;
        const TStaticZTreeDescr*    m_pstdStatic;
        TZTree                      m_treeData;
};


#pragma CIDLIB_POPPACK

