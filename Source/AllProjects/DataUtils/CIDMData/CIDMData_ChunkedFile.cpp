//
// FILE NAME: CIDMData_ChunkedFile.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2018
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
//  This file implements the classes related to our simple chunked file format.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMData_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TChunkedFileChunk, TObject)
RTTIDecls(TChunkedFileData, TChunkedFileChunk)
RTTIDecls(TChunkedFileExt, TChunkedFileChunk)
RTTIDecls(TChunkedFileMeta, TChunkedFileChunk)
RTTIDecls(TChunkedFile, TObject)


// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileChunk
// PREFIX: chflch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedFileChunk: Constructors and destructor
// ---------------------------------------------------------------------------
TChunkedFileChunk::~TChunkedFileChunk()
{
}


// ---------------------------------------------------------------------------
//  TChunkedFileChunk: Hidden constructors and operators
// ---------------------------------------------------------------------------
TChunkedFileChunk::TChunkedFileChunk(const  TString&            strId
                                    , const tCIDLib::TBoolean   bIsDataChange) :

    m_bIsDataChange(bIsDataChange)
    , m_c4Bytes(0)
    , m_strId(strId)
{
    //
    //  Generate a has for the zero sized buffer, since we need to have one, even if
    //  empty.
    //
    TSHA1Hasher mdigData;
    mdigData.StartNew();
    mdigData.DigestBuf(m_mbufData, m_c4Bytes);
    mdigData.Complete(m_mhashChunk);
}

// Add 1 when creating the buffer since it could be zero for a default initial value
TChunkedFileChunk::TChunkedFileChunk(const  TString&            strId
                                    , const tCIDLib::TCard4     c4Bytes
                                    , const TMemBuf&            mbufData
                                    , const tCIDLib::TBoolean   bIsDataChange) :

    m_bIsDataChange(bIsDataChange)
    , m_c4Bytes(c4Bytes)
    , m_mbufData(c4Bytes + 1)
    , m_strId(strId)
{
    if (c4Bytes)
        m_mbufData.CopyIn(mbufData, c4Bytes);

    // Generate a hash for the init buffer
    TSHA1Hasher mdigData;
    mdigData.StartNew();
    mdigData.DigestBuf(m_mbufData, m_c4Bytes);
    mdigData.Complete(m_mhashChunk);
}

TChunkedFileChunk::TChunkedFileChunk(const TChunkedFileChunk& chflchSrc) :

    m_bIsDataChange(chflchSrc.m_bIsDataChange)
    , m_c4Bytes(chflchSrc.m_c4Bytes)
    , m_mbufData(chflchSrc.m_mbufData)
    , m_mhashChunk(chflchSrc.m_mhashChunk)
    , m_strId(chflchSrc.m_strId)
{
}

