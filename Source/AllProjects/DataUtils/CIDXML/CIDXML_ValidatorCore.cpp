//
// FILE NAME: CIDXML_ValidatorCore.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
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
//  This file implements the TXMLValidator class.
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
RTTIDecls(TXMLValidator,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TXMLValidator
// PREFIX: xval
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLValidator: Destructor
// ---------------------------------------------------------------------------
TXMLValidator::~TXMLValidator()
{
}


// ---------------------------------------------------------------------------
//  TXMLValidator: Hidden constructors
// ---------------------------------------------------------------------------
TXMLValidator::TXMLValidator(TXMLParserCore* const pxprsOwner) :

    m_pxbmOwner(0)
    , m_pxemOwner(0)
    , m_pxprsOwner(pxprsOwner)
{
    // Get out our convenience pointers
    m_pxbmOwner = &pxprsOwner->xbmThis();
    m_pxemOwner = &pxprsOwner->xemThis();
}
