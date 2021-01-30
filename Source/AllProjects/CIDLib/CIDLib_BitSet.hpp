//
// FILE NAME: CIDLib_BitSet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1999
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
//  This is the header for the CIDLib_BitSet.Cpp file, which implements a bit set
//  class which lets client code have a set of bits it can easily access by index
//  and manipulate in various ways.
//
//  We have a base class that handles most of the functionality. Then we have a
//  standard TBitset class that implements that interface with a TCard4 as the
//  index type. Originally this was the only class, but we re-factored to allow
//  for either TCard4 or a templatized version that can use enums as the index.
//
//  The base class allows the vast bulk of the code to remain out of line so the
//  templatized ones don't add much bloat.
//
//  Bitsets must be the same bit size in order to compare, or do boolean operations
//  on them.
//
// CAVEATS/GOTCHAS:
//
//  1)  So that we can do simple byte comparisons for efficiency, we have to be sure
//      we keep any trailing last byte clear of extraneous bits! If we are set from
//      a raw array it could have bits set in the last byte that are beyond our
//      bit count. So we have to always AND that last byte. We pre-calculate what
//      that mask would be and keep it around.
//
//  2)  Because we refactored this and what is now the base class is basically what
//      the old TBitSet class was, we can't have any persistence info in the TBitSet
//      class now. There would no way for it to know if it is reading in one of the
//      old ones or a new one.
//
//      The templatized version can have its own stuff , though if any old code before
//      we refactored wants to use it to replace previously persisted ones, they will
//      have to handle that changeover by reading in an old one and writing out a
//      new templatized one.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBitsetBase
//  PREFIX: bts
// ---------------------------------------------------------------------------
class CIDLIBEXP TBitsetBase : public TObject, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TBitsetBase();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ANDRawArray
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4ByteCunt
        );

        tCIDLib::TBoolean bAllCleared() const;

        tCIDLib::TBoolean bBitState
        (
            const   tCIDLib::TCard4         c4BitIndex
        )   const;

        tCIDLib::TCard4 c4BitCount() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FromRawArray
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4ByteCount
        )   const;

        tCIDLib::TVoid ORRawArray
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4ByteCount
        );

        tCIDLib::TVoid SetAllBits();

        tCIDLib::TVoid ToRawArray
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4ByteCount
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TBitsetBase();

        TBitsetBase
        (
            const   tCIDLib::TCard4         c4BitCount
        );

        TBitsetBase
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::TCard1* const  pc1InitVals
        );

        TBitsetBase
        (
            const   TBitsetBase&            btsSrc
        );

        TBitsetBase
        (
                    TBitsetBase&&           btsSrc
        );

        TBitsetBase& operator=
        (
            const   TBitsetBase&            btsSrc
        );

        TBitsetBase& operator=
        (
                    TBitsetBase&&           btsSrc
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ANDWith
        (
            const   TBitsetBase&            btsSrc
        );

        tCIDLib::TBoolean bCompareTo
        (
            const   TBitsetBase&            btsSrc
        )   const;

        tCIDLib::TBoolean bSetABit
        (
            const   tCIDLib::TCard4         c4BitIndex
            , const tCIDLib::TBoolean       bNewState
        )   const;

        tCIDLib::TBoolean bTestABit
        (
            const   tCIDLib::TCard4         c4BitIndex
        )   const;

        tCIDLib::TVoid CheckIndex
        (
            const   tCIDLib::TCard4         c4ToCheck
        )   const;

        tCIDLib::TVoid CheckReady() const;

        tCIDLib::TVoid CopyFrom
        (
            const   TBitsetBase&            btsSrc
        );

        tCIDLib::TVoid ORWith
        (
            const   TBitsetBase&            btsSrc
        );

        tCIDLib::TVoid ResetBitCount
        (
            const   tCIDLib::TCard4         c4NewCount
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CalcTrailingMask();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c1TrailingMask
        //      See gotchas above. We have to make sure no extraneous bits get into
        //      the final byte. We pre-set this when our bit count is set.
        //
        //  m_c4Bits
        //      This is the number of bits in the set.
        //
        //  m_c4Bytes
        //      When the set size is set, we calculate how many bytes that is. This
        //      number is used in a lot of ops, so its worth just precalculating it
        //      when the set size changes and keeping it saround.
        //
        //  m_pc1Bits
        //      This is an array of bytes in which the bits are stored.
        // -------------------------------------------------------------------
        tCIDLib::TCard1     m_c1TrailingMask;
        tCIDLib::TCard4     m_c4Bits;
        tCIDLib::TCard4     m_c4Bytes;
        tCIDLib::TCard1*    m_pc1Bits;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitsetBase,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TBitset
//  PREFIX: bts
// ---------------------------------------------------------------------------
class CIDLIBEXP TBitset : public TBitsetBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBitset();

        TBitset
        (
            const   tCIDLib::TCard4         c4SetSize
        );

        TBitset
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::TCard1* const  pc1InitVals
        );

        TBitset
        (
            const   TBitset&                btsSrc
        );

        TBitset
        (
                    TBitset&&               btsSrc
        );

        ~TBitset();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitset& operator=
        (
            const   TBitset&                btsSrc
        );

        TBitset& operator=
        (
                    TBitset&&               btsSrc
        );

        tCIDLib::TVoid operator&=
        (
            const   TBitset&                btsSrc
        );

        tCIDLib::TVoid operator|=
        (
            const   TBitset&                btsSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TBitset&                btsSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TBitset&                btsSrc
        )   const;

        tCIDLib::TBoolean operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBitState
        (
            const   tCIDLib::TCard4         c4BitIndex
        )   const;

        tCIDLib::TBoolean bSetBitState
        (
            const   tCIDLib::TCard4         c4BitIndex
            , const tCIDLib::TBoolean       bNewValue
        )   const;

        tCIDLib::TVoid ChangeBitCount
        (
            const   tCIDLib::TCard4         c4NewSize
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitset,TBitsetBase)
};



