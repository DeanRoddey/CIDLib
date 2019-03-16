//
// FILE NAME: CIDLib_Facility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1996
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
//  TFacility is the basic facility class. All other Facility Classes are derived
//  from this guy. It makes sure that all Facilities provide a basic public API,
//  and it does a lot of the grunt work.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacility,TObject)


// ---------------------------------------------------------------------------
//  Local types
//
//  We have to maintain a local list of all constructed facilities. This
//  allows lookup of facility objects by name. Facility objects are all
//  singletons, so this is a reasonably safe thing to do. Because of the very
//  fundamental nature of facilities, we do a custom data structure. Since
//  facilities never die, we really only have to add. But, we do implement
//  the code to remove upon destruction just in case.
// ---------------------------------------------------------------------------
struct TFacListItem
{
    tCIDLib::THashVal   hshName;
    TFacility*          pfacThis;
    TFacListItem*       pfliNext;
};


namespace CIDLib_Facility
{
    // -----------------------------------------------------------------------
    //  Local const data
    //
    //  hshModulus
    //      THe hash modulus used for hashing facility names.
    //
    //  pszVersionFormat
    //      This is the format string for the version member. The provided maj,
    //      min, and revision values are formatted into it during construction.
    // -----------------------------------------------------------------------
    const tCIDLib::THashVal     hshModulus = 29;
    const tCIDLib::TCh* const   pszVersionFmt = L"%(1).%(2).%(3)";


    // -----------------------------------------------------------------------
    //  Local data
    //
    //  pfliHead
    //      The head of the facility list.
    // -----------------------------------------------------------------------
    TFacListItem*               pfliHead = 0;
}


// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------
static TCriticalSection* pcrsList()
{
    static TCriticalSection*   pcrsList = 0;

    if (!pcrsList)
    {
        TBaseLock lockSync;
        if (!pcrsList)
            TRawMem::pExchangePtr(&pcrsList, new TCriticalSection);
    }
    return pcrsList;
}

static tCIDLib::TVoid AddToList(TFacility* const pfacNew)
{
    TFacListItem* pfliNew = new TFacListItem;
    pfliNew->hshName = pfacNew->strName().hshCalcHash(CIDLib_Facility::hshModulus);
    pfliNew->pfacThis = pfacNew;

    // Lock the list and add the new facility
    TCritSecLocker lockList(pcrsList());

    pfliNew->pfliNext = CIDLib_Facility::pfliHead;
    CIDLib_Facility::pfliHead = pfliNew;
}


static tCIDLib::TVoid RemoveFromList(TFacility* const pfacToRemove)
{
    // Lock the list while we search
    TCritSecLocker lockList(pcrsList());

    TFacListItem* pfliCur = CIDLib_Facility::pfliHead;
    TFacListItem* pfliPrev = 0;
    while (pfliCur)
    {
        if (pfliCur->pfacThis == pfacToRemove)
        {
            //
            //  If the previous is zero, then this is the head node, so we
            //  do the simpler scenario. Else, we patch around.
            //
            if (pfliPrev)
                pfliPrev->pfliNext = pfliCur->pfliNext;
            else
                CIDLib_Facility::pfliHead = pfliCur->pfliNext;

            // And free this node and break out
            delete pfliCur;
            break;
        }

        pfliPrev = pfliCur;
        pfliCur = pfliPrev->pfliNext;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TFacility
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacility: Public, static methods
// ---------------------------------------------------------------------------

// Get/set the global log mode
tCIDLib::ESeverities
TFacility::eGlobalLogMode(const tCIDLib::ESeverities eSevToSet)
{
    s_eGlobalLogMode = eSevToSet;
    return s_eGlobalLogMode;
}

tCIDLib::ESeverities TFacility::eGlobalLogMode()
{
    return s_eGlobalLogMode;
}


// Look up a facility by name
TFacility* TFacility::pfacFromName(const TString& strFacName)
{
    // Get the hash of the passed name for use in the search
    const tCIDLib::THashVal hshName = strFacName.hshCalcHash(CIDLib_Facility::hshModulus);

    // Lock the list while we search
    TCritSecLocker lockList(pcrsList());

    // Try to find the passed name in the local hash table
    TFacListItem* pfliCur = CIDLib_Facility::pfliHead;
    while (pfliCur)
    {
        if (pfliCur->hshName == hshName)
        {
            if (strFacName == pfliCur->pfacThis->strName())
                return pfliCur->pfacThis;
        }
        pfliCur = pfliCur->pfliNext;
    }
    return nullptr;
}


TFacility& TFacility::facFromName(const TString& strFacName)
{
    TFacility* pfacRet = pfacFromName(strFacName);
    if (!pfacRet)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantFindFac
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strFacName
        );
    }
    return *pfacRet;
}



