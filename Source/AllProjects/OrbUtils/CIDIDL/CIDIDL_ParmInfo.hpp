//
// FILE NAME: CIDIDL_ParmInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/10/2004
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
//  This is the header for the CIDIDL_ParmInfo.cpp file. This file is used to
//  pass method parameter info into the code generator.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TCGenTypeInfo
//  PREFIX: tInfo
// ---------------------------------------------------------------------------
class TCGenTypeInfo : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCGenTypeInfo();

        TCGenTypeInfo
        (
            const   TXMLTreeElement&        xtnodeType
        );

        ~TCGenTypeInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strAuxType() const;

        const TString& strKeyExtract() const;

        const TString& strKeyOps() const;

        const TString& strKeyType() const;

        const TString& strModulus() const;

        const TString& strType() const;

        tCIDLib::TVoid Set
        (
            const   TXMLTreeElement&        xtnodeType
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strAuxType
        //      If it's any kind of templatized type, this is the type it is
        //      templatized over. If it's an object this is the class type.
        //      If its an enum, this is the actual enumerated type.
        //
        //  m_strKeyExtract
        //      If the type is a keyed type collection, the extracter attribute
        //      is stored here.
        //
        //  m_strKeyOps
        //  m_strKeyType
        //      Types that are keyed collections will have key ops and key
        //      type parameters
        //
        //  m_strModulus
        //      If its a hash type collection, then the modulus attribute is
        //      stored here.
        //
        //  m_strType
        //      The main type name.
        // -------------------------------------------------------------------
        TString m_strAuxType;
        TString m_strKeyExtract;
        TString m_strKeyOps;
        TString m_strKeyType;
        TString m_strModulus;
        TString m_strType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCGenTypeInfo,TObject)
};




// ---------------------------------------------------------------------------
//   CLASS: TCGenMethodParm
//  PREFIX: mparm
// ---------------------------------------------------------------------------
class TCGenMethodParm : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCGenMethodParm
        (
            const   TString&                strMethodName
            , const TXMLTreeElement&        xtnodeSrc
        );

        ~TCGenMethodParm();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EParmDirs eDir() const;

        const TString& strDefVal() const;

        const TString& strName() const;

        const TCGenTypeInfo& tinfoThis() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eDir
        //      The direction of the parameter
        //
        //  m_tinfoThis
        //      The type info for this parameter
        //
        //  m_strDefVal
        //      An optional default value for the parameter
        //
        //  m_strName
        //      The name of the parameter
        // -------------------------------------------------------------------
        tCIDLib::EParmDirs  m_eDir;
        TString             m_strDefVal;
        TString             m_strName;
        TCGenTypeInfo       m_tinfoThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCGenMethodParm,TObject)
};


