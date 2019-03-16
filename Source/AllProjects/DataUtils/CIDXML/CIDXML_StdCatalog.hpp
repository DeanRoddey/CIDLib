//
// FILE NAME: CIDXML_StdCatalog.hpp
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
//  This is the header file for the CIDXML_StdCatlog.Cpp file, which
//  implements the TXMLStdCatalog class. This is an implementation of the
//  TXMLCatalog interface, which is the generic interface by which parsers
//  interface to catalogs.
//
//  This particular catalog only maps public ids, not system ids. Catalogs
//  can map either, but this one is designed for mapping public ids which are
//  specifically designed to support this kind of thing.
//
//  This implementation optionally accepts an XML file which contains a list
//  of mappings. It will parse this file and use it to load up the map. But
//  you can also just add them yourself manually. If you use this mechanism,
//  it can only handle mapping to fully qualified file based entities, so if
//  you don't give a fully qualfied entry that represents a file that exists,
//  it will fail.
//
//  Most parsers will, by default, create one of these catalogs if you just
//  start adding mappings to it. If you want some other kind of catalog, you
//  should create one and install it on the parser.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that we internally have to use mappings, because we have an
//      internally hard coded DTD for the standard catalog XML format. The
//      client file should use the public id:
//
//          "urn:charmedquark.com:XML.StdCatDTD"
//
//      We will add a mapping to the parser instance we use, and tell it to
//      map that to a memory buffer that holds our hard coded DTD.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLStdCatalog
// PREFIX: xcat
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLStdCatalog : public TXMLCatalog
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLStdCatalog();

        TXMLStdCatalog
        (
                    tCIDXML::TEntitySrcRef& esrToParse
        );

        TXMLStdCatalog(const TXMLStdCatalog&) = delete;

        ~TXMLStdCatalog();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLStdCatalog& operator=(const TXMLStdCatalog&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMapping
        (
            const   TXMLEntitySrc* const    pxsrcToAdopt
        );

        tCIDLib::TVoid AddMapping
        (
                    tCIDXML::TEntitySrcRef& esrToAdd
        );

        tCIDLib::TBoolean bPublicMappingExists
        (
            const   TString&                strPublicId
        )   const;

        tCIDLib::TCard4 c4MapCount() const;

        tCIDXML::TEntitySrcRef esrMapId
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
        )   const;

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveMapping
        (
            const   TString&                strId
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TIdMapCol = TKeyedHashSet<tCIDXML::TEntitySrcRef,TString,TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize
        (
                    tCIDXML::TEntitySrcRef& esrToParse
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colIdMap
        //      A ref hash map used to store the entity sources. The base
        //      class makes us store them as counted pointers to the base
        //      entity source class.
        // -------------------------------------------------------------------
        TIdMapCol   m_colIdMap;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLStdCatalog,TXMLCatalog)
};

#pragma CIDLIB_POPPACK

