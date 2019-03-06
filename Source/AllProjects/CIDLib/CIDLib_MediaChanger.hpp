//
// FILE NAME: CIDLib_MediaChanger.hpp
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
//  This is the header for the CIDLib_MediaChanger.Cpp file. This file implements
//  the TMediaChanger class, which is a wrapper around the lower level CIDKernel
//  support for media changers (i.e. multi-disc changers.) So mostly this
//  class is just a passthrough, with some translation.
//
//  These have no inherent playback capabilities, only the ability to move
//  discs from slots to the drive and from the drive back to slots. It can
//  also load and eject discs.
//
//  We also provide a small class that is used to return info from the
//  enumeration and lookup static methods.
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
//   CLASS: TMediaChgInfo
//  PREFIX: mchi
// ---------------------------------------------------------------------------
class CIDLIBEXP TMediaChgInfo : public TObject
{
    public  :
        TMediaChgInfo();

        TMediaChgInfo
        (
            const   TString&                strProductId
            , const TString&                strDevPath
            , const tCIDLib::TCard4         c4ChangerNum
        );

        TMediaChgInfo
        (
            const   TKrnlMediaChanger::TMediaChgInfo& kmchiSrc
        );

        ~TMediaChgInfo();


        // -------------------------------------------------------------------
        //  Public data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4ChangerNum;
        TString         m_strDevPath;
        TString         m_strProductId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMediaChgInfo,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TMediaChanger
//  PREFIX: mch
// ---------------------------------------------------------------------------
class CIDLIBEXP TMediaChanger : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TMChgInfoList = TCollection<TMediaChgInfo>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindChangerNum
        (
            const   tCIDLib::TCard4         c4ChangerNum
            ,       TMediaChgInfo&          mciToFill
        );

        static tCIDLib::TCard4 c4EnumChangers
        (
                    TMChgInfoList&          colToFill
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TMediaChanger();

        TMediaChanger
        (
            const   tCIDLib::TCard4         c4ChangerNum
        );

        ~TMediaChanger();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSlotOccupied
        (
            const   tCIDLib::TCard4         c4SlotNum
        )   const;

        tCIDLib::TVoid Close();

        tCIDLib::TVoid LoadSlot
        (
            const   tCIDLib::TCard4         c4ToSlot
        );

        tCIDLib::TVoid QueryDriveInfo
        (
            const   tCIDLib::TCard4         c4DriveNum
            ,       tCIDLib::TCard4&        c4LoadedSlot
        )   const;

        tCIDLib::TVoid MoveDisc
        (
            const   tCIDLib::TCard4         c4SrcSlot
            , const tCIDLib::TCard4         c4TarSlot
        );

        tCIDLib::TVoid MountDisc
        (
            const   tCIDLib::TCard4         c4DriveNum
            , const tCIDLib::TCard4         c4SlotToPlay
        );

        tCIDLib::TVoid Open();

        tCIDLib::TVoid Open
        (
            const   tCIDLib::TCard4         c4ChangerNum
        );

        tCIDLib::TVoid UnloadSlot
        (
            const   tCIDLib::TCard4         c4SrcSlot
        );

        tCIDLib::TVoid UnmountDisc
        (
            const   tCIDLib::TCard4         c4DriveNum
            , const tCIDLib::TCard4         c4ToSlot
        );


    private             :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIsOpened() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kmchThis
        //      The underlying kernel level media changer class that actually
        //      presents our changer.
        //
        //  m_mchiThis
        //      The media changer info for our changer. This is only valid
        //      when the changer is opened.
        // -------------------------------------------------------------------
        TKrnlMediaChanger   m_kmchThis;
        TMediaChgInfo       m_mchiThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMediaChanger,TObject)
};

#pragma CIDLIB_POPPACK


