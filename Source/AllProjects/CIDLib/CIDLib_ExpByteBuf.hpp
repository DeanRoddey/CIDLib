//
// FILE NAME: CIDLib_ExpByteBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2002
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

        TExpByteBuf(TExpByteBuf&& expbSrc) :

            TExpByteBuf(1)
        {
            operator=(tCIDLib::ForceMove(expbSrc));
        }

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

        [[nodiscard]] tCIDLib::TBoolean bIsEmpty() const noexcept
        {
            return m_c4CurOfs == 0;
        }

        tCIDLib::TCard1 c1First() const;

        tCIDLib::TCard1 c1Last() const;

        tCIDLib::TCard1 c1PopLast();

        [[nodiscard]] tCIDLib::TCard4 c4Bytes() const noexcept;

        tCIDLib::TCard4 c4CheckSum() const;

        [[nodiscard]] const tCIDLib::TCard1* pc1Buffer() const noexcept;

        [[nodiscard]] tCIDLib::TCard1* pszReplicateBuffer();

        tCIDLib::TVoid Reset() noexcept;


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


