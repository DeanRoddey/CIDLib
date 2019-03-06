//
// FILE NAME: CIDGraphDev_FontMetrics.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a simple font metrics class. This guy is just for
//  moving around the group of attributes that defines a font, so its pretty
//  much just getters and setters and streaming stuff.
//
// CAVEATS/GOTCHAS:
//
//  1)  We must be able to construct from a host font info structure, then
//      be used to create exactly that font - which requires that we totally
//      recreate the original host font info. Because of this, we actually
//      store a copy of the host font structure, though we do not let it
//      be seen publically.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TGraphDrawDev;
struct  tagLOGFONTW;
struct  tagTEXTMETRICW;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TFontMetrics
// PREFIX: fmtr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TFontMetrics :

    public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4EnumAllFonts
        (
            const   TGraphDrawDev&              gdevTarget
            ,       TCollection<TFontMetrics>&  colToFill
            , const tCIDGraphDev::EFontTypes    eType
            , const tCIDGraphDev::EFontBases    eBase
            , const tCIDGraphDev::EFontPitches  ePitch
        );

        static tCIDLib::TCard4 c4EnumFontFace
        (
            const   TGraphDrawDev&              gdevTarget
            ,       TCollection<TFontMetrics>&  colToFill
            , const TString&                    strFace
            , const tCIDGraphDev::EFontTypes    eType
            , const tCIDGraphDev::EFontBases    eBase
            , const tCIDGraphDev::EFontPitches  ePitch
        );


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TFontMetrics();

        TFontMetrics
        (
            const   TGraphDrawDev&              gdevSrc
        );

        TFontMetrics
        (
            const   TFontMetrics&               fmtrToCopy
        );

        ~TFontMetrics();


        // Must be public, but for internal use only!!
        TFontMetrics
        (
            const   TString&                    strName
            , const tagTEXTMETRICW&             HostMetricInfo
        );



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFontMetrics& operator=
        (
            const   TFontMetrics&               fmtrToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TFontMetrics&               fmtrToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFontMetrics&               fmtrToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDeviceFont() const;

        tCIDLib::TBoolean bDeviceFont
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TBoolean bFixedPitch() const;

        tCIDLib::TBoolean bFixedPitch
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TBoolean bItalic() const;

        tCIDLib::TBoolean bItalic
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TBoolean bStruckOut() const;

        tCIDLib::TBoolean bStruckOut
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TBoolean bScalable() const;

        tCIDLib::TBoolean bScalable
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TBoolean bUnderlined() const;

        tCIDLib::TBoolean bUnderlined
        (
            const   tCIDLib::TBoolean           bState
        );

        tCIDLib::TCard4 c4Ascender() const;

        tCIDLib::TCard4 c4Ascender
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDLib::TCard4 c4AverageWidth() const;

        tCIDLib::TCard4 c4AverageWidth
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDLib::TCard4 c4Descender() const;

        tCIDLib::TCard4 c4Descender
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDLib::TCard4 c4ExternalLeading() const;

        tCIDLib::TCard4 c4ExternalLeading
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDLib::TCard4 c4Height() const;

        tCIDLib::TCard4 c4InternalLeading() const;

        tCIDLib::TCard4 c4InternalLeading
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDLib::TCard4 c4MaxWidth() const;

        tCIDLib::TCard4 c4MaxWidth
        (
            const   tCIDLib::TCard4             c4New
        );

        tCIDGraphDev::EFontWeights eWeight() const;

        tCIDGraphDev::EFontWeights eWeight
        (
            const   tCIDGraphDev::EFontWeights       eWeight
        );

        const TString& strFaceName() const;

        const TString& strFaceName
        (
            const   TString&                    strNewName
        );

        TSize szDevRes() const;

        TSize szDevRes
        (
            const   TSize&                      szNew
        );

        tCIDLib::TVoid UpdateFrom
        (
            const   TGraphDrawDev&              gdevSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ToLogFont
        (
                    tagLOGFONTW&                ToFill
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pHostInfo
        //      This is the host font info structure.
        //
        //  m_strFaceName
        //      The face name of the font, which is not part of the host
        //      info structure.
        // -------------------------------------------------------------------
        tagTEXTMETRICW*     m_pHostInfo;
        TString             m_strFaceName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFontMetrics,TObject)
        DefPolyDup(TFontMetrics)
        NulObject(TFontMetrics)
};

#pragma CIDLIB_POPPACK