TChunkedFileChunk& TChunkedFileChunk::operator=(const TChunkedFileChunk& chflchSrc)
{
    if (this != &chflchSrc)
    {
        m_bIsDataChange = chflchSrc.m_bIsDataChange;
        m_mhashChunk    = chflchSrc.m_mhashChunk;
        m_strId         = chflchSrc.m_strId;

        //
        //  If our current buffer is not the same size, reallocate (ignoring old content)
        //  Watch for a zero size and do 1 instead since zero is not valid. The m_c4Bytes
        //  value indicates the actual size so this is not a problem.
        //
        if (chflchSrc.m_c4Bytes != m_c4Bytes)
        {
            m_mbufData.Reallocate
            (
                chflchSrc.m_c4Bytes ? chflchSrc.m_c4Bytes : 1
                , kCIDLib::False
            );
        }

        // Now copy in the bytes we need and store the new size
        m_c4Bytes = chflchSrc.m_c4Bytes;
        m_mbufData.CopyIn(chflchSrc.m_mbufData, m_c4Bytes);
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TChunkedFileChunk: Public, inherited methods
// ---------------------------------------------------------------------------

// We just set an empty data buffer on ourself
tCIDLib::TBoolean TChunkedFileChunk::bClearData()
{
    THeapBuf mbufEmpty(1);
    return bSetData(0, mbufEmpty);
}


//
//  This is called to set new data on us. We calculate the hash for the incoming data
//  and, if it is different from what we have, then we store the new data and hash.
//
tCIDLib::TBoolean
TChunkedFileChunk::bSetData(const tCIDLib::TCard4 c4Bytes, const TMemBuf& mbufData)
{
    // Generate a hash for this data
    TSHA1Hasher mdigData;
    TSHA1Hash   mhashNew;

    mdigData.StartNew();
    mdigData.DigestBuf(mbufData, c4Bytes);
    mdigData.Complete(mhashNew);

    if (mhashNew != m_mhashChunk)
    {
        //
        //  If our current buffer is not the same size, reallocate (ignoring old content),
        //  then copy in the bytes we need. Handle possible zero size.
        //
        if (c4Bytes != m_c4Bytes)
            m_mbufData.Reallocate(c4Bytes ? c4Bytes : 1, kCIDLib::False);
        m_mbufData.CopyIn(mbufData, c4Bytes);

        m_c4Bytes = c4Bytes;
        m_mhashChunk = mhashNew;
        return kCIDLib::True;
    }

    return kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileData
// PREFIX: chflch
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TChunkedFileData: Constructors and destructor
// ---------------------------------------------------------------------------

// Indicate to the parent that we are a data change chunk
TChunkedFileData::TChunkedFileData() :

    TChunkedFileChunk(kCIDMData::strChunkId_Data, kCIDLib::True)
{
}

TChunkedFileData::TChunkedFileData( const   tCIDLib::TCard4 c4Bytes
                                    , const TMemBuf&        mbufData) :

    TChunkedFileChunk(kCIDMData::strChunkId_Data, c4Bytes, mbufData, kCIDLib::True)
{
}

TChunkedFileData::TChunkedFileData(const TChunkedFileData& chflchSrc) :

    TChunkedFileChunk(chflchSrc)
{
}

TChunkedFileData::~TChunkedFileData()
{
}


// ---------------------------------------------------------------------------
//  TChunkedFileData: Public operators
// ---------------------------------------------------------------------------
TChunkedFileData& TChunkedFileData::operator=(const TChunkedFileData& chflchSrc)
{
    if (this != &chflchSrc)
    {
        TParent::operator=(chflchSrc);
    }
    return *this;
}





// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileExt
// PREFIX: chflch
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TChunkedFileExt: Constructors and destructor
// ---------------------------------------------------------------------------

// Indicate to the parent that we are a data change chunk
TChunkedFileExt::TChunkedFileExt(const  TString&            strId
                                , const tCIDLib::TBoolean   bIsDataChange) :

    TChunkedFileChunk(strId, bIsDataChange)
{
}

TChunkedFileExt::TChunkedFileExt(   const   TString&            strId
                                    , const tCIDLib::TBoolean   bIsDataChange
                                    , const tCIDLib::TCard4     c4Bytes
                                    , const TMemBuf&            mbufData) :

    TChunkedFileChunk(strId, c4Bytes, mbufData, bIsDataChange)
{
}

TChunkedFileExt::TChunkedFileExt(const TChunkedFileExt& chflchSrc) :

    TChunkedFileChunk(chflchSrc)
{
}

TChunkedFileExt::~TChunkedFileExt()
{
}


// ---------------------------------------------------------------------------
//  TChunkedFileExt: Public operators
// ---------------------------------------------------------------------------
TChunkedFileExt& TChunkedFileExt::operator=(const TChunkedFileExt& chflchSrc)
{
    if (this != &chflchSrc)
    {
        TParent::operator=(chflchSrc);
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileMeta
// PREFIX: chflch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedFileMeta: Constructors and destructor
// ---------------------------------------------------------------------------

TChunkedFileMeta::TChunkedFileMeta() :

    TChunkedFileChunk(kCIDMData::strChunkId_Meta, kCIDLib::False)
    , m_colValues(8UL)
{
    // Do an initial regen of the data so that it's valid
    RegenRawData();
}

TChunkedFileMeta::TChunkedFileMeta(const tCIDLib::TKVPFList& colValues) :

    TChunkedFileChunk(kCIDMData::strChunkId_Meta, kCIDLib::False)
    , m_colValues(8UL)
{
    //
    //  We don't just copy ctor these since we want to enforce our own max elements
    //  and sorting.
    //
    AddKeys(colValues);
}

TChunkedFileMeta::TChunkedFileMeta(const TChunkedFileMeta& chflchSrc) :

    TChunkedFileChunk(chflchSrc)
    , m_colValues(chflchSrc.m_colValues)
{
}

TChunkedFileMeta::~TChunkedFileMeta()
{
}


// ---------------------------------------------------------------------------
//  TChunkedFileMeta: Public operators
// ---------------------------------------------------------------------------
TChunkedFileMeta& TChunkedFileMeta::operator=(const TChunkedFileMeta& chflchSrc)
{
    if (this != &chflchSrc)
    {
        TParent::operator=(chflchSrc);
        m_colValues = chflchSrc.m_colValues;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TChunkedFileMeta: Public, inherited methods
// ---------------------------------------------------------------------------

// We clear our values out and call our parent, returning his return
tCIDLib::TBoolean TChunkedFileMeta::bClearData()
{
    m_colValues.RemoveAll();
    return TParent::bClearData();
}


//
//  Called to set new data on us. We try to parse our data out. If it works, then
//  we call our parent to store the raw data.
//
tCIDLib::TBoolean
TChunkedFileMeta::bSetData(const tCIDLib::TCard4 c4Bytes, const TMemBuf& mbufData)
{
    //
    //  If our parent says the data has changed, then stream in our values to create
    //  our in-memory representation.
    //
    if (TParent::bSetData(c4Bytes, mbufData))
    {
        m_colValues.RemoveAll();

        // If we got any data, then stream in the new ones
        if (c4Bytes)
        {
            TBinMBufInStream strmSrc(&mbufData, c4Bytes);
            TKeyValFPair kvalfCur;

            strmSrc.CheckForStartMarker(CID_FILE, CID_LINE);
            tCIDLib::TCard4 c4Count;
            strmSrc >> c4Count;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                strmSrc >> kvalfCur;
                strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);
                m_colValues.objAdd(kvalfCur);
            }
            strmSrc.CheckForEndMarker(CID_FILE, CID_LINE);
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TChunkedFileMeta: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Adds a single new key to the list, with an initial value. The caller tells us if
//  this is a file change key or not. It will throw if the key is already present.
//
//  If you are going to add more than one key, then use AddKeys() below since that
//  will only require a single regen.
//
tCIDLib::TVoid
TChunkedFileMeta::AddKey(const  TString&            strKey
                        , const TString&            strInitVal
                        , const tCIDLib::TBoolean   bIsFileChange)
{
    tCIDLib::TCard4 c4At;
    if (m_colValues.pobjKeyedBinarySearch(strKey, TKeyValFPair::eCompKeyI, c4At))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_MetaKeyAlready
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strKey
        );
    }

    // It's new so add it and obviously regen
    m_colValues.InsertSorted
    (
        TKeyValFPair(strKey, strInitVal, bIsFileChange), TKeyValFPair::eCompI, c4At
    );
    RegenRawData();
}


//
//  Adds multiple new keys to the list. If any are already present, it will throw.
//  Use this to add multiple keys since it only re-gens once.
//
tCIDLib::TVoid TChunkedFileMeta::AddKeys(const tCIDLib::TKVPFList& colToAdd)
{
    tCIDLib::TCard4 c4At;

    const tCIDLib::TCard4 c4Count = colToAdd.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValFPair& kvalfCur = colToAdd[c4Index];

        if (m_colValues.pobjKeyedBinarySearch(kvalfCur.strKey(), TKeyValFPair::eCompKeyI, c4At))
        {
            facCIDMData().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMDataErrs::errcChFl_MetaKeyAlready
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
                , kvalfCur.strKey()
            );
        }

        m_colValues.InsertSorted(kvalfCur, TKeyValFPair::eCompI, c4At);
    }
    RegenRawData();
}


//
//  Find the value of a key, if present. Returns true or false to indicate whether it
//  found the value or not.
//
tCIDLib::TBoolean
TChunkedFileMeta::bFindValue(const TString& strKey, TString& strToFill) const
{
    // Do a binary search since they are sorted
    tCIDLib::TCard4 c4At;
    const TKeyValFPair* pkvalFind = m_colValues.pobjKeyedBinarySearch
    (
        strKey, &TKeyValFPair::eCompKeyI, c4At
    );

    if (pkvalFind)
    {
        strToFill = pkvalFind->strValue();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  This method will see if the passed key exists. If not, it will return the
//  IfNotFound value. Else it will return whether the key's value is equal to the
//  passed comparison value.
//
tCIDLib::TBoolean
TChunkedFileMeta::bTestValue(const  TString&            strKey
                            , const TString&            strCompare
                            , const tCIDLib::TBoolean   bIfNotFound
                            , const tCIDLib::TBoolean   bCaseSensitive) const
{
    tCIDLib::TCard4 c4At;
    const TKeyValFPair* pkvalFind = m_colValues.pobjKeyedBinarySearch
    (
        strKey, TKeyValFPair::eCompKeyI, c4At
    );

    if (!pkvalFind)
        return bIfNotFound;

    if (bCaseSensitive)
        return pkvalFind->strValue().bCompare(strCompare);

    return pkvalFind->strValue().bCompareI(strCompare);
}


//
//  Looks for the indicated key and compares its value to the passed one. It will
//  throw if the key is not present. Returns true or false to indicate quality or not.
//
tCIDLib::TBoolean
TChunkedFileMeta::bValueEquals( const   TString&            strKey
                                , const TString&            strCompare
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Do a binary search since they are sorted
    tCIDLib::TCard4 c4At;
    const TKeyValuePair* pkvalFind = m_colValues.pobjKeyedBinarySearch
    (
        strKey, TKeyValFPair::eCompKeyI, c4At
    );

    // If it doesn't exist, then that's an error
    if (!pkvalFind)
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_MetaKeyNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKey
        );
    }

    if (bCaseSensitive)
        return (pkvalFind->strValue() == strCompare);

    return pkvalFind->strValue().bCompareI(strCompare);
}


// We return true if we removed any keys. We also indicate if any were file change keys
tCIDLib::TBoolean TChunkedFileMeta::bRemoveAllKeys(tCIDLib::TBoolean& bFileChange)
{
    bFileChange = kCIDLib::False;

    // If already empty, nothing to do
    if (m_colValues.bIsEmpty())
    {
        CIDAssert(c4Bytes() == 0, L"No meta keys but data bytes is not zero");
        return kCIDLib::False;
    }

    // Remmeber if any were file change keys
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        bFileChange |= m_colValues[c4Index].bFlag();

    // Now clear them out and re-gin
    m_colValues.RemoveAll();
    RegenRawData();

    // We removed keys
    return kCIDLib::True;
}


//
//  Removes the key if present and regenerates raw data and hash. The return indicates
//  if it was removed or not. And we also return whether it was a file change.
//
tCIDLib::TBoolean
TChunkedFileMeta::bRemoveKey(const TString& strKey, tCIDLib::TBoolean& bFileChange)
{
    // If found and removed, zero the byte count to indicate we need to regen raw data
    tCIDLib::TCard4 c4At;
    TKeyValFPair* pkvalfFind = m_colValues.pobjKeyedBinarySearch
    (
        strKey, &TKeyValFPair::eCompKeyI, c4At
    );

    bFileChange = kCIDLib::False;
    if (pkvalfFind)
    {
        bFileChange = pkvalfFind->bFlag();
        m_colValues.RemoveAt(c4At);
        RegenRawData();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Set a metadata value. The key must already exist. We return true if the value was
//  changed. We also indicate if it was a file change.
//
//  If setting more than one use the SetValues() method below and avoid the regeneration
//  overhead because this will do it for each change.
//
tCIDLib::TBoolean
TChunkedFileMeta::bSetValue(const   TString&            strKey
                            , const TString&            strToSet
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    // Do a binary search since they are sorted
    tCIDLib::TCard4 c4At;
    TKeyValFPair* pkvalfFind = m_colValues.pobjKeyedBinarySearch
    (
        strKey, &TKeyValFPair::eCompKeyI, c4At
    );

    if (!pkvalfFind)
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_MetaKeyNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strKey
        );
    }

    // If the value is different, store it and update our raw data
    bFileChange = pkvalfFind->bFlag();
    if (strToSet != pkvalfFind->strValue())
    {
        pkvalfFind->strValue(strToSet);
        RegenRawData();
        return kCIDLib::True;
    }

    // No changes made
    return kCIDLib::False;
}


//
//  Much more efficient if setting multiple values since we only regenerate the raw
//  data and hash once! They keys must already exist. This doesn't add or remove any
//  keys, it just sets ones that are already there.
//
//  We don't just call bSetValue() for each one since it would do that regeneration.
//
//  We return true if any values were modified, and we also return whether any file
//  change values were modified.
//
//  We regenerate if any key values change.
//
//  We have two variations, one that takes a key/value pair list, and another that
//  takes a key/value/flag list. The latter is often convenient since that may be how
//  the program defines them in order to add them. We just ignore the flag in this
//  case since it's only used when adding (defining) the keys, not when setting them.
//
tCIDLib::TBoolean
TChunkedFileMeta::bSetValues(const  tCIDLib::TKVPList&  colNewVals
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    tCIDLib::TBoolean bRegen = kCIDLib::False;
    bFileChange = kCIDLib::False;

    tCIDLib::TCard4 c4At;
    const tCIDLib::TCard4 c4Count = colNewVals.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colNewVals[c4Index];

        TKeyValFPair* pkvalfFind = m_colValues.pobjKeyedBinarySearch
        (
            kvalCur.strKey(), &TKeyValFPair::eCompKeyI, c4At
        );

        if (!pkvalfFind)
        {
            facCIDMData().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMDataErrs::errcChFl_MetaKeyNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , kvalCur.strKey()
            );
        }

        if (pkvalfFind->strValue() != kvalCur.strValue())
        {
            // The value is different so we need to store it and regen
            pkvalfFind->strValue(kvalCur.strValue());
            bRegen = kCIDLib::True;

            // Remember any file change values changing
            bFileChange |= pkvalfFind->bFlag();
        }
    }

    // Regen the raw data if needed
    if (bRegen)
        RegenRawData();

    // Return true if we changed any values else false
    return bRegen;
}

tCIDLib::TBoolean
TChunkedFileMeta::bSetValues(const  tCIDLib::TKVPFList& colNewVals
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    tCIDLib::TBoolean bRegen = kCIDLib::False;
    bFileChange = kCIDLib::False;

    tCIDLib::TCard4 c4At;
    const tCIDLib::TCard4 c4Count = colNewVals.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValFPair& kvalfCur = colNewVals[c4Index];

        TKeyValFPair* pkvalfFind = m_colValues.pobjKeyedBinarySearch
        (
            kvalfCur.strKey(), &TKeyValFPair::eCompKeyI, c4At
        );

        if (!pkvalfFind)
        {
            facCIDMData().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMDataErrs::errcChFl_MetaKeyNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , kvalfCur.strKey()
            );
        }

        if (pkvalfFind->strValue() != kvalfCur.strValue())
        {
            // The value is different so we need to store it and regen
            pkvalfFind->strValue(kvalfCur.strValue());
            bRegen = kCIDLib::True;

            // Remember any file change values changing
            bFileChange |= pkvalfFind->bFlag();
        }
    }

    // Regen the raw data if needed
    if (bRegen)
        RegenRawData();

    // Return true if we changed any values else false
    return bRegen;
}

