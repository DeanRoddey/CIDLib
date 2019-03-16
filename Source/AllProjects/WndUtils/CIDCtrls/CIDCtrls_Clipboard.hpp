//
// FILE NAME: CIDCtrls_Clipboard.hpp
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
//  This is the header for the CIDCtrls_Clipboard.cpp file. This file implements
//  the TGUIClipboard class, which provides support for simple data transfer
//  operations.
//
//  We support a few standard host types, but mostly we just register a
//  type for flattened CIDLib objects, which is what most CIDLib objects will
//  pass around. The standard host types are mostly to allow us to get text,
//  images, and file drop lists in and out between non-CIDLib programs.
//
//  Clipboard objects are used on the fly, each time the user wants to do
//  a clipboard operation, because the existence of a clipboard object will
//  serialize access to the clipboard data. So create them on the stack in
//  response to user requests for clipboard operations.
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
//  CLASS: TGUIClipboard
// PREFIX: gclip
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGUIClipboard : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIClipboard() = delete;

        TGUIClipboard(const TGUIClipboard&) = delete;

        TGUIClipboard
        (
            const   TWindow&                wndOwner
        );

        ~TGUIClipboard();


        // -------------------------------------------------------------------
        //  We need a template member to put object onto the clipboard and
        //  to get objects off of it.
        // -------------------------------------------------------------------
        template <class T> tCIDLib::TBoolean bReadObj(T*& pobjToFill)
        {
            // See if a class value is stored. If so, get it, else we fail
            TString strClassName;
            if (!bGetAsClass(strClassName))
                return kCIDLib::False;

            //
            //  See if the same interface as the passed object. If not, then
            //  return failure. They must extract using the same base class
            //  that they used to store.
            //
            if (T::clsThis() != strClassName)
                return kCIDLib::False;

            // There's one there, so lets get the data
            THeapBuf mbufData;
            const tCIDLib::TCard4 c4Len = c4GetObjData(mbufData);

            //
            //  And do a polymorphic stream back in. If the stored object
            //  cannot be cast to the target type, it'll throw.
            //
            try
            {
                TBinMBufInStream strmIn(&mbufData, c4Len);
                ::PolymorphicRead<T>(pobjToFill, strmIn);
            }

            catch(...)
            {
                // Wasn't a legal cast
                return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <class T> tCIDLib::TVoid WriteObj(const T& objToWrite)
        {
            //
            //  We do the streaming at this level, then we can pass it off
            //  to an out of line method to do the storage. Do an initial
            //  1024 alloc, but it can grow up to a MB by default.
            //
            TBinMBufOutStream strmTmp(1024);
            ::PolymorphicWrite<T>(&objToWrite, strmTmp);
            strmTmp.Flush();

            //
            //  Ok now write it out. We store the flattened object as one
            //  format and the class name as another, not the actual derived
            //  class name but the class of T itself.
            //
            StoreObjData
            (
                strmTmp.mbufData()
                , strmTmp.c4CurSize()
                , T::clsThis().strClassName()
            );
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGUIClipboard& operator=(const TGUIClipboard&);


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetAsBitmap
        (
                    TBitmap&                bmpToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetAsClass
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetAsFileDropList
        (
                    tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetAsFlatObject
        (
            const   TString&                strExpTypeId
            ,       TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetAsStringList
        (
                    tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetAsText
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bIsFormatAvailable
        (
            const   tCIDCtrls::EClipFormats   eToCheck
        )   const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid StoreBitmap
        (
            const   TBitmap&                bmpToStore
        );

        tCIDLib::TVoid StoreFlatObject
        (
            const   TString&                strTypeId
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Bytes
        );

        tCIDLib::TVoid StoreStringList
        (
            const   tCIDLib::TStrCollect&   colToStore
        );

        tCIDLib::TVoid StoreText
        (
            const   TString&                strToStore
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4FmtCount() const;

        tCIDLib::TCard4 c4GetData
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4Fmt
        );

        tCIDLib::TCard4 c4GetObjData
        (
                    TMemBuf&                mbufToFill
        );

        tCIDLib::TCard4 c4XlatFormat
        (
            const   tCIDCtrls::EClipFormats   eToXlat
        )   const;

        tCIDLib::TVoid StoreObjData
        (
            const   TMemBuf&                mbufToStore
            , const tCIDLib::TCard4         c4Bytes
            , const TString&                strClass
        );

        tCIDLib::TVoid StoreData
        (
            const   TMemBuf&                mbufToStore
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Fmt
        );

        tCIDLib::TVoid StoreStringData
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Fmt
        );


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIClipboard,TObject)
};

#pragma CIDLIB_POPPACK


