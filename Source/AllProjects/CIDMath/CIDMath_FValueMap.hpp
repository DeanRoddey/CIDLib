//
// FILE NAME: CIDMath_FValueMap.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/1996
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
//  This header provides a simple interface for mapping input floating point
//  values to another value. These are intended for use where an algorithm
//  can support a pluggable mapping mechanism.
//
//  This class is a sibling to TValueMap, which defines a similar interface
//  for mapping one range of zero based unsigned indices to another. That
//  class is for mapping table indices, whereas this one maps actual floating
//  point data values.
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
//  CLASS: TFValueMap
// PREFIX: vmap
// ---------------------------------------------------------------------------
class CIDMATHEXP TFValueMap :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TFValueMap(const TFValueMap&) = delete;

        ~TFValueMap();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TFloat8 f8MapValue
        (
            const   tCIDLib::TFloat8        f8ToMap
        )   const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TFValueMap();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFValueMap,TObject)
};

#pragma CIDLIB_POPPACK


