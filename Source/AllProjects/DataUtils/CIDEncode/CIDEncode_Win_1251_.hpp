//
// FILE NAME: CIDEncode_Win_1251.hpp
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
//  from the Windows 1251 code page, which is a Cyrillic code page.
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
//   CLASS: TWin1251Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TWin1251Converter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWin1251Converter();

        TWin1251Converter
        (
            const   TWin1251Converter&      tcvToCopy
        );

        ~TWin1251Converter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWin1251Converter& operator=
        (
            const   TWin1251Converter&      tcvToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWin1251Converter,TMode1EncodingBase)
        DefPolyDup(TWin1251Converter)
};

#pragma CIDLIB_POPPACK


