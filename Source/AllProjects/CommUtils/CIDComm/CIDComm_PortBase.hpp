//
// FILE NAME: CIDComm_PortBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/23/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDComm_PortBase.cpp file, which implements
//  the TCommPortBase class. Each TCommPortBase object represents a virtual
//  comm port. We need this abstraction because there are various types of
//  remote com ports (usually over ethernet or USB) some of which do not
//  have drivers to make them look like local ports. So in those cases we
//  have to abstract them ourself.
//
//  The TCommPort derived class represents the standard local port.
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
//  CLASS: TCommPortBase
// PREFIX: comm
// ---------------------------------------------------------------------------
class CIDCOMMEXP TCommPortBase : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCommPortBase(const TCommPortBase&) = delete;

        ~TCommPortBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCommPortBase& operator=(const TCommPortBase&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsOpen() const = 0;

        virtual tCIDLib::TBoolean bQueryCTSOn() const = 0;

        virtual tCIDLib::TBoolean bQueryDSROn() const = 0;

        virtual tCIDLib::TCard4 c4ReadRawBufMS
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TCard4         c4Wait
        ) = 0;

        virtual tCIDLib::TCard4 c4ReadRawBuf
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctWait
        ) = 0;

        virtual tCIDLib::TCard4 c4WriteRawBufMS
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4Wait
        ) = 0;

        virtual tCIDLib::TVoid Close() = 0;

        virtual tCIDLib::TVoid Cycle
        (
            const   tCIDComm::EOpenFlags    eFlags = tCIDComm::EOpenFlags::None
            , const tCIDLib::TCard4         c4ReadBufSz = 0
            , const tCIDLib::TCard4         c4WriteBufSz = 0
        ) = 0;

        virtual tCIDComm::EFlags eFlags() const = 0;

        virtual tCIDLib::TVoid FlushWriteBuffer() = 0;

        virtual tCIDLib::TVoid Open
        (
            const   tCIDComm::EOpenFlags    eFlags = tCIDComm::EOpenFlags::None
            , const tCIDLib::TCard4         c4ReadBufSz = 0
            , const tCIDLib::TCard4         c4WriteBufSz = 0
        ) = 0;

        virtual tCIDLib::TVoid PurgeReadData() = 0;

        virtual tCIDLib::TVoid PurgeWriteData() = 0;

        virtual tCIDLib::TVoid QueryCfg
        (
                    TCommPortCfg&           cpcfgToFill
        )   const = 0;

        virtual tCIDLib::TVoid QueryLineStates
        (
                    tCIDLib::TBoolean&      bCTSOn
            ,       tCIDLib::TBoolean&      bDSROn
            ,       tCIDLib::TBoolean&      bRingOn
            ,       tCIDLib::TBoolean&      bRLSDOn
        )   const = 0;

        virtual tCIDLib::TVoid QueryXChars
        (
                    tCIDLib::TSCh&          schOn
            ,       tCIDLib::TSCh&          schOff
        )   const = 0;

        virtual tCIDLib::TVoid Reset() = 0;

        virtual tCIDLib::TVoid SetBaudRate
        (
            const   tCIDLib::TCard4         c4ToSet
        ) = 0;

        virtual tCIDLib::TVoid SetBreak
        (
            const   tCIDLib::TBoolean       bState
        ) = 0;

        virtual tCIDLib::TVoid SetDataBits
        (
            const   tCIDComm::EDataBits     eToSet
        ) = 0;

        virtual tCIDLib::TVoid SetDTR
        (
            const   tCIDComm::EPortDTR      eToSet
        ) = 0;

        virtual tCIDLib::TVoid SetParity
        (
            const   tCIDComm::EParities     eToSet
        ) = 0;

        virtual tCIDLib::TVoid SetRTS
        (
            const   tCIDComm::EPortRTS      eToSet
        ) = 0;

        virtual tCIDLib::TVoid SetStopBits
        (
            const   tCIDComm::EStopBits     eToSet
        ) = 0;

        virtual tCIDLib::TVoid SetCfg
        (
            const   TCommPortCfg&           cpcfgToSet
        ) = 0;

        virtual tCIDLib::TVoid SetEOFChar
        (
            const   tCIDLib::TSCh           schToSet
        ) = 0;

        virtual tCIDLib::TVoid SetFlags
        (
            const   tCIDComm::EFlags        eToSet
            , const tCIDComm::EFlags        eMask
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadMBufMS
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TCard4         c4Wait
        );

        tCIDLib::TCard4 c4ReadMBuf
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctWait
        );

        tCIDLib::TCard4 c4WriteMBufMS
        (
            const   TMemBuf&                mbufToWrite
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4Wait
        );

        const TString& strPath() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TFacCIDComm;


        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TCommPortBase();

        TCommPortBase
        (
            const   TString&                strPath
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strPath
        //      This is set on us by the facility class, which creates the
        //      ports via factories. This is the path that uniquely identifies
        //      the port at the abstract level. It is only meaningful to the
        //      factory that created it.
        // -------------------------------------------------------------------
        TString m_strPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCommPortBase,TObject)
};

#pragma CIDLIB_POPPACK