// Return an estimate of how many bytes we'd take to be flattened
tCIDLib::TCard4 TChunkedFileMeta::c4EstBytes() const
{
    tCIDLib::TCard4 c4Ret = 0;
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        c4Ret += m_colValues[c4Index].strKey().c4Length();
        c4Ret += m_colValues[c4Index].strValue().c4Length();
    }

    // And some storage overhead
    c4Ret += c4Count * 16;

    //
    //  Account for possible non-single byte chars in the flattened UTF-8 data. It
    //  could be all single byte, or all of them could be more than 2 bytes, but
    //  this is a reasonable estimate likely to be more than enough.
    //
    return c4Ret * 2;
}


//
//  Give back a copy of all of our meta key/values. One gives back just the key/value
//  pairs and the other the whole key/value/flag info.
//
tCIDLib::TVoid TChunkedFileMeta::QueryValues(tCIDLib::TKVPList& colToFill) const
{
    colToFill.RemoveAll();
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValFPair& kvalfCur = m_colValues[c4Index];
        colToFill.objPlace(kvalfCur.strKey(), kvalfCur.strValue());
    }
}


// We will resize the caller's list if necessary
tCIDLib::TVoid TChunkedFileMeta::QueryValues(tCIDLib::TKVPFList& colToFill) const
{
    colToFill = m_colValues;
}



// ---------------------------------------------------------------------------
//  TChunkedFileMeta: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TChunkedFileMeta::RegenRawData()
{
    TBinMBufOutStream strmTar(c4Bytes() + 1024);

    //
    //  Stream out the key value pairs in their already sorted order, with bracketing
    //  markers.
    //
    const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
    strmTar << tCIDLib::EStreamMarkers::StartObject
            << c4Count;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmTar << m_colValues[c4Index]
                << tCIDLib::EStreamMarkers::Frame;
    }
    strmTar << tCIDLib::EStreamMarkers::EndObject;
    strmTar.Flush();

    // Call our parent to set the new data
    bSetData(strmTar.c4CurSize(), strmTar.mbufData());
}





// ---------------------------------------------------------------------------
//  CLASS: TChunkedFile
// PREFIX: chfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedFile: Public, static methods
// ---------------------------------------------------------------------------

//
//  A helper to find the index of a given key in the passed meta value list, and another
//  that returns a boolean and just calls the index version.
//
tCIDLib::TBoolean
TChunkedFile::bFindMetaKey( const   tCIDLib::TKVPFList& colMetaVals
                            , const TString&            strToFind
                            , const tCIDLib::TBoolean   bCaseSensitive)
{
    return c4FindMetaKey(colMetaVals, strToFind, bCaseSensitive) != kCIDLib::c4MaxCard;
}

tCIDLib::TCard4
TChunkedFile::c4FindMetaKey(const   tCIDLib::TKVPFList& colMetaVals
                            , const TString&            strToFind
                            , const tCIDLib::TBoolean   bCaseSensitive)
{
    const tCIDLib::TCard4 c4Count = colMetaVals.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (bCaseSensitive)
        {
            if (colMetaVals[c4Index].strKey().bCompare(strToFind))
                return c4Index;
        }
         else
        {
            if (colMetaVals[c4Index].strKey().bCompareI(strToFind))
                return c4Index;
        }
    }
    return kCIDLib::c4MaxCard;
}


// A helper to just pull out the basic header info
tCIDLib::TVoid
TChunkedFile::ExtractHdrInfo(   TBinInStream&               strmSrc
                                , tCIDLib::TCard4&          c4SerialNum
                                , tCIDLib::TEncodedTime&    enctLastChange
                                , tCIDLib::TCard4&          c4ChunkCnt)
{
    // Validate the header and get the info out of it
    StreamInHdr(strmSrc, c4SerialNum, enctLastChange, c4ChunkCnt);
}



