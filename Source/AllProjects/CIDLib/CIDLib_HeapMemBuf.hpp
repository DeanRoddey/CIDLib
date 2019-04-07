//
// FILE NAME: CIDLib_Memory.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/1993
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
//  This is the header for the CIDLib_HeapMemBuf.Cpp file. This file provides
//  the THeapBuf class, which is a derivative of the base TMemBuf class. This
//  one provides a buffer of heap allocated memory.
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
//  CLASS: THeapBuf
// PREFIX: mbuf
// ---------------------------------------------------------------------------
class CIDLIBEXP THeapBuf : public TMemBuf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THeapBuf();

        explicit THeapBuf
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );

        THeapBuf
        (
            const   tCIDLib::TCard1* const  apc1InitData
            , const tCIDLib::TCard4         c4InitDataBytes
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );

        THeapBuf
        (
            const   TMemBuf&                mbufInitData
            , const tCIDLib::TCard4         c4InitDataBytes
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );

        explicit THeapBuf
        (
            const   TString&                strInitData
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );

        explicit THeapBuf
        (
            const   tCIDLib::TCh* const     pszInitData
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );

        THeapBuf
        (
            const   THeapBuf&               mbufSrc
            , const tCIDLib::TCard4         c4RelevantBytes
        );

        THeapBuf
        (
            const   THeapBuf&               mbufSrc
        );

        THeapBuf
        (
                    THeapBuf&&              mbufSrc
        );

        ~THeapBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THeapBuf& operator=
        (
            const   THeapBuf&               mbufSrc
        );

        THeapBuf& operator=
        (
                    THeapBuf&&              mbufSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   THeapBuf&               mbufToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   THeapBuf&               mbufToTest
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
        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4MaxCard
            , const tCIDLib::TCard4         c4ExpandIncrement = 0
        );


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
        //  Private data members
        //
        //  m_c4ExpandIncrement
        //      If we reallocate, this is the minimum we'll expand by, unless
        //      a full increment would go over the max size.
        //
        //  m_c4Size
        //      Our current allocated size. Due to reallocation, this must
        //      be mutable. In general, the current size is an internal
        //      implementation detail, since we expand as needed.
        //
        //  m_c4MaxSize
        //      The max size we can grow to.
        //
        //  m_pc1Data
        //      Our current buffer pointer. Due to reallocation, this must
        //      be mutable.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4ExpandIncrement;
        mutable tCIDLib::TCard4     m_c4Size;
        tCIDLib::TCard4             m_c4MaxSize;
        mutable tCIDLib::TCard1*    m_pc1Data;

        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THeapBuf,TMemBuf)
        DefPolyDup(THeapBuf)
};

#pragma CIDLIB_POPPACK

