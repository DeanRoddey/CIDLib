//
// FILE NAME: CIDLib_Pointer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/02/1997
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
//  This is the header for the CIDLib_Pointer.Cpp file, which implements
//  the TPointer class. This is a very simple class and is just for the
//  formatting of pointers to streams. Normally, if you try to format a
//  pointer to a stream, it misinterprets this in one way or another
//  according to the available formatting overrides available. This class
//  just makes sure that it comes out with the correct format and in hex
//  radix. We could use TCardinal for this, but we don't want to assume
//  always that a pointer and a TCardinal are the same size or have the
//  same formatting issues.
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
//  CLASS: TPointer
// PREFIX: ptr
// ---------------------------------------------------------------------------
class CIDLIBEXP TPointer :

    public TObject, public MFormattable, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TPointer() = default;

        TPointer(const TPointer&) = default;
        TPointer(TPointer&&) = default;

        TPointer
        (
            const   tCIDLib::TVoid* const   pPointer
        );

        ~TPointer() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPointer& operator=(const TPointer&) = default;
        TPointer& operator=(TPointer&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TPointer&               ptrToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TPointer&               ptrToCompare
        )   const;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP   TTextOutStream&         strmDest
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      This is the pointer that we are to format
        // -------------------------------------------------------------------
        const tCIDLib::TVoid*   m_pData = nullptr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPointer,TObject)
        DefPolyDup(TPointer)
};

#pragma CIDLIB_POPPACK



