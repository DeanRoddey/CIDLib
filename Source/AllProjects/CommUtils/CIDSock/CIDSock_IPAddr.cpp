//
// FILE NAME: CIDSock_IPAddr.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TIPAddress and TIPEndPoint classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TIPAddress,TObject)
RTTIDecls(TIPEndPoint,TIPAddress)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDSock_IPAddr
{
    // -----------------------------------------------------------------------
    //  The streaming format version for the IP address class
    //
    //  Version 2 -
    //      We started storing the host name along with the address info
    //
    //  Version 3 -
    //      The changes required for the IP V4/6 agnosticism, so we have to
    //      store a variable sized byte array for the address info.
    //
    //  Version 4 -
    //      We need to store the scope id as well, which is required for some
    //      IPV6 addresses.
    //
    //  Version 5 -
    //      And, again, we failed to store the flow info value of an IPV6
    //      address, so that was added as well.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1IPAddrFmtVersion = 5;


    // -----------------------------------------------------------------------
    //  The straeming format version for the IP end point class
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1EndPntFmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TIPAddress
//  PREFIX: ipa
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIPAddress: Public, static methods
// ---------------------------------------------------------------------------
TIPAddress& TIPAddress::Nul_TIPAddress()
{
    static TIPAddress* pipaNull = 0;
    if (!pipaNull)
    {
        TBaseLock lockInit;
        if (!pipaNull)
            TRawMem::pExchangePtr(&pipaNull, new TIPAddress);
    }
    return *pipaNull;
}



// ---------------------------------------------------------------------------
//  TIPAddress: Constructors and Destructor
// ---------------------------------------------------------------------------
TIPAddress::TIPAddress()
{
}

TIPAddress::TIPAddress( const   TString&                strIPAddressString
                        , const tCIDSock::EAddrTypes    eType)
{
    SetAddr(strIPAddressString, eType);
}

TIPAddress::TIPAddress(const TKrnlIPAddr& kipaToSet) :

    m_kipaThis(kipaToSet)
{
    // Format out the IP address to text and store it
    TKrnlString kstrAddr;
    if (!TKrnlIP::bTextFromIPAddr(kipaToSet, kstrAddr))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_CantConvertIPAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    m_strTextVersion = kstrAddr.pszValue();
}


//
//  This one is where they already know the host name and the IP address
//  of the target machine and can set it directly. We just accept that
//  they give us the right info. If not, it's their own fault.
//
TIPAddress::TIPAddress( const   TString&        strHostName
                        , const TKrnlIPAddr&    kipaToSet) :

    m_kipaThis(kipaToSet)
    , m_strHostName(strHostName)
{
    // Format out the IP address to text and store it
    TKrnlString kstrAddr;
    if (!TKrnlIP::bTextFromIPAddr(kipaToSet, kstrAddr))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_CantConvertIPAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    m_strTextVersion = kstrAddr.pszValue();
}


TIPAddress::TIPAddress( const   tCIDSock::ESpecAddrs    eAddr
                        , const tCIDSock::EAddrTypes    eType) :

    m_kipaThis(eAddr, eType)
{
    // Format out the IP address to text and store it
    TKrnlString kstrAddr;
    if (!TKrnlIP::bTextFromIPAddr(m_kipaThis, kstrAddr))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_CantConvertIPAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    m_strTextVersion = kstrAddr.pszValue();
}


TIPAddress::~TIPAddress()
{
}


// ---------------------------------------------------------------------------
//  TIPAddress: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This form cannot specific a particular IP address type if a host name
//  lookup is done.
//
TIPAddress& TIPAddress::operator=(const TString& strAddrToAssign)
{
    SetAddr(strAddrToAssign, tCIDSock::EAddrTypes::Unspec);
    return *this;
}


TIPAddress& TIPAddress::operator=(const TKrnlIPAddr& kipaToAssign)
{
    SetAddr(kipaToAssign);
    return *this;
}


tCIDLib::TBoolean TIPAddress::operator==(const TIPAddress& ipaToCompare) const
{
    if (this == &ipaToCompare)
        return kCIDLib::True;

    //
    //  We only have to compare the binary address, since the text version
    //  is just driven of of that. The host name is ignored in this case,
    //  which is a little dangerous but it really is only for display
    //  purposes generally.
    //
    return (m_kipaThis == ipaToCompare.m_kipaThis);
}

tCIDLib::TBoolean
TIPAddress::operator==(const TKrnlIPAddr& kipaToCompare) const
{
    return (m_kipaThis == kipaToCompare);
}


tCIDLib::TBoolean TIPAddress::operator!=(const TIPAddress& ipaToCompare) const
{
    return !operator==(ipaToCompare);
}

tCIDLib::TBoolean
TIPAddress::operator!=(const TKrnlIPAddr& kipaToCompare) const
{
    return !operator==(kipaToCompare);
}


tCIDLib::TCard1 TIPAddress::operator[](const tCIDLib::TCard4 c4Index) const
{
    if (c4Index > m_kipaThis.c4Count())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_BadByteIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , TCardinal(m_kipaThis.c4Count())
        );
    }
    return m_kipaThis.pc1Data()[c4Index];
}


