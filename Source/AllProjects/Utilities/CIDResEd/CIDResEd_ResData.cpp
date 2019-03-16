//
// FILE NAME: CIDResEd_ResData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdData,TObject)




// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDResEd_ResData
{
    // -----------------------------------------------------------------------
    //  Some strings that we look for during parsing
    // -----------------------------------------------------------------------
    const TString   strKW_BITMAPS(L"BITMAPS=");
    const TString   strKW_BITMAPSEnd(L"END BITMAPS");

    const TString   strKW_COMMON(L"COMMON=");
    const TString   strKW_COMMONEnd(L"END COMMON");

    const TString   strKW_CTRL(L"CTRL=");
    const TString   strKW_CTRLEnd(L"END CTRL");

    const TString   strKW_DIALOG(L"DIALOG=");

    const TString   strKW_ERRORS(L"ERRORS=");
    const TString   strKW_ERRORSEnd(L"END ERRORS");

    const TString   strKW_ICONS(L"ICONS=");
    const TString   strKW_ICONSEnd(L"END ICONS");

    const TString   strKW_JPEGS(L"JPEGS=");
    const TString   strKW_JPEGSEnd(L"END JPEGS");

    const TString   strKW_MENU(L"MENU=");
    const TString   strKW_MENUEnd(L"END MENU");

    const TString   strKW_MESSAGES(L"MESSAGES=");
    const TString   strKW_MESSAGESEnd(L"END MESSAGES");

    const TString   strKW_PNGS(L"PNGS=");
    const TString   strKW_PNGSEnd(L"END PNGS");
}



// ---------------------------------------------------------------------------
//   CLASS: TResEdData
//  PREFIX: redat
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdData: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdData::TResEdData() :

    m_remilCommon(tCIDResEd::EMsgTypes::Common)
    , m_remilErrs(tCIDResEd::EMsgTypes::Error)
    , m_remilMsgs(tCIDResEd::EMsgTypes::Message)
{
}

TResEdData::TResEdData(const TResEdData& redatSrc) :

    m_colBitmaps(redatSrc.m_colBitmaps)
    , m_colDlgs(redatSrc.m_colDlgs)
    , m_colIcons(redatSrc.m_colIcons)
    , m_colJPEGs(redatSrc.m_colJPEGs)
    , m_colMenus(redatSrc.m_colMenus)
    , m_colPNGs(redatSrc.m_colPNGs)
    , m_remilCommon(redatSrc.m_remilCommon)
    , m_remilErrs(redatSrc.m_remilErrs)
    , m_remilMsgs(redatSrc.m_remilMsgs)
    , m_strResNamespace(redatSrc.m_strResNamespace)
{
}

TResEdData::~TResEdData()
{
}


// ---------------------------------------------------------------------------
//  TResEdData: Public operator
// ---------------------------------------------------------------------------
TResEdData& TResEdData::operator=(const TResEdData& redatSrc)
{
    if (this != &redatSrc)
    {
        m_colBitmaps      =  redatSrc.m_colBitmaps;
        m_colDlgs         =  redatSrc.m_colDlgs;
        m_colIcons        =  redatSrc.m_colIcons;
        m_colJPEGs        =  redatSrc.m_colJPEGs;
        m_colMenus        =  redatSrc.m_colMenus;
        m_colPNGs         =  redatSrc.m_colPNGs;
        m_remilCommon     =  redatSrc.m_remilCommon;
        m_remilErrs       =  redatSrc.m_remilErrs;
        m_remilMsgs       =  redatSrc.m_remilMsgs;
        m_strResNamespace =  redatSrc.m_strResNamespace;
    }
    return *this;
}


tCIDLib::TBoolean TResEdData::operator==(const TResEdData& redatSrc) const
{
    //
    //  Note that the common messages don't count towards equality since we never
    //  change those.
    //
    return
    (
        (m_colDlgs == redatSrc.m_colDlgs)
        && (m_colBitmaps == redatSrc.m_colBitmaps)
        && (m_colIcons == redatSrc.m_colIcons)
        && (m_colJPEGs == redatSrc.m_colJPEGs)
        && (m_colMenus == redatSrc.m_colMenus)
        && (m_colPNGs == redatSrc.m_colPNGs)
        && (m_remilErrs == redatSrc.m_remilErrs)
        && (m_remilMsgs == redatSrc.m_remilMsgs)
        && (m_strResNamespace == redatSrc.m_strResNamespace)
    );
}

