//
// FILE NAME: MakeDocs_ParserError.cpp
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
//  This module implements the TDocError class, which is the exception thrown
//  when parsing errors occur.
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


// -----------------------------------------------------------------------------
//   CLASS: TDocError
//  PREFIX: err
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  TDocError: Constructors and destructors
// -----------------------------------------------------------------------------
TDocError::TDocError(   const   tCIDLib::TErrCode   errcId
                        , const tCIDLib::TCard4     c4SourceLine) :

    m_c4SourceLine(c4SourceLine)
    , m_errcId(errcId)
    , m_strText(kCIDLib::pszEmptyZStr, 1024)
{
    facMakeDocs.bLoadCIDMsg(errcId, m_strText);
}

TDocError::TDocError(   const   tCIDLib::TErrCode   errcId
                        , const tCIDLib::TCard4     c4SourceLine
                        , const TString&            strText1) :

    m_c4SourceLine(c4SourceLine)
    , m_errcId(errcId)
    , m_strText(kCIDLib::pszEmptyZStr, 1024)
{
    facMakeDocs.bLoadCIDMsg(errcId, m_strText, strText1);
}

TDocError::TDocError(   const   tCIDLib::TErrCode   errcId
                        , const tCIDLib::TCard4     c4SourceLine
                        , const TString&            strText1
                        , const TString&            strText2) :

    m_c4SourceLine(c4SourceLine)
    , m_errcId(errcId)
    , m_strText(kCIDLib::pszEmptyZStr, 1024)
{
    facMakeDocs.bLoadCIDMsg(errcId, m_strText, strText1, strText2);
}

TDocError::~TDocError()
{
}
