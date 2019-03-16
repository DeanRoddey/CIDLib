//
// FILE NAME: CIDAI_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This is the header for the facility class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TFacCIDAI
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDAIEXP TFacCIDAI : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDAI();

        TFacCIDAI(const TFacCIDAI&) = delete;

        ~TFacCIDAI();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDAI& operator=(const TFacCIDAI&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TAIBTNodeFact& nfactFind
        (
            const   TString&                strFactType
        );

        tCIDLib::TVoid RegisterFactory
        (
                    TAIBTNodeFact* const    pnfactToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colFactList
        //      This is where we store the registered behavior tree factories. They
        //      are keyed on the image factory, so that we can quickly find the factory
        //      that we need to create a requested node.
        // -------------------------------------------------------------------
        TRefKeyedHashSet<TAIBTNodeFact, TString, TStringKeyOps> m_colFactList;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDAI,TFacility)
};

#pragma CIDLIB_POPPACK
