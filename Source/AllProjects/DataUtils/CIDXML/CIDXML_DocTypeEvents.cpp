//
// FILE NAME: CIDXML_DocTypeEvents.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/21/2000
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
//  This file provides the default implementations of the document type
//  events. Providing these allows derived classes to just override the events
//  that they really want to deal with.
//
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
//  MXMLDTDEvents: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
MXMLDTDEvents::AttrDef(const TDTDAttrDef&, const tCIDLib::TBoolean)
{
}


tCIDLib::TVoid
MXMLDTDEvents::DocType(const TDTDElemDecl&, const TString&, const TString&)
{
}


tCIDLib::TVoid MXMLDTDEvents::DocTypeComment(const TString&)
{
}


tCIDLib::TVoid
MXMLDTDEvents::DocTypePI(const  TString&
                        , const TString&
                        , const tCIDXML::ELocations)
{
}


tCIDLib::TVoid MXMLDTDEvents::DocTypeWS(const TString&)
{
}


tCIDLib::TVoid
MXMLDTDEvents::ElementDecl(const TDTDElemDecl& , const tCIDLib::TBoolean)
{
}


tCIDLib::TVoid
MXMLDTDEvents::EndAttList(const TDTDElemDecl&)
{
}


tCIDLib::TVoid MXMLDTDEvents::EndExtSubset()
{
}


tCIDLib::TVoid MXMLDTDEvents::EndIntSubset()
{
}


tCIDLib::TVoid
MXMLDTDEvents::EntityDecl(const TDTDEntityDecl&, const tCIDLib::TBoolean)
{
}


tCIDLib::TVoid MXMLDTDEvents::EndDocType()
{
}


tCIDLib::TVoid
MXMLDTDEvents::NotationDecl(const TDTDNotationDecl&, const tCIDLib::TBoolean)
{
}


tCIDLib::TVoid MXMLDTDEvents::ResetDocType()
{
}


tCIDLib::TVoid MXMLDTDEvents::StartAttList(const TDTDElemDecl&)
{
}


tCIDLib::TVoid MXMLDTDEvents::StartExtSubset()
{
}


tCIDLib::TVoid MXMLDTDEvents::StartIntSubset()
{
}


tCIDLib::TVoid MXMLDTDEvents::TextDecl(const TString&, const TString&)
{
}
