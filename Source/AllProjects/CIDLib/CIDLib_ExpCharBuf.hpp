//
// FILE NAME: CIDLib_ExpCharbuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/14/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_ExpCharBuf.cpp file. This file implements
//  the TExpCharBuf class. There are many places where characters must be
//  accumulated from a source, in a way where the max size is unknown and in
//  which appending new chars must be very simple and fast. This class provides
//  that functionality.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  The buffer is not maintained null terminated. But, when you ask for
//      the raw buffer point, pszRawBuffer(), it will terminate at that point
//      so you will have a null terminated string.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TExpCharBuf
//  PREFIX: expb
// ---------------------------------------------------------------------------
class CIDLIBEXP TExpCharBuf : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TExpCharBuf
        (
            const   tCIDLib::TCard4         c4InitAlloc = 8
        );

        TExpCharBuf(const TExpCharBuf&) = delete;

        ~TExpCharBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TExpCharBuf& operator=(const TExpCharBuf&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh            chToAppend
        );

        tCIDLib::TCard4 c4Chars() const;

        const tCIDLib::TCh* pszBuffer() const;

        tCIDLib::TCh* pszReplicateBuffer() const;

        tCIDLib::TVoid RemoveTrailingSpace();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurOfs
        //      The current offset, which is where the next character will
        //      be placed.
        //
        //  m_c4CurSize
        //      The current size of m_pszBuffer. When m_c4CurOfs reaches this
        //      value, the buffer is expanded.
        //
        //  m_pszBuffer
        //      A pointer to the buffer that we are putting chars into. Its
        //      grown as needed.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CurOfs;
        tCIDLib::TCard4     m_c4CurSize;
        tCIDLib::TCh*       m_pszBuffer;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TExCharBuf,TObject)
};

#pragma CIDLIB_POPPACK


