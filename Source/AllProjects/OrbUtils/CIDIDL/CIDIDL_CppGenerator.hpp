//
// FILE NAME: CIDIDL_CppGenerator.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/10/2004
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
//  This is the header for the CIDIDL_CppGenerator.cpp file. This file
//  implements a derivative of the abstract base output generator class. This
//  one generates code for the C++ language.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TCppGenerator
//  PREFIX: cgen
// ---------------------------------------------------------------------------
class TCppGenerator : public TCodeGenerator
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCppGenerator();

        TCppGenerator(const TCppGenerator&) = delete;

        ~TCppGenerator();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=(const TCppGenerator&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BeginClientIntf
        (
            const   TString&                strBase
            , const TString&                strOutputDir
            , const TString&                strFacName
            , const TString&                strOptName
            , const TString&                strExpKeyword
            , const tCIDLib::TBoolean       bPrivHeader
        );

        tCIDLib::TVoid BeginCodeGen();

        tCIDLib::TVoid BeginConstants();

        tCIDLib::TVoid BeginGlobals
        (
            const   TString&                strGlobalsOutputDir
            , const TString&                strGlobalsFacName
            , const TString&                strGlobalsName
            , const TString&                strExpKeyword
            , const tCIDLib::TBoolean       bPrivHeader
        );

        tCIDLib::TVoid BeginIntf
        (
            const   TString&                strInterfaceName
            , const TString&                strInterfaceId
        );

        tCIDLib::TVoid BeginMethods();

        tCIDLib::TVoid BeginServerIntf
        (
            const   TString&                strBase
            , const TString&                strOutputDir
            , const TString&                strFacName
            , const TString&                strOptName
            , const TString&                strExpKeyword
            , const tCIDIDL::TParmList&     colCtorParams
            , const tCIDLib::TBoolean       bPrivHeader
        );

        tCIDLib::TVoid BeginTypes();

        tCIDLib::TVoid EndClientIntf();

        tCIDLib::TVoid EndCodeGen();

        tCIDLib::TVoid EndConstants();

        tCIDLib::TVoid EndGlobals();

        tCIDLib::TVoid EndIntf();

        tCIDLib::TVoid EndMethods();

        tCIDLib::TVoid EndServerIntf();

        tCIDLib::TVoid EndTypes();

        tCIDLib::TVoid GenConstant
        (
            const   TString&                strName
            , const tCIDIDL::ETypes         Type
            , const TString&                strValue
            , const TString&                strDocs
        );

        tCIDLib::TVoid GenEnum
        (
            const   TString&                strDocs
            ,       TCGenEnumInfo&          einfoToGen
        );

        tCIDLib::TVoid GenMethod
        (
            const   TString&                strName
            , const TCGenTypeInfo&          tinfoRet
            , const tCIDIDL::TParmList&     colParams
            , const tCIDLib::TBoolean       bPollMethod
            , const tCIDLib::TCard4         c4Timeout
            , const tCIDLib::TBoolean       bInBaseClass
            , const tCIDLib::TCard4         c4MethIndex
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        typedef tCIDLib::TStrHashSet        TMethNameList;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatConstType
        (
            const   TString&                strRawType
            ,       TString&                strToFill
        )   const;

        tCIDLib::TVoid FormatParam
        (
            const   TCGenMethodParm&        mparmFmt
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bHeader
        );

        tCIDLib::TVoid FormatType
        (
            const   TCGenTypeInfo&          tinfoFmt
            ,       TString&                strToFill
        )   const;

        tCIDLib::TVoid MarshallClientParms
        (
            const   TString&                strMethodName
            ,       TTextFileOutStream&     strmOut
            , const tCIDIDL::TParmList&     colParams
        );

        tCIDLib::TVoid UnMarshallClientParms
        (
            const   TString&                strMethodName
            ,       TTextFileOutStream&     strmOut
            , const tCIDIDL::TParmList&     colParams
            , const TCGenTypeInfo&          tinfoRet
            , const tCIDLib::TBoolean       bPollMethod
        );

        tCIDLib::TVoid WriteTemplate
        (
                    TTextFileOutStream&     strmOut
            , const tCIDLib::TCh* const     pszData
            , const TString&                strFilePath
            , const tCIDLib::TBoolean       bPrivHeader = kCIDLib::False
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colMethNameList
        //      We have to deal with overloaded methods, so we use a hash
        //      set of strings to keep track of all used method names. If
        //      we see one that's already been used, we start numbering them
        //      Foo1(), Foo2(), etc...
        //
        //  m_eMode
        //      The current output mode, e.g. server, client, globals, etc...
        //      This lets the more generic methods know what type of info
        //      to output.
        //
        //  m_strmHeader
        //  m_strmImpl
        //      We will always be either generating a header or a header and
        //      an implementation file, so these are set up during the
        //      appropriate callbacks above, and used by other methods to
        //      generate output.
        //
        //  m_strmXHeader
        //      We have some stuff that has to be done while we are processing
        //      the enums, but the actual contents has to go at the end of
        //      the header after namespace is closed off (the global
        //      bin/text streaming method sigs.) So we store them here and
        //      after the namespace is closed, we spit this stuff out into
        //      the real header.
        //
        //  m_strBaseClass
        //      We set this to either the server or client base class,
        //      according to which we are doing at the time, if either.
        //
        //  m_strClientClass
        //      The name of the client class, which is set in the beginning of
        //      the interface processing, if that's called. It is used in
        //      token replacement.
        //
        //  m_strInterfaceId
        //  m_strInterfaceName
        //      We store these away in the begin interface callback, for later
        //      use.
        //
        //  m_strExpKeyword
        //      The export keyword for the client or server side, when we are
        //      generating those.
        //
        //  m_strFacName
        //      This is set to the client or server or global facilty name,
        //      accoridng to what we are generating at the time.
        //
        //  m_strNList
        //  m_strNListC
        //  m_strNPList
        //  m_strNPListC
        //      If we have any 'passthrough' constructor parameters, we build
        //      up pre-done strings for the header and impl files so that they
        //      can be just stuck into the templates as replacement parms.
        //
        //  m_strServerClass
        //      The name of the server class, which is set in the beginning of
        //      the interface processing, if that's called. It is used in
        //      token replacement.
        //
        //  m_strTargetClass
        //      The name of the target class being generated, which is a copy
        //      of either the client or server class name, according to which
        //      mode we are in.
        // -------------------------------------------------------------------
        TMethNameList       m_colMethNameList;
        tCIDIDL::EOutputs   m_eMode;
        TTextFileOutStream  m_strmHeader;
        TTextFileOutStream  m_strmImpl;
        TTextStringOutStream m_strmXHeader;
        TString             m_strBaseClass;
        TString             m_strClientClass;
        TString             m_strExpKeyword;
        TString             m_strFacName;
        TString             m_strInterfaceId;
        TString             m_strInterfaceName;
        TString             m_strNList;
        TString             m_strNListC;
        TString             m_strNPList;
        TString             m_strNPListC;
        TString             m_strServerClass;
        TString             m_strTargetClass;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCppGenerator,TCodeGenerator)
};


