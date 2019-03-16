//
// FILE NAME: CIDXML_EntitySrc.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2001
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
//  This file implements the TXMLEntitySrc class, which is the base class from
//  which other entity source classes are derived.
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
RTTIDecls(TXMLEntitySrc,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLEntitySrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLEntitySrc::~TXMLEntitySrc()
{
}


// ---------------------------------------------------------------------------
//  TXMLEntitySrc: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TXMLEntitySrc::strEncoding() const
{
    return m_strEncoding;
}

const TString& TXMLEntitySrc::strEncoding(const TString& strEncodingToForce)
{
    m_strEncoding = strEncodingToForce;
    return m_strEncoding;
}


const TString& TXMLEntitySrc::strPublicId() const
{
    return m_strPublicId;
}

const TString& TXMLEntitySrc::strPublicId(const TString& strToSet)
{
    m_strPublicId = strToSet;
    return m_strPublicId;
}


const TString& TXMLEntitySrc::strSystemId() const
{
    return m_strSystemId;
}

const TString& TXMLEntitySrc::strSystemId(const TString& strToSet)
{
    m_strSystemId = strToSet;
    return m_strSystemId;
}


// ---------------------------------------------------------------------------
//  TXMLEntitySrc: Hidden constructors and operators
// ---------------------------------------------------------------------------
TXMLEntitySrc& TXMLEntitySrc::operator=(const TXMLEntitySrc& xsrcToAssign)
{
    if (this == &xsrcToAssign)
        return *this;

    m_strEncoding = xsrcToAssign.m_strEncoding;
    m_strPublicId = xsrcToAssign.m_strSystemId;
    m_strSystemId = xsrcToAssign.m_strSystemId;
    return *this;
}


// ---------------------------------------------------------------------------
//  TXMLEntitySrc: Hidden constructors and operators
// ---------------------------------------------------------------------------
TXMLEntitySrc::TXMLEntitySrc()
{
}

TXMLEntitySrc::TXMLEntitySrc(const TString& strSystemId) :

    m_strSystemId(strSystemId)
{
}

TXMLEntitySrc::TXMLEntitySrc(const  TString& strSystemId
                            , const TString& strPublicId) :

    m_strPublicId(strPublicId)
    , m_strSystemId(strSystemId)
{
}

TXMLEntitySrc::TXMLEntitySrc(const TXMLEntitySrc& xsrcToCopy) :

    m_strEncoding(xsrcToCopy.m_strEncoding)
    , m_strPublicId(xsrcToCopy.m_strPublicId)
    , m_strSystemId(xsrcToCopy.m_strSystemId)
{
}


