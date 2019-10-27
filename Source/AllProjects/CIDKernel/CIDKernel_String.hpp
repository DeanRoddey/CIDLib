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
        TKrnlString();

        TKrnlString
        (
            const   tCIDLib::TCh* const     pszValue
        );

        TKrnlString
        (
            const   TKrnlString&            kstrToCopy
        );

        ~TKrnlString();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlString& operator=
        (
            const   TKrnlString&            kstrToAssign
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
        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TVoid Clear();

        const tCIDLib::TCh* pszValue() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszValue
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszValue
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid SetFromShort
        (
            const   tCIDLib::TSCh* const    pszValue
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pszValue
        //      We allocate this to hold the content as required.
        // -------------------------------------------------------------------
        tCIDLib::TCh*   m_pszValue;
};

#pragma CIDLIB_POPPACK