//
//  A helper to pull out the basic header info plus the ids and sizes of each of
//  the chunks and the metadata values. The chunk ids and sizes are in the order
//  that they are in the file (so meta, data, then any extension chunks.)
//
tCIDLib::TVoid
TChunkedFile::ExtractInfo(  TBinInStream&               strmSrc
                            , tCIDLib::TCard4&          c4SerialNum
                            , tCIDLib::TEncodedTime&    enctLastChange
                            , tCIDLib::TKVPFList&       colMetaVals
                            , tCIDLib::TStrList&        colChunkIds
                            , tCIDLib::TCardList&       fcolChunkSzs)
{
    // Clear the outgoing lists to prepare to load them
    colChunkIds.RemoveAll();
    colMetaVals.RemoveAll();
    fcolChunkSzs.RemoveAll();

    // Validate the header and get the info out of it
    tCIDLib::TCard4 c4ChunkCnt;
    StreamInHdr(strmSrc, c4SerialNum, enctLastChange, c4ChunkCnt);

    // Create a local short char buffer more than long enough for the longest chunk id
    const tCIDLib::TCard4 c4MaxChunkChars = 511;
    tCIDLib::TSCh aschId[c4MaxChunkChars + 1];

    //
    //  We are now on the first byte of the first chunk, which is the meta data chunk.
    //  We call a helper to read in that one specifically since we need to get the
    //  data out o fit.
    //
    tCIDLib::TCard4 c4MetaChunkSz;
    TChunkedFileChunk* pchflchChunk = pchflchStreamInChunk(strmSrc, c4MetaChunkSz);
    TJanitor<TChunkedFileChunk> janChunk(pchflchChunk);
    if (!pchflchChunk->strId().bCompareI(kCIDMData::strChunkId_Meta))
        ThrowFmtErr(L"The first file chunk was not the Meta chunk", CID_LINE);

    // That worked so cast to type and query the meta values out
    TChunkedFileMeta* pchflchMeta = static_cast<TChunkedFileMeta*>(pchflchChunk);
    pchflchMeta->QueryValues(colMetaVals);

    // Add this one to the caller's lists
    colChunkIds.objAdd(kCIDMData::strChunkId_Meta);
    fcolChunkSzs.c4AddElement(c4MetaChunkSz);

    //
    //  Now we go through the rest of the chunks, starting at 1. The above left us at
    //  the start of the main data chunk.
    //
    tCIDLib::TCard1 c1Eat;
    tCIDLib::TCard4 c4CurSz;
    TString strCurId;
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4ChunkCnt; c4Index++)
    {
        // Read the chunk id and id character count (and the XOR'd version)
        tCIDLib::TCard1 c1Marker;
        strmSrc >> c1Marker;
        if (c1Marker != kCIDMData::c1ChunkFlId_ChunkStart)
        {
            ThrowFmtErr
            (
                L"Expected chunk start marker but got %(1)"
                , CID_LINE
                , TCardinal(c1Marker)
            );
        }

        // Get the chunk id bytes count
        tCIDLib::TCard1 c1IdBytes, c1IdXOR;
        strmSrc >> c1IdBytes >> c1IdXOR;
        if ((c1IdBytes ^ 0xFF) != c1IdXOR)
        {
            ThrowFmtErr(L"The file chunk's id length was not valid", CID_LINE);
        }

        if (c1IdBytes > c4MaxChunkChars)
            ThrowFmtErr(L"The chunk's id length was too long to read", CID_LINE);

        // Now pull in the chunk id bytes
        strmSrc.c4ReadRawBuffer(aschId, c1IdBytes);
        aschId[c1IdBytes] = 0;
        strCurId.FromShortZStr(aschId);
        colChunkIds.objAdd(strCurId);

        // Eat flags and check a frame marker
        strmSrc >> c1Eat;
        strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

        // And now we finally get the bytes in this chunk
        strmSrc >> c4CurSz;
        fcolChunkSzs.c4AddElement(c4CurSz);

        //
        //  Now we can skip over the rest of the header and the chunk data, and
        //  that leaves us on the start of the next chunk.
        //
        strmSrc.SkipForwardBy(22 + c4CurSz);
    }
}


//
//  A helper to just pull out the meta type info from the file, ignoring the actual
//  data, which the caller may not remotely understand. Since all of the file reading
//  is done via static protected methods normally, we can all them as well from this
//  static method to grab the info we want.
//
tCIDLib::TVoid
TChunkedFile::ExtractMetaInfo(  TBinInStream&               strmSrc
                                , tCIDLib::TCard4&          c4SerialNum
                                , tCIDLib::TEncodedTime&    enctLastChange
                                , tCIDLib::TKVPFList&       colToFill
                                , tCIDLib::TCard4&          c4ChunkCnt)
{
    // Validate the header and get the info out of it
    StreamInHdr(strmSrc, c4SerialNum, enctLastChange, c4ChunkCnt);

    //
    //  Use a helper to stream in the meta chunk, which is the first one in the file
    //  so we don't have to waste the overhead of skipping over any. Put a janitor on
    //  it since it's just temporary.
    //
    tCIDLib::TCard4 c4ChunkSz;
    TChunkedFileChunk* pchflchChunk = pchflchStreamInChunk(strmSrc, c4ChunkSz);
    TJanitor<TChunkedFileChunk> janChunk(pchflchChunk);

    // Make sure it's the meta chunk and then cast it
    if (!pchflchChunk->strId().bCompareI(kCIDMData::strChunkId_Meta))
    {
        ThrowFmtErr(L"The first file chunk was not the Meta chunk", CID_LINE);
    }

    // And give the caller a copy of the key/value pairs
    TChunkedFileMeta* pchflchMeta = static_cast<TChunkedFileMeta*>(pchflchChunk);
    pchflchMeta->QueryValues(colToFill);
}


// ---------------------------------------------------------------------------
//  TChunkedFile: Constructors and destructor
// ---------------------------------------------------------------------------

//
//  The last chunk hashes will be set to defaults, which will be different from the
//  ones that get set from the initial or default data, so we will show unsaved changes
//  after these ctors (not the copy ctor of course.)
//
TChunkedFile::TChunkedFile() :

    m_c4SerialNum(1)
    , m_colChunks(tCIDLib::EAdoptOpts::Adopt, 32)
    , m_colLastHashes(32)
    , m_enctLastChange(TTime::enctNow())
    , m_pchflchMeta(nullptr)
{
    // Create empty default chunks
    tCIDLib::TKVPFList colMeta;
    THeapBuf mbufData(1);
    MakeDefChunks(0, mbufData, colMeta);
}

TChunkedFile::TChunkedFile( const   tCIDLib::TKVPFList& colMeta
                            , const tCIDLib::TCard4     c4Bytes
                            , const TMemBuf&            mbufData) :

    m_c4SerialNum(1)
    , m_colChunks(tCIDLib::EAdoptOpts::Adopt, 32)
    , m_colLastHashes(32)
    , m_enctLastChange(TTime::enctNow())
    , m_pchflchMeta(nullptr)
{
    // Create the default chunks with the passed info
    MakeDefChunks(c4Bytes, mbufData, colMeta);
}

// A convenience to set up with meata and data plus one extension chunk
TChunkedFile::TChunkedFile( const   tCIDLib::TKVPFList& colMeta
                            , const tCIDLib::TCard4     c4Bytes
                            , const TMemBuf&            mbufData
                            , const TString&            strExtChunkId
                            , const tCIDLib::TCard4     c4ExtBytes
                            , const TMemBuf&            mbufExtData
                            , const tCIDLib::TBoolean   bIsDataChange) :

    m_c4SerialNum(1)
    , m_colChunks(tCIDLib::EAdoptOpts::Adopt, 32)
    , m_colLastHashes(32)
    , m_enctLastChange(TTime::enctNow())
    , m_pchflchMeta(nullptr)
{
    // Create the default chunks with the passed info
    MakeDefChunks(c4Bytes, mbufData, colMeta);

    //
    //  And add the extension chunk if there is one. It is optional, for convenience
    //  so the caller doesn't have to call this or the above ctor.
    //
    if (!strExtChunkId.bIsEmpty())
    {
        TChunkedFileExt* pchflchNew = new TChunkedFileExt
        (
            strExtChunkId, bIsDataChange, c4ExtBytes, mbufExtData
        );
        m_colChunks.Add(pchflchNew);
        m_colLastHashes.objAdd(pchflchNew->mhashChunk());
    }
}

