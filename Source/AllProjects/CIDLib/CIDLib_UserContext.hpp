//
// FILE NAME: CIDLib_UserContext.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/27/2011
//
// COPYRIGHT: 2011..2011, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This file defines a simple base class that represents a user security
//  slash rights context. It doesn't itself define anything other than a
//  user name and really it's all in derived classes. But it allows CIDLib
//  frameworks to accept a user context that it can store and make available
//  to derived classes that need to pass through some sort of security
//  context.
//
//  Mostly this is for the use of CQC related stuff, which needs to use things
//  like the macro engine, but some of the CQC implemented CML classes need
//  to have access to CQC level user security info. So it can set a user
//  context on the macro engine and it can make them available to the classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDUserCtx
//  PREFIX: cuctx
// ---------------------------------------------------------------------------
class CIDLIBEXP TCIDUserCtx : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCIDUserCtx();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Reset();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strUserName() const;

        const TString& strUserType() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCIDUserCtx();

        TCIDUserCtx
        (
            const   TString&                strUserName
            , const TString&                strUserType
        );

        TCIDUserCtx
        (
            const   TCIDUserCtx&            cuctxToCopy
        );

        TCIDUserCtx& operator=
        (
            const   TCIDUserCtx&            cuctxToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetUserInfo
        (
            const   TString&                strUserName
            , const TString&                strUserType
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strUserName
        //      The name of the user account that this context represents. The
        //      derived class defines that this means and any restrictions on
        //      the content. But having it here allows the lower level code
        //      at least report the user name in any errors message and such.
        //
        //  m_strUserType
        //      The derived class also gives us a text name for the type of
        //      use so that it can be used generically in logged or displayed
        //      messages and such, without us having to understand the derived
        //      class' concept of user types.
        // -------------------------------------------------------------------
        TString     m_strUserName;
        TString     m_strUserType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDUserCtx,TObject)
};

#pragma CIDLIB_POPPACK