// ---------------------------------------------------------------------------
//  TIPAddress: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TIPAddress::bIsLoopback() const
{
    return m_kipaThis.bIsLoopback();
}


// Return the address family type forthis address
tCIDSock::EAddrTypes TIPAddress::eAddrType() const
{
    return m_kipaThis.eType();
}


//
//  This can be called after the fact to force re-resolution of the host
//  name to IP address, to deal with changes in server address or something
//  of that nature. We just set our host name back on ourself, if it's
//  ever been set. If it hasn't, not much we can do.
//
tCIDLib::TVoid TIPAddress::ForceNameResolution(const tCIDSock::EAddrTypes eType)
{
    if (!m_strHostName.bIsEmpty())
    {
        TString strName = m_strHostName;
        SetAddr(strName, eType);
    }
}


// Provide access to the underlying ip address
const TKrnlIPAddr& TIPAddress::kipaThis() const
{
    return m_kipaThis;
}


// Return the text version of the address
const TString& TIPAddress::strAsText() const
{
    return m_strTextVersion;
}


const TString&
TIPAddress::strFromText(const   TString&                strToSet
                        , const tCIDSock::EAddrTypes    eType)
{
    SetAddr(strToSet, eType);
    return m_strTextVersion;
}


const TString&
TIPAddress::strHostName(const tCIDLib::TBoolean bThrowIfNot) const
{
    // If we already have it, then just return it. Else try to get it
    if (!m_strHostName.bIsEmpty())
        return m_strHostName;

    //
    //  Use a local buffer plenty long enough, and try to convert from the
    //  numeric address to text.
    //
    //  NOTE:   It is possible this can fail if reverse resolution is not
    //          available. If this happens and the bThrowIfNot parameter isn't
    //          set, we'll just make the dotted text version to be the host
    //          name.
    //
    TKrnlString kstrName;
    if (TKrnlIP::bQueryNameByAddr(m_kipaThis, kstrName))
    {
        // It worked so store the host name
        m_strHostName = kstrName.pszValue();
    }
     else
    {
        if (bThrowIfNot)
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcTCP_GetRemIPHostName
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strTextVersion
            );
        }
        m_strHostName = m_strTextVersion;
    }
    return m_strHostName;
}


// ---------------------------------------------------------------------------
//  TIPAddress: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TIPAddress::FormatTo(TTextOutStream& strmDest) const
{
    // Just dump out our stored text version
    strmDest << m_strTextVersion;
}


tCIDLib::TVoid TIPAddress::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Check the format version
    tCIDLib::TCard1 c1FmtVersion;
    strmToReadFrom  >> c1FmtVersion;
    if (!c1FmtVersion || (c1FmtVersion > CIDSock_IPAddr::c1IPAddrFmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1FmtVersion)
            , clsThis()
        );
    }

    if (c1FmtVersion > 2)
    {
        //
        //  It's the new 4/6 agnostic format. Stream in the type and the
        //  data bytes and set them on the IP address object.
        //
        tCIDLib::TCard1 ac1Data[kCIDSock::c4MaxIPAddrBytes];
        tCIDLib::TCard4 c4Count;
        tCIDSock::EAddrTypes eType;

        strmToReadFrom  >> eType
                        >> c4Count;

        if (c4Count > kCIDSock::c4MaxIPAddrBytes)
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcTCP_BadIPAddrSz
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4Count)
            );
        }

        //
        //  If V4 or beyond, read in the scope id, else default. These are
        //  in native endian form.
        //
        tCIDLib::TCard4 c4ScopeId = 0;
        if (c1FmtVersion > 3)
            strmToReadFrom >> c4ScopeId;

        //
        //  If V5 or beyond, read in the flow info, else default. These are
        //  in native endian form.
        //
        tCIDLib::TCard4 c4FlowInfo = 0;
        if (c1FmtVersion > 4)
            strmToReadFrom >> c4FlowInfo;

        // If any address bytes, read those in
        if (c4Count)
            strmToReadFrom.c4ReadRawBuffer(ac1Data, c4Count);

        // And now set up the host ip address object
        m_kipaThis.bSet(ac1Data, c4Count, eType, c4ScopeId, c4FlowInfo);

        strmToReadFrom >> m_strHostName;
    }
     else
    {
        //
        //  In version 1 we only streamed the IP address, and re-looked up
        //  the host name. Now we store the host name also.
        //
        //  And we have to read in the old IPV4 address and set it up on the
        //  kernel IP. It will have been stored in the network order, which
        //  is what we want to store in the new form.
        //
        tCIDLib::TCard4 c4OldAddr;
        if (c1FmtVersion == 1)
        {
            // For V1, read the addr into a temp and call the setter method
            strmToReadFrom >> c4OldAddr;

            TKrnlIPAddr kipaTmp;
            kipaTmp.bSet(&c4OldAddr, 4, tCIDSock::EAddrTypes::IPV4, 0, 0);
            SetAddr(kipaTmp);
        }
         else
        {
            // For V2, read in the addr and the host name
            strmToReadFrom  >> c4OldAddr
                            >> m_strHostName;

            m_kipaThis.bSet(&c4OldAddr, 4, tCIDSock::EAddrTypes::IPV4, 0, 0);
        }
    }

    if (c1FmtVersion > 1)
    {
        //
        //  Get the text version of the address, if not V1. If V1 we called
        //  SetAddr() to set up the new address and it did this for us. Post
        //  1.0 we don't want to call that because it clears the host name,
        //  and we already went through the trouble of storing the host name
        //  so as not to have to relook it up.
        //
        TKrnlString kstrText;
        if (!TKrnlIP::bTextFromIPAddr(m_kipaThis, kstrText))
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcTCP_CantConvertIPAddr
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        m_strTextVersion = kstrText.pszValue();
    }
}

tCIDLib::TVoid TIPAddress::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << CIDSock_IPAddr::c1IPAddrFmtVersion
                    << m_kipaThis.eType()
                    << m_kipaThis.c4Count()
                    << m_kipaThis.c4ScopeId()
                    << m_kipaThis.c4FlowInfo();

    // Write out the address bytes in network order
    if (m_kipaThis.c4Count())
        strmToWriteTo.c4WriteRawBuffer(m_kipaThis.pc1Data(), m_kipaThis.c4Count());

    strmToWriteTo  << m_strHostName;
}


// ---------------------------------------------------------------------------
//  TIPAddress: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TIPAddress::SetAddr(const TKrnlIPAddr& kipaToSet)
{
    //
    //  Try to get the text version of the address. It could be a textual
    //  address or host.
    //
    TKrnlString kstrText;
    if (!TKrnlIP::bTextFromIPAddr(kipaToSet, kstrText))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_CantConvertIPAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // It went ok, so store it
    m_kipaThis = kipaToSet;
    m_strTextVersion = kstrText.pszValue();

    // Clear out the host name, which will fault back in later if needed
    m_strHostName.Clear();
}


