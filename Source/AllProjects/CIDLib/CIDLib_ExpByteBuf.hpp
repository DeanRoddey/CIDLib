//
// FILE NAME: CIDLib_ExpByteBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_ExpByteBuf.cpp file. This file implements
//  the TExpByteBuf class. There are many places where bytes must be accumulated
//  from a source, in a way where the max size is unknown and in which appending
//  new bytes must be very simple and fast. This class provides that
//  functionality. It does not allow for modification of the data, its just for
//  accumulation until it can be put into some other structure.
//
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
//   CLASS: TExpByteBuf
//  PREFIX: expb
// ---------------------------------------------------------------------------
class CIDLIBEXP TExpByteBuf : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TExpByteBuf
        (
            const   tCIDLib::TCard4         c4InitAlloc = 256
        );

        TExpByteBuf
        (
            const   TExpByteBuf&            expbSrc
        );

        TExpByteBuf
        (
                    TExpByteBuf&&           expbSrc
        );

        ~TExpByteBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TExpByteBuf& operator=
        (
            const   TExpByteBuf&            expbSrc
        );

        TExpByteBuf& operator=
        (
                    TExpByteBuf&&           expbSrc
        );


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TCard1 operator[]
        (
            const   tCIDLib::TCard4         c4At
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCard1         c1ToAppend
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCard1* const  pc1ToAppend
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TCard1 c1First() const;

        tCIDLib::TCard1 c1Last() const;

        tCIDLib::TCard4 c4Bytes() const;

        tCIDLib::TCard4 c4CheckSum() const;

        const tCIDLib::TCard1* pc1Buffer() const;

        tCIDLib::TCard1* pszReplicateBuffer();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurOfs
        //      The current offset, which is where the next byte will be
        //      placed in the buffer.
        //
        //  m_c4CurSize
        //      The current size of m_pc1Buffer. When m_c4CurOfs reaches this
        //      value, the buffer is expanded.
        //
        //  m_pc1Buffer
        //      A pointer to the buffer that we are putting bytes into. Its
        //      grown as needed.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CurOfs;
        tCIDLib::TCard4     m_c4CurSize;
        tCIDLib::TCard1*    m_pc1Buffer;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TExCharBuf,TObject)
};

#pragma CIDLIB_POPPACK


