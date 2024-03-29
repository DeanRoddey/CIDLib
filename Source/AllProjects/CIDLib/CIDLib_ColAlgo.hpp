//
// FILE NAME: CIDLib_ColAlgo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/22/2019
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
//  Although we in no way want to get crazy like the C++ STL stuff, there are
//  some basic 'collection algorithms' we can do as templates that show up an
//  awful lot, and so it's worth providing generic ones where performance is
//  not particularly important.
//
//  Obviously these guys have to work in terms of cursors, so they will never be
//  as performant as just doing these things directly on the collections.
//
//  The ones that return a cursor have to get a specific collection type, i.e.
//  a concrete collection class that defines a TCursor/TNCCursor, because it has
//  to return the cursor by value.
//
//  The others will generally take a TCollection<T> or TRefCollection<T> and can
//  work on any type of collection of that sort (by value or by reference.)
//
//
//  We also define here some templates for accessing raw arrays, since there's
//  not really a better place for them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)


namespace tCIDColAlgo
{
    //
    //  Find the first element that matches the passed object to find. If found the
    //  cursor will be pointed at that element, else invalid. The default equality
    //  comparison functor will be used if not explicitly provided, which requires
    //  that the elements provide the equality operator. We have const and non-const
    //  versions.
    //
    //  These are first becasue some others below can make use of these.
    //
        template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    constexpr typename TCol::TCursor cursFind(  const   TCol&       colSrc
                                                , const TElem&      objToFind
                                                ,       TComp       pfnComp = TComp())
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    constexpr typename TCol::TNCCursor cursFindNC(          TCol&       colSrc
                                                    , const TElem&      objToFind
                                                    ,       TComp       pfnComp = TComp())
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }



    //
    //  If the passed object is not in the collection already, add it. Some
    //  collections have such things built in, like has sets. But, for other
    //  types, we have to just search it. This assumes it's not sorted already
    //  so we have to search the whole thing. Returns true if it was added.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    tCIDLib::TBoolean bAddIfNew(        TCol&   colTar
                                , const TElem&  objToAdd
                                ,       TComp   pfnComp = TComp())
    {
        typename TCol::TCursor cursTar(&colTar);
        for (; cursTar; ++cursTar)
        {
            // If we find a match, return false, didn't add it
            if (pfnComp(*cursTar, objToAdd))
                return kCIDLib::False;
        }

        // Never found it, so add it
        colTar.objAdd(objToAdd);
        return kCIDLib::True;
    }


    //
    //  Returns true if the passed element is in the passed container. This assumes they are
    //  not sorted so we have to do an exhaustive search until we find one or fail. If the
    //  container is sorted use the binary search options provided by the collection itself
    //  (those that can support such things will provide one, they have to indexable.)
    //
    template<typename  TCol
            , typename TElem = TCol::TMyElemType
            , typename TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    constexpr tCIDLib::TBoolean
    bContains(const TCol& colContains, const TElem& tFind, TComp pfnComp = TComp())
    {
        typename TCol::TCursor cursContains(&colContains);
        for (; cursContains; ++cursContains)
        {
            if (pfnComp(*cursContains, tFind))
                return kCIDLib::True;
        }

        // Never found a match
        return kCIDLib::False;
    }


    //
    //  Returns true if all of the elements of colTest are in conContains. This assumes that
    //  that the elements of conContains are not sorted, so this can be quite slow for large
    //  lists. We can't do better than just search, though we can stop as soon as we get a
    //  failure.
    //
    template<typename  TCol
            , typename TElem = TCol::TMyElemType
            , typename TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    constexpr tCIDLib::TBoolean
    bContainsAll(const TCol& colContains, const TCol& colTest, TComp pfnComp = TComp())
    {
        typename TCol::TCursor cursTest(&colTest);
        typename TCol::TCursor cursContains(&colContains);
        for (; cursTest; ++cursTest)
        {
            cursContains.bReset();
            for (; cursContains; ++cursContains)
            {
                if (pfnComp(*cursContains, *cursTest))
                    break;
            }

            // If we got to the end of contains, then we don't have this one
            if (!cursContains)
                return kCIDLib::False;
        }

        // Never failed to find a match
        return kCIDLib::True;
    }


    //
    //  Remove elements that the passed test indicates should be removed.
    //
    //  This only works for a collection that has a RemoveAt(cursor) method, but
    //  most do.
    //
    template<typename  TCol, typename TTest, typename  TElem = TCol::TMyElemType>
    tCIDLib::TBoolean bRemoveIf(TCol& colTar, TTest pfnTest)
    {
        tCIDLib::TBoolean bRet = kCIDLib::False;
        typename TCol::TCursor cursTar(&colTar);
        while (cursTar)
        {
            // Test this one, if a match remove it, else move forward
            if (pfnTest(*cursTar))
            {
                colTar.RemoveAt(cursTar);
                bRet = kCIDLib::True;
            }
             else
            {
                ++cursTar;
            }
        }
        return bRet;
    }


    //
    //  If the passed object is in the collection, remove it. It can remove just
    //  the first match, or all. Some collections have built in mechanisms for
    //  this kind of stuff, but this can be used for those that dont. Returns
    //  true if it removed any.
    //
    //  This only works for a collection that has a RemoveAt(cursor) method, but
    //  most do.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    tCIDLib::TBoolean bRemoveMatches(       TCol&               colTar
                                    , const TElem&              objToRem
                                    , const tCIDLib::TBoolean   bRemoveAll
                                    ,       TComp               pfnComp = TComp())
    {
        tCIDLib::TBoolean bRet = kCIDLib::False;
        typename TCol::TCursor cursTar(&colTar);
        while (cursTar)
        {
            // If we find a match, remove it
            if (pfnComp(*cursTar, objToRem))
            {
                colTar.RemoveAt(cursTar);
                bRet = kCIDLib::True;

                // If not removing all, then we are done
                if (!bRemoveAll)
                    break;
            }
             else
            {
                // No match, so move forward
                ++cursTar;
            }
        }
        return bRet;
    }


    //
    //  Returns the number of times that the indicted find element exists in the contains
    //  collection. We don't assume that the list is sorted, so we have to search the whole
    //  thing.
    //
    template<typename  TCol
            , typename TElem = TCol::TMyElemType
            , typename TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    constexpr tCIDLib::TCard4 c4Count(const TCol& colContains, const TElem& tFind, TComp pfnComp = TComp())
    {
        typename TCol::TCursor cursContains(&colContains);
        tCIDLib::TCard4 c4Ret = 0;
        for (; cursContains; ++cursContains)
        {
            if (pfnComp(*cursContains, tFind))
                c4Ret++;
        }
        return c4Ret;
    }


    //
    //  Given a collection and an initial value, call += for each value in the
    //  collection, adding them to the incoming value, which we return as the
    //  new value.
    //
    template <typename TCol, typename TElem = TCol::TMyElemType>
    TElem tAccumulate(const TCol& colSrc, const TElem& tInitVal)
    {
        TElem retVal = tInitVal;
        try
        {
            colSrc.bForEach
            (
                [&retVal](const TElem& tCur) -> tCIDLib::TBoolean
                {
                    retVal += tCur;
                    return kCIDLib::True;
                }
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        return retVal;
    }


    //
    //  Does a map operation. It will return a non-adopting by ref vector of the elements
    //  that pfnTest say should be kept. We return it by value, but that's ok since it's
    //  cheaply moveable. Of course the caller must not keep these references around beyond
    //  the life of the source collection.
    //
    //  We have a const one and a non-const one. So the const one takes a const collection
    //  and returns a vector of const references, and vice versa.
    //
    template<typename  TCol, typename TTest>
    typename TRefVector<const typename TCol::TMyElemType>
    colMap(const TCol& colSrc, TTest pfnTest, const tCIDLib::TCard4 c4InitAlloc = 0)
    {
        TRefVector<const typename TCol::TMyElemType> colKept
        (
            tCIDLib::EAdoptOpts::NoAdopt, (c4InitAlloc == 0) ? colSrc.c4ElemCount() / 4 : c4InitAlloc
        );
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnTest(*cursSrc))
            {
                if (colKept.c4ElemCount() == colKept.c4CurAlloc())
                {
                    const tCIDLib::TCard4 c4Exp = colKept.c4ElemCount() / 4;
                    if (colKept.c4ElemCount() + c4Exp > colSrc.c4ElemCount())
                        colKept.CheckExpansion(colSrc.c4ElemCount());
                    else
                        colKept.CheckExpansion(c4Exp);
                }
                colKept.Add(&cursSrc.objRCur());
            }
        }
        return colKept;
    }

    template<typename  TCol, typename TTest>
    typename TRefVector<typename TCol::TMyElemType>
    colMapNC(TCol& colSrc, TTest pfnTest, const tCIDLib::TCard4 c4InitAlloc = 0)
    {
        TRefVector<typename TCol::TMyElemType> colKept
        (
            tCIDLib::EAdoptOpts::NoAdopt, (c4InitAlloc == 0) ? colSrc.c4ElemCount() / 4 : c4InitAlloc
        );
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnTest(*cursSrc))
            {
                if (colKept.c4ElemCount() == colKept.c4CurAlloc())
                {
                    const tCIDLib::TCard4 c4Exp = colKept.c4ElemCount() / 4;
                    if (colKept.c4ElemCount() + c4Exp > colSrc.c4ElemCount())
                        colKept.CheckExpansion(colSrc.c4ElemCount());
                    else
                        colKept.CheckExpansion(c4Exp);
                }
                colKept.Add(&cursSrc.objWCur());
            }
        }
        return colKept;
    }


    //
    //  Does a map and reduce operation. It finds all of the elements that the test selects,
    //  and applies the reduce callback on them. We start with the passed initial value. This
    //  one doesn't need to allocate anything new, it's operating directly on the source
    //  collection.
    //
    template<typename  TCol, typename TTest, typename Reduce>
    typename TCol::TMyElemType
    tMapReduce( const   TCol&                           colSrc
                ,       TTest                           pfnTest
                ,       Reduce                          pfnReduce
                , const typename TCol::TMyElemType&     tInitVal
                , const tCIDLib::TCard4                 c4InitAlloc = 0)
    {
        TRefVector<const typename TCol::TMyElemType> colKept
        (
            tCIDLib::EAdoptOpts::NoAdopt, (c4InitAlloc == 0) ? colSrc.c4ElemCount() / 4 : c4InitAlloc
        );
        typename TCol::TCursor cursSrc(&colSrc);
        typename TCol::TMyElemType tRet = tInitVal;
        for (; cursSrc; ++cursSrc)
        {
            if (pfnTest(*cursSrc))
                pfnReduce(tRet, *cursSrc);
        }
        return tRet;
    }


    //
    //  Find the first element that is not equal to the passed object to find. If
    //  found the cursor will be pointed at that element, else invalid. The default
    //  equality comparison functor will be used if not explicitly provided, which
    //  requires that the elements provide the equality operator. We have const
    //  and non-const versions
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFindNot( const   TCol&   colSrc
                                        , const TElem&  objToFind
                                        ,       TComp   pfnComp = TComp())
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TNCCursor cursFindNotNC(         TCol&   colSrc
                                            , const TElem&  objToFind
                                            ,       TComp   pfnComp = TComp())
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element that matches, by a key value, which the comparator parameter
    //  knows how to access. And then find the first one that doesn't match. We have const
    //  and non-const versions.
    //
    template<typename   TCol
            , typename  TKey
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    typename TCol::TCursor cursFindByKey(const  TCol&   colSrc
                                        , const TKey&   objKey
                                        ,       TComp   pfnComp)
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objKey))
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TKey
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    typename TCol::TCursor cursFindNotByKey(const   TCol&   colSrc
                                            , const TKey&   objKey
                                            ,       TComp   pfnComp)
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objKey))
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TKey
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    typename TCol::TNCCursor cursFindByKeyNC(       TCol&   colSrc
                                            , const TKey&   objKey
                                            ,       TComp   pfnComp)
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objKey))
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TKey
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>>
    typename TCol::TNCCursor cursFindNotByKeyNC(        TCol&   colSrc
                                                , const TKey&   objKey
                                                ,       TComp   pfnComp)
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objKey))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element greater or less than the passed object to compare. The
    //  default magnitude comparison functor is used if not indicated explicitly (in
    //  which case the element type must provide < and > operators.) We have consta nd
    //  non-const versions.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefMagComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFirstGreater(const   TCol&   colSrc
                                            , const TElem&  objComp
                                            ,       TComp   pfnComp = TComp())
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstGreater)
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TComp = tCIDLib::TDefMagComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFirstLess(const  TCol&   colSrc
                                        , const TElem&  objComp
                                        ,       TComp   pfnComp = TComp())
    {
        typename TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstLess)
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TComp = tCIDLib::TDefMagComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TNCCursor cursFirstGreaterNC(        TCol&   colSrc
                                                , const TElem&  objComp
                                                ,       TComp   pfnComp = TComp())
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstGreater)
                break;
        }
        return cursSrc;
    }

    template<typename   TCol
            , typename  TComp = tCIDLib::TDefMagComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TNCCursor cursFirstLessNC(       TCol&   colSrc
                                            , const TElem&  objComp
                                            ,       TComp   pfnComp = TComp())
    {
        typename TCol::TNCCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstLess)
                break;
        }
        return cursSrc;
    }


    //
    //  This method will copy the elements from the source collection to the
    //  target collection. It is assumed that the caller flushed the target
    //  collection if that was desirable or required.
    //
    //  This version works only with 'by value' collections.
    //
    //  If you are dealing with two collections of the same type (and you know
    //  if you are), then use the assignment or copy methods since they will
    //  be a good bit more efficient, and will fully recreate the source
    //  collection. This one cannot do that and will only copy the elements
    //  of one collection into another. Since it does not know what type of
    //  collections it is working on, it cannot do things like update the hash
    //  modulus of a hashed collection.
    //
    //  So this will of course not create functionaly identical collections as the
    //  copy constructor or assignment operator would. But its advantage is that
    //  its can work across collections of different types.
    //
    template <typename T> tCIDLib::TVoid
    CopyElems(TCollection<T>& colDest, const TCollection<T>& colSource)
    {
        // Check for copy to self
        if (&colDest == &colSource)
            return;

        // Lock both collections
        TLocker lockrNew(&colDest);
        TLocker lockrToDup(&colSource);

        // Nothing to do if no elements!
        if (!colSource.c4ElemCount())
            return;

        TColCursor<T>* pcursSrc = colSource.pcursNew();
        TJanitor<TColCursor<T>> janCursor(pcursSrc);
        try
        {
            do
            {
                colDest.objAdd(pcursSrc->objRCur());
            }   while (pcursSrc->bNext());
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }


    //
    //  Remove's consequtive dups and returns the number of them that were
    //  removed. It must support a RemoveAt(curs) type method.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    tCIDLib::TCard4 c4RemoveSeqDups(TCol& colSrc, TComp pfnComp = TComp())
    {
        if (colSrc.bIsEmpty())
            return 0;

        tCIDLib::TCard4 c4RemCount = 0;
        typename TCol::TNCCursor cursCur(&colSrc);
        TElem& objLast = *cursCur;
        ++cursCur;
        while (cursCur.bIsValid())
        {
            if (pfnComp(*cursCur, objLast))
            {
                colSrc.RemoveAt(cursCur);
                c4RemCount++;
            }
             else
            {
                objLast = *cursCur;
                ++cursCur;
            }
        }
        return c4RemCount;
    }


    //
    //  Given an indexable fundamental collection (vector or array) that is sorted,
    //  this guy will find the value that has the most dups. If not sorted it will
    //  not collect them all up, it's just looking for sequential dups.
    //
    template <typename TCol, typename TElem = TCol::TMyElemType>
    TElem tFindMaxFundSeqDup(const TCol& fcolSrc)
    {
        TElem tLastBest = TElem(0);
        const tCIDLib::TCard4 c4Count = fcolSrc.c4ElemCount();
        if (c4Count)
        {
            tCIDLib::TCard4 c4LastBestCount = 0;
            tCIDLib::TCard4 c4Index = 0;
            while (c4Index < c4Count)
            {
                const TElem tCur = fcolSrc[c4Index++];
                tCIDLib::TCard4 c4RunCount = 1;
                while (c4Index < c4Count)
                {
                    // If not the same, we are done for this one, else bump values
                    const TElem tNext = fcolSrc[c4Index];
                    if (tNext != tCur)
                        break;

                    c4RunCount++;
                    c4Index++;
                }

                // If this count is better than the last, then take this one
                if (c4RunCount > c4LastBestCount)
                {
                    c4LastBestCount = c4RunCount;
                    tLastBest = tCur;
                }
            }
        }
        return tLastBest;
    }
}

