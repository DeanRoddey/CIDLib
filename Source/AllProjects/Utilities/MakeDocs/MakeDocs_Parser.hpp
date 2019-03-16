//
// FILE NAME: MakeDocs_Parser.hpp
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
//  This is the header for the MakeDocs_Parser.cpp module, which implements
//  the TDocParser class. This class is a derivative of the standard text
//  stream lexer, from CIDLib.Dll. Our derivative just adds a little value
//  by building on the functionality of the base lexer class which understands
//  the particular lexical elements of our source files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  CLASS: TDocParser
// PREFIX: prsr
// ----------------------------------------------------------------------------
class TDocParser : public TStreamLexer
{
    public  :
        // --------------------------------------------------------------------
        //  Public types
        //
        //  ETokens
        //      This enum represents the available tokens in the source.
        //      These are the actual syntax tokens, but not the embedded
        //      formatting tags. They are dealt with separately.
        //
        //      These MUST be in alphabetical order because its used to
        //      access an array internally which is in this order. The
        //      special ones must be at the end because they have no array
        //      entries.
        // --------------------------------------------------------------------
        enum class ETokens
        {
            Caveats             // @Caveats
            , Categories        // @Categories
            , Class             // @Class
            , Description       // @Description
            , EndErrors         // @EndErrors
            , EndExceptions     // @EndExceptions
            , EndMethod         // @EndMethod
            , EndOverview       // @EndOverview
            , EndParams         // @EndParams
            , Error             // @Error
            , Errors            // @Errors
            , Exception         // @Exception
            , Exceptions        // @Exceptions
            , Facility          // @Facility
            , Group             // @Group
            , Header            // @Header
            , Method            // @Method
            , Mixins            // @Mixins
            , Overview          // @Overview
            , Param             // @Param
            , Params            // @Params
            , ParentClass       // @Parent
            , Prefix            // @Prefix
            , Related           // @Related
            , Return            // @Return
            , Comment

            // These are special ones for parsing purposes
            , None
            , End
        };


        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        TDocParser
        (
                    TTextInStream&          strmSource
            , const TString&                strClassName
        );

        ~TDocParser();


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bIsCurrentClass
        (
            const   TString&                strToTest
        )   const;

        TDocParser::ETokens eNextToken();

        const TString& strClassName() const;

        TString strTokenText
        (
            const   ETokens                 eToken
        )   const;


    private :
        // --------------------------------------------------------------------
        //  Private, non-static methods
        // --------------------------------------------------------------------
        TDocParser::ETokens eMapToken
        (
            const   TString&                strText
        )   const;


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_strClassName
        //      The name of the class whose file we are parsing.
        // --------------------------------------------------------------------
        TString     m_strClassName;
};

extern TTextOutStream& operator<<(TTextOutStream&, const TDocParser::ETokens);


inline tCIDLib::TBoolean
TDocParser::bIsCurrentClass(const TString& strToTest) const
{
    // Just compare the text, not the objects!
    return m_strClassName.bCompare(strToTest);
}

inline const TString& TDocParser::strClassName() const
{
    return m_strClassName;
}
