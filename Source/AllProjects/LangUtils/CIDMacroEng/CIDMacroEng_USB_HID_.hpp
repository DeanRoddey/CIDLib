//
// FILE NAME: CIDMacroEng_USBHIDClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2005
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
//  This is the header file for the CIDMacroEng_USBHIDClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class, in this case a wrapper around the underlying C++ USB HID support.
//
//  It implements the MEng.System.Runtime.USBHID class.
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
//  CLASS: TMEngUSBHIDVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngUSBHIDVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngUSBHIDVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngUSBHIDVal(const TMEngUSBHIDVal&) = delete;

        ~TMEngUSBHIDVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngUSBHIDVal& operator=(const TMEngUSBHIDVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindDevPath
        (
            const   tCIDLib::TCard2         c2VendorId
            , const tCIDLib::TCard2         c2ProductId
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bReadBytes
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4ToRead
            , const tCIDLib::TCard4         c4WaitFor
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid Close();

        tCIDLib::TVoid Open
        (
            const   TString&                strDevPath
            , const tCIDLib::TCard2         c2VendorId
            , const tCIDLib::TCard2         c2ProductId
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid WriteBytes
        (
            const   TMemBuf&                mbufToSend
            , const tCIDLib::TCard4         c4ToSend
            , const tCIDLib::TCard4         c4WaitFor
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c2ProductId
        //  m_c2VendorId
        //      These are the ids of the device that this object represents,
        //      if it has been opened. Else they will be zero.
        //
        //  m_flDevice
        //      HID style USB is just done via file I/O in terms of reading
        //      and writing data.
        //
        //  m_evIO
        //      We never want to block so we use an async version of the
        //      I/O so that we can time out and cancel if necessary.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2ProductId;
        tCIDLib::TCard2 m_c2VendorId;
        TBinaryFile     m_flDevice;
        TEvent          m_evIO;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngUSBHIDVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngUSBHIDInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngUSBHIDInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngUSBHIDInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngUSBHIDInfo(const TMEngUSBHIDInfo&) = delete;

        ~TMEngUSBHIDInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngUSBHIDInfo& operator=(const TMEngUSBHIDInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We create some enum nested types, so we store the ids here
        //      for quick access later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrBadSet
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Close;
        tCIDLib::TCard2 m_c2MethId_FindDevice;
        tCIDLib::TCard2 m_c2MethId_Open;
        tCIDLib::TCard2 m_c2MethId_Read;
        tCIDLib::TCard2 m_c2MethId_Write;
        tCIDLib::TCard4 m_c4ErrCloseFailed;
        tCIDLib::TCard4 m_c4ErrDevNotFound;
        tCIDLib::TCard4 m_c4ErrOpenFailed;
        tCIDLib::TCard4 m_c4ErrReadFailed;
        tCIDLib::TCard4 m_c4ErrWriteFailed;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngUSBHIDInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


