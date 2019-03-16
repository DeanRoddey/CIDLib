//
// FILE NAME: CIDLib_KeyOps.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/24/1997
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
//  There are some collection types whose elements are keys, or whose elements
//  contain key fields. The collection must do particular operations on those
//  key fields. In order to avoid hard coding these operations in, and forcing
//  every instantiation type to do it exactly the same way, 'key ops' classes
//  are provided which are derived from and given to the collection to provide
//  these key operations. This is a little more hassle and makes for a little
//  more overhead, but provides massively more flexibility.
//
//  Some standard key ops classes are provided here because of their constant
//  use by everyone in the world.
//
//  We also provide some standard templatized implementations of the element
//  comparator functions that are used by some collections.
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
//   CLASS: TStringKeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringKeyOps : public TObject, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringKeyOps();

        explicit TStringKeyOps
        (
            const   tCIDLib::TBoolean       bCase
        );

        TStringKeyOps
        (
            const   TStringKeyOps&          kopsSrc
        );

        ~TStringKeyOps();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringKeyOps& operator=
        (
            const   TStringKeyOps&          kopsSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys
        (
            const   TString&                str1
            , const TString&                str2
        )   const;

        tCIDLib::THashVal hshKey
        (
            const   TString&                strToHash
            , const tCIDLib::TCard4         c4Modulus
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCase
        //      Normally we will do cache sensitive comparisons. But we can be told
        //      to do it insensitively.
        //
        //  m_strTmp
        //      If non-case sensitive, we have to upper case the keys before we do
        //      hashing. So this is a temp to avoid creating/destroying a string
        //      every time. It doesn't need to be copied or assigned, since it's
        //      purely for temp use. it has to be mutable because the hash method
        //      is const.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bCase;
        mutable TString     m_strTmp;


        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TStringKeyOps, TObject)
        DefPolyDup(TStringKeyOps)
};

#pragma CIDLIB_POPPACK


