//
// FILE NAME: CIDIDL_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2000
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
//
// DESCRIPTION:
//
//  This is the header for the facility class for the IDL compiler.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDIDL
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDIDL : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDIDL();

        ~TFacCIDIDL();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid GenErr
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TErrCode       errcToThrow
        );

        tCIDLib::TVoid GenErr
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TErrCode       errcToThrow
            , const MFormattable&           fmtblToken1
        );

        tCIDLib::TVoid GenErr
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TErrCode       errcToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLoadMapping
        (
                    TString&                strMapping
        );

        tCIDLib::TBoolean bParseParams();

        tCIDLib::TBoolean bParseInput();

        tCIDLib::TVoid GenCode();

        tCIDLib::TVoid GenClient
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement* const  pxtnodeConsts
            , const TXMLTreeElement* const  pxtnodeTypes
            , const TXMLTreeElement* const  pxtnodeMethods
        );

        tCIDLib::TVoid GenConstants
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeConsts
        );

        tCIDLib::TVoid GenEnum
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeEnum
        );

        tCIDLib::TVoid GenGlobals
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeGlobals
            , const TString&                strOutDir
            , const TString&                strFacName
            , const TString&                strNameExt
            , const TString&                strExpKeyword
        );

        tCIDLib::TVoid GenId();

        tCIDLib::TVoid GenInterface
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeIntf
        );

        tCIDLib::TVoid GenServer
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement* const  pxtnodeConsts
            , const TXMLTreeElement* const  pxtnodeTypes
            , const TXMLTreeElement* const  pxtnodeCtors
            , const TXMLTreeElement* const  pxtnodeMethods
        );

        tCIDLib::TVoid GenStruct
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeEnum
        );

        tCIDLib::TVoid GenTypes
        (
                    TCodeGenerator&         cgenTarget
            , const TXMLTreeElement&        xtnodeTypes
        );

        tCIDLib::TVoid ShowUsage();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bGenXXX
        //      These are set to indicate what stuff to output
        //
        //  m_bNoLogo
        //      This is defaulted to false, but can be set via the /NoLogo
        //      command line parm.
        //
        //  m_bPrivHeader
        //      Indicates whether we should generate an include of the private
        //      or public facilty header into the client proxy cpp file. App
        //      facilities don't have a private header, but library type
        //      facilities do, and it has to be used in order to get pre-comp
        //      headers to work right. It defaults to false, and is set if the
        //      /PrivHdr command line option is provided.
        //
        //  m_eAction
        //      The action we are given on the command line. It controls what
        //      we output. Its set during the params parsing.
        //
        //  m_strClientBase
        //  m_strServerBase
        //      Usually, these are not provided in the source XML file, and
        //      they just default to the Orb client and server bases. But
        //      sometimes it is desirable to derive a set of Orb objects
        //      from a common base class.
        //
        //  m_strClientFacName
        //  m_strClientOutputDir
        //  m_strClientOptName
        //      These are the input parameters that control the C++ client
        //      side output. From the /Client= parameter.
        //
        //  m_strCExportKeyword
        //  m_strGExportKeyword
        //  m_strSExportKeyword
        //      They can provide an /CExport=, /GExport=, or /SExport
        //      parameter to give us the keyword we should generate into the
        //      headers.
        //
        //  m_strGlobalsDir
        //  m_strGlobalsFac
        //  m_strGlobalsName
        //      The info from the /Global= parameter, which provides info for
        //      the C++ globals stuff.
        //
        //  m_strInputFile
        //      This is the input XML file that describes the interface that
        //      we are going to generate. It comes from the /Input= command
        //      line parameter.
        //
        //  m_strInterfaceId
        //  m_strInterfaceName
        //      These are loaded up from the input XML file. They are used
        //      often enough that we want to get them stored away and use them
        //      from here.
        //
        //  m_strServerFacName
        //  m_strServerOutputDir
        //  m_strServerOptName
        //      These are the input parameters that control the C++ server
        //      side output. From the /Server= parameter.
        //
        //  m_xtprsInput
        //      The XML tree parser that we use to parse the input. We use
        //      the parse options to parse in just non-ignorable space and
        //      tags. So that is all that will be in under the root.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bGenClient;
        tCIDLib::TBoolean       m_bGenGlobals;
        tCIDLib::TBoolean       m_bGenServer;
        tCIDLib::TBoolean       m_bNoLogo;
        tCIDLib::TBoolean       m_bPrivHeader;
        tCIDIDL::EActions       m_eAction;
        TString                 m_strClientBase;
        TString                 m_strClientFacName;
        TString                 m_strClientOutputDir;
        TString                 m_strClientOptName;
        TString                 m_strCExportKeyword;
        TString                 m_strGExportKeyword;
        TString                 m_strGlobalsFacName;
        TString                 m_strGlobalsName;
        TString                 m_strGlobalsOutputDir;
        TString                 m_strSExportKeyword;
        TString                 m_strInputFile;
        TString                 m_strInterfaceId;
        TString                 m_strInterfaceName;
        TString                 m_strServerBase;
        TString                 m_strServerFacName;
        TString                 m_strServerOutputDir;
        TString                 m_strServerOptName;
        TXMLTreeParser          m_xtprsInput;


        // -------------------------------------------------------------------
        //  Private static data members
        //
        //  s_strEmbeddedDTDText
        //      This is the text for the DTD that we apply to IDL files. We
        //      use an embedded DTD so that there is no chance of getting the
        //      wrong one. This one is kind of big, so its defined in its own
        //      file, CIDIDL_DTDText.cpp.
        // -------------------------------------------------------------------
        static TString          s_strEmbeddedDTDText;
};

