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
tCIDLib::TCard4 TBinFile::c4CurPos() const
{
    return 0;
}


tCIDLib::TCard4 TBinFile::c4SeekToEnd()
{
    return 0;
}

tCIDLib::TCard4 TBinFile::c4Size() const
{
    return 0;
}


tCIDLib::TVoid TBinFile::Close()
{
    
}


tCIDLib::TVoid TBinFile::Open(  const   TBldStr&                strName
                                , const tCIDLib::EAccessModes   eAccess
                                , const tCIDLib::ECreateActs    eAction)
{
}


tCIDLib::TVoid TBinFile::Open(const tCIDLib::EStdFiles eStdFile)
{

}

tCIDLib::TVoid TBinFile::SeekTo(const tCIDLib::TCard4 c4To)
{
}


// ---------------------------------------------------------------------------
//  TBinFile: Private implementation methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TBinFile::c4ReadBytes(          tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4Max)
{
    return 0;
}


tCIDLib::TCard4
TBinFile::c4WriteBytes( const   tCIDLib::TVoid* const   pToWrite
                        , const tCIDLib::TCard4         c4ToWrite)
{
    return 0;
}