TChunkedFile::TChunkedFile(const TChunkedFile& chflSrc) :

    m_c4SerialNum(chflSrc.m_c4SerialNum)
    , m_colChunks(tCIDLib::EAdoptOpts::Adopt, 32)
    , m_colLastHashes(32)
    , m_enctLastChange(chflSrc.m_enctLastChange)
{
    // Dup the chunks and hashes
    const tCIDLib::TCard4 c4Count = chflSrc.m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        m_colChunks.Add
        (
            static_cast<TChunkedFileChunk*>(chflSrc.m_colChunks[c4Index]->pobjDuplicate())
        );
        m_colLastHashes.objAdd(chflSrc.m_colLastHashes[c4Index]);
    }

    // Get the special meta pointer
    CIDAssert
    (
        m_colChunks[0]->strId() == kCIDMData::strChunkId_Meta
        , L"First chunk of source chunked file is not a meta chunk"
    );
    m_pchflchMeta = static_cast<TChunkedFileMeta*>(m_colChunks[0]);
}

TChunkedFile::~TChunkedFile()
{
    try
    {
        m_colChunks.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
        // <TBD>
    }
}


// ---------------------------------------------------------------------------
//  TChunkedFile: Public operators
// ---------------------------------------------------------------------------
TChunkedFile& TChunkedFile::operator=(const TChunkedFile& chflSrc)
{
    if (&chflSrc != this)
    {
        m_c4SerialNum       = chflSrc.m_c4SerialNum;
        m_enctLastChange    = chflSrc.m_enctLastChange;

        // Dup the source chunk lists
        m_colChunks.RemoveAll();
        m_colLastHashes.RemoveAll();

        const tCIDLib::TCard4 c4Count = chflSrc.m_colChunks.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            m_colChunks.Add
            (
                static_cast<TChunkedFileChunk*>(chflSrc.m_colChunks[c4Index]->pobjDuplicate())
            );
            m_colLastHashes.objAdd(chflSrc.m_colLastHashes[c4Index]);
        }

        // Get the special meta pointer
        CIDAssert
        (
            m_colChunks[0]->strId() == kCIDMData::strChunkId_Meta
            , L"First chunk of source chunked file is not a meta chunk"
        );
        m_pchflchMeta = static_cast<TChunkedFileMeta*>(m_colChunks[0]);
    }
    return *this;
}


//
//  For the chunks we only need to compare hashes. If they are not the same then they
//  cannot be equal, and vice versa.
//
tCIDLib::TBoolean TChunkedFile::operator==(const TChunkedFile& chflSrc) const
{
    if ((m_c4SerialNum  != chflSrc.m_c4SerialNum)
    ||  (m_enctLastChange != chflSrc.m_enctLastChange))
    {
        return kCIDLib::False;
    }

    //
    //  Check the lists. For the chunks we only have to compare the hashes. If they
    //  are the same, then we can assume the data is the same.
    //
    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colLastHashes[c4Index] != chflSrc.m_colLastHashes[c4Index])
            return kCIDLib::False;

        if (m_colChunks[c4Index]->mhashChunk() != chflSrc.m_colChunks[c4Index]->mhashChunk())
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TChunkedFile::operator!=(const TChunkedFile& chflSrc) const
{
    return !operator==(chflSrc);
}



// ---------------------------------------------------------------------------
//  TChunkedFile: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Allow the client code to add their own extension chunks. The id must not already
//  exist, which also prevents them from using one of our built in ids.
//
//  Note that this does NOT cause the serial number/time stamp to be bumped, since
//  this is part of building the file contents up. It's not a change to a chunk itself,
//  so we store the incoming has as the last hash for this chunk and don't bump.
//
tCIDLib::TVoid
TChunkedFile::AddExtChunk(const TString& strId, const tCIDLib::TBoolean bIsDataChange)
{
    if (pchflchFindById(strId, kCIDLib::False))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_DupChunk
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strId
        );
    }

    TChunkedFileExt* pchflchNew = new TChunkedFileExt(strId, bIsDataChange);
    m_colChunks.Add(pchflchNew);
    m_colLastHashes.objAdd(pchflchNew->mhashChunk());
}

tCIDLib::TVoid
TChunkedFile::AddExtChunk(  const   TString&            strId
                            , const tCIDLib::TCard4     c4Bytes
                            , const TMemBuf&            mbufData
                            , const tCIDLib::TBoolean   bIsDataChange)
{
    if (pchflchFindById(strId, kCIDLib::False))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_DupChunk
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strId
        );
    }

    TChunkedFileExt* pchflchNew = new TChunkedFileExt(strId, bIsDataChange, c4Bytes, mbufData);
    m_colChunks.Add(pchflchNew);
    m_colLastHashes.objAdd(pchflchNew->mhashChunk());
}


// Adds a new key to the meta key list for this file
tCIDLib::TVoid
TChunkedFile::AddMetaKey(const  TString&            strKey
                        , const TString&            strInitVal
                        , const tCIDLib::TBoolean   bIsFileChange)
{
    m_pchflchMeta->AddKey(strKey, strInitVal, bIsFileChange);
}


// Adds multiple meta keys
tCIDLib::TVoid TChunkedFile::AddMetaKeys(const tCIDLib::TKVPFList& colToAdd)
{
    m_pchflchMeta->AddKeys(colToAdd);
}


//
//  See if the passed hash is the same as the current one for the indicated chunk. We
//  throw if it's not found.
//
tCIDLib::TBoolean
TChunkedFile::bChunkChanged(const TString& strId, const TSHA1Hash& mhashCompare) const
{
    const TChunkedFileChunk* pchflchFind = pchflchFindById(strId, kCIDLib::True);
    return (pchflchFind->mhashChunk() == mhashCompare);
}


// Just see if there is a chunk with the passed id
tCIDLib::TBoolean TChunkedFile::bChunkExists(const TString& strId) const
{
    return (pchflchFindById(strId, kCIDLib::False) != nullptr);
}


//
//  Compares the current hashes of our chunks against the last stored (or initially loaded)
//  hashes. If they are not the same, then there are changes to store.
//
//  NOTE this includes all chunks, not just ones marked as 'file change' chunks. This
//  is to allow for a chunk for any change.
//
tCIDLib::TBoolean TChunkedFile::bFileChanged() const
{
    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colLastHashes[c4Index] != m_colChunks[c4Index]->mhashChunk())
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TChunkedFile::bMetaValueEquals( const   TString&            strKey
                                , const TString&            strToFill
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Pass it on to the meta chunk, so not case sensitive
    return m_pchflchMeta->bValueEquals(strKey, strToFill, bCaseSensitive);
}


//
//  Removes a key from the meta chunk. Returns true if it was removed. Also indicates
//  if it was a file change key.
//
tCIDLib::TBoolean
TChunkedFile::bRemoveMetaKey(const TString& strKey, tCIDLib::TBoolean& bFileChange)
{
    const tCIDLib::TBoolean bRes = m_pchflchMeta->bRemoveKey(strKey, bFileChange);
    if (bFileChange)
    {
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();
    }
    return bRes;
}


