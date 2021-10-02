// ----------------------------------------------------------------------------
//  FILE: CIDMData_Shared.hpp
//  DATE: Fri, Oct 01 19:41:28 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Constants namespace
// ----------------------------------------------------------------------------
namespace kCIDMData
{
    
    // ------------------------------------------------------------------------
    //  Attributes passed to the attribute editor window (in CIDWUtils)
    //  can have limit info provided, to help it do more validation
    //  generically. Some special values can be used that don't reprsent
    //  actual limits.
    //  
    //  [Required]  - For string values, no particular limits but it cannot be
    //                  empty. It will be limited to a single line.
    //  [SingleLine]- For string values, indicates it is limited to a single line
    //                  of text, else it's allowed to be multi-line.
    //  [SingleReq] - Is required and must be single line.
    //  Area:       - For Point types, limits the area where the point can
    //                  be. It's followed by an area in standard x,y,cx,cy form.
    //  Chars:      - For Char types. Indicates a list of valid characters, no
    //                  spaces or other whitespace unless those are valid chars.
    //  Enum:       - Followed by a comma separated list of values, of which
    //                  and empty value could be one, to allow for 'no selection'.
    //  FileSel:    - Followed by a file type description, another colon, then
    //                  a comma separated list of wild cards.
    //  Range:      - Followed by two comma separated numbers that define the
    //                  min and max values, only useful for Card/Int or
    //                  Card/Int ranges. For the latter indicates the max
    //                  range that the range has to be within.
    //  Size:       - Useful only for Size attributes. It is followed by two
    //                  sets of size values: cx,cy/cx,cy. The first is the
    //                  minimum size and the second is the max. Set the first
    //                  to zero if it doesn't matter.
    //  
    //                  
    // ------------------------------------------------------------------------
    CIDMDATAEXP const extern TString strAttrLim_Required;
    CIDMDATAEXP const extern TString strAttrLim_SingleLine;
    CIDMDATAEXP const extern TString strAttrLim_SingleReq;
    CIDMDATAEXP const extern TString strAttrLim_AreaPref;
    CIDMDATAEXP const extern TString strAttrLim_CharPref;
    CIDMDATAEXP const extern TString strAttrLim_EnumPref;
    CIDMDATAEXP const extern TString strAttrLim_FileSelPref;
    CIDMDATAEXP const extern TString strAttrLim_RangePref;
    CIDMDATAEXP const extern TString strAttrLim_SizePref;
    
    // ------------------------------------------------------------------------
    //  Special attribute values
    //                  
    // ------------------------------------------------------------------------
    CIDMDATAEXP const extern TString strAttrVal_False;
    CIDMDATAEXP const extern TString strAttrVal_AltFalse1;
    CIDMDATAEXP const extern TString strAttrVal_AltFalse2;
    CIDMDATAEXP const extern TString strAttrVal_True;
    CIDMDATAEXP const extern TString strAttrVal_AltTrue1;
    CIDMDATAEXP const extern TString strAttrVal_AltTrue2;
    
    // ------------------------------------------------------------------------
    //  The numeric constants for the flag byte that is in each chunk as stored
    //  on disk.
    //  
    //  DataChange - If it changes bump the serial number and last change
    //                  
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCard1 c1ChunkFlag_DataChange = 0x01;
    
    // ------------------------------------------------------------------------
    //  Some magic ids embedded in the chunked file format. We have magic
    //  start/end file markers, chunk start/end markers
    //                  
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCard4 c4ChunkFlId_FileEnd = 0x470FB9A9;
    constexpr tCIDLib::TCard4 c4ChunkFlId_FileStart = 0xA3957BE1;
    constexpr tCIDLib::TCard1 c1ChunkFlId_ChunkStart = 0xEA;
    constexpr tCIDLib::TCard1 c1ChunkFlId_ChunkEnd = 0xBC;
    
