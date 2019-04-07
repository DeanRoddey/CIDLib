//
// FILE NAME: CIDEncode_EBCDIC_037.hpp
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
//  from the EBCDIC-CP-US encoding, aka IBM037.
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
//   CLASS: TEBCDIC037Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TEBCDIC037Converter : public TMode1EncodingBase
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
        TEBCDIC037Converter();

        TEBCDIC037Converter
        (
            const   TEBCDIC037Converter&     tcvtToCopy
        );

        ~TEBCDIC037Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEBCDIC037Converter& operator=
        (
            const   TEBCDIC037Converter&     tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEBCDIC037Converter,TMode1EncodingBase)
        DefPolyDup(TEBCDIC037Converter)
};

#pragma CIDLIB_POPPACK


