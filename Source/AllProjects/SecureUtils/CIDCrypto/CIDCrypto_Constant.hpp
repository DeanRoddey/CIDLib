//
// FILE NAME: CIDCrypto_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/2001
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
//  This is the constants header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace kCIDCrypto
{
    // -----------------------------------------------------------------------
    //  Constants for the TUniqueId class
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4UniqueIdBytes     = 16;
    constexpr tCIDLib::TCard4   c4UniqueIdStrLen    = 33;


    // -----------------------------------------------------------------------
    //  Constants for the TMD5Hash class
    //
    //  NOTE:   If this changes from the unique id bytes above, that would
    //          be a problem, since they really need to be the same, but have
    //          separate names for self-documenting reasons.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4MD5HashBytes      = 16;


    // -----------------------------------------------------------------------
    //  Constants for the TSHA1Hash class
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4SHA1HashBytes     = 20;


    // -----------------------------------------------------------------------
    //  Constants for the TSHA256Hash class. These are 256 bytes
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4SHA256HashBytes   = 32;
    constexpr tCIDLib::TCard4   c4SHA256BlockSize   = 64;


    // -----------------------------------------------------------------------
    //  Constants for the machine id support.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   ac1IDKey[] =
    {
          0x16, 0x4A, 0x82, 0xFF, 0x82, 0xFF, 0x02, 0x93
        , 0x02, 0x16, 0x82, 0x16, 0xFF, 0x02, 0x93, 0xA3
        , 0x4A, 0xA3, 0xEC, 0x16, 0x82, 0xA3, 0xEC, 0x4A
        , 0x4A, 0x02, 0x93, 0x93, 0xA3, 0xEC, 0xFF, 0x02
    };
    const tCIDLib::TCard4   c4IDKeyBytes = tCIDLib::c4ArrayElems(ac1IDKey);
}


