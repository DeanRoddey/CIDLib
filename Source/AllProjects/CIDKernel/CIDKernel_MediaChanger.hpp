//
// FILE NAME: CIDKernel_MediaChanger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_MediaChanger.Cpp file. This file
//  implements the TKrnlMediaChanger class, which encapsulates the interface
//  to CD/DVD changers that can be controlled via the standard media changer
//  IOCTL interface of Windows.
//
//  The only such changers at the time of this writing are the PowerFile
//  changers, and Sony VGP-XL1B (and some other variants) that are just
//  rebadged PowerFile boxes. These are Firewire based devices.
//
//  This depends on the presence of the powerfil.sys driver being present
//  in the system. This is by default in later versions of XP MCE (and
//  in Vista if VMC is installed) but is not there by default in XP (or
//  the version that might be installed is not the right one in most cases.)
//  So you will have to move that file from a system that has it to those
//  that don't. We provide a method to check for the driver being available.
//  There is also an .inf file involved I think.
//
//  All of the access if via IOCTL calls, which the above driver implements
//  in a way that is appropriate for these types of changers. The IOCTLs
//  involved are the 'MEDIUMCHANGER' oriented ones.
//
//  We also have a media changer handle class. We don't provide the header
//  part of it, since it's defined in the same header that defines all of
//  the handle classes. We just provide the implementation of it in our
//  cpp file(s), which will be platform dependent. This is what we use to
//  interact with the device.
//
//  We provide enumeration which will get some information about the currently
//  attached changers. Among the info returned is a 'changer number' which
//  must be provided to us so that we can open the device. The caller is
//  free to save all of the info when the user selects a changer, and later
//  validate that information by comparing it to the results returned for
//  the Nth changer, to try to catch the fact that they may have changed.
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
//   CLASS: TKrnlMediaChanger
//  PREFIX: kmch
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlMediaChanger
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        struct TMediaChgInfo
        {
            tCIDLib::TCard4 c4ChangerNum;
            tCIDLib::TCh    achDevPath[64];
            tCIDLib::TCh    achProductId[512];
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bEnumChangers
        (
                    TMediaChgInfo* const    pkmchiToFill
            , const tCIDLib::TCard4         c4ListSz
            ,       tCIDLib::TCard4&        c4Found
        );

        static tCIDLib::TBoolean bFindChangerNum
        (
            const   tCIDLib::TCard4         c4ChangerNum
            ,       TMediaChgInfo&          kmchiToFill
            ,       tCIDLib::TBoolean&      bStatus
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlMediaChanger();

        TKrnlMediaChanger
        (
            const   tCIDLib::TCard4         c4ChangerNum
        );

        TKrnlMediaChanger(const TKrnlMediaChanger&) = delete;

        ~TKrnlMediaChanger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlMediaChanger& operator=(const TKrnlMediaChanger&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bIsOpened() const;

        tCIDLib::TBoolean bIsSlotOccupied
        (
            const   tCIDLib::TCard4         c4SlotNum
            ,       tCIDLib::TBoolean&      bResult
        )   const;

        tCIDLib::TBoolean bQueryDriveInfo
        (
            const   tCIDLib::TCard4         c4DriveNum
            ,       tCIDLib::TCard4&        c4LoadedSlot
        )   const;

        tCIDLib::TBoolean bLoadSlot
        (
            const   tCIDLib::TCard4         c4ToSlot
        );

        tCIDLib::TBoolean bMoveDisc
        (
            const   tCIDLib::TCard4         c4SrcSlot
            , const tCIDLib::TCard4         c4TarSlot
        );

        tCIDLib::TBoolean bMountDisc
        (
            const   tCIDLib::TCard4         c4DriveNum
            , const tCIDLib::TCard4         c4SlotToPlay
        );

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bOpen
        (
            const   tCIDLib::TCard4         c4ChangerNum
        );

        tCIDLib::TBoolean bUnloadSlot
        (
            const   tCIDLib::TCard4         c4SrcSlot
        );

        tCIDLib::TBoolean bUnmountDisc
        (
            const   tCIDLib::TCard4         c4DriveNum
            , const tCIDLib::TCard4         c4TarSlot
        );

        const TMediaChgInfo& kmchiThis() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hmchgThis
        //      This is the platform specific data that is required to interact
        //      with a changer. It's an opaque blob of data publically.
        //
        //  m_kmciThis
        //      A copy of the changer info structure that we were given in
        //      the constructor. This has the info we need get the changer
        //      opened.
        // -------------------------------------------------------------------
        TMediaChgHandle m_hmchgThis;
        TMediaChgInfo   m_kmchiThis;
};

#pragma CIDLIB_POPPACK


