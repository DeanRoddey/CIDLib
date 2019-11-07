//
// FILE NAME: CtrlsWnd_Clipboard.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/10/2002
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
//  This file implements the GUI clipboard access class
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGUIClipboard,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCtrls_Clipboard
{
    // -----------------------------------------------------------------------
    //  We have to fault in the registration of our custom types
    // -----------------------------------------------------------------------
    TAtomicFlag         atomInitDone;
    tCIDLib::TCard4     c4CIDStringListType;
    const tCIDLib::TCh* pszCIDStringListTypeName= L"CIDLib.StringList.ClipboardFormat";
    tCIDLib::TCard4     c4CIDObjectType;
    const tCIDLib::TCh* pszCIDObjectTypeName    = L"CIDLib.TObject.ClipboardFormat";
    tCIDLib::TCard4     c4CIDClassType;
    const tCIDLib::TCh* pszCIDClassTypeName     = L"CIDLib.TClass.ClipboardFormat";
    tCIDLib::TCard4     c4CIDFlatObjType;
    const tCIDLib::TCh* pszCIDFlatObjTypeName   = L"CIDLib.TFlatObj.ClipboardFormat";
}


//
//  In debug mode, force instantionation of templatized code in this class,
//  so we can catch errors faster.
//
#if CID_DEBUG_ON
void FooBar()
{
    TWindow* pwndTmp = new TCheckBox();
    TGUIClipboard gclipTmp(*pwndTmp);
    TArea* pareaTmp = new TArea();

    gclipTmp.WriteObj(*pareaTmp);
    if (!gclipTmp.bReadObj<TArea>(pareaTmp))
    {
    }
}
#endif


// ---------------------------------------------------------------------------
//  CLASS: TGUIClipboard
// PREFIX: gclip
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIClipboard: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIClipboard::TGUIClipboard(const TWindow& wndOwner)
{
    // If we've not faulted in our type registration, then do that
    if (!CIDCtrls_Clipboard::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDCtrls_Clipboard::atomInitDone)
        {
            CIDCtrls_Clipboard::c4CIDObjectType = ::RegisterClipboardFormat
            (
                CIDCtrls_Clipboard::pszCIDObjectTypeName
            );
            if (!CIDCtrls_Clipboard::c4CIDObjectType)
            {
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcClip_CantRegisterType
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TString(CIDCtrls_Clipboard::pszCIDObjectTypeName)
                );
            }

            CIDCtrls_Clipboard::c4CIDStringListType = ::RegisterClipboardFormat
            (
                CIDCtrls_Clipboard::pszCIDStringListTypeName
            );
            if (!CIDCtrls_Clipboard::c4CIDStringListType)
            {
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcClip_CantRegisterType
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TString(CIDCtrls_Clipboard::pszCIDStringListTypeName)
                );
            }

            CIDCtrls_Clipboard::c4CIDClassType = ::RegisterClipboardFormat
            (
                CIDCtrls_Clipboard::pszCIDClassTypeName
            );
            if (!CIDCtrls_Clipboard::c4CIDClassType)
            {
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcClip_CantRegisterType
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TString(CIDCtrls_Clipboard::pszCIDClassTypeName)
                );
            }

            CIDCtrls_Clipboard::c4CIDFlatObjType = ::RegisterClipboardFormat
            (
                CIDCtrls_Clipboard::pszCIDFlatObjTypeName
            );
            if (!CIDCtrls_Clipboard::c4CIDFlatObjType)
            {
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcClip_CantRegisterType
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TString(CIDCtrls_Clipboard::pszCIDFlatObjTypeName)
                );
            }
        }
        CIDCtrls_Clipboard::atomInitDone.Set();
    }

    if (!::OpenClipboard(wndOwner.hwndThis()))
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_OpenFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

TGUIClipboard::~TGUIClipboard()
{
    if (!::CloseClipboard())
    {
        // Not much we can do, but log it
        facCIDCtrls().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );

    }
}


