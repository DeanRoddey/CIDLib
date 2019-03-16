//
// FILE NAME: CIDKernel_KVPair.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2011
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
//  This is the header for the CIDKernel_KVPair.cpp module, which implements
//  a very simple class to hold two strings. It's common to need to return to
//  the CIDLib level pairs of strings that represent names and ids of some
//  sort of resource. This makes it a lot easier to do so.
//
//  For flexibility it actually allows for two values.
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
//   CLASS: TKrnlKVPair
//  PREFIX: kkvp
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlKVPair
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TKrnlKVPair();

        TKrnlKVPair
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszValue
        );

        TKrnlKVPair
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCh* const     pszValue2
        );

        TKrnlKVPair
        (
            const   TKrnlKVPair&            kkvpSrc
        );

        ~TKrnlKVPair();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlKVPair& operator=
        (
            const   TKrnlKVPair&            kkvpSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const tCIDLib::TCh* pszKey() const;

        const tCIDLib::TCh* pszValue() const;

        const tCIDLib::TCh* pszValue2() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszValue
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCh* const     pszValue2
        );

        tCIDLib::TVoid SetKey
        (
            const   tCIDLib::TCh* const     pszKey
        );

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TCh* const     pszValue
        );

        tCIDLib::TVoid SetValue2
        (
            const   tCIDLib::TCh* const     pszValue
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pszKey
        //  m_pszValue
        //      We allocate these to hold the provided values and delete them
        //      in the ctor. They are expanded if needed if new values are
        //      set.
        // -------------------------------------------------------------------
        tCIDLib::TCh*   m_pszKey;
        tCIDLib::TCh*   m_pszValue;
        tCIDLib::TCh*   m_pszValue2;
};

#pragma CIDLIB_POPPACK

