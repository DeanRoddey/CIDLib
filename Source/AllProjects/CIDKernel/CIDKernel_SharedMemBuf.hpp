//
// FILE NAME: CIDKernel_SharedMemBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/04/1997
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
//  This is the header for the CIDKernel_SharedMemBuf.Cpp file, which
//  implements an simple abstraction for a shared, named memory buffer.
//
//  Also implemented here is TKrnlTypedSharedBuf, which is a simple template
//  class for use inside the kernel where a shared buffer contains a
//  structure. This template makes life simple and typesafe by presenting the
//  memory buffer's raw buffer as the structure type. Note that the CIDLib
//  facility has its own version of this class for public use, which is more
//  complex and supports streaming and all of that.
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
//   CLASS: TKrnlSharedMemBuf
//  PREFIX: ksmb
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSharedMemBuf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSharedMemBuf();

        TKrnlSharedMemBuf(const TKrnlSharedMemBuf&) = delete;

        ~TKrnlSharedMemBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSharedMemBuf& operator=(const TKrnlSharedMemBuf&) = delete;

        tCIDLib::TBoolean operator==
        (
            const   TKrnlSharedMemBuf&      ksmbToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKrnlSharedMemBuf&      ksmbToCompare
        )   const;



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAlloc
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::EAllocTypes    eAllocType
            , const tCIDLib::EMemAccFlags   eAccess
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::ECreateActs    eCreateFlags = tCIDLib::ECreateActs::OpenOrCreate
        );

        tCIDLib::TBoolean bCommitToSize
        (
            const   tCIDLib::TCard4         c4NewSize
        );

        tCIDLib::TBoolean bDuplicate
        (
            const   TKrnlSharedMemBuf&      ksmbToDup
        );

        tCIDLib::TBoolean bFree();

        tCIDLib::TBoolean bSyncView();

        tCIDLib::TBoolean bZero();

        tCIDLib::TCard4 c4AllocatedPages() const;

        tCIDLib::TCard4 c4MaxPages() const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::EMemAccFlags eAccess() const;

        tCIDLib::EAllocTypes eAllocType() const;

        tCIDLib::TVoid* pData();

        const tCIDLib::TVoid* pData() const;

        const tCIDLib::TVoid* pNextToCommit() const;

        tCIDLib::TVoid* pNextToCommit();

        const tCIDLib::TCh* pszName() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4AllocatedPages
        //      The amount of the buffer size that has been allocated. We
        //      only support contiguous allocation from start to end of the
        //      buffer.
        //
        //  m_c4MaxPages
        //      This is the maximum size of the buffer allocated, though it
        //      might not be all committed at any one time.
        //
        //  m_c4MaxSize
        //      The maximum size in bytes of the buffer. Though we have to
        //      allocate in full pages, we need to know if they ask to expand
        //      it beyond the actual byte max indicated.
        //
        //  m_eAccess
        //      The access rights used to create the buffer. This is required
        //      for duplication in the copy constructor, but is also a
        //      convenient thing to know.
        //
        //  m_eAllocType
        //      The allocation type we were given in the ctor. We don't use
        //      it after creation, but the higher level code might need to
        //      know.
        //
        //  m_pBuffer
        //      This is a pointer to the buffer.
        //
        //  m_hmemThis
        //      The platform specific information for each platform.
        //
        //  m_pszName
        //      This the name of the buffer, which is allocated as needed
        //      to hold the given name.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4AllocatedPages;
        tCIDLib::TCard4         m_c4MaxPages;
        tCIDLib::TCard4         m_c4MaxSize;
        tCIDLib::EMemAccFlags   m_eAccess;
        tCIDLib::EAllocTypes    m_eAllocType;
        tCIDLib::TVoid*         m_pBuffer;
        TMemoryHandle           m_hmemThis;
        tCIDLib::TCh*           m_pszName;
};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlTypedSharedBuf
//  PREFIX: ktsb
// ---------------------------------------------------------------------------
template <typename T> class TKrnlTypedSharedBuf : public TKrnlSharedMemBuf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlTypedSharedBuf() : TKrnlSharedMemBuf()
        {
        }

        TKrnlTypedSharedBuf(const TKrnlTypedSharedBuf<T>&) = delete;
        TKrnlTypedSharedBuf(TKrnlTypedSharedBuf<T>&&) = delete;

        ~TKrnlTypedSharedBuf()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlTypedSharedBuf<T>& operator=(const TKrnlTypedSharedBuf<T>&) = delete;
        TKrnlTypedSharedBuf<T>& operator=(TKrnlTypedSharedBuf<T>&&) = delete;

        T* operator->()
        {
            return static_cast<T*>(pData());
        }

        const T* operator->() const
        {
            return static_cast<const T*>(pData());
        }

        const T& operator()() const
        {
            return *static_cast<const T*>(pData());
        }

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAlloc
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::EMemAccFlags   eAccess
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::ECreateActs    eCreateFlags = tCIDLib::ECreateActs::OpenOrCreate)
        {
            return TKrnlSharedMemBuf::bAlloc
            (
                pszName
                , sizeof(T)
                , tCIDLib::EAllocTypes::Commit
                , eAccess
                , bCreated
                , eCreateFlags
            );
        }

        tCIDLib::TBoolean bAlloc
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::EMemAccFlags   eAccess
            , const tCIDLib::ECreateActs    eCreateFlags = tCIDLib::ECreateActs::OpenOrCreate)
        {
            tCIDLib::TBoolean bDummy;
            return TKrnlSharedMemBuf::bAlloc
            (
                pszName
                , sizeof(T)
                , tCIDLib::EAllocTypes::Commit
                , eAccess
                , bDummy
                , eCreateFlags
            );
        }
};

#pragma CIDLIB_POPPACK


