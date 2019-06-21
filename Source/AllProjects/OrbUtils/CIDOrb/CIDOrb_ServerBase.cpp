//
// FILE NAME: CIDOrb_OrbServerBase.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/05/2000
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
//  This is implementation file for the base IDL server class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbServerBase,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TOrbServerBase
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbServerBase: Protected constructors
// ---------------------------------------------------------------------------

//
//  We create an object id for this instance. We use the passed interface id
//  and client proxy class. We provide a new unique instance id. The derived
//  class tells us the name of the client proxy class that was generated for
//  this interface.
//
TOrbServerBase::TOrbServerBase( const   TString&    strInterfaceId
                                , const TString&    strClientProxyClass) :

    m_bCleanup(kCIDLib::False)
    , m_ooidThis
      (
        strInterfaceId
        , TUniqueId::mhashMakeId()
        , facCIDSock().strIPHostName()
        , facCIDOrb().ippnORB()
        , strClientProxyClass
      )
{
}

TOrbServerBase::TOrbServerBase(const TOrbObjId& ooidThis) :

    m_bCleanup(kCIDLib::False)
    , m_ooidThis(ooidThis)
{
}


// ---------------------------------------------------------------------------
//  TOrbServerBase: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  The generated IDL server side stub overrides this. If it gets a command
//  that isn't for its IDL generated class, it calls us. We check for some
//  special internal methods that are sent between the client and server
//  ORB behind the client code's back. If one of those, we handle it. If
//  not, we throw an error.
//
tCIDLib::TVoid
TOrbServerBase::Dispatch(const TString& strCmd, TOrbCmd& orbcToDispatch)
{
    if (strCmd == TFacCIDOrb::strCmd_Ping)
    {
        // We just echo back the value it sent
        tCIDLib::TCard4 c4Val;
        orbcToDispatch.strmIn() >> c4Val;
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << c4Val;
    }
     else
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcCmd_UnknownMethod
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strCmd
            , ooidThis().oidKey()
        );
    }
}


// We just provide default impls of these
tCIDLib::TVoid TOrbServerBase::Initialize()
{
    // Reset the cleanup flag
    m_bCleanup = kCIDLib::False;
}


tCIDLib::TVoid TOrbServerBase::Terminate()
{
    //
    //  Just provided in case we need to add some init in the future. All
    //  derived classes should call us if they override.
    //
}


// ---------------------------------------------------------------------------
//  TOrbServerBase: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbServerBase::InitializeOrbServer()
{
    // No-op for now
}


tCIDLib::TVoid TOrbServerBase::TerminateOrbServer()
{
    // No-op for now
}

