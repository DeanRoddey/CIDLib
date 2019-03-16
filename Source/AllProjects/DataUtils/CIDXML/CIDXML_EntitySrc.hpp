//
// FILE NAME: CIDXML_EntitySrc.hpp
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
//  This is the header file for the CIDXML_EntitySrc.Cpp file, which
//  implements the TXMLEntitySrc class. This class represents the source
//  specification for some entity. It contains a string that indicates some
//  sort of 'path' to the source for an entity. That 'path' might be a real
//  local file path, a URL, the name of a shared memory buffer, etc... It is
//  not looked at here, just stored.
//
//  It can also store another id, which is usually a 'public id', i.e. an
//  abstracted identifier that is mostly used as keys to store them in
//  catalogs for indirect reference.
//
//  Derived classes are mainly responsible for returning a binary input stream
//  for the input source. Derived classes of this class are designed to
//  understand the specifics of creating a stream for the type of source they
//  represent. Generally, there are these types:
//
// CAVEATS/GOTCHAS:
//
//  1)  Technically some sources will not have a reasonable system id, such
//      as a memory buffer based source. But, since we have to have something
//      the client code must provide some identifying id. It will still be
//      effective for error reporting.
//
//  2)  You can force the parser to assume that the entity is in a particular
//      encoding, by calling strEncoding() to set an encoding name that will
//      be forced. If you are wrong, it will probably fail spectacularly. But
//      in some cases its known and conflicts with the encoding="" string
//      (such as when its transcoded along the way by some gateway that knows
//      nothing of XML.)
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLEntitySrc : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TXMLEntitySrc();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual TBinInStream* pstrmMakeNew() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strEncoding() const;

        const TString& strEncoding
        (
            const   TString&                strEncodingToForce
        );

        const TString& strPublicId() const;

        const TString& strPublicId
        (
            const   TString&                strToSet
        );

        const TString& strSystemId() const;

        const TString& strSystemId
        (
            const   TString&                strSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TXMLEntitySrc();

        TXMLEntitySrc
        (
            const   TXMLEntitySrc&          xsrcToCopy
        );

        TXMLEntitySrc
        (
            const   TString&                strSystemId
        );

        TXMLEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
        );

        TXMLEntitySrc& operator=
        (
            const   TXMLEntitySrc&          xsrcToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strEncoding
        //      It is possible to force the core parser to assume that the
        //      data from a particular entity is in a particular encoding.
        //      By default this is empty and the encoding is autosensed.
        //
        //  m_strPublicId
        //      A non-literal id, which is optional but usually used to do
        //      indirect referencing from a symbolic id to a real id (which
        //      will be in the system id field.)
        //
        //  m_strSystemId
        //      This is the system id provided. It is required, though what
        //      it means is really up to the derived class. It could be a
        //      URI or a file path, etc...
        // -------------------------------------------------------------------
        TString m_strEncoding;
        TString m_strPublicId;
        TString m_strSystemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLEntitySrc,TObject)
};


#pragma CIDLIB_POPPACK


