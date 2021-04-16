//
// FILE NAME: CIDLib_BinOutStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1999
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
//  This is the header for the CIDLib_BinOutStream.Hpp file, which implements
//  TBinOutStream class. This class provides the functionality of a write-
//  only, binary stream. There is a TBinInStream class for read-only, binary
//  streaming. Both of these classes work in terms of a 'stream implementation'
//  object, which is the actual source/sink for the data. So this class only
//  provides the logic, but then turns around and drives the implementation
//  object to do the actual movement of data.
//
//  Each derived class maintains some concept of current read position. It may
//  be indirectly, from a file it is reading from, or directly held in teh case
//  of memory buffer based stream.
//
//  c4ReadBytes - Reads from the current read position and updates it by the
//  number of bytes read.
//
//  Reset() - For read streams, just sets the read position back to the start.
//  It doesn't affect the end of data. It just starts reading from the start again.
//
//  SkipForwardBy - Moves the read position forward. It cannot be moved further
//  than the end of data.
//
//
//  Endianness
//
//  Binary streams have an endian setting. It defaults to little endian and is
//  normally not changed. Our canonical persistent format is little endian. But
//  for reading third party files that are big endian you can set the big endian
//  mode. If the current system endian type is not equal to this stream's endian
//  type, then swapping is done.
//
//  WE ASSUME that all data passed to be written is in the system endianness format.
//  We swap if the stream's format is not the same as the system's.
//
//
//  Pushback
//
//  We support pushback of data. We have a fundstack of bytes which can be used
//  used to push back bytes one at a time. We allow up to 8K of pushback.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  If an input stream is linked to an output stream, and the input stream
//      is used to reset the end of stream, that does not change the output
//      stream position. If it writes, it can write beyond the end of the current
//      end of data. So the output stream must deal with that by expanding the
//      data up to the write point, if the output stream is not also reset back
//      to the start.
//
//      And vice versa, if the output stream truncates or resets, the input stream
//      may have a read position past the end. In that case, any reads just return
//      zero bytes because the read position remains beyond the end of data.
//
//  2)  If on a non-Intel system, be sure to update TRawBits so that it the
//      floating point byte swap methods do the right thing for the local system
//      floating point format.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMemBuf;


