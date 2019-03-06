//
// FILE NAME: CIDLib_StringId.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
//  PREFIX: bts
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
            const   TString&                strText
            , const tCIDLib::TCard4         c4Id
        );

        TStringId
        (
            const   tCIDLib::TCh* const     pszText
            , const tCIDLib::TCard4         c4Id

        );

        TStringId
        (
            const   TStringId&              stridSrc
        );

        ~TStringId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringId& operator=
        (
            const   TStringId&              stridSrc
        );

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
            const   TString&                strToSet
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

