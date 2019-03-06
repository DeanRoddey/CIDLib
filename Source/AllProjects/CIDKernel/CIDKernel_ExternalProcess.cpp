//
// FILE NAME: CIDKernel_ExternalProcess.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the platform independent implementation of the
//  TKrnlExtProcess class, which provides host abstraction services for
//  execution of external programs. Most of it is platform dependent, so there
//  isn't a lot of code here.
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
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TKrnlExtProcess::c4BreakOutParms(const  tCIDLib::TCh* const pszCmdLine
                                ,       tCIDLib::TCh*       apszList[]
                                , const tCIDLib::TCard4     c4MaxParms)
{
    // Init the count return
    tCIDLib::TCard4 c4RetVal = 0;

    //
    //  Allocate a temp buffer to copy each parm into. Make it the same size
    //  as the whole cmd line, which is the biggest it could ever need to
    //  be in the case where there is just one big parm.
    //
    tCIDLib::TCh* pszVal = new tCIDLib::TCh[TRawStr::c4StrLen(pszCmdLine) + 1];
    TArrayJanitor<tCIDLib::TCh> janVal(pszVal);

    tCIDLib::TCard4 c4Count;

    // While we are not at the terminating nul, keep going
    const tCIDLib::TCh* pszCmd = pszCmdLine;
    while (*pszCmd)
    {
        // Move up to the next non-whitespace character
        while (*pszCmd && TRawStr::bIsSpace(*pszCmd))
            pszCmd++;

        // If we hit the end, we are done
        if (!*pszCmd)
            break;

        //
        //  It will either be a quote character or not. If it is, then we
        //  have to find the terminating quote. if its not, then we just
        //  have to search up to a whitespace again.
        //
        c4Count = 0;
        if ((*pszCmd == kCIDLib::chQuotation)
        ||  (*pszCmd == kCIDLib::chApostrophe))
        {
            const tCIDLib::TCh chQuote = *pszCmd++;

            while (*pszCmd)
            {
                //
                //  If its our quote char, and its followed by a space or
                //  the end of input, its the end of this param.
                //
                if (*pszCmd == chQuote)
                {
                    if (TRawStr::bIsSpace(*(pszCmd+1)) || !*(pszCmd+1))
                        break;
                }

                // Its nothing special, so store it
                pszVal[c4Count++] = *pszCmd++;
            }

            // Cap off the stuff we got this time
            pszVal[c4Count] = 0;

            //
            //  If we break out because of the end of input, instead of the
            //  terminating quote, then throw away any trailing spaces.
            //
            if (!*pszCmd)
            {
                TRawStr::StripStr
                (
                    pszVal
                    , L" \t\n\r"
                    , tCIDLib::EStripModes::Trailing
                );
            }
             else
            {
                // Not at the end, so move up for the next round
                pszCmd++;
            }
        }
         else
        {
            //
            //  Loop until we hit the end of input or a whitespace. Note
            //  that we don't have to watch for a quote here. If the only
            //  thing between us and the next quote was white space we would
            //  have taken the first loop above since the initial scan for
            //  a non-whitespace would have run up to the quote.
            //
            //  If there was a quote without any whitespace, then we don't
            //  treat it as a special quote.
            //
            while (*pszCmd)
            {
                // Break out if we hit a space
                if (TRawStr::bIsSpace(*pszCmd))
                    break;

                pszVal[c4Count++] = *pszCmd++;
            }

            //
            //  If we didn't get anything, then we are done, since we, had to
            //  have hit the end of input without seeing any non-whitspace. Only
            //  a quoted parameter can be empty.
            //
            if (!c4Count)
                break;

            // Cap off the stuff we got this time
            pszVal[c4Count] = 0;
        }

        // And add it into the list
        apszList[c4RetVal++] = TRawStr::pszReplicate(pszVal);

        // Make sure we don't overflow
        if (c4RetVal >= c4MaxParms)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);

            // Free all the strings and return a failure
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RetVal; c4Index++)
                delete [] apszList[c4Index];
            return 0;
        }
    }
    return c4RetVal;
}


// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszPath
                        , const tCIDLib::TCh* const     pszInitPath
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    return bStart(pszPath, pszInitPath, 0, 0, 0, 0, eFlag, eShow);
}


const TProcessHandle& TKrnlExtProcess::hprocThis() const
{
    return m_hprocThis;
}


