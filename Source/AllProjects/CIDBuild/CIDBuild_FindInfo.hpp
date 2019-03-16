//
// FILE NAME: CIDBuild_FindInfo.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/1998
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
//  This is the header implements the very simple TFindInfo class, which is
//  used to report directory search information.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TFindInfo
//  PREFIX: fndi
// ---------------------------------------------------------------------------
class TFindInfo
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsDirectory
        (
            const   TBldStr&                strToCheck
        );

        static tCIDLib::TBoolean bFindAFile
        (
            const   TBldStr&                strToFind
            ,       TFindInfo&              fndiToFill
            , const tCIDBuild::EPathModes   eMode = tCIDBuild::EPathModes::Base
        );

        static tCIDLib::TBoolean bFindAFile
        (
            const   TBldStr&                strDirectory
            , const TBldStr&                strFile
            ,       TFindInfo&              fndiToFill
            , const tCIDBuild::EPathModes   eMode = tCIDBuild::EPathModes::Base
        );

        static tCIDLib::TCard4 c4FindFiles
        (
            const   TBldStr&                strToFind
            ,       TList<TFindInfo>&       listToFill
            , const tCIDBuild::EPathModes   eMode = tCIDBuild::EPathModes::Base
        );

        static tCIDLib::TCard4 c4FindFiles
        (
            const   TBldStr&                strDirectory
            , const TBldStr&                strFile
            ,       TList<TFindInfo>&       listToFill
            , const tCIDBuild::EPathModes   eMode = tCIDBuild::EPathModes::Base
        );



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFindInfo();

        TFindInfo
        (
            const   TBldStr&                strFileName
            , const tCIDLib::TBoolean       bIsDir
        );

        TFindInfo
        (
            const   TBldStr&                strFileName
            , const tCIDLib::TBoolean       bIsDir
            , const tCIDLib::TFloat8        tmLastWrite
            , const tCIDLib::TCard4         c4Size
        );

        ~TFindInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        friend tCIDLib::TBoolean operator>
        (
            const   TFindInfo&              fndiLHS
            , const TFindInfo&              fndiRHS
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDirectory() const;

        tCIDLib::TBoolean bIsSpecialDirectory() const;

        const TBldStr& strFileName() const;

        tCIDLib::TVoid Set
        (
            const   TBldStr&                strFileName
            , const tCIDLib::TBoolean       bIsDir
            , const tCIDLib::TFloat8        tmLastWrite
            , const tCIDLib::TCard4         c4Size
        );

        tCIDLib::TFloat8 tmLastWrite() const;

        tCIDLib::TCard4 c4Size() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TFindInfo
        (
            const   tCIDLib::TVoid* const   pHostFindBuf
            , const TBldStr&                strSearchPath
            , const tCIDBuild::EPathModes   eMode = tCIDBuild::EPathModes::Base
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetFromHostInfo
        (
            const   tCIDLib::TVoid* const   pHostFindBuf
            , const TBldStr&                strSearchPath
            , const tCIDBuild::EPathModes   eMode
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsDir
        //      Indicates if this is a directory. If not, its a file.
        //
        //  m_c4Size
        //      The size of the file in bytes.
        //
        //  m_strFileName
        //      The name of the file that this info represents.
        //
        //  m_tmLastWrite
        //      The calculated time of the last write. Its not any kind of
        //      absolute value necessarily. Its just to do dependency checking
        //      to find out of a dependent file needs to be updated.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bIsDir;
        tCIDLib::TCard4     m_c4Size;
        TBldStr             m_strFileName;
        tCIDLib::TFloat8    m_tmLastWrite;
};


// ---------------------------------------------------------------------------
//  TFindInfo: Public operators
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
operator>(const TFindInfo& fndiLHS, const TFindInfo& fndiRHS)
{
    return fndiLHS.m_tmLastWrite > fndiRHS.m_tmLastWrite;
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TFindInfo::bIsDirectory() const
{
    return m_bIsDir;
}

inline const TBldStr& TFindInfo::strFileName() const
{
    return m_strFileName;
}

inline tCIDLib::TFloat8 TFindInfo::tmLastWrite() const
{
    return m_tmLastWrite;
}

inline tCIDLib::TCard4 TFindInfo::c4Size() const
{
    return m_c4Size;
}

