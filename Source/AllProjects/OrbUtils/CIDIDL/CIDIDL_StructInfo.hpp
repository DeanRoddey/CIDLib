//
// FILE NAME: CIDIDL_StructInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDIDL_StructInfo.cpp file. This file is used to
//  hold structure definitions for later generation.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TStructMem
//  PREFIX: tInfo
// ---------------------------------------------------------------------------
class TStructMem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStructMem();

        TStructMem
        (
            const   TXMLTreeElement&        xtnodeType
        );

        ~TStructMem();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSubStruct() const;

        const TString& strName() const;

        const TString& strType() const;

        tCIDLib::TVoid Set
        (
            const   TXMLTreeElement&        xtnodeType
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsSubStruct
        //      If true, this is a sub-structure, and m_strType is the type of the
        //      previously defined structure, just the raw name, any language specific
        //      decorators will be added during generation.
        //
        //  m_strName
        //      The name of the structure, just the raw name, any language specific
        //      decorators will be added during generation.
        //
        //  m_strType
        //      The type of the member. For sub-structures is the type of a previously
        //      defined structure. For fundamental members it's the basic data type as
        //      defined in the DTD.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bIsSubStruct;
        TString                 m_strName;
        TString                 m_strType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStructMem,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TStructInfo
//  PREFIX: tInfo
// ---------------------------------------------------------------------------
class TStructInfo : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStructInfo();

        TStructInfo
        (
            const   TXMLTreeElement&        xtnodeType
        );

        ~TStructInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MemberCount() const;

        const TStructMem& memAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TString& strType() const;

        tCIDLib::TVoid Set
        (
            const   TXMLTreeElement&        xtnodeType
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colMembers
        //      This is a vector of TStructMem objects for the members of this structure.
        //      Each one can be either a fundamental type member, or a structure that has
        //      already been defined.
        //
        //  m_strType
        //      The type to give to this structure, just the raw name, any language specific
        //      decorators will be added during generation.
        // -------------------------------------------------------------------

        TVector<TStructMem>     m_colMembers;
        TString                 m_strType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStructInfo,TObject)
};




