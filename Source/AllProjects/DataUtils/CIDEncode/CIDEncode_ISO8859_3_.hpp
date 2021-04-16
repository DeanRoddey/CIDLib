//
// FILE NAME: CIDEncode_ISO8859_3.hpp
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
//  This header defines a mode 1 converter that handles conversion to and
//  from the ISO-8859-3 code page, aka Latin-3.
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
//   CLASS: TLatin3Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TLatin3Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLatin3Converter();

        TLatin3Converter(const TLatin3Converter&) = default;
        TLatin3Converter(TLatin3Converter&&) = default;

        ~TLatin3Converter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLatin3Converter& operator=(const TLatin3Converter&) = default;
        TLatin3Converter& operator=(TLatin3Converter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLatin3Converter,TMode1EncodingBase)
        DefPolyDup(TLatin3Converter)
};

#pragma CIDLIB_POPPACK