tCIDLib::TVoid
TIPAddress::SetAddr(const   TString&                strIPAddress
                    , const tCIDSock::EAddrTypes    eType)
{
    //
    //  Try to convert the passed address to a binary address. We tell it
    //  to try both direct conversion (if it's a dotted address), or lookup
    //  if necessary. It tells us which one worked.
    //
    //  We work through the facility class' helper, so that we go through
    //  its lookup cache.
    //
    TKrnlIPAddr             kipaTmp;
    const tCIDSock::EAddrCvtRes eRes = facCIDSock().eResolveAddr
    (
        strIPAddress, kipaTmp, eType
    );

    // If it failed, give up
    if (eRes == tCIDSock::EAddrCvtRes::Failed)
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_CantConvertTextAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strIPAddress
        );
    }

    // It went ok, so store the binary address
    m_kipaThis = kipaTmp;

    //
    //  If the address was a numeric, then store it as the numeric text
    //  version, else convert the binary we got to the text.
    //
    if (eRes == tCIDSock::EAddrCvtRes::WasNumeric)
    {
        m_strTextVersion = strIPAddress;
        m_strTextVersion.StripWhitespace();

        //
        //  We did not get a host name, so clear the host name and let it
        //  fault in upon first use.
        //
        m_strHostName.Clear();
    }
     else
    {
        TKrnlString kstrText;
        if (!TKrnlIP::bTextFromIPAddr(kipaTmp, kstrText))
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcTCP_CantConvertIPAddr
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        m_strTextVersion = kstrText.pszValue();

        // Take the passed address as the host
        m_strHostName = strIPAddress;
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TIPEndPoint
//  PREFIX: ipep
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIPEndPoint: Public, static methods
// ---------------------------------------------------------------------------
TIPEndPoint& TIPEndPoint::Nul_TIPEndPoint()
{
    static TIPEndPoint* pipepNull = nullptr;
    if (!pipepNull)
    {
        TBaseLock lockInit;
        if (!pipepNull)
            TRawMem::pExchangePtr(&pipepNull, new TIPEndPoint);
    }
    return *pipepNull;
}



// ---------------------------------------------------------------------------
//  TIPEndPoint: Constructors and Destructor
// ---------------------------------------------------------------------------
TIPEndPoint::TIPEndPoint() :

    m_ippnThis(0)
{
}

TIPEndPoint::TIPEndPoint(const  TString&                strEncodedEndPoint
                        , const tCIDSock::EAddrTypes    eType) :

    TIPAddress()
    , m_ippnThis(0)
{
    FromEncodedStr(strEncodedEndPoint, eType);
}

TIPEndPoint::TIPEndPoint(const  TString&                strIPAddressString
                        , const tCIDLib::TIPPortNum     ippnThis
                        , const tCIDSock::EAddrTypes    eType) :

    TIPAddress(strIPAddressString, eType)
    , m_ippnThis(ippnThis)
{
}

TIPEndPoint::TIPEndPoint(const  TKrnlIPAddr&        kipaThis
                        , const tCIDLib::TIPPortNum ippnThis) :

    TIPAddress(kipaThis)
    , m_ippnThis(ippnThis)
{
}

TIPEndPoint::TIPEndPoint(const  TIPAddress&         ipaThis
                        , const tCIDLib::TIPPortNum ippnThis) :

    TIPAddress(ipaThis)
    , m_ippnThis(ippnThis)
{
}

TIPEndPoint::TIPEndPoint(const  tCIDSock::ESpecAddrs eAddr
                        , const tCIDSock::EAddrTypes    eType
                        , const tCIDLib::TIPPortNum     ippnThis) :

    TIPAddress(eAddr, eType)
    , m_ippnThis(ippnThis)
{
}


//
//  This one is for when they already know botht he host name and the IP
//  address of the target and can set it directly. So we don't end up doing
//  any name resolution or anything.
//
TIPEndPoint::TIPEndPoint(const  TString&            strHostName
                        , const TKrnlIPAddr&        kipaThis
                        , const tCIDLib::TIPPortNum ippnThis) :

    TIPAddress(strHostName, kipaThis)
    , m_ippnThis(ippnThis)
{
}

TIPEndPoint::~TIPEndPoint()
{
}


// ---------------------------------------------------------------------------
//  TIPEndPoint: Public operators
// ---------------------------------------------------------------------------

// The address type cannot be specified if done this way
TIPEndPoint& TIPEndPoint::operator=(const TString& strEncodedEndPoint)
{
    FromEncodedStr(strEncodedEndPoint, tCIDSock::EAddrTypes::Unspec);
    return *this;
}


tCIDLib::TBoolean TIPEndPoint::operator==(const TIPEndPoint& ipepToCompare) const
{
    if (this == &ipepToCompare)
        return kCIDLib::True;

    // Compare our parent first
    if (!TParent::operator==(ipepToCompare))
        return kCIDLib::False;

    return (m_ippnThis == ipepToCompare.m_ippnThis);
}

tCIDLib::TBoolean
TIPEndPoint::operator!=(const TIPEndPoint& ipepToCompare) const
{
    return !operator==(ipepToCompare);
}


//
//  Allow comparison against just the parent class, i.e. do we have the same
//  address as the passed TIPAddress object, or not.
//
tCIDLib::TBoolean TIPEndPoint::operator==(const TIPAddress& ipaToCompare) const
{
    return TParent::operator==(ipaToCompare);
}

tCIDLib::TBoolean
TIPEndPoint::operator!=(const TIPAddress& ipaToCompare) const
{
    return !TParent::operator==(ipaToCompare);
}


// ---------------------------------------------------------------------------
//  TIPEndPoint: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This one does a fancy format of the end point. If you just stream us to
//  a text output stream it'll be in the form x.x.x.x:y. This guy will see
//  if it can get the host name and format it like:
//
//      hostname.com (x.x.x.x:y)
//
//  BE CAREFUL about using this one since it forces a reverse DNS!
//
tCIDLib::TVoid
TIPEndPoint::FormatFancy(       TString&            strToFill
                        , const tCIDLib::TBoolean   bAppend) const
{
    // If not appending, then clear it
    if (!bAppend)
        strToFill.Clear();

    // Get the host name first
    strToFill.Append(kCIDLib::chOpenBracket);
    strToFill.Append(strHostName());

    //
    //  Now compare this to the dotted text version. If they are the same,
    //  then we've already tried the reverse DNS and it failed so the host
    //  name got set to the dotted text and we'll fall back to the standard
    //  format.
    //
    if (strToFill == strAsText())
    {
        strToFill.Append(kCIDLib::chColon);
        strToFill.AppendFormatted(m_ippnThis);
    }
     else
    {
        strToFill.Append(L" - ");
        strToFill.Append(strAsText());
        strToFill.Append(kCIDLib::chColon);
        strToFill.AppendFormatted(m_ippnThis);
    }
    strToFill.Append(kCIDLib::chCloseBracket);
}


//
//  This one does a simple format of the end point. This is the same as if we
//  were streamed, but just goes straight to a string, because this object is
//  so often formatted for display. This just saves the caller the overhead of
//  creating a stream.
//
tCIDLib::TVoid
TIPEndPoint::FormatPlain(       TString&            strToFill
                        , const tCIDLib::TBoolean   bAppend) const
{
    // If not appending, then clear it
    if (!bAppend)
        strToFill.Clear();

    strToFill.Append(strAsText());
    strToFill.Append(kCIDLib::chColon);
    strToFill.AppendFormatted(m_ippnThis);
}


//
//  Create a hash of our end point info, using the passed modulues. Note that
//  we stuff below should be compatible with the pre-4.2 stuff before we
//  updated for IPV4/V6 agnosticism. It should give the same result for an
//  IPV4 end point as the old scheme did.
//
tCIDLib::THashVal
TIPEndPoint::hshEndPoint(const tCIDLib::TCard4 c4Modulus) const
{
    //
    //  We have to come up with some sort of hash here. We will use the bytes
    //  of the IP address data and the port number. Put
    //
    tCIDLib::TCard1 ac1Data[kCIDSock::c4MaxIPAddrBytes + 4];
    tCIDLib::TCard4 c4Sz = kipaThis().c4Count();

    // Put the IP address bytes in first
    TRawMem::CopyMemBuf(ac1Data, kipaThis().pc1Data(), c4Sz);

    //
    //  The address is in a fixed network order, but we have to get the
    //  port number into a canonical order so that they hash is consistent
    //  across platforms. We use little endian mode here.
    //
    tCIDLib::TCard4 c4Port;
    #if defined(CIDLIB_BIGENDIAN)
    c4Port = TRawBits::c4SwapBytes(m_ippnThis);
    #else
    c4Port = m_ippnThis;
    #endif

    TRawMem::CopyMemBuf(&ac1Data[c4Sz], &c4Port, 4);
    c4Sz += 4;

    return TRawMem::hshHashBuffer(ac1Data, c4Modulus, c4Sz);
}


tCIDLib::TIPPortNum TIPEndPoint::ippnThis() const
{
    return m_ippnThis;
}

tCIDLib::TIPPortNum TIPEndPoint::ippnThis(const tCIDLib::TIPPortNum ippnNew)
{
    m_ippnThis = ippnNew;
    return m_ippnThis;
}


// ---------------------------------------------------------------------------
//  TIPEndPoint: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TIPEndPoint::FormatTo(TTextOutStream& strmDest) const
{
    // Call our parent first
    TParent::FormatTo(strmDest);

    // And dump our port number, separated by a colon
    strmDest << kCIDLib::chColon << TCardinal(m_ippnThis);
}


tCIDLib::TVoid TIPEndPoint::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Do our parent first
    TParent::StreamFrom(strmToReadFrom);

    // Check the format version
    tCIDLib::TCard1 c1FmtVersion;
    strmToReadFrom  >> c1FmtVersion;
    if (c1FmtVersion != CIDSock_IPAddr::c1EndPntFmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1FmtVersion)
            , clsThis()
        );
    }

    // And stream in our port number
    strmToReadFrom >> m_ippnThis;
}

