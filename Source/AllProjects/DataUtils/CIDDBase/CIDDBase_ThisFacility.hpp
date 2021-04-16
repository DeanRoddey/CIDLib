//
// FILE NAME: CIDDBase_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This is the header file for the CIDDBase_ThisFacility.Cpp file. This
//  file implements the facility class for this facility. This guy just provides
//  some global functionality for initializing/terminating database access, and
//  querying available data sources.
//
//  Initialization sets up an 'environment' handle that each platform can use
//  to store any sort of overall information they need to enable database access
//  on that platform. Their other per-platform content can access that as required.
//  If not needed, just define it as an empty structure and allocate one.
//
//  We have one method that returns the raw pointer, which will be null if we
//  have not been initialized (or have been terminated.) There is another that
//  returns a ref, and it will throw if we haven't been initialized so it's best
//  to use that one most of the time.
//
//  The sources query returns a list of key/value pairs, where the key is a name
//  and the value is some platform specific identifier for the source that can be
//  used to create a connection via it.
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
//  CLASS: TFacCIDDBase
// PREFIX: fac
// ---------------------------------------------------------------------------
class  CIDDBASEEXP TFacCIDDBase : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDDBase();

        TFacCIDDBase(const TFacCIDDBase&) = delete;

        ~TFacCIDDBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDDBase& operator=(const TFacCIDDBase&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize();

        tCIDLib::TVoid QuerySources
        (
                    tCIDLib::TKVPCollect&   colToFill
            , const tCIDDBase::ESrcTypes    eType
        );

        tCIDDBase::TDBEnvHandle* phEnv() const
        {
            return m_pEnvHandle;
        }

        tCIDDBase::TDBEnvHandle& hEnv() const;

        tCIDLib::TVoid Terminate();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_atomInit
        //      Though they have to call init/term methods, we want to be thread
        //      safe for these calls.
        //
        //  m_pEnvHandle
        //      This is an opaque structure that is defined on a per-platform basis.
        //      We make it available so that the other classes can get to this info
        //      which provides overall environmental info that might be required on
        //      a given platform.
        // -------------------------------------------------------------------
        TAtomicFlag                 m_atomInit;
        tCIDDBase::TDBEnvHandle*    m_pEnvHandle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDBase,TFacility)
};

#pragma CIDLIB_POPPACK


