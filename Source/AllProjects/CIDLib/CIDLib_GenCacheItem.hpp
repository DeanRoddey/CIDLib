//
// FILE NAME: CIDLib_GenCacheItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2014
//
// COPYRIGHT: 2014..2014, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  There is often a need to cache data in various programs. Often this involves
//  a server that maintains a serial number for the data being cached. So both the
//  server and the client may want to cache the data and need to track that serial
//  number (sine the server can tell the client it already has the most recent data
//  upon query and avoid a lot of overhead.)
//
//  It also is often desired to have a time stamp of the next time that a client
//  will go back to the server to see if new data is available, or that a server
//  will use for last access in an LRU cache. So we provide a time stamp as well.
//
//  We assume that the cache includes the raw data, to be stored in a memory buffer
//  object. So we provide a buffer and byte count. So the basic generic cache item
//  class is generally just used by itself.
//
//
//  However, we also provide a templatized derived class that allows the program to
//  store a specific type of data in the cache as well, which may be the data in
//  the memory buffer, just expanded out.
//
//
//  To help with a common scenario, we also provide a static method that will take
//  a standard generic cache collection and will throw out up to a given number of
//  items based on least recent use (assumes that the time stamp represents order of
//  access.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDGenCacheItem
//  PREFIX: cci
// ---------------------------------------------------------------------------
class CIDLIBEXP TCIDGenCacheItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------

        //
        //  There is a type in the CIDLib header, but we can't reference it here
        //  for circularity reasons. So we just use the actual type of TGenCache.
        //
        static tCIDLib::TCard4 c4LRUDiscard
        (
                    TKeyedHashSet<TCIDGenCacheItem, TString, TStringKeyOps>& colTar
            , const tCIDLib::TCard4         c4UpTo
        );

        static const TString& strGenKey(const TCIDGenCacheItem& cciSrc)
        {
            return cciSrc.m_strName;
        }


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCIDGenCacheItem();

        TCIDGenCacheItem
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4SerialNum
        );

        TCIDGenCacheItem
        (
            const   TCIDGenCacheItem&       cciToCopy
        );

        virtual ~TCIDGenCacheItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=
        (
            const   TCIDGenCacheItem&       cciToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryData
        (
                    tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TCard4&        c4Bytes
            ,       TMemBuf&                mbufToFill
        )   const;

        tCIDLib::TCard4 c4Bytes() const;

        tCIDLib::TCard4 c4IncSerialNum();

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TCard4 c4SerialNum
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TEncodedTime enctTimeStamp() const;

        const TMemBuf& mbufData() const;

        const TString& strName() const;

        tCIDLib::TVoid SetData
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        );

        tCIDLib::TVoid UpdateTimeStamp
        (
            const   tCIDLib::TCard4         c4SecsFromNow = 0
        );


    private :
        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GenInitSerialNum();


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Bytes;
        tCIDLib::TCard4         m_c4SerialNum;
        tCIDLib::TEncodedTime   m_enctStamp;
        THeapBuf                m_mbufData;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDGenCacheItem, TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDCacheItem
//  PREFIX: cci
// ---------------------------------------------------------------------------
template <class T> class TCIDCacheItem : public TCIDGenCacheItem
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        //
        //  This is sort of redundant, since the base class has one, but the compiler
        //  will complain if the key access method doesn't take the same type as the
        //  template is instantiated for.
        // -------------------------------------------------------------------
        static const TString& strKey(const TCIDCacheItem<T>& cciSrc)
        {
            return cciSrc.strName();
        }


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCIDCacheItem() : TCIDGenCacheItem()
        {
        }

        TCIDCacheItem(  const   TString&        strName
                        , const tCIDLib::TCard4 c4SerialNum) :

            TCIDGenCacheItem(strName, c4SerialNum)
        {
        }

        TCIDCacheItem(const TCIDCacheItem& cciToCopy) :

            TCIDGenCacheItem(cciToCopy)
            , m_objData(cciToCopy.m_objData)
        {
        }

        ~TCIDCacheItem()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=(const TCIDCacheItem<T>& cciToAssign)
        {
            TCIDGenCacheItem::operator=(cciToAssign);
            m_objData = cciToAssign.m_objData;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetTData(const T& objToSet)
        {
            m_objData = objToSet;
        }

        const T& objData() const
        {
            return m_objData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_objData
        //      The data object that is being cached
        // -------------------------------------------------------------------
        T   m_objData;
};

#pragma CIDLIB_POPPACK


