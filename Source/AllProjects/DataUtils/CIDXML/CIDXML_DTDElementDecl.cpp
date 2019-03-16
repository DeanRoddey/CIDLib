//
// FILE NAME: CIDXML_DTDElementDecl.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TDTDElementDecl class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDTDAttrDefCursor,TAttrDefCursor)
RTTIDecls(TDTDElemDecl, TXMLElementDecl)



// ---------------------------------------------------------------------------
//  TDTDAttrDefCursor: Constructors and Destructor
// ---------------------------------------------------------------------------
TDTDAttrDefCursor::TDTDAttrDefCursor(const TDTDElemDecl& xdeclToCursor) :

    m_pxnipcThis(0)
{
    //
    //  If the passed element ever got an attribute list created, then create
    //  a cursor for it. Else just leave it zero.
    //
    if (xdeclToCursor.m_pxnipAttrList)
        m_pxnipcThis = new TXMLNameIDPoolCursor<TDTDAttrDef>(xdeclToCursor.m_pxnipAttrList);
}

TDTDAttrDefCursor::TDTDAttrDefCursor(const TDTDAttrDefCursor& adcToCopy) :

    m_pxnipcThis(0)
{
    // If the source got set up, then create a copy, else stick with null
    if (adcToCopy.m_pxnipcThis)
        m_pxnipcThis = new TXMLNameIDPoolCursor<TDTDAttrDef>(*adcToCopy.m_pxnipcThis);
}

TDTDAttrDefCursor::~TDTDAttrDefCursor()
{
    delete m_pxnipcThis;
}


// ---------------------------------------------------------------------------
//  TDTDAttrDefCursor: Public operators
// ---------------------------------------------------------------------------
TDTDAttrDefCursor&
TDTDAttrDefCursor::operator=(const TDTDAttrDefCursor& adcToAssign)
{
    if (this != &adcToAssign)
    {
        // If none of the above scenarios, then we stick with a null cursor
        if (m_pxnipcThis && adcToAssign.m_pxnipcThis)
        {
            // Both of has have an underlying poll cursor, so just assign
            *m_pxnipcThis = *adcToAssign.m_pxnipcThis;
        }
         else if (m_pxnipcThis)
        {
            // We have one and they don't, so get rid of ours
            delete m_pxnipcThis;
            m_pxnipcThis = 0;
        }
         else if (adcToAssign.m_pxnipcThis)
        {
            // They have one and we don't so get a new one that is a copy of theirs
            m_pxnipcThis = new TXMLNameIDPoolCursor<TDTDAttrDef>(*adcToAssign.m_pxnipcThis);
        }
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TDTDAttrDefCursor: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDTDAttrDefCursor::bNext()
{
    if (!m_pxnipcThis)
        return kCIDLib::False;
    return m_pxnipcThis->bNext();
}

tCIDLib::TBoolean TDTDAttrDefCursor::bReset()
{
    if (!m_pxnipcThis)
        return kCIDLib::False;
    return m_pxnipcThis->bReset();
}

const TXMLAttrDef& TDTDAttrDefCursor::xadCur() const
{
    CIDAssert(m_pxnipcThis != 0, L"Accessed unset attr def cursor");

    //
    //  NOTE: The name id pool that we use for our storage returns the
    //  correct exception (defined by our base class) for an invalid cursor.
    //  So we don't need to trap it and translate it.
    //
    return m_pxnipcThis->objCur();
}



// ---------------------------------------------------------------------------
//  CLASS: TDTDElemDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TDTDElemDecl: Public, static methods
// ---------------------------------------------------------------------------
const TString& TDTDElemDecl::strExtractKey(const TDTDElemDecl& xdeclSrc)
{
    return xdeclSrc.strFullName();
}


// ---------------------------------------------------------------------------
//  TDTDElemDecl: Constructors and Destructor
// ---------------------------------------------------------------------------
TDTDElemDecl::TDTDElemDecl( const   TString&                strName
                            , const tCIDXML::EElemModels    eModel) :
    m_eModel(eModel)
    , m_eTextType(tCIDXML::EElemTextTypes::Any)
    , m_padcThis(0)
    , m_pxcmThis(0)
    , m_pxnipAttrList(0)
    , m_strName(strName)
{
}

TDTDElemDecl::~TDTDElemDecl()
{
    delete m_padcThis;
    delete m_pxcmThis;
    delete m_pxnipAttrList;
}


// ---------------------------------------------------------------------------
//  TDTDElemDecl: Public, inherited methods
// ---------------------------------------------------------------------------
TAttrDefCursor& TDTDElemDecl::adcThis() const
{
    // Fault in our attribute def cursor if not already there
    if (!m_padcThis)
        m_padcThis = new TDTDAttrDefCursor(*this);
    return *m_padcThis;
}


tCIDLib::TBoolean TDTDElemDecl::bClearAttrFlags()
{
    // If we don't have a hash set allocated, then we don't have any
    if (!m_pxnipAttrList)
        return kCIDLib::False;

    //
    //  We have to use the internal iterator of the hash set since we are
    //  going to modify the elements. If there are no elements in it, we
    //  don't have any.
    //
    if (!m_pxnipAttrList->bResetIter())
        return kCIDLib::False;

    // Iterate them all and clear the provided flags
    do
    {
        m_pxnipAttrList->objCur().bProvided(kCIDLib::False);
    }   while (m_pxnipAttrList->bNext());
    return kCIDLib::True;
}


tCIDLib::TVoid
TDTDElemDecl::FormatCMTo(       TTextOutStream& strmDest
                        , const TXMLValidator&  xvalPools) const
{
    switch(m_eModel)
    {
        case tCIDXML::EElemModels::Any :
            strmDest << L"ANY";
            break;

        case tCIDXML::EElemModels::Empty :
            strmDest << L"EMPTY";
            break;

        case tCIDXML::EElemModels::Mixed :
        case tCIDXML::EElemModels::Children :
            // Let the content model object do the work here
            m_pxcmThis->FormatTo(strmDest, xvalPools);
            break;

        default :
            // If debugging, then freak out here
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCM_UnknownCMType
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(m_eModel))
            );
            #endif
            break;
    }
}


