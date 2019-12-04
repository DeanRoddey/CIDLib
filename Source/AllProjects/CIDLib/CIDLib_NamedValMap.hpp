//
// FILE NAME: CIDLib_NamedValMapClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/01/2004
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
//  This is the header file for the CIDLib_NamedValMapClass.cpp file,
//  which implements a class that contains a list of named items, and each
//  named item contains one or more key/value pairs. So, a particular value
//  in the map is addressed by the key of the item and the sub-key of the
//  value within that item.
//
//  So the hierarchy is:
//
//      Item1 (which has a key)
//          SubKey1 - Value1
//              .
//              .
//          SubKeyX - ValueX
//              .
//              .
//              .
//              .
//      ItemX (which has a key)
//          SubKey1 - Value1
//              .
//              .
//          SubKeyX - ValueX
//
//  We have a small item value class that holds a single item (the key and
//  the list of sub-keyed items under it), and then the main value class that
//  holds a list of these.
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
//  CLASS: TNamedValMap
// PREFIX: nvm
// ---------------------------------------------------------------------------
template <class TVal> class TNamedValMap : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        //
        //  TPair is a specializeation of the TKeyObjPair template class which
        //  stores a string key and whatever our value class is as the value
        //  of a key/value pair.
        //
        //  TNVMItem is the internal class that we store the values for an
        //  item in. So it has a key for the overall item, and a keyed hash
        //  set for the underlying key/value pairs it holds.
        // -------------------------------------------------------------------
        using TMyType = TNamedValMap<TVal>;
        using TPair = TKeyObjPair<TString, TVal>;

        template <class TVal> class TNVMItem
        {
            public  :
                // -----------------------------------------------------------
                //  Class types
                //
                //  We need a key/object pair to hold the element type mapped
                //  to a key string.
                // -----------------------------------------------------------
                using TPairList = TKeyedHashSet<TPair,TString,TStringKeyOps>;


                // -----------------------------------------------------------
                //  Public, static methods
                // -----------------------------------------------------------
                static const TString& strKey(const TNVMItem& nviSrc)
                {
                    return nviSrc.m_strKey;
                }

                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TNVMItem() :

                    m_colPairs(29, TStringKeyOps(), &TPair::objExtractKey)
                {
                }

                TNVMItem(const TString& strKey) :

                    m_colPairs(29, TStringKeyOps(), &TPair::objExtractKey)
                    , m_strKey(strKey)
                {
                }

                TNVMItem(const TNVMItem& nviToCopy) :

                    m_colPairs(nviToCopy.m_colPairs)
                    , m_strKey(nviToCopy.m_strKey)
                {
                }

                ~TNVMItem() {}


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TNVMItem& operator=(const TNVMItem&) = delete;
                TNVMItem& operator=(TNVMItem&&) = delete;


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TBoolean bAddValue(const   TString&    strSubKey
                                            , const TVal&       objToSet)
                {
                    // If it already exists, then don't do it
                    const TPair* pkobjTar = m_colPairs.pobjFindByKey(strSubKey);
                    if (pkobjTar)
                        return kCIDLib::False;

                    // Else add a new one
                    m_colPairs.objAdd(TPair(strSubKey, objToSet));
                    return kCIDLib::True;
                }

                tCIDLib::TBoolean bIsEmpty() const
                {
                    return m_colPairs.bIsEmpty();
                }

                tCIDLib::TBoolean
                bQueryValue(const TString& strSubKey, TVal& objToFill) const
                {
                    // If this subkey doesn't exist, we fail
                    const TPair* pkobjTar = m_colPairs.pobjFindByKey(strSubKey);
                    if (!pkobjTar)
                        return kCIDLib::False;

                    objToFill = pkobjTar->objValue();
                    return kCIDLib::True;
                }

                tCIDLib::TBoolean bRemoveSubKey(const TString& strSubKey)
                {
                    return m_colPairs.bRemoveKey(strSubKey, kCIDLib::False);
                }

                tCIDLib::TBoolean bSubKeyExists(const TString& strSubKey) const
                {
                    return m_colPairs.bKeyExists(strSubKey);
                }

                tCIDLib::TBoolean bSetValue(const   TString&    strSubKey
                                            , const TVal&       objValToSet)
                {
                    // If the sub-key doesn't exist, we fail
                    TPair* pkobjTar = m_colPairs.pobjFindByKey(strSubKey);
                    if (!pkobjTar)
                        return kCIDLib::False;

                    pkobjTar->objValue(objValToSet);
                    return kCIDLib::True;
                }

                const TPairList& colPairs() const
                {
                    return m_colPairs;
                }

                TPairList& colPairs()
                {
                    return m_colPairs;
                }

                tCIDLib::TVoid Clear()
                {
                    m_colPairs.RemoveAll();
                }

                const TString& strKey() const
                {
                    return m_strKey;
                }

                const TString& strKey(const TString& strToSet)
                {
                    m_strKey = strToSet;
                    return m_strKey;
                }

                TVal* pobjValue(const TString& strSubKey)
                {
                    // If the sub-key doesn't exist, return nul for failiure
                    TPair* pkobjRet = m_colPairs.pobjFindByKey(strSubKey);
                    if (!pkobjRet)
                        return 0;
                    return &pkobjRet->objValue();
                }

                const TVal* pobjValue(const TString& strSubKey) const
                {
                    // If the sub-key doesn't exist, return null for failure
                    const TPair* pkobjRet = m_colPairs.pobjFindByKey(strSubKey);
                    if (!pkobjRet)
                        return 0;
                    return &pkobjRet->objValue();
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_colPairs
                //      The key/value pairs that make up the sub-keys of this
                //      item.
                //
                //  m_strKey
                //      The main key for this item.
                // -----------------------------------------------------------
                TPairList   m_colPairs;
                TString     m_strKey;
        };


        // -------------------------------------------------------------------
        //  We need a keyed hash set of the map elements above over the elem
        //  type, and define convenience types for cursor of our items or
        //  the values of an item.
        // -------------------------------------------------------------------
        using TElem         = TNVMItem<TVal>;
        using TItemList     = TKeyedHashSet<TElem, TString, TStringKeyOps>;
        using TValCursor    = typename TElem::TPairList::TCursor;
        using TNCValCursor  = typename TElem::TPairList::TNCCursor;
        using TItemCursor   = typename TItemList::TCursor;
        using TNCItemCursor = typename TItemList::TNCCursor;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNamedValMap() :

            m_colItems(29, TStringKeyOps(), &TNVMItem<TVal>::strKey)
        {
        }

        TNamedValMap(const TMyType& nvmSrc) :

            m_colItems(nvmSrc.m_colItems)
        {
        }

        ~TNamedValMap() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& nvmSrc)
        {
            if (this != &nvmSrc)
                m_colItems = nvmSrc.m_colItems;
            return *this;
        }

        tCIDLib::TBoolean operator==(const TNamedValMap<TVal>& nvmSrc) const
        {
            TItemList::TCursor cursThis(&m_colItems);
            TItemList::TCursor cursSrc(&nvmSrc.m_colItems);

            if (c4ElemCount() != nvmSrc.c4ElemCount())
                return kCIDLib::False;

            // If no elements in either, then they are equal
            if (!c4ElemCount())
                return kCIDLib::True;

            // They have the same count, so we can just cursor through them
            while (cursThis.bIsValid() && cursSrc.bIsValid())
            {
                const TElem& elemThis = cursThis.objRCur();
                const TElem& elemSrc = cursSrc.objRCur();

                // Compare keys
                if (elemThis.strKey() != elemSrc.strKey())
                    return kCIDLib::False;

                // Compare the sub-key collection
                if (elemThis.colPairs() != elemSrc.colPairs())
                    return kCIDLib::False;

                ++cursThis;
                ++cursSrc;
            }

            CIDAssert(!cursThis && !cursSrc, L"Both cursors should be invalid");
            return kCIDLib::True;
        }

        tCIDLib::TBoolean operator!=(const TNamedValMap<TVal>& nvmSrc) const
        {
            return !operator==(nvmSrc);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddItem(const TString& strKey)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (pnvmiTar)
                AlreadyExists(strKey, 0);
            m_colItems.objAdd(TNVMItem<TVal>(strKey));
        }

        tCIDLib::TVoid AddValue(const   TString&    strKey
                                , const TString&    strSubKey
                                , const TVal&       objToSet)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);

            if (!pnvmiTar->bAddValue(strSubKey, objToSet))
                AlreadyExists(strKey, &strSubKey);
        }

        tCIDLib::TVoid AppendFrom(const TNamedValMap<TVal>& nvmSrc)
        {
            m_colItems.AppendFrom(nvmSrc.m_colItems);
        }

        tCIDLib::TBoolean bKeyExists(const TString& strKey) const
        {
            return m_colItems.bKeyExists(strKey);
        }

        tCIDLib::TBoolean bIsEmpty() const
        {
            return m_colItems.bIsEmpty();
        }

        tCIDLib::TBoolean bItemIsEmpty(const TString& strKey) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (pnvmiTar)
                return pnvmiTar->bIsEmpty();

            // Make the compiler happy, this will actually throw
            NotFound(strKey, nullptr);
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bQueryValue(  const   TString&    strKey
                                        , const TString&    strSubKey
                                        ,       TVal&       objToFill) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                return kCIDLib::False;
            return pnvmiTar->bQueryValue(strSubKey, objToFill);
        }

        tCIDLib::TBoolean bSubKeyExists(const   TString&   strKey
                                        , const TString&   strSubKey) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                return kCIDLib::False;
            return pnvmiTar->bSubKeyExists(strSubKey);
        }

        tCIDLib::TCard4 c4ElemCount() const
        {
            return m_colItems.c4ElemCount();
        }

        tCIDLib::TVoid ClearItem(const TString& strKey)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);

            CIDLib_Suppress(6011)  // We null checked above
            pnvmiTar->Clear();
        }

        const TItemList& colItems() const
        {
            return m_colItems;
        }

        TValCursor cursItemVals(const TString& strKey) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);

            CIDLib_Suppress(6011)  // We null checked above
            return TValCursor(&pnvmiTar->colPairs());
        }

        TNCValCursor cursNCItemVals(const TString& strKey)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);

            CIDLib_Suppress(6011)  // We null checked above
            return TNCValCursor(&pnvmiTar->colPairs());
        }

        TItemCursor cursItems() const
        {
            return TItemCursor(&m_colItems);
        }

        TNCItemCursor cursNCItems()
        {
            return TNCItemCursor(&m_colItems);
        }

        const TVal& objValue(const TString& strKey, const TString& strSubKey) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);
            CIDLib_Suppress(6011)  // We null checked above
            const TVal* pvalRet = pnvmiTar->pobjValue(strSubKey);
            if (!pvalRet)
                NotFound(strKey, &strSubKey);
            CIDLib_Suppress(6011)  // We null checked above
            return *pvalRet;
        }

        tCIDLib::TVoid QueryValue(  const   TString&    strKey
                                    , const TString&    strSubKey
                                    ,       TVal&       objToFill) const
        {
            const TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);

            CIDLib_Suppress(6011)  // We null checked above
            if (!pnvmiTar->bQueryValue(strSubKey, objToFill))
                NotFound(strKey, &strSubKey);
        }

        tCIDLib::TVoid Reset()
        {
            m_colItems.RemoveAll();
        }

        tCIDLib::TVoid RemoveKey(const TString& strKey)
        {
            if (!m_colItems.bRemoveKey(strKey))
                NotFound(strKey, nullptr);
        }

        tCIDLib::TVoid RemoveSubKey(const   TString&   strKey
                                    , const TString&   strSubKey)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);
            if (!pnvmiTar->bRemoveSubKey(strSubKey))
                NotFound(strKey, &strSubKey);
        }

        tCIDLib::TVoid SetValue(const   TString&    strKey
                                , const TString&    strSubKey
                                , const TVal&       objValToSet)
        {
            TNVMItem<TVal>* pnvmiTar = m_colItems.pobjFindByKey(strKey);
            if (!pnvmiTar)
                NotFound(strKey, nullptr);
            if (!pnvmiTar->bSetValue(strSubKey, objValToSet))
                NotFound(strKey, &strSubKey);
        }


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_colItems
        //      The list of keyed items that we are managing.
        // -------------------------------------------------------------------
        TItemList   m_colItems;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AlreadyExists(const  TString&        strKey
                                    , const TString* const  pstrSubKey) const
        {
            tCIDLib::TErrCode errcThrow = pstrSubKey
                                          ? kCIDErrs::errcNVM_SubKeyExists
                                          : kCIDErrs::errcNVM_ItemExists;
            const TString& strSubKey    = pstrSubKey
                                          ? *pstrSubKey
                                          : TString::Nul_TString();

            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , errcThrow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
                , strKey
                , strSubKey
            );
        }

        tCIDLib::TVoid NotFound(const   TString&        strKey
                                , const TString* const  pstrSubKey) const
        {
            tCIDLib::TErrCode errcThrow = pstrSubKey
                                          ? kCIDErrs::errcNVM_SubKeyNotFound
                                          : kCIDErrs::errcNVM_ItemNotFound;
            const TString& strSubKey    = pstrSubKey
                                          ? *pstrSubKey
                                          : TString::Nul_TString();

            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , errcThrow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strKey
                , strSubKey
            );
        }


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TNamedValMap,TObject)
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Global operators
//
//  We don't want to force the value type in the map to support streaming,
//  so we do separate global streaming operators for it.
// ---------------------------------------------------------------------------
template <class TVal> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TNamedValMap<TVal>& nvmToFill)
{
    // Flush the map first
    nvmToFill.Reset();

    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != 1)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , nvmToFill.clsIsA()
        );
    }

    // Stream in the count and the XOR and validate it
    tCIDLib::TCard4     c4Count;
    tCIDLib::TCard4     c4XORCount;
    strmToReadFrom >> c4Count >> c4XORCount;

    if (c4XORCount != tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard))
        TCollectionBase::BadStoredCount(nvmToFill.clsIsA());

    // Read in this many items
    TString strKey;
    TString strSubKey;
    TVal    objVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // We should get a start marker
        strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

        // Next we should see the overall item key
        strmToReadFrom >> strKey;

        // Add a new item to the map
        nvmToFill.AddItem(strKey);

        // And now we should get the count of sub-keys
        tCIDLib::TCard4 c4SubCount;
        strmToReadFrom >> c4SubCount;
        for (tCIDLib::TCard4 c4SIndex = 0; c4SIndex < c4SubCount; c4SIndex++)
        {
            // We should see a frame marker
            strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

            // Stream in the next item and add it to the current item
            strmToReadFrom  >> strSubKey
                            >> objVal;
            nvmToFill.AddValue(strKey, strSubKey, objVal);
        }
    }

    // And the whole thing should end with an end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    return strmToReadFrom;
}


