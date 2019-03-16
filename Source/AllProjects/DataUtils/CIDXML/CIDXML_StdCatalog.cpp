//
// FILE NAME: CIDXML_StdCatalog.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2001
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
//  This file implements the TXMLStdCatalog class.
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
RTTIDecls(TXMLStdCatalog,TXMLCatalog)


// ---------------------------------------------------------------------------
// A simple macro to assert that an element has the expected name
// ---------------------------------------------------------------------------
#if CID_DEBUG_ON
#define AssertElemName(xtn,nam) \
if (xtn.strQName() != nam) \
{ \
    facCIDXML().ThrowErr \
    ( \
        CID_FILE \
        , CID_LINE \
        , kXMLErrs::errcSCat_ExpectedElem \
        , tCIDLib::ESeverities::Failed \
        , tCIDLib::EErrClasses::Format \
        , TString(nam) \
        , xtn.strQName() \
    ); \
}
#else
#define AssertElemName(xtn,nam) {}
#endif


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDXML_StdCatalog
{
    // -----------------------------------------------------------------------
    //  Our fixed DTD that we fault into all standard catalogs being parsed
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszFixedDTD =
                        L"<?xml encoding='$NativeWideChar$'?>\n"
                        L"<!ELEMENT CIDStdCat:Catalog (CIDStdCat:MapItem*)>\n"
                        L"<!ELEMENT CIDStdCat:MapItem EMPTY>\n"
                        L"<!ATTLIST CIDStdCat:MapItem\n"
                        L"          CIDStdCat:PublicId CDATA #REQUIRED\n"
                        L"          CIDStdCat:MapTo CDATA #REQUIRED>\n";
}



// ---------------------------------------------------------------------------
//  Local helper procedures
// ---------------------------------------------------------------------------
static const TString& strGetEntitySrcKey(const tCIDXML::TEntitySrcRef& esrSrc)
{
    return esrSrc->strPublicId();
}


// ---------------------------------------------------------------------------
//  CLASS: TXMLStdCatalog
// PREFIX: xcat
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLStdCatalog: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLStdCatalog::TXMLStdCatalog() :

    m_colIdMap(109, new TStringKeyOps, strGetEntitySrcKey)
{
}

TXMLStdCatalog::TXMLStdCatalog(tCIDXML::TEntitySrcRef& esrToParse) :

    m_colIdMap(109, new TStringKeyOps, strGetEntitySrcKey)
{
    //
    //  Parse the source and load our collection. Throws if it fails. If will
    //  allocate the collection if all goes well.
    //
    Initialize(esrToParse);
}

TXMLStdCatalog::~TXMLStdCatalog()
{
    // Flush the collection
    m_colIdMap.RemoveAll();
}


// ---------------------------------------------------------------------------
//  TXMLStdCatalog: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLStdCatalog::AddMapping(const TXMLEntitySrc* const pxsrcToAdopt)
{
    m_colIdMap.objAdd(tCIDXML::TEntitySrcRef(pxsrcToAdopt));
}

tCIDLib::TVoid TXMLStdCatalog::AddMapping(tCIDXML::TEntitySrcRef& esrToSet)
{
    m_colIdMap.objAdd(esrToSet);
}


// A convenience to just check for a public id mapping to exist
tCIDLib::TBoolean
TXMLStdCatalog::bPublicMappingExists(const TString& strPublicId) const
{
    return (m_colIdMap.pobjFindByKey(strPublicId) != 0);
}


// Return the number of mappings we have
tCIDLib::TCard4 TXMLStdCatalog::c4MapCount() const
{
    return m_colIdMap.c4ElemCount();
}


// Return a entity source ref for the public id if it exists
tCIDXML::TEntitySrcRef
TXMLStdCatalog::esrMapId(const TString& strPublicId, const TString&) const
{
    //
    //  See if we can find it. If not, return an entity source ref with a
    //  zero pointer.
    //
    const tCIDXML::TEntitySrcRef* esrFound = m_colIdMap.pobjFindByKey(strPublicId);
    if (!esrFound)
        return tCIDXML::TEntitySrcRef(0);

    //
    //  Return a copy, which gives them their own counted ref to the entity
    //  source.
    //
    return *esrFound;
}


