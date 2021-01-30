//
// FILE NAME: CIDOrb_Id.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/2001
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
//  This is the header for the CIDOrb_Id.cpp file, which implements the
//  TOrbId class. Each server side object has a unique id which is created from
//  a unique interface id, assigned by the IDL compiler for that class, and
//  a unique instance id that is generated for each new server side instance
//  created. This class represents the combination of those two ids.
//
//  This class mostly constructs from one string representation and one
//  binary representation. The stringized version is the interface id that
//  is hard coded into the IDL generated code. The instance id is usually
//  generated on the fly and so is usually in the form of a TMD5Hash object
//  from the unique id class in CIDCrypto. But, sometimes we have to take both
//  ids as strings.
//
//  Since these will be used in hashes a lot, we provide a key ops class for
/// Orb id objects. This is required to use them in a standard TKeyedHashSet
//  class. The id object pre-hashes the combined id values, to avoid having
//  to do it over and over.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TOrbId
//  PREFIX: oid
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbId : public TObject, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbId();

        TOrbId
        (
            const   TString&                strInterface
            , const TMD5Hash&               mhashInstance
        );

        TOrbId
        (
            const   TMD5Hash&               mhashInterface
            , const TMD5Hash&               mhashInstance
        );

        TOrbId
        (
            const   TString&                strInterface
            , const TString&                strInstance
        );

        TOrbId(const TOrbId&) = default;
        TOrbId(TOrbId&&) = default;

        ~TOrbId();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbId& operator=(const TOrbId&) = default;
        TOrbId& operator=(TOrbId&& oidSrc) = default;

        tCIDLib::TBoolean operator==
        (
            const   TOrbId&                 oidSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TOrbId&                 oidSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsInterfaceId
        (
            const   TString&                strToTest
        )   const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;

        [[nodiscard]] tCIDLib::THashVal hshThis() const;

        [[nodiscard]] const TMD5Hash& mhashInterface() const;

        [[nodiscard]] const TMD5Hash& mhashInstance() const;

        tCIDLib::TVoid Set
        (
            const   TString&                strInterface
            , const TMD5Hash&               mhashInstance
        );

        tCIDLib::TVoid Set
        (
            const   TMD5Hash&               mhashInterface
            , const TMD5Hash&               mhashInstance
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strInterface
            , const TString&                strInstance
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::THashVal hshDoHashCalc
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hshThis
        //      We pre-hash ourself in order to speed up comparisons and
        //      lookups.
        //
        //  m_mhashInterface
        //      The MD5 hash for the interface id
        //
        //  m_mhashInstance
        //      The mD5 hash for the instance id
        // -------------------------------------------------------------------
        tCIDLib::THashVal   m_hshThis;
        TMD5Hash            m_mhashInterface;
        TMD5Hash            m_mhashInstance;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbId,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TOrbIdKeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbIdKeyOps : public TObject, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbIdKeyOps();

        TOrbIdKeyOps(const TOrbIdKeyOps& kopsSrc);
        TOrbIdKeyOps(TOrbIdKeyOps&&) = delete;

        ~TOrbIdKeyOps();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbIdKeyOps& operator=(const TOrbIdKeyOps&);
        TOrbIdKeyOps& operator=(TOrbIdKeyOps&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompKeys
        (
            const   TOrbId&                 oid1
            , const TOrbId&                 oid2
        )   const;

        tCIDLib::THashVal hshKey
        (
            const   TOrbId&                 oidToHash
            , const tCIDLib::TCard4         c4Modulus
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbIdKeyOps, TObject)
        DefPolyDup(TOrbIdKeyOps)
};

#pragma CIDLIB_POPPACK

