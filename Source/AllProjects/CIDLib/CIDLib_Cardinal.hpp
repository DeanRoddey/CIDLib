//
// FILE NAME: CIDLib_Cardinal.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/1993
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
//  This is the header for the CIDLib_Cardinal.Cpp file. This file
//  implements a class wrapper (TCardinal) for a 32 bit unsigned integer. Its
//  a very simple class and makes no attempt to create a fully mathematical
//  functional class. Its purely for formatting purposes and for when a
//  cardinal value needs to act like an object. It does though provide simple
//  equality and relative magnitude operators.
//
//  A simple derivative, TLocCardinal, is provided also. This one just
//  overrides the text formatting stuff and formats according to locale
//  rules with grouping and separators and sign and all of that.
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
//  CLASS: TCardinal
// PREFIX: c
// ---------------------------------------------------------------------------
class CIDLIBEXP TCardinal :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TCardinal() = default;

        explicit TCardinal
        (
            const   tCIDLib::TCard1         c1Val
        );

        explicit TCardinal
        (
            const   tCIDLib::TCard2         c2Val
        );

        explicit TCardinal
        (
            const   tCIDLib::TCard4         c4Val
        );

        explicit TCardinal
        (
            const   tCIDLib::TUInt          uVal
        );

        explicit TCardinal
        (
            const   tCIDLib::TSInt          iVal
        );

        TCardinal
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::ERadices       eRadix
        );

        TCardinal(const TCardinal&) = default;
        TCardinal(TCardinal&&) = default;

        ~TCardinal() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCardinal& operator=(const TCardinal&) = default;

        TCardinal& operator=(TCardinal&&) = default;

        constexpr operator tCIDLib::TCard4() const
        {
            return m_c4Val;
        }

        constexpr tCIDLib::TBoolean operator==(const TCardinal& cToTest) const
        {
            return ((m_c4Val == cToTest.m_c4Val) && (m_eRadix == cToTest.m_eRadix));
        }

        constexpr tCIDLib::TBoolean operator!=(const TCardinal& cToTest) const
        {
            return !operator==(cToTest);
        }

        TCardinal& operator=
        (
            const   tCIDLib::TCard4         c4Src
        );

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TCard4         c4ToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TCard4         c4ToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TCardinal&              cToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TCardinal&              cToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TCardinal&              cToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TCardinal&              cToCompare
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TCard4         c4ToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TCard4         c4ToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TCard4         c4ToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TCard4         c4ToCompare
        )   const;

        TCardinal& operator+=
        (
            const   TCardinal&              cSrc
        );

        TCardinal& operator-=
        (
            const   TCardinal&              cSrc
        );

        TCardinal& operator+=
        (
            const   tCIDLib::TCard4         c4Src
        );

        TCardinal& operator-=
        (
            const   tCIDLib::TCard4         c4Src
        );

        TCardinal& operator--(int)
        {
            m_c4Val--;
            return *this;
        }

        TCardinal& operator++(int)
        {
            m_c4Val++;
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        constexpr tCIDLib::TCard4 c4Val() const
        {
            return m_c4Val;
        }

        tCIDLib::TCard4 c4Val
        (
            const   tCIDLib::TCard4         c4NewVal
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
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private             :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Val
        //      Storage for the data value.
        //
        //  m_eRadix
        //      This is the display radix
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Val = 0;
        tCIDLib::ERadices       m_eRadix = tCIDLib::ERadices::Dec;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCardinal,TObject)
        DefPolyDup(TCardinal)
        NulObject(TCardinal)
};



