//
// FILE NAME: CIDRTP_SrcTar.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  We have to abstract the source of media data because it could be from various
//  sources and have various formats. This interface allows us to define just that
//  functionality we need in order to do our thing, and it's simple enough to wrap
//  around various types of sources.
//
// CAVEATS/GOTCHAS:
//
//  1)  The info returned from the initialize call is in terms of a single channel
//      for samples and in terms of all channels for bytes.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIO
// PREFIX: rtpio
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPIO : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TCIDRTPIO(const TCIDRTPIO&) = delete;

        ~TCIDRTPIO();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTPIO& operator=(const TCIDRTPIO&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bInitialized() const = 0;

        virtual tCIDLib::TVoid Terminate() = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDRTPIO();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTPIO,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOSrc
// PREFIX: rtpio
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPIOSrc : public TCIDRTPIO
{
    public  :
        // -------------------------------------------------------------------
        //  Public constructors and destructor
        // -------------------------------------------------------------------
        TCIDRTPIOSrc(const TCIDRTPIOSrc&) = delete;

        ~TCIDRTPIOSrc();


        // -------------------------------------------------------------------
        //  Public constructors and destructor
        // -------------------------------------------------------------------
        TCIDRTPIOSrc& operator=(const TCIDRTPIOSrc&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4ReadSamples
        (
                    TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4AtSample
            ,       tCIDLib::TCard4&        c4AtMillisec
        ) = 0;

        virtual tCIDLib::TVoid Initialize
        (
                    tCIDLib::TCard1&        c1PayloadType
            ,       tCIDLib::TCard4&        c4PayloadMSs
            ,       tCIDLib::TCard4&        c4PayloadBytes
            ,       tCIDLib::TCard4&        c4PayloadSamples
            ,       tCIDLib::TCard4&        c4SampleBytes
            ,       tCIDLib::TCard4&        c4TotalMSs
            ,       tCIDLib::TCard4&        c4TotalBytes
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDRTPIOSrc();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTPIOSrc,TCIDRTPIO)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOTar
// PREFIX: rtpio
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPIOTar : public TCIDRTPIO
{
    public  :
        // -------------------------------------------------------------------
        //  Public constructors and destructor
        // -------------------------------------------------------------------
        TCIDRTPIOTar(const TCIDRTPIOTar&) = delete;

        ~TCIDRTPIOTar();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTPIOTar& operator=(const TCIDRTPIOTar&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid StoreSamples
        (
            const   tCIDLib::TCard4         c4Millis
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        ) = 0;

        virtual tCIDLib::TVoid Initialize() = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDRTPIOTar();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTPIOTar,TCIDRTPIO)
};


// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOJan
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPIOJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTPIOJan();

        TCIDRTPIOJan
        (
                    TCIDRTPIO* const        prtpioToSanitize
        );

        TCIDRTPIOJan(const TCIDRTPIOJan&) = delete;

        ~TCIDRTPIOJan();


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TCIDRTPIOJan& operator=(const TCIDRTPIOJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCIDRTPIO& rtpioData() const;

        TCIDRTPIO& rtpioData();

        TCIDRTPIO* prtpioOrphan();

        tCIDLib::TVoid Orphan();

        tCIDLib::TVoid SetNew
        (
                    TCIDRTPIO* const        prtpioToSanitize
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CleanupSource();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_prdioToSanitize
        //      A pointer to the I/O object to sanitize.
        // -------------------------------------------------------------------
        TCIDRTPIO*  m_prtpioToSanitize;
};

#pragma CIDLIB_POPPACK


