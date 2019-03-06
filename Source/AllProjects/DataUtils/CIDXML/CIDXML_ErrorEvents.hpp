//
// FILE NAME: CIDXML_ErrorEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/04/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file defines a mixin class that represents the event callouts for
//  the error events.
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
//  CLASS: MXMLErrorEvents
// PREFIX: xev
// ---------------------------------------------------------------------------
class CIDXMLEXP MXMLErrorEvents
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        virtual ~MXMLErrorEvents()
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLParserCore;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MXMLErrorEvents()
        {
        }


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid HandleXMLError
        (
            const   tCIDLib::TErrCode       errcToPost
            , const tCIDXML::EErrTypes      eType
            , const TString&                strText
            , const tCIDLib::TCard4         c4CurColumn
            , const tCIDLib::TCard4         c4CurLine
            , const TString&                strSystemId
        ) = 0;

        virtual tCIDLib::TVoid ResetErrors();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        MXMLErrorEvents(const MXMLErrorEvents&);
        tCIDLib::TVoid operator=(const MXMLErrorEvents&);
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  MXMLErrorEvents: Protected, virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid MXMLErrorEvents::ResetErrors()
{
}
