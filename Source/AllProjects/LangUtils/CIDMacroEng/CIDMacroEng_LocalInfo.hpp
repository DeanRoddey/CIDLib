//
// FILE NAME: CIDMacroEng_LocalInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_LocalInfo.cpp file, which
//  implements the TMEngLocalInfo class. This class is what is stored in
//  a procedure object to describe any local variables that are either
//  declared within a procedure explictly, or generated implicitly by the
//  compiler to hold non-immediate literals. The local info objects are used
//  to push storage onto the stack upon entry to the procedure.
//
//  The explicit ones have names given to them by the user. They are not
//  namespace based, since they are implicitly in the procedure space. The
//  implicit ones are given arbitrary names that realy don't mean much. They
//  are always accessed via their indexes relative to the base pointer (the
//  top of stack on entry to the procedure.) The user's locals are also
//  referred to this way, but they have real names given to them by the user,
//  so they mean something to the user in error messages.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that our own local info id (used for indexed access) is set by
//      the class when we are installed on him. So we don't set it initially,
//      we just pass our name on to the underlying named item class. The
//      passed class id and path represent the type of the local, not our
//      local info id!
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TMEngLocalInfo
// PREFIX: meli
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngLocalInfo : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TMEngLocalInfo&         meliSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngLocalInfo();

        TMEngLocalInfo
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngLocalInfo
        (
            const   TMEngLocalInfo&         meliToCopy
        );

        ~TMEngLocalInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngLocalInfo& operator=
        (
            const   TMEngLocalInfo&         meliToAssign
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
        //      Indicates the class type of the local. This must jibe with
        //      the class path member.
        //
        //  m_eConst
        //      Indicates whether this local is const or not.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2ClassId;
        tCIDMacroEng::EConstTypes  m_eConst;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngLocalInfo,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK


