//
// FILE NAME: CIDOrb_Cmd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2000
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
//  This file defines the format of a command packet that gets sent from
//  the client to the server. It derives from the reply class, which is used
//  to return the reply. The command is a superset of the reply, so this
//  saves a lot of effort and I/O buffer size (since we can use the same
//  object as both the command and the reply, without having to send back
//  the extra stuff in the reply.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TOrbCmd
//  PREFIX: ocmd
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbCmd : public TOrbReply
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TOrbCmd() = delete;

        TOrbCmd
        (
            const   tCIDLib::TCard4         c4InitSz
        );

        TOrbCmd(const TOrbCmd&) = delete;
        TOrbCmd(TOrbCmd&&) = delete;

        ~TOrbCmd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbCmd& operator=(const TOrbCmd&) = delete;
        TOrbCmd& operator=(TOrbCmd&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4Size
        )   final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TOrbId& oidKey() const;

        tCIDLib::TVoid SetCmdMode();

        tCIDLib::TVoid SetTarget
        (
            const   TOrbId&                 oidToSet
        );

        tCIDLib::TVoid SetReplyMode();


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
        //  m_eMode
        //      When we send out the command, we put it into Cmd mode. This
        //      causes it to spool in/out all of the data at this level. When
        //      we reply, we set it to the reply mode, which causes it to not
        //      try to do any data at this level, and just spool in/out the
        //      underlying parent class' data.
        //
        //  m_oidTarget
        //      This is the low level id of the object we are targeting. We
        //      get it out of the target object id we are given.
        // -------------------------------------------------------------------
        tCIDOrb::ECmdModes  m_eMode;
        TOrbId              m_oidTarget;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbCmd,TOrbReply)
        BefriendFactory(TOrbCmd)
};

#pragma CIDLIB_POPPACK

