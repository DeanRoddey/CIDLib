//
// FILE NAME: CIDOrbUC_NSInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2001
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
//  This is the header for the CIDOrbUC_NSInfo.cpp file, which implements
//  the TNameServerInfo class. This class defines the information that the
//  name server maintains for a binding. Each binding of course holds an
//  Orb object id, which is the object id for the bound server side object.
//  It also holds the node name, the descriptive string, and a couple of
//  optional strings that can be used by the app that registers the binding
//  to make it easier for clients to find desired services.
//
//  We also declare a simple derivative of this class which is used by the
//  auto-rebinder. It adds a parent path and a couple flags. This is what
//  the rebinder uses to store info on bindings it is managing, and also what
//  it sends over in the rebind operations. The extra bits of info are not
//  streamed since they aren't required on the NS side.
//
// CAVEATS/GOTCHAS:
//
//  1)  These objects are streamed obviously, but not persisted. But we
//      do still use a format version in the streamed data, in case we have
//      to have any backwards compatibility between servers and cients.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TNameServerInfo
//  PREFIX: nsvi
// ---------------------------------------------------------------------------
class CIDORBUCEXP TNameServerInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eCompOnName
        (
            const   TNameServerInfo&        nsvi1
            , const TNameServerInfo&        nsvi2
        );

        static tCIDLib::ESortComps eCompOnDescription
        (
            const   TNameServerInfo&        nsvi1
            , const TNameServerInfo&        nsvi2
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNameServerInfo();

        TNameServerInfo
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
        );

        TNameServerInfo
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
        );

        TNameServerInfo
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
        );

        TNameServerInfo
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
        );

        TNameServerInfo(const TNameServerInfo&) = default;
        TNameServerInfo(TNameServerInfo&&) = delete;

        ~TNameServerInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNameServerInfo& operator=(const TNameServerInfo&) = default;
        TNameServerInfo& operator=(TNameServerInfo&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TOrbObjId& ooidSrvObject() const;

        const TString& strDescription() const;

        const TString& strExtra1() const;

        const TString& strExtra1
        (
            const   TString&                strToSet
        );

        const TString& strExtra2() const;

        const TString& strExtra2
        (
            const   TString&                strToSet
        );

        const TString& strExtra3() const;

        const TString& strExtra3
        (
            const   TString&                strToSet
        );

        const TString& strExtra4() const;

        const TString& strExtra4
        (
            const   TString&                strToSet
        );

        const TString& strNodeName() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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
        //  Private data members
        //
        //  m_ooidSrvObject
        //      This is the orb object id of the server object that the node
        //      being reported on by this object represents.
        //
        //  m_strDescription
        //      The descriptive string, if any, that was provided when the
        //      server side object id was registered.
        //
        //  m_strExtraX
        //      Three extra string values that the registrar can use to provide
        //      information to clients searching the name server for services.
        //
        //  m_strNodeName
        //      This is the name of the name server terminal node in which
        //      this info is stored.
        // -------------------------------------------------------------------
        TOrbObjId   m_ooidSrvObject;
        TString     m_strDescription;
        TString     m_strExtra1;
        TString     m_strExtra2;
        TString     m_strExtra3;
        TString     m_strExtra4;
        TString     m_strNodeName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TNameServerInfo,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TNSRebindInfo
//  PREFIX: nsrbi
// ---------------------------------------------------------------------------
class CIDORBUCEXP TNSRebindInfo : public TNameServerInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TNSRebindInfo&          nsrbi1
            , const TNSRebindInfo&          nsrbi2
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNSRebindInfo();

        TNSRebindInfo
        (
            const   TString&                strParPath
            , const TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
        );

        TNSRebindInfo
        (
            const   TString&                strParPath
            , const TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
        );

        TNSRebindInfo
        (
            const   TString&                strParPath
            , const TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
        );

        TNSRebindInfo
        (
            const   TString&                strParPath
            , const TOrbObjId&              ooidSrvObject
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
        );

        TNSRebindInfo(const TNSRebindInfo&) = default;

        ~TNSRebindInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNSRebindInfo& operator=(const TNSRebindInfo&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsBound() const;

        tCIDLib::TBoolean bNeedsRefresh() const;

        tCIDLib::TBoolean bNeedsRefresh
        (
            const   tCIDLib::TEncodedTime   enctNow
        )   const;

        tCIDLib::TBoolean bNeedsWork() const;

        tCIDLib::TCard4 c4ErrCount() const;

        tCIDLib::TCard4 c4IncErrCount();

        const TString& strFullPath() const;

        const TString& strParentPath() const;

        tCIDLib::TVoid SetNextRenewal();

        tCIDLib::TVoid SetNotBound();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ErrCount
        //      Any time a bind fails, this is bumped. Any time it succeeds
        //      this is cleared. If this count gets to a certain point, the
        //      rebinder (or other binding client app) may wish to remove
        //      the binding from the rebinder to prevent continual connections
        //      to the name server to try to rebind it.
        //
        //  m_enctNextRenewal
        //      The next time this guy is to be renewed. It's zero initially
        //      which means it'll always be eligible. After a pass, we set
        //      it forward to a time a reasonable distance prior to the point
        //      at which the name server would time it out. That gives us
        //      time to get it renewed before it is dropped, plus some safety
        //      margin. If there's a failure to renew it, we set it back to
        //      zero again.
        //
        //  m_strFullPath
        //      The full path. The underlying NS info object only stores the
        //      basic name part, and we store the parent path. This is the
        //      full path we build so that quick lookups by path can be done.
        //
        //  m_strParPath
        //      The parent path of the binding. The binding info itself only
        //      stores the basic name, not the full path. So the server needs
        //      this info to know what the full path is.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ErrCount;
        tCIDLib::TEncodedTime   m_enctNextRenewal;
        TString                 m_strFullPath;
        TString                 m_strParPath;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TNSRebindInfo,TNameServerInfo)
};

#pragma CIDLIB_POPPACK


