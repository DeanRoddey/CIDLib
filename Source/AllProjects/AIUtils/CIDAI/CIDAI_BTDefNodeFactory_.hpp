//
// FILE NAME: CIDAI_BTDefNodeFactory_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
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
//  This is the header for the default BT node factory derivative. This is an internal
//  factory that is always just installed, and which knows how to create the node
//  types defined here.
//
// CAVEATS/GOTCHAS:
//
//  1)  The root node isn't created via the factory, so we don't handle that here.
//
// LOG:
//
//  $_CIDLib_Log_$
//

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTDefNodeFact
//  PREFIX: nfact
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTDefNodeFact : public TAIBTNodeFact
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTDefNodeFact();

        TAIBTDefNodeFact(const TAIBTDefNodeFact& ctxvSrc) = delete;

        ~TAIBTDefNodeFact();


        // -------------------------------------------------------------------
        //  Public constructors
        // -------------------------------------------------------------------
        TAIBTDefNodeFact& operator=(const TAIBTDefNodeFact&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        TAIBTNode* pbtnodeNew
        (
            const   TString&                strPath
            , const TString&                strName
            , const TString&                strType
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTDefNodeFact, TAIBTNodeFact)
};

#pragma CIDLIB_POPPACK

