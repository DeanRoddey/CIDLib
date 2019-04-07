//
// FILE NAME: ORB1SampSh_Record.hpp
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
//  This is the header for our pseudo patient record. It just has a little bit of
//  info that would be representative of such things. It implements the streamable
//  interface so it can be used as remote method call parameters.
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
//   CLASS: TORB1SampRec
//  PREFIX: rec
// ---------------------------------------------------------------------------
class ORB1SAMPSHEXP TORB1SampRec :   public TObject
                                    , public MStreamable
                                    , public MFormattable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TORB1SampRec();

        TORB1SampRec
        (
            const   TString&                strFirst
            , const TString&                strLast
            , const tORB1SampSh::EStatus    eStatus
            , const tCIDLib::TCard4         c4Age
            , const tCIDLib::TCard4         c4RecordNum
        );

        TORB1SampRec(const TORB1SampRec& recSrc) = default;

        ~TORB1SampRec();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TORB1SampRec& operator=(const TORB1SampRec& recSrc) = default;


        // -------------------------------------------------------------------
        //  Public data members (usually private but for this simple exmaple)
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4RecordNum;
        tCIDLib::TCard4         m_c4Age;
        tORB1SampSh::EStatus    m_eStatus;
        TString                 m_strFullName;
        TString                 m_strFirst;
        TString                 m_strLast;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private:
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TORB1SampRec,TObject)
};

#pragma CIDLIB_POPPACK
