//
// FILE NAME: CIDOrbUC_CfgServerClient.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDOrbUC_CfgServerClient.cpp file, which
//  implements the TCfgServerClient class. The configuration server uses an
//  ORB based interface. However, it requires a little magic on the client
//  end, so we don't expose it directly. This class wraps the config server's
//  client proxy class, and handles doing the work required to make the
//  interface work.
//
//  Config servers can be on multiple hosts. The default ctor gets you to
//  the one on the local host, and another ctor takes name server scope that
//  points to the binding of a config server to connect to.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCfgServerClient
//  PREFIX: cfgsc
// ---------------------------------------------------------------------------
class CIDORBUCEXP TCfgServerClient : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCfgServerClient
        (
            const   tCIDLib::TCard4         c4ConnWait = 2000
        );

        TCfgServerClient
        (
            const   TString&                strNSScope
            , const tCIDLib::TCard4         c4ConnWait = 2000
        );

        TCfgServerClient(const TCfgServerClient&) = delete;

        ~TCfgServerClient();


        // -------------------------------------------------------------------
        //  Pubic operators
        // -------------------------------------------------------------------
        TCfgServerClient& operator=(const TCfgServerClient&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddEncryptedScope
        (
            const   TString&                strToAdd
        );

        tCIDLib::TVoid AddObject
        (
            const   TString&                strKey
            , const MStreamable&            strmblToAdd
            , const tCIDLib::TCard4         c4Reserve
        );

        tCIDLib::TBoolean bAddOrUpdate
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            , const MStreamable&            strmblToAdd
            , const tCIDLib::TCard4         c4Reserve
        );

        tCIDLib::TBoolean bAllObjectsUnder
        (
            const   TString&                strStartScope
            ,       tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TBoolean bDeleteObjectIfExists
        (
            const   TString&                strKey
        );

        tCIDLib::TBoolean bFindNameUnder
        (
            const   TString&                strName
            , const TString&                strStartScope
            ,       tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TBoolean bIsConnected();

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
        );

        tCIDLib::TBoolean bReadObject
        (
            const   TString&                strKey
            ,       MStreamable&            strmblToFill
            ,       tCIDLib::TCard4&        c4Version
        );

        tCIDLib::ELoadRes eReadObject
        (
            const   TString&                strKey
            ,       MStreamable&            strmblToFill
            ,       tCIDLib::TCard4&        c4Version
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TCard4 c4QueryObjectsInScope
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TCard4 c4QuerySubScopes
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TCard4 c4UpdateObject
        (
            const   TString&                strKey
            , const MStreamable&            strmblToUpdate
        );

        tCIDLib::TVoid DeleteObject
        (
            const   TString&                strKey
        );

        tCIDLib::TVoid DeleteScope
        (
            const   TString&                strScope
        );

        tCIDLib::TVoid DropProxy();

        tCIDLib::TVoid SetEncrypter
        (
                    TBlockEncrypter* const  pcrypToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEncryptedScope
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TVoid CheckLostProxy
        (
            const   TError&                 errToCheck
        );

        TCIDCfgSrvClientProxy& orbcProxy();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ConnWait
        //      How long to wait to get connected initially or if we lose
        //      the connection and need to reconnect.
        //
        //  m_colEncryptedScopes
        //      If an encrypter is installed, we don't encrypt everything,
        //      we just encrypt things that are under the scopes in this
        //      list.
        //
        //  m_mbufIO
        //  m_mbufIO2
        //      We actually passed pre-flattened objects across the ORB
        //      interface, because we have to accept any streamable object,
        //      and you can't have a polymorphic output object. So this is
        //      our temp buffer that we use. The m_strmIO stream is wrapped
        //      around this stream.
        //
        //      We also support encrypted objects, which requires that we have
        //      an extra data to encrypt to and decrypt from.
        //
        //  m_pcrypToUse
        //      We can have an optional encrypter installed on us. We adopt
        //      it.
        //
        //  m_porbcProxy
        //      Our client proxy object. We initialize it upon first use,
        //      and will re-create if if we lose the connection and need to
        //      re-connect to the server.
        //
        //  m_strmOut
        //  m_strmIn
        //      We actually passed pre-flattened objects across the ORB
        //      interface, because we have to accept any streamable object,
        //      and you can't have a polymorphic output object. So these are
        //      our streams that we use to do the flattening on the way out and
        //      resurrecting on the way in. These are wrapped around the
        //      m_mbufIO buffer, which is then used for the actual reading
        //      and writing.
        //
        //      They must be in this order for proper construction, since
        //      the input stream must reference the already constructed input
        //      stream.
        //
        //  m_strNSScope
        //      THe name server scope of the ORB object we want to connect
        //      to.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ConnWait;
        tCIDLib::TStrList       m_colEncryptedScopes;
        THeapBuf                m_mbufIO;
        THeapBuf                m_mbufIO2;
        TBlockEncrypter*        m_pcrypToUse;
        TCIDCfgSrvClientProxy*  m_porbcProxy;
        TBinMBufOutStream       m_strmOut;
        TBinMBufInStream        m_strmIn;
        TString                 m_strNSScope;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCfgServerClient,TObject)
};

#pragma CIDLIB_POPPACK