//
//  We pass this through to the data chunk object. If he reports a change, we update our
//  serial number and last change time stamp. The return indicates if this actually
//  caused a change or not.
//
tCIDLib::TBoolean
TChunkedFile::bSetDataChunk(const tCIDLib::TCard4 c4Bytes, const TMemBuf& mbufToSet)
{
    CIDAssert
    (
        (m_colChunks[1]->strId() == kCIDMData::strChunkId_Data)
        , L"The first file chunk is not the data chunk"
    );

    if (m_colChunks[1]->bSetData(c4Bytes, mbufToSet))
    {
        // This one always is considered a change
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();

        // Store the
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Set's an arbitrary chunk's data by id. The return indicates if this was an actual
//  change in the data or not. If the id isn't found, we throw.
//
tCIDLib::TBoolean
TChunkedFile::bSetChunkById(const   TString&        strId
                            , const tCIDLib::TCard4 c4Bytes
                            , const TMemBuf&        mbufToSet)
{
    // Throw if not found
    TChunkedFileChunk* pchflchFind = pchflchFindById(strId, kCIDLib::True);
    if (pchflchFind->bSetData(c4Bytes, mbufToSet))
    {
        // If marked as a data change type, then update our stamps
        if (pchflchFind->bIsDataChange())
        {
            m_c4SerialNum++;
            m_enctLastChange = TTime::enctNowUTC();
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  A specialized mtehod to set a meta value on the meta chunk. The key must exist.
//  Returns whether the value was different, and also whether it was a file change
//  if the value was different. We bump the serial number/stamp on a file change.
//
tCIDLib::TBoolean
TChunkedFile::bSetMetaValue(const   TString&            strKey
                            , const TString&            strToSet
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    const tCIDLib::TBoolean bRet = m_pchflchMeta->bSetValue(strKey, strToSet, bFileChange);
    if (bFileChange)
    {
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();
    }
    return bRet;
}


//
//  Sets one or more meta key at a time. If any changed that were file change
//  type keys, then we bump the serial number and last change stamp. It returns
//  true if any values were changed. Returns whether any changed values were file
//  changes, in which case we bump the serial number and stamp.
//
//  The version that takes the key/value/flag list just ignores the flag. That is
//  only used when adding (defining) keys, but since they will already have such a
//  list around, it's convenient to accept them for setting values as well.
//
tCIDLib::TBoolean
TChunkedFile::bSetMetaValues(const  tCIDLib::TKVPList&  colValues
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    const tCIDLib::TBoolean bRes = m_pchflchMeta->bSetValues(colValues, bFileChange);
    if (bFileChange)
    {
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();
    }
    return bRes;
}

tCIDLib::TBoolean
TChunkedFile::bSetMetaValues(const  tCIDLib::TKVPFList& colValues
                            ,       tCIDLib::TBoolean&  bFileChange)
{
    const tCIDLib::TBoolean bRes = m_pchflchMeta->bSetValues(colValues, bFileChange);
    if (bFileChange)
    {
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();
    }
    return bRes;
}


// Test a meta value if it exists, else return the not found value
tCIDLib::TBoolean
TChunkedFile::bTestMetaValue(const  TString&            strKey
                            , const TString&            strCompare
                            , const tCIDLib::TBoolean   bIfNotFound
                            , const tCIDLib::TBoolean   bCaseSensitive) const
{
    return m_pchflchMeta->bTestValue(strKey, strCompare, bIfNotFound, bCaseSensitive);
}


//
//  Return the number of data bytes in the indicated chunk. We throw if the chunk
//  is not found.
//
tCIDLib::TCard4 TChunkedFile::c4DataBytesById(const TString& strId) const
{
    // Throw if not found
    const TChunkedFileChunk* pchflchFind = pchflchFindById(strId, kCIDLib::True);
    return pchflchFind->c4Bytes();
}


// Return an estimate of the bytes required to hold our flattened representation
tCIDLib::TCard4 TChunkedFile::c4FullSzEstimate() const
{
    tCIDLib::TCard4 c4Ret = 0;

    // Go through the non-meta chunks. Do data first
    c4Ret = m_colChunks[1]->c4Bytes();

    // Then any extension chunks
    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        c4Ret += m_colChunks[c4Index]->c4Bytes();

    // Do the meta values
    c4Ret += m_pchflchMeta->c4EstBytes();

    // Throw in some for streaming overhead and such
    return c4Ret + 128;
}


//
//  One of the convenient ways to access a chunk. We throw if the id isn't found. In
//  this case the caller needs a copy of the data, not just to see it.
//
tCIDLib::TCard4
TChunkedFile::c4QueryChunkById(const TString& strChunkId, TMemBuf& mbufToFill) const
{
    const TChunkedFileChunk* pchflchFind = pchflchFindById(strChunkId, kCIDLib::True);

    const tCIDLib::TCard4 c4Bytes = pchflchFind->c4Bytes();
    if (mbufToFill.c4Size() < c4Bytes)
        mbufToFill.Reallocate(c4Bytes, kCIDLib::False);
    mbufToFill.CopyIn(pchflchFind->mbufData(), c4Bytes);

    return c4Bytes;
}


//
//  Empties out all our of chunks. If this changes a data change marked chunk then
//  the serial number and last change are bumped. If you want a full reset back to
//  scratch, use Reset().
//
tCIDLib::TVoid TChunkedFile::ClearAllData()
{
    tCIDLib::TBoolean bBumped = kCIDLib::False;
    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colChunks[c4Index]->bClearData())
        {
            // If it's a data change type, then bump the serial number/stamp
            if (m_colChunks[c4Index]->bIsDataChange())
            {
                // Only needs to be done once
                if (!bBumped)
                {
                    m_c4SerialNum++;
                    m_enctLastChange = TTime::enctNowUTC();
                    bBumped = kCIDLib::True;
                }
            }
        }
    }
}


//
//  Clear the indicated chunk. If not found, we throw. If a data change chunk is
//  changed, the serial number/stamp are bumped.
//
tCIDLib::TVoid TChunkedFile::ClearChunkById(const TString& strId)
{
    // Throw if not found
    TChunkedFileChunk* pchflchFind = pchflchFindById(strId, kCIDLib::True);
    if (pchflchFind->bClearData())
    {
        if (pchflchFind->bIsDataChange())
        {
            m_c4SerialNum++;
            m_enctLastChange = TTime::enctNowUTC();
        }
    }
}

//
//  Clears just the data chunk. If not already clear, then this is a change and we
//  bump the serial number and change time stamp.
//
tCIDLib::TVoid TChunkedFile::ClearDataChunk()
{
    if (m_colChunks[1]->bClearData())
    {
        m_c4SerialNum++;
        m_enctLastChange = TTime::enctNowUTC();
    }
}


// Return buffer info on a chunk by index
const TMemBuf&
TChunkedFile::mbufChunkAt(  const   tCIDLib::TCard4     c4At
                            ,       TString&            strId
                            ,       tCIDLib::TCard4&    c4Bytes) const
{
    if (c4At >= m_colChunks.c4ElemCount())
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_BadChunkIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TCardinal(m_colChunks.c4ElemCount())
        );
    }

    const TChunkedFileChunk* pchflchFind = m_colChunks[c4At];
    strId = pchflchFind->strId();
    c4Bytes = pchflchFind->c4Bytes();
    return pchflchFind->mbufData();
}


// Return buffer info on the indicated chunk
const TMemBuf&
TChunkedFile::mbufChunkById(const TString& strId, tCIDLib::TCard4& c4Bytes) const
{
    // Throw if not found
    const TChunkedFileChunk* pchflchFind = pchflchFindById(strId, kCIDLib::True);

    c4Bytes = pchflchFind->c4Bytes();
    return pchflchFind->mbufData();
}


// Return the info on the chunk at the indicated index in our list
tCIDLib::TVoid
TChunkedFile::QueryChunkAt( const   tCIDLib::TCard4&    c4At
                            ,       TString&            strChunkId
                            ,       TMemBuf&            mbufData
                            ,       tCIDLib::TCard4&    c4Bytes) const
{
    if (c4At >= m_colChunks.c4ElemCount())
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_BadChunkIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TCardinal(m_colChunks.c4ElemCount())
        );
    }

    const TChunkedFileChunk* pchflchFind = m_colChunks[c4At];
    strChunkId = pchflchFind->strId();
    c4Bytes = pchflchFind->c4Bytes();
    if (mbufData.c4Size() < c4Bytes)
        mbufData.Reallocate(c4Bytes, kCIDLib::False);
    mbufData.CopyIn(pchflchFind->mbufData(), c4Bytes);
}



