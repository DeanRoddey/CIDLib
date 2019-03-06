//
// FILE NAME: CIDXML_EntityMgr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_EntityMgr.Cpp file, which
//  implements the TXMLEntityMgr class. This class is used by the core parser
//  to store a stack of entities that are being parsed. Since entities can
//  refer to other entities, a stack mechanism is required to represent this
//  LIFO type nested arrangement.
//
//  The stack is of TXMLEntSpooler objects. Each spooler represents one
//  XML entity and internalizes and spools out the characters from that
//  entity.
//
//  A lot of the public methods are just passthroughs that redirect the call
//  to a related method on the current top of stack spooler. This keeps the
//  issue of when spoolers end and are popped off away from the outside world.
//
//  We also create a couple of specialized janitor classes that allow us to
//  make safe changes to the entity manater on a scoped basis.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TXMLEMThrowJan;
class TXMLParserCore;


// ---------------------------------------------------------------------------
//  CLASS: TXMLEntityMgr
// PREFIX: xem
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLEntityMgr : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEntityMgr();

        ~TXMLEntityMgr();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAtRootEntity() const;

        tCIDLib::TBoolean bGetName
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bGetNameToken
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bGetQuotedString
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bIgnoreBadChars
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bParseEqualSign();

        tCIDLib::TBoolean bPeekAhead
        (
                    tCIDLib::TCh&           chToFill
            , const tCIDLib::TCard4         c4AheadBy
        );

        tCIDLib::TBoolean bPushEntity
        (
                    TXMLEntSpooler* const   pxesToAdd
            , const TXMLEntityDecl* const   pxdeclToAdd
            ,       TXMLParserCore&         xprsOwner
        );

        tCIDLib::TBoolean bSkipPastChar
        (
            const   tCIDLib::TCh            chToSkipPast
        );

        tCIDLib::TBoolean bSkippedChar
        (
            const   tCIDLib::TCh            chToSkip
        );

        tCIDLib::TBoolean bSkippedQuote
        (
                    tCIDLib::TCh&           chSkipped
        );

        tCIDLib::TBoolean bSkippedSpace();

        tCIDLib::TBoolean bSkippedSpaces
        (
            const   tCIDLib::TBoolean       bCrossEntities = kCIDLib::False
        );

        tCIDLib::TBoolean bSkippedString
        (
            const   TString&                strToSkip
        );

        tCIDLib::TBoolean bSkippedString
        (
            const   tCIDLib::TCh* const     pszToSkip
        );

        tCIDLib::TCard4 c4CurColumn() const;

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4CurSpoolerId() const;

        tCIDLib::TCh chGetNext();

        tCIDLib::TCh chGetNextIfNot
        (
            const   tCIDLib::TCh            chNotToGet
        );

        tCIDLib::TCh chPeekNext();

        tCIDLib::TVoid GetSpaces
        (
                    TString&                strToFill
        );

        TXMLEntSpooler* pxesMakeNew
        (
                    tCIDXML::TEntitySrcRef& esrFrom
            , const tCIDLib::TBoolean       bExpectDecl
            , const tCIDXML::ERefFrom       eFrom
            , const tCIDXML::EEntityTypes   eType
        );

        TXMLEntSpooler* pxesMakeNewFromInt
        (
            const   TXMLEntityDecl&         xdeclSource
            , const tCIDXML::ERefFrom       eFrom
            , const tCIDXML::EEntityTypes   eType
        );

        MXMLEntityEvents* pmxevEntityEvents() const;

        MXMLEntityEvents* pmxevEntityEvents
        (
                    MXMLEntityEvents* const pmxevToSet
        );

        tCIDLib::TVoid Reset();

        const TString& strCurSystemId() const;

        const TString& strLastExtId() const;

        tCIDLib::TVoid SetDeclEncoding
        (
            const   TString&                strDeclEncoding
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLEMThrowJan;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLEntityMgr(const TXMLEntityMgr&);
        tCIDLib::TVoid operator=(const TXMLEntityMgr&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPopEntity();


        // -------------------------------------------------------------------
        //  Private data types
        //
        //  This is a simple class which makes up the elements in our stack.
        //  Each stack element holds an entity declaration and the spooler
        //  that is being used to spool it in. The declaration is kept around
        //  for error reporting reasons and because it holds info about the
        //  entity that is needed.
        //
        //  This element owns the spoolers but the entity decls are just
        //  referenced from the pools that they come from.
        // -------------------------------------------------------------------
        class TEMStackElem
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TEMStackElem() :

                    pxesThis(0)
                    , pxdeclThis(0)
                {
                }

                TEMStackElem(       TXMLEntSpooler* const   pxesSrc
                            , const TXMLEntityDecl* const   pxdeclEntity) :

                    pxesThis(pxesSrc)
                    , pxdeclThis(pxdeclEntity)
                {
                }

                TEMStackElem(const TEMStackElem& elemToCopy) :

                    pxesThis(elemToCopy.pxesThis)
                    , pxdeclThis(elemToCopy.pxdeclThis)
                {
                }

                ~TEMStackElem()
                {
                    //
                    //  We don't delete anything here. The spooler does belong
                    //  to the manager, but they are deleted in the entity
                    //  mgr stack, not automatically here.
                    //
                }


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TEMStackElem& operator=(const TEMStackElem& elemToAssign)
                {
                    if (this == &elemToAssign)
                        return *this;

                    pxesThis = elemToAssign.pxesThis;
                    pxdeclThis = elemToAssign.pxdeclThis;

                    return *this;
                }


                // -----------------------------------------------------------
                //  Public data members
                //
                //  pxesSrc
                //      The source entity spooler for the current entity. We
                //      own this one.
                //
                //  pxdeclEntity
                //      A pointer to the entity decl that our spooler is
                //      spooling from. We don't own this!!
                // -----------------------------------------------------------
                TXMLEntSpooler*         pxesThis;
                const TXMLEntityDecl*   pxdeclThis;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bExhausted
        //      This is set when we hit the end of the original entity. Its
        //      used to efficiently catch subsequent calls after the end is
        //      hit (which is an error.)
        //
        //  m_bThrowAtEnd
        //      This flag indicates whether an exception is to be thrown at
        //      the end of entities or not.
        //
        //  m_c4StackTop
        //      The current top of the entity stack. When this hits the
        //      current vector size, we add another element to the vector.
        //
        //  m_pcolEntityStack
        //      This is a by value vector of TEMStackElem objects, which
        //      represents the nested structure of elements being parsed. Its
        //      a pointer because of portability issues. We use a vector
        //      instead of a stack because we want to just let it grow up as
        //      needed and keep those elements around for reuse. We'll just
        //      use m_c4StackTop to keep up with the current stack top and
        //      grow the vector as required.
        //
        //  m_pmxevEntityEvents
        //      This is the event handler, if any, currently installed. The
        //      core parser (who owns us) keeps up informed of the current
        //      entity handler. We use it to send begin/end entity events and
        //      to do entity resolution. We just reference it, not own it.
        //
        //  m_pxdeclCurrent
        //      The current entity decl. This is optional, since the entity
        //      could just be the main entity where the parser started. This
        //      enitity has no declaration. Same applies for the DTD external
        //      subset declaration.
        //
        //  m_pxesCurrent
        //      This is the current entity spooler. Since its the one that is
        //      always being passed through to, we don't keep it on the stack.
        //      So this one is never zero after the parse starts, because at
        //      worst it represents the main entity where the parse started.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bExhausted;
        tCIDLib::TBoolean       m_bIgnoreBadChars;
        tCIDLib::TBoolean       m_bThrowAtEnd;
        tCIDLib::TCard4         m_c4StackTop;
        TVector<TEMStackElem>*  m_pcolEntityStack;
        MXMLEntityEvents*       m_pmxevEntityEvents;
        const TXMLEntityDecl*   m_pxdeclCurrent;
        TXMLEntSpooler*         m_pxesCurrent;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLEntityMgr,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TXMLEMStackJan
// PREFIX: jan
// ---------------------------------------------------------------------------
class TXMLEMStackJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEMStackJan(TXMLEntityMgr* const pxemToRemove) :

            m_pxemToRemove(pxemToRemove)
        {
        }

        ~TXMLEMStackJan()
        {
            m_pxemToRemove->Reset();
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pxemToRemove
        //      This is the entity manager that we are to reset when we
        //      destruct.
        // -------------------------------------------------------------------
        TXMLEntityMgr*  m_pxemToRemove;
};


// ---------------------------------------------------------------------------
//  CLASS: TXMLEMThrowJan
// PREFIX: jan
// ---------------------------------------------------------------------------
class TXMLEMThrowJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEMThrowJan(         TXMLEntityMgr* const    pxemTarget
                        , const tCIDLib::TBoolean       bNewState) :

            m_bOldState(pxemTarget->m_bThrowAtEnd)
            , m_pxemTarget(pxemTarget)
        {
            m_pxemTarget->m_bThrowAtEnd = bNewState;
        }

        ~TXMLEMThrowJan()
        {
            m_pxemTarget->m_bThrowAtEnd = m_bOldState;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bOldState
        //      The previous state of the flag we manager, to be restored
        //      when we are destructed.
        //
        //  m_pxemTarget
        //      This is the entity manager that we are reset the flag on when
        //      we destruct.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bOldState;
        TXMLEntityMgr*      m_pxemTarget;
};

#pragma CIDLIB_POPPACK


