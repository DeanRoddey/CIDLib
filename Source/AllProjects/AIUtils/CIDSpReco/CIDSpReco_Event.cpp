//
// FILE NAME: CIDSpReco_Event.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/10/2017
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
//  This file implements the classes that we use to pass on speech recognition events
//  to the application.
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
#include    "CIDSpReco_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TCIDSpeechRecoEv
//  PREFIX: sprecev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSpeechRecoEv: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDSpeechRecoEv::TCIDSpeechRecoEv() :

    m_f4RuleConfidence(-1.0)
    , m_tmAt(tCIDLib::ESpecialTimes::CurrentTime)
{
    // Set the format we want the time stamp to be in
    m_tmAt.strDefaultFormat(TTime::strCTime());
}

TCIDSpeechRecoEv::TCIDSpeechRecoEv(const TCIDSpeechRecoEv& sprecevSrc) :

    m_colSemList(sprecevSrc.m_colSemList)
    , m_f4RuleConfidence(sprecevSrc.m_f4RuleConfidence)
    , m_strRuleName(sprecevSrc.m_strRuleName)
    , m_tmAt(sprecevSrc.m_tmAt)

    // If debugging, include the rule info
    #if CID_DEBUG_ON
    , m_colRuleList(sprecevSrc.m_colRuleList)
    #endif
{
}

TCIDSpeechRecoEv::~TCIDSpeechRecoEv()
{
}


// ---------------------------------------------------------------------------
//  TCIDSpeechRecoEv: Public operators
// ---------------------------------------------------------------------------
TCIDSpeechRecoEv& TCIDSpeechRecoEv::operator=(const TCIDSpeechRecoEv& sprecevSrc)
{
    if (this != &sprecevSrc)
    {
        m_colSemList        = sprecevSrc.m_colSemList;
        m_f4RuleConfidence  = sprecevSrc.m_f4RuleConfidence;
        m_strRuleName       = sprecevSrc.m_strRuleName;
        m_tmAt              = sprecevSrc.m_tmAt;

        // If debugging, include the rule info
        #if CID_DEBUG_ON
        m_colRuleList = sprecevSrc.m_colRuleList;
        #endif
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCIDSpeechRecoEv: Public, non-virtual methods
// ---------------------------------------------------------------------------

// A helper to find a specific semantic tree value by its name
tCIDLib::TBoolean
TCIDSpeechRecoEv::bFindSemVal(const TString& strName, TString& strValue) const
{
    tCIDLib::TCard4 c4At;
    if (bFindSemIndex(strName, c4At))
    {
        strValue = m_colSemList[c4At].strValue();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Provide raw access to our semantic list
const tCIDSpReco::TSemList& TCIDSpeechRecoEv::colSemList() const
{
    return m_colSemList;
}


tCIDLib::TFloat4 TCIDSpeechRecoEv::f4RuleConfidence() const
{
    return m_f4RuleConfidence;
}


// Find the raw semantic info for the passed semantic name
const TKeyNumPair*
TCIDSpeechRecoEv::pknumFindSemVal(const TString& strName) const
{
    tCIDLib::TCard4 c4At;
    if (bFindSemIndex(strName, c4At))
        return &m_colSemList[c4At];

    return nullptr;
}


//
//  This one finds a semantic value and throws if not present. We have one version
//  that just returns the value and ones that returns the value and the confidence
//  levels.
//
const TString&
TCIDSpeechRecoEv::strFindSemVal(const TString& strName) const
{
    tCIDLib::TCard4 c4At;
    if (!bFindSemIndex(strName, c4At))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcSem_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strName
        );
    }
    return m_colSemList[c4At].strValue();
}


const TString&
TCIDSpeechRecoEv::strFindSemVal(const   TString&            strName
                                ,       tCIDLib::TFloat4&   f4Confidence) const
{
    tCIDLib::TCard4 c4At;
    if (!bFindSemIndex(strName, c4At))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcSem_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strName
        );
    }

    const TKeyNumPair& knumRet = m_colSemList[c4At];
    f4Confidence = knumRet.f4Value();
    return knumRet.strValue();
}


const TString& TCIDSpeechRecoEv::strRuleName() const
{
    return m_strRuleName;
}