// Query the list of chunk ids and their respective byte sizes
tCIDLib::TVoid
TChunkedFile::QueryChunkSizes(  tCIDLib::TStrList&      colChunkIds
                                , tCIDLib::TCardList&   fcolChunkSizes) const
{
    colChunkIds.RemoveAll();
    fcolChunkSizes.RemoveAll();

    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TChunkedFileChunk* pchflchCur = m_colChunks[c4Index];
        colChunkIds.objAdd(pchflchCur->strId());
        fcolChunkSizes.c4AddElement(pchflchCur->c4Bytes());
    }
}



//
//  Resets us back to default ctor state. This will NOT show up as a change even if
//  there was previous data, since we are going back to scratch. We have another that
//  will reset us with initial values, which can be used to convert from old formats
//  easily.
//
//  If you just want to clear out the data use ClearAllData(), ClearDataChunk(), or
//  ClearChunkById().
//
tCIDLib::TVoid TChunkedFile::Reset(const tCIDLib::TCard4 c4SerialNum)
{
    // Clear our current lists first
    m_colChunks.RemoveAll();
    m_colLastHashes.RemoveAll();
    m_pchflchMeta = nullptr;

    // And create default ones with empty data and meta
    tCIDLib::TKVPFList colMeta;
    THeapBuf mbufData(1);
    MakeDefChunks(0, mbufData, colMeta);

    // Now reset our serial number and stamp
    m_c4SerialNum = c4SerialNum;
    m_enctLastChange = TTime::enctNow();
}

tCIDLib::TVoid
TChunkedFile::Reset(const   tCIDLib::TCard4     c4Bytes
                    , const TMemBuf&            mbufData
                    , const tCIDLib::TKVPFList& colMeta
                    , const   tCIDLib::TCard4   c4SerialNum)
{
    // Clear our current lists first
    m_colChunks.RemoveAll();
    m_colLastHashes.RemoveAll();
    m_pchflchMeta = nullptr;

    // And create default ones with the passed data
    MakeDefChunks(c4Bytes, mbufData, colMeta);

    // Now reset our serial number and stamp to the passed values
    m_c4SerialNum = c4SerialNum;
    m_enctLastChange = TTime::enctNow();
}


// ---------------------------------------------------------------------------
//  TChunkedFile: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We deal with the on-disk format here, and just feed data to the chunks after we
//  read it out of the file, and vice versa when writing. We can use our own streams
//  for this file format since we define it to be little endian, which is what our
//  streams expect.
//
//  To avoid leaving us in a bad state, we stream to temp info first
//
tCIDLib::TVoid TChunkedFile::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Validate the header and get the info out of it
    tCIDLib::TCard4 c4SerialNum;
    tCIDLib::TEncodedTime enctLast;
    tCIDLib::TCard4 c4ChunkCnt;
    StreamInHdr(strmToReadFrom, c4SerialNum, enctLast, c4ChunkCnt);

    // Use a helper to stream in the chunks
    TChunkList colChunks(tCIDLib::EAdoptOpts::Adopt, c4ChunkCnt);
    THashList  colHashes(c4ChunkCnt);
    tCIDLib::TCard4 c4ChunkSz;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChunkCnt; c4Index++)
    {
        TChunkedFileChunk* pchflchNew = pchflchStreamInChunk(strmToReadFrom, c4ChunkSz);
        colChunks.Add(pchflchNew);
        colHashes.objAdd(pchflchNew->mhashChunk());
    }

    // If debugging, check the special ones
    CIDAssert
    (
        colChunks[0]->strId() == kCIDMData::strChunkId_Meta
        , L"The first file chunk is not the meta chunk"
    );
    CIDAssert
    (
        (colChunks[1]->strId() == kCIDMData::strChunkId_Data)
        , L"The second file chunk is not the data chunk"
    );

    tCIDLib::TCard4 c4EndId;
    strmToReadFrom >> c4EndId;
    if (c4EndId != kCIDMData::c4ChunkFlId_FileEnd)
        ThrowFmtErr(L"Did not find file end marker", CID_LINE);

    //
    //  It worked, so store the new data. We just ask our temp chunk list to give  all
    //  of its elements to our member list to avoid duplicating the data.
    //
    m_colChunks.RemoveAll();
    colChunks.GiveAllTo(m_colChunks);
    m_colLastHashes = colHashes;
    m_c4SerialNum = c4SerialNum;
    m_enctLastChange = enctLast;

    // The serial number can never be 0, so just in case
    if (m_c4SerialNum == 0)
        m_c4SerialNum = 1;

    // Set the special metadata chunk pointer
    m_pchflchMeta = static_cast<TChunkedFileMeta*>(m_colChunks[0]);
}


tCIDLib::TVoid TChunkedFile::StreamTo(TBinOutStream& strmToWriteTo) const
{
    const tCIDLib::TCard1 c1ChunkCnt = tCIDLib::TCard1(m_colChunks.c4ElemCount());

    // Stream out the header
    StreamOutHdr(strmToWriteTo, m_c4SerialNum, m_enctLastChange, c1ChunkCnt);

    // Now write the chunks, all of which are the same
    for (tCIDLib::TCard1 c1Index = 0; c1Index < c1ChunkCnt; c1Index++)
        StreamOutChunk(strmToWriteTo, *m_colChunks[c1Index]);

    // End off with a file end marker
    strmToWriteTo << kCIDMData::c4ChunkFlId_FileEnd;

    //  Remember the current chunk hashes as the last stored ones
    for (tCIDLib::TCard1 c1Index = 0; c1Index < c1ChunkCnt; c1Index++)
        m_colLastHashes[c1Index] = m_colChunks[c1Index]->mhashChunk();
}


// ---------------------------------------------------------------------------
//  TChunkedFile: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Sets up the two chunks we always have by default
tCIDLib::TVoid
TChunkedFile::MakeDefChunks(const   tCIDLib::TCard4     c4Bytes
                            , const TMemBuf&            mbufData
                            , const tCIDLib::TKVPFList& colMeta)
{
    m_pchflchMeta = new TChunkedFileMeta(colMeta);
    m_colChunks.Add(m_pchflchMeta);
    m_colChunks.Add(new TChunkedFileData(c4Bytes, mbufData));

    // Add the last hashes for these guys
    m_colLastHashes.objAdd(m_colChunks[0]->mhashChunk());
    m_colLastHashes.objAdd(m_colChunks[1]->mhashChunk());
}


TChunkedFileChunk*
TChunkedFile::pchflchFindById(const TString& strToFind, const tCIDLib::TBoolean bThrowIfNot)
{
    const tCIDLib::TCard4 c4Count = m_colChunks.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colChunks[c4Index]->strId().bCompareI(strToFind))
            return m_colChunks[c4Index];
    }

    if (bThrowIfNot)
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcChFl_ChunkNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
        );
    }

    return nullptr;
}

const TChunkedFileChunk*
TChunkedFile::pchflchFindById(  const   TString&            strToFind
                                , const tCIDLib::TBoolean   bThrowIfNot) const
{
    return const_cast<TChunkedFile*>(this)->pchflchFindById(strToFind, bThrowIfNot);
}


// ---------------------------------------------------------------------------
//  TChunkedFile: Private, static methods
// ---------------------------------------------------------------------------

