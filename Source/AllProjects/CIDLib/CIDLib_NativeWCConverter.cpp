//
// FILE NAME: CIDLib_NativeWCConverter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/26/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TNativeWCConverter class, which is a specialized
//  text converter that is basically a 'no op' for streaming out text in the
//  native 'in memory' wide character format. Its mainly designed for
//  streaming to/from a TString object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros. We support advanced RTTI so that converters can be
//  created by class name.
// ---------------------------------------------------------------------------
AdvRTTIDecls(TNativeWCConverter,TTextConverter)


// ---------------------------------------------------------------------------
//   CLASS: TNativeWCConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TNativeWCConverter: Constructors and Destructor
// ---------------------------------------------------------------------------
TNativeWCConverter::TNativeWCConverter() :

    TTextConverter(kCIDLib::pszNativeWCEncoding)
{
}

TNativeWCConverter::TNativeWCConverter(const TNativeWCConverter& tcvtToCopy) :

    TTextConverter(tcvtToCopy)
{
}

TNativeWCConverter::~TNativeWCConverter()
{
}


// ---------------------------------------------------------------------------
// TNativeWCConverter: Public operators
// ---------------------------------------------------------------------------
TNativeWCConverter&
TNativeWCConverter::operator=(const TNativeWCConverter& tcvtToAssign)
{
    if (this == &tcvtToAssign)
        return *this;
    TTextConverter::operator=(tcvtToAssign);
    return *this;
}


// ---------------------------------------------------------------------------
// TNativeWCConverter: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EBaseTextFmts TNativeWCConverter::eBaseFmt() const
{
    #if defined(CIDLIB_LITTLEENDIAN)
    return tCIDLib::EBaseTextFmts::TwoByte_LE;
    #endif

    #if defined(CIDLIB_BIGENDIAN)
    return tCIDLib::EBaseTextFmts::TwoByte_BE;
    #endif
}


tCIDLib::TVoid TNativeWCConverter::Reset()
{
    // This is a no-op for us
}


// ---------------------------------------------------------------------------
// TNativeWCConverter: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TNativeWCConverter::c4BlockFrom(const   tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       tCIDLib::TCh* const     pszToFill
                                , const tCIDLib::TCard4         c4MaxChars
                                ,       tCIDLib::TCard4&        c4OutChars
                                ,       tCIDLib::TBoolean&      bStop)
{
    bStop = kCIDLib::False;

    //
    //  Lets calculate how many chars we can copy from the input buffer,
    //  given the bytes per character.
    //
    const tCIDLib::TCard4 c4MaxIn = c4SrcBytes / kCIDLib::c4CharBytes;

    //
    //  The number of chars we can do then is the smaller of the in bytes and
    //  the target max chars.
    //
    c4OutChars = tCIDLib::MinVal(c4MaxChars, c4MaxIn);

    // Calc the bytes that we are going to copy over
    const tCIDLib::TCard4 c4BytesDone = c4OutChars * kCIDLib::c4CharBytes;

    //
    //  And we can now just copy over in bulk the chars that will fit. We
    //  don't have to deal with byte swapping at all in this particular
    //  converter.
    //
    TRawMem::CopyMemBuf(pszToFill, pc1Src, c4BytesDone);

    // Return the bytes we did copy over
    return c4BytesDone;
}


tCIDLib::TCard4
TNativeWCConverter::c4BlockTo(  const   tCIDLib::TCh* const     pszSrc
                                , const tCIDLib::TCard4         c4SrcChars
                                ,       tCIDLib::TCard1* const  pc1ToFill
                                , const tCIDLib::TCard4         c4MaxBytes
                                ,       tCIDLib::TCard4&        c4OutBytes
                                ,       tCIDLib::TBoolean&      bStop)
{
    bStop = kCIDLib::False;

    //
    //  Lets calculate how many chars we can copy to the output buffer,
    //  given the bytes per character.
    //
    const tCIDLib::TCard4 c4MaxOut = c4MaxBytes / kCIDLib::c4CharBytes;

    //
    //  The number of chars we can do then is the smaller of the max out
    //  and the source count.
    //
    const tCIDLib::TCard4 c4CharsDone = tCIDLib::MinVal(c4SrcChars, c4MaxOut);

    // Go ahead and set the bytes we are going to put into the output
    c4OutBytes = c4CharsDone * kCIDLib::c4CharBytes;

    //
    //  And we can now just copy over in bulk the chars that will fit. We
    //  don't have to deal with byte swapping at all in this particular
    //  converter.
    //
    TRawMem::CopyMemBuf(pc1ToFill, pszSrc, c4OutBytes);

    // return the chars that we did
    return c4CharsDone;
}