//
//  We've precalculated this so just return it. Its inherited which is why we
//  out of line it despite its simplicity.
//
tCIDXML::EElemTextTypes TDTDElemDecl::eTextType() const
{
    return m_eTextType;
}


tCIDXML::EValidRes
TDTDElemDecl::eValidate(const   tCIDLib::TCard4* const  pc4ChildIds
                        , const tCIDLib::TCard4         c4ChildCount
                        ,       tCIDLib::TCard4&        c4FailedAt) const
{
    // Sanity check if debugging
    #if CID_DEBUG_ON
    if (((m_eModel == tCIDXML::EElemModels::Any)
    ||   (m_eModel == tCIDXML::EElemModels::Empty))
    &&  m_pxcmThis)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_StateError
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    switch(m_eModel)
    {
        case tCIDXML::EElemModels::Any :
            // This always passes
            break;

        case tCIDXML::EElemModels::Empty :
            if (c4ChildCount != 0)
                return tCIDXML::EValidRes::TooMany;
            break;

        case tCIDXML::EElemModels::Mixed :
        case tCIDXML::EElemModels::Children :
            // For these, we let the content model object do the work
            return m_pxcmThis->eValidate(pc4ChildIds, c4ChildCount, c4FailedAt);
            break;

        default :
            // If debugging, then freak out here
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCM_UnknownCMType
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(m_eModel))
            );
            #endif
            break;
    }

    return tCIDXML::EValidRes::Success;
}


tCIDLib::TVoid
TDTDElemDecl::FormatTo(         TTextOutStream& strmDest
                        , const TXMLValidator&  xvalPools) const
{
    strmDest << L"<!ELEMENT " << strName() << L" ";

    switch(m_eModel)
    {
        case tCIDXML::EElemModels::Any :
            strmDest << L"ANY";
            break;

        case tCIDXML::EElemModels::Empty :
            strmDest << L"EMPTY";
            break;

        case tCIDXML::EElemModels::Mixed :
        case tCIDXML::EElemModels::Children :
            // Let the content model object do the work here
            m_pxcmThis->FormatTo(strmDest, xvalPools);
            break;

        default :
            // If debugging, then freak out here
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCM_UnknownCMType
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(m_eModel))
            );
            #endif
            break;
    }

    strmDest << L">";
}


TXMLAttrDef* TDTDElemDecl::pxadFindAttrDef(const TString& strName)
{
    // If no list has been created yet, then we don't have any
    if (!m_pxnipAttrList)
        return 0;

    // Else look it up, returning zero if not found
    return m_pxnipAttrList->pobjByName(strName);
}

const TXMLAttrDef* TDTDElemDecl::pxadFindAttrDef(const TString& strName) const
{
    // If no list has been created yet, then we don't have any
    if (!m_pxnipAttrList)
        return 0;

    // Else look it up, returning zero if not found
    return m_pxnipAttrList->pobjByName(strName);
}


TXMLAttrDef*
TDTDElemDecl::pxadFindAttrDef(  const   TString&            strName
                                ,       tCIDLib::TBoolean&  bAdded)
{
    TDTDAttrDef* pxadRet = 0;

    // If we have a pool already, see if it's in there
    if (m_pxnipAttrList)
        pxadRet = m_pxnipAttrList->pobjByName(strName);

    // If no pool or this guy is not present, then add it
    if (!pxadRet)
    {
        pxadRet = new TDTDAttrDef(strName);
        AddAttrDef(pxadRet);
        bAdded = kCIDLib::True;
    }
     else
    {
        bAdded = kCIDLib::False;
    }
    return pxadRet;
}


const TString& TDTDElemDecl::strFullName() const
{
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TDTDElemDecl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDTDElemDecl::AddAttrDef(TDTDAttrDef* const pxadToAdopt)
{
    // If we don't have a pool yet, then add one
    if (!m_pxnipAttrList)
    {
        m_pxnipAttrList = new TXMLNameIDPool<TDTDAttrDef>
        (
            &TDTDAttrDef::strExtractKey
        );
    }

    // We don't use the pool ids for attributes, so just throw it away
    m_pxnipAttrList->c4AddNew(pxadToAdopt);
}

tCIDLib::TVoid
TDTDElemDecl::AdoptContentModel(TXMLContentModel* const pxcmToAdopt)
{
    delete m_pxcmThis;
    m_pxcmThis = pxcmToAdopt;
}


tCIDXML::EElemModels TDTDElemDecl::eModel() const
{
    return m_eModel;
}

tCIDXML::EElemTextTypes
TDTDElemDecl::eTextType(const tCIDXML::EElemTextTypes eType)
{
    m_eTextType = eType;
    return m_eTextType;
}


tCIDXML::EElemModels TDTDElemDecl::eModel(const tCIDXML::EElemModels eModel)
{
    m_eModel = eModel;
    return m_eModel;
}


const TString& TDTDElemDecl::strName() const
{
    return m_strName;
}

const TString& TDTDElemDecl::strName(const TString& strNewName)
{
    m_strName = strNewName;
    return m_strName;
}