template <class TVal> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TNamedValMap<TVal>& nvmToWrite)
{
    const tCIDLib::TCard4 c4Count = nvmToWrite.c4ElemCount();

    // STart with a start marker, format, and the count and XOR'd count
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << tCIDLib::TCard2(1)
                    << c4Count
                    << tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard);

    // Iterate through the items and write them out
    TNamedValMap<TVal>::TItemList::TCursor cursItems(&nvmToWrite.colItems());
    while (cursItems.bIsValid())
    {
        // Each one should start with a start object
        strmToWriteTo << tCIDLib::EStreamMarkers::StartObject;

        // Get the current item and get a cursor over it's pairs
        const TNamedValMap<TVal>::TNVMItem<TVal>& nvmiCur = cursItems.objRCur();
        TNamedValMap<TVal>::TNVMItem<TVal>::TPairList::TCursor cursSubs(&nvmiCur.colPairs());

        // Write out the key and the count of sub-keys
        strmToWriteTo   << nvmiCur.strKey()
                        << cursSubs.c4ElemCount();

        // Now iterate it and write the pairs out
        while (cursSubs.bIsValid())
        {
            //
            //  Each one should start with a frame marker, followed
            //  by the key and the value object
            //
            const TNamedValMap<TVal>::TPair& kobjCur
            (
                cursSubs.objRCur()
            );
            strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                            << kobjCur.objKey()
                            << kobjCur.objValue();
            ++cursSubs;
        }
        ++cursItems;
    }

    // And end up with an end marker
    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;

    return strmToWriteTo;
}


