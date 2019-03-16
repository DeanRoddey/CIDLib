//
// FILE NAME: MakeDocs_ParseError.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This is the header for the MakeDocs_ParseError.cpp module. This module
//  implements the TDocError class, which is the exception thrown when an
//  error occurs during parsing. It contains the info that should be shown
//  to the caller.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  CLASS: TDocError
// PREFIX: err
// ----------------------------------------------------------------------------
class TDocError : public TObject
{
    public  :
        // --------------------------------------------------------------------
        //  Constructors and destructors
        // --------------------------------------------------------------------
        TDocError
        (
            const   tCIDLib::TErrCode       errcId
            , const tCIDLib::TCard4         c4SourceLine
        );

        TDocError
        (
            const   tCIDLib::TErrCode       errcId
            , const tCIDLib::TCard4         c4SourceLine
            , const TString&                strText1
        );

        TDocError
        (
            const   tCIDLib::TErrCode       errcId
            , const tCIDLib::TCard4         c4SourceLine
            , const TString&                strText1
            , const TString&                strText2
        );

        ~TDocError();


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TCard4 c4Line() const;

        tCIDLib::TErrCode errcId() const;

        const TString& strText() const;

    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SourceLine
        //      This is the line in the source file where the error occured.
        //
        //  m_errcId
        //      This is the id of the error that was thrown.
        //
        //  m_strText
        //      The text of the error.
        // --------------------------------------------------------------------
        tCIDLib::TCard4     m_c4SourceLine;
        tCIDLib::TErrCode   m_errcId;
        TString             m_strText;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TDocError,TObject)
};

inline tCIDLib::TCard4 TDocError::c4Line() const
{
    return m_c4SourceLine;
}

inline tCIDLib::TErrCode TDocError::errcId() const
{
    return m_errcId;
}

inline const TString& TDocError::strText() const
{
    return m_strText;
}