tCIDLib::TVoid TIPEndPoint::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Do our parent first
    TParent::StreamTo(strmToWriteTo);

    // And stream out our format version and port number
    strmToWriteTo   << CIDSock_IPAddr::c1EndPntFmtVersion
                    << m_ippnThis;
}


// ---------------------------------------------------------------------------
//  TIPEndPoint: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TIPEndPoint::FromEncodedStr(const   TString&                strEncodedEndPoint
                            , const tCIDSock::EAddrTypes    eType)
{
    //
    //  It has to be in the form addr:port, so check for the last colon and throw
    //  if not found. It has to be the last one, since the address could include
    //  a colon, i.e. a V6 address
    //
    tCIDLib::TCard4 c4Ofs;
    if (!strEncodedEndPoint.bLastOccurrence(L':', c4Ofs))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_BadEndPointFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strEncodedEndPoint
        );
    }

    //
    //  Copy out the stuff before the colon to a temp and set it on our
    //  parent class.
    //
    TString strAddr;
    strEncodedEndPoint.CopyOutSubStr(strAddr, 0, c4Ofs);
    strFromText(strAddr, eType);

    // Now get the stuff past the colon, and convert as a port number
    strEncodedEndPoint.CopyOutSubStr(strAddr, c4Ofs + 1);

    tCIDLib::TBoolean bValid;
    m_ippnThis = TRawStr::c4AsBinary(strAddr.pszBuffer(), bValid, tCIDLib::ERadices::Dec);
    if (!bValid)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_BadEndPointFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strEncodedEndPoint
        );
    }
}