// ---------------------------------------------------------------------------
//   CLASS: TInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TInStreamImpl : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TInStreamImpl(const TInStreamImpl&) = delete;
        TInStreamImpl(TInStreamImpl&&) = delete;

        ~TInStreamImpl() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TInStreamImpl& operator=(const TInStreamImpl&) = delete;
        TInStreamImpl& operator=(TInStreamImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsOpen() const = 0;

        virtual tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        ) = 0;

        virtual tCIDLib::TCard8 c8CurPos() const = 0;

        virtual tCIDLib::TVoid Reset() = 0;

        virtual tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TInStreamImpl() = default;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TInStreamImpl,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TBinInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinInStream : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard2 c2CheckFmtVersion
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard2         c2MaxVersion
            , const TClass&                 clsCaller
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );

        static tCIDLib::TCard2 c2CheckFmtVersion
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::EStreamMarkers eMarker
            , const tCIDLib::TCard2         c2MaxVersion
            , const TClass&                 clsCaller
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );

        static tCIDLib::TVoid CheckRelationship
        (
             const   TObject* const         pobjTest
             , const TClass&                clsTest
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinInStream
        (
                    TInStreamImpl* const    pstrmiToAdopt
        );

        TBinInStream(const TBinInStream&) = delete;
        TBinInStream(TBinInStream&&) = delete;

        ~TBinInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinInStream& operator>>
        (
            COP     tCIDLib::TBoolean&      bToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TCard1&        c1ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TCard2&        c2ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TCard4&        c4ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TCard8&        c8ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::EStreamMarkers& eMarker
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TFloat4&       f4ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TFloat8&       f8ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TInt1&         i1ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TInt2&         i2ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TInt4&         i4ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TInt8&         i8ToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TUInt&         uToFill
        );

        TBinInStream& operator>>
        (
            COP     tCIDLib::TSInt&         iToFill
        );

        TBinInStream& operator>>
        (
            COP      tCIDLib::TCh&           chToFill
        );

        TBinInStream& operator=(const TBinInStream&) = delete;
        TBinInStream& operator=(TBinInStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBigEndian() const
        {
            return (m_eEndianMode == tCIDLib::EEndianModes::Big);
        }

        tCIDLib::TBoolean bEndOfStream() const;

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bLittleEndian() const
        {
            return m_eEndianMode == tCIDLib::EEndianModes::Little;
        }

        tCIDLib::TCard4 c4CacheAvail
        (
            const   tCIDLib::TBoolean       bIncludePushback
        )   const;

        tCIDLib::TCard4 c4CurPos() const;

        tCIDLib::TCard4 c4Pushback
        (
            const   tCIDLib::TCard1         c1ToPush
        );

        tCIDLib::TCard4 c4Pushback
        (
            const   tCIDLib::TCard1* const  pc1ToPush
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TCard4 c4Pushback
        (
            const   TMemBuf&                mbufToPush
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TCard8 c8CurPos() const;

        tCIDLib::TCard4 c4ReadBuffer
        (
                    TMemBuf&                mbufTarget
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReadRawBuffer
        (
                    tCIDLib::TVoid* const   pBufToFill
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReadEnum();

        TClass clsReadClassInfo();

        tCIDLib::TVoid CheckForMarker
        (
            const  tCIDLib::EStreamMarkers  eMarker
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid CheckForFrameMarker( const   tCIDLib::TCh* const pszFile
                                            , const tCIDLib::TCard4     c4Line)
        {
            CheckForMarker(tCIDLib::EStreamMarkers::Frame, pszFile, c4Line);
        }

        tCIDLib::TVoid CheckForEndMarker(const  tCIDLib::TCh* const pszFile
                                        , const tCIDLib::TCard4     c4Line)
        {
            CheckForMarker(tCIDLib::EStreamMarkers::EndObject, pszFile, c4Line);
        }

        tCIDLib::TVoid CheckForStartMarker( const   tCIDLib::TCh* const pszFile
                                            , const tCIDLib::TCard4     c4Line)
        {
            CheckForMarker(tCIDLib::EStreamMarkers::StartObject, pszFile, c4Line);
        }

        tCIDLib::EEndianModes eEndianMode() const
        {
            return m_eEndianMode;
        }

        tCIDLib::EEndianModes eEndianMode
        (
            const   tCIDLib::EEndianModes   eToSet
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TBoolean* const abList
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TCard1* const  ac1List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TCard2* const  ac2List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TCard4* const  ac4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TCard8* const  ac8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4CharsToRead
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TInt1* const   ai1List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TInt2* const   ai2List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TInt4* const   ai4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TInt8* const   ai8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TFloat4* const af4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ReadArray
        (
                    tCIDLib::TFloat8* const af8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        );


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TBinInStream();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptImplObject
        (
                    TInStreamImpl* const    pstrmiToAdopt
        );

        tCIDLib::TVoid DeleteImplObject();

        tCIDLib::TVoid TrashCache();


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4CacheSz = 8192;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid RefreshCache() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  NOTE:   All of the cache members must be mutable because they
        //          have to be refreshed when bEndOfStream() is called to
        //          check for end of stream.
        //
        //          So the RefreshCache() method is const, but it updates
        //          these members.
        //
        //  m_ac1Cache
        //      This is the input cache used to save up data and write it to
        //      the underlying impl object in chunks. This is key to being
        //      able to get efficiency.
        //
        //  m_c4CurAvail
        //      The number of bytes available in the cache. Normally, we read
        //      in full cache chunks. But, the last chunk read will generally
        //      not be a full chunk so we have to add this complexity.
        //
        //  m_c4CurIndex
        //      The current index into the cache, i.e. where the next write
        //      will go (and hence also the current number of bytes in the
        //      cache.)
        //
        //  m_eEndianMode
        //      See file header. The default is little, since our canonical
        //      persistent object format is little endian. But, for reading
        //      big endian data you can change this.
        //
        //  m_exbpPushback
        //      We support a pushback stack. We only allow pushback a byte at a
        //      time, which we push onto the stack. Due to circular dependency
        //      issues (al the collection types need to stream themselves) we
        //      cannot use any existing collection types for this. So we use the
        //      expandable byte buffer class as a stack. If it does need streaming
        //      it can be out of line and use a forward ref.
        //
        //  m_pstrmiIn
        //      This is the impl object that we do all of our work through.
        //
        //  m_tcvtText
        //      We store tCIDLib::TCh data in UTF-8 format to minimize storage
        //      overhead (at a small price in CPU overhead on some platforms.)
        //      The overhead of creating this converter is low enough that we
        //      don't bother with any kind of lazy faulting in.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard1     m_ac1Cache[c4CacheSz];
        mutable tCIDLib::TCard4     m_c4CurAvail;
        mutable tCIDLib::TCard4     m_c4CurIndex;
        tCIDLib::EEndianModes       m_eEndianMode;
        TExpByteBuf                 m_expbPushback;
        TInStreamImpl*              m_pstrmiIn;
        TUTF8Converter              m_tcvtText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinInStream,TObject)
};


#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  To help the fundamental collections, we define a set of globals for array
//  reading, which just map to the members of the input stream above. This
//  allows us to generate other globals for things like enumerations so that
//  the fundamental collections can stream enum types as well.
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TBoolean* const abList
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(abList, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TCard1* const  ac1List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ac1List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TCard2* const  ac2List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ac2List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TCard4* const  ac4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ac4List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TCard8* const  ac8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ac8List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TFloat4* const af4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(af4List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TFloat8* const af8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(af8List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TCh* const     pszToFill
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(pszToFill, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TInt1* const   ai1List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ai1List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TInt2* const   ai2List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ai2List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TInt4* const   ai4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ai4List, c4Count);
}

inline tCIDLib::TVoid
TBinInStream_ReadArray(         TBinInStream&           strmSrc
                        ,       tCIDLib::TInt8* const   ai8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmSrc.ReadArray(ai8List, c4Count);
}



// ---------------------------------------------------------------------------
//  This is a template function that greatly simplifies streaming out objects
//  polymorphically. It keeps it all typesafe and makes sure that the object
//  gotten really supports the streaming operators. The actual streaming
//  operators are in CIDLib_Streamable.Hpp since they have to be friends of
//  the streamable mixin.
// ---------------------------------------------------------------------------
template <typename T> tCIDLib::TVoid
PolymorphicRead(T*& pobjToFill, TBinInStream& strmToReadFrom)
{
    // Init caller's pointer to 0 in case of failure
    pobjToFill = nullptr;

    //
    //  Stream in the class name for this object, and use that class
    //  name to create a new object, which we look at first as a
    //  void pointer.
    //
    TClass              clsType(strmToReadFrom.clsReadClassInfo());
    TObject* const      pobjNew = clsType.pobjMakeNew();
    TJanitor<TObject>   janNewObj(pobjNew);

    //
    //  Make sure that the new type has a valid relationship with the
    //  type of the pointer we have to fill in. The class type we read
    //  in must be a class of or derived from the instantiation type.
    //
    TBinInStream::CheckRelationship(pobjNew, T::clsThis());

    // Its ok so we can cast our pointer safely
    T* pobjTyped = static_cast<T*>(pobjNew);

    //
    //  And now let it stream itself in. If this class does not support
    //  the streaming API, it will be caught here at compile time.
    //
    strmToReadFrom >> *pobjTyped;

    // Orphan the new object and give back the pointer
    janNewObj.Orphan();
    pobjToFill = pobjTyped;
}