tCIDLib::TBoolean TResEdData::operator!=(const TResEdData& redatToComp) const
{
    return !operator==(redatToComp);
}


// ---------------------------------------------------------------------------
//  TResEdData: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Store an edited dialog description back into the list
tCIDLib::TBoolean
TResEdData::bSetDialog(const TResEdDlgDesc& reddToSet)
{
    // Find the one with the same edit id
    tCIDLib::TCard4 c4Index = c4FindDlgIndex(reddToSet);
    if (c4Index == kCIDLib::c4MaxCard)
    {
        TErrBox msgbNotFnd(L"The dialog is no longer in the list");
        msgbNotFnd.ShowIt(facCIDResEd.wndMain());
        return kCIDLib::False;
    }

    m_colDlgs[c4Index] = reddToSet;
    return kCIDLib::True;
}


// Called after opening a session, to validate the parsed data
tCIDLib::TBoolean TResEdData::bValidate()
{
    tCIDLib::TStrList colErrs(16);
    TString strErrMsg;

    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgDesc& dlgdCur = m_colDlgs[c4Index];

        // Search for this guy's id, starting at the next index position
        const TResEdDlgDesc* pdlgdDup = preddByResId(dlgdCur.ridDlg(), c4Index + 1);
        if (pdlgdDup)
        {
            strErrMsg = L"Dialog id ";
            strErrMsg.AppendFormatted(dlgdCur.ridDlg());
            strErrMsg.Append(L" is used more than once");
            colErrs.objAdd(strErrMsg);
        }

        // And the same for names
        pdlgdDup = preddByName(dlgdCur.strName(), c4Index + 1);
        if (pdlgdDup)
        {
            strErrMsg = L"Dialog symbolic name ";
            strErrMsg.Append(dlgdCur.strName());
            strErrMsg.Append(L" is used more than once");
            colErrs.objAdd(strErrMsg);
        }

        // Only take so many errors
        if (colErrs.c4ElemCount() >= 128)
            break;
    }

    if (!colErrs.bIsEmpty())
    {
        facCIDWUtils().ShowList(facCIDResEd.wndMain(), L"Resource Error List", colErrs);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



//
//  Called to add a new dialog to the list. We just set it up with default
//  settings, other than the name, which must be unique and it's gotten
//  from the user.
//
tCIDLib::TCard4
TResEdData::c4AddDialog(const TString& strName, tCIDLib::TResId& ridAssigned)
{
    // We have to find an available dialog id.
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();

    tCIDLib::TResId ridNew = 1000;
    if (c4Count)
    {
        //
        //  More than zero, so let's find an unused value. So we add all the
        //  existing ids to a vector.
        //
        TFundVector<tCIDLib::TResId> fcolVals(c4Count);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            fcolVals.c4AddElement(m_colDlgs[c4Index].ridDlg());

        // Now just find the first unused value past zero
        tCIDLib::TCard4 c4Index = 0;
        for (; c4Index < c4Count; c4Index++)
        {
            CIDAssert(fcolVals[c4Index] != 0, L"Got a zero dialog id");
            if (!fcolVals.bElementPresent(ridNew))
                break;
            ridNew++;
        }
    }

    // Now set up a basic resource definition and insert sorted
    TResEdDlgDesc reddNew(strName, ridNew);

    tCIDLib::TCard4 c4At;
    m_colDlgs.InsertSorted(reddNew, &TResEdDlgDesc::eComp, c4At);

    // Return the insert index and the id assigned
    ridAssigned = ridNew;
    return c4At;
}


//
//  Find the index of the dialog with the same edit id as the passed one. If none
//  return max card.
//
tCIDLib::TCard4 TResEdData::c4FindDlgIndex(const TResEdDlgDesc& reddFind) const
{
    // Call the other version with the edit id
    return c4FindDlgIndex(reddFind.c4EditId());
}

tCIDLib::TCard4 TResEdData::c4FindDlgIndex(const tCIDLib::TCard4 c4EditId) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colDlgs[c4Index].c4EditId() == c4EditId)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


// Provide access to the dialog list
const tCIDResEd::TDlgDescList& TResEdData::colDlgs() const
{
    return m_colDlgs;
}


//
//  See if there is a dialog with the passed name or resource id or edit id
//
const TResEdDlgDesc* TResEdData::preddByEditId(const tCIDLib::TCard4 c4Find) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colDlgs[c4Index].c4EditId() == c4Find)
            return &m_colDlgs[c4Index];
    }
    return nullptr;
}

const TResEdDlgDesc*
TResEdData::preddByResId(const  tCIDLib::TResId ridFind
                        , const tCIDLib::TCard4 c4StartAt) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = c4StartAt; c4Index < c4Count; c4Index++)
    {
        if (m_colDlgs[c4Index].ridDlg() == ridFind)
            return &m_colDlgs[c4Index];
    }
    return nullptr;
}

const TResEdDlgDesc*
TResEdData::preddByName(const   TString&            strFind
                        , const tCIDLib::TCard4     c4StartAt) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = c4StartAt; c4Index < c4Count; c4Index++)
    {
        if (m_colDlgs[c4Index].strName().bCompareI(strFind))
            return &m_colDlgs[c4Index];
    }
    return nullptr;
}



//
//  We get the path to the .MsgText file and we parse it and fill in the
//  message lists. The caller tells us if we are parsing the facility's
//  messages or the common messages. If the latter, we have only messages,
//  and we use a different list.
//
tCIDLib::TVoid
TResEdData::ParseMsgText(const  TString&            strFile
                        , const tCIDLib::TBoolean   bCommon)
{
    // Create a file spooler
    TResEdSpooler spoolMsgs(strFile);

    // The first thing we have to see is the CTRL= block
    TString strInBuf;
    spoolMsgs.CheckNext(CIDResEd_ResData::strKW_CTRL);

    tCIDLib::TStrList colPrefs(2);
    colPrefs.objAdd(L"ErrPref=");
    colPrefs.objAdd(L"MsgPref=");

    // Process the lines until we hit the end
    while (kCIDLib::True)
    {
        const tCIDLib::TCard4 c4Ind = spoolMsgs.c4CheckPrefix
        (
            colPrefs, CIDResEd_ResData::strKW_CTRLEnd, strInBuf
        );

        // If the end string, then we are done
        if (c4Ind == kCIDLib::c4MaxCard)
        {
            break;
        }
         else if ((c4Ind == 0) || (c4Ind == 1))
        {
            //
            //  Sanity check the value. Either prefix should start with
            //  k and be at least a couple more characters long.
            //
            if ((strInBuf.c4Length() < 3)
            ||  (strInBuf[0] != kCIDLib::chLatin_k))
            {
                facCIDResEd.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kResEdErrs::errcSrcFile_BadNamespace
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(spoolMsgs.c4CurLine())
                    , strFile
                );
            }

            if (!bCommon)
            {
                if (c4Ind == 0)
                    m_remilErrs.strNamespace(strInBuf);
                else
                    m_remilMsgs.strNamespace(strInBuf);
            }
        }
         else
        {
            // Shouldn't get here but check if debugging
            CIDAssert2(L"Failed to catch prefix failure");
        }
    }

    //
    //  Now we can see one or more error or message blocks. There can
    //  be more than one of each, but they just append to the same lists.
    //
    while (kCIDLib::True)
    {
        //
        //  Check for another block start. It's legal to hit the end here
        //  so we can break out if so.
        //
        if (!spoolMsgs.bReadLine(strInBuf, kCIDLib::True))
            break;

        // It's got to be the start of a messge or error block
        if (strInBuf == CIDResEd_ResData::strKW_ERRORS)
            m_remilErrs.ParseBlock(spoolMsgs, CIDResEd_ResData::strKW_ERRORSEnd);
        else if (strInBuf == CIDResEd_ResData::strKW_MESSAGES)
            m_remilMsgs.ParseBlock(spoolMsgs, CIDResEd_ResData::strKW_MESSAGESEnd);
        else if (strInBuf == CIDResEd_ResData::strKW_COMMON)
            m_remilCommon.ParseBlock(spoolMsgs, CIDResEd_ResData::strKW_COMMONEnd);
        else
        {
            // Can't be valid
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_Expected
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"ERRORS= or MESSAGES=")
                , TCardinal(spoolMsgs.c4CurLine())
                , strFile
            );
        }
    }
}