namespace tCIDLib
{
    // Access a raw array safely
    template <typename T, tCIDLib::TCard4 c4Size>
    T& c4ArrayAt(T(& aSrc)[c4Size], const tCIDLib::TCard4 c4At)
    {
        if (c4At >= c4Size)
        {
            ThrowArrayIndexErr(c4At, c4Size);

            // Won't happen but make the analyzer happy
            return *static_cast<T*>(nullptr);
        }
        return aSrc[c4At];
    }

    template <typename T, tCIDLib::TCard4 c4Size>
    const T& c4ArrayAt(const T(& aSrc)[c4Size], const tCIDLib::TCard4 c4At)
    {
        if (c4At >= c4Size)
        {
            ThrowArrayIndexErr(c4At, c4Size);

            // Won't happen but make the analyzer happy
            return *static_cast<T*>(nullptr);
        }
        return aSrc[c4At];
    }


    //
    //  Get a pointer to the element past the end, for use with pointer
    //  increment type loops.
    //
    template <typename T, tCIDLib::TCard4 c4Size> T* pArrayEndPtr(T(& aSrc)[c4Size])
    {
        return &aSrc[c4Size];
    }

    template <typename T, tCIDLib::TCard4 c4Size>
    const T* pArrayEndPtr(const T(& aSrc)[c4Size])
    {
        return &aSrc[c4Size];
    }
}

#pragma CIDLIB_POPPACK
