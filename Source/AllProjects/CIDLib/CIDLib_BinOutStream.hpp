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
//  Each derivative will somehow keep up with a current output position. It may
//  be indirectly via a file being written to or directly held in the derived
//  class itself, in the case of a memory output impl.
//
//  c4WritesBytes - Outputs at the current output position, and updates it by the
//  number of bytes read.
//
//  c8CurPos - Returns the derived class' current output position
//
//  Reset - For an output stream, this trunciates teh end of data to zero and resets
//  the read position to zero.
//
//  SeekToEnd - Moves the output position to the end of the currently available data.
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
//  We swap if the stream's foramt is not the same as the system's.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  ALL writing must go through the c4WriteRawBuffer() method, because
//      that is where the cache management is done!!
//
//  2)  If an input stream is linked to an output stream, and the input stream
//      is used to reset the end of stream, that does not change the output
//      stream position. If it writes, it can write beyond the end of the current
//      end of data. So the output stream must deal with that by expanding the
//      data up to the write point, fi the output stream is not also reset back
//      to the start.
//
//      And vice versa, if the output stream truncates, the input stream may have
//      a read position past the end. In that case, any reads just return zero
//      bytes.
//
//  3)  If on a non-Intel system, be sure to update TRawBits so that it the
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
//   CLASS: TOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TOutStreamImpl : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TOutStreamImpl(const TOutStreamImpl&) = delete;
        TOutStreamImpl(TOutStreamImpl&&) = delete;

        ~TOutStreamImpl() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOutStreamImpl& operator=(const TOutStreamImpl&) = delete;
        TOutStreamImpl& operator=(TOutStreamImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsOpen() const = 0;

        virtual tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        ) = 0;

        virtual tCIDLib::TCard8 c8CurPos() const = 0;

        virtual tCIDLib::TVoid Reset() = 0;

        virtual tCIDLib::TVoid SeekToEnd() = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TOutStreamImpl() {}


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOutStreamImpl,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TBinOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinOutStream : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinOutStream
        (
                    TOutStreamImpl* const   pstrmiToAdopt
        );

        TBinOutStream(const TBinOutStream&) = delete;
        TBinOutStream(TBinOutStream&&) = delete;

        ~TBinOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinOutStream& operator<<
        (
            const   tCIDLib::TBoolean       bToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TCard1         c1ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TCard2         c2ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TCard4         c4ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TCard8         c8ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::EStreamMarkers eMarker
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TFloat4        f4ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TFloat8&       f8ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TInt1          i1ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TInt2          i2ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TInt4          i4ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TInt8          i8ToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TUInt          uToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TSInt          sToWrite
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::EStrmSpecial   eSpecial
        );

        TBinOutStream& operator<<
        (
            const   tCIDLib::TCh            chToWrite
        );

        TBinOutStream& operator=(const TBinOutStream&) = delete;
        TBinOutStream& operator=(TBinOutStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBigEndian() const
        {
            return (m_eEndianMode == tCIDLib::EEndianModes::Big);
        }

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bLittleEndian() const
        {
            return (m_eEndianMode == tCIDLib::EEndianModes::Little);
        }

        tCIDLib::TCard4 c4WriteBuffer
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4WriteRawBuffer
        (
            const   tCIDLib::TVoid* const   pBufToUse
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4WriteStream
        (
                    TBinInStream&           strmSrc
        );

        tCIDLib::TCard4 c4CurPos() const;

        tCIDLib::TCard8 c8CurPos() const;

        tCIDLib::EEndianModes eEndianMode() const
        {
            return m_eEndianMode;
        }

        tCIDLib::EEndianModes eEndianMode
        (
            const   tCIDLib::EEndianModes   eToSet
        );

        tCIDLib::TVoid Flush();

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SeekToEnd();

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TBoolean* const abList
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TCh* const     pszToWrite
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TCard1* const  ac1List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TCard2* const  ac2List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TCard4* const  ac4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TCard8* const  ac8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TFloat4* const af4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TFloat8* const af8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TInt1* const   ai1List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TInt2*  const  ai2List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TInt4*  const  ai4List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteArray
        (
            const   tCIDLib::TInt8*  const  ai8List
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid WriteClassInfo
        (
            const   TClass&                 clsToWrite
        );

        tCIDLib::TVoid WriteEnum
        (
            const   tCIDLib::TCard4         c4Enum
        );



    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TBinOutStream();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptImplObject
        (
                    TOutStreamImpl* const   pstrmiToAdopt
        );

        tCIDLib::TVoid DeleteImplObject();


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4CacheSz = 4196;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CacheSpaceAvail() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1Cache
        //      This is the output cache used to save up data and write it to
        //      the underlying impl object in chunks. This is key to being
        //      able to get efficiency.
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
        //  m_pstrmiOut
        //      This is the impl object that we do our actual work through.
        //
        //  m_tcvtText
        //      We store tCIDLib::TCh data in UTF-8 format to minimize storage
        //      overhead (at a small price in CPU overhead on some platforms.)
        //      The overhead of creating this converter is low enough that we
        //      don't bother with any kind of lazy faulting in.
        // -------------------------------------------------------------------
        tCIDLib::TCard1         m_ac1Cache[c4CacheSz];
        tCIDLib::TCard4         m_c4CurIndex;
        tCIDLib::EEndianModes   m_eEndianMode;
        TOutStreamImpl*         m_pstrmiOut;
        TUTF8Converter          m_tcvtText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinOutStream,TObject)
};


#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  To help the fundamental collections, we define a set of globals for array
//  reading, which just map to the members of the input stream above. This
//  allows us to generate other globals for things like enumerations so that
//  the fundamental collections can enum types as well.
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&              strmOut
                        , const tCIDLib::TBoolean* const    abList
                        , const tCIDLib::TCard4             c4Count)
{
    strmOut.WriteArray(abList, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TCh* const     pszToWrite
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(pszToWrite, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TCard1* const  ac1List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ac1List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TCard2* const  ac2List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ac2List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TCard4* const  ac4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ac4List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TCard8* const  ac8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ac8List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TFloat4* const af4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(af4List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TFloat8* const af8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(af8List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TInt1* const   ai1List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ai1List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TInt2*  const  ai2List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ai2List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TInt4*  const  ai4List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ai4List, c4Count);
}

inline tCIDLib::TVoid
TBinOutStream_WriteArray(       TBinOutStream&          strmOut
                        , const tCIDLib::TInt8*  const  ai8List
                        , const tCIDLib::TCard4         c4Count)
{
    strmOut.WriteArray(ai8List, c4Count);
}



// ---------------------------------------------------------------------------
//  This is a template function that greatly simplifies streaming out objects
//  polymorphically. It keeps it all typesafe and makes sure that the object
//  gotten really supports the streaming operators. The actual streaming
//  operators are in CIDLib_Streamable.hpp since they have to be friends of
//  the streamable mixin.
// ---------------------------------------------------------------------------
template <class T> tCIDLib::TVoid
PolymorphicWrite(const T* const pobjToWrite, TBinOutStream& strmToWriteTo)
{
    strmToWriteTo.WriteClassInfo(pobjToWrite->clsIsA());
    strmToWriteTo << *pobjToWrite;
}


// ---------------------------------------------------------------------------
//  Create some short cut values for the special stream enum values. These
//  are often used so having a short cut is very convenient.
// ---------------------------------------------------------------------------
namespace kCIDLib
{
    const tCIDLib::EStrmSpecial FlushIt     = tCIDLib::EStrmSpecial::FlushIt;
}


