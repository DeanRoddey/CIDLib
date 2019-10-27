//
// FILE NAME: CIDKernel_Console_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 03/12/1999
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
//  This file provides the Linux specific implementations for the console
//  input and output classes, TKrnlConIn and TKrnlConOut.
//
//  We also implement the TKrnlConsoleHandle class here, which provides our
//  implementation of a console handle.
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
#include    "CIDKernel_InternalHelpers_.hpp"

#include    <editline/readline.h>
#include    <editline/history.h>


// We don't need this because everything is in TConsoleHandleImpl.
struct TKrnlConIn::TConPlatInfo
{
};

namespace
{
    const tCIDLib::TCard4 c4TermFifoSize = 256;

    // ---------------------------------------------------------------------------
    //  Local data types
    //
    //  TTermInfoKey
    //      This is the structure used to translate CID console key values into
    //      terminfo capability names for looking them up.
    //
    //      pszTermInfo
    //          The terminfo capability name.
    //      eKey
    //          The CID key that goes with the capability.
    // ---------------------------------------------------------------------------
    struct TTermInfoKey
    {
        const tCIDLib::TSCh*    pszTermInfo;
        tCIDLib::EConKeys       eKey;
    };


    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    static tCIDLib::TBoolean
    bLookupKey(const    TKrnlLinux::TTermValueNode* pnodeTree
               , const tCIDLib::EConKeys            eKey
               ,        tCIDLib::TBoolean&          bFound
               ,        tCIDLib::TSCh*              pszTermValue
               ,        tCIDLib::TCard4             c4MaxChars)
    {
        const TKrnlLinux::TTermValueNode* pnodeCur = pnodeTree;

        while (pnodeCur)
        {
            if (pszTermValue && !(c4MaxChars - 1))
            {
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
                return kCIDLib::False;
            }

            tCIDLib::TSCh* pszActual = pszTermValue ? pszTermValue + 1 : 0;

            if (!bLookupKey(pnodeCur->pnodeChild, eKey, bFound, pszActual, c4MaxChars - 1))
                return kCIDLib::False;

            if (bFound)
                break;

            if (pnodeCur->c2Code == tCIDLib::TCard2(eKey))
            {
                bFound = kCIDLib::True;
                *(pszTermValue + 1) = '\000';
                break;
            }

            pnodeCur = pnodeCur->pnodeSibling;
        }

        if (bFound && pszTermValue)
        {
            *pszTermValue = pnodeCur->chThis ? pnodeCur->chThis : 128;
        }

        return kCIDLib::True;
    }

    static tCIDLib::TVoid
    AddTermValueNode(       TKrnlLinux::TTermValueNode*&    pnodeTree
                    , const tCIDLib::TSCh* const            pszValue
                    , const tCIDLib::EConKeys               eKey)
    {
        tCIDLib::TSCh* pszLocValue = const_cast<tCIDLib::TSCh*>(pszValue);
        tCIDLib::TSCh chCur;
        TKrnlLinux::TTermValueNode* pnodeCur;

        if (!pszLocValue || *pszLocValue == '\000')
            return;

        if (!pnodeTree)
        {
            pnodeCur = pnodeTree = new TKrnlLinux::TTermValueNode;
            pnodeCur->chThis = *pszLocValue++;
            /*
            if (pnodeCur->chThis == 128)
                pnodeCur->chThis = '\000';
            */
        }
        else
        {
            pnodeCur = pnodeTree;

            for (;;)
            {
                chCur = *pszValue;
                while (pnodeCur->chThis != chCur
                &&     pnodeCur->pnodeSibling)
                {
                    pnodeCur = pnodeCur->pnodeSibling;
                }

                if (pnodeCur->chThis == chCur)
                {
                    if (*(++pszLocValue) == '\000')
                    {
                        pnodeCur->c2Code = tCIDLib::TCard2(eKey);
                        return;
                    }

                    if (pnodeCur->pnodeChild)
                        pnodeCur = pnodeCur->pnodeChild;
                    else
                        break;
                }
                else
                {
                    pnodeCur->pnodeSibling = new TKrnlLinux::TTermValueNode;
                    pnodeCur = pnodeCur->pnodeSibling;

                    pnodeCur->chThis = *pszLocValue++;
                    /*
                    if (pnodeCur->chThis == 128)
                        pnodeCur->chThis = '\000';
                    */

                    break;
                }
            }
        }

        while (*pszLocValue)
        {
            pnodeCur->pnodeChild = new TKrnlLinux::TTermValueNode;
            pnodeCur = pnodeCur->pnodeChild;
            pnodeCur->chThis = *pszLocValue++;
            /*
            if (pnodeCur->chThis == 128)
                pnodeCur->chThis = '\000';
            */
        }

        pnodeCur->c2Code = tCIDLib::TCard2(eKey);
    }

