//
// FILE NAME: CIDSIP_Dialog.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 4/28/2013
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
//  This is the header for the CIDSIP_Dialog.cpp file. This implements the
//  class that is used to track dialogs between end points (a given agent
//  instance has to deal with multiple dialogs, so this stuff can't be
//  done at the agent level itself.
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
//   CLASS: TSIPDialog
//  PREFIX: sipdlg
// ---------------------------------------------------------------------------
class CIDSIPEXP TSIPDialog : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSIPDialog
        (
            const   tCIDLib::ECSSides       eSide
        );

        ~TSIPDialog();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset
        (
            const   tCIDLib::ECSSides       eSide
            , const TString&                strLocAddr
            , const TString&                strLocContact
            , const TString&                strLocDisplay
            , const TString&                strLocTag
            , const tCIDLib::TCard4         c4LocCSeq
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TSIPDialog(const TSIPDialog&);
        tCIDLib::TVoid operator=(const TSIPDialog&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4LocCSeq
        //  m_c4RemCSeq
        //      The local and requote call sequence ids for this dialog. The
        //      local one is set by this dialog. The remote one comes from the
        //      other side and is updated each time we get a new, higher, one
        //      so that we can reject retransmissions.
        //
        //  m_eSide
        //      When we are constructed, the caller indicates if this is
        //      for the server or client side of a dialog. We react differently
        //      depending on the side. This is gotten from the user agent
        //      that creates us.
        //
        //  m_strLocAddr
        //  m_strLocContact
        //  m_strLocDisplay
        //  m_strLocTag
        //      All the local side info. This stuff is set up on us by the
        //      user agent that creates us. How these relate to the To/From
        //      header lines depends on which C/S side we are part of.
        //
        //  m_strRemAddr
        //  m_strRemContact
        //  m_strRemDisplay
        //  m_strRemTag
        //      All the remote side info. This stuff is gotten from the other
        //      side during the first exchanges of the dialog. How these
        //      relate to the To/From header lines depends on which C/S side
        //      we are part of.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4LocCSeq;
        tCIDLib::TCard4     m_c4RemCSeq;
        tCIDLib::ECSSides   m_eSide;
        TString             m_strLocAddr;
        TString             m_strLocContact;
        TString             m_strLocDisplay;
        TString             m_strLocTag;
        TString             m_strRemAddr;
        TString             m_strRemContact;
        TString             m_strRemDisplay;
        TString             m_strRemTag;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSIPDialog, TObject)
};

#pragma CIDLIB_POPPACK


