//
// FILE NAME: CIDLib_StreamFmt.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/03/1994
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header defines the TStreamFmt class. This class is a janitor class
//  for the formatting settings of a TTextOutStream object.
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
//   CLASS: TStreamFmt
//  PREFIX: strmf
// ---------------------------------------------------------------------------
class CIDLIBEXP TStreamFmt :

    public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamFmt() = default;

        TStreamFmt
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Precision
            , const tCIDLib::EHJustify      eJustification
            , const tCIDLib::TCard4         c4Trailing = 0
        );

        TStreamFmt
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Precision
            , const tCIDLib::EHJustify      eJustification
            , const tCIDLib::TCh            chFill
            , const tCIDLib::TCard4         c4Trailing = 0
        );

        TStreamFmt
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Precision
            , const tCIDLib::EHJustify      eJustification
            , const tCIDLib::TCh            chFill
            , const tCIDLib::ERadices       eRadix
            , const tCIDLib::TCard4         c4Trailing = 0
        );

        TStreamFmt
        (
            const   tCIDLib::ERadices       eRadix
        );

        TStreamFmt
        (
            const   TTextOutStream&         strmSrc
        );

        TStreamFmt(const TStreamFmt&) = default;

        ~TStreamFmt() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamFmt& operator=(const TStreamFmt&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TStreamFmt&             strmfOther
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TStreamFmt&             strmfOther
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCh chFill() const;

        tCIDLib::TCh chFill
        (
            const   tCIDLib::TCh            chToSet
        );

        tCIDLib::TCard4 c4Precision() const;

        tCIDLib::TCard4 c4Precision
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4TrailingSpaces() const;

        tCIDLib::TCard4 c4TrailingSpaces
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Width() const;

        tCIDLib::TCard4 c4Width
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::EHJustify eJustification() const;

        tCIDLib::EHJustify eJustification
        (
            const   tCIDLib::EHJustify      eToSet
        );

        tCIDLib::ERadices eRadix() const;

        tCIDLib::ERadices eRadix
        (
            const   tCIDLib::ERadices       eToSet
        );


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TTextOutStream;


    protected   :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_chFill
        //      The file character to use when text formatting into a field
        //      larger than the text being formatted.
        //
        //  m_c4Precision
        //      The number of decimal digits to use when formatting a floating
        //      point value.
        //
        //  m_c4TrailingSp
        //      The field width to format into.
        //
        //  m_c4Width
        //      The field width to format into.

        //  m_eJustification
        //      This is the horizontal justification to use if an inserted
        //      string is smaller than the current field width.
        //
        //  m_eRadix
        //      The radix to use when formatting raw numeric values.
        // -------------------------------------------------------------------
        tCIDLib::TCh        m_chFill = kCIDLib::chSpace;
        tCIDLib::TCard4     m_c4Precision = 2;
        tCIDLib::TCard4     m_c4TrailingSp = 0;
        tCIDLib::TCard4     m_c4Width = 0;
        tCIDLib::EHJustify  m_eJustification = tCIDLib::EHJustify::Left;
        tCIDLib::ERadices   m_eRadix = tCIDLib::ERadices::Dec;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStreamFmt,TObject)
        DefPolyDup(TStreamFmt)
        NulObject(TStreamFmt)
};

#pragma CIDLIB_POPPACK

