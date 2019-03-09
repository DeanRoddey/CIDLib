//
// FILE NAME: CIDGraphDev_Icon.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Icon.cpp file. This file implements
//  the TIcon class, which encapsulates an icon in memory.
//
// CAVEATS/GOTCHAS:
//
//  1)  The actual icon handle is reference counted, so it is very practical
//      to pass icons around by value and to create copies of them.
//
//  2)  Because of this, be SURE that you make a 'deep copy' of an icon before
//      attempt to make either of its bitmaps a drawing surface (i.e before
//      modifying it.) Otherwise, you'll update everyone else who is referencing
//      the icon.
//
//  3)  The icon handle is available as a practical matter for those folks
//      who must interface with third party code. But, use it very carefully
//      because the handles are referenced counted and could go away if all
//      the TIcon objects referencing it go away. So make sure you keep a
//      TIcon object round while you are using the handle.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TIcon
//  PREFIX: ico
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TIcon : public TObject, public MDrawable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDGraphDev::TIconHandle hicoCreate
        (
            const   TFacility&                  facSrc
            , const tCIDLib::TResId             ridToCreate
            , const tCIDGraphDev::EIconSizes    eSize = tCIDGraphDev::EIconSizes::Normal
        );

        static tCIDGraphDev::TIconHandle hicoLoadRaw
        (
            const   TString&                    strLibName
            , const tCIDLib::TResId             ridToLoad
            , const TSize&                      szPreferred
            , const tCIDLib::TCard4             c4PrefDepth
        );

        static tCIDGraphDev::TIconHandle hicoLoadRaw
        (
            const   TString&                    strLibName
            , const TString&                    strToLoad
            , const TSize&                      szPreferred
            , const tCIDLib::TCard4             c4PrefDepth
        );

        static tCIDGraphDev::TIconHandle hicoLoadRaw
        (
            const   tCIDLib::TCh* const         pszLibName
            , const tCIDLib::TCh* const         pszId
            , const TSize&                      szPreferred
            , const tCIDLib::TCard4             c4PrefDepth
        );


        // -------------------------------------------------------------------
        //  Constuctors and destructor
        // -------------------------------------------------------------------
        TIcon();

        TIcon
        (
            const   tCIDGraphDev::TIconHandle   hicoToAdopt
        );

        TIcon
        (
            const   TFacility&                  facSrc
            , const tCIDLib::TResId             ridIcon
        );

        TIcon
        (
            const   TString&                    strIconFile
        );

        TIcon
        (
            const   TString&                    strIconFile
            , const TSize&                      szPreferredSz
            , const tCIDLib::TCard4             c4PrefDepth
        );

        TIcon
        (
            const   TPoint&                     pntHotspot
            , const TBitmap&                    bmpMask
            , const TBitmap&                    bmpColor
        );

        TIcon
        (
            const   TCIDImage&                  imgSrc
            , const TPoint&                     pntHotspot
        );

        TIcon
        (
            const   TIcon&                      icoSrc
        );

        ~TIcon();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIcon& operator=
        (
            const   TIcon&                      icoSrc
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&              gdevTarget
            , const TPoint&                     pntOrg
        )   const override;

        tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&              gdevTarget
            , const TArea&                      areaToFill
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TBitmap bmpImageCopy();

        TBitmap bmpMaskCopy();

        tCIDLib::TVoid DeepCopy();

        tCIDImage::EBitDepths eBitDepth() const;

        tCIDGraphDev::TIconHandle hicoThis() const;

        TIcon icoMakeGrayScale() const;

        TSize szBitmap() const;

        tCIDLib::TVoid Set
        (
            const   TFacility&                  facSource
            , const tCIDLib::TResId             ridIcon
        );


    private :
        // -------------------------------------------------------------------
        //   CLASS: TRefIconHandle
        //  PREFIX: rih
        // -------------------------------------------------------------------
        class TRefIconHandle
        {
            public :
                // -----------------------------------------------------------
                //  Ctors and Dtor
                // -----------------------------------------------------------
                TRefIconHandle();

                TRefIconHandle
                (
                    const   tCIDGraphDev::TIconHandle   hicoToAdopt
                );

                TRefIconHandle(const TRefIconHandle&) = delete;

                ~TRefIconHandle();


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TRefIconHandle& operator=(const TRefIconHandle&) = delete;


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDGraphDev::TIconHandle hicoThis() const;

                TSize szBitmap() const;


            private :
                // -----------------------------------------------------------
                //  Private non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TVoid UpdateInfo();


                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_c4Height
                //  m_c4Width.
                //      The size of the bitmap. This is often wanted and its
                //      a bit piggy to get, so we cache it.
                //
                //  m_hicoThis
                //      The icon handle we manage.
                // -----------------------------------------------------------
                tCIDLib::TCard4             m_c4Height;
                tCIDLib::TCard4             m_c4Width;
                tCIDGraphDev::TIconHandle   m_hicoThis;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrHandle
        //      This is the counted pointer that we used to manage the icon
        //      handle.
        // -------------------------------------------------------------------
        TCntPtr<TRefIconHandle> m_cptrHandle;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TIcon,TObject)
};

#pragma CIDLIB_POPPACK

