//
// FILE NAME: CIDLib_Boolean.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/26/2017
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
//  This is the header for the CIDLib_Boolean.Cpp file. This file implements a class wrapper
//  (TBoolean) for a boolean value. It is a very simple simple class and makes no attempt to
//  create a fully functional boolean class. Its purely for formatting purposes and for when a
//  boolean value needs to act like an object. It does though provide equality operators.
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
//  CLASS: TBoolean
// PREFIX: b
// ---------------------------------------------------------------------------
class CIDLIBEXP TBoolean :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TBoolean();

        TBoolean
        (
            const   tCIDLib::TBoolean       bVal
        );

        TBoolean(const TBoolean&) = default;
        TBoolean(TBoolean&&) = default;

        ~TBoolean();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBoolean& operator=(const TBoolean&) = default;
        TBoolean& operator=(TBoolean&&) = default;

        TBoolean& operator=
        (
                    tCIDLib::TBoolean       bSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TBoolean&               bToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TBoolean&               bToTest
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TBoolean       bToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TBoolean       bToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bVal() const
        {
            return m_bVal;
        }

        tCIDLib::TBoolean bVal
        (
            const   tCIDLib::TBoolean       bNewVal
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private             :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bVal
        //      Storage for the data value.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bVal;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBoolean,TObject)
        DefPolyDup(TBoolean)
        NulObject(TBoolean)
};

#pragma CIDLIB_POPPACK