// ---------------------------------------------------------------------------
//  TGUIClipboard: Constructors and Destructor
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGUIClipboard::bGetAsBitmap(        TBitmap&            bmpToFill
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Make sure that this format is available
    const tCIDLib::TBoolean bRet = bIsFormatAvailable(tCIDCtrls::EClipFormats::Bitmap);

    if (bRet)
    {
        // Get a copy of the bitmap handle
        tCIDGraphDev::TBmpHandle hbmpSrc = tCIDGraphDev::TBmpHandle
        (
            ::GetClipboardData(CF_BITMAP)
        );

        // If we didn't get it, then throw
        if (!hbmpSrc)
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_CantGetCBData
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }

        // Don't adopt, copy!
        bmpToFill = TBitmap(hbmpSrc, tCIDLib::EAdoptOpts::NoAdopt);
    }
     else
    {
        // The format we want isn't there, so throw if they want
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }
    return bRet;
}


tCIDLib::TBoolean
TGUIClipboard::bGetAsClass(         TString&            strToFill
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Make sure that this format is available
    const tCIDLib::TBoolean bRet = bIsFormatAvailable(tCIDCtrls::EClipFormats::Class);

    if (bRet)
    {
        // The data should be a memory handle
        HGLOBAL hMem = (HGLOBAL)::GetClipboardData(CIDCtrls_Clipboard::c4CIDClassType);
        if (!hMem)
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_CantGetCBData
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );

            // Won't happen, but makes analyzer happy
            return kCIDLib::False;
        }

        const tCIDLib::TCh* pszData = (tCIDLib::TCh*)::GlobalLock(hMem);
        strToFill = pszData;
        ::GlobalUnlock(hMem);
    }
     else
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }
    return bRet;
}



tCIDLib::TBoolean
TGUIClipboard::bGetAsFlatObject(const   TString&            strTypeId
                                ,       TMemBuf&            mbufToFill
                                ,       tCIDLib::TCard4&    c4BytesRead
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Make sure that this format is available
    const tCIDLib::TBoolean bRet = bIsFormatAvailable(tCIDCtrls::EClipFormats::FlatObj);
    if (bRet)
    {
        //
        //  It's there, so make sure the flattened data name is right. We use
        //  the class type thing, even though it's really not one.
        //
        TString strActTypeId;
        if (!bGetAsClass(strActTypeId))
            return kCIDLib::False;

        //
        //  See if the same interface as the passed object. If not, then
        //  return failure. They must extract using the same base class
        //  that they used to store.
        //
        if (strActTypeId != strTypeId)
            return kCIDLib::False;

        // Looks ok, so read the bytes
        c4BytesRead = c4GetData(mbufToFill, CIDCtrls_Clipboard::c4CIDFlatObjType);
    }
     else
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }
    return bRet;
}


