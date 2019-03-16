//
// FILE NAME: MakeDocs_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the header for the MakeDocs_ThisFacility.cpp module. This module
//  implements the facility class for this program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Typedefs
//
//  TClassList
//      This is a typedef for a hashed set of TClassInfo objects. It is used
//      to maintain a list of all available classes.
// ---------------------------------------------------------------------------
typedef TKeyedHashSet<class TClassDocInfo,class TString,class TStringKeyOps> TClassList;


// ---------------------------------------------------------------------------
//  CLASS: TFacMakeDocs
// PREFIX: fac
// ---------------------------------------------------------------------------
class TFacMakeDocs : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TFacMakeDocs();

        ~TFacMakeDocs();


        // -------------------------------------------------------------------
        //  Public, non-static methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsAToken
        (
            const   TString&                strToTest
            ,       TString&                strToFill
        )   const;

        tCIDLib::TBoolean bIsValidClassName
        (
            const   TString&                strToTest
        )   const;

        tCIDLib::TBoolean bParseCommandLine();

        tCIDLib::TVoid BuildClassList();

        tCIDLib::TVoid BuildTokenList();

        tMakeDocs::EClassCats eMapCategoryName
        (
            const   TString&                strName
        )   const;

        tCIDLib::TVoid GenerateDocs();

        const TString& strToken
        (
            const   TString&                strToken
        )   const;

        TTextOutStream& strmOut();

        tCIDLib::TVoid ShowBlurb();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckPath
        (
            const   TString&                strToCheck
            , const tCIDLib::TBoolean       bCreateOk = kCIDLib::False
        );

        tCIDLib::TBoolean bGenSubSystemPage
        (
            const   TString&                strSubName
        );

        tCIDLib::TBoolean bOutOfDate
        (
            const   TString&                strFileToCheck
            , const TString&                strSourceFile
        );

        TClassDocInfo& cldiForClass
        (
            const   TString&                strName
        );

        tCIDLib::TVoid CopyFiles
        (
            const   TString&                strSubDir
        );

        tCIDLib::TVoid DoSubSystem
        (
            const   TString&                strSubName
        );

        tCIDLib::TVoid GenCatClassIndex();

        tCIDLib::TVoid TranslateOverview
        (
                    TDocParser&             prsrSource
            , const TString&                strFileName
        );

        tCIDLib::TVoid TranslateMethod
        (
                    TDocParser&             prsrSource
            , const TString&                strMethodName
            , const TString&                strFileName
        );

        tCIDLib::TVoid TranslateSource
        (
                    TDocParser&             prsrSource
            ,       TTextOutStream&         strmTarget
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bForceUpdate
        //      This is set if the user uses the /FORCE option on the command
        //      line. This will force all files to be updated, even if they
        //      are not out of date.
        //
        //  m_colClasses
        //      A collection that is used to hold all of the class names.  It
        //      is built by doint an initial iteration of the directories and
        //      finding all of the .ClassDoc files. The base part of each
        //      ClassDoc file is the name of the class that it documents.
        //      During parsing though document content, this list is checked
        //      to see if a class name is present. If so, a link is generated
        //      automatically.
        //
        //  m_conIn
        //  m_conOut
        //      These are our console objects for user interaction. We have
        //      one for output, and one for input.
        //
        //  m_envTokens
        //      This is a TEnvironment object, which is usually used for
        //      system environment stuff but actually is a general purpose
        //      key=value string mapping structure. Its used to hold the
        //      replacement parameters from "Replacement.Txt".
        //
        //  m_pathSourceDir
        //  m_pathTargetDir
        //      These are the paths to the source directory, where the
        //      ClassDoc files are, and the output directory, where we put
        //      the corresponding formatted files.
        //
        //  m_ptransOut
        //      This is the translator object that handles the translation of
        //      formatting tags to the output.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bForceUpdate;
        TClassList          m_colClasses;
        TInConsole          m_conIn;
        TOutConsole         m_conOut;
        TEnvironment        m_envTokens;
        TPathStr            m_pathSourceDir;
        TPathStr            m_pathTargetDir;
        TTranslator*        m_ptransOut;
};


// ---------------------------------------------------------------------------
//  TFacMakeDocs: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline TTextOutStream& TFacMakeDocs::strmOut()
{
    return m_conOut;
}


// -----------------------------------------------------------------------------
//  An operator to output the text for categories
// -----------------------------------------------------------------------------
extern TTextOutStream& operator<<(TTextOutStream&, const tMakeDocs::EClassCats);