// ---------------------------------------------------------------------------
//   CLASS: TEnumBitset
//  PREFIX: bts
// ---------------------------------------------------------------------------
template <typename T, const T tSize> class TEnumBitset : public TBitsetBase
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TMyType = TEnumBitset<T,tSize>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEnumBitset() :

            TBitsetBase(tCIDLib::TCard4(tSize))
            , m_tBitCount(tSize)
        {
        }

        TEnumBitset(const tCIDLib::TCard1* const  pc1InitVals) :

            TBitsetBase(tCIDLib::TCard4(tSize), pc1InitVals)
            , m_tBitCount(tSize)
        {
        }

        TEnumBitset(const TMyType& btsSrc) :

            TBitsetBase(btsSrc)
            , m_tBitCount(btsSrc.m_tBitCount)
        {
        }

        TEnumBitset(TMyType&& btsSrc) :

            TBitsetBase()
        {
            *this = tCIDLib::ForceMove(btsSrc);
        }

        ~TEnumBitset()
        {
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEnumBitset& operator=(const TMyType& btsSrc)
        {
            if (&btsSrc != this)
                CopyFrom(btsSrc);
            return *this;
        }

        TEnumBitset& operator=(TMyType&& btsSrc)
        {
            if (&btsSrc != this)
            {
                TParent::operator=(tCIDLib::ForceMove(btsSrc));
                tCIDLib::Swap(m_tBitCount, btsSrc.m_tBitCount);
            }
            return *this;
        }

        tCIDLib::TVoid operator&=(const TMyType& btsSrc)
        {
            if (&btsSrc != this)
                ANDWith(btsSrc);
        }

        tCIDLib::TVoid operator|=(const TMyType& btsSrc)
        {
            if (&btsSrc != this)
                ORWith(btsSrc);
        }

        tCIDLib::TBoolean operator==(const TMyType& btsSrc) const
        {
            return bCompareTo(btsSrc);
        }

        tCIDLib::TBoolean operator!=(const TMyType& btsSrc) const
        {
            return !operator==(btsSrc);
        }

        tCIDLib::TBoolean operator[](const T tIndex) const
        {
            return bTestABit(tCIDLib::TCard4(tIndex));
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBitState(const T tIndex) const
        {
            return bTestABit(tCIDLib::TCard4(tIndex));
        }

        tCIDLib::TBoolean
        bSetBitState(const T tIndex, const tCIDLib::TBoolean bNewState) const
        {
            return bSetABit(tCIDLib::TCard4(tIndex), bNewState);
        }

        T eBitCount() const
        {
            return m_tBitCount;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_tBitCount
        //      We save our own enum format of the size so that we can return it.
        //      Else we'd not be able to gen it up just from the type.
        // -------------------------------------------------------------------
        T   m_tBitCount;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TMyType,TBitsetBase)
};

#pragma CIDLIB_POPPACK

