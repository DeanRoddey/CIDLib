//
// FILE NAME: CIDLib_StringId.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/2015
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
//  This class defines a simple combination of a string, a numerical id/serial
//  number, and a boolean flag. This is a general purpose construct that lets us
//  avoid a lot of ad hoc grunt work in various places.
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
//   CLASS: TStringId
//  PREFIX: strid
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringId
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringId();

        TStringId
        (
            const   TStringView&            strvText
            , const tCIDLib::TCard4         c4Id
        );

        TStringId(const TStringId&) = default;
        TStringId(TStringId&&) = default;

        ~TStringId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringId& operator=(const TStringId&) = default;
        TStringId& operator=(TStringId&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TStringId&              stridSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TStringId&              stridSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFlag() const;

        tCIDLib::TBoolean bFlag
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        const TString& strText() const;

        const TString& strText
        (
            const   TStringView&            strvToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFlag
        //  m_c4Id
        //  m_strText
        //      The three values are combine.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bFlag;
        tCIDLib::TCard4     m_c4Id;
        TString             m_strText;
};


#pragma CIDLIB_POPPACK

