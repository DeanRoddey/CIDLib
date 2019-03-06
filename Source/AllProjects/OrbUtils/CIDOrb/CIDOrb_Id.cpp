//
// FILE NAME: CIDOrb_Id.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TOrbId class. An ORB id uniquely represents a
//  specific object interface at runtime.
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
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbId,TObject)
RTTIDecls(TOrbIdKeyOps,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDOrb_Id
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TOrbIdKeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbIdKeyOps: Constructors and destructor
// ---------------------------------------------------------------------------
TOrbIdKeyOps::TOrbIdKeyOps()
{
}

TOrbIdKeyOps::TOrbIdKeyOps(const TOrbIdKeyOps&)
{
}

TOrbIdKeyOps::~TOrbIdKeyOps()
{
}


// ---------------------------------------------------------------------------
//  TOrbIdKeyOps: Public operators
// ---------------------------------------------------------------------------
TOrbIdKeyOps& TOrbIdKeyOps::operator=(const TOrbIdKeyOps&)
{
    return *this;
}


// ---------------------------------------------------------------------------
//  TOrbIdKeyOps: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TOrbIdKeyOps::bCompKeys(const TOrbId& oid1, const TOrbId& oid2) const
{
    return (oid1 == oid2);
}

tCIDLib::THashVal
TOrbIdKeyOps::hshKey(const  TOrbId&         oidToHash
                    , const tCIDLib::TCard4 c4Modulus) const
{
    //
    //  If the modulus is the one used internally, then just return the hash
    //  that the id object already has.
    //
    if (c4Modulus == kCIDOrb::c4IdModulus)
        return oidToHash.hshThis();

    // Else hash it using the caller's modulus
    return oidToHash.hshCalcHash(c4Modulus);
}





// ---------------------------------------------------------------------------
//   CLASS: TOrbId
//  PREFIX: oid
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbId: Constructors and Destructor
// ---------------------------------------------------------------------------
TOrbId::TOrbId() :

    m_hshThis(0)
{
}

TOrbId::TOrbId( const   TString&    strInterface
                , const TMD5Hash&   mhashInstance)
{
    // Call the set method, since it does what we need
    Set(strInterface, mhashInstance);
}

TOrbId::TOrbId( const   TMD5Hash&   mhashInterface
                , const TMD5Hash&   mhashInstance)
{
    // Call the set method, since it does what we need
    Set(mhashInterface, mhashInstance);
}

TOrbId::TOrbId( const   TString&    strInterface
                , const TString&    strInstance)
{
    // Call the set method, since it does what we need
    Set(strInterface, strInstance);
}

TOrbId::TOrbId(const TOrbId& oidToCopy) :

    m_mhashInterface(oidToCopy.m_mhashInterface)
    , m_mhashInstance(oidToCopy.m_mhashInstance)
{
    // Pre-calc our hash
    m_hshThis = hshDoHashCalc(kCIDOrb::c4IdModulus);
}

TOrbId::~TOrbId()
{
}


// ---------------------------------------------------------------------------
//  TOrbId: Public operators
// ---------------------------------------------------------------------------
TOrbId& TOrbId::operator=(const TOrbId& oidToAssign)
{
    if (&oidToAssign == this)
        return *this;

    m_hshThis        = oidToAssign.m_hshThis;
    m_mhashInterface = oidToAssign.m_mhashInterface;
    m_mhashInstance  = oidToAssign.m_mhashInstance;

    return *this;
}


tCIDLib::TBoolean TOrbId::operator==(const TOrbId& oidToCompare) const
{
    // Test the hash first
    if (m_hshThis != oidToCompare.m_hshThis)
        return kCIDLib::False;

    // Test for self comparison
    if (&oidToCompare == this)
        return kCIDLib::True;

    // Oh well, lets compare the two hashes
    if (m_mhashInterface != oidToCompare.m_mhashInterface)
        return kCIDLib::False;
    if (m_mhashInstance != oidToCompare.m_mhashInstance)
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TOrbId::operator!=(const TOrbId& oidToCompare) const
{
    return !operator==(oidToCompare);
}


// ---------------------------------------------------------------------------
//  TOrbId: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TOrbId::bIsInterfaceId(const TString& strToTest) const
{
    // Convert the passed string to a MD5Hash
    TMD5Hash mhashTmp;
    mhashTmp.ParseFormatted(strToTest);

    // And compare to our interface id
    return (m_mhashInterface == mhashTmp);
}


tCIDLib::THashVal TOrbId::hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
{
    //
    //  If the modulus is the one used internally, then just return the hash
    //  that the we already have calculated.
    //
    if (c4Modulus == kCIDOrb::c4IdModulus)
        return m_hshThis;

    // Else hash it using the caller's hash
    return hshDoHashCalc(c4Modulus);
}


tCIDLib::THashVal TOrbId::hshThis() const
{
    return m_hshThis;
}


const TMD5Hash& TOrbId::mhashInterface() const
{
    return m_mhashInterface;
}

const TMD5Hash& TOrbId::mhashInstance() const
{
    return m_mhashInstance;
}


tCIDLib::TVoid TOrbId::Set( const   TString&    strInterface
                            , const TMD5Hash&   mhashInstance)
{
    m_mhashInterface.ParseFormatted(strInterface);
    m_mhashInstance = mhashInstance;

    // Pre-calc our hash
    m_hshThis = hshDoHashCalc(kCIDOrb::c4IdModulus);
}

tCIDLib::TVoid TOrbId::Set( const   TMD5Hash&   mhashInterface
                            , const TMD5Hash&   mhashInstance)
{
    m_mhashInterface = mhashInterface;
    m_mhashInstance   = mhashInstance;

    // Pre-calc our hash
    m_hshThis = hshDoHashCalc(kCIDOrb::c4IdModulus);
}

tCIDLib::TVoid TOrbId::Set( const   TString&    strInterface
                            , const TString&    strInstance)
{
    m_mhashInterface.ParseFormatted(strInterface);
    m_mhashInstance.ParseFormatted(strInstance);

    // Pre-calc our hash
    m_hshThis = hshDoHashCalc(kCIDOrb::c4IdModulus);
}


// ---------------------------------------------------------------------------
//  TOrbId: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbId::FormatTo(TTextOutStream& strmDest) const
{
    strmDest << L"{" << m_mhashInterface << L"}" << m_mhashInstance;
}


tCIDLib::TVoid TOrbId::StreamFrom(TBinInStream& strmToReadFrom)
{
    // We should get a start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDOrb_Id::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    strmToReadFrom  >> m_hshThis
                    >> m_mhashInterface
                    >> m_mhashInstance;

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TOrbId::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDOrb_Id::c2FmtVersion
                    << m_hshThis
                    << m_mhashInterface
                    << m_mhashInstance
                    << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TOrbId: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::THashVal TOrbId::hshDoHashCalc(const tCIDLib::TCard4 c4Modulus) const
{
    //
    //  We need to get the two ids into one buffer so that we can hash the
    //  whole combined set of bytes.
    //
    tCIDLib::TCard1 ac1Tmp[kCIDCrypto::c4UniqueIdBytes << 1];

    // Sanity check
    #if CID_DEBUG_ON
    if ((m_mhashInterface.c4Bytes() != kCIDCrypto::c4UniqueIdBytes)
    ||  (m_mhashInstance.c4Bytes() != kCIDCrypto::c4UniqueIdBytes))
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcId_BadHashLen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    // Copy in the interface bytes first
    TRawMem::CopyMemBuf
    (
        ac1Tmp
        , m_mhashInterface.pc1Buffer()
        , kCIDCrypto::c4UniqueIdBytes
    );

    // And after that the instance bytes
    TRawMem::CopyMemBuf
    (
        &ac1Tmp[kCIDCrypto::c4UniqueIdBytes]
        , m_mhashInstance.pc1Buffer()
        , kCIDCrypto::c4UniqueIdBytes
    );

    // And hash this buffer and return the result
    return TRawMem::hshHashBuffer
    (
        ac1Tmp
        , c4Modulus
        , kCIDCrypto::c4UniqueIdBytes << 1
    );
}

