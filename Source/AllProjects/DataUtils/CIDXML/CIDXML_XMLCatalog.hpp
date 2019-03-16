//
// FILE NAME: CIDXML_XMLCatalog.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2001
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
//  This is the header file for the CIDXML_XMLCatlog.Cpp file, which
//  implements the TXMLCatalog class. This is a base class from which all
//  XML catalogs are derived. Catalogs are used to map ids to entity sources,
//  and are used in conjunction with entity resolvers to provide automatic
//  mapping of ids during parsing. Various versions, which represent the
//  mappings in different ways and get them from different places can be
//  derived from this class.
//
//  Entity sources are managed via counted pointers to the base class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TXMLCatalog
// PREFIX: xcat
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLCatalog : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TXMLCatalog();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AddMapping
        (
            const   TXMLEntitySrc* const    pxsrcToAdopt
        ) = 0;

        virtual tCIDLib::TVoid AddMapping
        (
                    tCIDXML::TEntitySrcRef& esrToAdd
        ) = 0;

        virtual tCIDLib::TBoolean bPublicMappingExists
        (
            const   TString&                strPublicId
        )   const = 0;

        virtual tCIDLib::TCard4 c4MapCount() const = 0;

        virtual tCIDXML::TEntitySrcRef esrMapId
        (
            const   TString&                strPublicId
            , const TString&                strSystemId
        )   const = 0;

        virtual tCIDLib::TVoid RemoveAll() = 0;

        virtual tCIDLib::TVoid RemoveMapping
        (
            const   TString&                strId
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors and operators
        // -------------------------------------------------------------------
        TXMLCatalog();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TXMLCatalog(const TXMLCatalog&);
        TXMLCatalog& operator=(const TXMLCatalog&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLCatalog,TObject)
};

#pragma CIDLIB_POPPACK