//
//  This is called to set us up. We get the name of the main rule that invoked, the
//  reported semantic and rule values, and a mapping list.
//
//  We use the mapping list to update the confidence levels in the semantic values.
//  The rules are discarded and just the resulting updated semantic values are stored.
//  (Unless in debug mode, where we keep the rules as well, for debugging purposes.)
//
//  If we have any mappings, then any unmapped semnatic values are set to 100%, on the
//  assumption that those were set from literal values and are always correct. If we
//  have no mappings, we leave the semantic confidence levels as is, as semi-useless as
//  they are.
//
tCIDLib::TVoid
TCIDSpeechRecoEv::Set(  const   TString&                strRuleName
                        , const tCIDLib::TFloat4        f4RuleConfidence
                        , const tCIDSpReco::TSemList&   colSemList
                        , const tCIDSpReco::TRuleList&  colRuleList)
{
    m_f4RuleConfidence = f4RuleConfidence;
    m_strRuleName = strRuleName;

    // Go ahead and store the semantics, since that makes things easier below
    m_colSemList = colSemList;

    // If in debug mode, save the rules for later formatting out
    #if CID_DEBUG_ON
    m_colRuleList = colRuleList;
    #endif

    // Reset the time stamp
    m_tmAt.SetToNow();

    //
    //  The first thing we need to do some pre-processing on the semantic values. We go
    //  through and pull out all of the ones that start with Map_. We put them into a
    //  separate mapping list, and we remove the Map_ prefix from those, so now we have
    //  a list where the key is the semantic value to update and the value is the sub-rule
    //  to get the confidence values from.
    //
    tCIDLib::TKVPList colMap(m_colSemList.c4ElemCount() / 2);
    {
        tCIDLib::TCard4 c4Count = m_colSemList.c4ElemCount();
        tCIDLib::TCard4 c4Index = 0;
        TString strKey;
        while (c4Index < c4Count)
        {
            TKeyNumPair& knumCur = m_colSemList[c4Index];

            if (knumCur.strKey().bStartsWithI(kCIDSpReco::strVal_MapPref))
            {
                //
                //  In some cases they may need to conditionally set these mappings and they
                //  can't conditionally create them, so we let htem set them to an empty
                //  value, to tell us to ignore them.
                //
                if (!knumCur.strValue().bIsEmpty())
                {
                    strKey.CopyInSubStr(knumCur.strKey(), kCIDSpReco::strVal_MapPref.c4Length());
                    colMap.objAdd(TKeyValuePair(strKey, knumCur.strValue()));
                }

                m_colSemList.RemoveAt(c4Index);
                c4Count--;
            }
             else
            {
                c4Index++;
            }
        }
    }

    //
    //  If we have semantic mappings, then let's go through and do those, pulling out
    //  the confidence levels from the mapped rules into the semantics. Else we will
    //  just keep the confidence values we have.
    //
    const tCIDLib::TCard4 c4MapCount = colMap.c4ElemCount();
    if (c4MapCount)
    {
        const tCIDLib::TCard4 c4SemCount = m_colSemList.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SemCount; c4Index++)
            m_colSemList[c4Index].f4Value(1.0);

        // Otherwise we need to do the mapping
        const tCIDLib::TCard4 c4RuleCount = colRuleList.c4ElemCount();
        TString strMapVal;
        TString strRulePath;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MapCount; c4Index++)
        {
            tCIDLib::TCard4 c4MappedInd;

            //
            //  The mapping key holds the target semantic value to update, and the value
            //  holds the source rule to get the confidence values from.
            //
            const TString& strMapTar = colMap[c4Index].strKey();
            const TString& strMapSrc = colMap[c4Index].strValue();

            // Now we need to create the rule path that this one maps to
            strRulePath = L"/";
            strRulePath.Append(strRuleName);
            strRulePath.Append(kCIDLib::chForwardSlash);
            strRulePath.Append(strMapSrc);

            // Now let's find this rule in the rule list
            tCIDLib::TCard4 c4CurRuleInd = 0;
            while (c4CurRuleInd < c4RuleCount)
            {
                if (colRuleList[c4CurRuleInd].strKey().bCompareI(strRulePath))
                    break;
                c4CurRuleInd++;
            }

            // If not found, we failed
            if (c4CurRuleInd == c4RuleCount)
            {
                facCIDSpReco().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSpRecoErrs::errcSem_MapSemRuleNotFound
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , strRulePath
                    , strRuleName
                );
            }

            // Now we have to find the target semantic value we are to update
            if (!bFindSemIndex(strMapTar, c4MappedInd))
            {
                facCIDSpReco().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSpRecoErrs::errcSem_MapSemTarNotFound
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , strMapTar
                    , strRuleName
                );
            }

            // OK, we can now update this guy
            m_colSemList[c4MappedInd].f4Value(colRuleList[c4CurRuleInd].f4Value());
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDSpeechRecoEv: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// For debugging purposes mainly
tCIDLib::TVoid TCIDSpeechRecoEv::FormatTo(TTextOutStream& strmDest)const
{
    strmDest    << L"{\n"
                << L"   RULE: " << m_strRuleName << kCIDLib::NewLn
                << L"   BASIC CONF: " << m_f4RuleConfidence << kCIDLib::NewLn
                << L"   TIME: " << m_tmAt << kCIDLib::NewLn;

    // Show the semantic info
    strmDest    << L"   SEMANTIC\n"
                << L"   {\n";

    tCIDLib::TCard4 c4Count = m_colSemList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colSemList[c4Index];

        strmDest << TTextOutStream::Spaces(4);
        strmDest    << L'[' << TFloat(knumCur.f4Value(), 2)
                    << L"] " << knumCur.strKey() << L"=" << knumCur.strValue()
                    << kCIDLib::NewLn;
    }
    strmDest    << L"   }\n";

    // IN debug mode, we keep the rules as well
    #if CID_DEBUG_ON
    strmDest    << L"   RULES\n"
                << L"   {\n";

    c4Count = m_colRuleList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyNumPair& knumCur = m_colRuleList[c4Index];

        strmDest << TTextOutStream::Spaces(4);
        strmDest    << L'[' << TFloat(knumCur.f4Value(), 2)
                    << L"] " << knumCur.strKey() << kCIDLib::NewLn;
    }
    strmDest    << L"   }\n";
    #endif

    strmDest << L"}" << kCIDLib::NewEndLn;
}


// ---------------------------------------------------------------------------
//  TCIDSpeechRecoEv: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  See if we have the indicated semantic value. If so, return true and the index that we
//  found it at.
//
tCIDLib::TBoolean
TCIDSpeechRecoEv::bFindSemIndex(const TString& strName, tCIDLib::TCard4& c4At) const
{
    const tCIDLib::TCard4 c4Count = m_colSemList.c4ElemCount();
    for (c4At = 0; c4At < c4Count; c4At++)
    {
        const TKeyNumPair& knumCur = m_colSemList[c4At];
        if (knumCur.strKey().bCompareI(strName))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