//
//  This is called to parse the resource file if there is one
//
tCIDLib::TVoid TResEdData::ParseResources(const TString& strSrcFile)
{
    // Create a file spooler
    TResEdSpooler spoolRes(strSrcFile);

    // The first thing we have to see is the CTRL= block
    TString strInBuf;
    spoolRes.CheckNext(CIDResEd_ResData::strKW_CTRL);

    tCIDLib::TStrList colPrefs(1);
    colPrefs.objAdd(L"ResPref=");

    // Process the lines until we hit the end of the block
    while (kCIDLib::True)
    {
        const tCIDLib::TCard4 c4Ind = spoolRes.c4CheckPrefix
        (
            colPrefs, CIDResEd_ResData::strKW_CTRLEnd, strInBuf
        );

        // If the end string, then we are done
        if (c4Ind == kCIDLib::c4MaxCard)
        {
            break;
        }
         else if (c4Ind == 0)
        {
            //
            //  Sanity check the value. The prefix should start with
            //  k and be at least a couple more characters long.
            //
            if ((strInBuf.c4Length() < 3)
            ||  (strInBuf[0] != kCIDLib::chLatin_k))
            {
                facCIDResEd.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kResEdErrs::errcSrcFile_BadNamespace
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(spoolRes.c4CurLine())
                    , strSrcFile
                );
            }

            m_strResNamespace = strInBuf;
        }
         else
        {
            // Shouldn't get here but check if debugging
            CIDAssert2(L"Failed to catch prefix failure");
        }
    }

    //
    //  Now we can see one or more blocks of images, menus, dialogs, etc..
    //
    while (kCIDLib::True)
    {
        //
        //  Check for another block start. It's legal to hit the end here
        //  so we can break out if so.
        //
        if (!spoolRes.bReadLine(strInBuf, kCIDLib::True))
            break;

        // It's got to be the start of a messge or error block
        if (strInBuf == CIDResEd_ResData::strKW_BITMAPS)
        {
            ParseImgs(spoolRes, CIDResEd_ResData::strKW_BITMAPSEnd, m_colBitmaps);
        }
         else if (strInBuf == CIDResEd_ResData::strKW_DIALOG)
        {
            // Create a temp dialog description item and ask it to parse itself
            TResEdDlgDesc reddNew;
            reddNew.ParseFrom(spoolRes);

            // Add it to our list, sorted by name
            tCIDLib::TCard4 c4At;
            m_colDlgs.InsertSorted(reddNew, &TResEdDlgDesc::eComp, c4At);
        }
         else if (strInBuf == CIDResEd_ResData::strKW_ICONS)
        {
            ParseImgs(spoolRes, CIDResEd_ResData::strKW_ICONSEnd, m_colIcons);
        }
         else if (strInBuf == CIDResEd_ResData::strKW_JPEGS)
        {
            ParseImgs(spoolRes, CIDResEd_ResData::strKW_JPEGSEnd, m_colJPEGs);
        }
         else if (strInBuf == CIDResEd_ResData::strKW_MENU)
        {
            // Create a new menu object and ask it to parse itself
            TResEdMenuDesc remenuNew;
            tCIDLib::TCard4 c4Level = 0;
            remenuNew.ParseFrom(spoolRes, c4Level);

            // It worked, so add it to our list
            m_colMenus.objAdd(remenuNew);
        }
         else if (strInBuf == CIDResEd_ResData::strKW_PNGS)
        {
            ParseImgs(spoolRes, CIDResEd_ResData::strKW_PNGSEnd, m_colPNGs);
        }
         else
        {
            // Can't be valid
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_Expected
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"ERRORS= or MESSAGES=")
                , TCardinal(spoolRes.c4CurLine())
                , strSrcFile
            );
        }
    }

}


// Return a ref to the indicated dialog description
const TResEdDlgDesc& TResEdData::reddAt(const tCIDLib::TCard4 c4At) const
{
    return m_colDlgs[c4At];
}


// Provide access to the various message lists
const TResEdMsgList&
TResEdData::remilByType(const tCIDResEd::EMsgTypes eType) const
{
    if (eType == tCIDResEd::EMsgTypes::Common)
        return m_remilCommon;
    else if (eType == tCIDResEd::EMsgTypes::Message)
        return m_remilMsgs;
    return m_remilErrs;
}


// Just resets us back to default state
tCIDLib::TVoid TResEdData::Reset()
{
    m_colBitmaps.RemoveAll();
    m_colDlgs.RemoveAll();
    m_colIcons.RemoveAll();
    m_colJPEGs.RemoveAll();
    m_colMenus.RemoveAll();
    m_colPNGs.RemoveAll();
    m_remilErrs.Reset();
    m_remilMsgs.Reset();
    m_remilCommon.Reset();

    m_strResNamespace.Clear();
}


// Look up the resource text and return it
const TString& TResEdData::strLoadResText(const TTextSym& tsymToLoad)
{
    TResEdMsgList* premilTar;
    if (tsymToLoad.eType() == tCIDResEd::EMsgTypes::Common)
        premilTar = &m_remilCommon;
    else if (tsymToLoad.eType() == tCIDResEd::EMsgTypes::Error)
        premilTar = &m_remilErrs;
    else
        premilTar = &m_remilMsgs;

    tCIDLib::TCard4 c4At = premilTar->c4FindByName(tsymToLoad.strRealSym());
    if (c4At != kCIDLib::c4MaxCard)
        return premilTar->remitAt(c4At).strMsgText();

    // Not found, just return an empty string
    return TString::strEmpty();
}


tCIDLib::TVoid
TResEdData::StoreMsgText(const  TResEdMsgList&  remilErrs
                        , const TResEdMsgList&  remilMsgs)
{
    m_remilErrs = remilErrs;
    m_remilMsgs = remilMsgs;
}


//
//  We'll write out data out to the target files if that is necessary (due
//  to a changes having been made, or the file not existing.) We get copy
//  that was made when the session was loaded (or saved) so we can know
//  if changes have occured, and won't bother to write any files that are
//  not neeeded.
//
tCIDLib::TVoid
TResEdData::WriteTo(const   TString&    strPath
                    , const TString&    strName
                    , const TResEdData& redatPrev)
{
    // Build up the message file path
    TPathStr pathTarFile = strPath;
    pathTarFile.AddLevel(strName);
    pathTarFile.Append(L"_en.MsgText");

    // Set up a time object to write out a time stamp
    TTime tmCur(tCIDLib::ESpecialTimes::CurrentTime);
    tmCur.strDefaultFormat(TTime::strCTime());

    //
    //  If either message list changed, then we have to write the msg file
    //
    if ((m_remilErrs != redatPrev.m_remilErrs)
    ||  (m_remilMsgs != redatPrev.m_remilMsgs))
    {
        // Resort the ones that changed
        if (m_remilErrs != redatPrev.m_remilErrs)
            m_remilErrs.SortAndNumber();

        if (m_remilMsgs != redatPrev.m_remilMsgs)
            m_remilMsgs.SortAndNumber();

        //
        //  Store the message file. We'll write it first to a temp file to
        //  make sure it gets written correctly, then we'll swap them.
        //
        TString strTmpFile(pathTarFile);
        strTmpFile.Append(L"_New");
        {
            TTextFileOutStream strmMsgTar
            (
                strTmpFile
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Write
            );

            // Write the header, then we can call the mesage lists to do their bit
            strmMsgTar << L"; FILE NAME: " << pathTarFile << kCIDLib::NewLn
                       << L";      DATE: " << tmCur << kCIDLib::NewLn
                       << L";\n"
                          L"; This file is generated by the CIDLib Resource Editor, so do not\n"
                          L"; edit it by hand\n\n" << kCIDLib::DNewLn;

            // Write the control block
            strmMsgTar << L"CTRL=\n";

            if (!m_remilErrs.strNamespace().bIsEmpty())
            {
                strmMsgTar  << L"    ErrPref=" << m_remilErrs.strNamespace()
                            << kCIDLib::NewLn;
            }

            if (!m_remilMsgs.strNamespace().bIsEmpty())
            {
                strmMsgTar  << L"    MsgPref=" << m_remilMsgs.strNamespace()
                            << kCIDLib::NewLn;
            }

            strmMsgTar << L"END CTRL\n\n";

            m_remilErrs.WriteMsgs(strmMsgTar);
            m_remilMsgs.WriteMsgs(strmMsgTar);

            strmMsgTar.Flush();
        }

        // Now swap the files
        TFileSys::ReplaceFile(pathTarFile, strTmpFile);
    }

    // Build up the resource file path
    pathTarFile = strPath;
    pathTarFile.AddLevel(strName);
    pathTarFile.AppendExt(L"CIDRC");

    //
    //  If any of the resource info changed, then we need to write out the
    //  file.
    //
    if ((m_colDlgs != redatPrev.m_colDlgs)
    ||  (m_colBitmaps != redatPrev.m_colBitmaps)
    ||  (m_colIcons != redatPrev.m_colIcons)
    ||  (m_colJPEGs != redatPrev.m_colJPEGs)
    ||  (m_colMenus != redatPrev.m_colMenus)
    ||  (m_colPNGs != redatPrev.m_colPNGs)
    ||  (m_strResNamespace != redatPrev.m_strResNamespace))
    {
        //
        //  Store the resource file. We'll write it first to a temp file to
        //  make sure it gets written correctly, then we'll swap them.
        //
        TString strTmpFile(pathTarFile);
        strTmpFile.Append(L"_New");
        {
            TTextFileOutStream strmResTar
            (
                strTmpFile
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Write
            );

            // Write the header, then we can call the mesage lists to do their bit
            strmResTar << L"; FILE NAME: " << pathTarFile << kCIDLib::NewLn
                       << L";      DATE: " << tmCur << kCIDLib::NewLn
                       << L";\n"
                          L"; This file is generated by the CIDLib Resource Editor, so do not\n"
                          L"; edit it by hand\n\n" << kCIDLib::DNewLn;

            // Write the control block
            strmResTar << L"CTRL=\n";
            if (!m_strResNamespace.bIsEmpty())
            {
                strmResTar  << L"    ResPref=" << m_strResNamespace
                            << kCIDLib::NewLn;
            }
            strmResTar << L"END CTRL\n\n";

            // Write out the various image blocks
            if (!m_colBitmaps.bIsEmpty())
                WriteImgs(strmResTar, L"BITMAPS", m_colBitmaps);

            if (!m_colIcons.bIsEmpty())
                WriteImgs(strmResTar, L"ICONS", m_colIcons);

            if (!m_colJPEGs.bIsEmpty())
                WriteImgs(strmResTar, L"JPEGS", m_colJPEGs);

            if (!m_colPNGs.bIsEmpty())
                WriteImgs(strmResTar, L"PNGS", m_colPNGs);

            // Write out the menus
            WriteMenus(strmResTar);

            // And write out the dialog descriptions
            WriteDlgs(strmResTar);

            strmResTar.Flush();
        }

        // Now swap the files
        TFileSys::ReplaceFile(pathTarFile, strTmpFile);
    }
}


