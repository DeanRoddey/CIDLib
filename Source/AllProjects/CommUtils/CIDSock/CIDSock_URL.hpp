//
// FILE NAME: CIDSock_URL.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/13/1999
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
//  This is the header for the CIDSock_URL.cpp file. This file implements
//  the TURL class, which provides processing for URL style URIs (Universal
//  Resource Identifiers.) URLs (Universal Resource Locators) are one type of
//  URI which are intended to (usually, people are inconsistent about this)
//  point to a retrievable resource on the web.
//
//  An important service of this class is to parse URLs, to return information
//  about the components of a parsed URL, to process relative URLs based on
//  another URL, and to create sockets which are appropriate for a particular
//  type of URL (.e.g to return an HTTP socket for a URL in the form
//  http://www.foo.com.)
//
// CAVEATS/GOTCHAS:
//
//  1)  Because of the complexity of escaping and all the variations, it is
//      assumed that any text provided is already pre-escaped, i.e. the
//      text is from a URL as it would be received over the wire. If you
//      build a URL by hand, you must pre-escape where appropriate and static
//      methods are provided in this class to do escaping and encoding based
//      on where in the URL the text would go or comes from.
//
//      The exception is BuildURL(), which takes all of the separate bits and so
//      assumes it must do the escapement.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TURL
//  PREFIX: url
// ---------------------------------------------------------------------------
class CIDSOCKEXP TURL :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        enum class EExpTypes
        {
            Path
            , Query
            , Fragment
            , Params
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsFirstProtocolChar
        (
            const   tCIDLib::TCh            chToTest
        );

        static tCIDLib::TBoolean bIsHostChar
        (
            const   tCIDLib::TCh            chToTest
        );

        static tCIDLib::TBoolean bIsProtocolChar
        (
            const   tCIDLib::TCh            chToTest
        );

        static tCIDLib::TVoid BuildURL
        (
            const   TString&                strProto
            , const TString&                strUser
            , const TString&                strPassword
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnHost
            , const TString&                strPath
            , const TString&                strParams
            , const TString&                strFragment
            , const tCIDLib::TKVPCollect* const pcolQuery
            ,       TString&                strToFill
            , const tCIDSock::EExpOpts      eEncOpt
        );

        static tCIDLib::TCard4 c4ParseQueryParms
        (
            const   TString&                strText
            ,       tCIDLib::TKVPCollect&   colToFill
            , const tCIDLib::TBoolean       bEncode
            , const EExpTypes               eType
        );

        static tCIDLib::TCard4 c4ParseQueryParms
        (
                    TTextInStream&          strmSrc
            ,       tCIDLib::TKVPCollect&   colToFill
            , const tCIDLib::TBoolean       bEncode
            , const EExpTypes               eType
        );

        static tCIDSock::EProtos eTextToProto
        (
            const   tCIDLib::TCh* const     pszToXLat
        );

        static tCIDSock::EProtos eTextToProto
        (
            const   TString&                strToXlat
        );

        static tCIDLib::TVoid EncodeTo
        (
            const   tCIDLib::TCh* const     pszSrc
            ,       TString&                strToFill
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
        );

        static tCIDLib::TVoid EncodeTo
        (
            const   TString&                strSrc
            ,       TString&                strToFill
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
        );

        static tCIDLib::TVoid ExpandTo
        (
            const   tCIDLib::TCh* const     pszSrc
            ,       TString&                strToFill
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
            ,       TTextConverter* const   ptcvtToUse = nullptr
        );

        static tCIDLib::TVoid ExpandTo
        (
            const   TString&                strSrc
            ,       TString&                strToFill
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
            ,       TTextConverter* const   ptcvtToUse = nullptr
        );

        static tCIDLib::TVoid ExpandTo
        (
            const   tCIDLib::TCh* const     pszSrc
            ,       TString&                strToFill
            ,       TMemBuf&                mbufToUse
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
            ,       TTextConverter* const   ptcvtToUse = nullptr
        );

        static tCIDLib::TVoid ExpandTo
        (
            const   TString&                strSrc
            ,       TString&                strToFill
            ,       TMemBuf&                mbufToUse
            , const EExpTypes               eType
            , const tCIDLib::EAppendOver    eAppend
            ,       TTextConverter* const   ptcvtToUse = nullptr
        );

        static tCIDLib::TVoid FormatQParms
        (
            const   tCIDLib::TKVPCollect&   colQParms
            ,       TString&                strToFill
            , const tCIDSock::EExpOpts      eEncOpt
            , const tCIDLib::EAppendOver    eAppend = tCIDLib::EAppendOver::Overwrite
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TURL();

        TURL
        (
            const   TString&                strURLText
            , const tCIDSock::EQualified    eQualified
        );

        TURL
        (
            const   tCIDLib::TCh* const     pszURLText
            , const tCIDSock::EQualified    eQualified
        );

        TURL
        (
            const   TString&                strRelTo
            , const TString&                strURLText
        );

        TURL
        (
            const   tCIDLib::TCh* const     pszRelTo
            , const tCIDLib::TCh* const     pszURLText
        );

        TURL
        (
            const   TURL&                   urlRelTo
            , const TString&                strURLText
        );

        TURL
        (
            const   TURL&                   urlRelTo
            , const tCIDLib::TCh* const     pszURLText
        );

        TURL(const TURL&) = default;

        ~TURL();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TURL& operator=(const TURL&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TURL&                   urlToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TURL&                   urlToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendToPath
        (
            const   TString&                strToAppend
        );

        tCIDLib::TBoolean bFullyQualified() const;

        const tCIDLib::TKVPList& colQParms() const;

        tCIDLib::TVoid Clear();

        tCIDSock::EProtos eProto() const;

        tCIDSock::EProtos eProto
        (
            const   tCIDSock::EProtos       eProto
        );

        TIPEndPoint ipepHost
        (
            const   tCIDSock::EAddrTypes    eAddrType = tCIDSock::EAddrTypes::Unspec
        )   const;

        tCIDLib::TIPPortNum ippnHost() const;

        TBinOutStream* pstrmFromURL
        (
                    TBinInStream*&          pstrmToFill
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDSock::EAddrTypes    eAddrType = tCIDSock::EAddrTypes::Unspec
        )   const;

        tCIDLib::TVoid QueryEncodedForm
        (
                    TString&                strProto
            ,       TString&                strUser
            ,       TString&                strPassword
            ,       TString&                strHost
            ,       tCIDLib::TIPPortNum&    ippnHost
            ,       TString&                strPath
            ,       TString&                strParams
            ,       TString&                strFragment
            ,       TString&                strQuery
        )   const;

        tCIDLib::TVoid QueryEncodedForm
        (
                    TString&                strProto
            ,       TString&                strUserPass
            ,       TString&                strHostPort
            ,       TString&                strPath
            ,       TString&                strSuffix
        )   const;

        tCIDLib::TVoid QueryExpandedForm
        (
                    TString&                strProto
            ,       TString&                strUser
            ,       TString&                strPassword
            ,       TString&                strHost
            ,       tCIDLib::TIPPortNum&    ippnHost
            ,       TString&                strPath
            ,       TString&                strParams
            ,       TString&                strFragment
            ,       TString&                strQuery
        )   const;

        tCIDLib::TVoid QueryExpQParams
        (
                    tCIDLib::TKVPList&      colToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid QueryResource
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bExpanded
            , const tCIDLib::TBoolean       bWithSuffix
        )   const;

        tCIDLib::TVoid QueryText
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bExpanded
            , const tCIDLib::TBoolean       bInclUserInfo
        )   const;

        tCIDLib::TVoid Reset
        (
            const   TString&                strNewText
            , const tCIDSock::EQualified    eQualified
        );

        tCIDLib::TVoid Reset
        (
            const   TString&                strRelTo
            , const TString&                strNewText
        );

        tCIDLib::TVoid Reset
        (
            const   TURL&                   urlRelTo
            , const TString&                strNewText
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCh* const     pszNewText
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCh* const     pszRelTo
            , const tCIDLib::TCh* const     pszNewText
        );

        tCIDLib::TVoid Reset
        (
            const   TURL&                   urlRelTo
            , const tCIDLib::TCh* const     pszNewText
        );

        const TString& strFragment() const;

        const TString& strHost() const;

        const TString& strParams() const;

        const TString& strPassword() const;

        const TString& strPath() const;

        const TString& strPath
        (
            const   TString&                strToSet
        );

        const TString& strUser() const;

        tCIDLib::TVoid Set
        (
            const   tCIDSock::EProtos       eProto
            , const TString&                strUser
            , const TString&                strPassword
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnHost
            , const TString&                strPath
            , const TString&                strParams
            , const TString&                strFragment
            , const tCIDLib::TKVPCollect&   colQParms
            , const tCIDLib::TBoolean       bEncoded
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TIPPortNum ippnDefPort() const;

        tCIDLib::TVoid MakeRelativeTo
        (
            const   TURL&                   urlRelTo
        );

        tCIDLib::TVoid Parse
        (
            const   tCIDLib::TCh* const     pszNewText
            , const tCIDSock::EQualified    eQualified
        );

        tCIDLib::TVoid SimplifyPath();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colQParms
        //      The broken out query parms, if any. We also create a full
        //      string version of this in m_strQuery, since sometimes they
        //      want it in that form as well. The parse routine breaks it
        //      out into this format.
        //
        //  m_eProto
        //      The Proto indicated in the URL. If its relative, then it
        //      might be EProto_None, which is what it is defaulted to
        //      during construction.
        //
        //  m_ippnHost
        //      This is the host port number. If none was explicitly provided,
        //      its set to the default for that protocol.
        //
        //  m_strFragment
        //      The fragment part of the URL, if any. This is the part after
        //      the # character.
        //
        //  m_strHost
        //      The host part of the URL, if any.
        //
        //  m_strParams
        //      If there are any parameters in the path, they are pulled out
        //      into here.
        //
        //  m_strPassword
        //      This is the password part of the URL, if any.
        //
        //  m_strPath
        //      This is the path part of the URL, i.e. its the part past
        //      the protocol, host, port, etc.. part, which starts with the
        //      forward slash after the host.
        //
        //  m_strUser
        //      The user name part of the the URL, if any. This is an optional
        //      part of the host name.
        // -------------------------------------------------------------------
        tCIDLib::TKVPList   m_colQParms;
        tCIDSock::EProtos   m_eProto;
        tCIDLib::TIPPortNum m_ippnHost;
        TString             m_strFragment;
        TString             m_strHost;
        TString             m_strParams;
        TString             m_strPassword;
        TString             m_strPath;
        TString             m_strUser;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TURL,TObject)
        DefPolyDup(TURL)
        NulObject(TURL)
};

#pragma CIDLIB_POPPACK