tCIDLib::TBoolean
TGUIClipboard::bGetAsFileDropList(          tCIDLib::TStrCollect&   colToFill
                                    , const tCIDLib::TBoolean       bThrowIfNot)
{
    colToFill.RemoveAll();

    if (!::IsClipboardFormatAvailable(CF_HDROP))
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
        return kCIDLib::False;
    }

    HDROP hDrop = (HDROP)::GetClipboardData(CF_HDROP);
    if (!hDrop)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4CurSz = kCIDLib::c4MaxPathLen;
    tCIDLib::TCh* pszBuf = new tCIDLib::TCh[c4CurSz + 1];
    TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);

    // See how many files there are in the list
    const tCIDLib::TCard4 c4Count = ::DragQueryFile(hDrop, 0xFFFFFFFF, 0, c4CurSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // Get the reqired size of this one
        tCIDLib::TCard4 c4Needed = ::DragQueryFile(hDrop, c4Index, 0, c4CurSz);
        if (c4Needed > c4CurSz)
        {
            // Reallocate the buffer
            c4CurSz = c4Needed;
            pszBuf = new tCIDLib::TCh[c4CurSz + 1];
            janBuf.Set(pszBuf);
        }

        // Query the value of this one and store it
        ::DragQueryFile(hDrop, c4Index, pszBuf, c4CurSz);
        colToFill.objAdd(pszBuf);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TGUIClipboard::bGetAsStringList(        tCIDLib::TStrCollect&   colToFill
                                , const tCIDLib::TBoolean       bThrowIfNot)
{
    colToFill.RemoveAll();

    // Make sure that this format is available
    const tCIDLib::TBoolean bRet = bIsFormatAvailable(tCIDCtrls::EClipFormats::StringList);
    if (bRet)
    {
        // Get a copy of the data
        THeapBuf mbufData;
        const tCIDLib::TCard4 c4Len = c4GetData
        (
            mbufData, CIDCtrls_Clipboard::c4CIDStringListType
        );

        //
        //  Stream it in by wrapping an input stream around the buffer. The
        //  default is not to adopt, which is what we want.
        //
        TBinMBufInStream strmIn(&mbufData, c4Len);
        strmIn.Reset();

        tCIDLib::TCard4 c4Count, c4XORCount;
        strmIn >> c4Count >> c4XORCount;

        if (c4Count != (c4XORCount ^ 0xFFFFFFFF))
            return kCIDLib::False;

        TString strCur;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmIn >> strCur;
            colToFill.objAdd(strCur);
        }
    }
     else
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }
    return bRet;
}


tCIDLib::TBoolean
TGUIClipboard::bGetAsText(          TString&            strToFill
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Make sure that this format is available
    const tCIDLib::TBoolean bRet = bIsFormatAvailable(tCIDCtrls::EClipFormats::Text);

    if (bRet)
    {
        // The data should be a memory handle
        HGLOBAL hMem = (HGLOBAL)::GetClipboardData(CF_UNICODETEXT);
        if (!hMem)
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_CantGetCBData
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );

            // Won't happen, but makes the analyzer
            return kCIDLib::False;
        }

        tCIDLib::TCh* pszData = (tCIDLib::TCh*)::GlobalLock(hMem);
        CIDAssert(pszData != nullptr, L"Could not get global clipboard memory pointer");
        try
        {
            const tCIDLib::TCard4 c4MaxLen = 64 * (1024 * 1024);
            if (TRawStr::c4StrLen(pszData) > c4MaxLen)
                pszData[c4MaxLen] = kCIDLib::chNull;

            strToFill = pszData;
            ::GlobalUnlock(hMem);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ::GlobalUnlock(hMem);
            throw;
        }
    }
     else
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcClip_FormatNotAvail
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }
    return bRet;
}


tCIDLib::TBoolean
TGUIClipboard::bIsFormatAvailable(const tCIDCtrls::EClipFormats eToCheck) const
{
    // Convert our enum to the internal format identifier
    const tCIDLib::TCard4 c4IntId = c4XlatFormat(eToCheck);

    if (::IsClipboardFormatAvailable(c4IntId))
        return kCIDLib::True;

    return kCIDLib::False;
}


