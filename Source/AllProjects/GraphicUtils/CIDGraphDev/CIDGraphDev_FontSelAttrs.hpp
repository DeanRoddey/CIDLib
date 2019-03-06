//
// FILE NAME: CIDGraphDev_FontSelAttrs.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/02/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDGraphDev_FontSelAttrs.cpp file. This file
//  implements the TFontSelAttrs class, which is used to indicate the
//  attributes for a font that you want to create.
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
//  CLASS: TFontSelAttrs
// PREFIX: fsel
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TFontSelAttrs :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDGraphDev::EFontWeights eWeightFromName
        (
            const   tCIDLib::TCh* const         pszName
        );

        static tCIDGraphDev::EFontWeights eWeightFromName
        (
            const   TString&                    strName
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFontSelAttrs();

        TFontSelAttrs
        (
            const   TGraphDrawDev&              gdevSource
        );

        TFontSelAttrs
        (
            const   tagLOGFONTW&                HostInfo
        );

        TFontSelAttrs
        (
            const   TFontSelAttrs&              fselToCopy
        );

        ~TFontSelAttrs();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFontSelAttrs& operator=
        (
            const   TFontSelAttrs&              fselToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustWeight
        (
            const   tCIDLib::TBoolean           bIncrement
        );

        tCIDLib::TBoolean bDevicePreferred() const;

        tCIDLib::TBoolean bDevicePreferred
        (
            const   tCIDLib::TBoolean           bNewState
        );

        tCIDLib::TBoolean bItalic() const;

        tCIDLib::TBoolean bItalic
        (
            const   tCIDLib::TBoolean           bNewState
        );

        tCIDLib::TBoolean bScalable() const;

        tCIDLib::TBoolean bScalable
        (
            const   tCIDLib::TBoolean           bNewState
        );

        tCIDLib::TBoolean bStruckOut() const;

        tCIDLib::TBoolean bStruckOut
        (
            const   tCIDLib::TBoolean           bNewState
        );

        tCIDLib::TBoolean bUnderlined() const;

        tCIDLib::TBoolean bUnderlined
        (
            const   tCIDLib::TBoolean           bNewState
        );

        tCIDLib::TCard4 c4Height() const;

        tCIDLib::TCard4 c4Height
        (
            const   tCIDLib::TCard4             c4NewHeight
            , const tCIDGraphDev::EFontHeights  eNewHeight = tCIDGraphDev::EFontHeights::EM

        );

        tCIDLib::TCard4 c4Width() const;

        tCIDLib::TCard4 c4Width
        (
            const   tCIDLib::TCard4             c4NewWidth
        );

        tCIDGraphDev::EFontDirs eDir() const;

        tCIDGraphDev::EFontDirs eDir
        (
            const   tCIDGraphDev::EFontDirs     eToSet
        );

        tCIDGraphDev::EFontFamilies eFamily() const;

        tCIDGraphDev::EFontFamilies eFamily
        (
            const   tCIDGraphDev::EFontFamilies eToSet
        );

        tCIDGraphDev::EFontHeights eHeightMapping() const;

        tCIDGraphDev::EFontPitches ePitch() const;

        tCIDGraphDev::EFontPitches ePitch
        (
            const   tCIDGraphDev::EFontPitches  eToSet
        );

        tCIDGraphDev::EFontWeights eWeight() const;

        tCIDGraphDev::EFontWeights eWeight
        (
            const   tCIDGraphDev::EFontWeights  eNewWeight
        );

        TString strFaceName() const;

        TString strFaceName
        (
            const   TString&                    strNewFace
        );

        tCIDLib::TVoid SetHeightAsPoints
        (
            const   TGraphDrawDev&              gdevTarget
            , const tCIDLib::TCard4             c4Points
        );

        // For internal use!
        const tagLOGFONTW& HostInfo() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&               strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&              strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pHostInfo
        //      This is the host font selection structure. We use it for
        //      convenience, but the outside world cannot see it.
        // -------------------------------------------------------------------
        tagLOGFONTW*    m_pHostInfo;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFontSelAttrs,TObject)
        DefPolyDup(TFontSelAttrs)
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TFontSelAttrs: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline const tagLOGFONTW& TFontSelAttrs::HostInfo() const
{
    return *m_pHostInfo;
}

