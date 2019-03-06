//
// FILE NAME: CIDLib_Memory.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_SysMemBuf.Cpp file. This file provides
//  the TSysBuf class, which is a derivative of the base TMemBuf class. This
//  one provides a buffer of system allocated memory, which generally has
//  more capabitilies than a heap allocated buffer, such as lazy faulting
//  in, setting read/write access, and so forth.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that the actual allocation of system buffers must be in terms
//      of pages, but we maintain the standard CIDLib buffer size semantics.
//      So this class will often get realloc calls in which it doesn't actually
//      have to do anything but bump the size member because it's already
//      actually allocated the next full page.
//
//      So the m_c4Size field is not the actual size allocated, but the
//      semantic size of the buffer. To calc the actual size take the size
//      and divide by the page size, then add one if not an even multiple,
//      and multiply that by the page size.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TSysBuf
// PREFIX: mbuf
// ---------------------------------------------------------------------------
class CIDLIBEXP TSysBuf : public TMemBuf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSysBuf();

        TSysBuf
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::EMemAccFlags   eAccFlags = tCIDLib::EMemAccFlags::ReadWrite
        );

        TSysBuf
        (
            const   tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::EMemAccFlags   eAccFlags = tCIDLib::EMemAccFlags::ReadWrite
        );

        TSysBuf
        (
            const   TString&                strInitData
            , const tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::EMemAccFlags   eAccFlags = tCIDLib::EMemAccFlags::ReadWrite
        );

        TSysBuf
        (
            const   tCIDLib::TCh* const     pszInitData
            , const tCIDLib::TCard4         c4MaxSize = 0
            , const tCIDLib::EMemAccFlags   eAccFlags = tCIDLib::EMemAccFlags::ReadWrite
        );

        TSysBuf
        (
            const   TSysBuf&                mbufSrc
        );

        ~TSysBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSysBuf& operator=
        (
            const   TSysBuf&                mbufToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TSysBuf&                mbufToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSysBuf&                mbufToTest
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
        tCIDLib::TCard4 c4AllocatedPages() const;

        tCIDLib::EMemAccFlags eMemAccFlags() const;


    protected :
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
        tCIDLib::TVoid Create();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Size
        //      Our current allocated size. Due to reallocation, it has to
        //      be mutable.
        //
        //  m_c4MaxSize
        //      The max size we can grow to.
        //
        //  m_eAccess
        //      They are the access rights that were given to this object
        //      during contruction.
        //
        //  m_pc1Data
        //      Our current data buffer pointer.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4 m_c4Size;
        tCIDLib::TCard4         m_c4MaxSize;
        tCIDLib::EMemAccFlags   m_eAccess;
        tCIDLib::TCard1*        m_pc1Data;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSysBuf,TMemBuf)
        DefPolyDup(TSysBuf)
};

#pragma CIDLIB_POPPACK

