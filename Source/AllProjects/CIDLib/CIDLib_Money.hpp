//
// FILE NAME: CIDLib_Money.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/15/1997
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
//  This is the definition file for the CIDLib_Money.Cpp file. This file
//  implements the TMoney class, which supports formatting of values that
//  represent monetary values. Monetary values are assumed to be floating
//  point values and are formatted according to the current locale.
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
//  CLASS: TMoney
// PREFIX: mon
// ---------------------------------------------------------------------------
class CIDLIBEXP TMoney :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TMoney() = default;

        explicit TMoney
        (
            const   tCIDLib::TFloat8        f8Val
        );

        TMoney(const TMoney&) = default;

        ~TMoney() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMoney& operator=(const TMoney&) = default;

        TMoney& operator=
        (
            const   tCIDLib::TFloat8        f8Src
        );

        tCIDLib::TBoolean operator==
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TMoney&                 monSrc
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TFloat8        f8Src
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TFloat8 f8Val() const;

        tCIDLib::TFloat8 f8Val
        (
            const   tCIDLib::TFloat8        f8NewVal
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_f8Val
        //      Storage for the data value
        // -------------------------------------------------------------------
        tCIDLib::TFloat8    m_f8Val = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMoney,TObject)
        DefPolyDup(TMoney)
        NulObject(TMoney)
};

#pragma CIDLIB_POPPACK


