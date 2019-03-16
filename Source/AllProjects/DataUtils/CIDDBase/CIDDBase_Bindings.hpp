//
// FILE NAME: CIDDBase_Bindings.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/22/2003
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
//  This is the header file for the CIDDBase_Bindings.Cpp file. This
//  file implements a set of binding classes, which are used in parameter
//  and column binding operations, and which hide the actual internal
//  database representations of data. The outside world just indicates
//  the expected SQL type (for parm bindings) of the field (the C++ type
//  is implied in the type of object created) and the parameter number
//  and the binding object will handle fielding the binding and can then
//  be queried for the value, which will be converted to the type returned
//  by the particular binding class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TDBStatement;


// ---------------------------------------------------------------------------
//  CLASS: TDBBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBBinding : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TDBBinding();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        ) = 0;

        virtual tCIDLib::TVoid PostFetch() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNull() const;

        tCIDDBase::ESQLTypes eType() const;

        const TString& strName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TDBBinding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType
            , const tCIDDBase::EParmDirs    eDir
        );

        TDBBinding
        (
            const   TDBBinding&             dbbindToCopy
        );

        tCIDLib::TVoid operator=
        (
            const   TDBBinding&             dbbindToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckBindResult
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TSInt          sToCheck
        );

        tCIDDBase::TSQLIndicator iIndicator() const;

        const tCIDDBase::TSQLIndicator* piIndicator() const;

        tCIDDBase::TSQLIndicator* piIndicator();


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_eDir
        //      Only used for parameter bindings, indicates the binding
        //      direction. It they will in except for a SQL procedure call
        //      parameter.
        //
        //  m_eType
        //      The SQL type of the parameter. This is only required for a
        //      parameter binding, and can be ESQLType_None for col bindings.
        //
        //  m_iIndicator
        //      Every binding needs one of these in case the data returned
        //      is null, which is indicated here. It's part of the binding
        //      of the column so it has to stay around as long the column is
        //      bound.
        //
        //  m_strName
        //      The name of the bindng, which is only for application use so
        //      that errors can use meaningful titles for bindings.
        // -------------------------------------------------------------------
        tCIDDBase::EParmDirs        m_eDir;
        tCIDDBase::ESQLTypes        m_eType;
        tCIDDBase::TSQLIndicator    m_iIndicator;
        TString                     m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBBinding,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TDBBoolBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBBoolBinding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBBoolBinding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBBoolBinding(const TDBBoolBinding&) = delete;

        ~TDBBoolBinding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBBoolBinding& operator=(const TDBBoolBinding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c1Storage
        //      The storage for our binding. SQL uses a BIT type for boolean
        //      values, but they are stored in a byte format.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_c1Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBBoolBinding,TDBBinding)
};


// ---------------------------------------------------------------------------
//  CLASS: TDBCard1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBCard1Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBCard1Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBCard1Binding(const TDBCard1Binding&) = delete;

        ~TDBCard1Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBCard1Binding& operator=(const TDBCard1Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard1 c1Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c1Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_c1Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBCard1Binding,TDBBinding)
};


