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
    //  Given an accumulator type T, and a collection of those, call += on each value
    //  in the collection and return the result. We don't know what += means in this
    //  case, we just call it. They have to provide us with an initial value to start
    //  with.
    //
    template <typename T> T tAccumulate(const TCollection<T>& colSrc, const T& tInitVal)
    {
        T retVal = tInitVal;
        TColCursor<T>* pcursIter = colSrc.pcursNew();
        TJanitor<TColCursor<T>> janCursor(pcursIter);

        try
        {
            while (pcursIter->bIsValid())
            {
                retVal += pcursIter->objRCur();
                pcursIter->bNext();
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        return retVal;
    }


    //
    //  Find the first element that matches the passed object to find. If found the
    //  cursor will be pointed at that element, else invalid. The default equality
    //  comparison functor will be used if not explicitly provided, which requires
    //  that the elements provide the equality operator.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFind(const   TCol&       colSrc
                                    , const TElem&      objToFind
                                    ,       TComp       pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element that is not equal to the passed object to find. If
    //  found the cursor will be pointed at that element, else invalid. The default
    //  equality comparison functor will be used if not explicitly provided, which
    //  requires that the elements provide the equality operator.
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefEqComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFindNot( const   TCol&   colSrc
                                        , const TElem&  objToFind
                                        ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element greater or less than the passed object to compare. The
    //  default magnitude comparison functor is used if not indicated explicitly (in
    //  which case the element type must provide < and > operators.)
    //
    template<typename   TCol
            , typename  TComp = tCIDLib::TDefMagComp<typename TCol::TMyElemType>
            , typename  TElem = TCol::TMyElemType>
    typename TCol::TCursor cursFirstGreater(const   TCol&   colSrc
                                            , const TElem&  objComp
                                            ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
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
        TCol::TCursor cursSrc(&colSrc);
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
        TMtxLocker lockNew(colDest.pmtxLock());
        TMtxLocker lockToDup(colSource.pmtxLock());

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
        TCol::TNCCursor cursCur(&colSrc);
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

#pragma CIDLIB_POPPACK
