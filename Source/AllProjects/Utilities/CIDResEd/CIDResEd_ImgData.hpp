//
// FILE NAME: CIDResEd_ImgData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDResEd_ImgData.cpp file, which implements
//  a set of classes that handle image resources. The main resource class
//  holds some lists of these for all of the defined images of each type.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResEdImgItem
//  PREFIX: reimgi
// ---------------------------------------------------------------------------
class TResEdImgItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TResEdImgItem&          reimgi1
            , const TResEdImgItem&          reimgi2
        );

        static tCIDLib::ESortComps eKeyComp
        (
            const   TString&                strKey
            , const TResEdImgItem&          reimgiComp
        );



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdImgItem();

        TResEdImgItem
        (
            const   TResEdImgItem&          reimgiToCopy
        );

        TResEdImgItem
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
        );

        TResEdImgItem
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
            , const TString&                strMsg
        );

        ~TResEdImgItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdImgItem& operator=
        (
            const   TResEdImgItem&          reimgiToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdImgItem&          reimgiToAssign
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdImgItem&          reimgiToAssign
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );

        const TString& strPath() const;

        const TString& strPath
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
            , const TString&                strPath
        );



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The numeric id of the image.
        //
        //  m_strName
        //      The symbolic name of the image.
        //
        //  m_strPath
        //      The path to the image file
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Id;
        TString         m_strName;
        TString         m_strPath;



        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdImgItem,TObject)
};


// Define a list of image items
namespace tCIDResEd
{
    typedef TVector<TResEdImgItem>      TImgItemList;
}

