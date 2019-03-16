//
// FILE NAME: CIDXML_ParserException.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/1999
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
//  This file implements the TXMLException class.
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
#include    "CIDXML_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLException,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLException
// PREFIX: xexc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLException: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLException::TXMLException(const  tCIDLib::TErrCode   errcThrow
                            , const TString&            strMessage
                            , const TString&            strEntityName
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCard4     c4Column) :
    m_c4Column(c4Column)
    , m_c4Line(c4Line)
    , m_errcThrown(errcThrow)
    , m_strEntityName(strEntityName)
    , m_strMessage(strMessage)
{
}

TXMLException::TXMLException(const TXMLException& xexcToCopy) :

    m_c4Column(xexcToCopy.m_c4Column)
    , m_c4Line(xexcToCopy.m_c4Line)
    , m_errcThrown(xexcToCopy.m_errcThrown)
    , m_strEntityName(xexcToCopy.m_strEntityName)
    , m_strMessage(xexcToCopy.m_strMessage)
{
}

TXMLException::~TXMLException()
{
}


// ---------------------------------------------------------------------------
//  TXMLException: Public operator
// ---------------------------------------------------------------------------
TXMLException& TXMLException::operator=(const TXMLException& xexcToAssign)
{
    if (this == &xexcToAssign)
        return *this;

    m_c4Column      = xexcToAssign.m_c4Column;
    m_c4Line        = xexcToAssign.m_c4Line;
    m_errcThrown    = xexcToAssign.m_errcThrown;
    m_strEntityName = xexcToAssign.m_strEntityName;
    m_strMessage    = xexcToAssign.m_strMessage;

    return *this;
}


// ---------------------------------------------------------------------------
//  TXMLException: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TXMLException::c4Column() const
{
    return m_c4Column;
}


tCIDLib::TCard4 TXMLException::c4Line() const
{
    return m_c4Line;
}


const TString& TXMLException::strEntityName() const
{
    return m_strEntityName;
}


const TString& TXMLException::strMessage() const
{
    return m_strMessage;
}



// ---------------------------------------------------------------------------
//  TXMLException: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLException::FormatTo(TTextOutStream& strmDest) const
{
    strmDest    << facCIDXML().strMsg(kXMLMsgs::midGen_EntityName)
                << L":" << m_strEntityName
                << L"  " << facCIDXML().strMsg(kXMLMsgs::midGen_LineCol)
                << L":" << m_c4Line << L"/" << m_c4Column
                << L" - " << m_strMessage;
}


