//
// FILE NAME: CIDCtrls_ImageList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/27/2015
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
//  This file implements the image list object.
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TImageList, TObject)




// ---------------------------------------------------------------------------
//  CLASS: TImageList
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TImageList: Constructors and Destructor
// ---------------------------------------------------------------------------
TImageList::TImageList() :

    m_himglThis(0)
{
}

TImageList::~TImageList()
{
    // If we created it, the close it
    if (m_himglThis)
    {
        ::ImageList_Destroy((HIMAGELIST)m_himglThis);
        m_himglThis = 0;
    }
}


// ---------------------------------------------------------------------------
// TImageList: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add an image. Has to be of the rightsize
tCIDLib::TCard4 TImageList::c4AddImage(const TBitmap& bmpToAdd)
{
    if (bmpToAdd.szBitmap() != m_szImages)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcImgList_WrongSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    int iRes = ::ImageList_Add
    (
        (HIMAGELIST)m_himglThis, TBitmap::hbmpDupHandle(bmpToAdd.hbmpThis()), 0
    );

    if (iRes == -1)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcImgList_AddFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    return tCIDLib::TCard4(iRes);
}


// Create this image list for images of the indicated size
tCIDLib::TVoid
TImageList::Create( const   TSize&          szImages
                    , const tCIDLib::TCard4 c4InitCount
                    , const tCIDLib::TCard4 c4MaxCount)
{
    // Can't already exist
    if (m_himglThis)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_AlreadyCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , TString(L"image list")
        );
    }

    tCIDLib::TCard4 c4Extra = 0;
    if (c4MaxCount > c4InitCount)
        c4Extra = c4MaxCount - c4InitCount;

    // Looks OK, so try to create it
    m_himglThis = ::ImageList_Create
    (
        szImages.c4Width()
        , szImages.c4Height()
        , ILC_COLOR32
        , c4InitCount
        , c4Extra
    );

    if (!m_himglThis)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_CreateFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TString(L"image list")
        );
    }

    // Save the image size
    m_szImages = szImages;
}


// Provide access to the size of the images we hold
const TSize& TImageList::szImages() const
{
    return m_szImages;
}

