//
// FILE NAME: CIDLib_VolumeInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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
//  TVolumeInfo is a wrapper for the TKrnlFileSys::TVolumeInfo structure.
//  It is used to query the characteristics of a volume. CIDLib does not
//  do drives, it does volumes. But, for a drive based system, a volume
//  is just a path to the root directory of the drive.
//
// CAVEATS/GOTCHAS:
//
//  Volume info can be queried for a path. The path must be fully qualified,
//  and the platform driver will try to glean from this the info required to
//  find out what volume its on and return info for that volume.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TVolumeInfo
//  PREFIX: voli
// ---------------------------------------------------------------------------
class CIDLIBEXP  TVolumeInfo

    : public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strVolKey
        (
            const   TVolumeInfo&            voliSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TVolumeInfo();

        TVolumeInfo
        (
            const   TString&                strSrcPath
        );

        TVolumeInfo(const TVolumeInfo&) = default;

        TVolumeInfo
        (
            const   TKrnlVolumeInfo&        kvoliSrc
        );

        ~TVolumeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TVolumeInfo& operator=(const TVolumeInfo&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TVolumeInfo&            voliToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TVolumeInfo&            voliToCompare
        )   const;

        TVolumeInfo& operator=
        (
            const   TKrnlVolumeInfo&        kvoliToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MaxPathLenComp() const;

        tCIDLib::EVolumeFlags eFlags() const;

        tCIDLib::EVolHWTypes eHWType() const;

        const TString& strFileSysType() const;

        const TString& strVolume() const;

        const TString& strVolumeLabel() const;

        const TString& strVolumeSerialNum() const;

        tCIDLib::TVoid SetFromPath
        (
            const   TString&                strNewPath
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FromRaw
        (
            const   TKrnlVolumeInfo&        kvoliSrc
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxPathLen
        //      The maximum length of a path component.
        //
        //  m_eFlags
        //      These flags indicate various attributes of the file system
        //      of the volume.
        //
        //  m_eHWType
        //      The general type of hardware the volume is hosted on. It
        //      might be the 'unknown' type if it could not be determined.
        //
        //  m_strFileSysType
        //      The name of the type of file system, e.g. FAT or NTFS.
        //
        //  m_strVolume
        //      The name for to the volume that this info represents. For
        //      Win32, this is like C:\. For other platforms it might be
        //      some other form.
        //
        //  m_strVolumeLabel
        //      The volume label for this volume, if any. Some platforms
        //      allow for a label to be applied to volumes.
        //
        //  m_strVolumeSerialNum
        //      The serial number for this volume, formatted to a string, if
        //      any. Some platforms apply a serial number to volumes.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxPathCompLen;
        tCIDLib::EVolumeFlags   m_eFlags;
        tCIDLib::EVolHWTypes    m_eHWType;
        TString                 m_strFileSysType;
        TString                 m_strVolume;
        TString                 m_strVolumeLabel;
        TString                 m_strVolumeSerialNum;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TVolumeInfo,TObject)
        DefPolyDup(TVolumeInfo)
};

#pragma CIDLIB_POPPACK


