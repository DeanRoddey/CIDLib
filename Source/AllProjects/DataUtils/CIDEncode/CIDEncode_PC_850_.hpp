//
// FILE NAME: CIDEncode_PC_850.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/15/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header defines a mode 1 converter that handles conversion to and
//  from the CP850 code page, which is a common code page on non-US IBM
//  PCs.
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
//   CLASS: TPC850Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TPC850Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPC850Converter();

        TPC850Converter
        (
            const   TPC850Converter&       tcvToCopy
        );

        ~TPC850Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPC850Converter& operator=
        (
            const   TPC850Converter&       tcvToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPC850Converter,TMode1EncodingBase)
        DefPolyDup(TPC850Converter)
};

#pragma CIDLIB_POPPACK


