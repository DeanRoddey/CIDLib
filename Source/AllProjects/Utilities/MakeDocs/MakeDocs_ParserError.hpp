//
// FILE NAME: MakeDocs_ParseError.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
        tCIDLib::TCard4 c4Line() const
        {
            return m_c4SourceLine;
        }

        tCIDLib::TErrCode errcId() const
        {
            return m_errcId;
        }

        const TString& strText() const
        {
            return m_strText;
        }


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


