//
// FILE NAME: CIDSock_IPHostInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/05/1998
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
//  This is the header for the CIDSock_IPHostInfo.cpp file. This file
//  implements the TIPHostInfo class, which is used to return a block of
//  information about a TCP/IP host. This information can be gotten via
//  an IP address (binary or textual) or a host name.
//
//  The default ctor will return all addresses. The other one can return both
//  non-loopback adddress and non-loopback. If you pass a host name and false,
//  you'll get non-LB. If you pass localhost, you'll get loopbacks and the
//  flag doesn't matter.
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
//   CLASS: TIPHostInfo
//  PREFIX: iphi
// ---------------------------------------------------------------------------
class CIDSOCKEXP TIPHostInfo : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TIPHostInfo();

        TIPHostInfo
        (
            const   TString&                strHostName
            , const tCIDLib::TBoolean       bIncludeLoopbacks
        );

        TIPHostInfo(const TIPHostInfo&) = default;

        ~TIPHostInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIPHostInfo& operator=(const TIPHostInfo&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4IPAddrCount() const;

        const TIPAddress& ipaAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TString& strHostName() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CommonSetup
        (
            const   tCIDLib::TBoolean       bIncludeLoopbacks
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colAddrs
        //      The list of addresses that the host lives on. Hosts can
        //      have multiple addresses that they respond to.
        //
        //  m_strHostName
        //      The full text name of the host, which will something like
        //      "hubba.bubba.com" or "OfficePC" depend on whether we are in
        //      a workgroup or domain.
        // -------------------------------------------------------------------
        TVector<TIPAddress> m_colAddrs;
        TString             m_strHostName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TIPHostInfo,TObject)
        DefPolyDup(TIPHostInfo)
};

#pragma CIDLIB_POPPACK


