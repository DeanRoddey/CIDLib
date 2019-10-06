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
        tCIDLib::TBoolean bIsMoveableType() const
        {
            return m_bMoveableType;
        }

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
        //  m_bMoveable
        //      Set to true during parsing if this is one of the types that could
        //      support moves (though we don't always know for sure. If it's an Object
        //      or heap buffer, or one of the moveable collections we allow it, though
        //      it might fail during compile later.
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
        tCIDLib::TBoolean       m_bMoveableType;
        TString                 m_strAuxType;
        TString                 m_strKeyExtract;
        TString                 m_strKeyOps;
        TString                 m_strKeyType;
        TString                 m_strModulus;
        TString                 m_strType;

        static const TString    s_strElem_Bag;
        static const TString    s_strElem_ElemType;
        static const TString    s_strElem_Enumerated;
        static const TString    s_strElem_FundArray;
        static const TString    s_strElem_FundVector;
        static const TString    s_strElem_HashSet;
        static const TString    s_strElem_HeapBuf;
        static const TString    s_strElem_KeyExtract;
        static const TString    s_strElem_KeyedHashSet;
        static const TString    s_strElem_KeyOps;
        static const TString    s_strElem_KeyType;
        static const TString    s_strElem_MemBuf;
        static const TString    s_strElem_Modulus;
        static const TString    s_strElem_Object;
        static const TString    s_strElem_String;
        static const TString    s_strElem_Type;
        static const TString    s_strElem_Vector;


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
        tCIDLib::TBoolean bMoveable() const
        {
            return m_bMoveable;
        }

        tCIDLib::EParmDirs eDir() const
        {
            return m_eDir;
        }

        const TString& strDefVal() const;

        const TString& strName() const;

        const TCGenTypeInfo& tinfoThis() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bMovable
        //      Some can be marked moveable, which will use a move operation on
        //      the server side to pass the temp that is streamed into into the
        //      server side call.
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
        tCIDLib::TBoolean       m_bMoveable;
        tCIDLib::EParmDirs      m_eDir;
        TString                 m_strDefVal;
        TString                 m_strName;
        TCGenTypeInfo           m_tinfoThis;

        static const TString    s_strAttr_DefVal;
        static const TString    s_strAttr_Dir;
        static const TString    s_strAttr_Moveable;
        static const TString    s_strAttr_Name;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCGenMethodParm,TObject)
};


