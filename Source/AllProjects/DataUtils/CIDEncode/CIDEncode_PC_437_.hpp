//
// FILE NAME: CIDEncode_PC_437.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/15/2000
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
//  from the CP437 code page, which is a common code page for US IBM PCs.
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
//   CLASS: TPC437Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TPC437Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPC437Converter();

        TPC437Converter(const TPC437Converter&) = default;
        TPC437Converter(TPC437Converter&&) = default;

        ~TPC437Converter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPC437Converter& operator=(const TPC437Converter&) = default;
        TPC437Converter& operator=(TPC437Converter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPC437Converter,TMode1EncodingBase)
        DefPolyDup(TPC437Converter)
};

#pragma CIDLIB_POPPACK


