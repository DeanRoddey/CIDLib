//
// FILE NAME: CIDEncode_ISO8859_5.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header defines a mode 1 converter that handles conversion to and
//  from the ISO-8859-5 code page, aka Latin-5 or Cyrillic.
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
//   CLASS: TLatin5Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TLatin5Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLatin5Converter();

        TLatin5Converter
        (
            const   TLatin5Converter&       tcvtToCopy
        );

        ~TLatin5Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLatin5Converter& operator=
        (
            const   TLatin5Converter&       tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLatin5Converter,TMode1EncodingBase)
        DefPolyDup(TLatin5Converter)
};

#pragma CIDLIB_POPPACK