    static tCIDLib::TVoid
    BuildTermValueTree(TKrnlLinux::TTermValueNode*& pnodeTree)
    {
        // This maps the terminfo capability identifiers
        // to the CIDLib console keys values, so we can
        // look them up in the terminfo database.
        static TTermInfoKey __atikXlat[] =

        {
                { "kbs"     , tCIDLib::EConKeys::Backspace  }
            ,   { "kcbt"    , tCIDLib::EConKeys::BackTab    }
            ,   { "kdch1"   , tCIDLib::EConKeys::Delete     }
            ,   { "kent"    , tCIDLib::EConKeys::Enter      }
            ,   { "kich1"   , tCIDLib::EConKeys::Insert     }
            ,   { "kcuu1"   , tCIDLib::EConKeys::Up         }
            ,   { "kcud1"   , tCIDLib::EConKeys::Down       }
            ,   { "kcub1"   , tCIDLib::EConKeys::Left       }
            ,   { "kcuf1"   , tCIDLib::EConKeys::Right      }
            ,   { "kprv"    , tCIDLib::EConKeys::Previous   }
            ,   { "knxt"    , tCIDLib::EConKeys::Next       }
            ,   { "khome"   , tCIDLib::EConKeys::Home       }
            ,   { "kend"    , tCIDLib::EConKeys::End        }
        };

        // This is a backup set to support weird mappings on
        // weird terminals.
        static TTermInfoKey __atikXlat2[] =
        {
                { "kfnd"   , tCIDLib::EConKeys::Home       }
            ,   { "kslt"   , tCIDLib::EConKeys::End        }
        };

        // Start with the no-brainer.
        AddTermValueNode(pnodeTree, "\t", tCIDLib::EConKeys::Tab);

        // Now do the initial round of lookups
        tCIDLib::TCard4 c4Idx;
        for (c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(__atikXlat); c4Idx++)
        {
            AddTermValueNode
            (
                pnodeTree
                , ::tigetstr(__atikXlat[c4Idx].pszTermInfo)
                , __atikXlat[c4Idx].eKey
            );
        }

        tCIDLib::TBoolean bGotIt;

        // Now check for failures and do the backup round.
        for (c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(__atikXlat2); c4Idx++)
        {
            if (bLookupKey( pnodeTree
                             , __atikXlat2[c4Idx].eKey
                             , bGotIt
                             , 0
                             , 0))
            {
                if (!bGotIt)
                {
                    AddTermValueNode
                    (
                        pnodeTree
                        , ::tigetstr(__atikXlat2[c4Idx].pszTermInfo)
                        , __atikXlat2[c4Idx].eKey
                    );
                }
            }
        }

        // Now we make Previous and Next to be meta-Left and
        // meta-Right, if they weren't originally found in the
        // terminfo database.
        tCIDLib::TSCh szValue[32];
        szValue[0] = '\033';
        tCIDLib::TSCh* pszFillHere = &szValue[1];
        if (bLookupKey( pnodeTree
                         , tCIDLib::EConKeys::Previous
                         , bGotIt
                         , 0
                         , 0))
        {
            if (!bGotIt)
            {
                if (bLookupKey( pnodeTree
                                , tCIDLib::EConKeys::Left
                                , bGotIt
                                , pszFillHere
                                , 31))
                {
                    if (bGotIt)
                        AddTermValueNode(pnodeTree, szValue, tCIDLib::EConKeys::Previous);
                }
            }
        }
        if (bLookupKey( pnodeTree
                        , tCIDLib::EConKeys::Next
                        , bGotIt
                        , 0
                        , 0))
        {
            if (!bGotIt)
            {
                if (bLookupKey( pnodeTree
                                , tCIDLib::EConKeys::Right
                                , bGotIt
                                , pszFillHere
                                , 31))
                {
                    if (bGotIt)
                        AddTermValueNode(pnodeTree, szValue, tCIDLib::EConKeys::Next);
                }
            }
        }

        // If Enter wasn't in the database, just make it "\n".
        if (bLookupKey(pnodeTree, tCIDLib::EConKeys::Enter, bGotIt, 0, 0))
        {
            if (!bGotIt)
                AddTermValueNode(pnodeTree, "\n", tCIDLib::EConKeys::Enter);
        }

        // If Backspace wasn't in the database, just make it "\b".
        if (bLookupKey(pnodeTree, tCIDLib::EConKeys::Backspace, bGotIt, 0, 0))
        {
            if (!bGotIt)
                AddTermValueNode(pnodeTree, "\b", tCIDLib::EConKeys::Backspace);
        }
    }

