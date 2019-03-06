//
// FILE NAME: CIDSIP_Msg.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 4/28/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDSIP_Msg.cpp file. This file implements
//  the class that contains SIP msg content.
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
//   CLASS: TSIPMsg
//  PREFIX: sipmsg
// ---------------------------------------------------------------------------
class CIDSIPEXP TSIPMsg : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSIPMsg();

        TSIPMsg
        (
            const   TSIPMsg&                sipmsgSrc
        );

        ~TSIPMsg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const TSIPMsg& operator=
        (
            const   TSIPMsg&                sipmsgSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddHdrLine
        (
            const   TString&                strKey
            , const TString&                strValue
        );

        tCIDLib::TBoolean bFindHdrLine
        (
            const   TString&                strKey
            ,       TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4ContentLen() const;

        tCIDLib::TCard4 c4MaxForwards() const;

        tCIDLib::TCard4 c4MaxForwards
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        const TMemBuf& mbufContent() const;

        const TString& strFindHdrLine
        (
            const   TString&                strKey
        )   const;

        tCIDLib::TVoid SetContent
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufCont
        );

        const TString& strFirstLine() const;

        const TString& strFirstLine
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ContLen
        //      We keep this redundantly (it's in a header line as well, or
        //      normally it will be) so that we don't have to keep looking
        //      for and parsing the content length line.
        //
        //  m_c4MaxForwards
        //      Defaults to 70 but can be set by the client app.
        //
        //  m_colHdrLines
        //      Holds the header lines that came in with the msg, or that were
        //      put in to be sent.
        //
        //  m_mbufContent
        //      Holds the content of the message.
        //
        //  m_strFirstLine
        //      Holds the first line to be sent or that was received.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ContLen;
        tCIDLib::TCard4     m_c4MaxForwards;
        tCIDLib::TKVHashSet m_colHdrLines;
        THeapBuf            m_mbufContent;
        TString             m_strFirstLine;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSIPClient,TObject)
};

#pragma CIDLIB_POPPACK

