//
// FILE NAME: CIDKernel_BaseInlines.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This file contains some very fundamental inline and templatized global
//  functions that are used all over the place.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace tCIDLib
{
    // Used in various metaprogramming templates below
    struct FalseType { static constexpr tCIDLib::TBoolean bState = false; };
    struct TrueType { static constexpr tCIDLib::TBoolean bState = true; };


    //
    //  Some helper sfor dealing with safe enums and the problems with converting them
    //  to numeric values.
    //
    template <typename T> constexpr tCIDLib::TCard4 c4EnumOrd(const T tVal)
    {
        return tCIDLib::TCard4(tVal);
    }

    template <typename T>
    constexpr tCIDLib::TCard4 c4EnumOrd(const T tVal, const tCIDLib::TCard4 c4Plus)
    {
        return tCIDLib::TCard4(tVal) + c4Plus;
    }

    template <typename T> constexpr tCIDLib::TInt4 i4EnumOrd(const T tVal)
    {
        return tCIDLib::TInt4(tVal);
    }

    template <typename T>
    constexpr tCIDLib::TInt4 i4EnumOrd(const T tVal, const tCIDLib::TInt4 i4Plus)
    {
        return tCIDLib::TInt4(tVal) + i4Plus;
    }


    // Some testing for types
    template <typename T> struct IsTCardX : FalseType {};
    template <> struct IsTCardX<tCIDLib::TCard1> : TrueType {};
    template <> struct IsTCardX<tCIDLib::TCard2> : TrueType {};
    template <> struct IsTCardX<tCIDLib::TCard4> : TrueType {};

    template <typename T> struct IsTFloatX : FalseType {};
    template <> struct IsTFloatX<tCIDLib::TFloat4> : TrueType {};
    template <> struct IsTFloatX<tCIDLib::TFloat8> : TrueType {};

    template <typename T> struct IsTIntX : FalseType {};
    template <> struct IsTIntX<tCIDLib::TInt1> : TrueType {};
    template <> struct IsTIntX<tCIDLib::TInt2> : TrueType {};
    template <> struct IsTIntX<tCIDLib::TInt4> : TrueType {};

    template <typename T> struct IsNumeric
    {
        static constexpr tCIDLib::TBoolean bState =
        (
            IsTCardX<T>::bState || IsTFloatX<T>::bState || IsTIntX<T>::bState
        );
    };

    // For conditional stuff based on const'ness
    template <typename T> struct IsConst : FalseType {};
    template <typename T> struct IsConst<const T> : TrueType {};

    // For conditional inclusion
    template<bool C, typename T = tCIDLib::TVoid> struct EnableIf { };
    template<typename T> struct EnableIf<kCIDLib::True, T> { typedef T Type; };


    // Remove constness
    template <typename T> struct RemoveConst { typedef T Type; };
    template <typename T> struct RemoveConst<const T> { typedef T Type; };


    //  To force move semantics
    template<class T> struct RemoveRef    {typedef T Type;};
    template<class T> struct RemoveRef    <T&>  {typedef T Type;};
    template<class T> struct RemoveRef    <T&&> {typedef T Type;};

    template <typename T>
    typename RemoveRef<T>::Type&& ForceMove(T&& src)
    {
        return static_cast<typename RemoveRef<T>::Type&&>(src);
    }


    // -----------------------------------------------------------------------
    //  Implements forwarding so that we can support in place construction for
    //  collections and other things.
    // -----------------------------------------------------------------------
    template <typename T>
    constexpr T&& Forward(typename RemoveRef<T>::Type& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr T&& Forward(typename RemoveRef<T>::type&& t) noexcept
    {
        return static_cast<T&&>(t);
    }


    // -----------------------------------------------------------------------
    //  These are just simple templates to return the smaller or larger of two
    //  quantities (with 'quantity' meaning anything with a < and > operator.)
    // -----------------------------------------------------------------------
    template <typename T> constexpr T MaxVal(const T& v1, const T& v2)
    {
        return (v1 > v2) ? v1 : v2;
    }

    template <typename T> constexpr T MinVal(const T& v1, const T& v2)
    {
        return (v1 < v2) ? v1 : v2;
    }


    // ---------------------------------------------------------------------------
    //  Calculate the size of an array
    // ---------------------------------------------------------------------------
    template <typename T, int size>
    constexpr tCIDLib::TCard4 c4ArrayElems(T(&)[size]) {return size;}


    // -----------------------------------------------------------------------
    //  Return the absolute value different
    // -----------------------------------------------------------------------
    template <typename T> constexpr T AbsDiff(const T& v1, const T& v2)
    {
        if (v1 > v2)
            return v1 - v2;
        return v2 - v1;
    }


    // -----------------------------------------------------------------------
    //  Tests whether the value is within the min/max values passed, inclusive
    // -----------------------------------------------------------------------
    template <typename T>
    constexpr tCIDLib::TBoolean bInRange(const T& val, const T& min, const T& max)
    {
        if ((val < min) || (val > max))
            return kCIDLib::False;
        return kCIDLib::True;
    }


    // -----------------------------------------------------------------------
    //  Swaps two values of any type that can support by value semantics.
    // -----------------------------------------------------------------------
    template <typename T> tCIDLib::TVoid Swap(T& v1, T& v2)
    {
        T vTmp = v1;
        v1 = v2;
        v2 = vTmp;
    }


    // -----------------------------------------------------------------------
    //  This template offsets a pointer by an integral value and casts the
    //  return back to the original type.
    // -----------------------------------------------------------------------
    template <typename T> const T*
    pOffsetPtr(const T* pToOffset, const tCIDLib::TInt4 i4Ofs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+i4Ofs);
    }

    template <typename T> const T*
    pOffsetPtr(const T* pToOffset, const tCIDLib::TCard4 c4Ofs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+c4Ofs);
    }

    template <typename T> const T*
    pOffsetPtr(const T* pToOffset, const unsigned int uiOfs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+uiOfs);
    }

    template <typename T> T*
    pOffsetNCPtr(T* pToOffset, const tCIDLib::TInt4 i4Ofs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+i4Ofs);
    }

    template <typename T> T*
    pOffsetNCPtr(T* pToOffset, const tCIDLib::TCard4 c4Ofs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+c4Ofs);
    }

    template <typename T> T*
    pOffsetNCPtr(T* pToOffset, const unsigned int uiOfs)
    {
        return (T*) (((tCIDLib::TCard1*)pToOffset)+uiOfs);
    }


    // -----------------------------------------------------------------------
    //  Generalized means of setting, clearing, and checking bits in enums
    //  that are used as bit masks.
    //
    //  Clear and Set assume that only the 'on' bits in the tBits value matter.
    //  If you want to arbitrarily turn off and on bits at once, use the
    //  eMaskEnumBits method, which takes a mask of relevant bits.
    // -----------------------------------------------------------------------
    template <typename T> constexpr tCIDLib::TBoolean
    bAnyBitsOn(const T tCurrent, const T tToCheck)
    {
        tCIDLib::TEnumMaskType emtCur = tCIDLib::TEnumMaskType(tCurrent);
        tCIDLib::TEnumMaskType emtToCheck = tCIDLib::TEnumMaskType(tToCheck);
        return (emtCur & emtToCheck) != 0;
    }

    template <typename T> constexpr tCIDLib::TBoolean
    bAllBitsOn(const T tCurrent, const T tToCheck)
    {
        tCIDLib::TEnumMaskType emtTmp = tCIDLib::TEnumMaskType(tCurrent);
        tCIDLib::TEnumMaskType emtToCheck = tCIDLib::TEnumMaskType(tToCheck);
        emtTmp &= emtToCheck;
        return (emtTmp == emtToCheck);
    }

    template <typename T> constexpr tCIDLib::TBoolean
    bBitsChanged(const T t1, const T t2, const T tMask)
    {
        tCIDLib::TEnumMaskType emt1 = tCIDLib::TEnumMaskType(t1);
        tCIDLib::TEnumMaskType emt2 = tCIDLib::TEnumMaskType(t2);
        tCIDLib::TEnumMaskType emtMask = tCIDLib::TEnumMaskType(tMask);
        return T(emt1 & emtMask) != T(emt2 * emtMask);
    }

    template <typename T> constexpr T
    eClearEnumBits(const T tCurrent, const T tBits)
    {
        tCIDLib::TEnumMaskType emtCur = tCIDLib::TEnumMaskType(tCurrent);
        tCIDLib::TEnumMaskType emtBits = tCIDLib::TEnumMaskType(tBits);
        emtCur &= ~emtBits;
        return T(emtCur);
    }

    template <typename T> constexpr T
    eMaskEnumBits(const T tCurrent, const T tBits, const T tMask)
    {
        tCIDLib::TEnumMaskType emtCur = tCIDLib::TEnumMaskType(tCurrent);
        tCIDLib::TEnumMaskType emtBits = tCIDLib::TEnumMaskType(tBits);
        tCIDLib::TEnumMaskType emtMask = tCIDLib::TEnumMaskType(tMask);

        emtCur &= ~emtMask;
        emtCur |= (emtBits & emtMask);
        return T(emtCur);
    }

    template <typename T> constexpr T eOREnumBits(const T t1, const T t2)
    {
        return T
        (
            tCIDLib::TEnumMaskType(t1) | tCIDLib::TEnumMaskType(t2)
        );
    }

    template <typename T> constexpr T
    eOREnumBits(const T t1, const T t2, const T t3)
    {
        return T
        (
            tCIDLib::TEnumMaskType(t1)
            | tCIDLib::TEnumMaskType(t2)
            | tCIDLib::TEnumMaskType(t3)
        );
    }

    template <typename T> constexpr T
    eOREnumBits(const T t1, const T t2, const T t3, const T t4, const T t5 = T(0))
    {
        return T
        (
            tCIDLib::TEnumMaskType(t1)
            | tCIDLib::TEnumMaskType(t2)
            | tCIDLib::TEnumMaskType(t3)
            | tCIDLib::TEnumMaskType(t4)
            | tCIDLib::TEnumMaskType(t5)
        );
    }

    template <typename T> constexpr T
    eANDEnumBits(const T tLeft, const T tRight)
    {
        return T(tCIDLib::TEnumMaskType(tLeft) & tCIDLib::TEnumMaskType(tRight));
    }

    template <typename T> constexpr T
    eANDEnumBits(const T t1, const T t2, const T t3)
    {
        return T
        (
            tCIDLib::TEnumMaskType(t1)
            & tCIDLib::TEnumMaskType(t2)
            & tCIDLib::TEnumMaskType(t3)
        );
    }


    //
    //  A simple generic pair of values of arbitrary type. Any required functionality
    //  of the members is the business of the user. But they have to be assignable
    //  at least.
    //
    template <typename F, typename S> struct TBasicPair
    {
        TBasicPair() = default;
        TBasicPair(const F& fVal, const S& sVal) :
            m_tF(fVal)
            , m_tS(sVal)
        {
        }
        F m_tF;
        S m_tS;
    };


    // Default relative magnitude comparator for sorting
    template <typename T> tCIDLib::ESortComps eComp(const T& t1, const T& t2)
    {
        if (t1 < t2)
            return tCIDLib::ESortComps::FirstLess;
        else if (t1 > t2)
            return tCIDLib::ESortComps::FirstGreater;
        return tCIDLib::ESortComps::Equal;
    }

    // For reverse sorting
    template <typename T> tCIDLib::ESortComps eRevComp(const T& t1, const T& t2)
    {
        if (t2 < t1)
            return tCIDLib::ESortComps::FirstLess;
        else if (t2 > t1)
            return tCIDLib::ESortComps::FirstGreater;
        return tCIDLib::ESortComps::Equal;
    }


    // And for pointers
    template <typename T> tCIDLib::ESortComps eCompPtr(const T* pt1, const T* pt2)
    {
        if (*pt1 < *pt2)
            return tCIDLib::ESortComps::FirstLess;
        else if (*pt1 > *pt2)
            return tCIDLib::ESortComps::FirstGreater;
        return tCIDLib::ESortComps::Equal;
    }

    template <typename T> tCIDLib::ESortComps eRevCompPtr(const T* pt1, const T* pt2)
    {
        if (*pt2 < *pt1)
            return tCIDLib::ESortComps::FirstLess;
        else if (*pt2 > *pt1)
            return tCIDLib::ESortComps::FirstGreater;
        return tCIDLib::ESortComps::Equal;
    }


    //
    //  For those cases where they need to use a function pointer comparator,
    //  but would like to get reverse order. We wrap it in a functor type
    //  class and flip the result.
    //
    template <typename T> class TRevCompWrap
    {
        public :
            using TWrapped = tCIDLib::ESortComps (*)(const T&, const T&);
            TRevCompWrap(TWrapped pfnWrapped) :

                m_pfnWrapped(pfnWrapped)
            {
            }

            tCIDLib::ESortComps operator()(const T& t1, const T& t2) const
            {
                tCIDLib::ESortComps eRet = m_pfnWrapped(t1, t2);
                if (eRet == tCIDLib::ESortComps::Equal)
                    return eRet;
                else if (eRet == tCIDLib::ESortComps::FirstGreater)
                    return tCIDLib::ESortComps::FirstLess;
                return tCIDLib::ESortComps::FirstGreater;
            }

        private :
            TWrapped    m_pfnWrapped;
    };

    // And one for pointers
    template <typename T> class TRevCompWrapPtr
    {
        public :
            using TWrapped = tCIDLib::ESortComps (*)(const T*, const T*);

            TRevCompWrapPtr(TWrapped pfnWrapped) :

                m_pfnWrapped(pfnWrapped)
            {
            }

            tCIDLib::ESortComps operator()(const T* pt1, const T* pt2) const
            {
                tCIDLib::ESortComps eRet = m_pfnWrapped(*pt1, *pt2);
                if (eRet == tCIDLib::ESortComps::Equal)
                    return eRet;
                else if (eRet == tCIDLib::ESortComps::FirstGreater)
                    return tCIDLib::ESortComps::FirstLess;
                return tCIDLib::ESortComps::FirstGreater;
            }

        private :
            TWrapped    m_pfnWrapped;
    };


    // Default equality comparator
    template <typename T> class TDefEqComp
    {
        public :
            TDefEqComp() = default;
            TDefEqComp(const TDefEqComp&) = default;
            ~TDefEqComp() = default;

            constexpr tCIDLib::TBoolean operator()(const T& t1, const T& t2) const
            {
                return t1 == t2;
            }
    };

    // Default relative magnitude comparator
    template <typename T> class TDefMagComp
    {
        public :
            TDefMagComp() = default;
            TDefMagComp(const TDefMagComp&) = default;
            ~TDefMagComp() = default;

            constexpr tCIDLib::ESortComps operator()(const T& t1, const T& t2) const
            {
                if (t1 < t2)
                    return tCIDLib::ESortComps::FirstLess;
                else if (t1 > t2)
                    return tCIDLib::ESortComps::FirstGreater;
                return tCIDLib::ESortComps::Equal;
            }
    };
}