    static tCIDLib::TBoolean bWaitForInput(const tCIDLib::TEncodedTime enctEnd)
    {
        const tCIDLib::TEncodedTime enctCur = TKrnlTimeStamp::enctNow();
        if (enctCur >= enctEnd)
            return kCIDLib::False;

        // Set up a time value with the remaining time
        const tCIDLib::TEncodedTime enctDiff(enctEnd - enctCur);
        tCIDLib::TInt4 i4Sec = tCIDLib::TInt4(enctDiff / kCIDLib::enctOneSecond);
        tCIDLib::TInt4 i4Microsec = tCIDLib::TInt4
        (
            (enctDiff - (i4Sec * kCIDLib::enctOneSecond)) / kCIDLib::enctOneMilliSec
        );

        struct timeval WaitTimeVal;
        WaitTimeVal.tv_sec = i4Sec;
        WaitTimeVal.tv_usec = i4Microsec;

        fd_set ReadSet;
        FD_ZERO(&ReadSet);
        FD_SET(0, &ReadSet);


        const tCIDLib::TSInt iReturn = ::select(1, &ReadSet, 0, 0, &WaitTimeVal);
        return (iReturn != 0 && iReturn != -1);
    }

    static tCIDLib::TCard4
    c4WriteString(const tCIDLib::TCh* const pszToWrite, tCIDLib::TCard4& c4BytesWritten)
    {
        const tCIDLib::TCard4 c4BufSize = 1023;
        tCIDLib::TSCh szTmp[c4BufSize + 1];

        tCIDLib::TCard4 c4SrcIndex = 0;
        tCIDLib::TSInt iWritten;
        tCIDLib::TSInt iTotalWritten = 0;
        tCIDLib::TCard4 c4ActualToWrite = TRawStr::c4StrLen(pszToWrite);

        while (c4SrcIndex < c4ActualToWrite)
        {
            tCIDLib::TSInt iOutIndex = 0;

            while (c4SrcIndex < c4ActualToWrite)
            {
                if (iOutIndex + MB_LEN_MAX >= c4BufSize)
                    break;
                tCIDLib::TSInt iCurBytes = ::wctomb(&szTmp[iOutIndex]
                                                    , pszToWrite[c4SrcIndex++]);
                if (iCurBytes == -1)
                {
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
                    return kCIDLib::False;
                }
                iOutIndex += iCurBytes;
            }
            if (iOutIndex)
            {
                iWritten = ::write(1, szTmp, iOutIndex);
                if (iWritten == -1)
                {
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }
                iOutIndex = 0;
                iTotalWritten += iWritten;
            }
        }

        c4BytesWritten = tCIDLib::TCard4(iTotalWritten);
        return c4SrcIndex;
    }

    static tCIDLib::TBoolean bInitTerminalInfo()
    {
        static tCIDLib::TBoolean bInitialized = kCIDLib::False;
        if (!bInitialized)
        {
            tCIDLib::TSInt iError;
            if (::setupterm(0, 1, &iError) == ERR)
            {
                tCIDLib::TErrCode errcThis = kKrnlErrs::errcGen_GenFailure;
                if (iError == 0)
                    errcThis = kKrnlErrs::errcDev_NoSuchDevice;
                else if (iError == -1)
                    errcThis = kKrnlErrs::errcFl_FileNotFound;
                TKrnlError::SetLastKrnlError(errcThis);
                return kCIDLib::False;
            }
            bInitialized = kCIDLib::True;
        }
        return kCIDLib::True;
    }

    static tCIDLib::TBoolean
    bSetSingleCharMode(TConsoleHandleImpl* phconiToSet, tCIDLib::TBoolean bNewState)
    {
        if (bNewState)
        {
            if (phconiToSet->pfifoThis == 0)
            {
                // Save the terminal settings, so we can restore them later.
                if (::tcgetattr(0, &phconiToSet->TermInfo))
                {
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }

                // Now read the terminfo database, so we can know
                // how to interpret key presses.
                if (!bInitTerminalInfo())
                    return kCIDLib::False;

                // Build up a tree. This will maximize efficiency when
                // reading characters from the terminal.
                phconiToSet->pnodeTree = 0;
                BuildTermValueTree(phconiToSet->pnodeTree);

                // Now create the FIFO that we'll use to buffer input.
                phconiToSet->pfifoThis = new TKrnlLinux::TTermFifo(c4TermFifoSize);
            }

            // Set the terminal to read one character at a time with no echo.
            struct termios TermInfoNew;
            TRawMem::CopyMemBuf(&TermInfoNew, &phconiToSet->TermInfo, sizeof(TermInfoNew));
            TermInfoNew.c_cc[VMIN] = 1;
            TermInfoNew.c_cc[VTIME] = 0;
            TermInfoNew.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHOKE);

            if (::tcsetattr(0, TCSAFLUSH, &TermInfoNew))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            // Restore the terminal state to what it was before we messed with it.
            if (::tcsetattr(0, TCSAFLUSH, &phconiToSet->TermInfo))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }

        phconiToSet->bSingleCharMode = bNewState;

        return kCIDLib::True;
    }
}



// ---------------------------------------------------------------------------
//  Initialization methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermConsole(const tCIDLib::EInitTerm eState)
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//   CLASS: TConsoleHandle
//  PREFIX: hcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TConsoleHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TConsoleHandle::TConsoleHandle() :

    m_phconiThis(0)
{
    m_phconiThis = new TConsoleHandleImpl;
    m_phconiThis->pfifoThis = 0;
    m_phconiThis->pnodeTree = 0;
    m_phconiThis->bValid = kCIDLib::False;
    m_phconiThis->bSingleCharMode = kCIDLib::False;
}

TConsoleHandle::TConsoleHandle(const TConsoleHandle& hconToCopy) :

    m_phconiThis(0)
{
    m_phconiThis = new TConsoleHandleImpl;
    // Shallow copy on purpose. The object that owns this can't be copied anyway.
    TRawMem::CopyMemBuf(m_phconiThis, hconToCopy.m_phconiThis, sizeof(*m_phconiThis));
}

TConsoleHandle::~TConsoleHandle()
{
    delete m_phconiThis->pfifoThis;
    delete m_phconiThis->pnodeTree;
    delete m_phconiThis;
    m_phconiThis = 0;
}


// ---------------------------------------------------------------------------
//  Public operators
// ---------------------------------------------------------------------------
TConsoleHandle& TConsoleHandle::operator=(const TConsoleHandle& hconToAssign)
{
    if (this != &hconToAssign)
    {
        TRawMem::CopyMemBuf(m_phconiThis, hconToAssign.m_phconiThis, sizeof(*m_phconiThis));
    }
    return *this;
}


