//
// FILE NAME: CIDBuild_BinFile.Hpp
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
//  This is the header for the CIDBuild_BinFile.Cpp file, which implements
//  the TBinFile class. This is a very simple binary file class, which
//  encapsulates the local system's file services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TBinFile
//  PREFIX: bfl
// ---------------------------------------------------------------------------
class TBinFile
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinFile();

        TBinFile
        (
            const   TBldStr&                strName
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Read
            , const tCIDLib::ECreateActs    eAction = tCIDLib::ECreateActs::OpenIfExists
        );

        TBinFile
        (
            const   tCIDLib::EStdFiles      eStdFile
        );

        ~TBinFile();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinFile& operator<<(const tCIDLib::TCard4 c4ToWrite);


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurPos() const;

        tCIDLib::TCard4 c4Read
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4Max
            , const tCIDLib::TBoolean       bMustReadAll = kCIDLib::True
        );

        tCIDLib::TCard4 c4SeekToEnd();

        tCIDLib::TCard4 c4Size() const;

        tCIDLib::TCard4 c4Write
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4ToWrite
            , const tCIDLib::TBoolean       bMustWriteAll = kCIDLib::True
        );

        tCIDLib::TVoid Close();

        tCIDLib::TVoid Open
        (
            const   TBldStr&                strName
            , const tCIDLib::EAccessModes   eAccess
            , const tCIDLib::ECreateActs    eAction
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eStdFile
        );

        tCIDLib::TVoid SeekTo
        (
            const   tCIDLib::TCard4         c4To
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TBinFile(const TBinFile&);
        tCIDLib::TVoid operator=(const TBinFile&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  Most of these are provided by the platform implementation
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4Max
        );

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4ToWrite
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hflThis
        //      The handle to this file. Only the platform implementation
        //      knows what this means, or what is a valid value.
        //
        //  m_strFileName
        //      The name of the file this object represents.
        // -------------------------------------------------------------------
        tCIDBuild::TFileHandle  m_hflThis;
        TBldStr                 m_strFileName;
};


inline TBinFile& TBinFile::operator<<(const tCIDLib::TCard4 c4ToWrite)
{
    c4Write(&c4ToWrite, sizeof(c4ToWrite));
    return *this;
}

