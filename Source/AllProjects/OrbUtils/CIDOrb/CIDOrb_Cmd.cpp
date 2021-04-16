//
// FILE NAME: CIDOrb_Command.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/06/2000
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
//  This is implementation file for the command class, which is used to
//  send commands to the server. Most of the work is provided by our parent
//  class, TOrbReply. The command is just a subset of the reply, with just
//  the addition of the interface key.
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
RTTIDecls(TOrbCmd,TOrbReply)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDOrb_Cmd
{
    namespace
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TOrbCmd
//  PREFIX: ocmd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbCmd: Constructor and Destructor
// ---------------------------------------------------------------------------
TOrbCmd::TOrbCmd(const tCIDLib::TCard4 c4Size) :

    TParent(c4Size)
    , m_eMode(tCIDOrb::ECmdModes::Cmd)
{
}

TOrbCmd::~TOrbCmd()
{
}


// ---------------------------------------------------------------------------
//  TOrbCmd: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbCmd::Reset(const tCIDLib::TCard4 c4Size)
{
    // Put us back into command mode and then call our parent
    m_eMode = tCIDOrb::ECmdModes::Cmd;
    TParent::Reset(c4Size);
}


// ---------------------------------------------------------------------------
//  TOrbCmd: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TOrbId& TOrbCmd::oidKey() const
{
    return m_oidTarget;
}


tCIDLib::TVoid TOrbCmd::SetCmdMode()
{
    m_eMode = tCIDOrb::ECmdModes::Cmd;
}


tCIDLib::TVoid TOrbCmd::SetTarget(const TOrbId& oidToSet)
{
    m_oidTarget = oidToSet;
}


tCIDLib::TVoid TOrbCmd::SetReplyMode()
{
    m_eMode = tCIDOrb::ECmdModes::Reply;
}


// ---------------------------------------------------------------------------
//  TOrbCmd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbCmd::StreamFrom(TBinInStream& strmToReadFrom)
{
    // If we are in command mode, then get out the target
    if (m_eMode == tCIDOrb::ECmdModes::Cmd)
    {
        // We should get a start object marker
        strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

        // Check the format version
        tCIDLib::TCard2 c2FmtVersion;
        strmToReadFrom  >> c2FmtVersion;
        if (c2FmtVersion != CIDOrb_Cmd::c2FmtVersion)
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

        // Next should come our object id target
        strmToReadFrom >> m_oidTarget;

        // And our part should end with a frame marker
        strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);
    }

    // Now call our parent class to do the rest
    TParent::StreamFrom(strmToReadFrom);
}


//
//  IMPORTANT! We set aside in the reply classes c4OutBytes() method
//  some extra bytes to more than cover the stuff that we are going to
//  wrap around the payload data here and in the derived class. This
//  doesn't have to be exact, it's just to give the ORB an idea how
//  large the data is going to be, for buffer management purposes. But
//  it has to be at least as large as the data we are going to require.
//
//  So if you change this, you must account for that in the extra byte
//  count added by c4OutBytes().
//
tCIDLib::TVoid TOrbCmd::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // If we are in command mode, then stream out the target
    if (m_eMode == tCIDOrb::ECmdModes::Cmd)
    {
        strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                        << CIDOrb_Cmd::c2FmtVersion
                        << m_oidTarget
                        << tCIDLib::EStreamMarkers::Frame;
    }

    // And call our parent for the rest
    TParent::StreamTo(strmToWriteTo);
}