tCIDLib::TBoolean
TConsoleHandle::operator==(const TConsoleHandle& hconToCompare) const
{
    return TRawMem::eCompareMemBuf(m_phconiThis, hconToCompare.m_phconiThis, sizeof(*m_phconiThis))
        == tCIDLib::ESortComps::Equal;
}



// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TConsoleHandle::bIsValid() const
{
    return m_phconiThis->bValid;
}


tCIDLib::TVoid TConsoleHandle::Clear()
{
    m_phconiThis->bValid = kCIDLib::False;
}


tCIDLib::TVoid
TConsoleHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    const tCIDLib::TCh* pszValue = L"False";
    if (m_phconiThis->bValid)
        pszValue = L"True";
    TRawStr::CopyStr(pszToFill, pszValue, c4MaxChars);
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlConIn
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConIn: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlConIn::bReadChar(          tCIDLib::EConKeys&      keyType
                        ,       tCIDLib::TCh&           chGotten
                        , const tCIDLib::TEncodedTime   enctEnd
                        ,       TIdleCB                 pfnCallback
                        ,       TObject* const          pobjCBData)
{
    if (!m_hconThis.m_phconiThis->bSingleCharMode)
    {
        if (!bSetSingleCharMode(m_hconThis.m_phconiThis, kCIDLib::True))
            return kCIDLib::False;
    }

    tCIDLib::TSCh chTmp;
    TKrnlLinux::TTermValueNode* pnodeCur = m_hconThis.m_phconiThis->pnodeTree;
    TKrnlLinux::TTermFifo* pFifo = m_hconThis.m_phconiThis->pfifoThis;

    while (kCIDLib::True)
    {
        if (pFifo->bIsEmpty())
        {
            if (!enctEnd || !pFifo->bPush())
                return kCIDLib::False;
        }

        chTmp = pFifo->chPeek();

        while (pnodeCur && pnodeCur->chThis != chTmp)
            pnodeCur = pnodeCur->pnodeSibling;

        if (!pnodeCur)
        {
            keyType = tCIDLib::EConKeys::Char;
            break;
        }

        if (pnodeCur->c2Code)
        {
            keyType = tCIDLib::EConKeys(pnodeCur->c2Code);
            pFifo->ClearPeeked();
            ::mbtowc(&chGotten, &chTmp, 1);
            return kCIDLib::True;
        }

        pnodeCur = pnodeCur->pnodeChild;

        if (pFifo->bIsEmpty())
        {
            if (!bWaitForInput(enctEnd))
                return kCIDLib::False;
        }
    }

    chTmp = pFifo->chPull();
    pFifo->ResetPeek();

    // This one can't be put in the tree because ESC is
    // the start of a lot of the sequences in the tree.
    // <TBD> Make the tree smarter.
    if (chTmp == '\033')
        keyType = tCIDLib::EConKeys::Escape;

    ::mbtowc(&chGotten, &chTmp, 1);

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlConIn: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlConIn::bReadLine(          tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4CharsToRead
                        ,       tCIDLib::TCard4&    c4CharsRead
                        ,       TIdleCB             pfnCallback
                        ,       TObject* const      pobjCBData)
{
    if (m_hconThis.m_phconiThis->bSingleCharMode)
    {
        if (!bSetSingleCharMode(m_hconThis.m_phconiThis, kCIDLib::False))
            return kCIDLib::False;
    }

    const tCIDLib::TSCh* pszLineRead = ::readline("");
    if (!pszLineRead)
        pszLineRead = "";
    c4CharsRead = ::strlen(pszLineRead);
    if (c4CharsRead)
        ::add_history(pszLineRead);
    TRawStr::pszConvert(pszLineRead, pszToFill, c4CharsToRead);
    if (c4CharsRead < c4CharsToRead)
    {
        pszToFill[c4CharsRead++] = kCIDLib::chLF;
        if (c4CharsRead < c4CharsToRead)
            pszToFill[c4CharsRead] = kCIDLib::chNull;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlConIn::bOpen()
{
    m_hconThis.m_phconiThis->bValid = kCIDLib::True;
    m_hconThis.m_phconiThis->bSingleCharMode = kCIDLib::False;
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlConIn::bSetInsertMode(const tCIDLib::TBoolean)
{
    return kCIDLib::True;
}

tCIDLib::TVoid TKrnlConIn::Close()
{
    if (m_hconThis.m_phconiThis->bSingleCharMode)
        ::tcsetattr(0, TCSADRAIN, &m_hconThis.m_phconiThis->TermInfo);
}

TKrnlConIn::TConPlatInfo*
TKrnlConIn::pInitPlatform(const tCIDLib::TCard4 c4MaxRecall)
{
    // Set up the history buffer according to instructions
    if (c4MaxRecall > 0)
        ::stifle_history(tCIDLib::MinVal(c4MaxRecall, tCIDLib::TCard4(kCIDLib::i4MaxInt)));

    // Make a new platform structure
    return 0;
}

tCIDLib::TVoid TKrnlConIn::ResetRecallBuf()
{
    ::clear_history();
}

tCIDLib::TVoid TKrnlConIn::TermPlatform()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlConOut
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConOut: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConOut::bClearScr()
{
    tCIDLib::TSCh* pszClear = ::tigetstr("clear");
    if (!pszClear)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }
    ::tputs(pszClear, 1, putchar);
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlConOut::bPutChar(const tCIDLib::TCh chToWrite)
{
    tCIDLib::TSCh szMultiByte[MB_LEN_MAX];
    tCIDLib::TSInt iToWrite = ::wctomb(szMultiByte, chToWrite);
    if (iToWrite == -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidData);
        return kCIDLib::False;
    }

    tCIDLib::TSInt iWritten = ::write(1, szMultiByte, iToWrite);
    if (iWritten != iToWrite)
    {
        if (iWritten == -1)
            TKrnlError::SetLastHostError(errno);
        else
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_NotAllData);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlConOut::bQueryVerticalLines(tCIDLib::TCard4& c4ToFill) const
{
    tCIDLib::TSInt iLines = ::tigetnum("lines");
    if (iLines == -2)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }
    c4ToFill = iLines;
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlConOut::bReset()
{
    return bClearScr();
}

tCIDLib::TBoolean
TKrnlConOut::bWriteBytes(const  tCIDLib::TVoid* const   pToWrite
                        , const tCIDLib::TCard4         c4ToWrite
                        ,       tCIDLib::TCard4&        c4Written)
{
    // If nothing to write, then do nothing
    if (!c4ToWrite)
        return kCIDLib::True;

    // Look at the buffer as a Unicode string
    tCIDLib::TCh* pszToWrite
                        = TRawStr::pszReplicate(reinterpret_cast<const tCIDLib::TCh*>(pToWrite));
    TArrayJanitor<tCIDLib::TCh> janToWrite(pszToWrite);

    tCIDLib::TCard4 c4CharsToWrite = c4ToWrite / kCIDLib::c4CharBytes;
    if (c4CharsToWrite < TRawStr::c4StrLen(pszToWrite))
        pszToWrite[c4CharsToWrite] = '\000';

    tCIDLib::TCard4 c4BytesWritten;
    c4Written = c4WriteString(pszToWrite, c4BytesWritten);

    c4Written *= kCIDLib::c4CharBytes;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlConOut::bWriteLine(const tCIDLib::TCh* const pszToWrite)
{
    tCIDLib::TCard4 c4BytesWritten;
    c4WriteString(pszToWrite, c4BytesWritten);
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlConOut: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConOut::bOpen()
{
    return bInitTerminalInfo();
}

tCIDLib::TVoid TKrnlConOut::Close()
{
}

TKrnlConOut::TConPlatInfo* TKrnlConOut::pInitPlatform()
{
    // We don't use this so we just return a null pointer
    return 0;
}


tCIDLib::TVoid TKrnlConOut::TermPlatform()
{
    // No-op for us
}
