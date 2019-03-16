//
// FILE NAME: TestOrb2.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrb.hpp"

#include    "CIDMath.hpp"


// ---------------------------------------------------------------------------
//  Include our own headers in the appropriate order
// ---------------------------------------------------------------------------
#include    "TestOrb2_ErrorIds.hpp"
#include    "TestOrb2_MessageIds.hpp"
#include    "TestOrb2_ClientProxy.hpp"
#include    "TestOrb2_ServerBase.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TFacTestOrb2
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacTestOrb2 : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacTestOrb2();

        ~TFacTestOrb2();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        TOutConsole& conOut();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacTestOrb2(const TFacTestOrb2&);
        tCIDLib::TVoid operator=(const TFacTestOrb2&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bOrbClientTest
        (
            const   tCIDLib::TIPPortNum     ippnSrv
        );

        tCIDLib::TBoolean bOrbServerTest();

        tCIDLib::TVoid ShowUsage();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_conIn
        //  m_conOut
        //      The in/output consoles we use. The input console is just used
        //      to wait for any key on the server side.
        // -------------------------------------------------------------------
        TInConsole      m_conIn;
        TOutConsole     m_conOut;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacTestOrb2,TFacility)
};


inline TOutConsole& TFacTestOrb2::conOut()
{
    return m_conOut;
}


// ---------------------------------------------------------------------------
//  Global data externs
// ---------------------------------------------------------------------------
extern TFacTestOrb2 facTestOrb2;