// ---------------------------------------------------------------------------
//  CLASS: TDBCard2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBCard2Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBCard2Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBCard2Binding(const TDBCard2Binding&) = delete;

        ~TDBCard2Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBCard2Binding& operator=(const TDBCard2Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c2Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBCard2Binding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBCard4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBCard4Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBCard4Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBCard4Binding(const TDBCard4Binding&) = delete;

        ~TDBCard4Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBCard4Binding& operator=(const TDBCard4Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c4Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBCard4Binding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBCard8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBCard8Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBCard8Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBCard8Binding(const TDBCard8Binding&) = delete;

        ~TDBCard8Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBCard8Binding& operator=(const TDBCard8Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard8 c8Value() const;

        tCIDLib::TVoid PostFetch();


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c8Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TCard8 m_c8Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBCard8Binding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBDateBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBDateBinding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBDateBinding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBDateBinding
        (
            const   TString&                strName
            , const TString&                strFormat
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBDateBinding(const TDBDateBinding&) = delete;

        ~TDBDateBinding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBDateBinding& operator=(const TDBDateBinding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TTime& tmValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_pStorage
        //      The storage for our binding. It's opaque to the outside world
        //      since it's a SQL specific structure.
        //
        //  m_tmValue
        //      We provide a TTime object to the outside world. When they
        //      call tmValue() we load up the info into it, so it has to be
        //      mutable.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pStorage;
        mutable TTime   m_tmValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBDateBinding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBInt1Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBInt1Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBInt1Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBInt1Binding(const TDBInt1Binding&) = delete;

        ~TDBInt1Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBInt1Binding& operator=(const TDBInt1Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt1 i1Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_i1Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TInt1  m_i1Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBInt1Binding,TDBBinding)
};


// ---------------------------------------------------------------------------
//  CLASS: TDBInt2Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBInt2Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBInt2Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBInt2Binding(const TDBInt2Binding&) = delete;

        ~TDBInt2Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBInt2Binding& operator=(const TDBInt2Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt2 i2Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_i2Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TInt2  m_i2Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBInt2Binding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBInt4Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBInt4Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBInt4Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBInt4Binding(const TDBInt4Binding&) = delete;

        ~TDBInt4Binding();


        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TDBInt4Binding& operator=(const TDBInt4Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 i4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_i4Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TInt4  m_i4Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBInt4Binding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBInt8Binding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBInt8Binding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBInt8Binding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBInt8Binding(const TDBInt8Binding&) = delete;

        ~TDBInt8Binding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBInt8Binding& operator=(const TDBInt8Binding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt8 i8Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c8Storage
        //      The storage for our binding.
        // -------------------------------------------------------------------
        tCIDLib::TInt8  m_i8Storage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBInt8Binding,TDBBinding)
};




// ---------------------------------------------------------------------------
//  CLASS: TDBStrBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBStrBinding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBStrBinding
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4MaxChars
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBStrBinding(const TDBStrBinding&) = delete;

        ~TDBStrBinding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBStrBinding& operator=(const TDBStrBinding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c4MaxChars
        //  m_pchStorage
        //      The storage for our binding. The caller indicates the max
        //      chars he'll accept.
        //
        //  m_strValue
        //      We provide a string object to the outside world. When they
        //      call strValue() we load up the text into it, so it has to be
        //      mutable.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4MaxChars;
        tCIDLib::TCh*   m_pchStorage;
        mutable TString m_strValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBStrBinding,TDBBinding)
};



// ---------------------------------------------------------------------------
//  CLASS: TDBTimeBinding
// PREFIX: dbbind
// ---------------------------------------------------------------------------
class CIDDBASEEXP TDBTimeBinding : public TDBBinding
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDBTimeBinding
        (
            const   TString&                strName
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBTimeBinding
        (
            const   TString&                strName
            , const TString&                strFormat
            , const tCIDDBase::ESQLTypes    eType = tCIDDBase::ESQLTypes::None
            , const tCIDDBase::EParmDirs    eDir = tCIDDBase::EParmDirs::In
        );

        TDBTimeBinding(const TDBTimeBinding&) = delete;

        ~TDBTimeBinding();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDBTimeBinding& operator=(const TDBTimeBinding&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindCol
        (
                    TDBStatement&           dbstmtSrc
            , const tCIDLib::TCard2         c2ParmInd
        )   override;

        tCIDLib::TVoid PostFetch() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TTime& tmValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_pStorage
        //      The storage for our binding. It's opaque to the outside world
        //      since it's a SQL specific structure.
        //
        //  m_tmValue
        //      We provide a TTime object to the outside world. When they
        //      call tmValue() we load up the info into it, so it has to be
        //      mutable.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pStorage;
        mutable TTime   m_tmValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDBTimeBinding,TDBBinding)
};


#pragma CIDLIB_POPPACK


