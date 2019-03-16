//
// FILE NAME: CIDXML.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDXML)
#define CIDXMLEXP   CID_DLLEXPORT
#else
#define CIDXMLEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  Our public constants namespace
// ---------------------------------------------------------------------------
namespace kCIDXML
{
    // -----------------------------------------------------------------------
    //  These are constants used by the XML reader class
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4RawBufSize    = 32 * 1024;
    const tCIDLib::TCard4       c4CharBufSize   = 4096;


    // -----------------------------------------------------------------------
    //  These are the character flags used with the TXMLCharFlags class
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1       c1BaseChar          = 0x1;
    const tCIDLib::TCard1       c1SpecialCharData   = 0x2;
    const tCIDLib::TCard1       c1NameChar          = 0x4;
    const tCIDLib::TCard1       c1MarkupTestChar    = 0x8;
    const tCIDLib::TCard1       c1StartTagChar      = 0x10;
    const tCIDLib::TCard1       c1LetterChar        = 0x20;
    const tCIDLib::TCard1       c1XMLChar           = 0x40;
    const tCIDLib::TCard1       c1WhitespaceChar    = 0x80;


    // -----------------------------------------------------------------------
    //  Some id constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4InvalidElemId     = kCIDLib::c4MaxCard;
    const tCIDLib::TCard4       c4PCDataElemId      = kCIDLib::c4MaxCard - 1;


    // -----------------------------------------------------------------------
    //  Some general constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszXMLVersion       = L"1.0";
    const tCIDLib::TCard4       c4XMLMajVersion     = 1;
    const tCIDLib::TCard4       c4XMLMinVersion     = 0;


    // -----------------------------------------------------------------------
    //  The magic URN that is used by the standard catalog class. It uses an
    //  XML file as its source, and those files should use this as the public
    //  id in their DOCTYPE statements. The system id will be ignored.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszURNStdCatDTD     = L"urn:charmedquark.com:XML.StdCatDTD";
}


// ---------------------------------------------------------------------------
//  These are aliases to make dealing with the simple pool template class more
//  convenient. We are using a pre-fab variation of the pool that does what we
//  want, which is maintain a pool of strings. The criteria is just a desired
//  string buffer length.
//
//  The prefixes are xbm and jan.
// ---------------------------------------------------------------------------
using TXMLBufMgr = TStringPool;
using TXMLBufJan = TSimplePoolJan<TString>;


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDXML_ErrorIds.hpp"
#include    "CIDXML_Type.hpp"

#include    "CIDXML_ThisFacility.hpp"

#include    "CIDXML_NameIDPool.hpp"
#include    "CIDXML_EntitySrc.hpp"
#include    "CIDXML_MemBufEntitySrc.hpp"
#include    "CIDXML_FileEntitySrc.hpp"

#include    "CIDXML_Attr.hpp"
#include    "CIDXML_CharFlags.hpp"
#include    "CIDXML_ContentModel.hpp"
#include    "CIDXML_ChildContentModel.hpp"
#include    "CIDXML_MixedContentModel.hpp"
#include    "CIDXML_AttrDef.hpp"
#include    "CIDXML_ElementDecl.hpp"
#include    "CIDXML_EntityDecl.hpp"
#include    "CIDXML_NotationDecl.hpp"
#include    "CIDXML_XMLException.hpp"
#include    "CIDXML_ValidatorCore.hpp"

#include    "CIDXML_DTDAttrDef.hpp"
#include    "CIDXML_DTDElementDecl.hpp"
#include    "CIDXML_DTDEntityDecl.hpp"
#include    "CIDXML_DTDNotationDecl.hpp"
#include    "CIDXML_DocTypeEvents.hpp"
#include    "CIDXML_DTDValidator.hpp"

#include    "CIDXML_DocumentEvents.hpp"
#include    "CIDXML_EntityEvents.hpp"
#include    "CIDXML_ErrorEvents.hpp"

#include    "CIDXML_EntityResolver.hpp"
#include    "CIDXML_XMLCatalog.hpp"
#include    "CIDXML_StdCatalog.hpp"

#include    "CIDXML_EntitySpooler.hpp"
#include    "CIDXML_EntityMgr.hpp"
#include    "CIDXML_ContextStack.hpp"
#include    "CIDXML_ParserCore.hpp"

#include    "CIDXML_SimpleTree.hpp"
#include    "CIDXML_SimpleTreeParser.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDXMLEXP TFacCIDXML& facCIDXML();


// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDXML_PREINST)
extern template class TCntPtr<const TXMLEntitySrc>;
extern template class TKeyedHashSet<tCIDXML::TEntitySrcRef, TString, TStringKeyOps>;
extern template class TXMLNameIDPool<TDTDAttrDef>;
extern template class TXMLNameIDPool<TDTDElemDecl>;
extern template class TXMLNameIDPool<TDTDEntityDecl>;
extern template class TXMLNameIDPool<TDTDNotationDecl>;
extern template class TXMLNameIDPoolCursor<TDTDAttrDef>;
extern template class TXMLNameIDPoolCursor<TDTDElemDecl>;
extern template class TXMLNameIDPoolCursor<TDTDEntityDecl>;
extern template class TXMLNameIDPoolCursor<TDTDNotationDecl>;
extern template class TRefVector<TXMLTreeAttr>;
extern template class TRefVector<TXMLTreeElement>;
extern template class TRefVector<TXMLTreeNode>;
extern template class TRefVector<TXMLTreeText>;
extern template class TSimplePoolJan<TString>;
extern template class TVector<TXMLAttr>;
extern template class TVector<TXMLTreeParser::TErrInfo>;
#endif

