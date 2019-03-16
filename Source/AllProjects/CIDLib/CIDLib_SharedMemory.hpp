//
// FILE NAME: CIDLib_SharedMemory.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/1997
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
//  This is the header for the CIDLib_SharedMemory.Cpp file. This file
//  provides the TSharedMemBuf class, which is a buffer of system allocated
//  memory that is named.
//
//  Shared memory buffers can be fully committed or they can be lazily
//  faulted in sequentially from start to end as needed. This is driven by
//  the alloc type parameter. If its lazy, then it will initially have zero
//  allocated pages and the user must commit pages as required. If they go
//  beyond that, they will get a system exception.
//
//  The default action during construction is to either open the buffer if
//  it already exists or create it if it does not.
//
// CAVEATS/GOTCHAS:
//
//  1)  As with other memory buffers, distinguish between equality of two
//      buffer objects and the equality of the contents of them. Use
//      bBuffersEqual() for testing the actual buffer contents. Or you can
//      just compare the names because two buffers with the same name
//      actually point at the same memory.
//
//  2)  Note that creating another shared memory buffer with the same name
//      and access flags as another buffer is basically the same as doing
//      a copy constructor, since it will refer to the same buffer with
//      the same access.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TSharedMemBuf
// PREFIX: mbuf
// ---------------------------------------------------------------------------
class CIDLIBEXP TSharedMemBuf : public TMemBuf
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSharedMemBuf
        (
            const   tCIDLib::TCard4         c4InitSize
            , const tCIDLib::TCard4         c4MaxSize
            , const TResourceName&          rsnToUse
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::EMemAccFlags   eAccFlags = tCIDLib::EMemAccFlags::ReadWrite
            , const tCIDLib::ECreateActs    eCreate = tCIDLib::ECreateActs::OpenOrCreate
        );

        TSharedMemBuf
        (
            const   TSharedMemBuf&          mbufSrc
        );

        ~TSharedMemBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSharedMemBuf& operator=
        (
            const   TSharedMemBuf&          mbufToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TSharedMemBuf&          mbufToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSharedMemBuf&          mbufToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamCount
        (
                    TBinOutStream&          strmToWriteTo
            , const tCIDLib::TCard4         c4Count
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EMemAccFlags eAccess() const;

        tCIDLib::TCard4 c4AllocatedPages() const;

        const TResourceName& rsnName() const;

        tCIDLib::TVoid SyncView();


    protected :
        // -------------------------------------------------------------------
        //  Protected methods
        // -------------------------------------------------------------------
        TSharedMemBuf();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1* pc1QueryBuf() override;

        const tCIDLib::TCard1* pc1QueryBuf() const override;

        tCIDLib::TCard1* pc1QueryBufInfo
        (
                    tCIDLib::TCard4&        c4CurSize
            ,       tCIDLib::TCard4&        c4MaxSize
        ) override;

        const tCIDLib::TCard1* pc1QueryBufInfo
        (
                    tCIDLib::TCard4&        c4CurSize
            ,       tCIDLib::TCard4&        c4MaxSize
        )   const override;

        tCIDLib::TVoid Realloc
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TBoolean       bPreserve
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateOrOpen
        (
            const   tCIDLib::ECreateActs eCreateFlags
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Size
        //      We have to keep this because the kernel object only deals in
        //      pages, whereas we have to keep up with the exact size that
        //      the user has set (for index checks.) Due to reallocation this
        //      has to be mutable.
        //
        //  m_rsnThis
        //      This is the resource name object that holds the abstracted
        //      name for the buffer.
        //
        //  m_ksmbThis
        //      This is the shared memory buffer object that holds the
        //      buffer information. Due to reallocation, this must be mutable.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4     m_c4Size;
        TResourceName               m_rsnThis;
        mutable TKrnlSharedMemBuf   m_ksmbThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSharedMemBuf,TMemBuf)
        DefPolyDup(TSharedMemBuf)
};

#pragma CIDLIB_POPPACK


