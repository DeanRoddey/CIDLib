//
// FILE NAME: MakeDocs_ParserError.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
