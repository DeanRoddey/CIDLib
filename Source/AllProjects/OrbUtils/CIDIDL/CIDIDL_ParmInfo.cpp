//
// FILE NAME: CIDIDL_ParmaInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/10/2004
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
//  This file implements the method parameter info class.
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
RTTIDecls(TCGenTypeInfo,TObject)
RTTIDecls(TCGenMethodParm,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCGenTypeInfo
//  PREFIX: tInfo
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCGenTypeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TCGenTypeInfo::TCGenTypeInfo()
{
}

TCGenTypeInfo::TCGenTypeInfo(const TXMLTreeElement& xtnodeType)
{
    Set(xtnodeType);
}

TCGenTypeInfo::~TCGenTypeInfo()
{
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TCGenTypeInfo::strAuxType() const
{
    return m_strAuxType;
}


const TString& TCGenTypeInfo::strKeyExtract() const
{
    return m_strKeyExtract;
}


const TString& TCGenTypeInfo::strKeyOps() const
{
    return m_strKeyOps;
}


const TString& TCGenTypeInfo::strKeyType() const
{
    return m_strKeyType;
}


const TString& TCGenTypeInfo::strModulus() const
{
    return m_strModulus;
}


const TString& TCGenTypeInfo::strType() const
{
    return m_strType;
}


tCIDLib::TVoid TCGenTypeInfo::Set(const TXMLTreeElement& xtnodeType)
{
    m_strType = xtnodeType.strQName();
    m_strAuxType.Clear();
    m_strKeyExtract.Clear();
    m_strKeyOps.Clear();
    m_strKeyType.Clear();
    m_strModulus.Clear();

    if (m_strType == L"CIDIDL:Object")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:Type").strValue();
    }
     else if (m_strType == L"CIDIDL:Enumerated")
    {
        // Get the type attribute out, which is the actual type
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:Type").strValue();
    }
     else if (m_strType == L"CIDIDL:TBag")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
    }
     else if (m_strType == L"CIDIDL:THashSet")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
        m_strKeyOps = xtnodeType.xtattrNamed(L"CIDIDL:KeyOps").strValue();
        m_strModulus = xtnodeType.xtattrNamed(L"CIDIDL:Modulus").strValue();
    }
     else if (m_strType == L"CIDIDL:TKeyedHashSet")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
        m_strKeyType = xtnodeType.xtattrNamed(L"CIDIDL:KeyType").strValue();
        m_strKeyOps = xtnodeType.xtattrNamed(L"CIDIDL:KeyOps").strValue();
        m_strModulus = xtnodeType.xtattrNamed(L"CIDIDL:Modulus").strValue();
        m_strKeyExtract = xtnodeType.xtattrNamed(L"CIDIDL:KeyExtract").strValue();
    }
     else if (m_strType == L"CIDIDL:TVector")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
    }
     else if (m_strType == L"CIDIDL:TFundArray")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
    }
     else if (m_strType == L"CIDIDL:TFundVector")
    {
        m_strAuxType = xtnodeType.xtattrNamed(L"CIDIDL:ElemType").strValue();
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TCGenMethodParm
//  PREFIX: mparm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCGenMethodParm: Constructors and Destructor
// ---------------------------------------------------------------------------
TCGenMethodParm::TCGenMethodParm(const  TString&            strMethodName
                                , const TXMLTreeElement&    xtnodeParm)
{
    m_strName = xtnodeParm.xtattrNamed(L"CIDIDL:Name").strValue();

    // There may be default value
    tCIDLib::TCard4 c4Index;
    if (xtnodeParm.bAttrExists(L"CIDIDL:DefVal", c4Index))
        m_strDefVal = xtnodeParm.xtattrAt(c4Index).strValue();

    // Get the direction value attribute, and map to the enum type
    const TString& strDir = xtnodeParm.xtattrNamed(L"CIDIDL:Dir").strValue();
    tCIDLib::EParmDirs eDir;
    if (strDir == L"In")
    {
        m_eDir = tCIDLib::EParmDirs::In;
    }
     else if (strDir == L"Out")
    {
        m_eDir = tCIDLib::EParmDirs::Out;
    }
     else if (strDir == L"InOut")
    {
        m_eDir = tCIDLib::EParmDirs::InOut;
    }
     else
    {
        // Make the compiler happy
        eDir = tCIDLib::EParmDirs::In;

        facCIDIDL.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kIDLErrs::errcGen_InvalidParmDir
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strMethodName
        );
    }

    // Get the 0th child which is a our type, and set it
    m_tinfoThis.Set(xtnodeParm.xtnodeChildAtAsElement(0));
}

TCGenMethodParm::~TCGenMethodParm()
{
}


// ---------------------------------------------------------------------------
//  TCGenMethodParm: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EParmDirs TCGenMethodParm::eDir() const
{
    return m_eDir;
}


const TString& TCGenMethodParm::strDefVal() const
{
    return m_strDefVal;
}


const TString& TCGenMethodParm::strName() const
{
    return m_strName;
}


const TCGenTypeInfo& TCGenMethodParm::tinfoThis() const
{
    return m_tinfoThis;
}
