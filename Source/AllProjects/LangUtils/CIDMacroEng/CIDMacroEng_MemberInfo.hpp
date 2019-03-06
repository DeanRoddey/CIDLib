//
// FILE NAME: CIDMacroEng_MemberInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_MemberInfo.cpp file, which
//  implements the TMEngMemberInfo class. When a class declares a member,
//  we have to store some information about it in the class object, to keep
//  up with the name of the member, it's class type, and some attributes of
//  it.
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
//  CLASS: TMEngMemberInfo
// PREFIX: memi
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngMemberInfo : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TMEngMemberInfo& meviSrc);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMemberInfo();

        TMEngMemberInfo
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngMemberInfo
        (
            const   TMEngMemberInfo&        memiToCopy
        );

        ~TMEngMemberInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMemberInfo& operator=
        (
            const   TMEngMemberInfo&        memiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConst() const;

        tCIDLib::TCard2 c2ClassId() const;

        tCIDMacroEng::EConstTypes eConst() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ClassId
        //      The id of the class of this variable.
        //
        //  m_eConst
        //      Indicates whether this is a variable or constant. They are
        //      the same except for the ability to modify them, so we use the
        //      same class for both. This is set in the ctor and cannot be
        //      changed.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2ClassId;
        tCIDMacroEng::EConstTypes  m_eConst;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMemberInfo,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK


