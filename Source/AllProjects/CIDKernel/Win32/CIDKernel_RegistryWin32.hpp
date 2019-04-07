//
// FILE NAME: CIDKernel_RegistryWin32.hpp
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
//  This file provides the platform dependent TKrnlWin32Registry namespace,
//  which provides some support code for managing items in the Win32
//  registry.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


struct HKEY__;


// ---------------------------------------------------------------------------
//  Some additions to the types namespace for our needs
// ---------------------------------------------------------------------------
namespace tCIDKernel
{
    // -----------------------------------------------------------------------
    //  A registry key handle
    // -----------------------------------------------------------------------
    using TWRegHandle = HKEY__*;


    // -----------------------------------------------------------------------
    //  Magic keys
    //
    //  KEEP THIS in sync with the array that is used in the cpp file to
    //  translate to the actual key.
    // -----------------------------------------------------------------------
    enum class ERootKeys
    {
        Classes
        , CurrentUser
        , LocalMachine
        , Users

        , Count
    };


    // -----------------------------------------------------------------------
    //  Access flags
    // -----------------------------------------------------------------------
    enum class ERegAccFlags : tCIDLib::TCard2
    {
        CreateSubKey    = 0x0001
        , EnumSubKey    = 0x0002
        , QueryValue    = 0x0004
        , SetValue      = 0x0008

        , MagicSub      = 0x0007
        , StdOwned      = 0x000F
        , ReadOnly      = 0x0006
    };


    // -----------------------------------------------------------------------
    //  Data types of registry data. We arrange them to match their actual
    //  Windows values.
    // -----------------------------------------------------------------------
    enum class ERegDTypes
    {
        None            = 0
        , ZStr          = 1
        , ExpandZStr    = 2
        , Binary        = 3
        , Card4         = 4
        , Card4BE       = 5
        , Link          = 6
        , MultiZStr     = 7
        , ResList       = 8
        , ResDescr      = 9
        , ResReqList    = 10
        , Card8         = 11

        , Count
    };
}



// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlWin32Registry
// ---------------------------------------------------------------------------
namespace TKrnlWin32Registry
{
    KRNLEXPORT tCIDLib::TBoolean bCloseKey
    (
                tCIDKernel::TWRegHandle     hkeyToClose
    );

    KRNLEXPORT tCIDLib::TBoolean bDeleteKey
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
    );

    KRNLEXPORT tCIDLib::TBoolean bDeleteValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryBinValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        , const tCIDLib::TCard4             c4MaxBytes
        ,       tCIDLib::TCard1* const      pc1ToFill
        ,       tCIDLib::TCard4&            c4Bytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryCardValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        ,       tCIDLib::TCard4&            c4ToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryKeyInfo
    (
        const   tCIDKernel::TWRegHandle     hkeyToQuery
        ,       tCIDLib::TCard4&            c4SubKeys
        ,       tCIDLib::TCard4&            c4Values
        ,       tCIDLib::TCard4&            c4MaxSubKeyLen
        ,       tCIDLib::TCard4&            c4MaxValueLen
        ,       tCIDLib::TCard4&            c4MaxDataLen
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryStrValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        ,       tCIDLib::TCh* const         pszToFill
        , const tCIDLib::TCard4             c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        ,       tCIDKernel::ERegDTypes&     eType
        , const tCIDLib::TCard4             c4MaxBytes
        ,       tCIDLib::TCard1* const      pc1ToFill
        ,       tCIDLib::TCard4&            c4Bytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryValueAt
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCard4             c4At
        ,       tCIDLib::TCh* const         pszName
        , const tCIDLib::TCard4             c4MaxChars
        ,       tCIDKernel::ERegDTypes&     eType
        , const tCIDLib::TCard4             c4MaxBytes
        ,       tCIDLib::TCard1* const      pc1ToFill
        ,       tCIDLib::TCard4&            c4Bytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryValueInfo
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        ,       tCIDKernel::ERegDTypes&     eType
        ,       tCIDLib::TCard4&            c4Bytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQuerySubKeyAt
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCard4             c4At
        ,       tCIDLib::TCh* const         pszNameToFill
        , const tCIDLib::TCard4             c4MaxNameChars
    );

    KRNLEXPORT tCIDLib::TBoolean bSetCardValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        , const tCIDLib::TCard4             c4Data
    );

    KRNLEXPORT tCIDLib::TBoolean bSetStrValue
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszName
        , const tCIDLib::TCh* const         pszData
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyCreate
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszKeyName
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyCreateOrOpen
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszKeyName
        ,       tCIDLib::TBoolean&          bCreated
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyCreateOrOpenSubKey
    (
        const   tCIDKernel::ERootKeys       eKey
        , const tCIDLib::TCh* const         pszSubKey
        , const tCIDLib::TCh* const         pszSubSubKey
        ,       tCIDLib::TBoolean&          bCreated
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyGetRootSubKey
    (
        const   tCIDKernel::ERootKeys       eKey
        , const tCIDLib::TCh* const         pszSubKey
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyOpen
    (
        const   tCIDKernel::TWRegHandle     hkeyParent
        , const tCIDLib::TCh* const         pszKeyName
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyOpenPath
    (
        const   tCIDKernel::ERootKeys       eKey
        , const tCIDLib::TCh* const         pszSubKey
        ,       TKrnlLList<TKrnlString>&    kllstPath
        , const tCIDKernel::ERegAccFlags    eFlags
    );

    KRNLEXPORT tCIDKernel::TWRegHandle hkeyOpenSubKey
    (
        const   tCIDKernel::ERootKeys       eKey
        , const tCIDLib::TCh* const         pszSubKey
        , const tCIDLib::TCh* const         pszSubSubKey
        , const tCIDKernel::ERegAccFlags    eFlags
    );

}


