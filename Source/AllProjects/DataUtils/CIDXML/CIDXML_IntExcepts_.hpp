//
// FILE NAME: CIDXML_IntExcepts_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  There are a few places internally where we need to throw exceptions for
//  purely internal reasons. They don't propogate out. So we don't need the
//  overhead of the standard CIDLib exception mechanism. So a couple of very
//  simple exception classes are defined here.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLEndOfEntity
// PREFIX: exc
// ---------------------------------------------------------------------------
class TXMLEndOfEntity
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEndOfEntity
        (
            const   TXMLEntityDecl&         xdeclEnded
            , const tCIDLib::TCard4         c4SpoolerId
        );

        TXMLEndOfEntity
        (
            const   TXMLEndOfEntity&        excToCopy
        );

        ~TXMLEndOfEntity();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLEndOfEntity& operator=
        (
            const   TXMLEndOfEntity&        excToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SpoolerId() const;

        const TXMLEntityDecl& xdeclEntityEnded() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors
        // -------------------------------------------------------------------
        TXMLEndOfEntity();


        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c4SpoolerId
        //      This is the id of the spooler which was spooling this
        //      particular instance of the entity that ended.
        //
        //  m_xdeclEntity
        //      This is a pointer to the entity which has ended. We don't own
        //      it, we just reference it.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4SpoolerId;
        const TXMLEntityDecl*   m_pxdeclEntity;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TXMLEndOfEntity: Constructors and Destructor
// ---------------------------------------------------------------------------
inline
TXMLEndOfEntity::TXMLEndOfEntity(const  TXMLEntityDecl& xdeclEnded
                                , const tCIDLib::TCard4 c4SpoolerId) :

    m_c4SpoolerId(c4SpoolerId)
    , m_pxdeclEntity(&xdeclEnded)
{
}

inline TXMLEndOfEntity::TXMLEndOfEntity(const TXMLEndOfEntity& excToCopy) :

    m_c4SpoolerId(excToCopy.m_c4SpoolerId)
    , m_pxdeclEntity(excToCopy.m_pxdeclEntity)
{
}

inline TXMLEndOfEntity::~TXMLEndOfEntity()
{
}


// ---------------------------------------------------------------------------
//  TXMLEndOfEntity: Public operators
// ---------------------------------------------------------------------------
inline TXMLEndOfEntity&
TXMLEndOfEntity::operator=(const TXMLEndOfEntity& excToCopy)
{
    if (this != &excToCopy)
        m_pxdeclEntity = excToCopy.m_pxdeclEntity;
    return *this;
}


// ---------------------------------------------------------------------------
//  TXMLEndOfEntity: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TXMLEndOfEntity::c4SpoolerId() const
{
    return m_c4SpoolerId;
}

inline const TXMLEntityDecl& TXMLEndOfEntity::xdeclEntityEnded() const
{
    return *m_pxdeclEntity;
}