// ---------------------------------------------------------------------------
//  TFacility: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacility::TFacility(   const   TString&            strFacName
                        , const tCIDLib::EModTypes  eModType
                        , const tCIDLib::TCard4     c4MajVer
                        , const tCIDLib::TCard4     c4MinVer
                        , const tCIDLib::TCard4     c4Revision
                        , const tCIDLib::EModFlags  eFlags
                        , const tCIDLib::TBoolean   bLoad) :

    TModule(strFacName, eModType, c4MajVer, c4MinVer, eFlags, bLoad)
    , m_c4MajVer(c4MajVer)
    , m_c4MinVer(c4MinVer)
    , m_c4Revision(c4Revision)
{
    // Make sure it's just a facility name, not a path
    if (strFacName.bContainsChar(kCIDLib::chPathSep))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_OnlyBaseName
            , strFacName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Call a common init method
    CommonInit(strFacName, c4MajVer, c4MinVer, c4Revision);

    // Add this facilty to the facility list
    AddToList(this);
}

TFacility::TFacility(   const   TString&            strFacName
                        , const TString&            strLoadPath
                        , const tCIDLib::EModTypes  eModType
                        , const tCIDLib::TCard4     c4MajVer
                        , const tCIDLib::TCard4     c4MinVer
                        , const tCIDLib::TCard4     c4Revision
                        , const tCIDLib::EModFlags  eFlags
                        , const tCIDLib::TBoolean   bLoad) :

    TModule(strFacName, strLoadPath, eModType, eFlags, bLoad)
    , m_c4MajVer(c4MajVer)
    , m_c4MinVer(c4MinVer)
    , m_c4Revision(c4Revision)
{
    // Call a common init method
    CommonInit(strFacName, c4MajVer, c4MinVer, c4Revision);

    // Add this facilty to the facility list
    AddToList(this);
}

TFacility::~TFacility()
{
    RemoveFromList(this);
}


// ---------------------------------------------------------------------------
//  TFacility: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Similar to eLogMode below, but a more convenient syntax for code to use
//  to check whether something they want to log at a given level should be
//  logged or not, i.e. whehter the passed severity is above the level for
//  this facility (or the global mode.)
//
tCIDLib::TBoolean
TFacility::bShouldLogAt(const tCIDLib::ESeverities eToCheck) const
{
    // Grab the verbosity for this facility from the stats cache
    tCIDLib::ESeverities eSev = tCIDLib::ESeverities
    (
        TStatsCache::c8CheckValue(m_sciLogThreshold)
    );

    // Sanity check it and adjust if needed
    if (eSev >= tCIDLib::ESeverities::Count)
        eSev = tCIDLib::ESeverities::Max;

    return ((eToCheck >= eSev) || (eToCheck >= s_eGlobalLogMode));
}


//
//  When exceptions are caught, the code often wants to see if should log
//  the exception. This will do that check for them.
//
tCIDLib::TBoolean TFacility::bShouldLog(const TLogEvent& logeToCheck) const
{
    // If already logged, say no
    if (logeToCheck.bLogged())
        return kCIDLib::False;

    // Else just check its severity
    return bShouldLogAt(logeToCheck.eSeverity());
}


// Return the verion and revision values of this facility
tCIDLib::TCard4 TFacility::c4MajVersion() const
{
    return m_c4MajVer;
}


tCIDLib::TCard4 TFacility::c4MinVersion() const
{
    return m_c4MinVer;
}


tCIDLib::TCard4 TFacility::c4Revision() const
{
    return m_c4MinVer;
}

//
//  Return the logging verbosity from the stats cache for this facility,
//  or the global mode, whichever is lower.
//
tCIDLib::ESeverities TFacility::eLogThreshold() const
{
    // Grab the verbosity for this facility from the stats cache
    tCIDLib::ESeverities eSev = tCIDLib::ESeverities
    (
        TStatsCache::c8CheckValue(m_sciLogThreshold)
    );

    // Sanity check it and adjust if needed
    if (eSev >= tCIDLib::ESeverities::Count)
        eSev = tCIDLib::ESeverities::Max;

    // If the facility level mode is larger than the global, return the global
    if (eSev > s_eGlobalLogMode)
        return s_eGlobalLogMode;

    // Otherwise, return the facility specific mode
    return eSev;
}


// Provide access to our version formatted out in standard test form
const TString& TFacility::strVersion() const
{
    return m_strVersion;
}


