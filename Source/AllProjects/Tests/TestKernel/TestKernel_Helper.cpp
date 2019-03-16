//
// FILE NAME: TestKernel_Helper.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module implements some simple helper stuff that we just want to
//  segregate out from the actual testing stuff.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"



// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
TOutStrm& operator<<(TOutStrm& strmOut, const TCurLn& clnToStream)
{
    strmOut << L"(";
    strmOut << clnToStream.m_pszFile;
    strmOut << L"." << clnToStream.m_c4Line << L" ";
    return strmOut;
}


TOutStrm& operator<<(TOutStrm& strmOut, const TKrnlError& kerrToStream)
{
    strmOut << L"Error: " << kerrToStream.errcId();
    if (kerrToStream.errcHostId())
        strmOut << L", Host Error: " << kerrToStream.errcHostId();
    return strmOut;
}


// ----------------------------------------------------------------------------
//   CLASS: TCurLn
//  PREFIX: cln
// ----------------------------------------------------------------------------

//
//  This guy will make a copy of the root part of the file name, in an
//  allocated buffer. The line is just stored away. The destructor cleans
//  up the buffer.
//
TCurLn::TCurLn( const   tCIDLib::TCh* const pszFile
                , const tCIDLib::TCard4     c4Line) :
    m_c4Line(c4Line)
    , m_pszFile(0)
{
    // Sanity check
    if (!pszFile || !*pszFile)
    {
        m_pszFile = new tCIDLib::TCh[1];
        *m_pszFile = 0;
    }
     else
    {
        //
        //  Find the root part of the file. We go to the end of the string
        //  and work back to the first slash.
        //
        const tCIDLib::TCh* pszCur = &pszFile[TRawStr::c4StrLen(pszFile)];
        while ((*pszCur != L'\\') && (pszCur > pszFile))
            pszCur--;

        if (*pszCur == L'\\')
            pszCur++;

        m_pszFile = TRawStr::pszReplicate(pszCur);
    }
}

TCurLn::~TCurLn()
{
    if (m_pszFile)
    {
        delete [] m_pszFile;
        m_pszFile = 0;
    }
}

