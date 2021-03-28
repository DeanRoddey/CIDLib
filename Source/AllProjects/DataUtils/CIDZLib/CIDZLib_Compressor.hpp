//
// FILE NAME: CIDZLib_Compressor.hpp
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
//  This is the header file for the CIDZLib_Compressor.Cpp file. This file
//  implements the zlib compression and decompression algorithms, and keeps
//  all the associated state for that.
//
//  This guy works in terms of streams. A binary input stream provides input
//  for the process, and a binary output stream accepts the results.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TZLibCompImpl;

// ---------------------------------------------------------------------------
//  CLASS: TZLibCompressor
// PREFIX: zlib
// ---------------------------------------------------------------------------
class CIDZLIBEXP TZLibCompressor : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TZLibCompressor();

        TZLibCompressor(const TZLibCompressor&) = delete;

        TZLibCompressor(TZLibCompressor&& zlibSrc) :

            m_pzimplThis(nullptr)
        {
            tCIDLib::Swap(m_pzimplThis, zlibSrc.m_pzimplThis);
        }

        ~TZLibCompressor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TZLibCompressor& operator=(const TZLibCompressor&) = delete;

        TZLibCompressor& operator=(TZLibCompressor&& zlibSrc)
        {
            if (&zlibSrc  != this)
            {
                tCIDLib::Swap(m_pzimplThis, zlibSrc.m_pzimplThis);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Compress
        (
                    TBinInStream&           strmInput
            ,       TBinOutStream&          strmOutput
            , const tCIDLib::TCard4         c4MaxInput = kCIDLib::c4MaxCard
        );

        tCIDLib::TCard4 c4Decompress
        (
                    TBinInStream&           strmInput
            ,       TBinOutStream&          strmOutput
            , const tCIDLib::TCard4         c4MaxInput = kCIDLib::c4MaxCard
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pzimplThis
        //      The actual code is all in an internal implementation class
        //      so that we don't have to expose lots of constants and whatnot.
        // -------------------------------------------------------------------
        TZLibCompImpl*  m_pzimplThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TZLibCompressor,TObject)
};

#pragma CIDLIB_POPPACK

