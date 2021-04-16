//
// FILE NAME: CIDCtrls_GUIFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2000
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
//  This is the header file for the CIDCtrls_GUIFacility.cpp file. This
//  file implements the TGUIFacility class, which is the base class for all
//  facility classes that need to support GUI resource loading and some other
//  GUI oriented stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TJPEGImage;
class TPNGImage;


// ---------------------------------------------------------------------------
//  CLASS: TGUIFacility
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGUIFacility : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TArea& areaPrimaryMonitor();

        static const TArea& areaVirtualScreen();

        static tCIDLib::TVoid AdjustDlgUnits
        (
                    TArea&                  areaToAdjust
        );

        static tCIDLib::TVoid AdjustDlgUnits
        (
                    TSize&                  szToAdjust
        );

        static tCIDLib::TVoid AdjustDlgUnits
        (
                    TPoint&                 pntToAdjust
        );

        static tCIDLib::TCard4 c4AdjustHDlgUnit
        (
            const   tCIDLib::TCard4         c4ToAdjust
        );

        static tCIDLib::TCard4 c4AdjustVDlgUnit
        (
            const   tCIDLib::TCard4         c4ToAdjust
        );

        static tCIDLib::TVoid AdjustWndUnits
        (
                    TArea&                  areaToAdjust
        );

        static tCIDLib::TVoid AdjustWndUnits
        (
                    TSize&                  szToAdjust
        );

        static tCIDLib::TVoid AdjustWndUnits
        (
                    TPoint&                 pntToAdjust
        );

        static tCIDLib::TCard4 c4DefFontHeight();

        static tCIDLib::TCard4 c4HorzDlgUnit();

        static tCIDLib::TCard4 c4VertDlgUnit();

        static const TGUIFont& gfontDefBold();

        static const TGUIFont& gfontDefFixed();

        static const TGUIFont& gfontDefLight();

        static const TGUIFont& gfontDefNorm();

        static const TGUIFont& gfontDefSmall();

        static const TGUIFont& gfontDefSmallLight();

        static const TGUIFont& gfontDefTitle();

        static tCIDLib::TInt4 i4AdjustHDlgUnit
        (
            const   tCIDLib::TInt4          i4ToAdjust
        );

        static tCIDLib::TInt4 i4AdjustHWndUnit
        (
            const   tCIDLib::TInt4          i4ToAdjust
        );

        static tCIDLib::TInt4 i4AdjustVDlgUnit
        (
            const   tCIDLib::TInt4          i4ToAdjust
        );

        static tCIDLib::TInt4 i4AdjustVWndUnit
        (
            const   tCIDLib::TInt4          i4ToAdjust
        );

        static TSize szDefIcon();

        static const TSize& szPrimaryMonitor();

        static const TSize& szVirtualScreen();

        static const TString& strDefFontFace();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIFacility
        (
            const   TString&                strFacName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCard4         c4Revision
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TGUIFacility
        (
            const   TString&                strFacName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCard4         c4Revision
            , const tCIDLib::EModFlags      eFlags
            , const TString&                strImageDir
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        ~TGUIFacility();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAppImageExists
        (
            const   TString&                strName
            , const tCIDCtrls::EAppImgSz    eSize
        )   const;

        tCIDLib::TBoolean bCreateDlgDesc
        (
            const   tCIDLib::TResId         ridToCreate
            ,       TDlgDesc&               dlgdToFill
        )   const;

        TBitmap bmpLoadAppImg
        (
            const   TString&                strName
            , const tCIDCtrls::EAppImgSz    eSize = tCIDCtrls::EAppImgSz::Default
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TBitmap bmpLoadLargeAppImg
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TBitmap bmpLoadResourceImg
        (
            const   tCIDLib::TResId         ridToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TBitmap bmpLoadSmallAppImg
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TBitmap bmpLoadSysBmp
        (
            const   tCIDCtrls::ESysBmps     eToLoad
        );

        TIcon icoLoadAppIcon
        (
            const   TString&                strName
            , const tCIDCtrls::EAppImgSz    eSize = tCIDCtrls::EAppImgSz::Default
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TIcon icoLoadSmallAppIcon
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        TCIDImage& imgLoadAppImg
        (
            const   TString&                strName
            , const tCIDCtrls::EAppImgSz    eSize = tCIDCtrls::EAppImgSz::Default
        );

        const tCIDCtrls::TRawMenuDesc* prmdLoad
        (
            const   tCIDLib::TResId         ridToLoad
        )   const;

        tCIDLib::TVoid SetFrameIcon
        (
                    TWindow&                wndTar
            , const TString&                strName
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TGUIFacility(const TGUIFacility&);
        tCIDLib::TVoid operator=(const TGUIFacility&);


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid FaultInMetrics();


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLoadRawAppImg
        (
            const   TString&                strRawName
            ,       TPNGImage&              imgToLoad
        );

        tCIDLib::TVoid MakeAppImgName
        (
            const   TString&                strName
            , const tCIDCtrls::EAppImgSz    eSize
            ,       TPathStr&               pathToFill
            ,       TString&                strCacheName
        )   const;

        TCIDImage* pimgLoadRawAppImg
        (
            const   TString&                strRawName
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strImageDir
        //      Indicates a subdirectory (under the standard AppImages dir) for this
        //      facility's app images. The standard ones from CIDCtrls are directly in
        //      AppImages, but other systems should make their own subdirectory for
        //      their images, to avoid clashes.
        // -------------------------------------------------------------------
        TString             m_strImageDir;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_areaPrimaryMon
        //      The coordinates of the primary monitor, which may not be at 0,0 if this is
        //      a multi-monitor system.
        //
        //  s_areaVirtualScr
        //      The coordinates of the virtual screen (combination of the areas of all
        //      of the monitors.)
        //
        //  s_c4DefFontHeight
        //      The EM height of our default font.
        //
        //  s_c4HorzDlgUnits
        //  s_c4VertDlgUnits
        //      These are used for mapping between pixels and dialog units. They
        //      are based on the font in m_pgfontDefault.
        //
        //  s_pgfontXXX
        //      These are our default fonts that we set up all of our windows to use,
        //      both custom and standard ones we wrap. We have a normal, bold, light, and
        //      title version, plus a small one for small print type stuff, and a
        //      standard sized, fixed pitch one.
        //
        //      Our dialog/window coordinate conversions will insure that anything 8
        //      dialog units high will fit the normal font, if it's positioned correctly
        //      within that area (i.e. doesn't include inter-line spacing and such. Title
        //      is 2 points larger and small is 2 points smaller.
        //
        //  m_strXXXFontFace
        //      The face names for our default fonts.
        //
        //  s_szPrimaryMon
        //      The size of the primary monitor
        //
        //  m_szVirtualScr
        //      The size of the virtual screen (the size of the s_areaVirtualScr member
        //      above.)
        // -------------------------------------------------------------------
        static TArea                s_areaPrimaryMon;
        static TArea                s_areaVirtualScr;
        static tCIDLib::TCard4      s_c4DefFontHeight;
        static tCIDLib::TCard4      s_c4HorzDlgUnits;
        static tCIDLib::TCard4      s_c4VertDlgUnits;
        static TGUIFont*            s_pgfontDefBold;
        static TGUIFont*            s_pgfontDefFixed;
        static TGUIFont*            s_pgfontDefLight;
        static TGUIFont*            s_pgfontDefNorm;
        static TGUIFont*            s_pgfontDefSmall;
        static TGUIFont*            s_pgfontDefSmallLight;
        static TGUIFont*            s_pgfontDefTitle;
        static TString              s_strBoldFontFace;
        static TString              s_strDefFontFace;
        static TString              s_strLightFontFace;
        static TSize                s_szPrimaryMon;
        static TSize                s_szVirtualScr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIFacility,TFacility)
};

#pragma CIDLIB_POPPACK