// Clear out the catalog
tCIDLib::TVoid TXMLStdCatalog::RemoveAll()
{
    // Just flush the collection
    m_colIdMap.RemoveAll();
}


// Remove a specific public id mapping
tCIDLib::TVoid TXMLStdCatalog::RemoveMapping(const TString& strId)
{
    m_colIdMap.bRemoveKey(strId);
}


// ---------------------------------------------------------------------------
//  TXMLStdCatalog: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLStdCatalog::Initialize(tCIDXML::TEntitySrcRef& esrToParse)
{
    // Create a parser to use here, and put a janitor on it
    TXMLTreeParser* pxtprsCat = new TXMLTreeParser;
    TJanitor<TXMLTreeParser> janParser(pxtprsCat);

    //
    //  Set a mapping on the parser. It will, interestingly enough, create an
    //  instance of this very class and put this entry into it. We give the
    //  magic id that the user is told to put into standard cat XML sources.
    //  The entity source must have one too, but its just for user consumption
    //  for a memory buffer anyway.
    //
    pxtprsCat->AddMapping
    (
        new TMemBufEntitySrc
        (
            TString(L"http://www.charmedquark.com/XML/StdCatDTD")
            , kCIDXML::pszURNStdCatDTD
        )
    );

    //
    //  Try to parse the input. We only care about tags, since we don't have
    //  any data as character data.
    //
    const tCIDLib::TBoolean bOk = pxtprsCat->bParseRootEntity
    (
        esrToParse
        , tCIDXML::EParseOpts::Validate
        , tCIDXML::EParseFlags(tCIDXML::EParseFlags::Tags)
    );

    if (!bOk)
    {
        //
        //  Get the first error that occured, and put that info into a TError
        //  object and throw it.
        //
        const TXMLTreeParser::TErrInfo& erriFirst = pxtprsCat->colErrors()[0];

        //
        //  Format the info so we can send it as the aux text in the error
        //  we are about to throw.
        //
        TTextStringOutStream strmTmp(128UL);
        strmTmp << L"[" << erriFirst.strSystemId() << L"/"
                << erriFirst.c4Line() << L"." << erriFirst.c4Column()
                << L"] " << erriFirst.strText() << kCIDLib::FlushIt;

        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcSCat_SrcParseFailed
            , strmTmp.strData()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  Lets go through the data and use it to fill in our info. We start by
    //  getting the document and then the root elements.
    //
    const TXMLTreeElement& xtnodeRoot = pxtprsCat->xtdocThis().xtnodeRoot();
    AssertElemName(xtnodeRoot, L"CIDStdCat:Catalog")

    //
    //  Ok, and the root node should just be a bunch of map nodes, so we just
    //  iterate the child nodes and get the two attributes of each that set
    //  the public and system ids.
    //
    const tCIDLib::TCard4 c4Count = xtnodeRoot.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // Get the current child element
        const TXMLTreeElement& xtnodeItem
                            = xtnodeRoot.xtnodeChildAtAsElement(c4Index);
        AssertElemName(xtnodeItem, L"CIDStdCat:MapItem");

        // Its our guy, so get the values of the two parameters
        const TString& strKey
                = xtnodeItem.xtattrNamed(L"CIDStdCat::PublicId").strValue();
        const TString& strValue
                = xtnodeItem.xtattrNamed(L"CIDStdCat::MapTo").strValue();

        //
        //  And add an element to our collection for this guy. Make the value
        //  the system id and the key the public id.
        //
        m_colIdMap.objAdd
        (
            tCIDXML::TEntitySrcRef(new TFileEntitySrc(strValue, strKey))
        );
    }
}

