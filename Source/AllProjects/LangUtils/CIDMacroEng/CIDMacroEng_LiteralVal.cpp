//
// FILE NAME: CIDMacroEng_LiteralVal.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/20/2003
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
//  This file implements TMEngLiteralVal class. This class represents a
//  literal declared in a class.
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngLiteralVal,TMacroEngNamedItem)


// ---------------------------------------------------------------------------
//  CLASS: TMEngLiteralVal
// PREFIX: melv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngLiteralVal: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngLiteralVal::strKey(const TMEngLiteralVal& meviSrc)
{
    return meviSrc.strName();
}


// ---------------------------------------------------------------------------
//  TMEngLiteralVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngLiteralVal::TMEngLiteralVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2ClassId
                                ,       TMEngClassVal* const    pmecvToAdopt) :
    TMEngNamedItem(strName)
    , m_c2ClassId(c2ClassId)
    , m_pmecvLiteral(pmecvToAdopt)
{
}

TMEngLiteralVal::TMEngLiteralVal(const  TString&                    strName
                                , const tCIDMacroEng::EIntrinsics    eClassType
                                ,       TMEngClassVal* const        pmecvToAdopt) :
    TMEngNamedItem(strName)
    , m_c2ClassId(tCIDLib::TCard2(eClassType))
    , m_pmecvLiteral(pmecvToAdopt)
{
}

TMEngLiteralVal::~TMEngLiteralVal()
{
    delete m_pmecvLiteral;
}


// ---------------------------------------------------------------------------
//  TMEngLiteralVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngLiteralVal::c2ClassId() const
{
    return m_c2ClassId;
}


tCIDMacroEng::EExprTypes
TMEngLiteralVal::ePushImmediate(        TMEngOpMethodImpl&  methToFill
                                , const tCIDLib::TBoolean   bAvoidDups) const
{
    tCIDMacroEng::EExprTypes eRet;
    TMEngOpCode meopTmp;
    switch(tCIDMacroEng::EIntrinsics(m_c2ClassId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            eRet = tCIDMacroEng::EExprTypes::BoolLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImBoolean
                , static_cast<TMEngBooleanVal*>(m_pmecvLiteral)->bValue()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Char :
            eRet = tCIDMacroEng::EExprTypes::CharLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImChar
                , static_cast<TMEngCharVal*>(m_pmecvLiteral)->chValue()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImCard1
                , static_cast<TMEngCard1Val*>(m_pmecvLiteral)->c1Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImCard2
                , static_cast<TMEngCard2Val*>(m_pmecvLiteral)->c2Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImCard4
                , static_cast<TMEngCard4Val*>(m_pmecvLiteral)->c4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImCard8
                , static_cast<TMEngCard8Val*>(m_pmecvLiteral)->c8Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImFloat4
                , static_cast<TMEngFloat4Val*>(m_pmecvLiteral)->f4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImFloat8
                , static_cast<TMEngFloat8Val*>(m_pmecvLiteral)->f8Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImInt1
                , static_cast<TMEngInt1Val*>(m_pmecvLiteral)->i1Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImInt2
                , static_cast<TMEngInt2Val*>(m_pmecvLiteral)->i2Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            eRet = tCIDMacroEng::EExprTypes::NumLiteral;
            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushImInt4
                , static_cast<TMEngInt4Val*>(m_pmecvLiteral)->i4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::String :
        {
            //
            //  This one is a special case. We have to put a string literal
            //  into the target method and issue the opcode to push that
            //  string literal id.
            //
            eRet = tCIDMacroEng::EExprTypes::StringLiteral;
            const tCIDLib::TCard2 c2StrId = methToFill.c2AddString
            (
                static_cast<TMEngStringVal*>(m_pmecvLiteral)->strValue()
                , bAvoidDups
            );

            meopTmp.SetImmediate
            (
                tCIDMacroEng::EOpCodes::PushStrPoolItem
                , c2StrId
            );
            break;
        }

        default :
            // Make the compiler happy
            eRet = tCIDMacroEng::EExprTypes::StringLiteral;
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcDbg_BadLiteralType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_c2ClassId)
            );
            break;
    };

    // Add the opcode to the method
    methToFill.c4AddOpCode(meopTmp);

    return eRet;
}


