//
// FILE NAME: CIDLib_TypedMBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  This is the header for the CIDLib_TypedMBuf.Cpp file. This file provides
//  the TTypedMBuf class. This class provides a very convenient way to manage
//  a TMemBuf derived class whose memory buffer is a structure. This is often
//  required to interact with third party code or to shared memory between
//  processes.
//
// CAVEATS/GOTCHAS:
//
//  1)  The default constructor creates a THeapBuf object. The other version
//      will create a system memory buffer, and the third version will create
//      a named shared buffer.
//
//  2)  They are all fully allocated immediately. I.e. their initial alloc
//      size is set to the maximum alloc size, which is the size of the
//      structure.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TTypedMBufBase
//  PREFIX: tmbuf
//
//  This class just provides a non-templatized base class where we can get
//  some code out of line
// ---------------------------------------------------------------------------
class CIDLIBEXP TTypedMBufBase : public TObject, public MDuplicable, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TTypedMBufBase(const TTypedMBufBase&) = delete;

        ~TTypedMBufBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTypedMBufBase& operator=(const TTypedMBufBase&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTypedMBufBase();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowNotReady
        (
            const   tCIDLib::TCard4         c4Line
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTypedMBufBase,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TTypedMBuf
//  PREFIX: tmbuf
// ---------------------------------------------------------------------------
template <typename T> class TTypedMBuf : public TTypedMBufBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------

        // Just create a buffer the size of the type
        TTypedMBuf() : m_pmbufData(nullptr)
        {
            m_pmbufData = new THeapBuf(sizeof(T), sizeof(T));
        }

        TTypedMBuf(const tCIDLib::EMemAccFlags eAccess) : m_pmbufData(nullptr)
        {
            m_pmbufData = new TSysBuf(sizeof(T), eAccess);
        }

        TTypedMBuf( const   TResourceName&          rsnToUse
                    , const tCIDLib::EMemAccFlags   eAccess = tCIDLib::EMemAccFlags::ReadWrite
                    , const tCIDLib::ECreateActs    eCreateFlags = tCIDLib::ECreateActs::OpenOrCreate) :

            m_pmbufData(nullptr)
        {
            tCIDLib::TBoolean bCreated;
            m_pmbufData = new TSharedMemBuf
            (
                sizeof(T)
                , sizeof(T)
                , rsnToUse
                , bCreated
                , eAccess
                , eCreateFlags
            );
        }

        TTypedMBuf(const TTypedMBuf<T>& tmbufSrc) : m_pmbufData(nullptr)
        {
            // Duplicate the memory buffer object
            m_pmbufData = ::pDupObject<TMemBuf>(tmbufSrc.m_pmbufData);
        }

        ~TTypedMBuf()
        {
            delete m_pmbufData;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTypedMBuf<T>& operator=(const TTypedMBuf<T>&) = delete;



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T* operator->()
        {
            return static_cast<T*>(m_pmbufData->pData());
        }

        const T* operator->() const
        {
            return static_cast<const T*>(m_pmbufData->pData());
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean
        bMakeBuffer(const   TResourceName&          rsnToUse
                    , const tCIDLib::EMemAccFlags   eAccess
                    , const tCIDLib::ECreateActs    eCreateFlags)
        {
            // Delete any old buffer
            if (m_pmbufData)
            {
                delete m_pmbufData;
                m_pmbufData = nullptr;
            }

            // And create the new one
            tCIDLib::TBoolean bCreated;
            m_pmbufData = new TSharedMemBuf
            (
                sizeof(T)
                , sizeof(T)
                , rsnToUse
                , bCreated
                , eAccess
                , eCreateFlags
            );
            return bCreated;
        }


        tCIDLib::TVoid Close()
        {
            // Delete any old buffer
            if (m_pmbufData)
            {
                delete m_pmbufData;
                m_pmbufData = nullptr;
            }
        }


        // Provides typed access to the buffer
        const T& objData() const
        {
            if (!m_pmbufData)
                ThrowNotReady(CID_LINE);
            return *static_cast<const T*>(m_pmbufData->pData());
        }

        T& objData()
        {
            if (!m_pmbufData)
                ThrowNotReady(CID_LINE);
            return *static_cast<T*>(m_pmbufData->pData());
        }


        const T* pobjData(const tCIDLib::TBoolean bThrowIfNotReady = kCIDLib::False) const
        {
            if (!m_pmbufData)
            {
                if (bThrowIfNotReady)
                    ThrowNotReady(CID_LINE);
                return nullptr;
            }
            return static_cast<const T*>(m_pmbufData->pData());
        }

        T* pobjData(const tCIDLib::TBoolean bThrowIfNotReady = kCIDLib::False)
        {
            if (!m_pmbufData)
            {
                if (bThrowIfNotReady)
                    ThrowNotReady(CID_LINE);
                return nullptr;
            }
            return static_cast<T*>(m_pmbufData->pData());
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom) final
        {
            // Destroy the existing buffer and read in the new one
            delete m_pmbufData;
            m_pmbufData = nullptr;
            ::PolymorphicRead(m_pmbufData, strmToReadFrom);
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const final
        {
            if (!m_pmbufData)
                ThrowNotReady(CID_LINE);
            ::PolymorphicWrite(m_pmbufData, strmToWriteTo);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pmbufData
        //      This is the pointer to the memory buffer object that we are
        //      wrapping. It some derivative of TMemBuf.
        // -------------------------------------------------------------------
        TMemBuf*    m_pmbufData;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TTypedMBuf<T>)
        TemplateRTTIDefs(TTypedMBuf<T>,TTypedMBufBase)
};

#pragma CIDLIB_POPPACK

