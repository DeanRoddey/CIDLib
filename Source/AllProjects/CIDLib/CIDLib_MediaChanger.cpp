//
// FILE NAME: CIDLib_MediaChanger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2008
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TMediaChanger class.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMediaChanger,TObject)
RTTIDecls(TMediaChgInfo,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TMediaChgInfo
//  PREFIX: mchi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMediaChgInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMediaChgInfo::TMediaChgInfo() :

    m_c4ChangerNum(kCIDLib::c4MaxCard)
{
}

TMediaChgInfo::TMediaChgInfo(const  TString&        strProductId
                            , const TString&        strDevPath
                            , const tCIDLib::TCard4 c4ChangerNum) :

    m_c4ChangerNum(c4ChangerNum)
    , m_strDevPath(strDevPath)
    , m_strProductId(strProductId)
{
}

TMediaChgInfo::TMediaChgInfo(const TKrnlMediaChanger::TMediaChgInfo& kmchiSrc) :

    m_c4ChangerNum(kmchiSrc.c4ChangerNum)
    , m_strDevPath(kmchiSrc.achDevPath)
    , m_strProductId(kmchiSrc.achProductId)
{
}

TMediaChgInfo::~TMediaChgInfo()
{
}





// ---------------------------------------------------------------------------
//   CLASS: TMediaChanger
//  PREFIX: mch
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TMediaChanger: Public, static methods
// ---------------------------------------------------------------------------

//
//  Looks up the info for the indicated changer number and fills in a changer
//  info object for it.
//
tCIDLib::TBoolean
TMediaChanger::bFindChangerNum( const   tCIDLib::TCard4 c4ChangerNum
                                ,       TMediaChgInfo&  mciToFill)
{
    tCIDLib::TBoolean bFound;
    TKrnlMediaChanger::TMediaChgInfo kmchiFind;
    if (!TKrnlMediaChanger::bFindChangerNum(c4ChangerNum, kmchiFind, bFound))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_Find
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ChangerNum)
        );
    }

    if (bFound)
        mciToFill = TMediaChgInfo(kmchiFind);
    return bFound;
}


//
//  Enumerates all the changers in the system and returns a changer info
//  object for each one.
//
tCIDLib::TCard4 TMediaChanger::c4EnumChangers(TMChgInfoList& colToFill)
{
    const tCIDLib::TCard4 c4ListSz = 128;
    TKrnlMediaChanger::TMediaChgInfo* pkmchiList
    (
        new TKrnlMediaChanger::TMediaChgInfo[c4ListSz]
    );
    TArrayJanitor<TKrnlMediaChanger::TMediaChgInfo> janList(pkmchiList);

    tCIDLib::TCard4 c4Found;
    if (!TKrnlMediaChanger::bEnumChangers(pkmchiList, c4ListSz, c4Found))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_EnumError
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Go through them and give back an item for each one found
    colToFill.RemoveAll();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Found; c4Index++)
        colToFill.objAdd(TMediaChgInfo(pkmchiList[c4Index]));

    return c4Found;
}





// ---------------------------------------------------------------------------
//  TMediaChanger: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMediaChanger::TMediaChanger()
{
}

TMediaChanger::TMediaChanger(const tCIDLib::TCard4 c4ChangerNum) :

    m_kmchThis(c4ChangerNum)
{
}

TMediaChanger::~TMediaChanger()
{
}



// ---------------------------------------------------------------------------
//  TMediaChanger: Public, non-virtual methods
// ---------------------------------------------------------------------------


// Just returns whether the indicated slot is occupied or not
tCIDLib::TBoolean
TMediaChanger::bIsSlotOccupied(const tCIDLib::TCard4 c4SlotNum) const
{
    // Make sure we are opened first
    CheckIsOpened();

    tCIDLib::TBoolean bIsOcc;
    if (!m_kmchThis.bIsSlotOccupied(c4SlotNum, bIsOcc))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_GetStatus
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_mchiThis.m_strDevPath
        );
    }
    return bIsOcc;
}


//
//  Closes the driver if it's open. Closing when it's already closed is
//  not an error.
//
tCIDLib::TVoid TMediaChanger::Close()
{
    //
    //  Only need to do this if we are opened. We check so that if we
    //  get an error back, we know we have the changer info and can use
    //  it in the error message.
    //
    if (m_kmchThis.bIsOpened())
    {
        if (!m_kmchThis.bClose())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMChg_Close
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_mchiThis.m_strDevPath
            );
        }
    }
}