    // ------------------------------------------------------------------------
    //  The numeric constant ids of the defined chunks in the chunked file
    //  format.
    //                  
    // ------------------------------------------------------------------------
    CIDMDATAEXP const extern TString strChunkId_Data;
    CIDMDATAEXP const extern TString strChunkId_Meta;
    
    // ------------------------------------------------------------------------
    //  Some magic constants related to the overall chunked file format.
    //                  
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCard1 c1ChunkFl_FmtVersion = 1;
    constexpr tCIDLib::TCard4 c4ChunkFl_MaxMetaKeys = 48;
    
    // ------------------------------------------------------------------------
    //  The predefined meta chunk keys.
    //  
    //  Source -    The name of the original source it came from. It might
    //              be the original file system path, or some other sort
    //              of indicator of where it came from, such as a path into
    //              a data structure or whatever.
    //                  
    // ------------------------------------------------------------------------
    CIDMDATAEXP const extern TString strChFlMetaKey_Application;
    CIDMDATAEXP const extern TString strChFlMetaKey_Author;
    CIDMDATAEXP const extern TString strChFlMetaKey_FileName;
    CIDMDATAEXP const extern TString strChFlMetaKey_Notes;
    CIDMDATAEXP const extern TString strChFlMetaKey_Title;
    CIDMDATAEXP const extern TString strChFlMetaKey_Version;
    CIDMDATAEXP const extern TString strChFlMetaKey_StdPrefix;
}

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDMData
{
    
    // ------------------------------------------------------------------------
    //  The base types supported by the TAttrData class. They can define
    //  their own extended types, but these are the fundamental types that
    //  define the storage formats available.
    //                  
    // ------------------------------------------------------------------------
    enum class EAttrTypes
    {
        Area
        , AppImage
        , Binary
        , Bool
        , Card
        , Card64
        , CardRange
        , Char
        , File
        , Float
        , Int
        , IntRange
        , MLString
        , Point
        , RGBColor
        , String
        , Separator
        , Size
        , Time
        , Count
        , Min = Area
        , Max = Time
    };
    [[nodiscard]] CIDMDATAEXP EAttrTypes eXlatEAttrTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMDATAEXP const TString& strXlatEAttrTypes(const EAttrTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDMDATAEXP tCIDLib::TBoolean bIsValidEnum(const EAttrTypes eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDMData::EAttrTypes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDMData::EAttrTypes& eToFill)
    {
        eToFill = tCIDMData::EAttrTypes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDMDATAEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMData::EAttrTypes eToStream);
    
    // ------------------------------------------------------------------------
    //  This is used by the attribute data class, which is in turn used by the
    //  the atttribute editor in CIDWUtils, and indicates the ways in which a
    //  given attribute can be edited.
    //                  
    // ------------------------------------------------------------------------
    enum class EAttrEdTypes
    {
        None
        , InPlace
        , Visual
        , Both
        , Count
        , Min = None
        , Max = Both
    };
    [[nodiscard]] CIDMDATAEXP EAttrEdTypes eXlatEAttrEdTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMDATAEXP const TString& strXlatEAttrEdTypes(const EAttrEdTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDMDATAEXP tCIDLib::TBoolean bIsValidEnum(const EAttrEdTypes eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDMData::EAttrEdTypes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDMData::EAttrEdTypes& eToFill)
    {
        eToFill = tCIDMData::EAttrEdTypes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
}

#pragma CIDLIB_POPPACK

CIDMDATAEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMData::EAttrTypes* const aeList, const tCIDLib::TCard4 c4Count);
CIDMDATAEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMData::EAttrTypes* const aeList, const tCIDLib::TCard4 c4Count);

CIDMDATAEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMData::EAttrEdTypes* const aeList, const tCIDLib::TCard4 c4Count);
CIDMDATAEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMData::EAttrEdTypes* const aeList, const tCIDLib::TCard4 c4Count);

