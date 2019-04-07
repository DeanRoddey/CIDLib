//
// FILE NAME: CIDSock_Pinger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2012
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
//  This is the header for the CIDSock_Pinger.cpp class. This class provides
//  a simple interface to the low level Ping protocol. It's mainly used to
//  test for the presence of hosts on the network.
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
//   CLASS: TSockPinger
//  PREFIX: sping
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSockPinger : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSockPinger();

        TSockPinger(const TSockPinger&) = delete;

        ~TSockPinger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSockPinger& operator=(const TSockPinger&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bWaitReply
        (
            const   tCIDLib::TCard4         c4WaitMillis
            ,       tCIDLib::TCard4&        c4RepMSs
            ,       tCIDLib::TCard4&        c4RepBytes
            ,       TIPAddress&             ipaFrom
        );

        tCIDLib::TVoid CancelReply
        (
            const   tCIDLib::TBoolean       bResetStamp
        );

        tCIDLib::TEncodedTime enctLastTime() const;

        tCIDSock::EPingStates eState() const;

        tCIDLib::TVoid EndPing();

        const TString& strRemAddr() const;

        tCIDLib::TVoid StartPing
        (
            const   TString&                strRemAddr
            , const tCIDSock::EAddrTypes    eType
            ,       TIPAddress&             ipaTar
        );

        tCIDLib::TVoid SendRequest();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckReady();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kspingThis
        //      The underlying kernel level socket pinger that we use to do
        //      our thing.
        //
        //  m_strRemAddr
        //      We keep the passed address around to use as an identifier
        //      for this pinger object.
        // -------------------------------------------------------------------
        TKrnlSockPinger m_kspingThis;
        TString         m_strRemAddr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSockPinger, TObject)
};

#pragma CIDLIB_POPPACK


