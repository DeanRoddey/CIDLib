//
// FILE NAME: CIDNet_URLEntitySrc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This is the header file for the CIDNet_XMLURLEntitySrc.Cpp file, which
//  implements the TURLEntitySrc class. This is a derivative of the XML parser's
//  base entity source class which provides support for URL based sources.
//
//  We set the system id (on our parent class) to the full encoded URL,
//  without username/password info if present.) So each ctor formats out
//  the full URL and sets that as the systemn id.
//
//  This is defined here because we don't want to make the XML parser dependent
//  on this networking facility (and the other stuff it brings in.) Also, the XML
//  parser is used by the IDL compiler facility, and anything that the IDL needs
//  cannot use IDL based content as well.
//
//  So we break this out here. If you need the XML parser to handle URL based
//  entities, then you have to install your own entity resolver on it and resolve
//  URL based entities to one of these objects.
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
//  CLASS: TURLEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------
class CIDNETEXP TURLEntitySrc : public TXMLEntitySrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TURLEntitySrc() = delete;

        TURLEntitySrc
        (
            const   TURL&                   urlSystemId
        );

        TURLEntitySrc
        (
            const   TURL&                   urlSystemId
            , const TString&                strPublicId
        );

        TURLEntitySrc
        (
            const   TString&                strSystemId
        );

        TURLEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
        );

        TURLEntitySrc
        (
            const   TString&                strParentSysId
            , const TString&                strSystemId
            , const TString&                strPublicId
        );

        TURLEntitySrc
        (
            const   TURLEntitySrc&          xsrcToCopy
        );

        ~TURLEntitySrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TURLEntitySrc& operator=
        (
            const   TURLEntitySrc&          xsrcToAssign
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TBinInStream* pstrmMakeNew() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_purlSysId
        //      This is a URL which represents the system id of the entity
        //      this source is for. It's a pointer so that we can avoid
        //      imposing the socket headers on our clients just for this one
        //      usage.
        // -------------------------------------------------------------------
        TURL*   m_purlSysId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TURLEntitySrc,TXMLEntitySrc)
        DefPolyDup(TURLEntitySrc)
};

#pragma CIDLIB_POPPACK

