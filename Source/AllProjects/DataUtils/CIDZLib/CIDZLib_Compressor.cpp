//
// FILE NAME: CIDZLib_Compressor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/14/2004
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
//  This file implements ZLib compressiong and decompression algorithm.
//  Because of the size we have a secondary implementation file to hold some
//  grunt work methods, so this one just holds the main logic.
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
#include    "CIDZLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TZLibCompressor,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TZLibCompressor
// PREFIX: zlib
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TZLibCompressor: Constructors and Destructor
// ---------------------------------------------------------------------------
TZLibCompressor::TZLibCompressor() :

    m_pzimplThis(nullptr)
{
}

TZLibCompressor::~TZLibCompressor()
{
    // Clean up the imple objects if we created it
    delete m_pzimplThis;
}


// ---------------------------------------------------------------------------
//  TZLibCompressor: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TZLibCompressor::c4Compress(        TBinInStream&   strmInput
                            ,       TBinOutStream&  strmOutput
                            , const tCIDLib::TCard4 c4InputBytes)
{
    //
    //  If it's not been initialized yet, then go ahead and initialize it
    //  with default values.
    //
    if (!m_pzimplThis)
    {
        m_pzimplThis = new TZLibCompImpl
        (
            tCIDZLib::ECompLevels::Default
            , tCIDZLib_::EStrategies::Default
        );
    }

    //
    //  Flush the output stream and remember the offset. This lets us figure out
    //  how much data was written.
    //
    strmOutput.Flush();
    const tCIDLib::TCard4 c4OrgPos = strmOutput.c4CurPos();

    // Do the compression
    m_pzimplThis->c4Compress(strmInput, strmOutput, c4InputBytes);

    // Now flush it again and return the difference
    strmOutput.Flush();
    return strmOutput.c4CurPos() - c4OrgPos;
}


tCIDLib::TCard4
TZLibCompressor::c4Decompress(          TBinInStream&   strmInput
                                ,       TBinOutStream&  strmOutput
                                , const tCIDLib::TCard4 c4InputBytes)
{
    //
    //  If it's not been initialized yet, then go ahead and initialize it
    //  with default values.
    //
    if (!m_pzimplThis)
    {
        m_pzimplThis = new TZLibCompImpl
        (
            tCIDZLib::ECompLevels::Default
            , tCIDZLib_::EStrategies::Default
        );
    }

    //
    //  Flush the output stream and remember the offset. This lets us figure out
    //  how much data was written.
    //
    strmOutput.Flush();
    const tCIDLib::TCard4 c4OrgPos = strmOutput.c4CurPos();

    m_pzimplThis->c4Decompress(strmInput, strmOutput, c4InputBytes);

    // Now flush it again and return the difference
    strmOutput.Flush();
    return strmOutput.c4CurPos() - c4OrgPos;
}

