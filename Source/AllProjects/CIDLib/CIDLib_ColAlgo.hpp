//
// FILE NAME: CIDLib_ColAlgo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/22/2019
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
//  Although we in no way want to get crazy like the C++ STL stuff, there are
//  some basic 'collection algorithms' we can do as templates that show up an
//  awful lot, and so it's worth providing generic ones where performance is
//  not particularly important.
//
//  Obviously these guys have to work in terms of cursors, so they will never be
//  as performant as just doing these things directly on the collections.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDColAlgo
{
    //
    //  Find the first element that matches the passed object to find. If found the
    //  cursor will be pointed at that element, else invalid. The default equality
    //  comparison functor will be used if not explicitly provided, which requires
    //  that the elements provide the equality operator.
    //
    template<typename TCol, typename TElem, typename TComp = tCIDLib::TDefEqComp<TElem>>
    typename TCol::TCursor cursFind(const   TCol&   colSrc
                                    , const TElem&  objToFind
                                    ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element that is not equal to the passed object to find. If
    //  found the cursor will be pointed at that element, else invalid. The default
    //  equality comparison functor will be used if not explicitly provided, which
    //  requires that the elements provide the equality operator.
    //
    template<typename TCol, typename TElem, typename TComp = tCIDLib::TDefEqComp<TElem>>
    typename TCol::TCursor cursFindNot( const   TCol&   colSrc
                                        , const TElem&  objToFind
                                        ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (!pfnComp(*cursSrc, objToFind))
                break;
        }
        return cursSrc;
    }


    //
    //  Find the first element greater or less than the passed object to compare. The
    //  default magnitude comparison functor is used if not indicated explicitly. It
    //  requires that the element type provide < and > operators.
    //
    template<typename TCol, typename TElem, typename TComp = tCIDLib::TDefMagComp<TElem>>
    typename TCol::TCursor cursFirstGreater(const   TCol&   colSrc
                                            , const TElem&  objComp
                                            ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstGreater)
                break;
        }
        return cursSrc;
    }

    template<typename TCol, typename TElem, typename TComp = tCIDLib::TDefMagComp<TElem>>
    typename TCol::TCursor cursFirstLess(const  TCol&   colSrc
                                        , const TElem&  objComp
                                        ,       TComp   pfnComp = TComp())
    {
        TCol::TCursor cursSrc(&colSrc);
        for (; cursSrc; ++cursSrc)
        {
            if (pfnComp(*cursSrc, objComp) == tCIDLib::ESortComps::FirstLess)
                break;
        }
        return cursSrc;
    }
}

#pragma CIDLIB_POPPACK
