//
// FILE NAME: TestOrb3.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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


// ---------------------------------------------------------------------------
//  Include our own headers in the appropriate order
// ---------------------------------------------------------------------------
#include    "TestOrb3_ErrorIds.hpp"
#include    "TestOrb3_MessageIds.hpp"
#include    "TestOrb3_Types.hpp"
#include    "TestOrb3_IntfClientProxy.hpp"
#include    "TestOrb3_IntfServerBase.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TFacTestOrb3
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacTestOrb3 : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacTestOrb3();

        ~TFacTestOrb3();


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
        TFacTestOrb3(const TFacTestOrb3&);
        tCIDLib::TVoid operator=(const TFacTestOrb3&);


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
        RTTIDefs(TFacTestOrb3,TFacility)
};


inline TOutConsole& TFacTestOrb3::conOut()
{
    return m_conOut;
}


// ---------------------------------------------------------------------------
//  Global data externs
// ---------------------------------------------------------------------------
extern TFacTestOrb3 facTestOrb3;

