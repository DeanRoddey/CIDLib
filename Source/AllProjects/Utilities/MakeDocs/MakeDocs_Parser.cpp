//
// FILE NAME: MakeDocs_Parser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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


RTTIDecls(TDocParser, TObject)


// ----------------------------------------------------------------------------
//  CLASS: TDocParser
// PREFIX: prsr
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//  TDocParser: Constructors and destructors
// ----------------------------------------------------------------------------
TDocParser::TDocParser()
{
}

TDocParser::~TDocParser()
{
}
