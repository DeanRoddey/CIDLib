//
// FILE NAME: CIDIDL_CodeGenerator.cpp
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
// DESCRIPTION:
//
//  This file implements the non-virtual bits of the base code generator class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDIDL.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCodeGenerator,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCodeGenerator
//  PREFIX: cgen
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCodeGenerator: Constructors and Destructor
// ---------------------------------------------------------------------------
TCodeGenerator::TCodeGenerator()
{
}

TCodeGenerator::~TCodeGenerator()
{
}


// ---------------------------------------------------------------------------
//  TCodeGenerator: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCodeGenerator::GenDocText(         TTextFileOutStream& strmOut
                            , const TString&            strRawText
                            , const tCIDIDL::EOutputs   eOutput)
{
    // Decide the fixed indent level
    tCIDLib::TCard4 c4FixedIndent;
    if ((eOutput == tCIDIDL::EOutputs::Client)
    ||  (eOutput == tCIDIDL::EOutputs::Server))
    {
        c4FixedIndent = 8;
    }
     else
    {
        c4FixedIndent = 4;
    }

    strmOut << TTextOutStream::Spaces(c4FixedIndent) << kCIDLib::NewLn
            << TTextOutStream::Spaces(c4FixedIndent)
            << L"// " << TTextOutStream::RepChars(L'-', 72) << kCIDLib::NewLn;

    //
    //  Set up an input stream over the text, because we have to do it
    //  a line at a time, so that we can handle indentation.
    //
    TTextStringInStream strmSrc(&strRawText);

    tCIDLib::TBoolean   bSetIndent = kCIDLib::False;
    tCIDLib::TCard4     c4Indent = 0;
    TString             strLine;
    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCard4 c4CurLen = strmSrc.c4GetLine(strLine);

        //
        //  The text will include the indent from the XML file itself,
        //  which is generally not what we want. So on the first non-
        //  empty line of doc text, figure out how many leading spaces
        //  it had, and indent the lines that much. That will will
        //  maintain relative indents for good formatting.
        //
        if (bSetIndent)
        {
            if (strLine.c4Length() > c4Indent)
                strLine.Cut(0, c4Indent);
            strmOut << TTextOutStream::Spaces(c4FixedIndent)
                    << L"//  " << strLine << kCIDLib::NewLn;
        }
         else
        {
            const tCIDLib::TCard4 c4OrgLen = strLine.c4Length();
            strLine.StripWhitespace();
            if (!strLine.bIsEmpty())
            {
                c4Indent = c4OrgLen - strLine.c4Length();
                bSetIndent = kCIDLib::True;
                strmOut << TTextOutStream::Spaces(c4FixedIndent)
                        << L"//  " << strLine << kCIDLib::NewLn;
            }
        }
    }
    strmOut << TTextOutStream::Spaces(c4FixedIndent)
            << L"// " << TTextOutStream::RepChars(L'-', 72) << kCIDLib::NewLn;
}


