//
// FILE NAME: CIDBuild_BinFile_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Linux specific parts of the TUtils namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "../CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  TBinFile: Public implementation methods
// ---------------------------------------------------------------------------
tCIDBuild::TUInt TBinFile::uiCurPos() const
{
    return 0;
}


tCIDBuild::TUInt TBinFile::uiSize() const
{
    return 0;
}


// ---------------------------------------------------------------------------
//  TBinFile: Private implementation methods
// ---------------------------------------------------------------------------
tCIDBuild::TVoid TBinFile::Close()
{
    
}


tCIDBuild::TVoid TBinFile::__Open(  const   tCIDBuild::EAccessModes eAccess
                                    , const tCIDBuild::EOpenActions eAction)
{

}


tCIDBuild::TVoid TBinFile::__Open(const tCIDBuild::EStdFiles eStdFile)
{

}


tCIDBuild::TUInt
TBinFile::__uiReadBytes(        tCIDBuild::TVoid* const pToFill
                        , const tCIDBuild::TUInt        uiMax)
{
    return 0;
}


tCIDBuild::TUInt
TBinFile::__uiWriteBytes(   const   tCIDBuild::TVoid* const pToWrite
                            , const tCIDBuild::TUInt        uiToWrite)
{
    return 0;
}
