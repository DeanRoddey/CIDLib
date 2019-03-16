//
// FILE NAME: CIDSIP_Dialog.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2013
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
//  This file implements the TSIPDialog class.
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
#include    "CIDSIP_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSIPDialog, TObject)



// ---------------------------------------------------------------------------
//   CLASS: SIPDialog
//  PREFIX: sipdlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSIPDialog: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Just a default so that lists of these can be created. It sets us up to
//  and invalid state that will cause an error if the Reset() method is not
//  called before it's used.
//
TSIPDialog::TSIPDialog(const tCIDLib::ECSSides eSide) :

    m_c4LocCSeq(0)
    , m_c4RemCSeq(0)
    , m_eSide(eSide)
{
}

TSIPDialog::~TSIPDialog()
{
}


// ---------------------------------------------------------------------------
//  TSIPDialog: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Reset this dalog for use. The user agent that is being used to manage the
//  call will provide the appropriate initial info.
//
tCIDLib::TVoid
TSIPDialog::Reset(const tCIDLib::ECSSides   eSide
                , const TString&            strLocAddr
                , const TString&            strLocContact
                , const TString&            strLocDisplay
                , const TString&            strLocTag
                , const tCIDLib::TCard4     c4LocCSeq)
{
    m_eSide = eSide;

    // Store our local side info
    m_c4LocCSeq     = c4LocCSeq;
    m_strLocAddr    = strLocAddr;
    m_strLocContact = strLocContact;
    m_strLocDisplay = strLocDisplay;
    m_strLocTag     = strLocTag;

    // Clear out any remote info till we get the actual stuff
    m_c4RemCSeq = 0;
    m_strRemAddr.Clear();
    m_strRemContact.Clear();
    m_strRemDisplay.Clear();
    m_strRemTag.Clear();
}

