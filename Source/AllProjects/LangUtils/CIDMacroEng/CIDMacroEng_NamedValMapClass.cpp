//
// FILE NAME: CIDMacroEng_NamedValMapClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/31/2004
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the MEng.System.Runtime.NamedValMap class.
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
RTTIDecls(TMEngNamedValMapVal,TMEngClassVal)
RTTIDecls(TMEngNamedValMapInfo,TMEngClassInfo)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_NamedValMapClass
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    static const TString   strNamedValMap(L"NamedValMap");
    static const TString   strNamedValMapClassPath(L"MEng.System.Runtime.NamedValMap");
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngNamedValMapVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngNamedValMapVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngNamedValMapVal::TMEngNamedValMapVal(const  TString&                strName
                                        , const tCIDLib::TCard2         c2Id
                                        , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngNamedValMapVal::
TMEngNamedValMapVal(const   TString&                strName
                    , const tCIDLib::TCard2         c2Id
                    , const tCIDMacroEng::EConstTypes  eConst
                    , const TNamedValMap<TString>&  nvmInit) :
    TMEngClassVal(strName, c2Id, eConst)
    , m_nvmItems(nvmInit)
{
}

TMEngNamedValMapVal::~TMEngNamedValMapVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngNamedValMapVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngNamedValMapVal::bDbgFormat(        TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        strmTarget << L"Item Count = " << m_nvmItems.c4ElemCount();
        return kCIDLib::True;
    }
     else if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        // <TBD> Display the top level keys?
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngNamedValMapVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                                ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    const TMEngNamedValMapVal& mecvSrc(static_cast<const TMEngNamedValMapVal&>(mecvToCopy));

    //
    //  In our case, we can just assign the maps directly since they only
    //  ever contain strings.
    //
    m_nvmItems = mecvSrc.m_nvmItems;
}



// ---------------------------------------------------------------------------
//  TMEngNamedValMapVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngNamedValMapVal::AppendFrom(const TNamedValMap<TString>& nvmSrc)
{
    // Just delegate to the value name
    m_nvmItems.AppendFrom(nvmSrc);
}

TNamedValMap<TString>& TMEngNamedValMapVal::nvmItems()
{
    return m_nvmItems;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngNamedValMapInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngNamedValMapInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngNamedValMapInfo::strPath()
{
    return CIDMacroEng_NamedValMapClass::strNamedValMapClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngNamedValMapInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngNamedValMapInfo::TMEngNamedValMapInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NamedValMapClass::strNamedValMap
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_AddItem(kMacroEng::c2BadId)
    , m_c2MethId_AddValue(kMacroEng::c2BadId)
    , m_c2MethId_ClearMap(kMacroEng::c2BadId)
    , m_c2MethId_ClearItem(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetValue(kMacroEng::c2BadId)
    , m_c2MethId_IsEmpty(kMacroEng::c2BadId)
    , m_c2MethId_ItemExists(kMacroEng::c2BadId)
    , m_c2MethId_ItemIsEmpty(kMacroEng::c2BadId)
    , m_c2MethId_RemoveItem(kMacroEng::c2BadId)
    , m_c2MethId_RemoveSubKey(kMacroEng::c2BadId)
    , m_c2MethId_SetValue(kMacroEng::c2BadId)
    , m_c2MethId_SubKeyExists(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c4ErrId_AddItem(kCIDLib::c4MaxCard)
    , m_c4ErrId_AddValue(kCIDLib::c4MaxCard)
    , m_c4ErrId_ClearItem(kCIDLib::c4MaxCard)
    , m_c4ErrId_GetValue(kCIDLib::c4MaxCard)
    , m_c4ErrId_KeyAlreadyExists(kCIDLib::c4MaxCard)
    , m_c4ErrId_KeyNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrId_RemoveItem(kCIDLib::c4MaxCard)
    , m_c4ErrId_RemoveValue(kCIDLib::c4MaxCard)
    , m_c4ErrId_SetValue(kCIDLib::c4MaxCard)
    , m_c4ErrId_SubKeyAlreadyExists(kCIDLib::c4MaxCard)
    , m_c4ErrId_SubKeyNotFound(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngNamedValMapInfo::~TMEngNamedValMapInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngNamedValMapInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngNamedValMapInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"NamedValMapErrs"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        m_c4ErrId_AddItem = m_pmeciErrors->c4AddEnumItem(L"AddItem", L"An error occured while adding a map item %(1). Error=%(2)");
        m_c4ErrId_AddValue = m_pmeciErrors->c4AddEnumItem(L"AddValue", L"An error occured while adding a map value %(1).%(2). Error=%(3)");
        m_c4ErrId_ClearItem = m_pmeciErrors->c4AddEnumItem(L"ClearItem", L"An error occured while clearing map item %(1). Error=%(2)");
        m_c4ErrId_GetValue = m_pmeciErrors->c4AddEnumItem(L"GetValue", L"An error occured while getting map value %(1).%(2). Error=%(3)");
        m_c4ErrId_KeyAlreadyExists = m_pmeciErrors->c4AddEnumItem(L"KeyAlreadyExists", L"Key %(1) already exists in the map");
        m_c4ErrId_KeyNotFound = m_pmeciErrors->c4AddEnumItem(L"KeyNotFound", L"Key %(1) was not found in the map");
        m_c4ErrId_RemoveItem = m_pmeciErrors->c4AddEnumItem(L"RemoveItem", L"An error occured while removing a map item %(1). Error=%(2)");
        m_c4ErrId_RemoveValue = m_pmeciErrors->c4AddEnumItem(L"RemoveValue", L"An error occured while removing map value %(1).%(2). Error=%(3)");
        m_c4ErrId_SetValue = m_pmeciErrors->c4AddEnumItem(L"SetValue", L"An error occured while setting a map value %(1).%(2). Error=%(3)");
        m_c4ErrId_SubKeyAlreadyExists = m_pmeciErrors->c4AddEnumItem(L"SubKeyAlreadyExists", L"Sub-key %(1).%(2) already exists in the map");
        m_c4ErrId_SubKeyNotFound = m_pmeciErrors->c4AddEnumItem(L"SubKeyNotFound", L"Sub-key %(1).%(2) was not found");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add a new named item
    {
        TMEngMethodInfo methiNew
        (
            L"AddItem"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"NewKey", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_AddItem = c2AddMethodInfo(methiNew);
    }

    // Add a value to an existing item
    {
        TMEngMethodInfo methiNew
        (
            L"AddValue"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKey", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_AddValue = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.NamedValMap"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Clear the whole map
    {
        TMEngMethodInfo methiNew
        (
            L"ClearMap"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ClearMap = c2AddMethodInfo(methiNew);
    }

    // Clear all subkeys from an item
    {
        TMEngMethodInfo methiNew
        (
            L"ClearItem"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ClearItem = c2AddMethodInfo(methiNew);
    }

    // Get a value
    {
        TMEngMethodInfo methiNew
        (
            L"GetValue"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKey", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetValue = c2AddMethodInfo(methiNew);
    }

    // Get a value if it exists
    {
        TMEngMethodInfo methiNew
        (
            L"GetValueIfExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKey", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetValueIfExists = c2AddMethodInfo(methiNew);
    }

    // See if the map is empty
    {
        TMEngMethodInfo methiNew
        (
            L"IsEmpty"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_IsEmpty = c2AddMethodInfo(methiNew);
    }

    // See if an item exists in the map
    {
        TMEngMethodInfo methiNew
        (
            L"ItemExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ItemExists = c2AddMethodInfo(methiNew);
    }

    // See if the map is empty
    {
        TMEngMethodInfo methiNew
        (
            L"ItemIsEmpty"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ItemIsEmpty = c2AddMethodInfo(methiNew);
    }

    // Remove an item from the map
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveItem"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToRemove", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_RemoveItem = c2AddMethodInfo(methiNew);
    }

    // Remove a subkey from an item
    {
        TMEngMethodInfo methiNew
        (
            L"RemoveSubKey"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKeyToRemove", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_RemoveSubKey = c2AddMethodInfo(methiNew);
    }

    // Set the value field
    {
        TMEngMethodInfo methiNew
        (
            L"SetValue"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKey", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValToSet", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetValue = c2AddMethodInfo(methiNew);
    }

    // See if a subkey exists in the map
    {
        TMEngMethodInfo methiNew
        (
            L"SubKeyExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Key", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"SubKey", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SubKeyExists = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngNamedValMapInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngNamedValMapVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngNamedValMapInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngNamedValMapInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngNamedValMapVal& mecvActual = static_cast<TMEngNamedValMapVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_AddItem)
    {
        const TString& strKey = meOwner.strStackValAt(c4FirstInd);
        try
        {
            mecvActual.nvmItems().AddItem(strKey);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_AddItem, strKey);
        }
    }
     else if (c2MethId == m_c2MethId_AddValue)
    {
        const TString& strKey = meOwner.strStackValAt(c4FirstInd);
        const TString& strSubKey = meOwner.strStackValAt(c4FirstInd + 1);
        const TString& strValue = meOwner.strStackValAt(c4FirstInd + 2);
        try
        {
            mecvActual.nvmItems().AddValue(strKey, strSubKey, strValue);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_AddValue, strKey, strSubKey);
        }
    }
     else if (c2MethId == m_c2MethId_ClearMap)
    {
        mecvActual.nvmItems().Reset();
    }
     else if (c2MethId == m_c2MethId_ClearItem)
    {
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        try
        {
            mecvActual.nvmItems().ClearItem(strKey);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_ClearItem, strKey);
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        mecvActual.nvmItems().Reset();
    }
     else if (c2MethId == m_c2MethId_GetValue)
    {
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        const TString& strSubKey(meOwner.strStackValAt(c4FirstInd + 1));

        //
        //  Get the value into the return string. Call the value getter
        //  method that gives us the value directly, but that means that it
        //  will throw if the keys aren't found.
        //
        try
        {
            TMEngStringVal& mecvRet
            (
                meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1)
            );
            mecvRet.strValue(mecvActual.nvmItems().objValue(strKey, strSubKey));
        }

        catch(const TError& errToCatch)
        {
            // Throw a not found error
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_GetValue, strKey, strSubKey);
        }
    }
     else if (c2MethId == m_c2MethId_GetValueIfExists)
    {
        // Get the keys and output string and return
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        const TString& strSubKey(meOwner.strStackValAt(c4FirstInd + 1));
        TMEngStringVal& mecvToFill
        (
            meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2)
        );
        TMEngBooleanVal& mecvRet
        (
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
        );

        // Now try to query into the output string and store the result
        mecvRet.bValue
        (
            mecvActual.nvmItems().bQueryValue(strKey, strSubKey, mecvToFill.strValue())
        );
    }
     else if (c2MethId == m_c2MethId_IsEmpty)
    {
        TMEngBooleanVal& mecvRet
        (
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
        );
        mecvRet.bValue(mecvActual.nvmItems().bIsEmpty());
    }
     else if (c2MethId == m_c2MethId_ItemExists)
    {
        // Get the key, sub-key, and return value
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        TMEngBooleanVal& mecvRet
        (
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
        );
        mecvRet.bValue(mecvActual.nvmItems().bKeyExists(strKey));
    }
     else if (c2MethId == m_c2MethId_ItemIsEmpty)
    {
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        try
        {
            TMEngBooleanVal& mecvRet
            (
                meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
            );
            mecvRet.bValue(mecvActual.nvmItems().bItemIsEmpty(strKey));
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_KeyNotFound, strKey);
        }
    }
     else if (c2MethId == m_c2MethId_RemoveItem)
    {
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        try
        {
            mecvActual.nvmItems().RemoveKey(strKey);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_RemoveValue, strKey);
        }
    }
     else if (c2MethId == m_c2MethId_RemoveSubKey)
    {
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        const TString& strSubKey(meOwner.strStackValAt(c4FirstInd + 1));

        try
        {
            mecvActual.nvmItems().RemoveSubKey(strKey, strSubKey);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_RemoveValue, strKey, strSubKey);
        }
    }
     else if (c2MethId == m_c2MethId_SetValue)
    {
        // Get the key, sub-key, value, and return value
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        const TString& strSubKey(meOwner.strStackValAt(c4FirstInd + 1));
        const TString& strValue(meOwner.strStackValAt(c4FirstInd + 2));

        try
        {
            mecvActual.nvmItems().SetValue(strKey, strSubKey, strValue);
        }

        catch(const TError& errToCatch)
        {
            ThrowCIDErr(meOwner, errToCatch, m_c4ErrId_SetValue, strKey, strSubKey);
        }
    }
     else if (c2MethId == m_c2MethId_SubKeyExists)
    {
        // Get the key, sub-key, and return value
        const TString& strKey(meOwner.strStackValAt(c4FirstInd));
        const TString& strSubKey(meOwner.strStackValAt(c4FirstInd + 1));
        TMEngBooleanVal& mecvRet
        (
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1)
        );
        mecvRet.bValue(mecvActual.nvmItems().bSubKeyExists(strKey, strSubKey));
    }
     else
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngNamedValMapInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngNamedValMapInfo::ThrowCIDErr(          TCIDMacroEngine&    meOwner
                                    , const TError&             errCaught
                                    , const tCIDLib::TCard4     c4Default
                                    , const TString&            strKey)
{
    if (errCaught.strFacName() == facCIDLib().strName())
    {
        if (errCaught.errcId() == kCIDErrs::errcNVM_ItemExists)
            ThrowAnErr(meOwner, m_c4ErrId_KeyAlreadyExists, strKey);
        else if (errCaught.errcId() == kCIDErrs::errcNVM_ItemNotFound)
            ThrowAnErr(meOwner, m_c4ErrId_KeyNotFound, strKey);
        else
            ThrowAnErr(meOwner, c4Default, strKey, errCaught.strErrText());
    }
     else
    {
        // Throw the default error
        ThrowAnErr(meOwner, c4Default, strKey, errCaught.strErrText());
    }
}


tCIDLib::TVoid
TMEngNamedValMapInfo::ThrowCIDErr(          TCIDMacroEngine&    meOwner
                                    , const TError&             errCaught
                                    , const tCIDLib::TCard4     c4Default
                                    , const TString&            strKey
                                    , const TString&            strSubKey)
{
    if (errCaught.strFacName() == facCIDLib().strName())
    {
        if (errCaught.errcId() == kCIDErrs::errcNVM_SubKeyExists)
            ThrowAnErr(meOwner, m_c4ErrId_SubKeyAlreadyExists, strKey, strSubKey);
        else if (errCaught.errcId() == kCIDErrs::errcNVM_SubKeyNotFound)
            ThrowAnErr(meOwner, m_c4ErrId_SubKeyNotFound, strKey, strSubKey);
        else if (errCaught.errcId() == kCIDErrs::errcNVM_ItemNotFound)
            ThrowAnErr(meOwner, m_c4ErrId_KeyNotFound, strKey);
        else if (errCaught.errcId() == kCIDErrs::errcNVM_ItemExists)
            ThrowAnErr(meOwner, m_c4ErrId_KeyAlreadyExists, strKey);
        else
            ThrowAnErr(meOwner, c4Default, strKey, errCaught.strErrText());
    }
     else
    {
        // Throw the default error
        ThrowAnErr(meOwner, c4Default, strKey, strSubKey, errCaught.strErrText());
    }
}


tCIDLib::TVoid
TMEngNamedValMapInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngNamedValMapInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken1
                                , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngNamedValMapInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken1
                                , const MFormattable&       fmtblToken2
                                , const MFormattable&       fmtblToken3)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);
    strText.eReplaceToken(fmtblToken3, kCIDLib::chDigit3);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


