//
// FILE NAME: CIDKernel_RegistryWin32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/30/2002
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
//  This file provides the Windows specific namespace that supports Windows
//  registry functionality.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_RegistryWin32.hpp"


// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4XlatFlags(const tCIDKernel::ERegAccFlags eFlags)
{
    tCIDLib::TCard4 c4Flags = STANDARD_RIGHTS_REQUIRED;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDKernel::ERegAccFlags::CreateSubKey))
        c4Flags |= (KEY_CREATE_SUB_KEY | STANDARD_RIGHTS_WRITE);

    if (tCIDLib::bAllBitsOn(eFlags, tCIDKernel::ERegAccFlags::EnumSubKey))
        c4Flags |= (KEY_ENUMERATE_SUB_KEYS | STANDARD_RIGHTS_READ);

    if (tCIDLib::bAllBitsOn(eFlags, tCIDKernel::ERegAccFlags::QueryValue))
        c4Flags |= (KEY_QUERY_VALUE | STANDARD_RIGHTS_READ);

    if (tCIDLib::bAllBitsOn(eFlags, tCIDKernel::ERegAccFlags::SetValue))
        c4Flags |= (KEY_SET_VALUE | STANDARD_RIGHTS_WRITE);

    return c4Flags;
}



// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlWin32Registry
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlWin32Registry::bCloseKey(tCIDKernel::TWRegHandle hkeyToClose)
{
    const tCIDLib::TCard4 c4Err = ::RegCloseKey(hkeyToClose);
    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlWin32Registry::bDeleteKey( const   tCIDKernel::TWRegHandle hkeyParent
                                , const tCIDLib::TCh* const     pszName)
{
    const tCIDLib::TCard4 c4Err = ::RegDeleteKey(hkeyParent, pszName);
    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bDeleteValue(const  tCIDKernel::TWRegHandle hkeyParent
                                , const tCIDLib::TCh* const     pszName)
{
    const tCIDLib::TCard4 c4Err = ::RegDeleteValue(hkeyParent, pszName);
    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bQueryBinValue( const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCh* const     pszName
                                    , const tCIDLib::TCard4         c4MaxBytes
                                    ,       tCIDLib::TCard1* const  pc1ToFill
                                    ,       tCIDLib::TCard4&        c4Bytes)
{
    c4Bytes = c4MaxBytes;
    tCIDLib::TCard4 c4Type;
    const tCIDLib::TCard4 c4Err = ::RegQueryValueEx
    (
        hkeyParent
        , pszName
        , 0
        , &c4Type
        , pc1ToFill
        , &c4Bytes
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // Validate that its the right type
    if (c4Type != REG_BINARY)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcData_InvalidFormat);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bQueryCardValue(const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCh* const     pszName
                                    ,       tCIDLib::TCard4&        c4ToFill)
{
    tCIDLib::TCard4 c4Size = sizeof(c4ToFill);
    tCIDLib::TCard4 c4Type;
    const tCIDLib::TCard4 c4Err = ::RegQueryValueEx
    (
        hkeyParent
        , pszName
        , 0
        , &c4Type
        , reinterpret_cast<unsigned char*>(&c4ToFill)
        , &c4Size
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // Validate that its the right type
    if (c4Type != REG_DWORD)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcData_InvalidFormat);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bQueryKeyInfo(  const   tCIDKernel::TWRegHandle hkeyToQuery
                                    ,       tCIDLib::TCard4&        c4SubKeys
                                    ,       tCIDLib::TCard4&        c4Values
                                    ,       tCIDLib::TCard4&        c4MaxSubKeyLen
                                    ,       tCIDLib::TCard4&        c4MaxValueLen
                                    ,       tCIDLib::TCard4&        c4MaxDataLen)
{
    const tCIDLib::TCard4 c4Err = ::RegQueryInfoKey
    (
        hkeyToQuery
        , 0
        , 0
        , 0
        , &c4SubKeys
        , &c4MaxSubKeyLen
        , 0
        , &c4Values
        , &c4MaxValueLen
        , &c4MaxDataLen
        , 0
        , 0
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bQueryStrValue( const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCh* const     pszName
                                    ,       tCIDLib::TCh* const     pszToFill
                                    , const tCIDLib::TCard4         c4MaxChars)
{
    tCIDLib::TCard4 c4Size = (c4MaxChars * kCIDLib::c4CharBytes);
    tCIDLib::TCard4 c4Type;
    const tCIDLib::TCard4 c4Err = ::RegQueryValueEx
    (
        hkeyParent
        , pszName
        , 0
        , &c4Type
        , reinterpret_cast<unsigned char*>(pszToFill)
        , &c4Size
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // Validate that its the right type
    if (c4Type != REG_SZ)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcData_InvalidFormat);
        return kCIDLib::False;
    }

    pszToFill[c4Size] = kCIDLib::chNull;
    return kCIDLib::True;
}


// Generically query the type and returns the value
tCIDLib::TBoolean
TKrnlWin32Registry::bQueryValue(const   tCIDKernel::TWRegHandle hkeyParent
                                , const tCIDLib::TCh* const     pszName
                                ,       tCIDKernel::ERegDTypes& eType
                                , const tCIDLib::TCard4         c4MaxBytes
                                ,       tCIDLib::TCard1* const  pc1ToFill
                                ,       tCIDLib::TCard4&        c4Bytes)
{
    c4Bytes = c4MaxBytes;
    tCIDLib::TCard4 c4Type;
    const tCIDLib::TCard4 c4Err = ::RegQueryValueEx
    (
        hkeyParent, pszName, 0, &c4Type, pc1ToFill, &c4Bytes
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // If it's some type we know nothing about, set it to none
    if (c4Type >= tCIDLib::c4EnumOrd(tCIDKernel::ERegDTypes::Count))
        eType = tCIDKernel::ERegDTypes::None;
    else
        eType = tCIDKernel::ERegDTypes(c4Type);

    return kCIDLib::True;
}


// For interating values, returns name, type, and data by index
tCIDLib::TBoolean
TKrnlWin32Registry::bQueryValueAt(  const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCard4         c4At
                                    ,       tCIDLib::TCh* const     pszName
                                    , const tCIDLib::TCard4         c4MaxChars
                                    ,       tCIDKernel::ERegDTypes& eType
                                    , const tCIDLib::TCard4         c4MaxBytes
                                    ,       tCIDLib::TCard1* const  pc1Data
                                    ,       tCIDLib::TCard4&        c4Bytes)
{
    c4Bytes = c4MaxBytes;
    tCIDLib::TCard4 c4NameSz = c4MaxChars + 1;
    tCIDLib::TCard4 c4Type;
    c4Bytes = 0;
    const tCIDLib::TCard4 c4Err = ::RegEnumValue
    (
        hkeyParent, c4At, pszName, &c4NameSz, 0, &c4Type, pc1Data, &c4Bytes
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // If it's some type we know nothing about, set it to none
    if (c4Type >= tCIDLib::c4EnumOrd(tCIDKernel::ERegDTypes::Count))
        eType = tCIDKernel::ERegDTypes::None;
    else
        eType = tCIDKernel::ERegDTypes(c4Type);
    return kCIDLib::True;
}



// Just returns the type and byte size of a value
tCIDLib::TBoolean
TKrnlWin32Registry::bQueryValueInfo(const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCh* const     pszName
                                    ,       tCIDKernel::ERegDTypes& eType
                                    ,       tCIDLib::TCard4&        c4Bytes)
{
    tCIDLib::TCard4 c4Type;
    c4Bytes = 0;
    const tCIDLib::TCard4 c4Err = ::RegQueryValueEx
    (
        hkeyParent, pszName, 0, &c4Type, 0, &c4Bytes
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }

    // If it's some type we know nothing about, set it to none
    if (c4Type >= tCIDLib::c4EnumOrd(tCIDKernel::ERegDTypes::Count))
        eType = tCIDKernel::ERegDTypes::None;
    else
        eType = tCIDKernel::ERegDTypes(c4Type);

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlWin32Registry::bQuerySubKeyAt( const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCard4         c4At
                                    ,       tCIDLib::TCh* const     pszNameToFill
                                    , const tCIDLib::TCard4         c4MaxNameChars)
{
    FILETIME LastWrite;
    tCIDLib::TCard4 c4NameSz = c4MaxNameChars + 1;
    const tCIDLib::TCard4 c4Err = ::RegEnumKeyEx
    (
        hkeyParent, c4At, pszNameToFill, &c4NameSz, 0, 0, 0, &LastWrite
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlWin32Registry::bSetCardValue(  const   tCIDKernel::TWRegHandle hkeyParent
                                    , const tCIDLib::TCh* const     pszName
                                    , const tCIDLib::TCard4         c4Data)
{
    const tCIDLib::TCard4 c4Err = ::RegSetValueEx
    (
        hkeyParent
        , pszName
        , 0
        , REG_DWORD
        , reinterpret_cast<const unsigned char*>(&c4Data)
        , sizeof(c4Data)
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Registry::bSetStrValue(const   tCIDKernel::TWRegHandle hkeyParent
                                , const tCIDLib::TCh* const      pszName
                                , const tCIDLib::TCh* const      pszData)
{
    const tCIDLib::TCard4 c4Err = ::RegSetValueEx
    (
        hkeyParent
        , pszName
        , 0
        , REG_SZ
        , reinterpret_cast<const unsigned char*>(pszData)
        , (TRawStr::c4StrLen(pszData) + 1) * kCIDLib::c4CharBytes
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyCreate( const   tCIDKernel::TWRegHandle  hkeyParent
                                , const tCIDLib::TCh* const      pszKeyName
                                , const tCIDKernel::ERegAccFlags eFlags)
{
    tCIDKernel::TWRegHandle hkeyRet;

    const tCIDLib::TCard4 c4Err = ::RegCreateKeyEx
    (
        hkeyParent
        , pszKeyName
        , 0
        , 0
        , REG_OPTION_NON_VOLATILE
        , c4XlatFlags(eFlags)
        , 0
        , &hkeyRet
        , 0
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return 0;
    }
    return hkeyRet;
}


tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyCreateOrOpen(const  tCIDKernel::TWRegHandle  hkeyParent
                                    , const tCIDLib::TCh* const      pszKeyName
                                    ,       tCIDLib::TBoolean&       bCreated
                                    , const tCIDKernel::ERegAccFlags eFlags)
{
    tCIDLib::TCard4         c4Disp;
    tCIDKernel::TWRegHandle hkeyRet;

    const tCIDLib::TCard4 c4Err = ::RegCreateKeyEx
    (
        hkeyParent
        , pszKeyName
        , 0
        , 0
        , REG_OPTION_NON_VOLATILE
        , c4XlatFlags(eFlags)
        , 0
        , &hkeyRet
        , &c4Disp
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return 0;
    }

    bCreated = (c4Disp == REG_CREATED_NEW_KEY);
    return hkeyRet;
}


//
//  This one does the common operation of opening a subkey of one of the magic
//  hives, then opening or creating a child key of that.
//
tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyCreateOrOpenSubKey(const  tCIDKernel::ERootKeys    eKey
                                          , const tCIDLib::TCh* const      pszSubKey
                                          , const tCIDLib::TCh* const      pszSubSubKey
                                          ,       tCIDLib::TBoolean&       bCreated
                                          , const tCIDKernel::ERegAccFlags eFlags)
{
    // Open the magic subkey for internal use
    tCIDKernel::TWRegHandle hkeySub = hkeyGetRootSubKey
    (
        eKey, pszSubKey, tCIDKernel::ERegAccFlags::MagicSub
    );

    // If we can't open that, then we really have problems
    if (!hkeySub)
        return 0;

    // Now open or create the child of that one
    tCIDKernel::TWRegHandle hkeyRet = hkeyCreateOrOpen
    (
        hkeySub, pszSubSubKey, bCreated, eFlags
    );

    // Now close the original key
    bCloseKey(hkeySub);

    // And return the new key, if any
    return hkeyRet;
}


tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyGetRootSubKey(  const   tCIDKernel::ERootKeys    eKey
                                        , const tCIDLib::TCh* const      pszSubKey
                                        , const tCIDKernel::ERegAccFlags eFlags)
{
    static const tCIDKernel::TWRegHandle ahkeyMagicKeys[tCIDKernel::ERootKeys::Count] =
    {
        HKEY_CLASSES_ROOT
        , HKEY_CURRENT_USER
        , HKEY_LOCAL_MACHINE
        , HKEY_USERS
    };
    return hkeyOpen(ahkeyMagicKeys[tCIDLib::c4EnumOrd(eKey)], pszSubKey, eFlags);
}


tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyOpen(const  tCIDKernel::TWRegHandle     hkeyParent
                            , const tCIDLib::TCh* const         pszKeyName
                            , const tCIDKernel::ERegAccFlags    eFlags)
{
    tCIDKernel::TWRegHandle hkeyRet;

    // See if we can open it
    const tCIDLib::TCard4 c4Err = ::RegOpenKeyEx
    (
        hkeyParent, pszKeyName, 0, c4XlatFlags(eFlags), &hkeyRet
    );

    if (c4Err)
    {
        TKrnlError::SetLastHostError(c4Err);
        return 0;
    }
    return hkeyRet;
}


//
//  Open a path an arbitrary number of scopes down into a path.
//
tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyOpenPath(const  tCIDKernel::ERootKeys       eKey
                                , const tCIDLib::TCh* const         pszSubKey
                                ,       TKrnlLList<TKrnlString>&    kllstPath
                                , const tCIDKernel::ERegAccFlags    eFlags)
{
    tCIDKernel::TWRegHandle hkeyRet = hkeyGetRootSubKey
    (
        eKey, pszSubKey, tCIDKernel::ERegAccFlags::MagicSub
    );

    // If we can't open that, then we really have problems
    if (!hkeyRet)
        return 0;

    // Now loop through all the entries in the path list
    if (!kllstPath.bResetCursor())
        return hkeyRet;

    TKrnlString* pkstrScope;
    while (kllstPath.bNext(pkstrScope))
    {
        //
        //  Only use the caller's flags on the last one. Until then all
        //  we care about is the ability to open child keys.
        //
        tCIDKernel::ERegAccFlags eCurFlags;
        if (kllstPath.bAtEnd())
            eCurFlags = eFlags;
        else
            eCurFlags = tCIDKernel::ERegAccFlags::ReadOnly;

        tCIDKernel::TWRegHandle hkeyCur = hkeyOpen
        (
            hkeyRet, pkstrScope->pszValue(), eCurFlags
        );

        if (!hkeyCur)
        {
            // Couldn't get the current one, so close what we have and fail
            bCloseKey(hkeyRet);
            return 0;
        }

        // We got another one, so close the previous one and make this the new one
        bCloseKey(hkeyRet);
        hkeyRet = hkeyCur;
    }

    // Return the last key we opened
    return hkeyRet;
}


//
//  This one does the common operation of opening a subkey of one of the magic
//  hives, then opening a child key of that.
//
tCIDKernel::TWRegHandle
TKrnlWin32Registry::hkeyOpenSubKey( const   tCIDKernel::ERootKeys    eKey
                                    , const tCIDLib::TCh* const      pszSubKey
                                    , const tCIDLib::TCh* const      pszSubSubKey
                                    , const tCIDKernel::ERegAccFlags eFlags)
{
    // Open the magic subkey for internal use
    tCIDKernel::TWRegHandle hkeySub = hkeyGetRootSubKey
    (
        eKey, pszSubKey, tCIDKernel::ERegAccFlags::MagicSub
    );

    // If we can't open that, then we really have problems
    if (!hkeySub)
        return 0;

    // Now open or create the child of that one, and close the original
    tCIDKernel::TWRegHandle hkeyRet = hkeyOpen(hkeySub, pszSubSubKey, eFlags);
    bCloseKey(hkeySub);

    // And return the new key, if any
    return hkeyRet;
}


