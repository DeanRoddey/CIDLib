//
// FILE NAME: CIDEncode_ZWaveOEM_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/16/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header defines a mode 1 converter that handles conversion to and
//  from the OEM character set used by the Z-Wave NAMING command class.
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
//   CLASS: TZWaveOEMConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TZWaveOEMConverter : public TMode1EncodingBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TZWaveOEMConverter();

        TZWaveOEMConverter
        (
            const   TZWaveOEMConverter&     tcvToCopy
        );

        ~TZWaveOEMConverter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TZWaveOEMConverter& operator=
        (
            const   TZWaveOEMConverter&     tcvToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TZWaveOEMConverter,TMode1EncodingBase)
        DefPolyDup(TZWaveOEMConverter)
};

#pragma CIDLIB_POPPACK


