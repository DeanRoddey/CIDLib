// ----------------------------------------------------------------------------
//  FILE: CIDGraphDev_Shared.hpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDGraphDev
{
    
    // ------------------------------------------------------------------------
    //  The available background mixing modes
    //                  
    // ------------------------------------------------------------------------
    enum class EBackMixModes
    {
        Opaque
        , Transparent
        , Count
        , Min = Opaque
        , Max = Transparent
    };
    [[nodiscard]] CIDGRDEVEXP EBackMixModes eXlatEBackMixModes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEBackMixModes(const EBackMixModes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEBackMixModes
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EBackMixModes eVal);

    inline tCIDGraphDev::EBackMixModes operator++(CIOP tCIDGraphDev::EBackMixModes& eVal, int)
    {
        const tCIDGraphDev::EBackMixModes eTmp = eVal;
        eVal = tCIDGraphDev::EBackMixModes(int(eVal)+1);
        return eTmp;
    }

    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EBackMixModes eToStream);
    
    // ------------------------------------------------------------------------
    //  The facility class' eCalcPlacement uses this. It returns the type
    //  of blit that will be needed to do the calculated placement.
    //                  
    // ------------------------------------------------------------------------
    enum class EBltTypes
    {
        None
        , Clip
        , Stretch
        , Both
        , Tile
        , Count
        , Min = None
        , Max = Tile
    };
    [[nodiscard]] CIDGRDEVEXP EBltTypes eXlatEBltTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEBltTypes(const EBltTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEBltTypes
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EBltTypes eVal);

    inline tCIDGraphDev::EBltTypes operator++(CIOP tCIDGraphDev::EBltTypes& eVal, int)
    {
        const tCIDGraphDev::EBltTypes eTmp = eVal;
        eVal = tCIDGraphDev::EBltTypes(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EBltTypes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EBltTypes& eToFill)
    {
        eToFill = tCIDGraphDev::EBltTypes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    
    // ------------------------------------------------------------------------
    //  The available bitmap raster modes
    //                  
    // ------------------------------------------------------------------------
    enum class EBmpModes
    {
        Blackness
        , CaptureBlt
        , DstInvert
        , MergeCopy
        , MergePaint
        , NoMirrorBitmap
        , NotSrcCopy
        , NotSrcErase
        , PatCopy
        , PatInvert
        , PatPaint
        , SrcAnd
        , SrcCopy
        , SrcErase
        , SrcInvert
        , SrcPaint
        , Whiteness
        , Count
        , Min = Blackness
        , Max = Whiteness
    };
    [[nodiscard]] CIDGRDEVEXP EBmpModes eXlatEBmpModes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEBmpModes(const EBmpModes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EBmpModes eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EBmpModes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EBmpModes& eToFill)
    {
        eToFill = tCIDGraphDev::EBmpModes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    
    // ------------------------------------------------------------------------
    //  The text drawing directions we allow, which are basically 0, 90.
    //  180, and 270 degrees.
    //                  
    // ------------------------------------------------------------------------
    enum class EFontDirs
    {
        D0
        , D90
        , D180
        , D270
        , Count
        , Min = D0
        , Max = D270
    };
    [[nodiscard]] CIDGRDEVEXP EFontDirs eXlatEFontDirs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEFontDirs(const EFontDirs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEFontDirs
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::AltText
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EFontDirs eVal);

    inline tCIDGraphDev::EFontDirs operator++(CIOP tCIDGraphDev::EFontDirs& eVal, int)
    {
        const tCIDGraphDev::EFontDirs eTmp = eVal;
        eVal = tCIDGraphDev::EFontDirs(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EFontDirs eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EFontDirs& eToFill)
    {
        eToFill = tCIDGraphDev::EFontDirs(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EFontDirs eToStream);
    
    // ------------------------------------------------------------------------
    //  The available font weights
    //                  
    // ------------------------------------------------------------------------
    enum class EFontWeights
    {
        DontCare
        , Thin
        , ExtraLight
        , Light
        , Normal
        , Medium
        , SemiBold
        , Bold
        , ExtraBold
        , Heavy
        , Count
        , Min = DontCare
        , Max = Heavy
    };
    [[nodiscard]] CIDGRDEVEXP EFontWeights eXlatEFontWeights(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEFontWeights(const EFontWeights eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP const TString& strAltXlatEFontWeights(const EFontWeights eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP EFontWeights eAltXlatEFontWeights(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    CIDGRDEVEXP tCIDLib::TVoid FormatEFontWeights
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::Text
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EFontWeights eVal);

    inline tCIDGraphDev::EFontWeights operator++(CIOP tCIDGraphDev::EFontWeights& eVal, int)
    {
        const tCIDGraphDev::EFontWeights eTmp = eVal;
        eVal = tCIDGraphDev::EFontWeights(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EFontWeights eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EFontWeights& eToFill)
    {
        eToFill = tCIDGraphDev::EFontWeights(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EFontWeights eToStream);
    
    // ------------------------------------------------------------------------
    //  The available gradient fill directions
    //                  
    // ------------------------------------------------------------------------
    enum class EGradDirs
    {
        Horizontal
        , Vertical
        , UpDiagonal
        , DownDiagonal
        , Count
        , Min = Horizontal
        , Max = DownDiagonal
    };
    [[nodiscard]] CIDGRDEVEXP EGradDirs eXlatEGradDirs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEGradDirs(const EGradDirs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEGradDirs
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EGradDirs eVal);

    inline tCIDGraphDev::EGradDirs operator++(CIOP tCIDGraphDev::EGradDirs& eVal, int)
    {
        const tCIDGraphDev::EGradDirs eTmp = eVal;
        eVal = tCIDGraphDev::EGradDirs(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EGradDirs eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EGradDirs& eToFill)
    {
        eToFill = tCIDGraphDev::EGradDirs(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EGradDirs eToStream);
    
    // ------------------------------------------------------------------------
    //  The available line end types. Controls how geometric lines that end
    //  at the same end point in a polygon will form a corner.
    //                  
    // ------------------------------------------------------------------------
    enum class ELineEnds
    {
        Flat
        , Square
        , Round
        , Count
        , Min = Flat
        , Max = Round
    };
    [[nodiscard]] CIDGRDEVEXP ELineEnds eXlatELineEnds(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatELineEnds(const ELineEnds eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatELineEnds
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ELineEnds eVal);

    inline tCIDGraphDev::ELineEnds operator++(CIOP tCIDGraphDev::ELineEnds& eVal, int)
    {
        const tCIDGraphDev::ELineEnds eTmp = eVal;
        eVal = tCIDGraphDev::ELineEnds(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ELineEnds eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ELineEnds& eToFill)
    {
        eToFill = tCIDGraphDev::ELineEnds(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::ELineEnds eToStream);
    
    // ------------------------------------------------------------------------
    //  The available ine join styles.
    //                  
    // ------------------------------------------------------------------------
    enum class ELineJoins
    {
        Bevel
        , Round
        , Mitre
        , Count
        , Min = Bevel
        , Max = Mitre
    };
    [[nodiscard]] CIDGRDEVEXP ELineJoins eXlatELineJoins(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatELineJoins(const ELineJoins eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatELineJoins
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ELineJoins eVal);

    inline tCIDGraphDev::ELineJoins operator++(CIOP tCIDGraphDev::ELineJoins& eVal, int)
    {
        const tCIDGraphDev::ELineJoins eTmp = eVal;
        eVal = tCIDGraphDev::ELineJoins(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ELineJoins eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ELineJoins& eToFill)
    {
        eToFill = tCIDGraphDev::ELineJoins(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::ELineJoins eToStream);
    
    // ------------------------------------------------------------------------
    //  The available line drawing styles, Controls whether the line is
    //  solid or uses one of a set of periodic patterns.
    //                  
    // ------------------------------------------------------------------------
    enum class ELineStyles
    {
        Solid
        , Dot
        , Dash
        , DashDot
        , DashDoubleDot
        , Invisible
        , Alternate
        , Count
        , Min = Solid
        , Max = Alternate
    };
    [[nodiscard]] CIDGRDEVEXP ELineStyles eXlatELineStyles(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatELineStyles(const ELineStyles eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatELineStyles
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ELineStyles eVal);

    inline tCIDGraphDev::ELineStyles operator++(CIOP tCIDGraphDev::ELineStyles& eVal, int)
    {
        const tCIDGraphDev::ELineStyles eTmp = eVal;
        eVal = tCIDGraphDev::ELineStyles(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ELineStyles eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ELineStyles& eToFill)
    {
        eToFill = tCIDGraphDev::ELineStyles(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::ELineStyles eToStream);
    
    // ------------------------------------------------------------------------
    //  The foreground mix modes
    //                  
    // ------------------------------------------------------------------------
    enum class EMixModes
    {
        Black
        , CopyPen
        , MaskNotPen
        , MaskPen
        , MaskPenNot
        , MergeNotPen
        , MergePen
        , MergePenNot
        , NOP
        , Not
        , NotCopyPen
        , NotMaskPen
        , NotMergePen
        , NotXorPen
        , White
        , XorPen
        , Count
        , Min = Black
        , Max = XorPen
    };
    [[nodiscard]] CIDGRDEVEXP EMixModes eXlatEMixModes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEMixModes(const EMixModes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEMixModes
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EMixModes eVal);

    inline tCIDGraphDev::EMixModes operator++(CIOP tCIDGraphDev::EMixModes& eVal, int)
    {
        const tCIDGraphDev::EMixModes eTmp = eVal;
        eVal = tCIDGraphDev::EMixModes(int(eVal)+1);
        return eTmp;
    }

    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EMixModes eToStream);
    
    // ------------------------------------------------------------------------
    //  The available built in brush fill patterns. Others can be created
    //  using bitmaps, but these are always available.
    //                  
    // ------------------------------------------------------------------------
    enum class EPatterns
    {
        Solid
        , BackDiagonal
        , Cross
        , DiagonalCross
        , ForwardDiagonal
        , Horizontal
        , Vertical
        , Count
        , Min = Solid
        , Max = Vertical
    };
    [[nodiscard]] CIDGRDEVEXP EPatterns eXlatEPatterns(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEPatterns(const EPatterns eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEPatterns
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EPatterns eVal);

    inline tCIDGraphDev::EPatterns operator++(CIOP tCIDGraphDev::EPatterns& eVal, int)
    {
        const tCIDGraphDev::EPatterns eTmp = eVal;
        eVal = tCIDGraphDev::EPatterns(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EPatterns eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EPatterns& eToFill)
    {
        eToFill = tCIDGraphDev::EPatterns(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EPatterns eToStream);
    
    // ------------------------------------------------------------------------
    //  The placement options of images within their target display area.
    //                  
    // ------------------------------------------------------------------------
    enum class EPlacement
    {
        Center
        , SizeToFit
        , Tile
        , UpperLeft
        , SizeIfTooBig
        , KeepAR
        , FitAR
        , Count
        , Min = Center
        , Max = FitAR
    };
    [[nodiscard]] CIDGRDEVEXP EPlacement eXlatEPlacement(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatEPlacement(const EPlacement eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDGRDEVEXP tCIDLib::TVoid FormatEPlacement
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const EPlacement eVal);

    inline tCIDGraphDev::EPlacement operator++(CIOP tCIDGraphDev::EPlacement& eVal, int)
    {
        const tCIDGraphDev::EPlacement eTmp = eVal;
        eVal = tCIDGraphDev::EPlacement(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::EPlacement eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::EPlacement& eToFill)
    {
        eToFill = tCIDGraphDev::EPlacement(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::EPlacement eToStream);
    
    // ------------------------------------------------------------------------
    //  The system bitmaps
    //  
    //  This enum must remain zero based and contiguous. If you change it, adjust the
    //  mapping array in CIDGraphDev_Bitmap.cpp!
    //                  
    // ------------------------------------------------------------------------
    enum class ESysBmps
    {
        None
        , Close
        , UpArrow
        , DownArrow
        , RightArrow
        , LeftArrow
        , Reduce
        , Zoom
        , Restore
        , Reduced
        , Zoomed
        , Restored
        , UpArrowD
        , DownArrowD
        , RightArrowD
        , LeftArrowD
        , MnArrow
        , Combo
        , UpArrowI
        , DownArrowI
        , RightArrowI
        , LeftArrowI
        , Size
        , BtSize
        , Check
        , Halftone1
        , Halftone2
        , Halftone3
        , Halftone4
        , Halftone5
        , Halftone6
        , Halftone7
        , Halftone8
        , Halftone9
        , Count
        , Min = None
        , Max = Halftone9
        , FirstSys = Close
        , LastSys = Check
    };
    [[nodiscard]] CIDGRDEVEXP const TString& strAltXlatESysBmps(const ESysBmps eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP ESysBmps eAltXlatESysBmps(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ESysBmps eVal);

    inline tCIDGraphDev::ESysBmps operator++(CIOP tCIDGraphDev::ESysBmps& eVal, int)
    {
        const tCIDGraphDev::ESysBmps eTmp = eVal;
        eVal = tCIDGraphDev::ESysBmps(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ESysBmps eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ESysBmps& eToFill)
    {
        eToFill = tCIDGraphDev::ESysBmps(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    
    // ------------------------------------------------------------------------
    //  The available system pointers
    //                  
    // ------------------------------------------------------------------------
    enum class ESysPtrs
    {
        AppStarting
        , Arrow
        , Cross
        , Hand
        , Help
        , IBeam
        , No
        , SizeAll
        , SizeNESW
        , SizeNS
        , SizeNWSE
        , SizeWE
        , UpArrow
        , Wait
        , Count
        , Min = AppStarting
        , Max = Wait
    };
    [[nodiscard]] CIDGRDEVEXP ESysPtrs eXlatESysPtrs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatESysPtrs(const ESysPtrs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ESysPtrs eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ESysPtrs eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ESysPtrs& eToFill)
    {
        eToFill = tCIDGraphDev::ESysPtrs(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::ESysPtrs eToStream);
    
    // ------------------------------------------------------------------------
    //  This enum is used by the caption interface and most widgets that use this
    //  interface. It indicates what special text effect to use when drawing the
    //  text.
    //  
    //  NOTE:   Add new ones at the END, because this enum is streamed out
    //          to disk in various places.
    //                  
    // ------------------------------------------------------------------------
    enum class ETextFX
    {
        None
        , Gradient
        , GradientRefl
        , Reflected
        , GaussianBlur
        , DropShadow
        , Count
        , Min = None
        , Max = DropShadow
    };
    [[nodiscard]] CIDGRDEVEXP ETextFX eXlatETextFX(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDGRDEVEXP const TString& strXlatETextFX(const ETextFX eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP const TString& strAltXlatETextFX(const ETextFX eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDGRDEVEXP ETextFX eAltXlatETextFX(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    CIDGRDEVEXP tCIDLib::TVoid FormatETextFX
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::Text
    );
    [[nodiscard]] CIDGRDEVEXP tCIDLib::TBoolean bIsValidEnum(const ETextFX eVal);

    inline tCIDGraphDev::ETextFX operator++(CIOP tCIDGraphDev::ETextFX& eVal, int)
    {
        const tCIDGraphDev::ETextFX eTmp = eVal;
        eVal = tCIDGraphDev::ETextFX(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDGraphDev::ETextFX eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDGraphDev::ETextFX& eToFill)
    {
        eToFill = tCIDGraphDev::ETextFX(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDGRDEVEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDGraphDev::ETextFX eToStream);
}

#pragma CIDLIB_POPPACK


CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EBltTypes* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EBltTypes* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EBmpModes* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EBmpModes* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EFontDirs* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EFontDirs* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EFontWeights* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EFontWeights* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EGradDirs* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EGradDirs* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ELineEnds* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ELineEnds* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ELineJoins* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ELineJoins* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ELineStyles* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ELineStyles* const aeList, const tCIDLib::TCard4 c4Count);


CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EPatterns* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EPatterns* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::EPlacement* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::EPlacement* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ESysBmps* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ESysBmps* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ESysPtrs* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ESysPtrs* const aeList, const tCIDLib::TCard4 c4Count);

CIDGRDEVEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDGraphDev::ETextFX* const aeList, const tCIDLib::TCard4 c4Count);
CIDGRDEVEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDGraphDev::ETextFX* const aeList, const tCIDLib::TCard4 c4Count);