// ---------------------------------------------------------------------------
//  TResEdData: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Parse out all the image items in an image block and store them. The caller
//  saw the start line, so he knows which list we should put themin and passes
//  us that list.
//
tCIDLib::TVoid
TResEdData::ParseImgs(          TResEdSpooler&              spoolSrc
                        , const TString&                    strEndText
                        ,       tCIDResEd::TImgItemList&    colToFill)
{
    tCIDLib::TCard4 c4Id;
    TResEdImgItem   reimgiNew;
    TString         strInBuf;
    TString         strName;
    TString         strPath;
    while(kCIDLib::True)
    {
        spoolSrc.bReadLine(strName);

        // If we hit the end of the block, we are done
        if (strName == strEndText)
            break;

        //
        //  It has to be a message definition, so parse out the three
        //  values.
        //
        spoolSrc.ParseNameValText(strName, c4Id, strPath);



        //
        //  The first and last characters will be quotes, so remove those.
        //  We'll put them back when we write it back out.
        //
        if ((strPath.chFirst() != kCIDLib::chQuotation)
        ||  (strPath.chLast() != kCIDLib::chQuotation))
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_BadImgPath
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(spoolSrc.c4CurLine())
                , spoolSrc.strSrcFile()
            );
        }
        strPath.Cut(0, 1);
        strPath.DeleteLast();

        // It all looks good, so let's add it, sorted
        tCIDLib::TCard4 c4At;
        reimgiNew.Set(c4Id, strName, strPath);
        colToFill.InsertSorted(reimgiNew, TResEdImgItem::eComp, c4At);
    }
}


tCIDLib::TVoid TResEdData::WriteDlgs(TTextOutStream& strmTar) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgDesc& reddCur = m_colDlgs[c4Index];
        reddCur.WriteTo(strmTar);
    }
}


tCIDLib::TVoid
TResEdData::WriteImgs(          TTextOutStream&             strmTar
                        , const TString&                    strType
                        , const tCIDResEd::TImgItemList&    colList) const
{
    strmTar << strType << L"=\n";

    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdImgItem& reimgiCur = colList[c4Index];
        strmTar << L"    " << reimgiCur.strName()
                << L"    " << reimgiCur.c4Id()
                << L"    \"" << reimgiCur.strPath()
                << L"\"\n";
    }
    strmTar << L"END " << strType << kCIDLib::DNewLn;
}


tCIDLib::TVoid TResEdData::WriteMenus(TTextOutStream& strmTar) const
{
    const tCIDLib::TCard4 c4Count = m_colMenus.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdMenuDesc& remendCur = m_colMenus[c4Index];

        //
        //  Menus are nested, so a level is used. We start it off at zero
        //  level here, and it will bump the level each time it recurses,
        //  so that stuff is correctly nested.
        //
        remendCur.WriteTo(strmTar, 0);
    }
}

