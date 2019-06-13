//
// FILE NAME: CIDMData_UniqueStr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2019
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
//  This file implements a class performs a commonly used function that isn't
//  that hard but ends up involving a good bit of grunt work that can be avoided
//  by making this simple class.
//
//  It creates a keyed has set over a very simple structure that contains a string
//  and an id. It has a running id value that it uses to assign an id to each
//  unique string added. The caller asks to add a string, we see if it's already
//  in our list. If so, we return that id. Else we add it and return that new
//  id. Ids are 1 to whatever limit they set. Zero is not used. That means that
//  the max strings we can hold are limit-1.
//
//  We also keep a non-adopting ref vector that we point at each entry that is
//  added to the list. This provides us with a 'by id' view. Id-1 is the index
//  for that string into this list.
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
//  CLASS: TUniqueStr
// PREFIX: ustr
// ---------------------------------------------------------------------------
class CIDMDATAEXP TUniqueString
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUniqueString
        (
            const   tCIDLib::TCard4         c4MaxId = kCIDLib::c4MaxCard
            , const tCIDLib::TCard4         c4Modulus = 109
        );

        TUniqueString(const TUniqueString&) = delete;
        TUniqueString(TUniqueString&&) = delete;

        ~TUniqueString() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUniqueString& operator=(const TUniqueString&) = delete;
        TUniqueString& operator=( TUniqueString&&) = delete;

        const TString& operator[]
        (
            const   tCIDLib::TCard4         c4Id
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const tCIDLib::TBoolean bHasString
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bIsFull() const
        {
            return (m_c4NextId >= m_c4MaxId);
        }

        tCIDLib::TCard4 c4StringCount() const
        {
            return m_colUniqueList.c4ElemCount();
        }

        tCIDLib::TCard4 c4StringToId
        (
            const   TString&                strToFind
        )   const;

        const tCIDLib::TCard4 c4AddString
        (
            const   TString&                strToAdd
        );

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private types
        // -------------------------------------------------------------------
        struct TAString
        {
            TString         m_strVal;
            tCIDLib::TCard4 m_c4Id;
        };
        using TUStringSet = TKeyedHashSet<TAString, TString, TStringKeyOps>;
        using TIdView     = TRefVector<const TAString>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckId
        (
            const   tCIDLib::TCard4         c4ToCheck
        )   const;


        // -------------------------------------------------------------------
        //  Private members
        //
        //  m_c4MaxId
        //      The maximum id we can allow, provided by the ctor
        //
        //  m_c4NextId
        //      The next id we will assign
        //
        //  m_colUniqueList
        //  m_colURLIdView
        //      The hash table and by id view collections
        // -------------------------------------------------------------------
        const tCIDLib::TCard4   m_c4MaxId;
        tCIDLib::TCard4         m_c4NextId;
        TUStringSet             m_colUniqueList;
        TIdView                 m_colURLIdView;
};

#pragma CIDLIB_POPPACK