tCIDLib::TVoid TGUIClipboard::Clear()
{
    if (!::EmptyClipboard())
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_Clear
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid TGUIClipboard::StoreBitmap(const TBitmap& bmpToStore)
{
    //
    //  Make a copy of the bitmap handle and store it. For this one, since its
    //  already a handle, we can store it directly.
    //
    tCIDGraphDev::TBmpHandle hbmpCopy = TBitmap::hbmpDupHandle(bmpToStore.hbmpThis());
    if (!::SetClipboardData(CF_BITMAP, hbmpCopy))
    {
        // It didn't work so lets free the copy
        ::DeleteObject(hbmpCopy);

        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_CantStoreHandle
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid
TGUIClipboard::StoreFlatObject( const   TString&        strTypeId
                                , const TMemBuf&        mbufData
                                , const tCIDLib::TCard4 c4Bytes)
{
    // Call the basic storage method to store it with the flattend object type
    StoreData(mbufData, c4Bytes, CIDCtrls_Clipboard::c4CIDFlatObjType);

    //
    //  And store the type id as a class type. It's not really a class type,
    //  it is just an indicator of what's flattened in the buffer stored.
    //  The caller should be careful to make it non-ambiguous and not to
    //  match an actual class name.
    //
    StoreStringData
    (
        reinterpret_cast<const tCIDLib::TCard1*>(strTypeId.pszBuffer())
        , (strTypeId.c4Length() + 1) * kCIDLib::c4CharBytes
        , CIDCtrls_Clipboard::c4CIDClassType
    );
}


tCIDLib::TVoid
TGUIClipboard::StoreStringList(const tCIDLib::TStrCollect& colToStore)
{
    tCIDLib::TCard4 c4Len;
    THeapBuf        mbufData(4096);

    //
    //  Use a binary memory stream to flatten the data into the buffer. It
    //  doesn't adopt by default, which is what we want.
    //
    {
        TBinMBufOutStream strmTmp(&mbufData);

        //
        //  We don't stream the collection itself. That is irrelevant. We just
        //  stream out all the strings. We have to store first though how many
        //  we streamed out.
        //
        //  For safety, we also stream out a XOR'd version, just to make sure
        //  we aren't getting junk when we pull it back in.
        //
        const tCIDLib::TCard4 c4Count = colToStore.c4ElemCount();
        strmTmp << c4Count << (c4Count ^ kCIDLib::c4MaxCard);

        // And now dump out out the strings
        if (c4Count)
        {
            TColCursor<TString>* pcursList = colToStore.pcursNew();
            TJanitor<TColCursor<TString> > janCurs(pcursList);
            for (; pcursList->bIsValid(); pcursList->bNext())
                strmTmp << pcursList->objRCur();
        }

        // Flush it to make sure its all in the buffer, then store it
        strmTmp.Flush();

        // Get the length before we let the stream go
        c4Len = strmTmp.c4CurSize();
    }

    // And now store the data under the correct type
    StoreData(mbufData, c4Len, CIDCtrls_Clipboard::c4CIDStringListType);
}


tCIDLib::TVoid TGUIClipboard::StoreText(const TString& strToStore)
{
    StoreStringData
    (
        reinterpret_cast<const tCIDLib::TCard1*>(strToStore.pszBuffer())
        , (strToStore.c4Length() + 1) * kCIDLib::c4CharBytes
        , CF_UNICODETEXT
    );
}


// ---------------------------------------------------------------------------
//  TGUIClipboard: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TGUIClipboard::c4FmtCount() const
{
    return ::CountClipboardFormats();
}


tCIDLib::TCard4
TGUIClipboard::c4GetData(TMemBuf& mbufToFill, const tCIDLib::TCard4 c4Fmt)
{
    // The data should be a memory handle
    HGLOBAL hMem = (HGLOBAL)::GetClipboardData(c4Fmt);
    if (!hMem)
        return 0;

    //
    //  Get a pointer to the data. The first TCard4 will be the number of
    //  bytes to copy out, which we discard after we get it out, since its
    //  not part of the caller's data.
    //
    tCIDLib::TCard4* pc4Bytes = (tCIDLib::TCard4*)::GlobalLock(hMem);
    tCIDLib::TCard4 c4Ret = 0;
    try
    {
        if (pc4Bytes)
        {
            c4Ret = *pc4Bytes;
            mbufToFill.CopyIn(pc4Bytes + 1, c4Ret);
        }
        ::GlobalUnlock(hMem);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        ::GlobalUnlock(hMem);
        throw;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGUIClipboard::c4GetObjData(TMemBuf& mbufToFill)
{
    // Just pass on to the generic version, with the correct format
    return c4GetData(mbufToFill, CIDCtrls_Clipboard::c4CIDObjectType);
}


tCIDLib::TCard4
TGUIClipboard::c4XlatFormat(const tCIDCtrls::EClipFormats eToXlat) const
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eToXlat)
    {
        case tCIDCtrls::EClipFormats::Text :
            c4Ret = CF_UNICODETEXT;
            break;

        case tCIDCtrls::EClipFormats::Bitmap :
            c4Ret = CF_BITMAP;
            break;

        case tCIDCtrls::EClipFormats::FileDropList :
            c4Ret = CF_HDROP;
            break;

        case tCIDCtrls::EClipFormats::Object :
            // Its our customize format
            c4Ret = CIDCtrls_Clipboard::c4CIDObjectType;
            break;

        case tCIDCtrls::EClipFormats::StringList :
            // Its our customize format
            c4Ret = CIDCtrls_Clipboard::c4CIDStringListType;
            break;

        case tCIDCtrls::EClipFormats::Class :
            // Its our customize format
            c4Ret = CIDCtrls_Clipboard::c4CIDClassType;
            break;

        case tCIDCtrls::EClipFormats::FlatObj :
            // It's a flattened object
            c4Ret = CIDCtrls_Clipboard::c4CIDFlatObjType;
            break;

        default :
        {
            // We are throwing a CIDLib error here, not one of ours!
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TInteger(tCIDLib::i4EnumOrd(eToXlat))
                , TString(L"tCIDCtrls::EClipFormats")
            );
        }
    }
    return c4Ret;
}


tCIDLib::TVoid TGUIClipboard::StoreData(const   TMemBuf&        mbufToStore
                                        , const tCIDLib::TCard4 c4Bytes
                                        , const tCIDLib::TCard4 c4Fmt)
{
    // Make room to store the size in addition to the data bytes
    HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, c4Bytes + sizeof(c4Bytes));
    if (!hMem)
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_CantAlloc
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );

        // Won't happen, but makes analyzer nappy
        return;
    }

    //
    //  Get a writeable view of the data, copy our text into it, and unlock.
    //  we allocated it 4 bytes larger, so that we could store the size as
    //  the first field of the stored data.
    //
    tCIDLib::TCard1* pc1Target = static_cast<tCIDLib::TCard1*>(::GlobalLock(hMem));
    CIDAssert(pc1Target != nullptr, L"Could not get the global clipbard member pointer");

    *reinterpret_cast<tCIDLib::TCard4*>(pc1Target) = c4Bytes;
    mbufToStore.CopyOut(pc1Target + sizeof(c4Bytes), c4Bytes);
    ::GlobalUnlock(hMem);

    if (!::SetClipboardData(c4Fmt, hMem))
    {
        // It didn't work so lets free the data first, then throw
        ::GlobalFree(hMem);

        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_CantStoreMem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid
TGUIClipboard::StoreObjData(const   TMemBuf&        mbufToStore
                            , const tCIDLib::TCard4 c4Bytes
                            , const TString&        strClass)
{
    // Call the basic storage method to store it with the object type
    StoreData(mbufToStore, c4Bytes, CIDCtrls_Clipboard::c4CIDObjectType);

    // And store the class name as a class type
    StoreStringData
    (
        reinterpret_cast<const tCIDLib::TCard1*>(strClass.pszBuffer())
        , (strClass.c4Length() + 1) * kCIDLib::c4CharBytes
        , CIDCtrls_Clipboard::c4CIDClassType
    );
}


tCIDLib::TVoid
TGUIClipboard::StoreStringData( const   tCIDLib::TCard1* const  pc1Data
                                , const tCIDLib::TCard4         c4Bytes
                                , const tCIDLib::TCard4         c4Fmt)
{
    //
    //  This can be stored as a memory handle. Since its null terminated,
    //  we don't have to store a size into it.
    //
    HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, c4Bytes);
    if (!hMem)
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_CantAlloc
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );

        // Won't happen but makes analyzer happy
        return;
    }

    // Get a writeable view of the data, copy our text into it, and unlock
    tCIDLib::TVoid* pTarget = ::GlobalLock(hMem);
    TRawMem::CopyMemBuf(pTarget, pc1Data, c4Bytes);
    ::GlobalUnlock(hMem);

    if (!::SetClipboardData(c4Fmt, hMem))
    {
        // It didn't work so lets free the data first, then throw
        ::GlobalFree(hMem);

        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcClip_CantStoreMem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


