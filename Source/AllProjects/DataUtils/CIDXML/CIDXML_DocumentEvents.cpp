//
// FILE NAME: CIDXML_DocumentEvents.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/04/1999
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
//  This file provides the default implementations of most of the virtual
//  callback methods. We don't want every derivative to have to provide
//  overrides of lots and lots of methods they don't care about.
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
//  MXMLDocEvents: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
MXMLDocEvents::DocCharacters(const  TString&
                            , const tCIDLib::TBoolean
                            , const tCIDLib::TBoolean
                            , const tCIDXML::ELocations)
{
}

tCIDLib::TVoid
MXMLDocEvents::DocComment(const TString&, const tCIDXML::ELocations)
{
}


tCIDLib::TVoid MXMLDocEvents::EndDocument(const TXMLEntitySrc&)
{
}


tCIDLib::TVoid MXMLDocEvents::EndTag(const TXMLElemDecl&)
{
}


tCIDLib::TVoid
MXMLDocEvents::DocPI(const TString&, const TString&, const tCIDXML::ELocations)
{
}


tCIDLib::TVoid MXMLDocEvents::StartDocument(const TXMLEntitySrc&)
{
}


tCIDLib::TVoid
MXMLDocEvents::StartTag(        TXMLParserCore&
                       , const  TXMLElemDecl&
                        , const tCIDLib::TBoolean
                        , const TVector<TXMLAttr>&
                        , const tCIDLib::TCard4)
{
}


tCIDLib::TVoid
MXMLDocEvents::XMLDecl(const TString&, const TString&, const TString&)
{
}