//
//  Set our logging flags. They caller provides the bits to change and a mask of those
//  they want to be affected.
//
tCIDLib::TVoid
TFacility::SetLoggingFlags( const   tCIDLib::TCard8 c8ToSet
                            , const tCIDLib::TCard8 c8Mask)
{
    // Get the current value as the full Card8
    tCIDLib::TCard8 c8Val = TStatsCache::c8CheckValue(m_sciLogFlags);

    // Turn off any mask bits, then set the ones that need setting
    c8Val &= ~c8Mask;
    c8Val |= (c8Mask & c8ToSet);

    // ANd now set it back
    TStatsCache::SetValue(m_sciLogFlags, c8Val);
}



// Set our logging threshold
tCIDLib::TVoid TFacility::SetLogThreshold(const tCIDLib::ESeverities eToSet)
{
    TStatsCache::SetValue(m_sciLogThreshold, tCIDLib::c4EnumOrd(eToSet));
}


//
//  Set our log tracing flag. If set, we log all thrown exceptions, or or parent
//  TModule class does.
//
tCIDLib::TVoid TFacility::SetLogTrace(const tCIDLib::TBoolean bToSet)
{
    TStatsCache::bSetFlag(m_sciLogTrace, bToSet);
}


// ---------------------------------------------------------------------------
//  TFacility: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  Let our parent class see our error tracking setting. If it's set, he will log
//  all thrown exceptions.
//
tCIDLib::TBoolean TFacility::bTraceErrs() const
{
    return TStatsCache::bCheckFlag(m_sciLogTrace);
}


// ---------------------------------------------------------------------------
//  TFacility: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We cannot provide these directly, because each facility has it's own logging
//  flag enum and we want each to have its own, typesafe, method. But they can just
//  turn around and call us here after converting the enum to a number and we can
//  do the work.
//
//  We call our bShouldLog type methods above to check the overall severity. If it
//  returns true, then we also check the indicated flag bit.
//
tCIDLib::TBoolean
TFacility::bCheckLogFlags(  const   tCIDLib::ESeverities    eToCheck
                            , const tCIDLib::TCard4         c4FlagBit) const
{
    if (!bShouldLogAt(eToCheck))
        return kCIDLib::False;
    return TStatsCache::bCheckBitFlag(m_sciLogFlags, c4FlagBit);
}


tCIDLib::TBoolean
TFacility::bCheckLogFlags(  const   TLogEvent&      logeToCheck
                            , const tCIDLib::TCard4 c4FlagBit) const
{
    if (!bShouldLog(logeToCheck))
        return kCIDLib::False;
    return TStatsCache::bCheckBitFlag(m_sciLogFlags, c4FlagBit);
}


// ---------------------------------------------------------------------------
//  TFacility: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Theres a bit of setup and we have more than one ctor so this is broken out
tCIDLib::TVoid
TFacility::CommonInit(  const   TString&        strFacName
                        , const tCIDLib::TCard4 c4MajVer
                        , const tCIDLib::TCard4 c4MinVer
                        , const tCIDLib::TCard4 c4Revision)
{
    //
    //  Set the log level initial to any value that might have been set on
    //  the command line or in the environment for this facility. This
    //  wil register the item and default to the correct type because of
    //  the method we call.
    //
    TString strKey(kCIDLib::pszStat_Scope_LogThresh);
    strKey.Append(strFacName);
    TStatsCache::SetValue
    (
        strKey.pszBuffer()
        , m_sciLogThreshold
        , tCIDLib::c4EnumOrd(TSysInfo::eDefModuleLog(strFacName))
    );

    // For this one, the default value is fine, so just register it
    strKey = kCIDLib::pszStat_Scope_LogFlags;
    strKey.Append(strFacName);
    TStatsCache::RegisterItem(strKey.pszBuffer(), tCIDLib::EStatItemTypes::BitFlags, m_sciLogFlags);

    // For this one, the default value is fine, so just register it
    strKey = kCIDLib::pszStat_Scope_LogTrace;
    strKey.Append(strFacName);
    TStatsCache::RegisterItem(strKey.pszBuffer(), tCIDLib::EStatItemTypes::Flag, m_sciLogTrace);

    // Format the version
    m_strVersion = CIDLib_Facility::pszVersionFmt;
    m_strVersion.eReplaceToken(c4MajVer, L'1');
    m_strVersion.eReplaceToken(c4MinVer, L'2');
    m_strVersion.eReplaceToken(c4Revision, L'3');
}



// ---------------------------------------------------------------------------
//  TFacility: Private, static data members
// ---------------------------------------------------------------------------
tCIDLib::ESeverities TFacility::s_eGlobalLogMode(tCIDLib::ESeverities::Failed);


