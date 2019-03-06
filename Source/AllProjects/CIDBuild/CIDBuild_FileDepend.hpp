//
// FILE NAME: CIDBuild_FileDepend.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/23/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_FileDepend.Cpp file. This file
//  implements the TFileDepAnalyzer class, which does the job of figuring
//  out file dependencies for a particular project and outputting them to a
//  .Depend file in that project's directory.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: THeaderInfo
//  PREFIX: hdri
// ---------------------------------------------------------------------------
class THeaderInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THeaderInfo();

        THeaderInfo
        (
            const   TBldStr&                strFileName
            , const TBldStr&                strPath
        );

        ~THeaderInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddHeader
        (
            const   TBldStr&                strToAdd
        );

        tCIDLib::TBoolean bGenerated() const;

        tCIDLib::TBoolean bGenerated
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bSearched() const;

        tCIDLib::TBoolean bSearched
        (
            const   tCIDLib::TBoolean       bToSet
        );

        TList<TBldStr>& listIncluded();

        const TBldStr& strFileName() const;

        const TBldStr& strPath() const;


    protected :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        THeaderInfo(const THeaderInfo&);
        tCIDLib::TVoid operator=(const THeaderInfo&);


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bGenerated
        //      When we get ready to spit out all the dependencies for a cpp,
        //      we have to spit out each header only once. So we run through
        //      and clear this flag on each one, and then as we generate the
        //      dependencies, we mark each one to indicate it's been done.
        //
        //  m_bSearched
        //      This indicated whether this file has been searched. We have
        //      to do a search of all include files found so this keeps us
        //      from doing redundant or circular searches.
        //
        //  m_strFileName
        //      The name part of the file. Just the name is stripped out into
        //      this field.
        //
        //  m_strPath
        //      This is the full path to the file. If the file was found in
        //      the current directory, then this is empty. Otherwise, it is
        //      the path to the file, including the file name itself.
        //
        //  m_listIncluded
        //      The list of files that this file included (not transitive, just
        //      direct children.)
        // -----------------------------------------------------------------------------
        tCIDLib::TBoolean   m_bGenerated;
        tCIDLib::TBoolean   m_bSearched;
        TBldStr             m_strFileName;
        TBldStr             m_strPath;
        TList<TBldStr>      m_listIncluded;
};



// ---------------------------------------------------------------------------
//   CLASS: TTokenInfo
//  PREFIX: toki
// ---------------------------------------------------------------------------
class TTokenInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTokenInfo();

        TTokenInfo
        (
            const   TBldStr&                strTokenName
            , const tCIDBuild::ETokens      eType
        );

        ~TTokenInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TBldStr& strTokenName() const;


    protected :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTokenInfo(const TTokenInfo&);
        tCIDLib::TVoid operator=(const TTokenInfo&);


    private:
        // -------------------------------------------------------------------
        //  Private data membes
        //
        //  m_eType
        //      The type of token that this object represents.
        //
        //  m_strTokenName
        //      The name of the token, from the #define statement.
        // -------------------------------------------------------------------
        tCIDBuild::ETokens  m_eType;
        TBldStr             m_strTokenName;
};



// ---------------------------------------------------------------------------
//   CLASS: TFileDepAnalyzer
//  PREFIX: fda
// ---------------------------------------------------------------------------
class TFileDepAnalyzer
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileDepAnalyzer
        (
            const   TProjectInfo&           projiTarget
        );

        ~TFileDepAnalyzer();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid MakeDeps();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFileDepAnalyzer(const TFileDepAnalyzer&);
        tCIDLib::TVoid operator=(const TFileDepAnalyzer&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSearchInclude
        (
            const   TBldStr&                strToFind
            ,       TBldStr&                strToFill
        )   const;

        tCIDLib::TVoid ClearCacheGenMarks();

        tCIDLib::TVoid DoFile
        (
                    THeaderInfo&            hdriCur
        );

        tCIDLib::TVoid DumpDeps
        (
                    THeaderInfo&            hdriCur
            , const tCIDLib::TCard4         c4Level
        );

        tCIDBuild::ETokens eNextToken
        (
                    TLineSpooler&           lsplInput
            ,       TBldStr&                strToFill
        );

        THeaderInfo* phdriFind
        (
            const   TBldStr&                strName
        )   const;

        TTokenInfo* ptokiFind
        (
            const   TBldStr&                strName
        )   const;

        tCIDLib::TVoid UnDefToken
        (
            const   TBldStr&                strName
        );

        tCIDLib::TVoid WriteHeaders
        (
            const   tCIDLib::TCard4         c4Level
            ,       TTextFile&              tflOut
            ,       THeaderInfo&            hdriCur
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_listIncludes
        //      These are the parts of the include path. Since we use a
        //      model that copies all headers to an output path before
        //      they are used, we know where all of the headers are. So we
        //      just set them up upon construction and keep them around.
        //      This list just references existing header info objects in
        //      the m_listHeaders list.
        //
        //  m_listTokens
        //      This is the list of defined tokens.
        //
        //  m_projiTarget
        //      A pointer to the project info structure that we were given.
        //      It tells us all we need to know about the project, though
        //      we also get some global info from the facility object.
        //
        //  m_strDepFile
        //      The path to the .Depend file for the project that we are
        //      do dependencies for.
        // -------------------------------------------------------------------
        TList<TBldStr>      m_listIncludes;
        TList<TTokenInfo>   m_listTokens;
        const TProjectInfo& m_projiTarget;
        TBldStr             m_strDepFile;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_listHeaders
        //      This is the list of cached header info objects. We track
        //      information about each header as it is processed and use that
        //      info to speed up later work. This info is built across all
        //      project's processed, since each subsequent one is likely to
        //      depend the headers of the underlying projects.
        // -------------------------------------------------------------------
        static TList<THeaderInfo>   s_listHeaders;
};



