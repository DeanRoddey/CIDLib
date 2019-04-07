//
// FILE NAME: CIDLib_NativeWCConverter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/26/1999
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
//  This header defines a derivative of TTextConverter that is used for the
//  special scenario of wanting the text to be in the native wide character
//  format, i.e. the tCIDLib::TCh format. tCIDLib::TCh always represents
//  Unicode code points; however, it might not be a UTF-16 format. It can
//  also be a 32 bit value.
//
//  So this converter is dedicated to just passing through text in the same
//  format, with no byte swapping. Mainly its just for use by the test streams
//  that stream to/from a TString object. These streams must insure that the
//  output is valid for a TString object, and that source streamed from the
//  TString object is in native format. So this class is used, which can
//  provide the maximum optimization of this operation.
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
//   CLASS: TNativeWCConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TNativeWCConverter : public TTextConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNativeWCConverter();

        TNativeWCConverter
        (
            const   TNativeWCConverter&     tcvtToCopy
        );

        ~TNativeWCConverter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNativeWCConverter& operator=
        (
            const   TNativeWCConverter&     tcvtToAssign
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EBaseTextFmts eBaseFmt() const override;

        tCIDLib::TVoid Reset() override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BlockFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCard4&        c4OutChars
            ,       tCIDLib::TBoolean&      bStop
        );

        tCIDLib::TCard4 c4BlockTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
            ,       tCIDLib::TBoolean&      bStop
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNativeWCConverter,TTextConverter)
        DefPolyDup(TNativeWCConverter)
};

#pragma CIDLIB_POPPACK


