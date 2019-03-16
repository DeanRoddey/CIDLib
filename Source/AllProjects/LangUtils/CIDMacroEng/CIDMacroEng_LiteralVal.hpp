//
// FILE NAME: CIDMacroEng_LiteralVal.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/20/2003
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
//  This is the header file for the CIDMacroEng_LiteralVal.cpp file, which
//  implements the TMEngLiteralVal class. When a class declares a literal,
//  we have to store some information about it in the class object, to keep
//  up with the name of the member, it's class type, and it's value.
//
//  Note that literals are implictly constant.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngOpMethodImpl;

// ---------------------------------------------------------------------------
//  CLASS: TMEngLiteralVal
// PREFIX: melv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngLiteralVal : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TMEngLiteralVal& meviSrc);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngLiteralVal();

        TMEngLiteralVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            ,       TMEngClassVal* const    pmecvToAdopt
        );

        TMEngLiteralVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics eClassType
            ,       TMEngClassVal* const    pmecvToAdopt
        );

        TMEngLiteralVal(const TMEngLiteralVal&) = delete;

        ~TMEngLiteralVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngLiteralVal& operator=(const TMEngLiteralVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2ClassId() const;

        tCIDMacroEng::EExprTypes ePushImmediate
        (
                    TMEngOpMethodImpl&      methToFill
            , const tCIDLib::TBoolean       bAvoidDups
        )   const;

        tCIDLib::TVoid FormatToStr
        (
                    TString&                strToFill
        )   const;

        TMEngClassVal* pmecvMakeNew
        (
            const   TString&                strName
        )   const;

        const TString& strValueAs() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ClassId
        //      The id of the class of this literal. It will always be one of
        //      the fundamental, char, bool, string, numeric types.
        //
        //  m_pmecvLiteral
        //      The value of the literal. We just use the already existing
        //      class value object to store them, since we have to store
        //      polymorphically anyway.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2ClassId;
        TMEngClassVal*      m_pmecvLiteral;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngLiteralVal,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK


