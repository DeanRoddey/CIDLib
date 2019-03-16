//
// FILE NAME: CIDResEd_MsgData.cpp
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
//  This file implements the classes related to the loadable message text
//  related resources.
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
RTTIDecls(TResEdMsgItem,TObject)
RTTIDecls(TResEdMsgList,TObject)




// ---------------------------------------------------------------------------
//   CLASS: TResEdMsgItem
//  PREFIX: remit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdMsgItem: Public, static methods
// ---------------------------------------------------------------------------

// Return a sort comparison for two msg items via their name fields
tCIDLib::ESortComps
TResEdMsgItem::eCompByName( const   TResEdMsgItem&  remit1
                            , const TResEdMsgItem&  remit2)
{
    return remit1.m_strName.eCompareI(remit2.m_strName);
}


// REturn a sort comparision of a key value and the msg item's name field
tCIDLib::ESortComps
TResEdMsgItem::eKeyComp(const   TString&        strKey
                        , const TResEdMsgItem&  remitComp)
{
    return strKey.eCompareI(remitComp.m_strName);
}


// ---------------------------------------------------------------------------
//  TResEdMsgItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdMsgItem::TResEdMsgItem() :

    m_c4Id(0)
{
}

TResEdMsgItem::TResEdMsgItem(const  tCIDLib::TCard4 c4Id
                            , const TString&        strName) :

    m_c4Id(c4Id)
    , m_strName(strName)
{
}

TResEdMsgItem::TResEdMsgItem(const  tCIDLib::TCard4 c4Id
                            , const TString&        strName
                            , const TString&        strMsg) :

    m_c4Id(c4Id)
    , m_strMsgText(strMsg)
    , m_strName(strName)
{
}

TResEdMsgItem::TResEdMsgItem(const TResEdMsgItem& remitToCopy) :

    m_c4Id(remitToCopy.m_c4Id)
    , m_strMsgText(remitToCopy.m_strMsgText)
    , m_strName(remitToCopy.m_strName)
{
}

TResEdMsgItem::~TResEdMsgItem()
{
}


// ---------------------------------------------------------------------------
//  TResEdMsgItem: Public operators
// ---------------------------------------------------------------------------
TResEdMsgItem& TResEdMsgItem::operator=(const TResEdMsgItem& remitToAssign)
{
    if (this != &remitToAssign)
    {
        m_c4Id       = remitToAssign.m_c4Id;
        m_strMsgText = remitToAssign.m_strMsgText;
        m_strName    = remitToAssign.m_strName;
    }
    return *this;
}


tCIDLib::TBoolean
TResEdMsgItem::operator==(const TResEdMsgItem& remitToComp) const
{
    return ((m_c4Id == remitToComp.m_c4Id)
           && (m_strMsgText == remitToComp.m_strMsgText)
           && (m_strName == remitToComp.m_strName));
}

tCIDLib::TBoolean
TResEdMsgItem::operator!=(const TResEdMsgItem& remitToComp) const
{
    return !operator==(remitToComp);
}


// ---------------------------------------------------------------------------
//  TResEdMsgItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the message item id
tCIDLib::TCard4 TResEdMsgItem::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TResEdMsgItem::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


// Get/set the message item name
const TString& TResEdMsgItem::strName() const
{
    return m_strName;
}

const TString& TResEdMsgItem::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


// Get/set the message item text
const TString& TResEdMsgItem::strMsgText() const
{
    return m_strMsgText;
}

const TString& TResEdMsgItem::strMsgText(const TString& strToSet)
{
    m_strMsgText = strToSet;
    return m_strMsgText;
}


// Set up all the values of this message item at once
tCIDLib::TVoid
TResEdMsgItem::Set( const   tCIDLib::TCard4 c4Id
                    , const TString&        strName
                    , const TString&        strMsg)
{
    m_c4Id = c4Id;
    m_strMsgText = strMsg;
    m_strName = strName;
}




// ---------------------------------------------------------------------------
//   CLASS: TResEdMsgList
//  PREFIX: remil
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdMsgList: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdMsgList::TResEdMsgList() :

    m_eType(tCIDResEd::EMsgTypes::Message)
{
    SetIdRange();
}

TResEdMsgList::TResEdMsgList(const tCIDResEd::EMsgTypes eType) :

    m_eType(eType)
{
    SetIdRange();
}

TResEdMsgList::TResEdMsgList(const TResEdMsgList& remilToCopy) :

    m_colList(remilToCopy.m_colList)
    , m_eType(remilToCopy.m_eType)
    , m_strNamespace(remilToCopy.m_strNamespace)
{
    SetIdRange();
}

TResEdMsgList::~TResEdMsgList()
{
}


// ---------------------------------------------------------------------------
//  TResEdMsgList: Public operators
// ---------------------------------------------------------------------------
TResEdMsgList& TResEdMsgList::operator=(const TResEdMsgList& remilSrc)
{
    if (this != &remilSrc)
    {
        m_colList       = remilSrc.m_colList;
        m_eType         = remilSrc.m_eType;
        m_strNamespace  = remilSrc.m_strNamespace;
    }
    return *this;
}


tCIDLib::TBoolean
TResEdMsgList::operator==(const TResEdMsgList& remilSrc) const
{
    return
    (
        (m_eType == remilSrc.m_eType)
        && (m_strNamespace == remilSrc.m_strNamespace)
        && (m_colList == remilSrc.m_colList)
    );
}

tCIDLib::TBoolean
TResEdMsgList::operator!=(const TResEdMsgList& remilSrc) const
{
    return !operator==(remilSrc);
}


// ---------------------------------------------------------------------------
//  TResEdMsgList: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add a new message item to the list
tCIDLib::TCard4
TResEdMsgList::c4AddNew(const   tCIDLib::TCard4 c4Id
                        , const TString&        strName
                        , const TString&        strMsgText)
{
    TResEdMsgItem remitNew(c4Id, strName, strMsgText);

    tCIDLib::TCard4 c4Ret;
    m_colList.InsertSorted(remitNew, TResEdMsgItem::eCompByName, c4Ret);
    return c4Ret;
}

tCIDLib::TCard4 TResEdMsgList::c4AddNew(const TResEdMsgItem& remitAdd)
{
    tCIDLib::TCard4 c4Ret;
    m_colList.InsertSorted(remitAdd, TResEdMsgItem::eCompByName, c4Ret);
    return c4Ret;
}


// Return the list of message items in this list
tCIDLib::TCard4 TResEdMsgList::c4Count() const
{
    return m_colList.c4ElemCount();
}


// Provide acces sto the min/max text id ranges for this list type
tCIDLib::TCard4 TResEdMsgList::c4MaxId() const
{
    return m_c4MaxId;
}

tCIDLib::TCard4 TResEdMsgList::c4MinId() const
{
    return m_c4MinId;
}


// Find a message by id and return its index within this list
tCIDLib::TCard4 TResEdMsgList::c4FindById(const tCIDLib::TCard4 c4Id) const
{
    //
    //  See if we have this id in our list. We have to do this one by just
    //  old fashioned complete search since the list is sorted by name
    //  not by number.
    //
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdMsgItem& remitCur = m_colList[c4Index];
        if (remitCur.c4Id() == c4Id)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


tCIDLib::TCard4 TResEdMsgList::c4FindByName(const TString& strName) const
{
    // For this one we can do a binary search
    tCIDLib::TCard4 c4Ret;
    if (m_colList.pobjKeyedBinarySearch(strName, TResEdMsgItem::eKeyComp, c4Ret))
        return c4Ret;
    return kCIDLib::c4MaxCard;
}


// Return an available message id that's not currently in use
tCIDLib::TCard4 TResEdMsgList::c4NextId() const
{
    // Get all the current ids
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    TFundVector<tCIDLib::TCard4> fcolIds(c4Count);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        fcolIds.c4AddElement(m_colList[c4Index].c4Id());

    // Sort the list
    fcolIds.Sort(tCIDLib::eComp<tCIDLib::TCard4>);

    //
    //  Now we just find the first missing value. We assign errors and
    //  messages to separate ranges.
    //
    tCIDLib::TCard4 c4Ret = m_c4MinId;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (fcolIds[c4Index] != c4Ret)
            break;
        c4Ret++;
    }

    // Make sure it's not beyond it's legal range
    CIDAssert(c4Ret <= m_c4MaxId, L"All available range ids are taken");
    return c4Ret;
}


//
//  Updates the message item at the indicated index. This may cause us to
//  re-sort the list if the symbolic name has changed. We give back the
//  index at which this changed item ended up.
//
tCIDLib::TCard4
TResEdMsgList::c4UpdateAt(  const   tCIDLib::TCard4 c4Index
                            , const TResEdMsgItem&  remitNew)
{
    // Get the current one
    TResEdMsgItem& remitCur = m_colList[c4Index];

    // Remember if we need to re-sort before we update, then update it
    const tCIDLib::TBoolean bResort = remitNew.strName() != remitCur.strName();

    //
    //  If not re-sorting, then we just update the existing one in place,
    //  and leave the current index as the return.
    //
    if (!bResort)
    {
        remitCur = remitNew;
        return c4Index;
    }

    //
    //  Oh well, we have to re-sort. So remove the current one from the
    //  list (WHICH INVALIDATES the remitCur we got above, so don't use it
    //  anymore.
    //
    m_colList.RemoveAt(c4Index);

    // And insert it back in at the new position and return that position
    tCIDLib::TCard4 c4NewInd;
    m_colList.InsertSorted(remitNew, TResEdMsgItem::eCompByName, c4NewInd);
    return c4NewInd;
}


// Removes the item at the indicated index
tCIDLib::TVoid TResEdMsgList::DeleteAt(const tCIDLib::TCard4 c4Index)
{
    m_colList.RemoveAt(c4Index);
}


//
//  Looks for an item in this list that has the indicated symbol name or id, in
//  a fault tolerant way that returns a pointer.
//
const TResEdMsgItem* TResEdMsgList::premitById(const tCIDLib::TMsgId midFind) const
{
    const tCIDLib::TCard4 c4Index = c4FindById(midFind);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;

    return &remitAt(c4Index);
}

const TResEdMsgItem* TResEdMsgList::premitByName(const TString& strName) const
{
    const tCIDLib::TCard4 c4Index = c4FindByName(strName);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;

    return &remitAt(c4Index);
}


//
//  This is called to parse a block of messages that are for our list.
//  We parse them in and add them to our list until we hit the end of the
//  block.
//
tCIDLib::TVoid
TResEdMsgList::ParseBlock(TResEdSpooler& spoolSrc, const TString& strEndText)
{
    tCIDLib::TCard4 c4Id;
    TResEdMsgItem   remitNew;
    TString         strInBuf;
    TString         strName;
    TString         strText;
    while(kCIDLib::True)
    {
        spoolSrc.bReadLine(strName);

        // If we hit the end of the block, we are done
        if (strName == strEndText)
            break;

        // Break out the three values
        spoolSrc.ParseNameValText(strName, c4Id, strText);

        // Make sure that the name isn't taken
        if (c4FindByName(strName) != kCIDLib::c4MaxCard)
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_DupName
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strName
                , TCardinal(spoolSrc.c4CurLine())
                , spoolSrc.strSrcFile()
            );
        }

        // Make sure that the id isn't taken
        if (c4FindById(c4Id) != kCIDLib::c4MaxCard)
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_DupId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4Id)
                , TCardinal(spoolSrc.c4CurLine())
                , spoolSrc.strSrcFile()
            );
        }

        //
        //  If the text ends with a slash, then it's continued and we have
        //  to read more lines until we get one that's not continued.
        //
        while (strText.chLast() == kCIDLib::chBackSlash)
        {
            // Remove the slash
            strText.DeleteLast();

            // Now read another line and append it, it's already stripped
            spoolSrc.bReadLine(strInBuf);
            strText.Append(strInBuf);
        }

        // It all looks good, so let's add it, sorted
        tCIDLib::TCard4 c4At;
        remitNew.Set(c4Id, strName, strText);
        m_colList.InsertSorted(remitNew, TResEdMsgItem::eCompByName, c4At);
    }
}


// Access a message item at a specific index
const TResEdMsgItem& TResEdMsgList::remitAt(const tCIDLib::TCard4 c4Index) const
{
    return m_colList[c4Index];
}


// Access a message item by its message id
const TResEdMsgItem& TResEdMsgList::remitById(const tCIDLib::TMsgId midFind) const
{
    const tCIDLib::TCard4 c4Index = c4FindById(midFind);
    if (c4Index == kCIDLib::c4MaxCard)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcMsg_IdNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(midFind)
        );
    }
    return remitAt(c4Index);
}


// Resets ourself back to default state
tCIDLib::TVoid TResEdMsgList::Reset()
{
    m_colList.RemoveAll();
    m_strNamespace.Clear();
}


// Get/set the namespace for this message list
const TString& TResEdMsgList::strNamespace() const
{
    return m_strNamespace;
}

const TString& TResEdMsgList::strNamespace(const TString& strToSet)
{
    m_strNamespace = strToSet;
    return m_strNamespace;
}


// Let's the outside world change the text for a given slot
tCIDLib::TVoid
TResEdMsgList::SetTextAt(const  tCIDLib::TCard4 c4At
                        , const TString&        strToSet)
{
    m_colList[c4At].strMsgText(strToSet);
}


//
//  This is called before a save is done. We sort the message items by
//  name and then renumber them sequentially. This keeps it easy to find
//  things. If nothing has changed since the last time, then it won't
//  get saved, so this doesn't cause unneeded rebuilds. If something got
//  renamed or added/removed, then the header would have to be rewritten
//  anyway, so it doesn't matter if we do some housekeeping. And it makes
//  a huge amount of difference in terms of finding messages during the
//  design process.
//
//  To make it easier, we try to group similar ids with some spare
//  ids between them. So we look for the last underscore in the name. If
//  the part up to there is the same as the last one, we use the next id.
//  Else, we assumea new group of ids and move forward to the next chunk
//  of 20 and start there.
//
//  Also, to catch scenarios like this:
//
//      midDlg_Base
//      midDlg_Base_Opt1
//      midDlg_Base_Opt2
//
//  If the new base is equal to the entire previous name, then we continue
//  it in the same chunk of ids.
//
tCIDLib::TVoid TResEdMsgList::SortAndNumber()
{
    // Sort the list
    m_colList.Sort(&TResEdMsgItem::eCompByName);

    //
    //  Now let's renumber them based on the current order. We have to set
    //  the correct base id.
    //
    const tCIDLib::TMsgId midBase = m_c4MinId;
    tCIDLib::TMsgId midCur = midBase;

    TString strCur(128UL);
    TString strCur2(128UL);
    TString strPrev(128UL);
    TString strNext(128UL);
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TResEdMsgItem& remitCur = m_colList[c4Index];

        //
        //  Look ahead to the next one, if we are not already at the end. If
        //  next one begins with the current one plus an undercore, then this
        //  current one is the start of a new group, so we move move forward to
        //  the next block of ids, else we just take the next one.
        //
        //  Also just take the next id if we are on the 0th entry
        //
        if (c4Index + 1 >= c4Count)
        {
            // We are on the last one, so just take the next id
            remitCur.c4Id(midCur++);
        }
         else if (!c4Index)
        {
            remitCur.c4Id(midCur++);

            //
            // Cap the current one after the last underscore and make it the initial
            //  previous value.
            //
            strPrev = remitCur.strName();
            tCIDLib::TCard4 c4SepAt;
            if (strPrev.bLastOccurrence(kCIDLib::chUnderscore, c4SepAt))
                strPrev.CapAt(c4SepAt + 1);
        }
         else
        {
            TResEdMsgItem& remitNext = m_colList[c4Index + 1];

            // Get a copy of the current name and the next one's name
            strCur = remitCur.strName();
            strCur2 = strCur;
            strNext = remitNext.strName();

            // Cap the next one after the last underscore
            tCIDLib::TCard4 c4SepAt;
            if (strNext.bLastOccurrence(kCIDLib::chUnderscore, c4SepAt))
                strNext.CapAt(c4SepAt + 1);

            // Add an underscope to the current one
            strCur.Append(kCIDLib::chUnderscore);

            // Do another version of the current, capped after the last underscore
            if (strCur2.bLastOccurrence(kCIDLib::chUnderscore, c4SepAt))
                strCur2.CapAt(c4SepAt + 1);

            //
            //  If they are the same or if the truncated version of this one and the
            //  previous one are not the same, then the current one is a new block,
            //  else take the next contiguous id.
            //
            if ((strCur == strNext) || (strCur2 != strPrev))
            {
                if (midCur != midBase)
                    midCur = ((midCur / 20) + 1) * 20;
                remitCur.c4Id(midCur++);
            }
             else
            {
                remitCur.c4Id(midCur++);
            }

            // Remember the truncated current one as the previous one
            strPrev = strCur2;
        }
    }
}


// We write our messages out to the passed output stream
tCIDLib::TVoid TResEdMsgList::WriteMsgs(TTextOutStream& strmTar) const
{
    // Reset the stream format back to default
    strmTar << TTextOutStream::strmfDefault();

    // Put out the block header
    if (m_eType == tCIDResEd::EMsgTypes::Error)
        strmTar << L"ERRORS=\n";
    else if (m_eType == tCIDResEd::EMsgTypes::Message)
        strmTar << L"MESSAGES=\n\n";

    // Make one pass to find the longest name value
    tCIDLib::TCard4 c4NameWidth = 5;
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdMsgItem& remitCur = m_colList[c4Index];
        if (remitCur.strName().c4Length() > c4NameWidth)
            c4NameWidth = remitCur.strName().c4Length();
    }

    // Set up some stream formatters for the three fields
    TStreamFmt  strmfName(c4NameWidth + 4, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
    TStreamFmt  strmfId(8, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
    TStreamFmt  strmfText(0, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    //
    //  Now put out all the messages. Note that if they are over a certain
    //  length we have to put them out in multiple lines with a trailing
    //  continuation character.
    //
    TString strChunk;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdMsgItem& remitCur = m_colList[c4Index];

        strmTar << TTextOutStream::Spaces(4)
                << strmfName << remitCur.strName()
                << strmfId << remitCur.c4Id();

        const TString& strMsg = remitCur.strMsgText();
        const tCIDLib::TCard4 c4Len = strMsg.c4Length();
        if (c4Len > 255)
        {
            //
            //  We have to break it into chunks. We'll break it out on
            //  space boundaries, trying to keeping the max width below
            //  128 chars.
            //
            tCIDLib::TCard4 c4ChunkCnt = 0;
            tCIDLib::TCard4 c4CurInd = 0;
            while(c4CurInd < c4Len)
            {
                //
                //  Load a chunk's worth, starting to look for a break
                //  at 180. No word will be over 48 characters long,
                //  so we have to see one before 128.
                //
                strChunk.Clear();
                tCIDLib::TCard4 c4ThisChunk = 0;
                while ((c4ThisChunk < 250) && (c4CurInd < c4Len))
                {
                    const tCIDLib::TCh chCur = strMsg[c4CurInd++];
                    strChunk.Append(chCur);

                    //
                    //  Bump the number of chars in this chunk so far. If
                    //  we hit 180, start looking for a break. Note that
                    //  these are often followed by a space, and we want
                    //  to get that space at the end, so it doesn't get lost
                    //  as part of the leading space when the text is parsed
                    //  back in. So we look ahead to see if the next one is
                    //  a space take that also if so.
                    //
                    c4ThisChunk++;
                    if ((c4ThisChunk >= 180)
                    &&  ((chCur == kCIDLib::chSpace)
                    ||   (chCur == kCIDLib::chComma)
                    ||   (chCur == kCIDLib::chHyphenMinus)
                    ||   (chCur == kCIDLib::chPeriod)))
                    {
                        if ((c4CurInd < c4Len)
                        &&  (strMsg[c4CurInd] == kCIDLib::chSpace))
                        {
                            c4CurInd++;
                            strChunk.Append(kCIDLib::chSpace);
                        }
                        break;
                    }
                }

                // If not the first chunk, space it over
                if (c4ChunkCnt)
                    strmTar << TTextOutStream::Spaces(c4NameWidth + 16);

                // Put out this chunk
                strmTar << strmfText << strChunk;

                // If not done yet, output a continuation
                if (c4CurInd < c4Len)
                    strmTar << L'\\';

                // Bump the chunk count again
                c4ChunkCnt++;

                // And now a new line either way
                strmTar << kCIDLib::NewLn;
            }
        }
         else
        {
            // Nothing overly long, do it as a single line
            strmTar << strmfText << strMsg << kCIDLib::NewLn;
        }
    }

    // Reset the stream format back to default
    strmTar << TTextOutStream::strmfDefault();

    // Close off this block
    if (m_eType == tCIDResEd::EMsgTypes::Error)
        strmTar << L"END ERRORS\n\n";
    else if (m_eType == tCIDResEd::EMsgTypes::Message)
        strmTar << L"END MESSAGES\n\n";
}



// ---------------------------------------------------------------------------
//  TResEdMsgList: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Based on msg list type, set the valid range
tCIDLib::TVoid TResEdMsgList::SetIdRange()
{
    if (m_eType == tCIDResEd::EMsgTypes::Common)
    {
        m_c4MinId = kCIDLib::c4ComId_First;
        m_c4MaxId = kCIDLib::c4ComId_Last;
    }
     else if (m_eType == tCIDResEd::EMsgTypes::Error)
    {
        m_c4MinId = kCIDLib::c4ErrId_First;
        m_c4MaxId = kCIDLib::c4ErrId_Last;
    }
     else if (m_eType == tCIDResEd::EMsgTypes::Message)
    {
        m_c4MinId = kCIDLib::c4MsgId_First;
        m_c4MaxId = kCIDLib::c4MsgId_Last;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMCListMsgOps
// PREFIX: mclops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMCListMsgOps: Constructors and destructors
// ---------------------------------------------------------------------------
TMCListMsgOps::TMCListMsgOps(const TResEdMsgList* const premilMsgs) :

    m_premilMsgs(premilMsgs)
    , m_premitSingle(nullptr)
{
}

TMCListMsgOps::TMCListMsgOps(const TResEdMsgItem* const premitSingle) :

    m_premilMsgs(nullptr)
    , m_premitSingle(premitSingle)
{
}

TMCListMsgOps::~TMCListMsgOps()
{
}


// ---------------------------------------------------------------------------
//  TMCListMsgOps: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  The colums are:
//
//      1. The symbol name
//      2. The msg id
//      3. The text
//
tCIDLib::TCard4 TMCListMsgOps::c4ColCount() const
{
    return 3;
}


tCIDLib::TCard4 TMCListMsgOps::c4RowCount() const
{
    // If in single item mode, we only have 1
    if (m_premitSingle)
        return 1;

    // Else, however many are in the list
    return m_premilMsgs->c4Count();
}


tCIDLib::TCard4 TMCListMsgOps::c4RowId(const tCIDLib::TCard4 c4RowIndex) const
{
    if (m_premitSingle)
    {
        CIDAssert(c4RowIndex == 0, L"MCListMsgOps row index must be 0 for single mode");
        return m_premitSingle->c4Id();
    }
    return m_premilMsgs->remitAt(c4RowIndex).c4Id();
}


const TString&
TMCListMsgOps::strColumnText(const  tCIDLib::TCard4 c4RowIndex
                            , const tCIDLib::TCard4 c4ColIndex) const
{
    // Get a pointer to teh single item or the list entry
    const TResEdMsgItem* premitRow = 0;
    if (m_premitSingle)
    {
        CIDAssert(c4RowIndex == 0, L"MCListDlgOps index must be 0 for single mode");
        premitRow = m_premitSingle;
    }
     else
    {
        premitRow = &m_premilMsgs->remitAt(c4RowIndex);
    }

    // The id we have to format into a temp string
    if (c4ColIndex == 0)
    {
        return premitRow->strName();
    }
     else if (c4ColIndex == 1)
    {
        m_strTmpFmt.SetFormatted(premitRow->c4Id());
        return m_strTmpFmt;
    }
    return premitRow->strMsgText();
}