//
//  Asks the changer to allow the user to load a disc into the indicated
//  slot. The indicated slot must not be full.
//
tCIDLib::TVoid
TMediaChanger::LoadSlot(const tCIDLib::TCard4 c4TarSlot)
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bLoadSlot(c4TarSlot))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_MoveOpFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"load disc")
            , m_mchiThis.m_strDevPath
            , TInteger(0)
            , TCardinal(c4TarSlot)
        );
    }
}


//
//  Asks the changer for it's status, which is just the slot loaded
//  into the drive.
//
tCIDLib::TVoid
TMediaChanger::QueryDriveInfo(  const   tCIDLib::TCard4     c4DriveNum
                                ,       tCIDLib::TCard4&    c4LoadedSlot) const
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bQueryDriveInfo(c4DriveNum, c4LoadedSlot))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_GetStatus
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_mchiThis.m_strDevPath
        );
    }
}


//
//  Asks the changer to move a disc from a source slot to a target
//  slot. Obviously they must be occupied and unoccupied, respectively.
//
tCIDLib::TVoid
TMediaChanger::MoveDisc(const   tCIDLib::TCard4 c4SrcSlot
                        , const tCIDLib::TCard4 c4TarSlot)
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bMoveDisc(c4SrcSlot, c4TarSlot))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_MoveOpFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"move disc")
            , m_mchiThis.m_strDevPath
            , TCardinal(c4SrcSlot)
            , TCardinal(c4TarSlot)
        );
    }
}


//
//  Asks the changer to mount the indicated disc into the indicated
//  drive.
//
tCIDLib::TVoid
TMediaChanger::MountDisc(const  tCIDLib::TCard4 c4DriveNum
                        , const tCIDLib::TCard4 c4SlotToPlay)
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bMountDisc(c4DriveNum, c4SlotToPlay))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_MoveOpFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"mount disc")
            , m_mchiThis.m_strDevPath
            , TCardinal(c4SlotToPlay)
            , TCardinal(c4DriveNum)
        );
    }
}


//
//  Tries to open the drive number currently set on us. If the changer
//  is already open, this will fail. If the drive number has not be set
//  yet it will fail. We also have one that lets them provide the changer
//  number upon open, instead of setting it in the ctor.
//
tCIDLib::TVoid TMediaChanger::Open()
{
    if (m_kmchThis.bIsOpened())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_mchiThis.m_strDevPath
        );
    }

    if (!m_kmchThis.bOpen())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_mchiThis.m_c4ChangerNum)
        );
    }

    // Load up our CIDLib level changer info from the kernel level
    m_mchiThis = TMediaChgInfo(m_kmchThis.kmchiThis());
}


tCIDLib::TVoid TMediaChanger::Open(const tCIDLib::TCard4 c4ChangerNum)
{
    if (m_kmchThis.bIsOpened())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_mchiThis.m_strDevPath
        );
    }

    if (!m_kmchThis.bOpen(c4ChangerNum))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ChangerNum)
        );
    }

    // Load up our CIDLib level changer info from the kernel level
    m_mchiThis = TMediaChgInfo(m_kmchThis.kmchiThis());
}


//
//  Asks the changer to spit out the disc in the indicated source
//  slot, which of course must be occupied.
//
tCIDLib::TVoid
TMediaChanger::UnloadSlot(const tCIDLib::TCard4 c4SrcSlot)
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bUnloadSlot(c4SrcSlot))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_MoveOpFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"unload disc")
            , m_mchiThis.m_strDevPath
            , TCardinal(c4SrcSlot)
            , TInteger(0)
        );
    }

}


//
//  Asks the changer to unload the disc in the indicated drive and put
//  it into the indicated slot. The target slot must be empty of course.
//
tCIDLib::TVoid
TMediaChanger::UnmountDisc( const   tCIDLib::TCard4 c4DriveNum
                            , const tCIDLib::TCard4 c4TarSlot)
{
    // Make sure we are opened first
    CheckIsOpened();

    if (!m_kmchThis.bUnmountDisc(c4DriveNum, c4TarSlot))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_MoveOpFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"unmount disc")
            , m_mchiThis.m_strDevPath
            , TCardinal(c4DriveNum)
            , TCardinal(c4TarSlot)
        );
    }
}


// ---------------------------------------------------------------------------
//  TMediaChanger: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Makes sure we are opened, and if not will throw an exception. This avoids
//  a lot of grunt work above. Also, by doing a separate check, if there's
//  an error in the actual call to the kernel object, we know we have our
//  device path and can put it into the error message, whereas we cannot with
//  a not opened error, since it won't have been set yet.
//
tCIDLib::TVoid TMediaChanger::CheckIsOpened() const
{
    if (!m_kmchThis.bIsOpened())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMChg_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}


