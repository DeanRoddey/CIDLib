//
// FILE NAME: CIDSock_IPAddr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/06/1998
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
//  This is the header for the CIDSock_IPAddr.cpp file. This file implements
//  the TIPAddress class, which encapsulates IP addresses and allows them to
//  be manipulated very easily.
//
//  Also implemented here is TIPEndPoint, which derives from TIPAddress.
//  It adds to the base class the concept of a port number.
//
// CAVEATS/GOTCHAS:
//
//  1)  The host name is not included in the comparison between ip addresses
//      (and by extension the end points that derive from them.) Only the
//      binary address and the port number count. Depending on network
//      configuration, different names might point to the same machine and
//      we only care about what it points to.
//
//      So the host name can never in the future be included in such a
//      comparison, because other code depends on it not being.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TIPAddress
//  PREFIX: ipa
// ---------------------------------------------------------------------------
class CIDSOCKEXP TIPAddress :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TIPAddress();

        TIPAddress
        (
            const   TString&                strIPAddressString
            , const tCIDSock::EAddrTypes    eType
        );

        TIPAddress
        (
            const   TKrnlIPAddr&            kipaToSet
        );

        TIPAddress
        (
            const   TString&                strHostName
            , const TKrnlIPAddr&            kipaToSet
        );

        TIPAddress
        (
            const   tCIDSock::ESpecAddrs    eAddr
            , const tCIDSock::EAddrTypes    eType
        );

        TIPAddress(const TIPAddress& ipaSrc) :

            m_kipaThis(ipaSrc.m_kipaThis)
            , m_strHostName(ipaSrc.m_strHostName)
            , m_strTextVersion(ipaSrc.m_strTextVersion)
        {
        }

        TIPAddress(TIPAddress&& ipaSrc)
        {
            *this = tCIDLib::ForceMove(ipaSrc);
        }

        ~TIPAddress();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIPAddress& operator=(const TIPAddress& ipaSrc)
        {
            if (&ipaSrc != this)
            {
                m_kipaThis = ipaSrc.m_kipaThis;
                m_strHostName = ipaSrc.m_strHostName;
                m_strTextVersion = ipaSrc.m_strTextVersion;
            }
            return *this;
        }

        TIPAddress& operator=(TIPAddress&& ipaSrc)
        {
            if (&ipaSrc != this)
            {
                tCIDLib::Swap(m_kipaThis, ipaSrc.m_kipaThis);
                m_strHostName = tCIDLib::ForceMove(ipaSrc.m_strHostName);
                m_strTextVersion = tCIDLib::ForceMove(ipaSrc.m_strTextVersion);
            }
            return *this;
        }

        TIPAddress& operator=
        (
            const   TString&                strAddrToAssign
        );

        TIPAddress& operator=
        (
            const   TKrnlIPAddr&            kipaToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TIPAddress&             ipaToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TIPAddress&             ipaToCompare
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TKrnlIPAddr&            kipaToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKrnlIPAddr&            kipaToCompare
        )   const;

        tCIDLib::TCard1 operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsLoopback() const;

        tCIDSock::EAddrTypes eAddrType() const;

        tCIDLib::TVoid ForceNameResolution
        (
            const   tCIDSock::EAddrTypes    eType
        );

        const TKrnlIPAddr& kipaThis() const;

        const TString& strAsText() const;

        const TString& strFromText
        (
            const   TString&                strToSet
            , const tCIDSock::EAddrTypes    eType
        );

        const TString& strHostName
        (
            const   tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetAddr
        (
            const   TKrnlIPAddr&            lipaToSet
        );

        tCIDLib::TVoid SetAddr
        (
            const   TString&                strIPAddress
            , const tCIDSock::EAddrTypes    eType
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kipaThis
        //      This is the actual IP address stored as a kernel IP address
        //      object. This guy abstracts the actual format of the address
        //      from us, so it can be V4 or V6 and we don't much care.
        //
        //  m_strHostName
        //      If we are set by address (either a numeric address, or via a
        //      string which is in the form of a dotted address), we'll try to
        //      do a reverse lookup on the address and get a host name. If
        //      we cannot, then we'll just store the dotted text address.
        //
        //      We will fault this in upon first use if it needs to be done
        //      via reverse lookup (by clearing this string, and when strName()
        //      is called it'll know it needs to try to fault it in.) If we
        //      get a non-numeric address to begin with, we just store that
        //      up front. That's why this guy is mutable.
        //
        //      NOTE: This is NOT included in equality tests and can never
        //            be! Only the binary address matters.
        //
        //  m_strTextVersion
        //      In order to avoid converting the address to text over and
        //      over, its done once and kept around here. This is the text
        //      version of the numerical dotted address. We clear this if the
        //      address data is changed, so it can be faulted back in again
        //      later if needed.
        // -------------------------------------------------------------------
        TKrnlIPAddr         m_kipaThis;
        mutable TString     m_strHostName;
        TString             m_strTextVersion;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TIPAddress,TObject)
        DefPolyDup(TIPAddress)
        NulObject(TIPAddress)
};



// ---------------------------------------------------------------------------
//   CLASS: TIPEndPoint
//  PREFIX: ipep
// ---------------------------------------------------------------------------
class CIDSOCKEXP TIPEndPoint : public TIPAddress
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TIPEndPoint();

        TIPEndPoint
        (
            const   TString&                strIPAddressString
            , const tCIDLib::TIPPortNum     ippnThis
            , const tCIDSock::EAddrTypes    eType
        );

        TIPEndPoint
        (
            const   TString&                strEncodedEndPoint
            , const tCIDSock::EAddrTypes    eType
        );

        TIPEndPoint
        (
            const   TKrnlIPAddr&            kipaThis
            , const tCIDLib::TIPPortNum     ippnThis
        );

        TIPEndPoint
        (
            const   TIPAddress&             ipaThis
            , const tCIDLib::TIPPortNum     ippnThis
        );

        TIPEndPoint
        (
            const   tCIDSock::ESpecAddrs    eAddr
            , const tCIDSock::EAddrTypes    eType
            , const tCIDLib::TIPPortNum     ippnThis
        );

        TIPEndPoint
        (
            const   TString&                strHostName
            , const TKrnlIPAddr&            kipaThis
            , const tCIDLib::TIPPortNum     ippnThis
        );

        TIPEndPoint(const TIPEndPoint&) = default;
        TIPEndPoint(TIPEndPoint&&) = default;

        ~TIPEndPoint();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIPEndPoint& operator=(const TIPEndPoint&) = default;
        TIPEndPoint& operator=(TIPEndPoint&&) = default;

        TIPEndPoint& operator=
        (
            const   TString&                strEncodedEndPoint
        );

        tCIDLib::TBoolean operator==
        (
            const   TIPEndPoint&            ipepToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TIPEndPoint&            ipepToCompare
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TIPAddress&             ipaToCheck
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TIPAddress&             ipaToCheck
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatFancy
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid FormatPlain
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::THashVal hshEndPoint
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;

        tCIDLib::TIPPortNum ippnThis() const;

        tCIDLib::TIPPortNum ippnThis
        (
            const   tCIDLib::TIPPortNum     ippnThis
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FromEncodedStr
        (
            const   TString&                strEncodedEndPoint
            , const tCIDSock::EAddrTypes    eType
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ippnThis
        //      This is the number of the port that this end point represents.
        // -------------------------------------------------------------------
        tCIDLib::TIPPortNum m_ippnThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TIPEndPoint,TIPAddress)
        DefPolyDup(TIPEndPoint)
        NulObject(TIPEndPoint)
};

#pragma CIDLIB_POPPACK



