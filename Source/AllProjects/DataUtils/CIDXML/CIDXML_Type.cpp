//
// FILE NAME: CIDXML_XML.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This file provides formatting for the facility specific enums from
//  CIDXML_Type.hpp.
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
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDXML::EAttrTypes eType)
{
    if (eType == tCIDXML::EAttrTypes::CData)
        strmToWriteTo << L"CDATA";
    else if (eType == tCIDXML::EAttrTypes::Enumeration)
        strmToWriteTo << L"ENUMERATION";
    else if (eType == tCIDXML::EAttrTypes::Entity)
        strmToWriteTo << L"ENTITY";
    else if (eType == tCIDXML::EAttrTypes::Entities)
        strmToWriteTo << L"ENTITIES";
    else if (eType == tCIDXML::EAttrTypes::ID)
        strmToWriteTo << L"ID";
    else if (eType == tCIDXML::EAttrTypes::IDRef)
        strmToWriteTo << L"IDREF";
    else if (eType == tCIDXML::EAttrTypes::IDRefs)
        strmToWriteTo << L"IDREFS";
    else if (eType == tCIDXML::EAttrTypes::NmToken)
        strmToWriteTo << L"NMTOKEN";
    else if (eType == tCIDXML::EAttrTypes::NmTokens)
        strmToWriteTo << L"NMTOKENS";
    else if (eType == tCIDXML::EAttrTypes::Notation)
        strmToWriteTo << L"NOTATION";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDXML::EAttrTypes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDXML::EDefAttrTypes eType)
{
    if (eType == tCIDXML::EDefAttrTypes::Default)
        strmToWriteTo << L"#DEFAULT";
    else if (eType == tCIDXML::EDefAttrTypes::Required)
        strmToWriteTo << L"#REQUIRED";
    else if (eType == tCIDXML::EDefAttrTypes::Implied)
        strmToWriteTo << L"#IMPLIED";
    else if (eType == tCIDXML::EDefAttrTypes::Fixed)
        strmToWriteTo << L"#FIXED";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDXML::EDefAttrTypes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDXML::ENodeTypes eType)
{
    if (eType == tCIDXML::ENodeTypes::Text)
        strmToWriteTo << L"Text";
    else if (eType == tCIDXML::ENodeTypes::Element)
        strmToWriteTo << L"Element";
    else if (eType == tCIDXML::ENodeTypes::PI)
        strmToWriteTo << L"PI";
    else if (eType == tCIDXML::ENodeTypes::Comment)
        strmToWriteTo << L"Comment";
    else if (eType == tCIDXML::ENodeTypes::DTD)
        strmToWriteTo << L"DTD";
    else if (eType == tCIDXML::ENodeTypes::Decl)
        strmToWriteTo << L"Decl";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDXML::ENodeTypes")
        );
    }
    return strmToWriteTo;
}


