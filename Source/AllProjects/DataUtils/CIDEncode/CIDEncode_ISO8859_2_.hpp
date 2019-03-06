//
// FILE NAME: CIDEncode_ISO8859_2.hpp
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
//  from the ISO-8859-2 code page, aka Latin-2.
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
//   CLASS: TLatin2Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TLatin2Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLatin2Converter();

        TLatin2Converter
        (
            const   TLatin2Converter&       tcvtToCopy
        );

        ~TLatin2Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLatin2Converter& operator=
        (
            const   TLatin2Converter&       tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLatin2Converter,TMode1EncodingBase)
        DefPolyDup(TLatin2Converter)
};

#pragma CIDLIB_POPPACK


