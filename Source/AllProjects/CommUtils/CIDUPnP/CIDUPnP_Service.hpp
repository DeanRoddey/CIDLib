//
// FILE NAME: CIDUPnP_Service.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDUPnP_Service.cpp file, which implements
//  the class that represents a basic UPnP service. We'll have other classes
//  that derive from this that provide more specific types of services. But
//  we can't make this guy an abstract, because often we will want to create
//  and deal with services generically, in order to get information and
//  such.
//
// CAVEATS/GOTCHAS:
//
//  1)  Eventing can be enabled on a service object. If so, the base service
//      class will maintain a queue of reported variable values. The derived
//      classes can either call QueryStateVar(), which will get the already
//      stored value if evented (and it's been reported), and optionally they
//      can ask for it to be actively queried if not.
//
//      Or, if the value they are returning has a direct method to call, they
//      check if eventing and the value is available or not, and if not they
//      can call their direct method. In some cases they are getting multiple
//      values, so they can't just call QueryStateValue() which will try to
//      do multiple individual value queries, one for each value.
//
//      For values that are not evented and are not state variables, the
//      derived class always just calls a method to get that info.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TUPnPDevice;

// ---------------------------------------------------------------------------
//  CLASS: TUPnPService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPService : public TObject, public MUPnPSvcCallback
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPService();

        TUPnPService(const TUPnPService&) = delete;

        ~TUPnPService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPService& operator=(const TUPnPService&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid UPnPServiceDied() override;

        tCIDLib::TVoid UPnPVarChanged
        (
            const   tCIDLib::TCh* const     pszVarName
            , const tCIDLib::TCh* const     pszVarVal
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEvented() const;

        tCIDLib::TBoolean bIsDead() const;

        tCIDLib::TBoolean bLogErrs() const;

        tCIDLib::TBoolean bLogErrs
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bQueryStateVar
        (
            const   TString&                strVarName
            ,       TString&                strValToFill
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bQueryStateVar
        (
            const   TString&                strVarName
            ,       TString&                strValToFill
            ,       tCIDLib::TCard4&        c4SerialNum
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TVoid EnableEvents();

        const TString& strTypeID() const;

        tCIDLib::TVoid QueryStateVar
        (
            const   TString&                strVarName
            ,       TString&                strValToFill
            , const tCIDLib::TBoolean       bQueryIfNot = kCIDLib::False
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TVoid QueryStateVar
        (
            const   TString&                strVarName
            ,       TString&                strValToFill
            ,       tCIDLib::TCard4&        c4SerialNum
            , const tCIDLib::TBoolean       bQueryIfNot = kCIDLib::False
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TVoid QueryStateVar
        (
            const   TString&                strVarName
            ,       tCIDLib::TStrCollect&   colAddTo
            , const tCIDLib::TBoolean       bQueryIfNot = kCIDLib::False
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TVoid QueryStateVar
        (
            const   TString&                strVarName
            ,       tCIDLib::TStrCollect&   colAddTo
            ,       tCIDLib::TCard4&        c4SerialNum
            , const tCIDLib::TBoolean       bQueryIfNot = kCIDLib::False
            , const tCIDLib::TBoolean       bClear = kCIDLib::False
        )   const;

        tCIDLib::TVoid Release();


    protected :
        // -------------------------------------------------------------------
        //  The device class is our friend
        // -------------------------------------------------------------------
        friend class TUPnPDevice;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid SvcReset();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseXML
        (
                    TXMLTreeParser&         xtprsItems
            , const TString&                strXMLText
            , const TString&                strContDescr
        );

        tCIDLib::TVoid BumpSerialNum() const;

        tCIDLib::TVoid CheckRetParmCnt
        (
            const   TString&                strCmdName
            , const tCIDLib::TCard4         c4OutCnt
            , const tCIDLib::TCard4         c4ExpCnt
            , const tCIDLib::TBoolean       bExact = kCIDLib::True
        )   const;

        tCIDLib::TVoid InvokeAction
        (
            const   TString&                strCmd
            , const tCIDLib::TStrCollect&   colInParms
            ,       tCIDLib::TStrCollect&   colOutParms
        )   const;

        tCIDLib::TVoid SetKrnlObj
        (
                    TKrnlUPnPService* const pkupnpsToAdopt
            , const TString&                strTypeID
            ,       MUPnPSvcCallback* const pupnpscbHandler
        );

        tCIDLib::TVoid SetStateVar
        (
            const   TString&                strVarName
            , const TString&                strValToFill
        )   const;

        tCIDLib::TVoid ThrowBadOutActParms
        (
            const   TString&                strCmdName
            , const tCIDLib::TCard4         c4Line
        )   const;


        // -------------------------------------------------------------------
        //  Protected data members
        //
        //  Some of these are mutable because they have to be used in otherwise
        //  const methods that just return info queried from the device.
        //
        //  m_bIsEvented
        //      If eventing is enabled, this is set to let the derived class
        //      know that it should come to get the latest value from our
        //      queue instead of actively querying it.
        //
        //  m_bIsDead
        //      If event subscription is enabled, this will be set if we
        //      get a service instance died notification.
        //
        //  m_bLogErrs
        //      This can be set to have us log more verbosely.
        //
        //  m_colInParms
        //  m_colOutParms
        //      We keep a couple string collections around for use by our
        //      derived classes, for in/out parms. Otherwise every one of
        //      them would have to redundantly declare these.
        //
        //  m_strTmp
        //      A temp string for derived classes to use, mostly to read in
        //      state variables.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bIsEvented;
        tCIDLib::TBoolean           m_bIsDead;
        tCIDLib::TBoolean           m_bLogErrs;
        mutable tCIDLib::TStrList   m_colInParms;
        mutable tCIDLib::TStrList   m_colOutParms;
        mutable TString             m_strTmp;


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TVarList = tCIDLib::TKVHashSet;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckSvcSet(const tCIDLib::TCard4 c4Line) const
        {
            if (!m_pkupnpsThis)
                ThrowNotSet(c4Line);
        }

        tCIDLib::TVoid ThrowNotSet
        (
            const   tCIDLib::TCard4         c4Line
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SerialNum
        //      If we are evented, every time a state variable is modified, this is
        //      bumped, so that code that uses the server can check whether anything
        //      has changed. Has to be mutable since variables can be faulted in upon
        //      read in some cases.
        //
        //  m_colVarList
        //      If we have events enabled, then our event callback will dump
        //      new values into this list. If the value is already in the
        //      list it will be updated, so it's never bigger than the number
        //      of available variables for the service type. It is thread
        //      safe so that it can be locked. Derived classes, if eventing
        //      is enabled, will check here for a newer value and store it
        //      before returning to the caller.
        //
        //      This guy has to be mutable because some service variables we
        //      want to be able to clear after they are read.
        //
        //  m_pkupnpdThis
        //      The underlying kernel level UPnP service object that we do our
        //      work through. We are responsible for cleaning it up, and for
        //      reference counting it, so that we can use value semantics.
        //
        //  m_strTypeID
        //      We get this when we are set up with the underlying kernel
        //      service object, so we keep it around to avoid having to go
        //      back and get it when required.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4 m_c4SerialNum;
        mutable TVarList        m_colVarList;
        TKrnlUPnPService*       m_pkupnpsThis;
        TString                 m_strTypeID;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPService,TObject)
};

// Define some lists of services
namespace tCIDUPnP
{
    using TSvcCollect   = TCollection<TUPnPService>;
    using TSvcList      = TVector<TUPnPService>;
}

#pragma CIDLIB_POPPACK
