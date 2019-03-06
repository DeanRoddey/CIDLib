//
// FILE NAME: CIDResEd_FileSpooler.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDResEd_FileSpooler.cpp file, which implements
//  a simple convenience class to help with parsing the source files (resource
//  and message text) that we read in (from existing facilities, and write
//  out again later.)
//
//  It just wraps a text input stream, and handles skipping over comments
//  and empty lines. The source files are line oriented, so it just has to
//  read lines at a time. It will strip leading and trailng whitespace from
//  lines for us as well before giving them back.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResEdSpooler
//  PREFIX: spool
// ---------------------------------------------------------------------------
class TResEdSpooler : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdSpooler
        (
            const   TString&                strSrcFile
        );

        ~TResEdSpooler();



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckPrefix
        (
            const   TString&                strPref
            ,       TString&                strRemainder
            , const tCIDLib::TBoolean       bRequired
        );

        tCIDLib::TBoolean bCheckPrefixVal
        (
            const   TString&                strPref
            ,       tCIDLib::TCard4&        c4Val
            , const tCIDLib::TBoolean       bRequired
        );

        tCIDLib::TBoolean bEndOfStream() const;

        tCIDLib::TBoolean bPeakLine
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bEndOk = kCIDLib::False
        );

        tCIDLib::TBoolean bReadLine
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bEndOk = kCIDLib::False
        );

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4CheckPrefix
        (
            const   tCIDLib::TStrList&      colPrefs
            , const TString&                strEnd
            ,       TString&                strRemainder
        );

        tCIDLib::TVoid CheckNext
        (
            const   TString&                strToCheck
        );

        tCIDLib::TVoid EatLine();

        tCIDLib::TVoid ParseNameVal
        (
                    TString&                strToParse
            ,       tCIDLib::TCard4&        c4Value
        );

        tCIDLib::TVoid ParseNameValText
        (
                    TString&                strToParse
            ,       tCIDLib::TCard4&        c4Value
            ,       TString&                strText
        );

        const TString& strSrcFile() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TResEdSpooler();
        TResEdSpooler(const TResEdSpooler&);
        tCIDLib::TVoid operator=(const TResEdSpooler&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurLine
        //      We count lines as we read them so we can report source file
        //      error positions.
        //
        //  m_strmSrc
        //      The stream that we create for streaming in the text
        //
        //  m_strPushBack
        //      Allows us to handle pushing back a line so that we can do a
        //      single line peek.
        //
        //  m_strSrcFile
        //      We save the source file for error reporting purposes
        //
        //  m_strTmp
        //      A temp string for internal reading purposes
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CurLine;
        TTextFileInStream   m_strmSrc;
        TString             m_strSrcFile;
        TString             m_strPushBack;
        TString             m_strTmp;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdSpooler,TObject)
};


