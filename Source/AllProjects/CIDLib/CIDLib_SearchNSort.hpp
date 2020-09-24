//
// FILE NAME: CIDLib_SearchNSort.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1996
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
//  This file implements some template based search and sort algorithms for
//  sorting and search raw arrays.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TArrayOps
{
    // A threashold that we use to decide if it's worth qsorting a sub-file
    constexpr tCIDLib::TCard4   c4MinMergeSz = 32;

    // Max stack elements required
    constexpr tCIDLib::TCard4   c4MaxPending = 85;

    //
    //  Temp merge array size. Larger than this and we allocate a buffer
    //  to use instead.
    //
    constexpr tCIDLib::TCard4   c4TmpArraySz = 256;

    // The default minimum size of a run
    constexpr tCIDLib::TCard4   c4DefMinGallop = 7;

    //
    //  Some out of line methods used by the sort stuff below, to limit
    //  code bloat where possible.
    //
    CIDLIBEXP tCIDLib::TCard4 c4CalcMinRunLen(const tCIDLib::TCard4 c4ArraySz);


    //
    //  Control info for the sort operation. It provides a stack for
    //  pushing pending sub-files onto and some memory used for doing
    //  merges of runs.
    //
    template <typename T> class TSortState
    {
        public :
            //We don't initialize the arrays since that's just unnecessary overhead
            #pragma warning(suppress : 26495)
            TSortState(T* const pToSort, const tCIDLib::TCard4 c4Size) :

                m_c4PendingCnt(0)
                , m_c4MemSize(c4TmpArraySz)
                , m_c4MinGallop(c4DefMinGallop)
                , m_c4SortSz(c4Size)
                , m_ptMergeArray()
                , m_ptMergeMem(nullptr)
                , m_ptToSort(pToSort)
            {
                m_ptMergeMem = m_ptMergeArray;
            }

            ~TSortState()
            {
            }

            tCIDLib::TBoolean bIsEmpty() const
            {
                return (m_c4PendingCnt == 0);
            }

            tCIDLib::TCard4 c4LenAt(const tCIDLib::TCard4 c4At)
            {
                CIDAssert(c4At < m_c4PendingCnt, L"Invalid pending run index");
                return m_ac4Len[c4At];
            }

            tCIDLib::TCard4 c4MinGallop() const
            {
                return m_c4MinGallop;
            }

            tCIDLib::TCard4 c4MinGallop(const tCIDLib::TCard4 c4ToSet)
            {
                m_c4MinGallop = c4ToSet;
                return c4ToSet;
            }

            tCIDLib::TCard4 c4PendingCnt() const
            {
                return m_c4PendingCnt;
            }

            tCIDLib::TCard4 c4PendingCnt(const tCIDLib::TCard4 c4ToSet)
            {
                m_c4PendingCnt = c4ToSet;
                return m_c4PendingCnt;
            }

            tCIDLib::TCard4 c4Push( const   tCIDLib::TCard4 c4Base
                                    , const tCIDLib::TCard4 c4Len)
            {
                CIDAssert(m_c4PendingCnt < c4MaxPending, L"Pending run overflow");

                CIDLib_Suppress(6386)  // We range checked above
                m_ac4BaseInd[m_c4PendingCnt] = c4Base;

                CIDLib_Suppress(6386)  // We range checked above
                m_ac4Len[m_c4PendingCnt] = c4Len;
                return m_c4PendingCnt++;
            }

            tCIDLib::TVoid DecCount()
            {
                CIDAssert(m_c4PendingCnt != 0, L"Pending count underflow");
                m_c4PendingCnt--;
            }

            tCIDLib::TVoid GetAt(const  tCIDLib::TCard4     c4At
                                ,       tCIDLib::TInt4&     i4Base
                                ,       tCIDLib::TInt4&     i4Len)
            {
                CIDAssert(c4At < m_c4PendingCnt, L"Invalid pending run index");

                i4Base = tCIDLib::TInt4(m_ac4BaseInd[c4At]);
                i4Len = tCIDLib::TInt4(m_ac4Len[c4At]);
            };

            tCIDLib::TVoid Move(const   tCIDLib::TCard4 c4Tar
                                , const tCIDLib::TCard4 c4Src)
            {
                CIDAssert(c4Src < m_c4PendingCnt, L"Invalid pending run index");
                CIDAssert(c4Tar < m_c4PendingCnt, L"Invalid pending run index");

                m_ac4BaseInd[c4Tar] = m_ac4BaseInd[c4Src];
                m_ac4Len[c4Tar] = m_ac4Len[c4Src];
            }

            tCIDLib::TVoid PutAt(const  tCIDLib::TCard4 c4At
                                , const tCIDLib::TCard4 c4Base
                                , const tCIDLib::TCard4 c4Len)
            {
                CIDAssert(c4At < m_c4PendingCnt, L"Invalid pending run index");

                m_ac4BaseInd[c4At] = c4Base;
                m_ac4Len[c4At] = c4Len;
            }

            tCIDLib::TVoid PutLenAt(const   tCIDLib::TCard4 c4At
                                    , const tCIDLib::TCard4 c4Len)
            {
                CIDAssert(c4At < m_c4PendingCnt, L"Invalid pending run index");
                m_ac4Len[c4At] = c4Len;
            }


            //
            //  If we allocated memory, free it and go back to pointing
            //  at the local buffer.
            //
            void FreeMem()
            {
                if (m_ptMergeMem != m_ptMergeArray)
                    delete [] m_ptMergeMem;

                m_c4MemSize = c4TmpArraySz;
                m_ptMergeMem = m_ptMergeArray;
            }


            //
            //  Sets up the merge memory. If it's more than our local
            //  temp buffer, we allocate a buffer instead.
            //
            tCIDLib::TVoid AllocMergeMem(const tCIDLib::TCard4 c4Size)
            {
                // If smaller than what we already have, go with that
                if (c4Size <= m_c4MemSize)
                    return;

                // We need more, so free our current one and allocate again
                if (m_ptMergeMem != m_ptMergeArray)
                {
                    delete [] m_ptMergeMem;
                    m_ptMergeMem = nullptr;
                }

                m_ptMergeMem = new T[c4Size];
                m_c4MemSize = c4Size;
            }

            // Provide access to the merge memory info and original array
            T* ptToSort()
            {
                return m_ptToSort;
            }

            T* ptMergeMem()
            {
                return m_ptMergeMem;
            }

            T tToSortAt(const tCIDLib::TCard4 c4At)
            {
                CIDAssert(c4At < m_c4SortSz, L"Invalid sort array index");
                return m_ptToSort[c4At];
            }


        private :
            // Pending run list
            tCIDLib::TCard4 m_ac4BaseInd[c4MaxPending];
            tCIDLib::TCard4 m_ac4Len[c4MaxPending];

            // Merge memory info
            tCIDLib::TCard4 m_c4MemSize;
            T*              m_ptMergeMem;
            T               m_ptMergeArray[c4TmpArraySz];

            // The original array we are sorting, and it's size
            tCIDLib::TCard4 m_c4SortSz;
            T*              m_ptToSort;

            tCIDLib::TCard4 m_c4MinGallop;
            tCIDLib::TCard4 m_c4PendingCnt;
    };


    // Reverses a run (subfile) in place
    template <typename T>
    tCIDLib::TVoid ReverseSlice(        T* const        ptArray
                                , const tCIDLib::TCard4 c4Low
                                , const tCIDLib::TCard4 c4High)
    {
        //
        //  If it's a fundamental there's no def ctor so we get an uninitialized
        //  warning.
        //
        #pragma warning(suppress : 26494)
        T tTemp;

        tCIDLib::TCard4 c4L = c4Low;
        tCIDLib::TCard4 c4H = c4High;
        c4H--;
        while (c4L < c4H)
        {
            tTemp        = ptArray[c4L];
            ptArray[c4L] = ptArray[c4H];
            ptArray[c4H] = tTemp;

            c4L++;
            c4H--;
        }
    }


    template <typename T, typename TCompFunc> tCIDLib::TCard4
    c4CountRunAndMakeAscending(         T* const            ptArray
                                , const tCIDLib::TInt4      i4Low
                                , const tCIDLib::TInt4      i4High
                                ,       TCompFunc           pfnComp)
    {
        tCIDLib::TInt4 i4CurInd = 0;

        CIDAssert(i4Low < i4High, L"Out of order sort run high/low");

        // If we have hit two consequtive values, return a 1 element run
        if (i4Low + 1 == i4High)
            return 1;

        //
        //  We are looking for pre-sorted (either up or down) runs here,
        //  so we check the first two and then see how far they remain
        //  sorted in the direction of the first two.
        //
        if (pfnComp(ptArray[i4Low], ptArray[i4Low + 1]) == tCIDLib::ESortComps::FirstLess)
        {
            // They are assending
            i4CurInd = i4Low + 2;

            while (i4CurInd < i4High)
            {
                if (pfnComp(ptArray[i4CurInd - 1], ptArray[i4CurInd]) != tCIDLib::ESortComps::FirstGreater)
                    i4CurInd++;
                 else
                    break;
            }
        }
         else
        {
            // They are descending
            i4CurInd = i4Low + 2;
            while (i4CurInd < i4High)
            {
                if (pfnComp(ptArray[i4CurInd - 1], ptArray[i4CurInd]) == tCIDLib::ESortComps::FirstGreater)
                    i4CurInd++;
                    break;
            }

            // And since this one is backwards, reverse it
            ReverseSlice(ptArray, i4Low, i4CurInd);
        }

        // And return the difference, cast back to unsigned
        return tCIDLib::TCard4(i4CurInd - i4Low);
    }


    //
    //  When we find a run that merits doing a binary insertion sort, this
    //  is used to do that.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid BinarySort(          T* const            ptArray
                                , const tCIDLib::TInt4      i4Low
                                , const tCIDLib::TInt4      i4High
                                , const tCIDLib::TInt4      i4Start
                                ,       TCompFunc           pfnComp)
    {
        //
        //  If the type happens to be a fundamental, this we get an uninit warning on
        //  the pivot type because no default constructor.
        //
        #pragma warning(suppress : 26494)
        T               tPivot;
        tCIDLib::TInt4  i4Left = 0;
        tCIDLib::TInt4  i4Right = 0;
        tCIDLib::TInt4  i4Middle = 0;

        CIDAssert
        (
            (i4Low <= i4Start) && (i4Start <= i4High)
            , L"Invalid binary sort subfile order"
        );

        // If we are starting at the first element, move upwards
        tCIDLib::TInt4 i4Index = i4Start;
        if (i4Low == i4Index)
            i4Index++;

        for (; i4Index< i4High; i4Index++)
        {
            tPivot = ptArray[i4Index];
            i4Left  = i4Low;
            i4Right = i4Index;

            CIDAssert(i4Left < i4Right, L"Left was > right");

            while (i4Left < i4Right)
            {
                i4Middle = (i4Left + i4Right) >> 1;

                if (pfnComp(tPivot, ptArray[i4Middle]) == tCIDLib::ESortComps::FirstLess)
                    i4Right = i4Middle;
                else
                    i4Left = i4Middle + 1;
            }

            CIDAssert(i4Left == i4Right, L"Left was != right");

            // Make room and put the pivot item in place
            for (tCIDLib::TInt4 i4Ind = i4Index; i4Ind > i4Left; i4Ind--)
            {
                ptArray[i4Ind] = ptArray[i4Ind - 1];
            }
            ptArray[i4Left] = tPivot;
        }
    }


    //
    //  Locates the position at which to insert the specified key into the
    //  specified sorted range;
    //
    //  If the range contains an element equal to key, returns the index of
    //  the leftmost equal element.
    //
    //  tKey    : the key whose insertion point to search for
    //  ptArray : the array in which to search
    //  i4Base   : the index of the first element in the range
    //  i4Len    : the length of the range
    //  i4Hint   : the index at which to begin the search.
    //           the closer hint is to the result, the faster timSort runs.
    //
    //  returns k (0 <= k <= i4Len) such that
    //
    //       ptArray[i4Base + k - 1] < tKey <= ptArray[i4Base + k]
    //
    //  Pretending that ptArray[i4Base - 1] is minus infinity and
    //  aArray[aBase + i4Len] is infinity.
    //
    //  IOW, key belongs at index i4Base + k;
    //
    //  or IOW, the first k elements of aArray should precede key,
    //  and the last aLen - k should follow it.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TInt4 i4GallopLeft(const   T                   tKey
                                ,       T* const            ptArray
                                , const tCIDLib::TInt4      i4Base
                                , const tCIDLib::TInt4      i4Len
                                , const tCIDLib::TInt4      i4Hint
                                ,       TCompFunc           pfnComp)
    {
        tCIDLib::TInt4  i4Offset = 0;
        tCIDLib::TInt4  i4LastOffset = 0;
        tCIDLib::TInt4  i4MaxOfs = 0;
        tCIDLib::TInt4  i4Temp = 0;
        tCIDLib::TInt4  i4Middle = 0;

        CIDAssert
        (
            (i4Len > 0) && (i4Hint >= 0) && (i4Hint < i4Len)
            , L"Left Gallop parms are invalid"
        );

        i4LastOffset = 0;
        i4Offset     = 1;

        if (pfnComp(tKey, ptArray[i4Base + i4Hint]) == tCIDLib::ESortComps::FirstGreater)
        {
            //
            // key > a[b+h]
            // Gallop right until a[b+h+i4LastOffset] < key <= a[b+h+i4Offset]
            // (a : ptArray, b : i4Base, h : i4Hint)
            //
            i4MaxOfs = i4Len - i4Hint;

            while (i4Offset < i4MaxOfs)
            {
                if (pfnComp(tKey, ptArray[i4Base + i4Hint + i4Offset]) == tCIDLib::ESortComps::FirstGreater)
                {
                    i4LastOffset = i4Offset;
                    i4Offset     = (i4Offset << 1) + 1;
                    if (i4Offset <= 0)
                        i4Offset = i4MaxOfs;
                }
                 else
                {
                    break;
                }
            }

            if (i4Offset > i4MaxOfs)
                i4Offset = i4MaxOfs;

            // Make i4Offset relative to i4Base
            i4LastOffset += i4Hint;
            i4Offset     += i4Hint;
        }
        else
        {
            //
            // key <= a[b+h]
            // Gallop right until a[b+h+i4LastOffset] < key <= a[b+h+i4Offset]
            // (a : ptArray, b : i4Base, h : i4Hint)
            //
            i4MaxOfs = i4Hint + 1;
            while (i4Offset < i4MaxOfs)
            {
                if (pfnComp(tKey, ptArray[i4Base + i4Hint - i4Offset]) == tCIDLib::ESortComps::FirstGreater)
                {
                    break;
                }
                 else
                {
                    i4LastOffset = i4Offset;
                    i4Offset     = (i4Offset << 1) + 1;

                    // Overflow
                    if (i4Offset <= 0)
                        i4Offset = i4MaxOfs;
                }
            }

            if (i4Offset > i4MaxOfs)
                i4Offset = i4MaxOfs;

            // Make i4Offset relative to i4Base
            i4Temp       = i4LastOffset;
            i4LastOffset = i4Hint - i4Offset;
            i4Offset     = i4Hint - i4Temp;
        }

        CIDAssert
        (
            (-1 <= i4LastOffset) && (i4LastOffset < i4Offset) && (i4Offset <= i4Len)
            , L"Left Gallop first step failed"
        );

        //
        // Now a[b+i4LastOffset] < key <= a[b+i4Offset].
        // So, key belongs somewhere to the right of i4LastOffset,
        // but no farther right than i4Offset.
        //
        // Do a binary search with invariant
        //
        //      a[b+i4LastOffset-1] < key <= a[b+i4Offset].
        //
        i4LastOffset++;
        while (i4LastOffset < i4Offset)
        {
            i4Middle = i4LastOffset + ((i4Offset - i4LastOffset) >> 1);

            if (pfnComp(tKey, ptArray[i4Base + i4Middle]) == tCIDLib::ESortComps::FirstGreater)
                i4LastOffset = i4Middle + 1;
             else
                i4Offset = i4Middle;
        }

        CIDAssert(i4LastOffset == i4Offset, L"Last offset != offset");
        return i4Offset;
    }


    //
    // Like timGallopLeft(), except that if the range contains an element equal to
    // the tKey, timGallopRight() returns the index after the rightmost equal element.
    //
    // tKey   : the key whose insertion point to search for
    // ptArray : the array in which to search
    // i4Base  : the index of the first element in the range
    // i4Len   : the length of the range
    // i4Hint  : the index at which to begin the search.
    //          the closer hint is to the result, the faster timSort runs.
    //
    // returns k (0 <= k <= i4Len) such that
    //
    //      ptArray[i4Base + k - 1] <= key < ptArray[i4Base + k]
    //
    // This is called gallop RIGHT because searching direction is from left to RIGHT.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TInt4 i4GallopRight(const  T                   tKey
                                ,       T* const            ptArray
                                , const tCIDLib::TInt4      i4Base
                                , const tCIDLib::TInt4      i4Len
                                , const tCIDLib::TInt4      i4Hint
                                ,       TCompFunc           pfnComp)
    {
        tCIDLib::TInt4  i4Ofs = 0;
        tCIDLib::TInt4  i4LastOfs = 0;
        tCIDLib::TInt4  i4MaxOfs = 0;
        tCIDLib::TInt4  i4Temp = 0;
        tCIDLib::TInt4  i4Middle = 0;

        CIDAssert
        (
            (i4Len > 0) && (i4Hint >= 0) && (i4Hint < i4Len)
            , L"Gallop right parms incorrect"
        );

        i4LastOfs = 0;
        i4Ofs     = 1;

        if (pfnComp(tKey, ptArray[i4Base + i4Hint]) == tCIDLib::ESortComps::FirstLess)
        {
            //
            // key < a[b+h]
            // Gallop left until a[b+h-i4Ofs] <= key < a[b+h-i4LastOfs]
            // (a : ptArray, b : i4Base, h : i4Hint)
            //
            i4MaxOfs = i4Hint + 1;

            while (i4Ofs < i4MaxOfs)
            {
                if (pfnComp(tKey, ptArray[i4Base + i4Hint - i4Ofs]) == tCIDLib::ESortComps::FirstLess)
                {
                    i4LastOfs = i4Ofs;
                    i4Ofs     = (i4Ofs << 1) + 1;

                    if (i4Ofs <= 0)
                        i4Ofs = i4MaxOfs;
                }
                 else
                {
                    break;
                }
            }

            if (i4Ofs > i4MaxOfs)
                i4Ofs = i4MaxOfs;

            // Make i4Ofs relative to i4Base
            i4Temp       = i4LastOfs;
            i4LastOfs = i4Hint - i4Ofs;
            i4Ofs     = i4Hint - i4Temp;
        }
         else
        {
            //
            // key >= a[b+h]
            // Gallop right until a[b+h+i4LastOfs] <= key < a[h+i4Ofs]
            // (a : ptArray, b : i4Base, h : i4Hint)
            //
            i4MaxOfs = i4Len - i4Hint;

            while (i4Ofs < i4MaxOfs)
            {
                if (pfnComp(tKey, ptArray[i4Base + i4Hint + i4Ofs]) == tCIDLib::ESortComps::FirstLess)
                {
                    break;
                }
                 else
                {
                    i4LastOfs = i4Ofs;
                    i4Ofs     = (i4Ofs << 1) + 1;

                    if (i4Ofs <= 0)
                        i4Ofs = i4MaxOfs;
                }
            }

            if (i4Ofs > i4MaxOfs)
                i4Ofs = i4MaxOfs;

            // Make i4Ofs relative to i4Base
            i4LastOfs += i4Hint;
            i4Ofs     += i4Hint;
        }

        CIDAssert
        (
            (-1 <= i4LastOfs) && (i4LastOfs < i4Ofs) && (i4Ofs <= i4Len)
            , L"Gallop right first step failed"
        );

        //
        // Now a[b + i4LastOfs] <= key < a[b + i4Ofs].
        // So, key belongs somewhere to the right of i4LastOfs,
        // but no farther right than i4Ofs.
        //
        // Do a binary search with invariant
        //
        //      a[b+i4LastOfs-1] <= key < a[b+i4Ofs]
        //
        i4LastOfs++;
        while (i4LastOfs < i4Ofs)
        {
            i4Middle = i4LastOfs + ((i4Ofs - i4LastOfs) >> 1);

            if (pfnComp(tKey, ptArray[i4Base + i4Middle]) == tCIDLib::ESortComps::FirstLess)
                i4Ofs = i4Middle;
            else
                i4LastOfs = i4Middle + 1;
        }

        CIDAssert(i4LastOfs == i4Ofs, L"i4LastOfs != i4Ofs");

        return i4Ofs;
    }


    //
    // Merges two adjacent runs in place, in a stable way.
    // The first element of the first run must be greater than the first
    // element of the second run.
    //
    //      a[i4Base1] > a[i4Base2]
    //
    // and the last element of the first run must be greater than
    // all elements of the second run.
    //
    //     a[i4Base1 + i4Len1] > every a[i4Base2 .. i4Base2 + i4Len2]
    //
    //      IOW, a[i4Base1 + i4Len1] is the maximum.
    //
    //      timGallopRight() and timGallopLeft() called just before
    //      timMergeLow() or timMergeHigh() is called creates this condition.
    //
    // For performance, this should be called only when
    //
    //     i4Len1 <= i4Len2;
    //
    // its counterpart, timMergeHigh() should be called if i4Len1 >= i4Len2.
    // Either can be called if i4Len1 == i4Len2.
    //
    // This method conducts merge from left to right.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid MergeLow(        TSortState<T>&      State
                            , const tCIDLib::TInt4      i4Base1
                            ,       tCIDLib::TInt4      i4Len1
                            , const tCIDLib::TInt4      i4Base2
                            ,       tCIDLib::TInt4      i4Len2
                            ,       TCompFunc           pfnComp)
    {
        T *ptArray = State.ptToSort();
        T *ptTmp = nullptr;

        tCIDLib::TCard4 c4MinGallop = 0;
        tCIDLib::TInt4  i4Cursor1 = 0;      // Indexes into tmp array (run1)
        tCIDLib::TInt4  i4Cursor2 = 0;      // Indexes into original array. run2
        tCIDLib::TInt4  i4DestIndex = 0;    // Indexes into original array.

        CIDAssert
        (
            (i4Len1 > 0) && (i4Len2 > 0) && (i4Base1 + i4Len1 == i4Base2)
            , L"MergeLow parameters are invalid"
        );

        //
        // Should always prepare temp memory with size s, where
        //
        //   s = min(len(run1), len(run2))
        //
        // In MergeLow, i4Len1 is always less than i4Len2
        //
        State.AllocMergeMem(i4Len1);
        TRawMem::CopyMemBuf
        (
            State.ptMergeMem()
            , ptArray + i4Base1
            , sizeof(T) * i4Len1
        );
        ptTmp = State.ptMergeMem();

        i4Cursor1    = 0;
        i4Cursor2    = i4Base2;
        i4DestIndex  = i4Base1;

        // Move first element of second run
        ptArray[i4DestIndex] = ptArray[i4Cursor2];
        i4DestIndex++;
        i4Cursor2++;
        i4Len2--;

        // Dealing with degenerate cases
        if (i4Len2 == 0) goto LABEL_SUCCEED;
        if (i4Len1 == 1) goto LABEL_COPY_B;

        c4MinGallop = State.c4MinGallop();

        while (1)
        {
            //
            // Do the straightforward merge until (if ever) one run
            // appears to win consistently
            //

            // Number of times first and second runs won in a row
            tCIDLib::TCard4 c4Count1 = 0;
            tCIDLib::TCard4 c4Count2 = 0;

            // Normal left to right merg
            do
            {
                CIDAssert(i4Len1 > 1 && i4Len2 > 0, L"Left/right merge len error");

                if (pfnComp(ptArray[i4Cursor2], ptTmp[i4Cursor1]) == tCIDLib::ESortComps::FirstLess)
                {
                    ptArray[i4DestIndex] = ptArray[i4Cursor2];
                    i4DestIndex++;
                    i4Cursor2++;
                    i4Len2--;

                    c4Count1 = 0;
                    c4Count2++;

                    if (i4Len2 == 0)
                        goto LABEL_SUCCEED;
                }
                 else
                {
                    ptArray[i4DestIndex] = ptTmp[i4Cursor1];
                    i4DestIndex++;
                    i4Cursor1++;
                    i4Len1--;

                    c4Count1++;
                    c4Count2 = 0;

                    if (i4Len1 == 1)
                        goto LABEL_COPY_B;
                }

                // if Count1 > 0 then Count2 == 0
                // if Count2 > 0 then Count1 == 0;
            }   while ((c4Count1 | c4Count2) < c4MinGallop);

            //
            // One run is winning so consistently that galloping may
            // be a huge win. So try that, and continue galloping until
            // (if ever) neither run appears to be winning consistently
            // anymore.
            //
            c4MinGallop++;
            do
            {
                CIDAssert(i4Len1 > 1 && i4Len2 > 0, L"Merge length error");
                c4MinGallop -= c4MinGallop > 1;
                State.c4MinGallop(c4MinGallop);

                c4Count1 = i4GallopRight
                (
                    ptArray[i4Cursor2]
                    , ptTmp
                    , i4Cursor1
                    , i4Len1
                    , 0
                    , pfnComp
                );

                if (c4Count1 != 0)
                {
                    TRawMem::CopyMemBuf
                    (
                        ptArray + i4DestIndex, ptTmp + i4Cursor1, sizeof(T) * c4Count1
                    );

                    i4DestIndex += c4Count1;
                    i4Cursor1   += c4Count1;
                    i4Len1      -= c4Count1;
                    if (i4Len1 == 1) goto LABEL_COPY_B;
                    if (i4Len1 == 0) goto LABEL_SUCCEED;
                }

                ptArray[i4DestIndex] = ptArray[i4Cursor2];
                i4DestIndex++;
                i4Cursor2++;
                i4Len2--;

                if (i4Len2 == 0)
                    goto LABEL_SUCCEED;

                c4Count2 = i4GallopLeft
                (
                    ptTmp[i4Cursor1]
                    , ptArray
                    , i4Cursor2
                    , i4Len2
                    , 0
                    , pfnComp
                );

                if (c4Count2 != 0)
                {
                    // Src and dst may overlap, so do move instead of copy
                    TRawMem::MoveMemBuf
                    (
                        ptArray + i4DestIndex, ptArray + i4Cursor2, sizeof(T) * c4Count2
                    );

                    i4DestIndex += c4Count2;
                    i4Cursor2   += c4Count2;
                    i4Len2      -= c4Count2;

                    if (i4Len2 == 0)
                        goto LABEL_SUCCEED;
                }

                ptArray[i4DestIndex] = ptTmp[i4Cursor1];
                i4DestIndex++;
                i4Cursor1++;
                i4Len1--;

                if (i4Len1 == 1)
                    goto LABEL_COPY_B;

            } while ((c4Count1 >= c4MinGallop) || (c4Count2 >= c4MinGallop));

            // Penalize it for leaving galloping mode
            c4MinGallop++;
            State.c4MinGallop(c4MinGallop);
        }

    LABEL_SUCCEED:

        if (i4Len1 > 0)
        {
            TRawMem::CopyMemBuf
            (
                ptArray + i4DestIndex, ptTmp + i4Cursor1, sizeof(T) * i4Len1
            );
        }
        return;

    LABEL_COPY_B:
        CIDAssert(i4Len1 == 1 && i4Len2 > 0, L"COPY B failure");

        // The last element of the first run belongs at the end of the merge
        TRawMem::MoveMemBuf
        (
            ptArray + i4DestIndex, ptArray + i4Cursor2, sizeof(T) * i4Len2
        );
        ptArray[i4DestIndex + i4Len2] = ptTmp[i4Cursor1];
        return;
    }


    //
    // Just same as timMergeLow(), except that this one should be called only if
    //
    //      i4Len1 >= i4Len2;
    //
    // The first element of the first run must be greater than the first
    // element of the second run.
    //
    //      a[i4Base1] > a[i4Base2]
    //
    // and the last element of the first run must be greater than
    // all elements of the second run.
    //
    //     a[i4Base1 + i4Len1] > every a[i4Base2 .. i4Base2 + i4Len2]
    //
    //     IOW, a[i4Base1 + i4Len1] is the maximum.
    //
    //      timGallopRight() and timGallopLeft() called just before
    //      timMergeLow() or timMergeHigh() is called creates this condition.
    //
    //  This one merges from right to left.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid MergeHigh(       TSortState<T>&      State
                            , const tCIDLib::TInt4      i4Base1
                            ,       tCIDLib::TInt4      i4Len1
                            , const tCIDLib::TInt4      i4Base2
                            ,       tCIDLib::TInt4      i4Len2
                            ,       TCompFunc           pfnComp)

    {
        T *ptArray = State.ptToSort();
        T *ptTmp = nullptr;

        tCIDLib::TCard4 c4MinGallop = 0;
        tCIDLib::TInt4  i4Cursor1 = 0;      // Indexes into original array (run1)
        tCIDLib::TInt4  i4Cursor2 = 0;      // Indexes into tmp array (run2)
        tCIDLib::TInt4  i4DestInd = 0;      // Indexes into original array

        //
        // Should always prepare temp memory with size s, where
        //
        //      s = min(len(run1), len(run2))
        //
        // In MergeHigh, i4Len2 is always less than i4Len1
        //
        State.AllocMergeMem(i4Len2);

        // Copy second run into temp memory
        TRawMem::CopyMemBuf
        (
            State.ptMergeMem(), ptArray + i4Base2, sizeof(T) * i4Len2
        );
        ptTmp = State.ptMergeMem();

        i4Cursor1   = i4Base1 + i4Len1 - 1;
        i4Cursor2   = i4Len2 - 1;
        i4DestInd = i4Base2 + i4Len2 - 1;

        // Move last element of first run
        ptArray[i4DestInd] = ptArray[i4Cursor1];
        i4DestInd--;
        i4Cursor1--;
        i4Len1--;

        if (i4Len1 == 0) goto LABEL_SUCCEED;
        if (i4Len2 == 1) goto LABEL_COPY_A;

        c4MinGallop = State.c4MinGallop();

        while (1)
        {
            //
            // Do the straightforward merge until (if ever) one run
            // appears to win consistently
            //
            tCIDLib::TCard4 c4Count1 = 0;
            tCIDLib::TCard4 c4Count2 = 0;

            // Normal right to left merge
            do
            {
                CIDAssert(i4Len1 > 0 && i4Len2 > 1, L"Right/left merge index error");

                if (pfnComp(ptTmp[i4Cursor2], ptArray[i4Cursor1]) == tCIDLib::ESortComps::FirstLess)
                {
                    ptArray[i4DestInd] = ptArray[i4Cursor1];
                    i4DestInd--;
                    i4Cursor1--;
                    i4Len1--;

                    c4Count1++;
                    c4Count2 = 0;

                    if (i4Len1 == 0)
                        goto LABEL_SUCCEED;
                }
                 else
                {
                    ptArray[i4DestInd] = ptTmp[i4Cursor2];
                    i4DestInd--;
                    i4Cursor2--;
                    i4Len2--;

                    c4Count1 = 0;
                    c4Count2++;

                    if (i4Len2 == 1)
                        goto LABEL_COPY_A;
                }
            }   while ((c4Count1 | c4Count2) < c4MinGallop);

            //
            // One run is winning so consistently that galloping may
            // be a huge win.  So try that, and continue galloping until
            // (if ever) neither run appears to be winning consistently
            // anymore.
            //
            c4MinGallop++;
            do
            {
                CIDAssert(i4Len1 > 0 && i4Len2 > 1, L"High merge len error");
                c4MinGallop -= c4MinGallop > 1;
                State.c4MinGallop(c4MinGallop);

                c4Count1 = i4GallopRight
                (
                    ptTmp[i4Cursor2]
                    , ptArray
                    , i4Base1
                    , i4Len1
                    , i4Len1 - 1
                    , pfnComp
                );

                c4Count1 = i4Len1 - c4Count1;

                if (c4Count1 != 0)
                {
                    i4DestInd -= c4Count1;
                    i4Cursor1   -= c4Count1;
                    i4Len1      -= c4Count1;
                    TRawMem::MoveMemBuf
                    (
                        ptArray + i4DestInd + 1, ptArray + i4Cursor1 + 1, sizeof(T) * c4Count1
                    );

                    if (i4Len1 == 0)
                        goto LABEL_SUCCEED;
                }

                ptArray[i4DestInd] = ptTmp[i4Cursor2];
                i4DestInd--;
                i4Cursor2--;
                i4Len2--;

                if (i4Len2 == 1)
                    goto LABEL_COPY_A;

                c4Count2 = i4GallopLeft
                (
                    ptArray[i4Cursor1]
                    , ptTmp
                    , 0
                    , i4Len2
                    , i4Len2 - 1
                    , pfnComp
                );

                c4Count2 = i4Len2 - c4Count2;

                if (c4Count2 != 0)
                {
                    i4DestInd -= c4Count2;
                    i4Cursor2 -= c4Count2;
                    i4Len2 -= c4Count2;
                    TRawMem::CopyMemBuf
                    (
                        ptArray + i4DestInd + 1, ptTmp + i4Cursor2 + 1, sizeof(T) * c4Count2
                    );
                    if (i4Len2 == 1)
                        goto LABEL_COPY_A;

                    if (i4Len2 == 0)
                        goto LABEL_SUCCEED;
                }

                ptArray[i4DestInd] = ptArray[i4Cursor1];
                i4DestInd--;
                i4Cursor1--;
                i4Len1--;

                if (i4Len1 == 0)
                    goto LABEL_SUCCEED;

            } while (c4Count1 >= c4DefMinGallop || c4Count2 >= c4DefMinGallop);

            // Penalize it for leaving galloping mode
            c4MinGallop++;
            State.c4MinGallop(c4MinGallop);
        }

    LABEL_SUCCEED:
        if (i4Len2 > 0)
        {
            TRawMem::CopyMemBuf
            (
                ptArray + i4DestInd - (i4Len2 - 1), ptTmp, i4Len2 * sizeof(T)
            );
        }
        return;

    LABEL_COPY_A:
        CIDAssert(i4Len2 == 1 && i4Len1 > 0, L"Copy A error");

        i4DestInd -= i4Len1;
        i4Cursor1   -= i4Len1;
        TRawMem::MoveMemBuf
        (
            ptArray + i4DestInd + 1, ptArray + i4Cursor1 + 1, i4Len1 * sizeof(T)
        );
        ptArray[i4DestInd] = ptTmp[i4Cursor2];
        return;
    }


    //
    // Merges the two runs at stack indices i and i + 1.
    // Run i must be the penultimate or antepenultimate run on the stack.
    // IOW, i must be equal to
    //
    //       i == PendingRunCnt - 2 or
    //       i == PendingRuncnt - 3.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid MergeAt(         TSortState<T>&      State
                            , const tCIDLib::TCard4     c4Where
                            ,       TCompFunc           pfnComp)
    {
        tCIDLib::TInt4 i4BaseA = 0;
        tCIDLib::TInt4 i4LenA = 0;
        tCIDLib::TInt4 i4BaseB = 0;
        tCIDLib::TInt4 i4LenB = 0;
        tCIDLib::TInt4 i4K = 0;

        const tCIDLib::TCard4 c4InPendCnt = State.c4PendingCnt();
        CIDAssert(c4InPendCnt >= 2, L"Bad pending run count");
        CIDAssert
        (
            c4Where == c4InPendCnt - 2 || (c4Where == c4InPendCnt - 3)
            , L"Bad merge at point"
        );

        State.GetAt(c4Where, i4BaseA, i4LenA);
        State.GetAt(c4Where + 1, i4BaseB, i4LenB);

        CIDAssert(i4LenA > 0 && i4LenB > 0, L"MergeAt length error");
        CIDAssert(i4BaseA + i4LenA == i4BaseB, L"MergeAt base+length error");

        // Record the length of the combined runs;
        State.PutLenAt(c4Where, i4LenA + i4LenB);

        //
        //  If c4Where is the 3rd-last run now, also slide over
        //  the last run (which is not involved in this merge).
        //  The current run (c4Where + 1) goes away in this case.
        //
        if (c4Where == (c4InPendCnt - 3))
            State.Move(c4Where + 1, c4Where + 2);
        State.DecCount();

        //
        // Find where the first element of run2 goes in run1. Prior elements
        //  in run1 can be ignored (because they are already in place).
        //
        i4K = i4GallopRight
        (
            State.tToSortAt(i4BaseB)
            , State.ptToSort()
            , i4BaseA
            , i4LenA
            , 0
            , pfnComp
        );
        CIDAssert(i4K >= 0, L"Bad gallop right result");

        i4BaseA += i4K;
        i4LenA  -= i4K;
        if (i4LenA == 0)
            return;

        //
        // Find where the last element of run1 goes in run2.
        // Subsequent elements in run2 can be ignored
        // (because they are already in place).
        //
        i4LenB = i4GallopLeft
        (
            State.tToSortAt(i4BaseA + i4LenA - 1)
            , State.ptToSort()
            , i4BaseB
            , i4LenB
            , i4LenB - 1
            , pfnComp
        );
        CIDAssert(i4LenB >= 0, L"Bad gallop left result");

        if (i4LenB == 0)
            return;

        if (i4LenA <= i4LenB)
            MergeLow(State, i4BaseA, i4LenA, i4BaseB, i4LenB, pfnComp);
         else
            MergeHigh(State, i4BaseA, i4LenA, i4BaseB, i4LenB, pfnComp);
    }


    //
    //  Examines the stack of runs waiting to be merged and merges adjacent
    //  runs until the stack invariants are reestablished:
    //
    //     1. PendingRun[i - 3].mLen > PendingRun[i - 2].mLen + PendingRun[i - 1].mLen
    //     2. PendingRun[i - 2].mLen > PendingRun[i - 1].mLen
    //
    //  This method is called each time a new run is pushed onto the stack,
    //  so the invariants are guaranteed to hold for i < PendingRunCnt upon
    //  entry to the method.
    //
    //  [UPDATE 10/25/2017] Updated the per-loop check below, which had been found to
    //  be insufficient for some types of inputs and could cause the 85 entry pending
    //  stack to be overrun.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid MergeCollapse(TSortState<T>&  State, TCompFunc pfnComp)
    {
        tCIDLib::TCard4  c4N = 0;

        while (State.c4PendingCnt() > 1)
        {
            c4N = State.c4PendingCnt() - 2;
            if (((c4N >= 1) && (State.c4LenAt(c4N - 1) <= State.c4LenAt(c4N) + State.c4LenAt(c4N + 1)))
            ||  ((c4N >= 2) && (State.c4LenAt(c4N - 2) <= State.c4LenAt(c4N) + State.c4LenAt(c4N - 1))))
            {
                if (State.c4LenAt(c4N - 1) < State.c4LenAt(c4N + 1))
                    c4N--;

                MergeAt(State, c4N, pfnComp);
            }
             else if (State.c4LenAt(c4N) <= State.c4LenAt(c4N + 1))
            {
                MergeAt(State, c4N, pfnComp);
            }
             else
            {
                break;
            }
        }
    }

    //
    //  Merges all runs on the stack until only one remains.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid MergeForceCollapse(TSortState<T>&  State, TCompFunc pfnComp)
    {
        tCIDLib::TInt4  i4N = 0;

        while (State.c4PendingCnt() > 1)
        {
            i4N = State.c4PendingCnt() - 2;
            if ((i4N > 0) && (State.c4LenAt(i4N - 1) < State.c4LenAt(i4N + 1)))
                i4N--;

            MergeAt(State, (tCIDLib::TCard4)i4N, pfnComp);
        }
    }


    //
    //  The top level sort entry point. Sorts c4Count elements of the
    //  passed array, using the passed comparator function.
    //
    template <typename T, typename TCompFunc>
    tCIDLib::TVoid DoTSort(         T*              ptArray
                            , const tCIDLib::TCard4 c4Count
                            ,       TCompFunc       pfnComp)
    {
        TSortState<T> State(ptArray, c4Count);

        tCIDLib::TInt4  i4IndexLow  = 0;
        tCIDLib::TInt4  i4IndexHigh = c4Count;
        tCIDLib::TInt4  i4Remaining = c4Count;
        tCIDLib::TInt4  i4MinRunLen = 0;
        tCIDLib::TInt4  i4RunLen = 0;
        tCIDLib::TInt4  i4ForcedRunLen = 0;

        CIDAssert(c4Count <= 0x7fffffff, L"Array is too large to sort");

        // Arrays of size 1 are always sorted.
        if (i4Remaining < 2)
            return;

        i4MinRunLen = c4CalcMinRunLen(c4Count);
        i4Remaining = c4Count;

        do
        {
            i4RunLen = c4CountRunAndMakeAscending
            (
                ptArray, i4IndexLow, i4IndexHigh, pfnComp
            );

            if (i4RunLen < i4MinRunLen)
            {
                i4ForcedRunLen = i4Remaining <= i4MinRunLen ? i4Remaining : i4MinRunLen;

                //
                //  From i4IndexLow to i4IndexLow + i4RunLen - 1 is already
                //  sorted. So we need to start the binary sort from
                //  i4IndexLow + i4RunLen.
                //
                BinarySort
                (
                    ptArray
                    , i4IndexLow
                    , i4IndexLow + i4ForcedRunLen
                    , i4IndexLow + i4RunLen
                    , pfnComp
                );

                i4RunLen = i4ForcedRunLen;
            }

            // Push this run onto pending-runs stack, and maybe merge
            State.c4Push(i4IndexLow, i4RunLen);
            MergeCollapse(State, pfnComp);

            // Advance to find next run
            i4IndexLow  += i4RunLen;
            i4Remaining -= i4RunLen;

        }   while (i4Remaining != 0);

        CIDAssert(i4IndexLow == i4IndexHigh, L"Failed to sort whole array");

        // Merge all remaining runs to complete sort
        MergeForceCollapse(State, pfnComp);

        CIDAssert(State.c4PendingCnt() == 1, L"Failed to clear pending run stack");
    }


    //
    //  These are the actual sort calls to sort by value arrays, one is just
    //  a convenience to sort sub-files within an array. And then we have
    //  a set for doing by ref arrays.
    //
    template <typename T, typename TComp = tCIDLib::TDefMagComp<T>>
    tCIDLib::TVoid TSort(       T*                  ptArray
                        , const tCIDLib::TCard4     c4Count
                        ,       TComp               pfnComp = TComp())
    {
        DoTSort<T,TComp>(ptArray, c4Count, pfnComp);
    }

    template <typename T, typename TComp = tCIDLib::TDefMagComp<T>>
    tCIDLib::TVoid TSortSubFile(        T*                  ptArray
                                , const tCIDLib::TCard4     c4Start
                                , const tCIDLib::TCard4     c4Count
                                ,       TComp               pfnComp = TComp())
    {
        DoTSort<T,TComp>(&ptArray[c4Start], c4Count, pfnComp);
    }


    //
    //  A simple template method for binary searching a sorted array. If the
    //  element was found, the return is kCIDLib::True and the index of
    //  the element is in c4Index.
    //
    template <typename T, typename TComp = tCIDLib::TDefMagComp<T>> tCIDLib::TBoolean
    bBinarySearch(  const   T* const            ptElems
                    , const T                   tToFind
                    ,       tCIDLib::TCard4&    c4Index
                    , const tCIDLib::TCard4     c4ElemCount
                    ,       TComp               pfnComp = TComp())
    {
        // Set up the two end points that are used to subdivide the list
        tCIDLib::TInt4 i4End = tCIDLib::TInt4(c4ElemCount) - 1;
        tCIDLib::TInt4 i4Begin = 0;

        while (i4Begin <= i4End)
        {
            // Divide the current range
            tCIDLib::TInt4 i4MidPoint = (i4Begin + i4End) / 2;

            // Check this guy
            const tCIDLib::ESortComps eRes = pfnComp(tToFind, ptElems[i4MidPoint]);
            if (eRes == tCIDLib::ESortComps::Equal)
            {
                c4Index = tCIDLib::TCard4(i4MidPoint);
                return kCIDLib::True;
            }

            // Didn't find it, so see which way to go and adjust begin/end
            if (eRes == tCIDLib::ESortComps::FirstLess)
                i4End = i4MidPoint - 1;
            else
                i4Begin = i4MidPoint + 1;
        }
        return kCIDLib::False;
    }


    //
    //  This is another form of binary search that is useful when you are
    //  both searching for a value and, if not found, need to insert a new
    //  one at the correct spot. So this one returns more info that allows
    //  the caller to do the insert if no match is made. If the return is
    //  false, then c4Index is where you should insert the new one. If it's
    //  true, then that's where the match was found.
    //
    template <typename T, typename TComp = tCIDLib::TDefMagComp<T>>
    tCIDLib::TBoolean bBinarySearch2(const  T* const            ptElems
                                    , const T&                  tToFind
                                    ,       tCIDLib::TCard4&    c4Index
                                    , const tCIDLib::TCard4     c4ElemCount
                                    ,       TComp               pfnComp = TComp())
    {
        // Some special case checks to make the logic below safer
        tCIDLib::ESortComps eRes = tCIDLib::ESortComps::Equal;
        if (!c4ElemCount)
        {
            // It's empty so the new insert point is obviously 0
            c4Index = 0;
            return kCIDLib::False;
        }
         else if (c4ElemCount == 1)
        {
            // There's only one so we are either above or below or equal
            eRes = pfnComp(tToFind, ptElems[0]);

            if (eRes == tCIDLib::ESortComps::Equal)
            {
                // If equal, then we found it, return 0 as the match point
                c4Index = 0;
                return kCIDLib::True;
            }

            // We didn't find it, so insert before or after
            if (eRes == tCIDLib::ESortComps::FirstLess)
                c4Index = 0;
            else
                c4Index = 1;
            return kCIDLib::False;
        }

        //
        //  More than one, so let's do the binary search. So set up the
        //  two end points that are used to subdivide the list.
        //
        tCIDLib::TInt4 i4End = tCIDLib::TInt4(c4ElemCount) - 1;
        tCIDLib::TInt4 i4Begin = 0;
        tCIDLib::TInt4 i4MidPoint = 0;
        while (i4Begin <= i4End)
        {
            // Divide the current range
            i4MidPoint = (i4Begin + i4End) / 2;

            // Check this guy
            eRes = pfnComp(tToFind, ptElems[i4MidPoint]);

            if (eRes == tCIDLib::ESortComps::Equal)
            {
                // We found it so return the mid point as the match
                c4Index = tCIDLib::TCard4(i4MidPoint);
                return kCIDLib::True;
            }

            // Didn't find it, so see which way to go and adjust begin/end
            if (eRes == tCIDLib::ESortComps::FirstLess)
                i4End = i4MidPoint - 1;
            else
                i4Begin = i4MidPoint + 1;
        }

        // We never found it so return the insert point
        if (i4End < 0)
        {
            // We went all the way past the start, so put it at zero
            c4Index = 0;
        }
         else if (tCIDLib::TCard4(i4Begin) >= c4ElemCount)
        {
            // We went all the way past the end, so put it at the end
            c4Index = c4ElemCount;
        }
         else
        {
            // Else either insert before or after where we break out
            switch(eRes)
            {
                case tCIDLib::ESortComps::Equal :
                case tCIDLib::ESortComps::FirstLess :
                    c4Index = tCIDLib::TCard4(i4MidPoint);
                    break;

                default :
                    c4Index = tCIDLib::TCard4(i4MidPoint + 1);
                    break;
            };
        }
        return kCIDLib::False;
    }


    template <typename TElem, typename TKey, typename TComp  = tCIDLib::TDefMagComp<TElem>>
    tCIDLib::TBoolean bSearch(  const   TElem* const        ptElems
                                , const TKey                tToFind
                                ,       tCIDLib::TCard4&    c4Index
                                , const tCIDLib::TCard4     c4ElemCount
                                ,       TComp               pfnComp = TComp())
    {
        // Set up the two end points that are used to subdivide the list
        tCIDLib::TInt4 i4End = tCIDLib::TInt4(c4ElemCount) - 1;
        tCIDLib::TInt4 i4Begin = 0;

        while (i4Begin <= i4End)
        {
            // Divide the current range
            tCIDLib::TInt4 i4MidPoint = (i4Begin + i4End) / 2;

            // Check this guy, and return it if this is the one
            const tCIDLib::ESortComps eRes = pfnComp(tToFind, ptElems[i4MidPoint]);
            if (eRes == tCIDLib::ESortComps::Equal)
            {
                c4Index = tCIDLib::TCard4(i4MidPoint);
                return kCIDLib::True;
            }

            // Didn't find it, so see which way to go and adjust begin/end
            if (eRes == tCIDLib::ESortComps::FirstLess)
                i4End = i4MidPoint - 1;
            else
                i4Begin = i4MidPoint + 1;
        }
        return kCIDLib::False;
    }
}
