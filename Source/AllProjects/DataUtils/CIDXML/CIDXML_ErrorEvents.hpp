//
// FILE NAME: CIDXML_ErrorEvents.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/04/1999
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
        MXMLErrorEvents(const MXMLErrorEvents&) = delete;
        MXMLErrorEvents(MXMLErrorEvents&&) = delete;
        MXMLErrorEvents& operator=(const MXMLErrorEvents&) = delete;
        MXMLErrorEvents& operator=(MXMLErrorEvents&) = delete;

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
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  MXMLErrorEvents: Protected, virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid MXMLErrorEvents::ResetErrors()
{
}
