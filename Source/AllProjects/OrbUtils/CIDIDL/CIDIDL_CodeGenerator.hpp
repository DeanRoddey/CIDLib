//
// FILE NAME: CIDIDL_CodeGenerator.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/10/2004
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the header for the CIDIDL_CodeGenerator.cpp file. This file
//  implements an abstract based class via which the IDL generates it's code.
//  This allows multiple languages to be generated via derivatives of this
//  class.
//
//  The IDL generator cannot dictate what files get created, so that is the
//  responsibility of the code generator. The IDL generator will just pass
//  along the data parsed out of the XML input. So the generators don't have
//  to deal with XML parsing, just spitting out the code in the correct
//  format.
//
//  The flow of calls will be like this. Note that either of the top level
//  sections are optional and there will be no calls for that section if it
//  is not present in the input XML.
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
class TCodeGenerator : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCodeGenerator();

        ~TCodeGenerator();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid BeginClientIntf
        (
            const   TString&                strBase
            , const TString&                strOutputDir
            , const TString&                strFacName
            , const TString&                strOptName
            , const TString&                strExpKeyword
            , const tCIDLib::TBoolean       bPrivHeader
        ) = 0;

        virtual tCIDLib::TVoid BeginCodeGen() = 0;

        virtual tCIDLib::TVoid BeginConstants() = 0;

        virtual tCIDLib::TVoid BeginGlobals
        (
            const   TString&                strGlobalsOutputDir
            , const TString&                strGlobalsFacName
            , const TString&                strGlobalsName
            , const TString&                strExpKeyword
            , const tCIDLib::TBoolean       bPrivHeader
        ) = 0;

        virtual tCIDLib::TVoid BeginIntf
        (
            const   TString&                strInterfaceName
            , const TString&                strInterfaceId
        ) = 0;

        virtual tCIDLib::TVoid BeginMethods() = 0;

        virtual tCIDLib::TVoid BeginServerIntf
        (
            const   TString&                strBase
            , const TString&                strOutputDir
            , const TString&                strFacName
            , const TString&                strOptName
            , const TString&                strExpKeyword
            , const tCIDIDL::TParmList&     colCtorParams
            , const tCIDLib::TBoolean       bPrivHeader

        ) = 0;

        virtual tCIDLib::TVoid BeginTypes() = 0;

        virtual tCIDLib::TVoid EndClientIntf() = 0;

        virtual tCIDLib::TVoid EndCodeGen() = 0;

        virtual tCIDLib::TVoid EndGlobals() = 0;

        virtual tCIDLib::TVoid EndConstants() = 0;

        virtual tCIDLib::TVoid EndIntf() = 0;

        virtual tCIDLib::TVoid EndMethods() = 0;

        virtual tCIDLib::TVoid EndServerIntf() = 0;

        virtual tCIDLib::TVoid EndTypes() = 0;

        virtual tCIDLib::TVoid GenConstant
        (
            const   TString&                strName
            , const TString&                strType
            , const TString&                strValue
            , const TString&                strDocs
        ) = 0;

        virtual tCIDLib::TVoid GenEnum
        (
            const   TString&                strDocs
            ,       TCGenEnumInfo&          einfoToGen
        ) = 0;

        virtual tCIDLib::TVoid GenMethod
        (
            const   TString&                strName
            , const TCGenTypeInfo&          tinfoRet
            , const tCIDIDL::TParmList&     colParams
            , const tCIDLib::TBoolean       bPollMethod
            , const tCIDLib::TCard4         c4Timeout
            , const tCIDLib::TBoolean       bInBaseClass
            , const tCIDLib::TCard4         c4MethIndex
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenDocText
        (
                    TTextFileOutStream&     strmOut
            , const TString&                strRawText
            , const tCIDIDL::EOutputs       eOutput
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TCodeGenerator(const TCodeGenerator&);
        tCIDLib::TVoid operator=(const TCodeGenerator&);


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCodeGenerator,TObject)
};


