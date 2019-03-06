//
// FILE NAME: CIDComm_PortFactory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the abstract com port factory class. There's not
//  much to it since most of the interface is virtual and handled in the
//  derived classes.
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TComPortFactory,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TComPortFactory
// PREFIX: cfact
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TComPortFactory: Destructor
// ---------------------------------------------------------------------------
TComPortFactory::~TComPortFactory()
{
}


// ---------------------------------------------------------------------------
//  TComPortFactory: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TComPortFactory::bCanConfigure() const
{
    return m_bCanConfigure;
}


tCIDLib::TBoolean
TComPortFactory::bOwnsThisPort(const TString& strPath) const
{
    // Let the derived class validate the path
    return bValidatePath(strPath);
}


const TString& TComPortFactory::strId() const
{
    return m_strId;
}


const TString& TComPortFactory::strNamespace() const
{
    return m_strNamespace;
}


// ---------------------------------------------------------------------------
//  TComPortFactory: Hidden Constructors
// ---------------------------------------------------------------------------
TComPortFactory::TComPortFactory(const  TString&            strId
                                , const TString&            strNamespace
                                , const tCIDLib::TBoolean   bCanConfigure) :

    m_bCanConfigure(bCanConfigure)
    , m_strId(strId)
    , m_strNamespace(strNamespace)
{
}


// ---------------------------------------------------------------------------
//  TComPortFactory: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TComPortFactory::SetNamespace(const TString& strNamespace)
{
    m_strNamespace = strNamespace;
}

