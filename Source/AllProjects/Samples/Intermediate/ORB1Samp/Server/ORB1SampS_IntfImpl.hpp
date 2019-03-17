//
// FILE NAME: ORB1SampS_IntfImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2019
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
//  This is the header for our implementation of the server side ORB1 sample
//  interface. We just have to override the virtual methods that define the
//  interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TORB1SampIntfImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class TORB1SampIntfImpl : public TORB1SampServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TORB1SampIntfImpl() {}

        TORB1SampIntfImpl(const TORB1SampIntfImpl&) = delete;

        ~TORB1SampIntfImpl() {}

        // -------------------------------------------------------------------
        //  Public, inherited methods
        //
        //  Inherited from the ORB generated server side stub
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryByName
        (
            const   TString&                strToFind
            ,       TORB1SampRec&           recFound
        )   override;

        tCIDLib::TBoolean bQueryAllStatus
        (
            const   tORB1SampSh::EStatus    eToFind
            ,       TVector<TORB1SampRec>&  colFound
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate()  override;

    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colPatients
        //      Filled with some dummy records for this demo. So this is our 'patient
        //      database.'
        // -------------------------------------------------------------------
        TVector<TORB1SampRec> m_colPatients;

        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TORB1SampIntfImpl,TORB1SampServerBase)
};

