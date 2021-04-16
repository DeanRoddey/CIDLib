//
// FILE NAME: CIDKernel_String.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2011
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
//  This is the header for the CIDKernel_String.cpp module, which implements
//  a very simple class to hold a string. It provides little manipulation
//  features. It just exists to allow us to return lists of dynamically allocated
//  strings to the CIDLib layer in a safe way, i.e. value semantics (generally
//  though the simple kernel level linked list class.)
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
//   CLASS: TKrnlString
//  PREFIX: kstr
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlString
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TKrnlString
        (
            const   tCIDLib::TCard4         c4BufSz = 32
        );

        TKrnlString
        (
            const   tCIDLib::TCh* const     pszValue
        );

        TKrnlString
        (
            const   tCIDLib::TSCh* const    pszValue
        );

        TKrnlString
        (
            const   tCIDLib::TCh* const     pszValue1
            , const tCIDLib::TCh* const     pszValue2
            , const tCIDLib::TCh* const     pszValue3 = nullptr
            , const tCIDLib::TCh* const     pszValue4 = nullptr
            , const tCIDLib::TCh* const     pszValue5 = nullptr
        );

        TKrnlString
        (
            const   TKrnlString&            kstrSrc
        );

        TKrnlString
        (
                    TKrnlString&&           kstrSrc
        );

        ~TKrnlString();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TKrnlString&            kstrSrc
        )   const;

        tCIDLib::TBoolean operator!=(const TKrnlString& kstrSrc) const
        {
            return !operator==(kstrSrc);
        }

        TKrnlString& operator=
        (
            const   TKrnlString&            kstrSrc
        );

        TKrnlString& operator=
        (
                    TKrnlString&&           kstrSrc
        );

        TKrnlString& operator=
        (
            const   tCIDLib::TCh* const     pszRawStr
        );

        TKrnlString& operator=
        (
            const   tCIDLib::TSCh* const    pszRawSStr
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptBuffer
        (
                    tCIDLib::TCh*&          pszSrc
            , const tCIDLib::TCard4         c4BufSz
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszSrc
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszSrc1
            , const tCIDLib::TCh* const     pszSrc2
            , const tCIDLib::TCh* const     pszSrc3 = nullptr
            , const tCIDLib::TCh* const     pszSrc4 = nullptr
        );

        tCIDLib::TVoid Append
        (
            const   TKrnlString&            kstrSrc
        );

        tCIDLib::TVoid AppendIfNotAlready
        (
            const   tCIDLib::TCh            chSrc
        );

        tCIDLib::TBoolean bCompareI
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean bCompareI
        (
            const   TKrnlString&            kstrSrc
        )   const;

        tCIDLib::TBoolean bCompareNI
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bCompareNI
        (
            const   TKrnlString&            kstrSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bIsEmpty() const
        {
            return m_c4Length == 0;
        }

        tCIDLib::TCard4 c4Length() const
        {
            return m_c4Length;
        }

        tCIDLib::TCard4 c4BufSize() const
        {
            return m_c4BufSz;
        }

        tCIDLib::TVoid Combine
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCh            chSepChar
        );

        tCIDLib::TVoid Combine( const   TKrnlString&    kstrSrc
                                , const tCIDLib::TCh    chSepChar)
        {
            Combine(kstrSrc.m_pszValue, chSepChar);
        }

        tCIDLib::TVoid Clear();

        [[nodiscard]] tCIDLib::TCh* pszReplicate() const;

        [[nodiscard]] tCIDLib::TCh* pszReplicateAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        [[nodiscard]] tCIDLib::TCh* pszOrphanBuffer
        (
                    tCIDLib::TCard4&        c4Size
            ,       tCIDLib::TCard4&        c4Length
        );

        const tCIDLib::TCh* pszValue() const;

        const tCIDLib::TCh* pszValueAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid Reallocate
        (
            const   tCIDLib::TCard4         c4MinSize
        );

        tCIDLib::TVoid Set(const tCIDLib::TCh* const pszValue)
        {
            SetWithExtra(pszValue, 0);
        }

        tCIDLib::TVoid SetWithExtra
        (
            const   tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCard4         c4Extra
        );

        tCIDLib::TVoid SetFromShort
        (
            const   tCIDLib::TSCh* const    pszValue
        );

        tCIDLib::TVoid SetUnterminated
        (
            const   tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCard4         c4Count
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ExpandBuf
        (
            const   tCIDLib::TCard4         c4Needed
            , const tCIDLib::TBoolean       bKeepOld
        );

        tCIDLib::TVoid SetNewBuffer
        (
            const   tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCard4         c4SrcLen
            , const tCIDLib::TCard4         c4Extra
        );

        tCIDLib::TBoolean bSetNewSBuffer
        (
            const   tCIDLib::TSCh* const    pszValue
            , const tCIDLib::TCard4         c4Extra
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufSize
        //      The allocated size of the buffer. In reality it's one larger than
        //      this so taht there's alwyas room for a null terminator.
        //
        //  m_c4Length
        //      The current length of the string, though we also keep it null
        //      terminated.
        //
        //  m_pszValue
        //      We allocate this to hold the content as required.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4BufSz;
        tCIDLib::TCard4 m_c4Length;
        tCIDLib::TCh*   m_pszValue;
};

#pragma CIDLIB_POPPACK


