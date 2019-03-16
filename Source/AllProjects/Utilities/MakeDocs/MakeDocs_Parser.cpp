//
// FILE NAME: MakeDocs_Parser.cpp
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
//  This module implements the parser class that is used to parse the source
//  text files. It is built on the standard text stream lexer, and just adds
//  some extra code to look for our particular lexical elements.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "MakeDocs.hpp"


// ----------------------------------------------------------------------------
//  Local constant values
// ----------------------------------------------------------------------------
const tCIDLib::TCard4 c4HashModulus = 17;


// ----------------------------------------------------------------------------
//  Local types
//
//  TTokenItem
//      This small structure represents a token that we look for. The tokens
//      are hashed for speed.
// ----------------------------------------------------------------------------
struct TTokenItem
{
    TDocParser::ETokens         eToken;
    tCIDLib::TCh*               pszToken;
    tCIDLib::THashVal           hshToken;
};


// ----------------------------------------------------------------------------
//  Local static data
//
//  aszTokens
//      These are our syntax tokens. We tell the parser about them and it will
//      treat them as atomic tokens even when there is no whitespace around
//      them.
// ----------------------------------------------------------------------------
const   tCIDLib::TCh* aszTokens[] =
{
    // This token starts a comment
    L"//"
    , L">"
};
const tCIDLib::TCard4  c4SyntaxCount =   tCIDLib::c4ArrayElems(aszTokens);

TTokenItem atokList[] =
{
        { TDocParser::ETokens::Caveats        , L"@Caveats"       , 0 }
    ,   { TDocParser::ETokens::Categories     , L"@Categories"    , 0 }
    ,   { TDocParser::ETokens::Class          , L"@Class"         , 0 }
    ,   { TDocParser::ETokens::Description    , L"@Description"   , 0 }
    ,   { TDocParser::ETokens::EndErrors      , L"@EndErrors"     , 0 }
    ,   { TDocParser::ETokens::EndExceptions  , L"@EndExceptions" , 0 }
    ,   { TDocParser::ETokens::EndMethod      , L"@EndMethod"     , 0 }
    ,   { TDocParser::ETokens::EndOverview    , L"@EndOverview"   , 0 }
    ,   { TDocParser::ETokens::EndParams      , L"@EndParams"     , 0 }
    ,   { TDocParser::ETokens::Error          , L"@Error"         , 0 }
    ,   { TDocParser::ETokens::Errors         , L"@Errors"        , 0 }
    ,   { TDocParser::ETokens::Exception      , L"@Exception"     , 0 }
    ,   { TDocParser::ETokens::Exceptions     , L"@Exceptions"    , 0 }
    ,   { TDocParser::ETokens::Facility       , L"@Facility"      , 0 }
    ,   { TDocParser::ETokens::Group          , L"@Group"         , 0 }
    ,   { TDocParser::ETokens::Header         , L"@Header"        , 0 }
    ,   { TDocParser::ETokens::Method         , L"@Method"        , 0 }
    ,   { TDocParser::ETokens::Mixins         , L"@Mixins"        , 0 }
    ,   { TDocParser::ETokens::Overview       , L"@Overview"      , 0 }
    ,   { TDocParser::ETokens::Param          , L"@Param"         , 0 }
    ,   { TDocParser::ETokens::Params         , L"@Params"        , 0 }
    ,   { TDocParser::ETokens::ParentClass    , L"@Parent"        , 0 }
    ,   { TDocParser::ETokens::Prefix         , L"@Prefix"        , 0 }
    ,   { TDocParser::ETokens::Related        , L"@Related"       , 0 }
    ,   { TDocParser::ETokens::Return         , L"@Return"        , 0 }
    ,   { TDocParser::ETokens::Comment        , L"//"             , 0 }
};
const tCIDLib::TCard4 c4TokenCount = tCIDLib::c4ArrayElems(atokList);


