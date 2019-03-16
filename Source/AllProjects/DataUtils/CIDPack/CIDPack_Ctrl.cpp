//
// FILE NAME: CIDPack_Ctrl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2015
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
//  This file implements classes that describe the control structures in the
//  file.
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
#include    "CIDPack_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macro
// ---------------------------------------------------------------------------
RTTIDecls(TCIDPackFlHdr,TObject)
RTTIDecls(TCIDPackHdr,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDPack_Ctrl
{
    const tCIDLib::TCard1   c1HdrFmtVersion = 1;
    const tCIDLib::TCard1   c1FlFmtVersion = 1;
};




// ---------------------------------------------------------------------------
//  CLASS: TCIDPackFlHdr
// PREFIX: pkhdr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDPackFlHdr: Constructors and Destructor.
// ---------------------------------------------------------------------------
TCIDPackFlHdr::TCIDPackFlHdr() :

    m_c4CompBytes(0)
    , m_c4OrgBytes(0)
{
}

TCIDPackFlHdr::TCIDPackFlHdr(const  tCIDLib::TCard4     c4CompBytes
                            , const tCIDLib::TCard4     c4OrgBytes
                            , const TMD5Hash&           mhashOrg
                            , const TString&            strRelPath) :

    m_c4CompBytes(c4CompBytes)
    , m_c4OrgBytes(c4OrgBytes)
    , m_mhashOrg(mhashOrg)
    , m_strRelPath(strRelPath)
{
}

TCIDPackFlHdr::TCIDPackFlHdr(const TCIDPackFlHdr& pkhdrSrc) :

    m_c4CompBytes(pkhdrSrc.m_c4CompBytes)
    , m_c4OrgBytes(pkhdrSrc.m_c4OrgBytes)
    , m_mhashOrg(pkhdrSrc.m_mhashOrg)
    , m_strRelPath(pkhdrSrc.m_strRelPath)
{
}

TCIDPackFlHdr::~TCIDPackFlHdr()
{
}


// ---------------------------------------------------------------------------
//  TCIDPackFlHdr: Public operators
// ---------------------------------------------------------------------------
TCIDPackFlHdr& TCIDPackFlHdr::operator=(const TCIDPackFlHdr& pkfhdrSrc)
{
    if (this != &pkfhdrSrc)
    {
        m_c4CompBytes   = pkfhdrSrc.m_c4CompBytes;
        m_c4OrgBytes    = pkfhdrSrc.m_c4OrgBytes;
        m_mhashOrg      = pkfhdrSrc.m_mhashOrg;
        m_strRelPath    = pkfhdrSrc.m_strRelPath;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCIDPackFlHdr: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TCIDPackFlHdr::c4CompBytes() const
{
    return m_c4CompBytes;
}


tCIDLib::TCard4 TCIDPackFlHdr::c4OrgBytes() const
{
    return m_c4OrgBytes;
}


const TMD5Hash& TCIDPackFlHdr::mhashOrg() const
{
    return m_mhashOrg;
}


const TString& TCIDPackFlHdr::strRelPath() const
{
    return m_strRelPath;
}



// ---------------------------------------------------------------------------
//  TCIDPackFlHdr: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDPackFlHdr::StreamFrom(TBinInStream& strmToReadFrom)
{
    try
    {
        // Should start with a start marker
        strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

        // Check the format version
        tCIDLib::TCard1 c1FmtVersion;
        strmToReadFrom  >> c1FmtVersion;
        if (!c1FmtVersion && (c1FmtVersion > CIDPack_Ctrl::c1FlFmtVersion))
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

        //
        //  Read in the size values. We also read in their XOR'd values and
        //  validate them.
        //
        tCIDLib::TCard4 c4XComp, c4XOrg;
        strmToReadFrom  >> m_c4CompBytes
                        >> m_c4OrgBytes
                        >> c4XComp
                        >> c4XOrg;

        if ((m_c4CompBytes != (c4XComp ^ kCIDLib::c4MaxCard))
        ||  (m_c4OrgBytes != (c4XOrg ^ kCIDLib::c4MaxCard)))
        {
            facCIDPack().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kPackErrs::errcCtrl_BadFlHdr
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"size")
            );
        }

        // Read in the hash and relative path
        strmToReadFrom  >> m_mhashOrg
                        >> m_strRelPath;

        // And it should end with an end object marker
        strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

}

tCIDLib::TVoid TCIDPackFlHdr::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Start with the usual marker and format version
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDPack_Ctrl::c1FlFmtVersion

                    // The sizes and their XOR'd values
                    << m_c4CompBytes
                    << m_c4OrgBytes
                    << tCIDLib::TCard4(m_c4CompBytes ^ kCIDLib::c4MaxCard)
                    << tCIDLib::TCard4(m_c4OrgBytes ^ kCIDLib::c4MaxCard)

                    << m_mhashOrg
                    << m_strRelPath
                    << tCIDLib::EStreamMarkers::EndObject;
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDPackHdr
// PREFIX: pkhdr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDPackHdr: Constructors and Destructor.
// ---------------------------------------------------------------------------
TCIDPackHdr::TCIDPackHdr() :

    m_c4FileCount(0)
    , m_c4User(0)
    , m_c8User(0)
    , m_c8Version(0)
    , m_enctStamp(0)
{
}

TCIDPackHdr::TCIDPackHdr(const  tCIDLib::TCard4         c4FileCount
                        , const tCIDLib::TCard8         c8Version
                        , const tCIDLib::TEncodedTime   enctStamp) :

    m_c4FileCount(c4FileCount)
    , m_c4User(0)
    , m_c8Version(c8Version)
    , m_c8User(0)
    , m_enctStamp(enctStamp)
{
}

TCIDPackHdr::TCIDPackHdr(const TCIDPackHdr& pkhdrSrc) :

    m_c4FileCount(pkhdrSrc.m_c4FileCount)
    , m_c4User(pkhdrSrc.m_c4User)
    , m_c8User(pkhdrSrc.m_c8User)
    , m_c8Version(pkhdrSrc.m_c8Version)
    , m_enctStamp(pkhdrSrc.m_enctStamp)
    , m_strUser(pkhdrSrc.m_strUser)
{
}

TCIDPackHdr::~TCIDPackHdr()
{
}


// ---------------------------------------------------------------------------
//  TCIDPackHdr: Public operators
// ---------------------------------------------------------------------------
TCIDPackHdr& TCIDPackHdr::operator=(const TCIDPackHdr& pkhdrSrc)
{
    if (this != &pkhdrSrc)
    {
        m_c4FileCount   = pkhdrSrc.m_c4FileCount;
        m_c4User        = pkhdrSrc.m_c4User;
        m_c8User        = pkhdrSrc.m_c8User;
        m_c8Version     = pkhdrSrc.m_c8Version;
        m_enctStamp     = pkhdrSrc.m_enctStamp;
        m_strUser       = pkhdrSrc.m_strUser;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCIDPackHdr: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get the count of files
tCIDLib::TCard4 TCIDPackHdr::c4FileCount() const
{
    return m_c4FileCount;
}


// Get/set the 32 cardinal user value
tCIDLib::TCard4 TCIDPackHdr::c4User() const
{
    return m_c4User;
}

tCIDLib::TCard4 TCIDPackHdr::c4User(const tCIDLib::TCard4 c4ToSet)
{
    m_c4User = c4ToSet;
    return m_c4User;
}



// Get/set the 64 cardinal user value
tCIDLib::TCard8 TCIDPackHdr::c8User() const
{
    return m_c8User;
}

tCIDLib::TCard8 TCIDPackHdr::c8User(const tCIDLib::TCard8 c8ToSet)
{
    m_c8User = c8ToSet;
    return m_c8User;
}


// Get the version
tCIDLib::TCard8 TCIDPackHdr::c8Version() const
{
    return m_c8Version;
}


// Get the time stamp
tCIDLib::TEncodedTime TCIDPackHdr::enctStamp() const
{
    return m_enctStamp;
}


// Get/set the user string value
const TString& TCIDPackHdr::strUser() const
{
    return m_strUser;
}

const TString& TCIDPackHdr::strUser(const TString& strToSet)
{
    m_strUser = strToSet;
    return m_strUser;
}


// ---------------------------------------------------------------------------
//  TCIDPackHdr: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDPackHdr::StreamFrom(TBinInStream& strmToReadFrom)
{
    try
    {
        //
        //  Unlike usual, we write a special marker at the start of the output,
        //  which will be at the start of the file in this case. So read that
        //  in and let's check it.
        //
        tCIDLib::TCard1 ac1Mark[kCIDPack::c4MarkerLen];
        strmToReadFrom.ReadArray(ac1Mark, kCIDPack::c4MarkerLen);

        if (!TRawMem::bCompareMemBuf(ac1Mark, kCIDPack::ac1Marker, kCIDPack::c4MarkerLen))
        {
            facCIDPack().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kPackErrs::errcDbg_NotACIDPack
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        //
        //  Now we can check the normal marker and format version stuff
        //
        strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);
        tCIDLib::TCard1 c1FmtVersion;
        strmToReadFrom  >> c1FmtVersion;
        if (!c1FmtVersion && (c1FmtVersion > CIDPack_Ctrl::c1HdrFmtVersion))
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

        strmToReadFrom  >> m_c8Version
                        >> m_c4FileCount
                        >> m_enctStamp

                        >> m_c4User
                        >> m_c8User
                        >> m_strUser;

        // And it should end with an end object marker
        strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

}

tCIDLib::TVoid TCIDPackHdr::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Normally the first thing written is a marker, but in this case, we
    //  want to write a file signature that is easy to check for and not
    //  likely to be duplicated. That will make it easier to figure out if
    //  a file really is a CIDPack.
    //
    strmToWriteTo.WriteArray(kCIDPack::ac1Marker, kCIDPack::c4MarkerLen);

    // Now write the normal stuff
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDPack_Ctrl::c1HdrFmtVersion

                    << m_c8Version
                    << m_c4FileCount
                    << m_enctStamp

                    << m_c4User
                    << m_c8User
                    << m_strUser

                    << tCIDLib::EStreamMarkers::EndObject;
}


