//
// FILE NAME: CIDEncode_Mode1Base.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/1999
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
//  This header defines a base class from which all 'mode 1' encoders can
//  be derived. Mode 1 is an encoding where:
//
//  1)  The encoding is an 8 bit character set
//  2)  The encoding is a single byte character set
//  3)  There is a one to one mapping between character set and Unicode
//      code points.
//
//  The derived class just has to provide two tables. One provides a look
//  up table of 256 tCIDLib::TCh entries, where the entry for code point X
//  contains the Unicode mapping for that code point (or 0xFFFF if that code
//  point is not valid in that encoding.)
//
//  The other table is an array of tCIDEncode::TCharMap structures. The
//  entries are sorted by the chWide (the tCIDLib::TCh) field. It is of
//  variable size, since it has entries only for the valid target encodings
//  code points. It must be sorted correctly because it is binarily searched.
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
//   CLASS: TMode1EncodingBase
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TMode1EncodingBase : public TTextConverter
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TMode1EncodingBase();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDoTests
        (
                    TString&                strErr
        );

        tCIDLib::EBaseTextFmts eBaseFmt() const;

        tCIDLib::TVoid Reset();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TMode1EncodingBase
        (
            const   TString&                strEncodingName
            , const tCIDLib::TCh* const     achToWide
            , const tCIDEncode::TCharMap*   acmFromWide
            , const tCIDLib::TCard4         c4FromSize
        );

        TMode1EncodingBase
        (
            const   TMode1EncodingBase&     tcvtToCopy
        );

        TMode1EncodingBase& operator=
        (
            const   TMode1EncodingBase&     tcvtToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BlockFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCard4&        c4OutChars
            ,       tCIDLib::TBoolean&      bStop
        );

        tCIDLib::TCard4 c4BlockTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
            ,       tCIDLib::TBoolean&      bStop
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_acmFromWide
        //      This is the 'from' map. It contains the mappings from
        //      Unicode characters to the encoding code points. It has a
        //      variable size, in m_c4FromWideSize.
        //
        //  m_achToWide
        //      This is the 'to' map. It holds 256 wide characters.
        //
        //  m_c4FromMapSize
        //      The size of the from map. It is not a fixed size because
        //      it only has entries for the valid code points of the target
        //      encoding.
        // -------------------------------------------------------------------
        const tCIDEncode::TCharMap* m_acmFromWide;
        const tCIDLib::TCh*         m_achToWide;
        tCIDLib::TCard4             m_c4FromMapSize;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMode1EncodingBase,TTextConverter)
        DefPolyDup(TMode1EncodingBase)
};

#pragma CIDLIB_POPPACK


