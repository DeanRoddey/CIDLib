//
// FILE NAME: CIDLib_MemoryStreamImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/29/1995
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
//  This is the header for the CIDLib_MemoryStreamImpl.Cpp file. This file
//  implements the binary stream implementation objects for memory buffers.
//  These allow a TBinInStream or TBinOutStream object to work on a memory
//  buffer.
//
//  Also defined here is a TMemStreamImplInfo class, which holds the info
//  that must be shared among streams using the same memory buffer. This
//  keeps them in sync regarding the contents of the buffer and the current
//  logical end of stream. This class must be reference counted since its
//  shared amoung multiple stream impl objects (and hence by multiple stream
//  objects.) So its managed by the impl object via a counted pointer object.
//
// CAVEATS/GOTCHAS :
//
//  See our base class TInStreamImpl/TOutStreamImpl for other gotchas that we have
//  to deal with.
//
//  1)  TruncateAt() cannot truncate past the current end of data, or an exception
//      will be thrown. The current output position is set back to zero when a
//      truncation is done.
//
//  2)  SetEndIndex() will update the current end of data so that the stream can
//      before forced to see data written to its buffer behind its back. It cannot
//      be set beyond the current size of the buffer (or max size of course.)
//
//  3)  SkipForwardBy can not skip past the current end of data, or it will throw.
//      So it cannot push the read point past the end.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMemOutStreamImpl;


// ---------------------------------------------------------------------------
//   CLASS: TMemStreamImplInfo
//  PREFIX: sii
// ---------------------------------------------------------------------------
class CIDLIBEXP TMemStreamImplInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemStreamImplInfo() = delete;

        TMemStreamImplInfo
        (
            const   TMemBuf* const          pmbufToUse
            , const tCIDLib::EAdoptOpts     eAdoptBuf
            , const tCIDLib::TCard4         c4InitLogicalEnd
        );

        TMemStreamImplInfo(const TMemStreamImplInfo&) = delete;

        ~TMemStreamImplInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemStreamImplInfo& operator=(const TMemStreamImplInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  The data is public because this object serves only to hold data
        //  for the impl objects and they are trusted to do the right thing.
        //  The outside world doesn't ever look inside here.
        //
        //  c4EndIndex
        //      This is the current end of data index, i.e. the logical end
        //      of stream.
        //
        //  eAdopted
        //      Indicates whether we were told to adopt this buffer object
        //      and clean it up.
        //
        //  pmbufData
        //      This is the memory buffer that provides the data sink/source
        //      for the stream impl. We store only a const pointer here, so
        //      that we can work for both in and out stream impls.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     c4EndIndex;
        tCIDLib::EAdoptOpts eAdopted;
        const TMemBuf*      pmbufData;
};



// ---------------------------------------------------------------------------
//   CLASS: TMemInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TMemInStreamImpl : public TInStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemInStreamImpl() = delete;

        TMemInStreamImpl
        (
            const   TMemBuf* const          pmbufToStream
            , const tCIDLib::TCard4         c4InitLogicalEnd = 0
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TMemInStreamImpl
        (
            const   TMemOutStreamImpl&      strmiToSyncTo
        );

        TMemInStreamImpl(const TMemInStreamImpl&) = delete;

        ~TMemInStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemInStreamImpl& operator=(const TMemInStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override
        {
            // This one just always returns true
            return kCIDLib::True;
        }

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        )   override;

        tCIDLib::TCard8 c8CurPos() const override
        {
            return m_c4Index;
        }

        tCIDLib::TVoid Reset() override
        {
            m_c4Index = 0;
        }

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const
        {
            return m_cptrInfo->c4EndIndex;
        }

        const TMemBuf& mbufData() const
        {
            return *m_pmbufSrc;
        }

        tCIDLib::TVoid SetEndIndex
        (
            const   tCIDLib::TCard4         c4NewIndex
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index
        //      The current index into the memory buffer, i.e. the read
        //      position.
        //
        //  m_pcptrInfo
        //      This impl object's stream impl info object. It holds the part
        //      of the data that has to be shared among impls that are using
        //      the same memory buffer. Its managed via a counted pointer
        //      object so that we don't have to track its usage.
        //
        //  m_pmbufSrc
        //      We keep our own view of the memory buffer, though it really
        //      belongs to the impl info object.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4Index;
        TCntPtr<TMemStreamImplInfo> m_cptrInfo;
        const TMemBuf*              m_pmbufSrc;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMemInStreamImpl,TInStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TMemOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TMemOutStreamImpl : public TOutStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemOutStreamImpl() = delete
        ;
        TMemOutStreamImpl(const TMemOutStreamImpl&) = delete;

        TMemOutStreamImpl
        (
                    TMemBuf* const          pmbufToStream
            , const tCIDLib::TCard4         c4InitLogicalEnd = 0
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        ~TMemOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemOutStreamImpl& operator=(const TMemOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override
        {
            // This one just always returns true
            return kCIDLib::True;
        }

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        )   override;

        tCIDLib::TCard8 c8CurPos() const override
        {
            return m_c4Index;
        }

        tCIDLib::TVoid Reset() override
        {
            m_c4Index = 0;
            m_cptrInfo->c4EndIndex = 0;
        }

        tCIDLib::TVoid SeekToEnd() override
        {
            m_c4Index = m_cptrInfo->c4EndIndex;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const
        {
            return m_cptrInfo->c4EndIndex;
        }

        const TMemBuf& mbufData() const
        {
            return *m_pmbufSink;
        }

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard4         c4At
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TMemInStreamImpl;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index
        //      The current index into the memory buffer, i.e. the write
        //      position.
        //
        //  m_cptrInfo
        //      This impl object's stream impl info object. It holds the part
        //      of the data that has to be shared among impls that are using
        //      the same memory buffer. Its managed via a counted pointer
        //      object so that we don't have to track its usage.
        //
        //  m_pmbufSink
        //      We also have to have our own, non-const, view of hte memory
        //      buffer, since the info object's is const (so that it can work
        //      for both in and out streams.)
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4Index;
        TCntPtr<TMemStreamImplInfo> m_cptrInfo;
        TMemBuf*                    m_pmbufSink;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMemOutStreamImpl,TOutStreamImpl)
};


#pragma CIDLIB_POPPACK

