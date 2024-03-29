//
// FILE NAME: XMLDemo1.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/99
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
//  This is the main header for the program which just brings in everything
//  that we need and which defines the facility class, on which the main thread
//  is started.
//
//  It gets a source file from the user, parses it, handles the events that
//  get spit out of the parser and outputs them to the console. So basically it
//  echoes the file back out but in one of a few different formats you can
//  indicate as a parameter.
//
//  Type the name with no parameters to see the parameters.
//
// CAVEATS/GOTCHAS:
//
//  1)  To keep things clean, all of the event handler methods are actually
//      implemented in XMLDemo1_EventHandler.Cpp instead of the main Cpp
//      file for this class.
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//  Bring in the underlying includes
// ---------------------------------------------------------------------------
#include "CIDXML.Hpp"


// ---------------------------------------------------------------------------
//   CLASS: TFacXMLDemo1
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacXMLDemo1 :

    public TFacility
    , public MXMLDocEvents
    , public MXMLDTDEvents
    , public MXMLEntityEvents
    , public MXMLErrorEvents
    , public MXMLEntityResolver
{
    public :
        // -------------------------------------------------------------------
        //  Class specific types
        // -------------------------------------------------------------------
        enum class EDisplayModes
        {
            None
            , Canonical
            , ErrLoc
            , ErrLoc2
            , Standard
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacXMLDemo1();

        TFacXMLDemo1(const TFacXMLDemo1&) = delete;
        TFacXMLDemo1(TFacXMLDemo1&&) = delete;

        ~TFacXMLDemo1();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacXMLDemo1& operator=(const TFacXMLDemo1&) = delete;
        TFacXMLDemo1& operator=(TFacXMLDemo1&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThreadFunc
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pUserData
        );



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods (from MXMLDocEvents)
        // -------------------------------------------------------------------
        tCIDLib::TVoid DocCharacters
        (
            const   TString&                strChars
            , const tCIDLib::TBoolean       bIsCDATA
            , const tCIDLib::TBoolean       bIsIgnorable
            , const tCIDXML::ELocations     eLocation
            , const tCIDLib::TBoolean       bAllSpaces
        )   final;

        tCIDLib::TVoid DocComment
        (
            const   TString&                strCommentText
            , const tCIDXML::ELocations     eLocation
        )   final;

        tCIDLib::TVoid DocPI
        (
            const   TString&                strTarget
            , const TString&                strValue
            , const tCIDXML::ELocations     eLocation
        )   final;

        tCIDLib::TVoid EndDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   final;

        tCIDLib::TVoid EndTag
        (
            const   TXMLElemDecl&           xdeclElem
        )   final;

        tCIDLib::TVoid ResetDocument() override;

        tCIDLib::TVoid StartDocument
        (
            const   TXMLEntitySrc&          xsrcOfRoot
        )   final;

        tCIDLib::TVoid StartTag
        (
                    TXMLParserCore&         xprsSrc
            , const TXMLElemDecl&           xdeclElem
            , const tCIDLib::TBoolean       bEmpty
            , const TVector<TXMLAttr>&      colAttrList
            , const tCIDLib::TCard4         c4AttrListSize
        )   final;

        tCIDLib::TVoid XMLDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
            , const TString&                strStandalone
        )   final;



        // -------------------------------------------------------------------
        //  Protected, inherited methods (from MXMLErrorEvents)
        // -------------------------------------------------------------------
        tCIDLib::TVoid HandleXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
            , const tCIDXML::EErrTypes      eType
            , const TString&                strText
            , const tCIDLib::TCard4         c4CurColumn
            , const tCIDLib::TCard4         c4CurLine
            , const TString&                strSystemId
        )   final;

        tCIDLib::TVoid ResetErrors() final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (from MXMLDocTypeEvents)
        // -------------------------------------------------------------------
        tCIDLib::TVoid AttrDef
        (
            const   TDTDAttrDef&            xadAttr
            , const tCIDLib::TBoolean       bIgnored
        )   final;

        tCIDLib::TVoid DocType
        (
            const   TDTDElemDecl&           xdeclRoot
            , const TString&                strPublicId
            , const TString&                strSystemId
        )   final;

        tCIDLib::TVoid DocTypeComment
        (
            const   TString&                strCommentText
        )   final;

        tCIDLib::TVoid DocTypePI
        (
            const   TString&                strTarget
            , const TString&                strValue
            , const tCIDXML::ELocations     eLocation
        )   final;

        tCIDLib::TVoid DocTypeWS
        (
            const   TString&                strChars
        )   final;

        tCIDLib::TVoid ElementDecl
        (
            const   TDTDElemDecl&           xdeclElement
            , const tCIDLib::TBoolean       bIgnored
        )   final;

        tCIDLib::TVoid EndAttList
        (
            const   TDTDElemDecl&           xdeclParentElement
        )   final;

        tCIDLib::TVoid EndDocType() final;

        tCIDLib::TVoid EndExtSubset() final;

        tCIDLib::TVoid EndIntSubset() final;

        tCIDLib::TVoid EntityDecl
        (
            const   TDTDEntityDecl&         xdeclEntity
            , const tCIDLib::TBoolean       bIgnored
        )   final;

        tCIDLib::TVoid NotationDecl
        (
            const   TDTDNotationDecl&       xdeclNotation
            , const tCIDLib::TBoolean       bIgnored
        )   final;

        tCIDLib::TVoid ResetDocType() override;

        tCIDLib::TVoid StartAttList
        (
            const   TDTDElemDecl&           xdeclParentElement
        )   final;

        tCIDLib::TVoid StartExtSubset() final;

        tCIDLib::TVoid StartIntSubset() final;

        tCIDLib::TVoid TextDecl
        (
            const   TString&                strVersion
            , const TString&                strEncoding
        )   final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (from MXMLEntityEvents)
        // -------------------------------------------------------------------
        tCIDLib::TVoid EndEntity
        (
            const   TXMLEntityDecl&         xdeclEnding
        )   final;

        tCIDLib::TVoid ResetEntities() final;

        tCIDLib::TVoid StartEntity
        (
            const   TXMLEntityDecl&         xdeclStarting
        )   final;


        // -------------------------------------------------------------------
        //  Protected, inherited methods (entity resolver)
        // -------------------------------------------------------------------
        tCIDXML::TEntitySrcRef esrResolve
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
            , const TString&                strEntityName
            , const TString&                strParentId
            , const tCIDXML::EResolveTypes  eResType
        )   final;

        tCIDLib::TVoid ResetResolver() final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLoadMapping
        (
                    TString&                strMapping
        );

        tCIDLib::TVoid DoParse
        (
                    TXMLParserCore&         xprsTest
            , const TString&                strToParse
        );

        tCIDLib::TVoid ShowChars
        (
            const   TString&                strChars
            , const tCIDLib::TBoolean       bDoEntReplacement = kCIDLib::True
        );

        tCIDLib::TVoid ShowUsage();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bShowWarnings
        //      By default, we supress warnings. This can be set to allow them
        //      to be displayed.
        //
        //  m_bTimeIt
        //      Defaults to false, but can be set via the /Time command line
        //      parameter. It cauess the parses to be timed and the results
        //      displayed.
        //
        //  m_c4ElemDepth
        //      This is used to track the element depth as the start/end
        //      tag events occur.
        //
        //  m_c4EntityNesting
        //      This is driven by the start/end entity events. It just tracks
        //      our entity nesting level, which we use to control some display
        //      stuff.
        //
        //  m_c4MaxErrs
        //      We take a command line parm to control how many errors we'll
        //      get before giving up. It defaults to 1.
        //
        //  m_eDisplayMode
        //      This indicates what type of display output we do.
        //
        //  m_eOpts
        //      The parser options, defaulted then possibly modified by
        //      command line parms.
        //
        //  m_pstrmErr
        //  m_pstrmOut
        //      The output stream objects. We support CIDLib style redirection
        //      here, so we have to get the redirected output streams and
        //      store them here.
        //
        //  m_pxvalTest
        //      We have to keep a pointer to the validator around so that
        //      callbacks can access it if needed. It's set and cleared within
        //      DoParse().
        //
        //  m_xcatMappings
        //      If we get any mappings on the command line, we set them up
        //      here and we set this on the parser if we have any.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bShowWarnings;
        tCIDLib::TBoolean   m_bTimeIt;
        tCIDLib::TCard4     m_c4ElemDepth;
        tCIDLib::TCard4     m_c4EntityNesting;
        tCIDLib::TCard4     m_c4MaxErrs;
        EDisplayModes       m_eDisplayMode;
        tCIDXML::EParseOpts m_eOpts;
        TTextOutStream*     m_pstrmErr;
        TTextOutStream*     m_pstrmOut;
        TDTDValidator*      m_pxvalTest;
        TXMLStdCatalog      m_xcatMappings;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacDemo1,TFacility)
};

