//
// FILE NAME: CIDLib_CollectionUtils.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/1998
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
//  This header defines some 'collection utilities'. These are templates that can
//  be applied to any sorts of (non-fundamental) collections. They work via cursors
//  so they don't care what the collection type is, though of course it has to be
//  somehing such that the elements are syntactically correct for the helper.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


namespace tColUtils
{
    //
    //  This method will copy the elements from the source collection to the
    //  target collection. It is assumed that the caller flushed the target
    //  collection if that was desirable or required.
    //
    //  This version works only with 'by value' collections. There is another
    //  version for 'by reference' collections.
    //
    //  If you are dealing with two collections of the same type (and you know
    //  you are), then use the assignment operator or copy constructor since they
    //  will be a good bit more efficient, and will fully recreate the source
    //  collection. This one cannot do that and will only copy the elements
    //  of one collection into another. Since it does not know what type of
    //  collections it is working on, it cannot do things like update the hash
    //  modulus of a hashed collection, or update the maximum element count to
    //  match the source collection. So it will fail if there are too many
    //  elements for the destination and it will use the existing hash modulus
    //  of the source when it adds the new elements to it.
    //
    //  This will of course not create functionaly identical collections as the
    //  copy constructor or assignment operator would. But its advantage is that
    //  its can work across collections of different types.
    //
    template <class TElem> tCIDLib::TVoid
    CopyElems(TCollection<TElem>& colDest, const TCollection<TElem>& colSource)
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

        TColCursor<TElem>* pcursSrc = colSource.pcursNew();
        TJanitor<TColCursor<TElem> > janCursor(pcursSrc);
        do
        {
            colDest.objAdd(pcursSrc->objCur());
        }   while (pcursSrc->bNext());
    }


    //
    //  This function will duplicate a 'by reference' collection of objects. The
    //  base class must define the virtual pobjDuplicate() method, and of course
    //  any of the derived classes must implement it. Any classes which implement
    //  the standard MDuplicable interface will have this support.
    //
    //  By reference collections have no copy or assigment built into them,
    //  precisely because it would force this functionality on all classes that
    //  would ever be put into such a collection.
    //
    template <class TCol> TCol* pDupRefCollect(const TCol& colSrc)
    {
        //
        //  This will Create a new collection of the same type, with the same basic
        //  state, but without any content. Put a janitor on it in case we hose
        //  during the process of duplicating it.
        //
        typename TCol* pcolRet = colSrc.pcolMakeNewOf();
        TJanitor<TCol> janRet(pcolRet);

        //
        //  Get a cursor for the passed collection. We'll use this to iterate it
        //  and get each node to duplicate.
        //
        TCol::TCursor cursSrc(&colSrc);
        while (cursSrc.bIsValid())
        {
            pcolRet->Add(cursSrc.objRCur().pobjDuplicate());
            ++cursSrc;
        }

        // Orphan the collection from the janitor and return it
        return janRet.pobjOrphan();
    }


    //
    //  This function will iterate any collection and do a callback on each of
    //  the elements in the collection. This is a convenient way to do something
    //  for each collection element.
    //
    //  These use a templatized callback that will accept either a function pointer
    //  or a lambda expression.
    //
    template <typename IterCB, typename TCol>
    tCIDLib::TVoid ForEach(IterCB iterCB, const TCol& colToIterate)
    {
        TCol::TCursor cursCB(&colToIterate);
        while (cursCB.bIsValid())
        {
            // Break out if the callback returns kCIDLib::False
            if (!iterCB(cursCB.objRCur()))
                break;
            ++cursCB;
        }
    }


    template <typename IterCB, typename TCol>
    tCIDLib::TVoid ForEachNC(IterCB iterCB, TCol& colToIterate)
    {
        TCol::TNCCursor cursCB(&colToIterate);
        if (!cursCB.bReset())
            return;

        do
        {
            // Break out if the callback returns kCIDLib::False
            if (!iterCB(cursCB.objWCur()))
                break;
        }   while (cursCB.bNext());
    }


    //
    //  This is a siple algorithm that will accumulate the members of the passed
    //  collection. This may or may not be be a numeric addition. It just depends
    //  on what operator+ is for type T.
    //
    template <typename T, T tInitVal = 0> T tAccumulate(const TCollection<T>& colSrc)
    {
        T retVal = tInitVal;
        TColCursor<T>* pcursIter = colSrc.pcursNew();
        try
        {
            while (pcursIter->bIsValid)
            {
                retVal += pcursIter->objRCur();
                pcursIter->bNext();
            }
            delete pcursIter;
        }

        catch(...)
        {
            delete pcursIter;
            throw;
        }
        return retVal;
    }
}

