//
// FILE NAME: CIDLib_Integer.hpp
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
//  This is the definition file for the CIDLib_Integer.Cpp file. This file
//  implements a class wrapper for a signed integer. Its a very simple class
//  and makes no attempt to create a fully mathematical functional class. Its
//  purely for formatting purposes and for when an integer needs to act like
//  an object.
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
class   TFloat;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TInteger
// PREFIX: i
// ---------------------------------------------------------------------------
class CIDLIBEXP TInteger :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TInteger() = default;

        explicit TInteger
        (
            const   tCIDLib::TInt1          i1Val
        );

        explicit TInteger
        (
            const   tCIDLib::TInt2          i2Val
        );

        explicit TInteger
        (
            const   tCIDLib::TInt4          i4Val
        );

        explicit TInteger
        (
            const   tCIDLib::TSInt          iVal
        );

        TInteger
        (
            const   tCIDLib::TInt4          i4Val
            , const tCIDLib::ERadices       eRadix
        );

        TInteger(const TInteger&) = default;

        ~TInteger() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TInteger& operator=(const TInteger&) = default;

        constexpr operator tCIDLib::TInt4() const
        {
            return m_i4Val;
        }

        TInteger& operator=
        (
            const   tCIDLib::TInt4          i4NewVal
        );

        constexpr tCIDLib::TBoolean operator==(const TInteger& iToTest) const
        {
            return ((m_i4Val == iToTest.m_i4Val) && (m_eRadix == iToTest.m_eRadix));
        }

        constexpr tCIDLib::TBoolean operator!=(const TInteger& iToTest) const
        {
            return !operator==(iToTest);
        }

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TInteger&               iToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TInteger&               iToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TInteger&               iToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TInteger&               iToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TInt4          i4ToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        constexpr tCIDLib::TInt4 i4Val() const
        {
            return m_i4Val;
        }

        tCIDLib::TInt4 i4Val
        (
            const   tCIDLib::TInt4          i4NewVal
        );

        tCIDLib::ERadices eRadix() const;

        tCIDLib::ERadices eRadix
        (
            const   tCIDLib::ERadices       eNewRadix
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
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eRadix
        //      The display radix for the value
        //
        //  m_i4Val
        //      Storage for the data value.
        // -------------------------------------------------------------------
        tCIDLib::ERadices   m_eRadix = tCIDLib::ERadices::Dec;
        tCIDLib::TInt4      m_i4Val = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TInteger,TObject)
        DefPolyDup(TInteger)
        NulObject(TInteger)
};


// ---------------------------------------------------------------------------
//  CLASS: TInteger64
// PREFIX: i
// ---------------------------------------------------------------------------
class CIDLIBEXP TInteger64 :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TInteger64() = default;

        TInteger64
        (
            const   tCIDLib::TInt8&         i8Val
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TInteger64(const TInteger64&) = default;

        ~TInteger64() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TInteger64& operator=(const TInteger64&) = default;

        TInteger64& operator=
        (
            const   tCIDLib::TInt8&         i8NewVal
        );

        constexpr tCIDLib::TBoolean operator==(const TInteger64& iToTest) const
        {
            return ((m_i8Val == iToTest.m_i8Val) && (m_eRadix == iToTest.m_eRadix));
        }

        constexpr tCIDLib::TBoolean operator!=(const TInteger64& iToTest) const
        {
            return !operator==(iToTest);
        }

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TInteger64&             iToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TInteger64&             iToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TInteger64&             iToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TInteger64&             iToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TInt8&         i8ToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        constexpr tCIDLib::TInt8 i8Val() const
        {
            return m_i8Val;
        }

        tCIDLib::TInt8 i8Val
        (
            const   tCIDLib::TInt8&         i8NewVal
        );

        tCIDLib::ERadices eRadix() const;

        tCIDLib::ERadices eRadix
        (
            const   tCIDLib::ERadices       eNewRadix
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
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eRadix
        //      The display radix for the value
        //
        //  m_i8Val
        //      Storage for the data value.
        // -------------------------------------------------------------------
        tCIDLib::ERadices   m_eRadix = tCIDLib::ERadices::Dec;
        tCIDLib::TInt8      m_i8Val = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TInteger64,TObject)
        DefPolyDup(TInteger64)
        NulObject(TInteger64)
};


// ---------------------------------------------------------------------------
//  CLASS: TLocInteger
// PREFIX: i
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocInteger : public TInteger
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLocInteger();

        TLocInteger
        (
            const   tCIDLib::TInt4          i4Val
        );

        TLocInteger(const TLocInteger&) = default;

        ~TLocInteger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocInteger& operator=(const TLocInteger&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TLocInteger&            iToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLocInteger&            iToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TLocInteger&            iToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TLocInteger&            iToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TLocInteger&            iToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TLocInteger&            iToTest
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
        RTTIDefs(TLocInteger,TInteger)
        DefPolyDup(TLocInteger)
};


// ---------------------------------------------------------------------------
//  CLASS: TLocInteger64
// PREFIX: i
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocInteger64 : public TInteger64
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLocInteger64();

        TLocInteger64
        (
            const   tCIDLib::TInt8&         i8Val
        );

        TLocInteger64(const TLocInteger64&) = default;

        ~TLocInteger64();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocInteger64& operator=(const TLocInteger64&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TLocInteger64&          iToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLocInteger64&          iToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TLocInteger64&          iToTest
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TLocInteger64&          iToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TLocInteger64&          iToTest
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TLocInteger64&          iToTest
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
        RTTIDefs(TLocInteger64,TInteger64)
        DefPolyDup(TLocInteger64)
};

#pragma CIDLIB_POPPACK

