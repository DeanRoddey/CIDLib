//
// FILE NAME: TestKernel_Helper.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the header for the TestKernel_Helper.cpp module. This module
//  implements some grunt work stuff that we just want to keep separated
//  out.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ----------------------------------------------------------------------------
//  A simple class that simplifies the reporting of the current file and
//  line when errors occur.
// ----------------------------------------------------------------------------
class TCurLn
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructors
        // --------------------------------------------------------------------
        TCurLn
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );

        ~TCurLn();


    private :
        // --------------------------------------------------------------------
        //  Declare our friends
        // --------------------------------------------------------------------
        friend TOutStrm& operator<<
        (
                    TOutStrm&               strmTarget
            , const TCurLn&                 clnToStream
        );


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Line
        //      The line number passed in the constructor.
        //
        //  m_pszFile
        //      The name of the file. The path is stripped off so its only
        //      the name part.
        // --------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Line;
        tCIDLib::TCh*   m_pszFile;
};

// ----------------------------------------------------------------------------
//  A streaming operator to get TCurLn objects to an output text stream
// ----------------------------------------------------------------------------
extern TOutStrm& operator<<(TOutStrm& strmTarget, const TCurLn& clnToStream);


// ----------------------------------------------------------------------------
//  A macro to generate the constructor at the point where we want to get
//  the file and line.
// ----------------------------------------------------------------------------
#define CUR_LN TCurLn(CID_FILE, CID_LINE)



// ----------------------------------------------------------------------------
//  Streaming operators to stream out some things
// ----------------------------------------------------------------------------
extern TOutStrm& operator<<(TOutStrm& strmTarget, const TKrnlError& kerrToStream);

