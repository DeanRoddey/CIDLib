//
// FILE NAME: CIDLib_UserContenxt.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/27/2011
//
// COPYRIGHT: 2011..2011, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This is the implementation file for the user context class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDUserCtx,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCIDUserCtx
//  PREFIX: cuctx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDUserCtx: Destructor
// ---------------------------------------------------------------------------
TCIDUserCtx::~TCIDUserCtx()
{
}


// ---------------------------------------------------------------------------
//  TCIDUserCtx: Hidden Constructors and Operators
// ---------------------------------------------------------------------------
TCIDUserCtx::TCIDUserCtx()
{
}

TCIDUserCtx::TCIDUserCtx(const TString& strUserName, const TString& strUserType) :

    m_strUserName(strUserName)
    , m_strUserType(strUserType)
{
}


// ---------------------------------------------------------------------------
//  TCIDUserCtx: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDUserCtx::Reset()
{
    m_strUserName.Clear();
    m_strUserType.Clear();
}


// ---------------------------------------------------------------------------
//  TCIDUserCtx: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Read only access to the user name and type
const TString& TCIDUserCtx::strUserName() const
{
    return m_strUserName;
}

const TString& TCIDUserCtx::strUserType() const
{
    return m_strUserName;
}



// ---------------------------------------------------------------------------
//  TCIDUserCtx: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Allow the derived class to update us
tCIDLib::TVoid
TCIDUserCtx::SetUserInfo(const TString& strUserName, const TString& strUserType)
{
    m_strUserName = strUserName;
    m_strUserType = strUserType;
}

