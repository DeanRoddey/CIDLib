//
// FILE NAME: CIDMath_ValueMap.hpp
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
//  This header provides a simple class for mapping one range of index values
//  to another. You put in one index and it returns the mapped index. This is
//  an often done operation, and we make it flexible by doing it as an abstract
//  base class from which a number of canned mapping systems can be derived and
//  used polymorphically. This lets a framework plug in different mapping
//  schemes dynamically.
//
//  CIDMath_ValueMap2.hpp provides a couple of canned mappers that are of
//  general purpose use.
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
//  CLASS: TValueMap
// PREFIX: vmap
// ---------------------------------------------------------------------------
class CIDMATHEXP TValueMap :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TValueMap();

        TValueMap
        (
            const   tCIDLib::TCard4         c4MaxIndex1
            , const tCIDLib::TCard4         c4MaxIndex2
        );

        TValueMap(const TValueMap&) = default;

        ~TValueMap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TValueMap& operator=(const TValueMap&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TValueMap&              vmapToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TValueMap&              vmapToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4MapIndex
        (
            const   tCIDLib::TCard4         c4Index1
        )   const = 0;

        virtual tCIDLib::TVoid SetNewRanges
        (
            const   tCIDLib::TCard4         c4MaxIndex1
            , const tCIDLib::TCard4         c4MaxIndex2
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MaxIndex1() const;

        tCIDLib::TCard4 c4MaxIndex2() const;


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
        //  Private data members
        //
        //  m_c4MaxIndex1
        //  m_c4MaxIndex2
        //      The define the two index ranges we map between.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4MaxIndex1;
        tCIDLib::TCard4 m_c4MaxIndex2;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TValueMap,TObject)
};

#pragma CIDLIB_POPPACK


