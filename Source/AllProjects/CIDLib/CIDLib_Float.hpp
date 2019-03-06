//
// FILE NAME: CIDLib_Float.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the definition file for the CIDLib_Float.Cpp file. This file
//  implements a class wrapper for a 64 bit floating point value. Its a very
//  simple class and makes no attempt to create a fully mathematical
//  functional class. Its purely for formatting purposes and for when an
//  floating point value needs to act like an object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TCardinal;
class   TInteger;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TFloat
// PREFIX: f
// ---------------------------------------------------------------------------
class CIDLIBEXP TFloat :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TFloat() = default;

        explicit TFloat
        (
            const   tCIDLib::TFloat4&       f4Val
        );

        explicit TFloat
        (
            const   tCIDLib::TFloat8&       f8Val
        );

        TFloat
        (
            const   tCIDLib::TFloat8&       f8Val
            , const tCIDLib::TCard1         c1Precision
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TFloat
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::TCard1         c1Precision = 0
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TFloat
        (
            const   tCIDLib::TInt4          i4Val
            , const tCIDLib::TCard1         c1Precision = 0
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TFloat
        (
            const   tCIDLib::TSInt          sVal
            , const tCIDLib::TCard1         c1Precision = 0
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TFloat(const TFloat&) = default;

        ~TFloat() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFloat& operator=(const TFloat&) = default;

        constexpr operator tCIDLib::TFloat8() const
        {
            return m_f8Val;
        }

        TFloat& operator=
        (
            const   tCIDLib::TFloat8&       f8Src
        );

        tCIDLib::TBoolean operator==
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TFloat&                 fToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TFloat8&       f8ToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1 c1Precision() const;

        tCIDLib::TCard1 c1Precision
        (
            const   tCIDLib::TCard1         c1Precision
        );

        tCIDLib::ETrailFmts eTrailingStyle() const;

        tCIDLib::ETrailFmts eTrailingStyle
        (
            const   tCIDLib::ETrailFmts  eStyle
        );

        constexpr tCIDLib::TFloat8 f8Val() const
        {
            return m_f8Val;
        }

        tCIDLib::TFloat8 f8Val
        (
            const   tCIDLib::TFloat8&       f8NewVal
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
        //  m_c1Precision
        //      This is the number of decimal places when formatting
        //
        //  m_eTrailingStyle
        //      Indicates whether insignficant trailing zeros should be
        //      formatted as zeros, spaces, or ignored.
        //
        //  m_f8Val
        //      Storage for the data value
        // -------------------------------------------------------------------
        tCIDLib::TCard1     m_c1Precision = 2;
        tCIDLib::ETrailFmts m_eTrailingStyle = tCIDLib::ETrailFmts::Zeroes;
        tCIDLib::TFloat8    m_f8Val = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFloat,TObject)
        DefPolyDup(TFloat)
        NulObject(TFloat)
};



// ---------------------------------------------------------------------------
//  CLASS: TLocFloat
// PREFIX: f
//
//  This class provides a double floating point value
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocFloat : public TFloat
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLocFloat();

        explicit TLocFloat
        (
            const   tCIDLib::TFloat8&       f8Val
        );

        TLocFloat
        (
            const   tCIDLib::TFloat8&       f8Val
            , const tCIDLib::TCard1         c1Precision
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TLocFloat
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::TCard1         c1Precision = 0
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TLocFloat
        (
            const   tCIDLib::TInt4          i4Val
            , const tCIDLib::TCard1         c1Precision = 0
            , const tCIDLib::ETrailFmts     eStyle = tCIDLib::ETrailFmts::Zeroes
        );

        TLocFloat
        (
            const   TLocFloat&              fToCopy
        );

        ~TLocFloat();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocFloat& operator=
        (
            const   TLocFloat&              fToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TLocFloat&              fToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLocFloat&              fToCompare
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TLocFloat&              fToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TLocFloat&              fToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TLocFloat&              fToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TLocFloat&              fToCompare
        )   const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLocFloat,TFloat)
        DefPolyDup(TLocFloat)
};

#pragma CIDLIB_POPPACK