// ----------------------------------------------------------------------------
//  CLASS: TDocParser
// PREFIX: prsr
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TDocParser: Global operators
// ----------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmOut, const TDocParser::ETokens eToken)
{
    if (eToken == TDocParser::ETokens::Categories)
        strmOut << L"Categories";
    else if (eToken == TDocParser::ETokens::Caveats)
        strmOut << L"Caveats";
    else if (eToken == TDocParser::ETokens::Class)
        strmOut << L"Class";
    else if (eToken == TDocParser::ETokens::Description)
        strmOut << L"Description";
    else if (eToken == TDocParser::ETokens::EndErrors)
        strmOut << L"End Errors";
    else if (eToken == TDocParser::ETokens::EndExceptions)
        strmOut << L"End Exceptions";
    else if (eToken == TDocParser::ETokens::EndMethod)
        strmOut << L"End Method";
    else if (eToken == TDocParser::ETokens::EndOverview)
        strmOut << L"End Overview";
    else if (eToken == TDocParser::ETokens::EndParams)
        strmOut << L"End Params";
    else if (eToken == TDocParser::ETokens::Error)
        strmOut << L"Error";
    else if (eToken == TDocParser::ETokens::Errors)
        strmOut << L"Errors";
    else if (eToken == TDocParser::ETokens::Exception)
        strmOut << L"Exception";
    else if (eToken == TDocParser::ETokens::Exceptions)
        strmOut << L"Exceptions";
    else if (eToken == TDocParser::ETokens::Facility)
        strmOut << L"Facility";
    else if (eToken == TDocParser::ETokens::Group)
        strmOut << L"Group";
    else if (eToken == TDocParser::ETokens::Header)
        strmOut << L"Header";
    else if (eToken == TDocParser::ETokens::Method)
        strmOut << L"Method";
    else if (eToken == TDocParser::ETokens::Mixins)
        strmOut << L"Mixin Classes";
    else if (eToken == TDocParser::ETokens::Overview)
        strmOut << L"Overview";
    else if (eToken == TDocParser::ETokens::Param)
        strmOut << L"Parameter";
    else if (eToken == TDocParser::ETokens::Params)
        strmOut << L"Parameters";
    else if (eToken == TDocParser::ETokens::ParentClass)
        strmOut << L"Parent Class";
    else if (eToken == TDocParser::ETokens::Prefix)
        strmOut << L"Class Prefix";
    else if (eToken == TDocParser::ETokens::Related)
        strmOut << L"Related";
    else if (eToken == TDocParser::ETokens::Return)
        strmOut << L"Return";
    else if (eToken == TDocParser::ETokens::Comment)
        strmOut << L"//";
    else if (eToken == TDocParser::ETokens::None)
        strmOut << L"None";
    else if (eToken == TDocParser::ETokens::End)
        strmOut << L"End";
    else
    {
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcPrsrBadToken
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TInteger(tCIDLib::i4EnumOrd(eToken))
        );
    }

    return strmOut;
}

// ----------------------------------------------------------------------------
//  TDocParser: Constructors and destructors
// ----------------------------------------------------------------------------
TDocParser::TDocParser(TTextInStream& strmSource, const TString& strClassName) :

    TStreamLexer(&strmSource, tCIDLib::EAdoptOpts::NoAdopt)
    , m_strClassName(strClassName)
{
    static tCIDLib::TBoolean bFirstTime = kCIDLib::True;
    if (bFirstTime)
    {
        bFirstTime = kCIDLib::False;

        // Do the hashes for all of the tokens.
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4TokenCount; c4Ind++)
        {
            atokList[c4Ind].hshToken = TRawStr::hshHashStr
            (
                atokList[c4Ind].pszToken
                , c4HashModulus
            );

            if (!atokList[c4Ind].hshToken)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_NotValidUnicode
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
        }
    }

    // Set up our token list
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4SyntaxCount; c4Ind++)
        AddSyntaxToken(aszTokens[c4Ind]);
}

TDocParser::~TDocParser()
{
}


// ----------------------------------------------------------------------------
//  TDocParser: Public, non-virtual methods
// ----------------------------------------------------------------------------
TDocParser::ETokens TDocParser::eNextToken()
{
    //
    //  We do this in a loop because we throw away any comment lines.
    //  We use a local string to read tokesn into.
    //
    TString strToken(128UL);
    while (1)
    {
        // Get the next token, using whitespace as separators
        GetNextToken(kCIDLib::szWhitespace, strToken);

        //
        //  If the token is empty, then we are done so we return End
        //
        if (strToken.bIsEmpty())
            return TDocParser::ETokens::End;

        //
        //  See if its one of our tags. If its the end, then just return
        //  that.
        //
        TDocParser::ETokens eToken = eMapToken(strToken);

        if (eToken == TDocParser::ETokens::End)
            return eToken;

        if (eToken == TDocParser::ETokens::None)
        {
            UnGetToken(strToken);
            return eToken;
        }

        //
        //  If its a comment, then blow off the rest of the line and
        //  keep going.
        //
        if (eToken == TDocParser::ETokens::Comment)
        {
            FlushLine();
            continue;
        }

        // Return the token
        return eToken;
    }
}


TString TDocParser::strTokenText(const ETokens eToken) const
{
    // Check for special ones first
    if (eToken == ETokens::None)
        return TString(L"None");
    else if (eToken == ETokens::End)
        return TString(L"End");
    else if (eToken == ETokens::Comment)
        return TString(L"//");

    // Else we just return the text from the token list
    return TString(atokList[tCIDLib::c4EnumOrd(eToken)].pszToken);
}


// ----------------------------------------------------------------------------
//  TDocParser: Private, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method checks the passed text against our token list and maps
//  the text to a token enum.
//
TDocParser::ETokens TDocParser::eMapToken(const TString& strText) const
{
    // Get the hash of the passed token text
    tCIDLib::THashVal hshToken = strText.hshCalcHash(c4HashModulus);

    //
    //  Now use that to check each token for a possible match. If the
    //  hash matches, then do the actuall comparision.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TokenCount; c4Index++)
    {
        if (atokList[c4Index].hshToken == hshToken)
        {
            if (strText == atokList[c4Index].pszToken)
                return atokList[c4Index].eToken;
        }
    }
    return TDocParser::ETokens::None;
}
