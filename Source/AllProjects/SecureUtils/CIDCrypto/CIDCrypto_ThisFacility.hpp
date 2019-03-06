//
// FILE NAME: CIDCrypto_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDCrypto_ThisFacility.cpp file. This
//  file implements the facility class for this facility. An object of this
//  class is created and exported to the outside world to give them access to
//  this facility's global functionality and resources. The actual object is
//  lazily evaluated by way of an exported access method.
//
// CAVEATS/GOTCHAS:
//
//  1)  This one currently adds no extra functionality to its base class, but
//      it might in the future so we go ahead and derive our own class.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TRandomNum;

// ---------------------------------------------------------------------------
//  CLASS: TFacCIDCrypto
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDCRYPTEXP TFacCIDCrypto : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TFacCIDCrypto();

        TFacCIDCrypto(const TFacCIDCrypto&) = delete;

        ~TFacCIDCrypto();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDCrypto& operator=(const TFacCIDCrypto&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetRandom
        (
            const   tCIDLib::TBoolean       bNonZero = kCIDLib::True
        )   const;

        tCIDLib::TCard8 c8GetRandom
        (
            const   tCIDLib::TBoolean       bNonZero = kCIDLib::True
        )   const;

        tCIDLib::TVoid GenerateHMAC
        (
            const   TMemBuf&                mbufKey
            , const tCIDLib::TCard4         c4KeyBytes
            , const TMemBuf&                mbufMsg
            , const tCIDLib::TCard4         c4MsgBytes
            ,       THashDigest&            mdigToUse
            ,       TMsgHash&               mhashToFill
        );

        tCIDLib::TVoid GenerateHMAC
        (
            const   TString&                strKey
            , const TMemBuf&                mbufMsg
            , const tCIDLib::TCard4         c4MsgBytes
            ,       THashDigest&            mdigToUse
            ,       TMsgHash&               mhashToFill
        );

        tCIDLib::TVoid GetRandomBytes
        (
                    TMemBuf&                mbufTarget
            , const tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid GetRandomBytes
        (
                    tCIDLib::TCard1* const  pc1Target
            , const tCIDLib::TCard4         c4Count
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      We have to protect the random number generator against multiple thread
        //      usage which may well happen.
        //
        //  m_prandGen
        //      We have various needs for random numbers here, so we provide helpers
        //      methods to generate them. Make it a pointer since we can then avoid
        //      pushing CIDMath on anyone who uses this facility.
        // -------------------------------------------------------------------
        TMutex      m_mtxSync;
        TRandomNum* m_prandGen;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDCrypto,TFacility)
};

#pragma CIDLIB_POPPACK
