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
//  This is the header for the CIDLib_Memory.Cpp file. This file provides
//  the TMemBuf class, and two derivatives thereof THeapBuf and TSysBuf. TMemBuf
//  is the base memory class, which knows how to manage a memory buffer but does
//  not know how to create or destroy them. Derived classes handle creating and
//  destroying the buffers.
//
// CAVEATS/GOTCHAS:
//
//  1)  We implement empty copy, assign, move stuff so that derived classes can call
//      us first in the normal way, or declare defaults (which would call us.)
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TMemBuf
// PREFIX: mbuf
// ---------------------------------------------------------------------------
class CIDLIBEXP TMemBuf :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TMemBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TCard1& operator[]
        (
            const   tCIDLib::TCard4         c4Ind
        );

        tCIDLib::TCard1 operator[]
        (
            const   tCIDLib::TCard4         c4Ind
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TMemBuf&                mbufToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMemBuf&                mbufToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid StreamCount
        (
                    TBinOutStream&          strmToWriteTo
            , const tCIDLib::TCard4         c4Count
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendFrom
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4AppendAt
            , const tCIDLib::TCard4         c4SrcBytes
        );

        tCIDLib::TBoolean bAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TBoolean bCompare
        (
            const   TMemBuf&                mbufToComp
            , const tCIDLib::TCard4         c4Count = kCIDLib::c4MaxCard
        )   const;

        tCIDLib::TBoolean bCompare
        (
            const   tCIDLib::TCard1* const  pc1ToComp
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TCard1 c1At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard2 c2At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard4 c4At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard4 c4CheckSum() const;

        tCIDLib::TCard4 c4CheckSum
        (
            const   tCIDLib::TCard4         c4StartInd
        )   const;

        tCIDLib::TCard4 c4CheckSum
        (
            const   tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TCard4 c4ExpandIncrement() const;

        tCIDLib::TCard4 c4FindByte
        (
            const   tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard1         c1ToFind
        )   const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::TCard4 c4Size() const;

        tCIDLib::TCard8 c8At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCh chAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid CopyIn
        (
            const   tCIDLib::TVoid* const   pData
            , const tCIDLib::TCard4         c4DataSz
            , const tCIDLib::TCard4         c4StartInd = 0
        );

        tCIDLib::TVoid CopyIn
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4DataSz
            , const tCIDLib::TCard4         c4StartInd = 0
        );

        tCIDLib::TVoid CopyOut
        (
                    tCIDLib::TVoid* const   pData
            , const tCIDLib::TCard4         c4DataSz
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::TVoid CopyOut
        (
                    TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4DataSz
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::ESortComps eCompare
        (
            const   TMemBuf&                mbufToComp
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::ESortComps eCompare
        (
            const   tCIDLib::TCard1* const  pc1ToComp
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TFloat4 f4At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TFloat8 f8At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TVoid HexEncode
        (
            const   tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCh            chSepChar
            , const tCIDLib::TBoolean       bAppend
            , const tCIDLib::TBoolean       bLowerCase
            ,       TString&                strTarget
        );

        tCIDLib::TInt1 i1At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TInt2 i2At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TInt4 i4At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TInt8 i8At
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid MakeSpace
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4SpaceSz
            , const tCIDLib::TCard4         c4OrgCnt
        );

        tCIDLib::TVoid MoveToStart
        (
            const   tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4Count
        );

        const tCIDLib::TVoid* pData() const;

        tCIDLib::TVoid* pData();

        template <class T> const T* pDataAs() const
        {
            return reinterpret_cast<const T*>(pc1QueryBuf());
        }

        template <class T> T* pDataAs()
        {
            return reinterpret_cast<T*>(pc1QueryBuf());
        }

        const tCIDLib::TCard1* pc1Data() const;

        tCIDLib::TCard1* pc1Data();

        const tCIDLib::TCard1* pc1DataAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard1* pc1DataAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        template <class T> const T* pDataAtAs(const tCIDLib::TCard4 c4At) const
        {
            return reinterpret_cast<const T*>(pc1DataAt(c4At));
        }

        template <class T> T* pDataAtAs(const tCIDLib::TCard4 c4At)
        {
            return reinterpret_cast<T*>(pc1DataAt(c4At));
        }

        TBinInStream* pstrmMakeReadable();

        TBinOutStream* pstrmMakeWriteable();

        tCIDLib::TVoid PutBool
        (
            const   tCIDLib::TBoolean       bToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutCard1
        (
            const   tCIDLib::TCard1         c1ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutCard2
        (
            const   tCIDLib::TCard2         c2ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutCard4
        (
            const   tCIDLib::TCard4         c4ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutCard8
        (
            const   tCIDLib::TCard8         c8ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutFloat4
        (
            const   tCIDLib::TFloat4        f4ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutFloat8
        (
            const   tCIDLib::TFloat8        f8ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutInt1
        (
            const   tCIDLib::TInt1          i1ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutInt2
        (
            const   tCIDLib::TInt2          i2ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutInt4
        (
            const   tCIDLib::TInt4          i4ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutInt8
        (
            const   tCIDLib::TInt8          i8ToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutSCh
        (
            const   tCIDLib::TSCh           schToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid PutCh
        (
            const   tCIDLib::TCh            chToPut
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid Reallocate
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TBoolean       bPreserve = kCIDLib::True
        )   const;

        tCIDLib::TVoid RemoveSpace
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4SpaceSz
            , const tCIDLib::TCard4         c4OrgCnt
        );

        tCIDLib::TSCh schAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1         c1Fill
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1         c1Fill
            , const tCIDLib::TCard4         c4StartInd
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1         c1Fill
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4Count
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TMemBuf();

        TMemBuf
        (
            const   TMemBuf&                mbufSrc
        );

        TMemBuf
        (
                    TMemBuf&&               mbufSrc
        );

        TMemBuf& operator=
        (
            const   TMemBuf&                mbufSrc
        );

        TMemBuf& operator=
        (
                    TMemBuf&&               mbufSrc
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard1* pc1QueryBuf() = 0;

        virtual const tCIDLib::TCard1* pc1QueryBuf() const = 0;

        virtual tCIDLib::TCard1* pc1QueryBufInfo
        (
                    tCIDLib::TCard4&        c4CurSize
            ,       tCIDLib::TCard4&        c4MaxSize
        )   = 0;

        virtual const tCIDLib::TCard1* pc1QueryBufInfo
        (
                    tCIDLib::TCard4&        c4CurSize
            ,       tCIDLib::TCard4&        c4MaxSize
        )   const = 0;

        virtual tCIDLib::TVoid Realloc
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TBoolean       bPreserve = kCIDLib::True
        )   const = 0;


        tCIDLib::TVoid ValidateParms();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ValidateExpInc
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::TCard4         c4ExpandIncrement
        );

        tCIDLib::TVoid ValidateParms
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize
            , const tCIDLib::TCard4         c4ExpandIncrement
        );

        tCIDLib::TVoid ValidateSizes
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4MaxSize
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        const tCIDLib::TCard1* pc1CheckIndex
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4Index
        )   const;

        tCIDLib::TCard1* pc1CheckIndex
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4Index
        );

        const tCIDLib::TCard1* pc1CheckIndex
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4Index
            ,       tCIDLib::TCard4&    c4Size
        )   const;

        tCIDLib::TCard1* pc1CheckIndex
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4Index
            ,       tCIDLib::TCard4&    c4Size
        );

        const tCIDLib::TCard1* pc1CheckRange
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4StartIndex
            , const tCIDLib::TCard4     c4Count
        )   const;

        tCIDLib::TCard1* pc1CheckRange
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4StartIndex
            , const tCIDLib::TCard4     c4Count
        );

        const tCIDLib::TCard1* pc1CheckRange
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4StartIndex
            , const tCIDLib::TCard4     c4Count
            ,       tCIDLib::TCard4&    c4Size
        )   const;

        tCIDLib::TCard1* pc1CheckRange
        (
            const   tCIDLib::TCard4     c4Line
            , const tCIDLib::TCard4     c4StartIndex
            , const tCIDLib::TCard4     c4Count
            ,       tCIDLib::TCard4&    c4Size
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMemBuf,TObject)
        NulObject(TMemBuf)
};

#pragma CIDLIB_POPPACK


inline tCIDLib::TVoid
TMemBuf::ValidateParms( const   tCIDLib::TCard4 c4Size
                        , const tCIDLib::TCard4 c4MaxSize
                        , const tCIDLib::TCard4 c4ExpandIncrement)
{
    // Call the other versions that check the various parts
    ValidateSizes(c4Size, c4MaxSize);
    ValidateExpInc(c4Size, c4MaxSize, c4ExpandIncrement);
}


