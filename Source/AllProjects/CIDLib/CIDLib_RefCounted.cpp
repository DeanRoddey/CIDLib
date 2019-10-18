//
// FILE NAME: CIDLib_RefCounted.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2019
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
//  This file just implements the wee bit of out of line stuff for the ref countable
//  mixing.
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


//
//  A very simple class that implements this interface, which we can use as the
//  nul object instance.
//
class TNulRefCounted : public TObject, public MRefCounted
{
    private :
        RTTIDefs(TNulRefCounted,TObject)
};
RTTIDecls(TNulRefCounted,TObject)




// ---------------------------------------------------------------------------
//   CLASS: MRefCounted
//  PREFIX: mrefc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MRefCounted: Public, static methods
// ---------------------------------------------------------------------------
MRefCounted& MRefCounted::Nul_MRefCounted()
{
    static TNulRefCounted mrefcNull;
    return mrefcNull;
}


// ---------------------------------------------------------------------------
//  MRefCounted: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean MRefCounted::bDecMRefCount()
{
    if (!m_c4MRefCount)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRefCnt_AlreadyZero
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Underflow
        );
    }

    m_c4MRefCount--;
    return (m_c4MRefCount > 0);
}


tCIDLib::TVoid MRefCounted::IncMRefCount()
{
    m_c4MRefCount++;
}


//
//  Sometimes they just want to do bulk release and need to force all of the counts
//  back to the default 1 value.
//
tCIDLib::TVoid MRefCounted::ResetMRefCount()
{
    m_c4MRefCount = 1;
}
