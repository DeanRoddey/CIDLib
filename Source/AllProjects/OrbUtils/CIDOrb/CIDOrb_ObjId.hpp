//
// FILE NAME: CIDOrb_ObjId.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/2000
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
//  This is the header for the CIDOrb_ObjId.cpp file, which implements the
//  TOrbObjId class. Instances of this class represent a live server object
//  at runtime. It includes the underlying TOrbId info that represents the
//  unique id for the intrface plus info on the approprirate client proxy
//  class to use and the info required to connect to the server ORB hosting
//  that object.
//
//  For future flexibility, clients should treat these like magic cookies.
//  So, if the underlying transport changes or becomes pluggable in the
//  future, they will not be affected. In general, clients will get these
//  from the CIDLib Name Server, on which servers advertise their services
//  by posting their object ids.
//
//  A server side object is uniquely identified in the universe by three
//  pieces of information. First is the interface id, which is a unique
//  id (in MD5 hash form) generated for the IDL interface that the object is
//  implementing. Next is a per-instance unique id (also MD5) that is generated
//  for each newly created server side instance. And thirdly, though its more
//  important for finding the host than indentifying the object, is the
//  address info for the host.
//
//  For now, that is the name/port of the host which will be resolved to
//  the actual end point by users of the object id.
//
//  Note that we have a time stamp field as well. This is not used by the
//  object id, but allows the CIDOrbUC facility to maintain a local cache
//  of object ids. This can greatly reduce network overhead and increase
//  performance for commonly accessed servers. But it needs to actually go
//  back to the name server for a given binding periodically. The time
//  stamp field is used for that timeout processing. It's not persistted
//  since it's purely for runtime purposees.
//
//  We also have an IP address that is not persisted, and is used by the
//  caching scheme as well. When a successfully connected object id is
//  put into the cache, we'll store the now looked up remote server address,
//  so that it doesn't have to be looked up again if the cached object is
//  used. This is only assumed to be set on objects in the cache.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TOrbServerBase;

// ---------------------------------------------------------------------------
//   CLASS: TOrbObjId
//  PREFIX: ooid
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbObjId

    : public TObject, public MStreamable, public MFormattable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbObjId();

        TOrbObjId
        (
            const   TString&                strInterfaceId
            , const TString&                strInstanceId
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnHost
            , const TString&                strClientProxyClass
        );

        TOrbObjId
        (
            const   TString&                strInterfaceId
            , const TMD5Hash&               mhashInstanceId
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnHost
            , const TString&                strClientProxyClass
        );

        TOrbObjId
        (
            const   TOrbObjId&              ooidSrc
        );

        TOrbObjId(TOrbObjId&& ooidSrc) :

            TOrbObjId()
        {
            *this = tCIDLib::ForceMove(ooidSrc);
        }

        ~TOrbObjId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbObjId& operator=
        (
            const   TOrbObjId&              ooidSrc
        );

        TOrbObjId& operator=
        (
                    TOrbObjId&&             ooidSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TOrbObjId&              ooidSrc
        )   const;

        tCIDLib::TBoolean operator!=(const TOrbObjId& ooidSrc) const
        {
            return !operator==(ooidSrc);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TBoolean bHasCachedAddr() const;

        tCIDLib::TVoid ClearCachedAddr();

        [[nodiscard]] tCIDLib::TEncodedTime enctCache() const;

        tCIDLib::TEncodedTime enctCache
        (
            const   tCIDLib::TEncodedTime   enctToSet
        );

        [[nodiscard]] tCIDLib::THashVal hshKey() const;

        const TIPAddress& ipaCached() const;

        [[nodiscard]] tCIDLib::TIPPortNum ippnHost() const;

        const TOrbId& oidKey() const;

        const TString& strClientProxyClass() const;

        const TString& strAddrText() const;

        tCIDLib::TVoid SetCachedAddr
        (
            const   TIPAddress&             ipaToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;

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
        //  m_bHasCachedAddr
        //      If a cached address is set on us, this is set to indicate
        //      that. It can be cleared to cause any previously set value to
        //      be ignored. This is runtime only, for use by the client side
        //      object id caching in the CIDOrbUC facility.
        //
        //  m_enctCache
        //      A time stamp used by CIDOrbUC's facility to maintain a local
        //      cache of object ids.
        //
        //  m_ipaSrvCache
        //      Use donly by the client side object id caching. This is set
        //      to the host address that was looked up in order to create the
        //      successful connection that will then be cached. This is
        //      intended to be used by the caching system, but it could be used
        //      by any other code that manages object ids. It's not persisted.
        //
        //  m_ippnHost
        //      The port number we were given for the object server ORB
        //      hosting the object.
        //
        //  m_oidKey
        //      The orb id object that holds the interface/instance id for
        //      the object represented by this object id.
        //
        //  m_strClientProxyClass
        //      The class type of the client proxy for the interface. This
        //      is used for dynamic proxy creation. The CIDIDL compiler always
        //      generates client proxy classes so that they support advanced
        //      RTTI, and hence can be dynamically created.
        //
        //  m_strHostName
        //      The host name that we were given for the server ORB hosting
        //      the object. Could be a dotted address as well. We keep it
        //      in whatever form we got.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bHasCachedAddr;
        tCIDLib::TEncodedTime   m_enctCache;
        TIPAddress              m_ipaSrvCache;
        tCIDLib::TIPPortNum     m_ippnHost;
        TOrbId                  m_oidKey;
        TString                 m_strClientProxyClass;
        TString                 m_strHostName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbObjId,TObject)
};

#pragma CIDLIB_POPPACK

