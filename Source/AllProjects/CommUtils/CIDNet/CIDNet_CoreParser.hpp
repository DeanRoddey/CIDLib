//
// FILE NAME: CIDNet_CoreParser.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/05/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDNet_CoreParser.cpp file. This file implements
//  the TTNetCoreParser class. There are a number of protocols that share a
//  common structure. So we break out the core stuff here and each protocol
//  speciifc parser can extend it. It's used for protocols like HTTP and
//  SIP, for instance.
//
//  We have to support more than just stream sockets. For sending that's
//  easily done by just providing two different methods, one that takes a
//  stream socket and the other a datagram socket.
//
//  For reading replies, we have to provide an abstract data source class,
//  which we can create derivatives of. The TCP one just continues to read
//  data until it doesn't get anymore, putting more data into the spool
//  buffer that the base data source class to be spooled out. For the UDP
//  scenario, it just puts one datagram chunk in if it gets one, and that's
//  all it will provide.
//
//  Note that we can't use a standard socket based stream, else it would be
//  already done for us. But this is a different sort of scenario because
//  the header lines are text and we have to transcode them out, whereas
//  the body is potentially binary, and there's nothing to tell us where that
//  crossover happens until we get to the empty divider line. We then have
//  to grab any remaining spool buffer data, and continue reading binary data
//  from there on.
//
//  So it's too special case to use standard streams. We just provide
//  methods that take different data source types and create the correct
//  source wrapper internally, so we don't have to expose all that stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TNetPDataSrc;


// ---------------------------------------------------------------------------
//   CLASS: TNetCoreParser
//  PREFIX: httpc
// ---------------------------------------------------------------------------
class CIDNETEXP TNetCoreParser : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindHdrLine
        (
            const   tCIDLib::TKVPCollect&   colHdrLines
            , const TString&                strToFind
            ,       TString&                strValue
        );

        static tCIDLib::TBoolean bHdrLineExists
        (
            const   tCIDLib::TKVPCollect&   colHdrLines
            , const TString&                strToFind
        );

        static tCIDLib::TBoolean bParseTextEncoding
        (
            const   TString&                strContType
            ,       TString&                strEncoding
            , const tCIDLib::TBoolean       bProvideDefault = kCIDLib::True
        );

        static tCIDLib::TBoolean bFindTextEncoding
        (
            const   tCIDLib::TKVPCollect&   colHdrLines
            , const TMemBuf&                mbufCont
            , const tCIDLib::TCard4         c4ContLen
            ,       TString&                strEncoding
        );

        static tCIDLib::TBoolean bParseFirstLn
        (
            const   TString&                strFirstLn
            ,       TString&                strParm1
            ,       TString&                strParm2
            ,       TString&                strParm3
        );

        static tCIDLib::TBoolean bParseSrvFirstLn
        (
            const   TString&                strFirstLn
            ,       TString&                strRepType
            ,       tCIDLib::TCard4&        c4Code
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TErrCode&      errcFail
        );


        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TNetCoreParser(const TNetCoreParser&) = delete;

        ~TNetCoreParser();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNetCoreParser& operator=(const TNetCoreParser&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TNetCoreParser
        (
            const   TString&                strHdrSep
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bParseHdrLine
        (
                    TString&                strHdrLn
            ,       TString&                strValue
        );

        virtual tCIDNet::ENetPReadRes eBasicMsgVal
        (
            const   tCIDNet::EReadTypes     eReadType
            , const tCIDLib::TKVPCollect&   colHdrLines
            , const TString&                strFirstLn
            ,       tCIDLib::TBoolean&      bClose
            ,       tCIDLib::TBoolean&      bPersist
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TString&                strParm1
            ,       TString&                strParm2
            ,       TString&                strParm3
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDNet::ENetPReadRes eGetMsg
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDNet::EReadTypes     eReadType
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bNoCont
            ,       TString&                strFirstLine
            ,       tCIDLib::TKVPCollect&   colHdrLines
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TString&                strContType
            ,       TString&                strParm1
            ,       TString&                strParm2
            ,       TString&                strParm3
        );


        tCIDLib::TVoid SendMsg
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strFirstLine
            , const tCIDLib::TKVPCollect&   colHdrLines
            , const tCIDLib::TKVPCollect&   colExtraHdrs
            , const TMemBuf&                mbufCont
            , const tCIDLib::TCard4&        c4ContLen
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BuildMsg
        (
            const   TString&                strFirstLine
            , const tCIDLib::TKVPCollect&   colHdrLines
            , const tCIDLib::TKVPCollect&   colExtraHdrs
            , const TMemBuf&                mbufCont
            , const tCIDLib::TCard4&        c4ContLen
        );

        tCIDNet::ENetPReadRes eReadHdrLines
        (
                    TCIDDataSrc&            cnsSrc
            , const tCIDLib::TEncodedTime   enctEnd
            ,       tCIDLib::TKVPCollect&   colHdrLines
            ,       TThread&                thrCaller
        );

        tCIDLib::TVoid FmtHdrLines
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TKVPCollect&   colHdrs
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mbufTmp
        //      A buffer for us to use for in/out msg handling, to avoid
        //      creating them over and over.
        //
        //  m_strHdrSep
        //      The derived class tells us what to put between the name and
        //      value of the header lines.
        // -------------------------------------------------------------------
        THeapBuf    m_mbufTmp;
        TString     m_strHdrSep;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNetCoreParser,TObject)
};

#pragma CIDLIB_POPPACK


