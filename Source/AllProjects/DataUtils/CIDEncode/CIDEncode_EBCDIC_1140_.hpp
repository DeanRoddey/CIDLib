//
// FILE NAME: CIDEncode_EBCDIC_1140.hpp
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
//  from the EBCDIC-US encoding, aka IBM1140.
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
//   CLASS: TEBCDIC1140Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TEBCDIC1140Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCh chDecodeOne
        (
            const   tCIDLib::TCard1         c1ToDecode
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEBCDIC1140Converter();

        TEBCDIC1140Converter
        (
            const   TEBCDIC1140Converter&   tcvtToCopy
        );

        ~TEBCDIC1140Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEBCDIC1140Converter& operator=
        (
            const   TEBCDIC1140Converter&   tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEBCDIC1140Converter,TMode1EncodingBase)
        DefPolyDup(TEBCDIC1140Converter)
};

#pragma CIDLIB_POPPACK


