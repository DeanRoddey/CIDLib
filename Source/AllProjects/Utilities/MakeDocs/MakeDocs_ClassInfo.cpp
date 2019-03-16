//
// FILE NAME: MakeDocs_ClassInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1998
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
//  This module implements the TClassDocInfo class, which is a class used to
//  store information on each class.
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


// ----------------------------------------------------------------------------
//  CLASS: TClassDocInfo
// PREFIX: cldi
// ----------------------------------------------------------------------------
TClassDocInfo::TClassDocInfo()
{
    CommonInit();
}

TClassDocInfo::TClassDocInfo(const TString& strClassName) :

    m_strClassName(strClassName)
{
    CommonInit();
}

TClassDocInfo::TClassDocInfo(const TClassDocInfo& cldiToCopy) :

    m_fcolCategories(cldiToCopy.m_fcolCategories)
    , m_strClassName(cldiToCopy.m_strClassName)
    , m_strParentClass(cldiToCopy.m_strParentClass)
{
}

TClassDocInfo::~TClassDocInfo()
{
}


// ----------------------------------------------------------------------------
//  TClassDocInfo: Private, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TClassDocInfo::CommonInit()
{
    tMakeDocs::EClassCats eCats = tMakeDocs::EClassCats::Min;
    while (eCats <= tMakeDocs::EClassCats::Max)
        m_fcolCategories[eCats++] = kCIDLib::False;
}
