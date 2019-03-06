//
// FILE NAME: CIDOrbUC_CoreAdminBaseImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
//
// DESCRIPTION:
//
//  This is the header for the CIDOrbUC_CoreAdminBaseIMpl.cpp file, which
//  provides a basic derivative of the IDL interface for core server admin
//  functions. Some stuff can be done on behalf of all servers and save a
//  lot of redundancy, so we provide it here, and then the servers derive
//  from us instead of the IDL generated stub for this interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDCoreAdminBaseImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class CIDORBUCEXP TCIDCoreAdminBaseImpl : public TCIDCoreAdminServerBase
{
    public :
        // --------------------------------------------------------------------
        // Constructors and Destructor
        // --------------------------------------------------------------------
        TCIDCoreAdminBaseImpl() = delete;

        TCIDCoreAdminBaseImpl(const TCIDCoreAdminBaseImpl&) = delete;

        ~TCIDCoreAdminBaseImpl();


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TCIDCoreAdminBaseImpl& operator=(const TCIDCoreAdminBaseImpl&) = delete;


        // --------------------------------------------------------------------
        // Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bPollValuesInScope
        (
            const   TString&                strParScope
            , const tCIDLib::TBoolean       bDirectOnly
            , const tCIDLib::TCard8         c8QueryStamp
            ,       tCIDLib::TCard8&        c8PollStamp
            ,       TFundVector<tCIDLib::TCard4>& fcolIds
            ,       TFundVector<tCIDLib::TCard8>& fcolValues
        )   override;

        tCIDLib::TCard4 c4QueryStats
        (
            const   TString&                strParScope
            ,       TVector<TStatsCacheItemInfo>& colToFill
            , const tCIDLib::TBoolean       bDirectOnly
            ,       tCIDLib::TCard8&        c8Stamp
        )   override;

        tCIDLib::TVoid QueryBasicInfo
        (
                    TString&                strHostName
            ,       TString&                strProcName
            ,       tCIDLib::TCard4&        c4CPUCount
            ,       TString&                strOSInfo
            ,       tCIDLib::TCard8&        c8StartTime
        )   override;

        tCIDLib::TVoid SetModLogLevel
        (
            const   TString&                strModName
            , const TString&                strLevel
        )   override;

        tCIDLib::TVoid SetModLogTrace
        (
            const   TString&                strModName
            , const tCIDLib::TBoolean       bState
        )   override;


    protected :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TCIDCoreAdminBaseImpl
        (
            const   TString&                strProcName
        );

    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_enctStartedAt
        //      We set this during the ctor and keep it around to return in
        //      the basic stats query.
        //
        //  m_strProcName
        //      The derived class provides us with a process name that we can
        //      hand out to remote callers.
        // --------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctStartedAt;
        TString                 m_strProcName;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TCIDCoreAdminBaseImpl,TCIDCoreAdminServerBase)
};

#pragma CIDLIB_POPPACK


