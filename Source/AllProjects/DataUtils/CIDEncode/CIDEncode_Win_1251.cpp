//
// FILE NAME: CIDEncode_Win_1251.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/15/2000
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
//  This file implements the TWin1251Converter class, which handles converting
//  the CP1251 encoding to and from the internal Unicode format. We really
//  just have to provide it the mapping arrays.
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
#include    "CIDEncode_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros. We support advanced RTTI so that converters can be
//  created by class name.
// ---------------------------------------------------------------------------
AdvRTTIDecls(TWin1251Converter,TMode1EncodingBase)

namespace
{
    namespace CIDEncode_Win1251
    {
        // -----------------------------------------------------------------------
        //  Local const data
        //
        //  achFromMap
        //      This is the 'from' map that maps from the external encoding to the
        //      internal Unicode encoding. External code point X is used to index
        //      the table, and that entry holds the equivalent Unicode value. If
        //      there is no legal external code point at that index, its 0xFFFF.
        //
        //  achToMap
        //      This is the table that maps from the internal Unicode encoding to
        //      the external encoding. The table is sorted by the Unicode value.
        //
        //  c1RepChar
        //      This is the replacement char, which is used to represent Unicode
        //      values that cannot be represented, if converter is set up to do
        //      that.
        //
        //  c4ToMapSize
        //      This is the number of entries in the 'to' map. Since there can be
        //      more than one Unicode value that maps to a particular external
        //      code point, its not just a 256 entry table.
        // ---------------------------------------------------------------------------
        constexpr tCIDLib::TCh achFromMap[256] =
        {
            0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007
          , 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F
          , 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017
          , 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F
          , 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027
          , 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F
          , 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037
          , 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F
          , 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047
          , 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F
          , 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057
          , 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F
          , 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067
          , 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F
          , 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077
          , 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F
          , 0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021
          , 0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F
          , 0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014
          , 0xFFFF, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F
          , 0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7
          , 0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407
          , 0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7
          , 0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457
          , 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417
          , 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F
          , 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427
          , 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F
          , 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437
          , 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F
          , 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447
          , 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
        };
        constexpr tCIDEncode::TCharMap acmToMap[] =
        {
            { 0x0000, 0x00 }, { 0x0001, 0x01 }, { 0x0002, 0x02 }, { 0x0003, 0x03 }
          , { 0x0004, 0x04 }, { 0x0005, 0x05 }, { 0x0006, 0x06 }, { 0x0007, 0x07 }
          , { 0x0008, 0x08 }, { 0x0009, 0x09 }, { 0x000A, 0x0A }, { 0x000B, 0x0B }
          , { 0x000C, 0x0C }, { 0x000D, 0x0D }, { 0x000E, 0x0E }, { 0x000F, 0x0F }
          , { 0x0010, 0x10 }, { 0x0011, 0x11 }, { 0x0012, 0x12 }, { 0x0013, 0x13 }
          , { 0x0014, 0x14 }, { 0x0015, 0x15 }, { 0x0016, 0x16 }, { 0x0017, 0x17 }
          , { 0x0018, 0x18 }, { 0x0019, 0x19 }, { 0x001A, 0x1A }, { 0x001B, 0x1B }
          , { 0x001C, 0x1C }, { 0x001D, 0x1D }, { 0x001E, 0x1E }, { 0x001F, 0x1F }
          , { 0x0020, 0x20 }, { 0x0021, 0x21 }, { 0x0022, 0x22 }, { 0x0023, 0x23 }
          , { 0x0024, 0x24 }, { 0x0025, 0x25 }, { 0x0026, 0x26 }, { 0x0027, 0x27 }
          , { 0x0028, 0x28 }, { 0x0029, 0x29 }, { 0x002A, 0x2A }, { 0x002B, 0x2B }
          , { 0x002C, 0x2C }, { 0x002D, 0x2D }, { 0x002E, 0x2E }, { 0x002F, 0x2F }
          , { 0x0030, 0x30 }, { 0x0031, 0x31 }, { 0x0032, 0x32 }, { 0x0033, 0x33 }
          , { 0x0034, 0x34 }, { 0x0035, 0x35 }, { 0x0036, 0x36 }, { 0x0037, 0x37 }
          , { 0x0038, 0x38 }, { 0x0039, 0x39 }, { 0x003A, 0x3A }, { 0x003B, 0x3B }
          , { 0x003C, 0x3C }, { 0x003D, 0x3D }, { 0x003E, 0x3E }, { 0x003F, 0x3F }
          , { 0x0040, 0x40 }, { 0x0041, 0x41 }, { 0x0042, 0x42 }, { 0x0043, 0x43 }
          , { 0x0044, 0x44 }, { 0x0045, 0x45 }, { 0x0046, 0x46 }, { 0x0047, 0x47 }
          , { 0x0048, 0x48 }, { 0x0049, 0x49 }, { 0x004A, 0x4A }, { 0x004B, 0x4B }
          , { 0x004C, 0x4C }, { 0x004D, 0x4D }, { 0x004E, 0x4E }, { 0x004F, 0x4F }
          , { 0x0050, 0x50 }, { 0x0051, 0x51 }, { 0x0052, 0x52 }, { 0x0053, 0x53 }
          , { 0x0054, 0x54 }, { 0x0055, 0x55 }, { 0x0056, 0x56 }, { 0x0057, 0x57 }
          , { 0x0058, 0x58 }, { 0x0059, 0x59 }, { 0x005A, 0x5A }, { 0x005B, 0x5B }
          , { 0x005C, 0x5C }, { 0x005D, 0x5D }, { 0x005E, 0x5E }, { 0x005F, 0x5F }
          , { 0x0060, 0x60 }, { 0x0061, 0x61 }, { 0x0062, 0x62 }, { 0x0063, 0x63 }
          , { 0x0064, 0x64 }, { 0x0065, 0x65 }, { 0x0066, 0x66 }, { 0x0067, 0x67 }
          , { 0x0068, 0x68 }, { 0x0069, 0x69 }, { 0x006A, 0x6A }, { 0x006B, 0x6B }
          , { 0x006C, 0x6C }, { 0x006D, 0x6D }, { 0x006E, 0x6E }, { 0x006F, 0x6F }
          , { 0x0070, 0x70 }, { 0x0071, 0x71 }, { 0x0072, 0x72 }, { 0x0073, 0x73 }
          , { 0x0074, 0x74 }, { 0x0075, 0x75 }, { 0x0076, 0x76 }, { 0x0077, 0x77 }
          , { 0x0078, 0x78 }, { 0x0079, 0x79 }, { 0x007A, 0x7A }, { 0x007B, 0x7B }
          , { 0x007C, 0x7C }, { 0x007D, 0x7D }, { 0x007E, 0x7E }, { 0x007F, 0x7F }
          , { 0x00A0, 0xA0 }, { 0x00A4, 0xA4 }, { 0x00A6, 0xA6 }, { 0x00A7, 0xA7 }
          , { 0x00A9, 0xA9 }, { 0x00AB, 0xAB }, { 0x00AC, 0xAC }, { 0x00AD, 0xAD }
          , { 0x00AE, 0xAE }, { 0x00B0, 0xB0 }, { 0x00B1, 0xB1 }, { 0x00B5, 0xB5 }
          , { 0x00B6, 0xB6 }, { 0x00B7, 0xB7 }, { 0x00BB, 0xBB }, { 0x0401, 0xA8 }
          , { 0x0402, 0x80 }, { 0x0403, 0x81 }, { 0x0404, 0xAA }, { 0x0405, 0xBD }
          , { 0x0406, 0xB2 }, { 0x0407, 0xAF }, { 0x0408, 0xA3 }, { 0x0409, 0x8A }
          , { 0x040A, 0x8C }, { 0x040B, 0x8E }, { 0x040C, 0x8D }, { 0x040E, 0xA1 }
          , { 0x040F, 0x8F }, { 0x0410, 0xC0 }, { 0x0411, 0xC1 }, { 0x0412, 0xC2 }
          , { 0x0413, 0xC3 }, { 0x0414, 0xC4 }, { 0x0415, 0xC5 }, { 0x0416, 0xC6 }
          , { 0x0417, 0xC7 }, { 0x0418, 0xC8 }, { 0x0419, 0xC9 }, { 0x041A, 0xCA }
          , { 0x041B, 0xCB }, { 0x041C, 0xCC }, { 0x041D, 0xCD }, { 0x041E, 0xCE }
          , { 0x041F, 0xCF }, { 0x0420, 0xD0 }, { 0x0421, 0xD1 }, { 0x0422, 0xD2 }
          , { 0x0423, 0xD3 }, { 0x0424, 0xD4 }, { 0x0425, 0xD5 }, { 0x0426, 0xD6 }
          , { 0x0427, 0xD7 }, { 0x0428, 0xD8 }, { 0x0429, 0xD9 }, { 0x042A, 0xDA }
          , { 0x042B, 0xDB }, { 0x042C, 0xDC }, { 0x042D, 0xDD }, { 0x042E, 0xDE }
          , { 0x042F, 0xDF }, { 0x0430, 0xE0 }, { 0x0431, 0xE1 }, { 0x0432, 0xE2 }
          , { 0x0433, 0xE3 }, { 0x0434, 0xE4 }, { 0x0435, 0xE5 }, { 0x0436, 0xE6 }
          , { 0x0437, 0xE7 }, { 0x0438, 0xE8 }, { 0x0439, 0xE9 }, { 0x043A, 0xEA }
          , { 0x043B, 0xEB }, { 0x043C, 0xEC }, { 0x043D, 0xED }, { 0x043E, 0xEE }
          , { 0x043F, 0xEF }, { 0x0440, 0xF0 }, { 0x0441, 0xF1 }, { 0x0442, 0xF2 }
          , { 0x0443, 0xF3 }, { 0x0444, 0xF4 }, { 0x0445, 0xF5 }, { 0x0446, 0xF6 }
          , { 0x0447, 0xF7 }, { 0x0448, 0xF8 }, { 0x0449, 0xF9 }, { 0x044A, 0xFA }
          , { 0x044B, 0xFB }, { 0x044C, 0xFC }, { 0x044D, 0xFD }, { 0x044E, 0xFE }
          , { 0x044F, 0xFF }, { 0x0451, 0xB8 }, { 0x0452, 0x90 }, { 0x0453, 0x83 }
          , { 0x0454, 0xBA }, { 0x0455, 0xBE }, { 0x0456, 0xB3 }, { 0x0457, 0xBF }
          , { 0x0458, 0xBC }, { 0x0459, 0x9A }, { 0x045A, 0x9C }, { 0x045B, 0x9E }
          , { 0x045C, 0x9D }, { 0x045E, 0xA2 }, { 0x045F, 0x9F }, { 0x0490, 0xA5 }
          , { 0x0491, 0xB4 }, { 0x2013, 0x96 }, { 0x2014, 0x97 }, { 0x2018, 0x91 }
          , { 0x2019, 0x92 }, { 0x201A, 0x82 }, { 0x201C, 0x93 }, { 0x201D, 0x94 }
          , { 0x201E, 0x84 }, { 0x2020, 0x86 }, { 0x2021, 0x87 }, { 0x2022, 0x95 }
          , { 0x2026, 0x85 }, { 0x2030, 0x89 }, { 0x2039, 0x8B }, { 0x203A, 0x9B }
          , { 0x20AC, 0x88 }, { 0x2116, 0xB9 }, { 0x2122, 0x99 }
        };
        constexpr tCIDLib::TCard1 c1RepChar = 26;
        constexpr tCIDLib::TCard4 c4ToMapSize = tCIDLib::c4ArrayElems(acmToMap);
    }
}




// ---------------------------------------------------------------------------
//  TWin1251Converter: Constructors and Destructor
// ---------------------------------------------------------------------------
TWin1251Converter::TWin1251Converter() :

    TMode1EncodingBase
    (
        L"CP1251"
        , CIDEncode_Win1251::achFromMap
        , CIDEncode_Win1251::acmToMap
        , CIDEncode_Win1251::c4ToMapSize
    )
{
}

