//
// FILE NAME: CIDMath_ValueMap2.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header provides a couple of small derivatives of the abstract base
//  class TValueMap. TLogMap does a logarithmic mapping, TModuloMap does a
//  modulo division mapping.
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
//  CLASS: TLogMap
// PREFIX: vmap
// ---------------------------------------------------------------------------
class CIDMATHEXP TLogMap : public TValueMap
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLogMap();

        TLogMap
        (
            const   tCIDLib::TCard4         c4MaxIndex1
            , const tCIDLib::TCard4         c4MaxIndex2
            , const tCIDLib::ELogMapModes   eMode = tCIDLib::ELogMapModes::Spread
        );

        TLogMap
        (
            const   TLogMap&                vmapSrc
        );

        ~TLogMap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLogMap& operator=
        (
            const   TLogMap&                vmapSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TLogMap&                vmapSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLogMap&                vmapSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MapIndex
        (
            const   tCIDLib::TCard4         c4Index1
        )   const;

        tCIDLib::TVoid SetNewRanges
        (
            const   tCIDLib::TCard4         c4MaxIndex1
            , const tCIDLib::TCard4         c4MaxIndex2
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AllocTable();

        tCIDLib::TCard4 c4ElemAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid InitTable();

        tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4Val
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1LogTable
        //      A pointer to the dynamically allocated mapping table. Its
        //      size depends upon the 1st max index and the size it takes to
        //      store a value of the second index. The m_c4ElemAt() method
        //      handles internally getting values out, given the m_c4ElemSize
        //      value.
        //
        //  m_eMode
        //      This is the log mapping mode. There are two possibilities.
        //      One spreads out the lower values more smoothly than the raw
        //      mapping algorithm creates.
        //
        //  m_c4ElemSize
        //      This is the number of bytes per index element in the array.
        //      We use the smallest we can get away with.
        // -------------------------------------------------------------------
        tCIDLib::TCard1*        m_ac1LogTable;
        tCIDLib::TCard4         m_c4ElemSize;
        tCIDLib::ELogMapModes   m_eMode;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLogMap,TValueMap)
        DefPolyDup(TLogMap)
};


// ---------------------------------------------------------------------------
//  CLASS: TModMap
// PREFIX: vmap
// ---------------------------------------------------------------------------
class CIDMATHEXP TModMap : public TValueMap
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TModMap();

        TModMap
        (
            const   tCIDLib::TCard4         c4MaxIndex1
            , const tCIDLib::TCard4         c4MaxIndex2
        );

        TModMap(const TModMap&) = default;

        ~TModMap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TModMap& operator=(const TModMap&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TModMap&                vmapSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TModMap&                vmapSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MapIndex
        (
            const   tCIDLib::TCard4         c4ToMap
        )   const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TModMap,TValueMap)
        DefPolyDup(TModMap)
};

#pragma CIDLIB_POPPACK