tCIDLib::TVoid TMEngLiteralVal::FormatToStr(TString& strToFill) const
{
    strToFill.Clear();
    TMEngClassVal* pmecvRet = 0;
    switch(tCIDMacroEng::EIntrinsics(m_c2ClassId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<TMEngBooleanVal*>(m_pmecvLiteral)->bValue())
                strToFill = L"True";
            else
                strToFill = L"False";
            break;

        case tCIDMacroEng::EIntrinsics::Char :
            strToFill = L"0x";
            strToFill.AppendFormatted
            (
                tCIDLib::TCard4(static_cast<TMEngCharVal*>(m_pmecvLiteral)->chValue())
                , tCIDLib::ERadices::Hex
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            strToFill.AppendFormatted(static_cast<TMEngCard1Val*>(m_pmecvLiteral)->c1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            strToFill.AppendFormatted(static_cast<TMEngCard2Val*>(m_pmecvLiteral)->c2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            strToFill.AppendFormatted(static_cast<TMEngCard4Val*>(m_pmecvLiteral)->c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            strToFill.AppendFormatted(static_cast<TMEngCard8Val*>(m_pmecvLiteral)->c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            strToFill.AppendFormatted(static_cast<TMEngFloat4Val*>(m_pmecvLiteral)->f4Value(), 2);
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            strToFill.AppendFormatted(static_cast<TMEngFloat8Val*>(m_pmecvLiteral)->f8Value(), 2);
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            strToFill.AppendFormatted(static_cast<TMEngInt1Val*>(m_pmecvLiteral)->i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            strToFill.AppendFormatted(static_cast<TMEngInt2Val*>(m_pmecvLiteral)->i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            strToFill.AppendFormatted(static_cast<TMEngInt4Val*>(m_pmecvLiteral)->i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::String :
            strToFill = static_cast<TMEngStringVal*>(m_pmecvLiteral)->strValue();
            break;

        default :
            #if CID_DEBUG_ON
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcDbg_BadLiteralType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_c2ClassId)
            );
            #endif
            break;
    };
}


TMEngClassVal* TMEngLiteralVal::pmecvMakeNew(const TString& strName) const
{
    TMEngClassVal* pmecvRet = 0;
    switch(tCIDMacroEng::EIntrinsics(m_c2ClassId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            pmecvRet = new TMEngBooleanVal
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngBooleanVal*>(m_pmecvLiteral)->bValue()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Char :
            pmecvRet = new TMEngCharVal
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngCharVal*>(m_pmecvLiteral)->chValue()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            pmecvRet = new TMEngCard1Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngCard1Val*>(m_pmecvLiteral)->c1Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            pmecvRet = new TMEngCard2Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngCard2Val*>(m_pmecvLiteral)->c2Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            pmecvRet = new TMEngCard4Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngCard4Val*>(m_pmecvLiteral)->c4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            pmecvRet = new TMEngCard8Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngCard8Val*>(m_pmecvLiteral)->c8Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            pmecvRet = new TMEngFloat4Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngFloat4Val*>(m_pmecvLiteral)->f4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            pmecvRet = new TMEngFloat8Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngFloat8Val*>(m_pmecvLiteral)->f8Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            pmecvRet = new TMEngInt1Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngInt1Val*>(m_pmecvLiteral)->i1Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            pmecvRet = new TMEngInt2Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngInt2Val*>(m_pmecvLiteral)->i2Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            pmecvRet = new TMEngInt4Val
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngInt4Val*>(m_pmecvLiteral)->i4Value()
            );
            break;

        case tCIDMacroEng::EIntrinsics::String :
            pmecvRet = new TMEngStringVal
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , static_cast<TMEngStringVal*>(m_pmecvLiteral)->strValue()
            );
            break;

        default :
            #if CID_DEBUG_ON
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcDbg_BadLiteralType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_c2ClassId)
            );
            #endif
            break;
    };
    return pmecvRet;
}


const TString& TMEngLiteralVal::strValueAs() const
{
    if (m_c2ClassId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_BadLitValAccess
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strName()
        );
    }
    return static_cast<const TMEngStringVal*>(m_pmecvLiteral)->strValue();
}


