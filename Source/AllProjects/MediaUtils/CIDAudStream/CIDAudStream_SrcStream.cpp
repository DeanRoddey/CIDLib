//
// FILE NAME: CIDAudStream_SrcStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
//  This file implements the wee bit of non-virtual functionality of our base
//  audio source stream.
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
#include    "CIDAudStream_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDAudioSrcStream, TObject)
RTTIDecls(TCIDAudioStreamInfo, TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDAudStream_SrcStream
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TCIDAudioStreamInfo
//  PREFIX: asinfo
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDAudioStreamInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
TCIDAudioStreamInfo::TCIDAudioStreamInfo(const  TString&    strName
                                        , const TString&    strId) :

    m_strId(strId)
    , m_strName(strName)
{
}

// ---------------------------------------------------------------------------
//  TCIDAudioStreamInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDAudioStreamInfo::Set(const   TString&            strName
                                        , const TString&            strId)
{
    m_strName = strName;
    m_strId = strId;
}


// ---------------------------------------------------------------------------
//  TCIDAudioStreamInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDAudioStreamInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDAudStream_SrcStream::c2FmtVersion)
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

    // Looks ok, so stream our other stuff in
    strmToReadFrom  >> m_strName
                    >> m_strId;

    // And it should end with an end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TCIDAudioStreamInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDAudStream_SrcStream::c2FmtVersion
                    << m_strName
                    << m_strId
                    << tCIDLib::EStreamMarkers::EndObject;
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDAudioSrcStream
//  PREFIX: audss
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TCIDAudioSrcStream: Constructors and Destructor
// -------------------------------------------------------------------
TCIDAudioSrcStream::TCIDAudioSrcStream() :

    m_eState(tCIDAudStream::EStrmStates::WaitConnect)
{
}

TCIDAudioSrcStream::~TCIDAudioSrcStream()
{
}


// -------------------------------------------------------------------
//  TCIDAudioSrcStream: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDAudStream::EStrmStates TCIDAudioSrcStream::eState() const
{
    return m_eState;
}

tCIDAudStream::EStrmStates
TCIDAudioSrcStream::eState(const tCIDAudStream::EStrmStates eState)
{
    m_eState = eState;
    return m_eState;
}