// ---------------------------------------------------------------------------
//  CLASS: TCardinal64
// PREFIX: c
// ---------------------------------------------------------------------------
class CIDLIBEXP TCardinal64 :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TCardinal64() = default;

        TCardinal64
        (
            const   tCIDLib::TCard8&        c8Val
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TCardinal64(const TCardinal64&) = default;
        TCardinal64(TCardinal64&&) = default;

        ~TCardinal64() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCardinal64& operator=(const TCardinal64&) = default;
        TCardinal64& operator=(TCardinal64&&) = default;

        TCardinal64& operator=
        (
            const   tCIDLib::TCard8&        c8ToAssign
        );

        constexpr operator tCIDLib::TCard8() const
        {
            return m_c8Val;
        }

        constexpr tCIDLib::TBoolean operator==(const TCardinal64& cToTest) const
        {
            return ((m_c8Val == cToTest.m_c8Val) && (m_eRadix == cToTest.m_eRadix));
        }

        constexpr tCIDLib::TBoolean operator!=(const TCardinal64& cToTest) const
        {
            return !operator==(cToTest);
        }

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TCard8&        c8ToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TCard8&        c8ToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TCardinal64&            cToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TCardinal64&            cToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TCardinal64&            cToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TCardinal64&            cToCompare
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TCard8&        c8ToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TCard8&        c8ToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TCard8&        c8ToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TCard8&        c8ToCompare
        )   const;

        TCardinal64& operator+=
        (
            const   TCardinal64&            cSrc
        );

        TCardinal64& operator-=
        (
            const   TCardinal64&            cSrc
        );

        TCardinal64& operator+=
        (
            const   tCIDLib::TCard8         c8Src
        );

        TCardinal64& operator-=
        (
            const   tCIDLib::TCard8         c8Src
        );

        TCardinal64& operator--(int)
        {
            m_c8Val--;
            return *this;
        }

        TCardinal64& operator++(int)
        {
            m_c8Val++;
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        constexpr tCIDLib::TCard8 c8Val() const
        {
            return m_c8Val;
        }

        tCIDLib::TCard8 c8Val
        (
            const   tCIDLib::TCard8&        c4NewVal
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
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private             :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8Val
        //      Storage for the data value.
        //
        //  m_eRadix
        //      This is the display radix
        // -------------------------------------------------------------------
        tCIDLib::TCard8     m_c8Val = 0;
        tCIDLib::ERadices   m_eRadix = tCIDLib::ERadices::Dec;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCardinal64,TObject)
        DefPolyDup(TCardinal64)
        NulObject(TCardinal64)
};


// ---------------------------------------------------------------------------
//  CLASS: TLocCardinal
// PREFIX: c
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocCardinal : public TCardinal
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLocCardinal();

        TLocCardinal
        (
            const   tCIDLib::TCard4         c4Val
        );

        TLocCardinal(const TLocCardinal&) = default;
        TLocCardinal(TLocCardinal&&) = default;

        ~TLocCardinal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocCardinal& operator=(const TLocCardinal&) = default;
        TLocCardinal& operator=(TLocCardinal&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TLocCardinal&           cToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLocCardinal&           cToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TLocCardinal&           cToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TLocCardinal&           cToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TLocCardinal&           cToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TLocCardinal&           cToCompare
        )   const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLocCardinal,TCardinal)
        DefPolyDup(TLocCardinal)
};


// ---------------------------------------------------------------------------
//  CLASS: TLocCardinal64
// PREFIX: c
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocCardinal64 : public TCardinal64
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TLocCardinal64();

        TLocCardinal64
        (
            const   tCIDLib::TCard8&        c8Val
        );

        TLocCardinal64(const TLocCardinal64&) = default;
        TLocCardinal64(TLocCardinal64&&) = default;

        ~TLocCardinal64();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocCardinal64& operator=(const TLocCardinal64&)  = default;
        TLocCardinal64& operator=(TLocCardinal64&&)  = default;

        tCIDLib::TBoolean operator==
        (
            const   TLocCardinal64&         cToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TLocCardinal64&         cToTest
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   TLocCardinal64&         cToCompare
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TLocCardinal64&         cToCompare
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TLocCardinal64&         cToCompare
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TLocCardinal64&         cToCompare
        )   const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLocCardinal64,TCardinal64)
        DefPolyDup(TLocCardinal64)
};

#pragma CIDLIB_POPPACK


