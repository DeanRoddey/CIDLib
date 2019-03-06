//
// FILE NAME: CIDMacroDbg_StateInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/19/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the file CIDMacroDbg_StateInfo.cpp, which implements
//  a small class that we can give back to the outside world. It holds our
//  current client window position info, so that we can put every thing back
//  to where it was last time.
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
//   CLASS: TMacroDbgStateInfo
//  PREFIX: sti
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TMacroDbgStateInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMacroDbgStateInfo();

        TMacroDbgStateInfo
        (
            const   TMacroDbgStateInfo&     stiToCopy
        );

        ~TMacroDbgStateInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMacroDbgStateInfo& operator=
        (
            const   TMacroDbgStateInfo&     stiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetDefaults();


        // -------------------------------------------------------------------
        //  Private data members
        //
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMacroDbgStateInfo,TObject)
};

#pragma CIDLIB_POPPACK


