//
// FILE NAME: CIDKernel_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This file is the main constants file of the facility. Since this is
//  the core facility, it has all of the fundamental stuff. We use the name
//  space system to make this stuff fit into the CIDLib.Dll facility's
//  name spaces, since this facility is really hidden from end users.
//
//  Only the platform independent stuff is here. The platform dependent stuff
//  is in the CIDKernel_PlatformConstants.hpp file, in the per-platform
//  subdirectories.
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace kCIDLib
{
    // -----------------------------------------------------------------------
    //  The environment variables that are looked for by this facility
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const       pszErrDumpDir   = L"CID_ERRDUMPDIR";


    // -----------------------------------------------------------------------
    //  Our mapped boolean values, which just map to the built in C++ boolean
    //  values. Note that TBoolean is just an alias for 'bool'.
    // -----------------------------------------------------------------------
    const   tCIDLib::TBoolean       True            = true;
    const   tCIDLib::TBoolean       False           = false;


    // -----------------------------------------------------------------------
    //  Some very basic constants.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1           c1MaxCard       = 0xFF;
    const tCIDLib::TCard2           c2MaxCard       = 0xFFFF;
    const tCIDLib::TCard4           c4MaxCard       = 0xFFFFFFFF;
    const tCIDLib::TCard8           c8MaxCard       = 0xFFFFFFFFFFFFFFFF;

    const tCIDLib::TInt1            i1MaxInt        = 127;
    const tCIDLib::TInt2            i2MaxInt        = 32767;
    const tCIDLib::TInt4            i4MaxInt        = 2147483647;
    const tCIDLib::TInt8            i8MaxInt        = 9223372036854775807;

    const tCIDLib::TInt1            i1MinInt        = -128;
    const tCIDLib::TInt2            i2MinInt        = -32768;
    const tCIDLib::TInt4            i4MinInt        = -2147483647 - 1;
    const tCIDLib::TInt8            i8MinInt        = -9223372036854775807 - 1;

    const tCIDLib::TFloat4          f4MaxFloat      = 3.402823466e+38F;
    const tCIDLib::TFloat8          f8MaxFloat      = 1.7976931348623158e+308;

    const tCIDLib::TFloat4          f4MinFloat      = 1.175494351e-38F;
    const tCIDLib::TFloat8          f8MinFloat      = 2.2250738585072014e-308;

    const tCIDLib::TFloat4          f4Epsilon       = 1.192092896e-07F;
    const tCIDLib::TFloat4          f8Epsilon       = 2.2204460492503131e-016;

    const tCIDLib::TCard4           c4Sz_1K         = 1024;
    const tCIDLib::TCard4           c4Sz_2K         = 2048;
    const tCIDLib::TCard4           c4Sz_4K         = 4096;
    const tCIDLib::TCard4           c4Sz_8K         = 8192;
    const tCIDLib::TCard4           c4Sz_16K        = 16384;
    const tCIDLib::TCard4           c4Sz_32K        = 32768;
    const tCIDLib::TCard4           c4Sz_64K        = 0x10000;
    const tCIDLib::TCard4           c4Sz_128K       = 0x20000;
    const tCIDLib::TCard4           c4Sz_256K       = 0x40000;
    const tCIDLib::TCard4           c4Sz_1M         = 0x100000;
    const tCIDLib::TCard4           c4Sz_2M         = 0x200000;
    const tCIDLib::TCard4           c4Sz_4M         = 0x400000;
    const tCIDLib::TCard4           c4Sz_8M         = 0x800000;
    const tCIDLib::TCard4           c4Sz_16M        = 0x1000000;
    const tCIDLib::TCard4           c4Sz_32M        = 0x2000000;
    const tCIDLib::TCard4           c4Sz_64M        = 0x4000000;
    const tCIDLib::TCard4           c4Sz_128M       = 0x8000000;


    // -----------------------------------------------------------------------
    //  Some character oriented constants. Note that the native short and wide
    //  char types, TSCh and TCh, are defined by the platform, so those types
    //  and constants are in the per-platform headers.
    //
    //  Note that L'' and L"" type constants are of the local native wide char
    //  size, which may or may not be the same size as a Unicode char. Though
    //  their encoding MUST use Unicode code points, regardless of the storage
    //  size of chars.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4UniBytes      = 2;
    const tCIDLib::TCh              uchMaxUniChar   = tCIDLib::TCh(0xFFFF);
    const tCIDLib::TCard4           c4MaxUTF8Bytes  = 6;


    // -----------------------------------------------------------------------
    //  A magic encoding string which always represents the local tCIDLib::TCh
    //  format in memory, and how it would be in a string object.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const       pszNativeWCEncoding = L"$NativeWideChar$";


    // -----------------------------------------------------------------------
    //  Time related constants
    //
    //  The c4XXX time values are in milliseconds. The enctXXX ones are in
    //  100 nanosecond increments.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4MinYear       = 1601;
    const tCIDLib::TCard4           c4MaxYear       = 2100;
    const tCIDLib::TCard4           c4OneSecond     = 1000;
    const tCIDLib::TCard4           c4OneMinute     = 60 * 1000;
    const tCIDLib::TCard4           c4OneHour       = 60 * c4OneMinute;
    const tCIDLib::TCard4           c4OneDay        = 24 * c4OneHour;

    const tCIDLib::TEncodedTime     enctHalfSecond  = 5000000;
    const tCIDLib::TEncodedTime     enctOneMilliSec = 10000;
    const tCIDLib::TEncodedTime     enctOneSecond   = 10000000;
    const tCIDLib::TEncodedTime     enctOnePtFiveSec= 15000000;
    const tCIDLib::TEncodedTime     enctTwoSeconds  = 20000000;
    const tCIDLib::TEncodedTime     enctThreeSeconds= 30000000;
    const tCIDLib::TEncodedTime     enctFourSeconds = 40000000;
    const tCIDLib::TEncodedTime     enctFiveSeconds = 50000000;
    const tCIDLib::TEncodedTime     enctTenSeconds  = 100000000;
    const tCIDLib::TEncodedTime     enctOneMinute   = 60 * enctOneSecond;
    const tCIDLib::TEncodedTime     enctOneHour     = 60 * enctOneMinute;
    const tCIDLib::TEncodedTime     enctOneDay      = 24 * enctOneHour;


    // -----------------------------------------------------------------------
    //  A common use of Julian dates are Julian days since 01/01/1900, so this
    //  is the Julian for that special date. So subtract this from a regular
    //  Julian date to get the 1900 based date.
    // -----------------------------------------------------------------------
    const   tCIDLib::TCard4         c4Julian1900    = 2415021;


    // -----------------------------------------------------------------------
    //  Common mathematical constants
    // -----------------------------------------------------------------------
    const tCIDLib::TFloat4          f4PI            = 3.141592653589F;
    const tCIDLib::TFloat8          f8PI            = 3.141592653589;
    const tCIDLib::TFloat4          f4PIx2          = (3.141592653589F * 2.0F);
    const tCIDLib::TFloat8          f8PIx2          = (3.141592653589 * 2.0);
    const tCIDLib::TFloat8          f8HalfPI        = 1.570796325;
    const tCIDLib::TFloat4          f4PI_Sqr        = 9.869604379F;
    const tCIDLib::TFloat8          f8PI_Sqr        = 9.869604379;
    const tCIDLib::TFloat4          f4OneDegree     = (180.0F / f4PI);
    const tCIDLib::TFloat8          f8OneDegree     = (180.0 / f8PI);
    const tCIDLib::TFloat4          f4OneRadian     = 57.296F;
    const tCIDLib::TFloat8          f8OneRadian     = 57.296;
    const tCIDLib::TFloat8          f8Log10_PI      = 049.71;
    const tCIDLib::TFloat8          f8Log10_e       = 0.4343;
    const tCIDLib::TFloat8          f8Log10_2       = 0.30103;
    const tCIDLib::TFloat8          f8Log2_e        = 1.4427;
    const tCIDLib::TFloat8          f8Log2_10       = 3.32193;
    const tCIDLib::TFloat8          f8Loge_2        = 0.693147;
    const tCIDLib::TFloat4          f4e             = 2.7183F;
    const tCIDLib::TFloat8          f8e             = 2.7183;


    // -----------------------------------------------------------------------
    //  Miscellaneous constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4MaxWait       = 0xFFFFFFFF;
    const tCIDLib::TCard4           c4MaxCmdLineParms = 128;
    const tCIDLib::TCard4           c4ClassModulus  = 109;
    const tCIDLib::TEncodedTime     enctMaxWait     = 0xFFFFFFFFFFFFFFFF;
    const tCIDLib::TCh* const       pszEmptyZStr    = L"";
    const tCIDLib::TCard4           c4MsgIdModulus  = 109;
    const tCIDLib::TCh* const       pszNullStr      = L"<Null>";
    const tCIDLib::TCh* const       pszErrInfExt    = L".ErrorInf";
    const tCIDLib::TCh* const       pszPrimaryThrdName = L"$PrimaryThread$";



    // -----------------------------------------------------------------------
    //  The standard 16 bit CRC lookup table
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2 ac2CRCTable[256] =
    {
         0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241
       , 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440
       , 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40
       , 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841
       , 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40
       , 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41
       , 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641
       , 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040
       , 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240
       , 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441
       , 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41
       , 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840
       , 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41
       , 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40
       , 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640
       , 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041
       , 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240
       , 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441
       , 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41
       , 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840
       , 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41
       , 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40
       , 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640
       , 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041
       , 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241
       , 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440
       , 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40
       , 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841
       , 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40
       , 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41
       , 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641
       , 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };


    // -----------------------------------------------------------------------
    //  A map from the language enum to the suffix
    // -----------------------------------------------------------------------
    const tCIDLib::TCh apszLangSuffixes[tCIDLib::TCard4(tCIDLib::ELanguages::Count)][3] =
    {
        L"af"       // ELanguage_Afrikaans
        , L"al"     // ELanguage_Albanian
        , L"ar"     // ELanguage_Arabic
        , L"ba"     // ELanguage_Basque
        , L"be"     // ELanguage_Belarusian
        , L"bu"     // ELanguage_Bulgarian
        , L"ca"     // ELanguage_Catalan
        , L"ch"     // ELanguage_Chinese
        , L"cr"     // ELanguage_Croatian
        , L"cz"     // ELanguage_Czech
        , L"da"     // ELanguage_Danish
        , L"de"     // ELanguage_Dutch
        , L"en"     // ELanguage_English
        , L"fa"     // ELanguage_Farsi
        , L"fi"     // ELanguage_Finnish
        , L"fr"     // ELanguage_French
        , L"ge"     // ELanguage_German
        , L"gr"     // ELanguage_Greek
        , L"he"     // ELanguage_Hebrew
        , L"hu"     // ELanguage_Hungarian
        , L"ic"     // ELanguage_Icelandic
        , L"in"     // ELanguage_Indonesian
        , L"it"     // ELanguage_Italian
        , L"ja"     // ELanguage_Japanese
        , L"ko"     // ELanguage_Korean
        , L"la"     // ELanguage_Latvian
        , L"li"     // ELanguage_Lithuanian
        , L"no"     // ELanguage_Norwegian
        , L"po"     // ELanguage_Polish
        , L"pr"     // ELanguage_Portuguese
        , L"ro"     // ELanguage_Romanian
        , L"ru"     // ELanguage_Russian
        , L"se"     // ELanguage_Serbian
        , L"sl"     // ELanguage_Slovak
        , L"sv"     // ELanguage_Slovenian
        , L"sp"     // ELanguage_Spanish
        , L"sw"     // ELanguage_Swedish
        , L"tu"     // ELanguage_Turkish
        , L"uk"     // ELanguage_Ukrainian
        , L"vi"     // ELanguage_Vietnamese
    };


    // ------------------------------------------------------------------------
    //  Message file related constants
    // ------------------------------------------------------------------------
    const tCIDLib::TCh* const   szMsgFileExt    = L"MsgText";
    const tCIDLib::TCh* const   szBinMsgFileExt = L"CIDMsg";


    // ------------------------------------------------------------------------
    //  Ranges for the three categories of loadable text resources we support. We
    //  keep them in these non-overlapping ranges to make it easy to know what type
    //  of text to load, just from the id.
    // ------------------------------------------------------------------------
    const tCIDLib::TCard4   c4ErrId_First   = 0;
    const tCIDLib::TCard4   c4ErrId_Last    = 12999;

    const tCIDLib::TCard4   c4MsgId_First   = 13000;
    const tCIDLib::TCard4   c4MsgId_Last    = 29999;

    const tCIDLib::TCard4   c4ComId_First   = 30000;
    const tCIDLib::TCard4   c4ComId_Last    = 50000;
}


//
//  These need to look like they are part of CIDSock, but must be here becase
//  they are used in an underlying CIDKernel API.
//
namespace kCIDSock
{
    // -----------------------------------------------------------------------
    //  Host info constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MaxAddrs          = 32;
    const tCIDLib::TCard4   c4MaxHWAddrSz       = 64;
    const tCIDLib::TCard4   c4MaxAdapterNameLn  = 260;
    const tCIDLib::TCard4   c4MaxAdapterDescLn  = 132;
    const tCIDLib::TCard4   c4MaxIPAddrBytes    = 16;
}



//
//  These need to look like they are part of CIDCtrls, but must be here in
//  order to be shared with the resource compiler
//
namespace kCIDCtrls
{
    // ------------------------------------------------------------------------
    //  Resource file constants
    // ------------------------------------------------------------------------
    const tCIDLib::TCard4           c4ResFmtVersion = 1;
    const tCIDLib::TCard4           c4ResMagicVal   = 0xFACEBEEF;


    // ------------------------------------------------------------------------
    //  Max touch points supported for multi-touch
    // ------------------------------------------------------------------------
    const tCIDLib::TCard4           c4MaxTouchPoints = 10;
}

