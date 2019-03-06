//
// FILE NAME: CIDResEd_MsgData.hpp
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
//  This is the header for the CIDResEd_MsgData.cpp file, which implements a set
//  of classes that handle message text resources. We have a single msg item class
//  and a list of messages. The main resource data class contains a set of these
//  lists, one for each message type.
//
//  We also create a multi-column ops class that makes it easy to load msg items
//  into a mc list.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResEdMsgItem
//  PREFIX: remit
// ---------------------------------------------------------------------------
class TResEdMsgItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eCompByName
        (
            const   TResEdMsgItem&          remit1
            , const TResEdMsgItem&          remit2
        );

        static tCIDLib::ESortComps eKeyComp
        (
            const   TString&                strKey
            , const TResEdMsgItem&          remitComp
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdMsgItem();

        TResEdMsgItem
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
        );

        TResEdMsgItem
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
            , const TString&                strMsg
        );

        TResEdMsgItem
        (
            const   TResEdMsgItem&          remitToCopy
        );

        ~TResEdMsgItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdMsgItem& operator=
        (
            const TResEdMsgItem&            remitToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const TResEdMsgItem&            remitToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const TResEdMsgItem&            remitToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );

        const TString& strMsgText() const;

        const TString& strMsgText
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
            , const TString&                strMsg
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The numeric id of the message, unique within this and any
        //      other msg lists we have. They are assigned automatically, but
        //      we don't want to change them every time we write them out, so
        //      we remember the id we read in for existing messages.
        //
        //  m_strMsgText
        //      The text of the message
        //
        //  m_strName
        //      The header identifier of the message, also unique within this
        //      and any other msg lists we have.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Id;
        TString         m_strMsgText;
        TString         m_strName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdMsgItem,TObject)
};


// Define a list of message items
namespace tCIDResEd
{
    typedef TVector<TResEdMsgItem>      TMsgItemList;
}


// ---------------------------------------------------------------------------
//   CLASS: TResEdMsgList
//  PREFIX: remil
// ---------------------------------------------------------------------------
class TResEdMsgList : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdMsgList();

        TResEdMsgList
        (
            const   tCIDResEd::EMsgTypes    eType
        );

        TResEdMsgList
        (
            const   TResEdMsgList&          remilToCopy
        );

        ~TResEdMsgList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdMsgList& operator=
        (
            const   TResEdMsgList&          remilToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const TResEdMsgList&            remilToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const TResEdMsgList&            remilToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AddNew
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strName
            , const TString&                strMsgText
        );

        tCIDLib::TCard4 c4AddNew
        (
            const   TResEdMsgItem&          remitAdd
        );

        tCIDLib::TCard4 c4Count() const;

        tCIDLib::TCard4 c4MaxId() const;

        tCIDLib::TCard4 c4MinId() const;

        tCIDLib::TCard4 c4FindById
        (
            const   tCIDLib::TCard4         c4Id
        )   const;

        tCIDLib::TCard4 c4FindByName
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard4 c4NextId() const;

        tCIDLib::TCard4 c4UpdateAt
        (
            const   tCIDLib::TCard4         c4Index
            , const TResEdMsgItem&          remitNew
        );

        tCIDLib::TVoid DeleteAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        const TResEdMsgItem* premitById
        (
            const   tCIDLib::TMsgId         midFind
        )   const;

        const TResEdMsgItem* premitByName
        (
            const   TString&                strName
        )   const;

        tCIDLib::TVoid ParseBlock
        (
                    TResEdSpooler&          spoolSrc
            , const TString&                strEndText
        );

        const TResEdMsgItem& remitAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        const TResEdMsgItem& remitById
        (
            const   tCIDLib::TMsgId         midFind
        )   const;

        tCIDLib::TVoid Reset();

        const TString& strNamespace() const;

        const TString& strNamespace
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetTextAt
        (
            const   tCIDLib::TCard4         c4At
            , const TString&                strToSet
        );

        tCIDLib::TVoid SortAndNumber();

        tCIDLib::TVoid WriteMsgs
        (
                    TTextOutStream&         strmTar
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetIdRange();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxId
        //  m_c4MinId
        //      The range of ids that are valid for text values of the type of
        //      this list. We want to keep them all unique.
        //
        //  m_eType
        //      Indicates which type of message this is.
        //
        //  m_colList
        //      We maintain a vector of message items, sorted by name for
        //      display purposes.
        //
        //  m_strNamespace
        //      The namespace that the user wants to have used when spitting
        //      out the header (later when the file is compiled.) This is
        //      either the default (based on the facility name) or indicated
        //      in the CTRL= block in the message text file.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxId;
        tCIDLib::TCard4         m_c4MinId;
        tCIDResEd::EMsgTypes    m_eType;
        tCIDResEd::TMsgItemList m_colList;
        TString                 m_strNamespace;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdMsgList,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TMCListMsgOps
// PREFIX: mclops
// ---------------------------------------------------------------------------
class TMCListMsgOps : public TMCListOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TMCListMsgOps
        (
            const   TResEdMsgList* const    premilMsgs
        );

        TMCListMsgOps
        (
            const   TResEdMsgItem* const    premitSingle
        );


        TMCListMsgOps(const TMCListMsgOps&) = delete;

        ~TMCListMsgOps();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColCount() const override;

        tCIDLib::TCard4 c4RowCount() const override;

        tCIDLib::TCard4 c4RowId(const tCIDLib::TCard4 c4RowIndex) const override;

        const TString& strColumnText
        (
            const   tCIDLib::TCard4     c4RowIndex
            , const tCIDLib::TCard4     c4ColIndex
        )   const override;


        const TResEdMsgItem*    m_premitSingle;
        const TResEdMsgList*    m_premilMsgs;
        mutable TString         m_strTmpFmt;
};


