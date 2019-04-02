//
// FILE NAME: CIDDBase_Bindings.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This file implements a family of binding description objects
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
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDBBinding,TObject)
RTTIDecls(TDBBoolBinding,TDBBinding)
RTTIDecls(TDBCard1Binding,TDBBinding)
RTTIDecls(TDBCard2Binding,TDBBinding)
RTTIDecls(TDBCard4Binding,TDBBinding)
RTTIDecls(TDBCard8Binding,TDBBinding)
RTTIDecls(TDBDateBinding,TDBBinding)
RTTIDecls(TDBInt1Binding,TDBBinding)
RTTIDecls(TDBInt2Binding,TDBBinding)
RTTIDecls(TDBInt4Binding,TDBBinding)
RTTIDecls(TDBInt8Binding,TDBBinding)
RTTIDecls(TDBTimeBinding,TDBBinding)
RTTIDecls(TDBStrBinding,TDBBinding)


// ---------------------------------------------------------------------------
//  CLASS: TDBBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBBinding: Destructor
// ---------------------------------------------------------------------------
TDBBinding::~TDBBinding()
{
    // If per-platform info was allocated, then let the platform code free it
    if (m_pPlatInfo)
    {
        FreeBindingInfo(m_pPlatInfo);
        m_pPlatInfo = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TDBBinding: Hidden constructors
// ---------------------------------------------------------------------------
TDBBinding::TDBBinding( const   TString&                strName
                        , const tCIDDBase::ESQLTypes    eType
                        , const tCIDDBase::EParmDirs    eDir) :

    m_bIsNull(kCIDLib::False)
    , m_eDir(eDir)
    , m_eType(eType)
    , m_pPlatInfo(nullptr)
    , m_strName(strName)
{
    // Ask the per-platform code to allocate any binding info it needs
    m_pPlatInfo = pAllocBindingInfo();
}




// ---------------------------------------------------------------------------
//  CLASS: TDBBoolBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBBoolBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBBoolBinding::TDBBoolBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c1Storage(0)
{
}

TDBBoolBinding::~TDBBoolBinding()
{
}


// ---------------------------------------------------------------------------
//  TDBBoolBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBBoolBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_c1Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBBoolBinding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBBoolBinding::bValue() const
{
    return (m_c1Storage != 0);
}



// ---------------------------------------------------------------------------
//  CLASS: TDBCard1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard1Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard1Binding::TDBCard1Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c1Storage(0)
{
}

TDBCard1Binding::~TDBCard1Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBCard1Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_c1Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBCard1Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1 TDBCard1Binding::c1Value() const
{
    return m_c1Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBCard2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard2Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard2Binding::TDBCard2Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c2Storage(0)
{
}

TDBCard2Binding::~TDBCard2Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBCard2Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_c2Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBCard2Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TDBCard2Binding::c2Value() const
{
    return m_c2Storage;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBCard4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard4Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard4Binding::TDBCard4Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c4Storage(0)
{
}

TDBCard4Binding::~TDBCard4Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBCard4Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_c4Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TDBCard4Binding::c4Value() const
{
    return m_c4Storage;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBCard8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBCard8Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBCard8Binding::TDBCard8Binding(const  TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c8Storage(0)
{
}

TDBCard8Binding::~TDBCard8Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBCard8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBCard8Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_c8Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBCard8Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TDBCard8Binding::c8Value() const
{
    return m_c8Storage;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBDateBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBDateBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBDateBinding::TDBDateBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(nullptr)
{
    InitStorage();
}

TDBDateBinding::TDBDateBinding( const   TString&                strName
                                , const TString&                strDefFormat
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(nullptr)
    , m_strDefFormat(strDefFormat)
{
    InitStorage();
}

TDBDateBinding::~TDBDateBinding()
{
    DeleteStorage();
}


// ---------------------------------------------------------------------------
//  TDBDateBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBDateBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        NullStorage();
}





// ---------------------------------------------------------------------------
//  CLASS: TDBInt1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt1Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt1Binding::TDBInt1Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i1Storage(0)
{
}

TDBInt1Binding::~TDBInt1Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt1Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBInt1Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_i1Storage = 0;
}


// ---------------------------------------------------------------------------
//  TDBInt1Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt1 TDBInt1Binding::i1Value() const
{
    return m_i1Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBInt2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt2Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt2Binding::TDBInt2Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i2Storage(0)
{
}

TDBInt2Binding::~TDBInt2Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt2Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBInt2Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_i2Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt2Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt2 TDBInt2Binding::i2Value() const
{
    return m_i2Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBInt4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt4Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt4Binding::TDBInt4Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i4Storage(0)
{
}

TDBInt4Binding::~TDBInt4Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt4Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBInt4Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_i4Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TDBInt4Binding::i4Value() const
{
    return m_i4Storage;
}




// ---------------------------------------------------------------------------
//  CLASS: TDBInt8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBInt8Binding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBInt8Binding::TDBInt8Binding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_i8Storage(0)
{
}

TDBInt8Binding::~TDBInt8Binding()
{
}


// ---------------------------------------------------------------------------
//  TDBInt8Binding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBInt8Binding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        m_i8Storage = 0;
}



// ---------------------------------------------------------------------------
//  TDBInt8Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt8 TDBInt8Binding::i8Value() const
{
    return m_i8Storage;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBStrBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBStrBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBStrBinding::TDBStrBinding(const  TString&                strName
                            , const tCIDLib::TCard4         c4MaxChars
                            , const tCIDDBase::ESQLTypes    eType
                            , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_c4MaxChars(c4MaxChars)
    , m_pStorage(nullptr)
{
    // Make sure the max chars isn't zero
    if (!m_c4MaxChars)
        m_c4MaxChars = 8;

    InitStorage();
}

TDBStrBinding::~TDBStrBinding()
{
    // Clean up the storage
    DeleteStorage();
}


// ---------------------------------------------------------------------------
//  TDBStrBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBStrBinding::PostFetch()
{
    // Store out data to the string member, or clear if it null data
    if (bIsNull())
        m_strValue.Clear();
    else
        StorageToStr();
}



// ---------------------------------------------------------------------------
//  TDBCard4Binding: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TDBStrBinding::strValue() const
{
    return m_strValue;
}



// ---------------------------------------------------------------------------
//  CLASS: TDBTimeBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBTimeBinding: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBTimeBinding::TDBTimeBinding( const   TString&                strName
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(nullptr)
{
    InitStorage();
}

TDBTimeBinding::TDBTimeBinding( const   TString&                strName
                                , const TString&                strDefFormat
                                , const tCIDDBase::ESQLTypes    eType
                                , const tCIDDBase::EParmDirs    eDir) :

    TDBBinding(strName, eType, eDir)
    , m_pStorage(nullptr)
    , m_strDefFormat(strDefFormat)
{
    InitStorage();
}

TDBTimeBinding::~TDBTimeBinding()
{
    DeleteStorage();
}


// ---------------------------------------------------------------------------
//  TDBTimeBinding: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBTimeBinding::PostFetch()
{
    // If we got a null value, then just set our value to zero
    if (bIsNull())
        NullStorage();
}

