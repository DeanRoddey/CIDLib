//
// FILE NAME: CIDResEd_ResData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDResEd_ResData.cpp file, which implements
//  a set of classes that hold all the info that we parse from the resource
//  files, and which provides the interface to allow us to edit that info
//  and to write it back out.
//
//  This is just the main class. The data for each of the different data
//  types is defined in other headers. We conglomerate it all here.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TResEdData
//  PREFIX: redat
// ---------------------------------------------------------------------------
class TResEdData : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdData();

        TResEdData
        (
            const   TResEdData&             redatToCopy
        );

        ~TResEdData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdData& operator=
        (
            const   TResEdData&             redatToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdData&             redatToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdData&             redatToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetDialog
        (
            const   TResEdDlgDesc&          reddToSet
        );

        tCIDLib::TBoolean bValidate();

        tCIDLib::TCard4 c4AddDialog
        (
            const   TString&                strName
            ,       tCIDLib::TResId&        ridAssigned
        );

        const tCIDResEd::TDlgDescList& colDlgs() const;

        tCIDLib::TCard4 c4FindDlgIndex
        (
            const   TResEdDlgDesc&          reddFind
        )   const;

        tCIDLib::TCard4 c4FindDlgIndex
        (
            const   tCIDLib::TCard4         c4EditId
        )   const;

        const TResEdDlgDesc* preddByEditId
        (
            const   tCIDLib::TResId         ridFind
        )   const;

        const TResEdDlgDesc* preddByResId
        (
            const   tCIDLib::TResId         ridFind
            , const tCIDLib::TCard4         c4StartAt = 0
        )   const;

        const TResEdDlgDesc* preddByName
        (
            const   TString&                strFind
            , const tCIDLib::TCard4         c4StartAt = 0
        )   const;

        const TResEdDlgDesc& reddAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid ParseMsgText
        (
            const   TString&                strSrcFile
            , const tCIDLib::TBoolean       bCommon
        );

        tCIDLib::TVoid ParseResources
        (
            const   TString&                strSrcFile
        );

        const TResEdMsgList& remilByType
        (
            const   tCIDResEd::EMsgTypes    eType
        )   const;

        tCIDLib::TVoid Reset();

        const TString& strLoadResText
        (
            const   TTextSym&               tsymToLoad
        );

        tCIDLib::TVoid StoreMsgText
        (
            const   TResEdMsgList&          remilErrs
            , const TResEdMsgList&          remilMsgs
        );

        tCIDLib::TVoid WriteTo
        (
            const   TString&                strPath
            , const TString&                strName
            , const TResEdData&             redatPrev
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseImgs
        (
                    TResEdSpooler&          spoolSrc
            , const TString&                strEndText
            ,       tCIDResEd::TImgItemList& colToFill
        );

        tCIDLib::TVoid WriteDlgs
        (
                    TTextOutStream&         strmTar
        )   const;

        tCIDLib::TVoid WriteImgs
        (
                    TTextOutStream&         strmTar
            , const TString&                strType
            , const tCIDResEd::TImgItemList& colList
        )   const;

        tCIDLib::TVoid WriteMenus
        (
                    TTextOutStream&         strmTar
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colDlgs
        //      The list of dialog descriptions.
        //
        //  m_colBitmaps
        //  m_colIcons
        //  m_colJPEGs
        //  m_colPNGs
        //      The lists of various types of image resources we can have
        //
        //  m_colMenus
        //      A list of defined menus
        //
        //  m_remilCommon
        //  m_remilErrs
        //  m_remilMsgs
        //      The message lists for the error messages and regular messages.
        //      They may be empty if none are defined, in which case no files
        //      will be emitted. We also have a set of common messages that
        //      are not editable, and we never write it back out, but we have
        //      to parse them so that we can display the text.
        //
        //  m_strResNamespace
        //      The namespace that the user wants to have used when spitting
        //      out the resource ids header. We get this from the CTRL=
        //      block at the start of the header. If it's not indicated, we
        //      set this to the default.
        // -------------------------------------------------------------------
        tCIDResEd::TImgItemList m_colBitmaps;
        tCIDResEd::TDlgDescList m_colDlgs;
        tCIDResEd::TImgItemList m_colIcons;
        tCIDResEd::TImgItemList m_colJPEGs;
        tCIDResEd::TMenuList    m_colMenus;
        tCIDResEd::TImgItemList m_colPNGs;
        TResEdMsgList           m_remilCommon;
        TResEdMsgList           m_remilErrs;
        TResEdMsgList           m_remilMsgs;
        TString                 m_strResNamespace;
        TString                 m_strTmp;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdData,TObject)
};



