//
// FILE NAME: CIDBuild_BlkParser.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_BlkParser.cpp file, which implements
//  the TBlkParser class. This is a helper class that automates some of the
//  burden of parsing the common file format used by the input files to the
//  CIDBuild program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TBlkField
//  PREFIX: blkf
// ---------------------------------------------------------------------------
class TBlkField
{
    public :
        // -------------------------------------------------------------------
        //  Class specific types
        // -------------------------------------------------------------------
        enum class EValTypes
        {
            Text
            , Cardinal
            , Integer
            , Boolean
            , Alpha
            , AlphNum
            , Alphas
            , CppName
        };


        // -------------------------------------------------------------------
        //  Constructors and operators
        // -------------------------------------------------------------------
        TBlkField
        (
            const   tCIDLib::TCh*           pszFldName
            , const tCIDLib::TBoolean       bRequired
            , const tCIDLib::TCard4         c4MinVals = 1
            , const tCIDLib::TCard4         c4MaxVals = 1
        );

        ~TBlkField();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFound() const;

        tCIDLib::TBoolean bFound
        (
            const   tCIDLib::TBoolean       bNew
        );

        tCIDLib::TBoolean bRequired() const;

        tCIDLib::TBoolean bRequired
        (
            const   tCIDLib::TBoolean       bValue
        );

        tCIDLib::TCard4 c4LineAt() const;

        tCIDLib::TCard4 c4LineAt
        (
            const   tCIDLib::TCard4         c4New
        );

        EValTypes eValType
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        EValTypes eValType
        (
            const   tCIDLib::TCard4         c4Index
            , const EValTypes               eToSet
        );

        tCIDLib::TVoid Reset();

        const TBldStr& strFieldName() const;

        const TBldStr& strFullValue() const;

        const TBldStr& strFullValue
        (
            const   TBldStr&                strNew
        );

        const TBldStr& strField
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid SetAllValTypes
        (
            const   EValTypes               eType
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckFoundIndex
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid CheckMaxIndex
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid TestAgainstType
        (
            const   tCIDLib::TCh* const     pszVal
            , const tCIDLib::TCard4         c4Index
        )   const;


        // -------------------------------------------------------------------
        //  Private data
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bAllVals;
        tCIDLib::TBoolean   m_bFound;
        tCIDLib::TBoolean   m_bRequired;
        tCIDLib::TCard4     m_c4LineAt;
        tCIDLib::TCard4     m_c4MaxVals;
        tCIDLib::TCard4     m_c4MinVals;
        tCIDLib::TCard4     m_c4ValsFound;
        EValTypes*          m_peValTypes;
        TBldStr*            m_pstrValues;
        TBldStr             m_strFieldName;
        TBldStr             m_strFullValue;
};



// ---------------------------------------------------------------------------
//   CLASS: TBlkParser
//  PREFIX: blkp
// ---------------------------------------------------------------------------
class TBlkParser
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and operators
        // -------------------------------------------------------------------
        TBlkParser
        (
                    TBlkField*              pablkfToParse
            , const tCIDLib::TCard4         c4FldInfoCount
            ,       TLineSpooler*           plsplInput
            , const TBldStr&                strEndText
        );

        ~TBlkParser();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Parse();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIndex
        (
            const   tCIDLib::TCard4         c4ToCheck
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ablkfToParse
        //      The array of block field info objects that drive the parse
        //      operation.
        //
        //  m_c4FldCount
        //      The number of entries in the field data array.
        //
        //  m_plsplInput
        //      The input spooler to get the data from.
        //
        //  m_strEndText
        //      The text that triggers the end of the block are are parsing.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4FldCount;
        TBlkField*          m_pablkfToParse;
        TLineSpooler*       m_plsplInput;
        TBldStr             m_strEndText;
};


// ---------------------------------------------------------------------------
//  TBlkField: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TBlkField::bFound() const
{
    return m_bFound;
}

inline tCIDLib::TBoolean TBlkField::bFound(const tCIDLib::TBoolean bNew)
{
    m_bFound = bNew;
    return m_bFound;
}

inline tCIDLib::TBoolean TBlkField::bRequired() const
{
    return m_bRequired;
}

inline tCIDLib::TBoolean TBlkField::bRequired(const tCIDLib::TBoolean bValue)
{
    m_bRequired = bValue;
    return m_bRequired;
}

inline tCIDLib::TCard4 TBlkField::c4LineAt() const
{
    return m_c4LineAt;
}

inline tCIDLib::TCard4 TBlkField::c4LineAt(const tCIDLib::TCard4 c4New)
{
    m_c4LineAt = c4New;
    return m_c4LineAt;
}

inline const TBldStr& TBlkField::strFieldName() const
{
    return m_strFieldName;
}

inline const TBldStr& TBlkField::strFullValue() const
{
    return m_strFullValue;
}

