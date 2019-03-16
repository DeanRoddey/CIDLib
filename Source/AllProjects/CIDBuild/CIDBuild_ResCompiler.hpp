//
// FILE NAME: CIDBuild_ResCompiler.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This is the header for the CIDBuild_ResCompiler.Cpp file. This file
//  implements the TResCompiler class, which handles compilation of the
//  program's message files and resource files into binary output files and
//  headers.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResCompiler
//  PREFIX: msgp
// ---------------------------------------------------------------------------
class TResCompiler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Operators
        // -------------------------------------------------------------------
        TResCompiler
        (
            const   TProjectInfo&           projiSrc
            , const tCIDLib::TBoolean       bVerbose
            , const tCIDLib::TBoolean       bForce
        );

        ~TResCompiler();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcess();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TResCompiler(const TResCompiler&);
        tCIDLib::TVoid operator=(const TResCompiler&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods. These are distributed among a few
        //  cpp files that implement the various parts.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindMsgEntryParts
        (
                    TBldStr&                strSrc
            ,       TBldStr&                strName
            ,       TBldStr&                strId
            ,       TBldStr&                strText
        );

        tCIDLib::TBoolean bFindMsgSymbol
        (
            const   tCIDLib::TCh* const     pszSymName
            ,       tCIDLib::TMsgId&        midToFill
        )   const;

        tCIDLib::TBoolean bFindSpecialMsgSymbol
        (
            const   tCIDLib::TCh* const     pszSymName
            ,       tCIDLib::TMsgId&        midToFill
        )   const;

        TIdInfo* pidiProcessMsg
        (
                    TBldStr&                strFirstLine
            , const TIdInfo::ETypes         eMsgType
        );

        tCIDLib::TBoolean bMassageMsgText
        (
            const   TBldStr&                strOriginal
            ,       TBldStr&                strNewToFill
        );

        tCIDLib::TBoolean bProcessMsgs();

        tCIDLib::TBoolean bScanForUpdates
        (
            const   TFindInfo&              fndiTarget
            , const tCIDLib::TBoolean       bCheckDates
        );

        tCIDLib::TCard4 c4SplitFields
        (
            const   TBldStr&                strSrc
            ,       TBldStr                 strFlds[]
            , const tCIDLib::TCard4         c4MaxFlds
            , const tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid LoadCommonText();

        tCIDLib::TCard1* pc1ParseMenuItemFlds
        (
                    tCIDLib::TCard1* const  pc1Start
            ,       TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
        );

        tCIDLib::TCard1* pc1ParseSubMenu
        (
                    tCIDCtrls::TRawMenuDesc& rmdToFill
            , const tCIDLib::TCard1* const  pc1End
            ,       tCIDLib::TCard1* const  pc1Cur
            ,       TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
        );

        tCIDLib::TVoid OutputBinMsgData
        (
            const   TBldStr&                strPath
            ,       TList<TIdInfo>&         listToWrite
        );

        tCIDLib::TVoid ProcessMsgCtrl();

        tCIDLib::TVoid ProcessResCtrl
        (
                    TTextFile&              tflResHpp
        );

        tCIDLib::TVoid ProcessDialog
        (
                    TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
        );

        tCIDLib::TVoid ProcessDialogWidget
        (
            const   TBldStr&                strDlgName
            , const TBldStr&                strStartText
            ,       tCIDCtrls::TRawDlgItem& rdlgiToFill
            ,       TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
            ,       TBldStr&                strHints
            ,       TBldStr&                strImage
        );

        tCIDLib::TVoid ProcessImages
        (
            const   tCIDLib::TCh* const     pszEndText
            , const tCIDLib::EResTypes      eType
            ,       TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
        );

        tCIDLib::TVoid ProcessMenu
        (
                    TBinFile&               bflBinOut
            ,       TTextFile&              tflResHpp
        );

        tCIDLib::TVoid ProcessMsgFile
        (
                    TList<TIdInfo>&         listToFill
        );

        tCIDLib::TVoid ProcessResources
        (
            const   tCIDLib::TBoolean       bMsgsBuilt
        );

        tCIDLib::TVoid WriteMsgHeaders();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bForce
        //      This is passed t us, and tells us if we should force a build
        //      of resources even if nothing is out of date.
        //
        //  m_bGenHeaders
        //      There are times when we have to rebuild the output binary
        //      file, but not necessarily regen the headers. Its important
        //      to avoid if possible, since it causes rebuilds. So this flag
        //      is set during initial dependency checks for both the messages
        //      and resources, and is used during the build of each to know
        //      if header output should be done.
        //
        //  m_bVerbose
        //      This is passed to us, and tells us if we are to be verbose
        //      in our output.
        //
        //  m_listMsgIds
        //      This list is loaded during the scan of the message text file,
        //      and holds all of the message text info. Its used both to
        //      generate the loadable text output files, and sometimes during
        //      the GUI resource processing, if one of the GUI resources
        //      refers to a message text symbol.
        //
        //  m_plsplInput
        //      The line spooler for the input file. It can be either ASCII
        //      or Unicode, but the line spooler will convert it all to
        //      Unicode on the way in.
        //
        //  m_projiSrc
        //      The project info object for the project that we are building
        //      for. We just store a reference to it.
        //
        //  m_reshOut
        //      This is the resource file header that we have to write out
        //      at the end. Some info is accumulated and put into here so
        //      we just use the structure itself to accumulate, then fill in
        //      the rest and write it out.
        //
        //  m_strXXXNamespace
        //      These are the namespaces generated into the message, error,
        //      and resource id headers, if they are generated. We default
        //      them initially based on the project name, but they can be
        //      overridden in the CTRL= block that must be first in the .CIDRC
        //      and .MsgText files.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bForce;
        tCIDLib::TBoolean       m_bGenHeaders;
        tCIDLib::TBoolean       m_bVerbose;
        TList<TIdInfo>          m_listMsgIds;
        TLineSpooler*           m_plsplInput;
        const TProjectInfo&     m_projiSrc;
        tCIDLib::TResHeader     m_reshOut;
        TBldStr                 m_strErrNamespace;
        TBldStr                 m_strMsgNamespace;
        TBldStr                 m_strResNamespace;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  s_bLoadedCommIds
        //  s_listCommonMsgIds
        //      There is a special message text file that contains just a set
        //      of common message ids that can be used in dialog definitions
        //      in any facility. So we always parse and load up those ids.
        //      The bFindSpecialMsgId() method above is used to search this
        //      list.
        //
        //      The boolean is used to fault in the list and then know we
        //      don't need to do it anymore.
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean    s_bLoadedCommIds;
        static TList<TIdInfo>       s_listCommonMsgIds;
};

