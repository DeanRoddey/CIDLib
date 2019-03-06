//
// FILE NAME: CIDBuild_MsgIdInfo.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_MsgIdInfo.cpp, which implements the
//  TIdInfo class. This class contains information parsed in from the message
//  text file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TIdInfo
//  PREFIX: idi
// ---------------------------------------------------------------------------
class TIdInfo
{

    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        enum class ETypes
        {
            Common
            , Err
            , Msg
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TIdInfo();

        TIdInfo
        (
            const   TBldStr&                strName
            , const ETypes                  eType
            , const tCIDLib::TCard4         c4Id
            , const TBldStr&                strText
        );

        ~TIdInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        ETypes eType() const;

        const TBldStr& strText() const;

        const TBldStr& strName() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eType
        //      The type of id, Common, Error, or Messages.
        //
        //  m_strText
        //      The text of the message, which is always in Unicode in the
        //      local wide character format. If its not that way in the source
        //      file, its trancoded before storing.
        //
        //  m_strName
        //      The name of the id, i.e. the name of the constant that gets
        //      written to the namespace.
        //
        //  m_c4Id
        //      The numeric value of this id.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Id;
        ETypes          m_eType;
        TBldStr         m_strText;
        TBldStr         m_strName;
};


// ---------------------------------------------------------------------------
//  TIdInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TIdInfo::c4Id() const
{
    return m_c4Id;
}

inline TIdInfo::ETypes TIdInfo::eType() const
{
    return m_eType;
}

inline const TBldStr& TIdInfo::strText() const
{
    return m_strText;
}

inline const TBldStr& TIdInfo::strName() const
{
    return m_strName;
}

