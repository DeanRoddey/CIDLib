//
// FILE NAME: CIDMacroEng_ErrorHandler.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/15/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the little bit of out of line stuff for the error
//  handler base class, and the implementatino of a simple implementation
//  of a standalone error handler that just dumps to a given text output
//  stream.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngStrmErrHandler,TObject)
RTTIDecls(TMEngStrmPrsErrHandler,TObject)


// ---------------------------------------------------------------------------
//  CLASS: MMEngErrHandler
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMEngErrHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
MMEngErrHandler::~MMEngErrHandler()
{
}


// ---------------------------------------------------------------------------
//  MMEngErrHandler: Hidden constructors
// ---------------------------------------------------------------------------
MMEngErrHandler::MMEngErrHandler()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrmErrHandler
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStrmErrHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStrmErrHandler::TMEngStrmErrHandler() :

    m_pstrmTarget(0)
{
}

TMEngStrmErrHandler::TMEngStrmErrHandler(TTextOutStream* const pstrmToUse) :

    m_pstrmTarget(pstrmToUse)
{
}

TMEngStrmErrHandler::~TMEngStrmErrHandler()
{
}


// ---------------------------------------------------------------------------
//  TMEngStrmErrHandler: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStrmErrHandler::Exception(const TError& errThrown, TCIDMacroEngine& meSource)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    *m_pstrmTarget  << L"A CIDLib exception occured:\n"
                    << errThrown
                    << L"\n\nStack dump:\n"
                    << L"----------------------\n";

    tCIDLib::TCard4 c4Index = kCIDLib::c4MaxCard;
    while (meSource.bFormatNextCallFrame(*m_pstrmTarget, c4Index))
        *m_pstrmTarget << kCIDLib::NewLn;
    *m_pstrmTarget << kCIDLib::DNewLn << kCIDLib::FlushIt;
}


tCIDLib::TVoid
TMEngStrmErrHandler::MacroException(const   TMEngExceptVal&     mecvThrown
                                    ,       TCIDMacroEngine&    meSource)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    // Output the exception info
    *m_pstrmTarget  << L"An macro exception occured:\n"
                    << L"  CLASS: " << mecvThrown.strSrcClassPath() << kCIDLib::NewLn
                    << L"   LINE: " << mecvThrown.c4LineNum() << kCIDLib::NewLn
                    << L"  ERROR: " << mecvThrown.strErrorName() << kCIDLib::NewLn
                    << L"   TEXT: " << mecvThrown.strErrorText() << kCIDLib::DNewLn
                    << L"\nStack dump:\n"
                    << L"----------------------\n";

    tCIDLib::TCard4 c4Index = kCIDLib::c4MaxCard;
    while (meSource.bFormatNextCallFrame(*m_pstrmTarget, c4Index))
        *m_pstrmTarget << kCIDLib::NewLn;
    *m_pstrmTarget << kCIDLib::DNewLn << kCIDLib::FlushIt;
}


tCIDLib::TVoid
TMEngStrmErrHandler::UnknownException(TCIDMacroEngine& meSource)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    *m_pstrmTarget  << L"An unknown exception occured:\n"
                    << L"\nStack dump:\n"
                    << L"----------------------\n";

    tCIDLib::TCard4 c4Index = kCIDLib::c4MaxCard;
    while (meSource.bFormatNextCallFrame(*m_pstrmTarget, c4Index))
        *m_pstrmTarget << kCIDLib::NewLn;
    *m_pstrmTarget << kCIDLib::DNewLn << kCIDLib::FlushIt;
}


// ---------------------------------------------------------------------------
//  TMEngStrmErrHandler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStrmErrHandler::SetStream(TTextOutStream* const pstrmToUse)
{
    m_pstrmTarget = pstrmToUse;
}




// ---------------------------------------------------------------------------
//  CLASS: MMEngPrsErrHandler
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMEngPrsErrHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
MMEngPrsErrHandler::~MMEngPrsErrHandler()
{
}


// ---------------------------------------------------------------------------
//  MMEngPrsErrHandler: Hidden constructors
// ---------------------------------------------------------------------------
MMEngPrsErrHandler::MMEngPrsErrHandler()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrmPrsErrHandler
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStrmPrsErrHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStrmPrsErrHandler::TMEngStrmPrsErrHandler() :

    m_pstrmTarget(0)
{
}

TMEngStrmPrsErrHandler::TMEngStrmPrsErrHandler(TTextOutStream* const pstrmToUse) :

    m_pstrmTarget(pstrmToUse)
{
}

TMEngStrmPrsErrHandler::~TMEngStrmPrsErrHandler()
{
}


// ---------------------------------------------------------------------------
//  TMEngStrmPrsErrHandler: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStrmPrsErrHandler::ParseEvent( const   tCIDMacroEng::EPrsEvs   eEvent
                                    , const TString&                strText
                                    , const tCIDLib::TCard4         c4LineNum
                                    , const tCIDLib::TCard4         c4ColNum
                                    , const TString&                strClassPath)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    switch(eEvent)
    {
        case tCIDMacroEng::EPrsEvs::InfoMsg :
            *m_pstrmTarget << L"(INFO: ";
            break;

        case tCIDMacroEng::EPrsEvs::WarningMsg :
            *m_pstrmTarget << L"(WARNING: ";
            break;

        case tCIDMacroEng::EPrsEvs::ErrorMsg :
        case tCIDMacroEng::EPrsEvs::CIDLibExcept :
        case tCIDMacroEng::EPrsEvs::UnknownExcept :
        default :
            *m_pstrmTarget << L"(ERROR: ";
            break;
    };

    *m_pstrmTarget  << strClassPath << L", "
                    << c4LineNum << L"/" << c4ColNum << L")\n  "
                    << strText << kCIDLib::DNewLn << kCIDLib::FlushIt;
}


tCIDLib::TVoid
TMEngStrmPrsErrHandler::ParseException( const   TError&         errCaught
                                        , const tCIDLib::TCard4 c4LineNum
                                        , const tCIDLib::TCard4 c4ColNum
                                        , const TString&        strClassPath)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    *m_pstrmTarget  << L"(EXCEPTION: " << strClassPath << L", "
                    << c4LineNum << L"/" << c4ColNum << L")\n  "
                    << errCaught.strErrText() << kCIDLib::DNewLn << kCIDLib::FlushIt;
}


tCIDLib::TVoid
TMEngStrmPrsErrHandler::ParseException( const   tCIDLib::TCard4 c4LineNum
                                        , const tCIDLib::TCard4 c4ColNum
                                        , const TString&        strClassPath)
{
    // Save the target stream format then reset it to defaults
    TStreamJanitor janStream(m_pstrmTarget);
    m_pstrmTarget->SetDefaultFormat();

    *m_pstrmTarget  << L"(EXCEPTION: " << strClassPath << L", "
                    << c4LineNum << L"/" << c4ColNum
                    << L")\n  Unknown exception error\n\n"
                    << kCIDLib::FlushIt;
}


// ---------------------------------------------------------------------------
//  TMEngStrmPrsErrHandler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStrmPrsErrHandler::SetStream(TTextOutStream* const pstrmToUse)
{
    m_pstrmTarget = pstrmToUse;
}


