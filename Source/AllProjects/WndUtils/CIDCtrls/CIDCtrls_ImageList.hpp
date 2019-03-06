//
// FILE NAME: CIDCtrl_ImageList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/27/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This class implements the standard image list object. THis isn't a window, it
//  just contains images in a way that is used by a number of other controls.
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
//  CLASS: TImageList
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TImageList : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TImageList();

        TImageList(const TImageList&) = delete;

        ~TImageList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TImageList& operator=(const TImageList&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AddImage
        (
            const   TBitmap&                bmpToAdd
        );

        tCIDLib::TVoid Create
        (
            const   TSize&                  szImages
            , const tCIDLib::TCard4         c4InitCount
            , const tCIDLib::TCard4         c4MaxCount
        );

        const TSize& szImages() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare some friends
        // -------------------------------------------------------------------
        friend class TObjView;
        friend class TTabbedWnd;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_himglThis
        //      The handle to this image list we are managing.
        //
        //  m_szImages
        //      The size of the images we hold
        // -------------------------------------------------------------------
        tCIDCtrls::TImgListHandle   m_himglThis;
        TSize                       m_szImages;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TImageList, TObject)
};

#pragma CIDLIB_POPPACK


