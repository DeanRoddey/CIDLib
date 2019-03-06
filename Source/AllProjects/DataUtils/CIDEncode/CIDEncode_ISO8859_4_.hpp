//
// FILE NAME: CIDEncode_ISO8859_4.hpp
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
//  from the ISO-8859-4 code page, aka Latin-4.
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
//   CLASS: TLatin4Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TLatin4Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLatin4Converter();

        TLatin4Converter
        (
            const   TLatin4Converter&       tcvtToCopy
        );

        ~TLatin4Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLatin4Converter& operator=
        (
            const   TLatin4Converter&       tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLatin4Converter,TMode1EncodingBase)
        DefPolyDup(TLatin4Converter)
};

#pragma CIDLIB_POPPACK