// All chunks are the same on disk so we have a helper to do this
TChunkedFileChunk*
TChunkedFile::pchflchStreamInChunk(TBinInStream& strmSrc, tCIDLib::TCard4& c4Bytes)
{
    tCIDLib::TCard1 c1Marker;

    strmSrc >> c1Marker;
    if (c1Marker != kCIDMData::c1ChunkFlId_ChunkStart)
    {
        ThrowFmtErr
        (
            L"Expected chunk start marker but got %(1)"
            , CID_LINE
            , TCardinal(c1Marker)
        );
    }

    // Get the chunk id bytes count
    tCIDLib::TCard1 c1IdBytes, c1IdXOR;
    strmSrc >> c1IdBytes >> c1IdXOR;
    if ((c1IdBytes ^ 0xFF) != c1IdXOR)
    {
        ThrowFmtErr(L"The file chunk's id length was not valid", CID_LINE);
    }

    //
    //  Get the bytes, which are ASCII chars and create a string. We have to null
    //  terminate it so add one to the buffer size.
    //
    tCIDLib::TSCh* pschBuf = new tCIDLib::TSCh[c1IdBytes + 1];
    TArrayJanitor<tCIDLib::TSCh> janIdBuf(pschBuf);
    strmSrc.c4ReadRawBuffer(pschBuf, c1IdBytes);
    pschBuf[c1IdBytes] = 0;
    TString strId(pschBuf);

    // Get the flags
    tCIDLib::TCard1 c1Flags;
    strmSrc >> c1Flags;
    const tCIDLib::TBoolean bDataChange((c1Flags & kCIDMData::c1ChunkFlag_DataChange) != 0);

    // We should have a frame marker now
    strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

    // Get the number of bytes in the chunk
    strmSrc >> c4Bytes;

    // Read in the hash bytes and create the hash
    tCIDLib::TCard1 ac1Hash[kCIDCrypto::c4SHA1HashBytes];
    strmSrc.c4ReadRawBuffer(ac1Hash, kCIDCrypto::c4SHA1HashBytes);
    TSHA1Hash mhashStored(ac1Hash, kCIDCrypto::c4SHA1HashBytes);

    // We should have a frame marker now
    strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

    //
    //  Create a buffer and read in the data. The size may be zero, so set them to
    //  bytes plus one to be safe. The actual byte count value is all that really
    //  matters so this doesn't confuse anyone.
    //
    THeapBuf mbufData(c4Bytes + 1, c4Bytes + 1);
    strmSrc.c4ReadBuffer(mbufData, c4Bytes);

    // Now let's create the chunk. It's either one of our built in ones or an extension
    TChunkedFileChunk* pchflchRet = nullptr;
    if (strId.bCompareI(kCIDMData::strChunkId_Data))
        pchflchRet = new TChunkedFileData();
    else if (strId.bCompareI(kCIDMData::strChunkId_Meta))
        pchflchRet = new TChunkedFileMeta();
    else
        pchflchRet = new TChunkedFileExt(strId, bDataChange);

    // Put a janitor on it till we get it set up and safely returned
    TJanitor<TChunkedFileChunk> janChunk(pchflchRet);

    // Now we can let the chunk object stream itself in from the data
    pchflchRet->bSetData(c4Bytes, mbufData);

    // Check its hash against the stored one
    if (mhashStored != pchflchRet->mhashChunk())
    {
        ThrowFmtErr
        (
            L"The stored hash for the '%(1)' file chunk != the generated one"
            , CID_LINE
            , strId
        );
    }

    // And we should get an end chunk
    strmSrc >> c1Marker;
    if (c1Marker != kCIDMData::c1ChunkFlId_ChunkEnd)
    {
        ThrowFmtErr
        (
            L"Expected chunk end marker but got %(1), chunk=%(2)"
            , CID_LINE
            , TCardinal(c1Marker)
            , strId
        );
    }

    // And we can orphan it out and return it now
    return janChunk.pobjOrphan();
}


//
//  Streams in and validates the file header part of the file, returning the two
//  actual data values in the header.
//
tCIDLib::TVoid
TChunkedFile::StreamInHdr(  TBinInStream&               strmSrc
                            , tCIDLib::TCard4&          c4SerialNum
                            , tCIDLib::TEncodedTime&    enctLastChange
                            , tCIDLib::TCard4&          c4ChunkCnt)
{
    tCIDLib::TCard1 c1Val;
    tCIDLib::TCard4 c4Val;

    // Check the file format marker at the start. If not valid, we are doomed
    strmSrc >> c4Val;
    if (c4Val != kCIDMData::c4ChunkFlId_FileStart)
        ThrowFmtErr(L"Did not find file start marker", CID_LINE);

    // Get the format version
    strmSrc >> c1Val;
    if (c1Val != kCIDMData::c1ChunkFl_FmtVersion)
        ThrowFmtErr(L"Unexpected file version %(1)", CID_LINE, TCardinal(c1Val));

    //
    //  We should see a frame marker, then the serial number, last change, then the
    //  count of chunks and the same XORed with 0xFF.
    //
    strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

    tCIDLib::TCard1 c1ChunkCnt, c1CntXOR;
    strmSrc >> c4SerialNum
            >> enctLastChange
            >> c1ChunkCnt
            >> c1CntXOR;

    if ((c1ChunkCnt ^ 0xFF) != c1CntXOR)
    {
        ThrowFmtErr(L"The file chunk count was invalid", CID_LINE);
    }

    // Looks ok so give it back
    c4ChunkCnt = c1ChunkCnt;

    // And it ends with a frame marker
    strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);
}


//
//  Streams out the standard header info. At that point the stream is ready to stream
//  out the first chunk.
//
tCIDLib::TVoid
TChunkedFile::StreamOutHdr(         TBinOutStream&          strmTar
                            , const tCIDLib::TCard4         c4SerialNum
                            , const tCIDLib::TEncodedTime   enctLastChange
                            , const tCIDLib::TCard1         c1ChunkCnt)
{
    strmTar << kCIDMData::c4ChunkFlId_FileStart
            << kCIDMData::c1ChunkFl_FmtVersion
            << tCIDLib::EStreamMarkers::Frame
            << c4SerialNum
            << enctLastChange
            << c1ChunkCnt
            << tCIDLib::TCard1(c1ChunkCnt ^ 0xFF)
            << tCIDLib::EStreamMarkers::Frame;
}


// All chunks are the same on disk so we have a helper to do this
tCIDLib::TVoid
TChunkedFile::StreamOutChunk(       TBinOutStream&      strmTar
                            , const TChunkedFileChunk&  chflchSrc)
{
    const tCIDLib::TCard1 c1IdBytes = tCIDLib::TCard1(chflchSrc.strId().c4Length());
    strmTar << kCIDMData::c1ChunkFlId_ChunkStart
            << tCIDLib::TCard1(c1IdBytes)
            << tCIDLib::TCard1(c1IdBytes ^ 0xFF);

    // Write out the id characters as ASCII bytes
    for (tCIDLib::TCard1 c1Index = 0; c1Index < c1IdBytes; c1Index++)
        strmTar << tCIDLib::TCard1(chflchSrc.strId()[c1Index]);

    // Set up the flags
    tCIDLib::TCard1 c1Flags = 0;
    if (chflchSrc.bIsDataChange())
        c1Flags |= kCIDMData::c1ChunkFlag_DataChange;

    // Now we can spit out the flags a marker, then the data bytes count
    strmTar << c1Flags
            << tCIDLib::EStreamMarkers::Frame
            << chflchSrc.c4Bytes();

    // Write out the hash bytes
    CIDAssert(chflchSrc.mhashChunk().c4Bytes() == 20, L"File chunk hash is not 20 bytes");
    strmTar.c4WriteRawBuffer
    (
        chflchSrc.mhashChunk().pc1Buffer(), chflchSrc.mhashChunk().c4Bytes()
    );

    // Do a frame marker and then the chunk bytes
    strmTar << tCIDLib::EStreamMarkers::Frame;
    strmTar.c4WriteBuffer(chflchSrc.mbufData(), chflchSrc.c4Bytes());

    // And it all ends with a chunk end id marker
    strmTar << kCIDMData::c1ChunkFlId_ChunkEnd;
}


//
//  A helper since have a lot of places during reading where we may not be happy with the
//  format and need to throw an error.
//
tCIDLib::TVoid
TChunkedFile::ThrowFmtErr(  const   tCIDLib::TCh* const pszAltText
                            , const tCIDLib::TCard4     c4Line
                            , const MFormattable&       mfmtblToken1
                            , const MFormattable&       mfmtblToken2
                            , const MFormattable&       mfmtblToken3)
{
    facCIDMData().ThrowErr
    (
        CID_FILE
        , c4Line
        , kMDataErrs::errcChFl_ReadFmtError
        , pszAltText
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , mfmtblToken1
        , mfmtblToken2
        , mfmtblToken3
    );
}
