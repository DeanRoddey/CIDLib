//
// FILE NAME: CIDBuild_TextFile.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_TextFile.Cpp file, which implements
//  the TTextFile class. This is a very simple text file class, built on top
//  of the TBinFile class (which abstracts the local file services for us.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TTextFile
//  PREFIX: tfl
// ---------------------------------------------------------------------------
class TTextFile
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextFile();

        TTextFile
        (
            const   TBldStr&                strName
            , const tCIDBuild::EEncodings   eEncoding
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Read
            , const tCIDLib::ECreateActs    eAction = tCIDLib::ECreateActs::OpenIfExists
        );

        TTextFile
        (
            const   tCIDLib::EStdFiles      eStdFile
        );

        ~TTextFile();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextFile& operator<<(const tCIDBuild::ESpecialChars eSpecial);

        TTextFile& operator<<(const tCIDLib::TSCh* const pszToPrint);

        TTextFile& operator<<(const tCIDLib::TCh* const pszToPrint);

        TTextFile& operator<<(const tCIDLib::TCh chToPrint);

        TTextFile& operator<<(const tCIDLib::TCard4 c4ToPrint);

        TTextFile& operator<<(const tCIDLib::TInt4 i4ToPrint);



        // -------------------------------------------------------------------
        //  Public methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Close();

        tCIDLib::TVoid ConIndent(const tCIDLib::TCard4 c4Level);

        tCIDLib::TVoid Indent(const tCIDLib::TCard4 c4Level);

        tCIDLib::TVoid Open
        (
            const   TBldStr&                strName
            , const tCIDBuild::EEncodings   eEncoding
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Read
            , const tCIDLib::ECreateActs    eAction = tCIDLib::ECreateActs::OpenIfExists
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTextFile(const TTextFile&);
        tCIDLib::TVoid operator=(const TTextFile&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bflData
        //      This is the data file that we use for our underlying data
        //      sink/source.
        //
        //  m_eEncoding
        //      The encoding that we should expect the text to be in, or to
        //      write it out as.
        // -------------------------------------------------------------------
        TBinFile                m_bflData;
        tCIDBuild::EEncodings   m_eEncoding;
};

