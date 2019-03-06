//
// FILE NAME: CIDLib_DirChangeIfno.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/2018
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  A simple class used by the directory change monitoring stuff. It has to be broken
//  out for order of inclusion reasons. The implementation is still in the same
//  main file.
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
//   CLASS: TDirChangeInfo
//  PREFIX: dchi
// ---------------------------------------------------------------------------
class CIDLIBEXP TDirChangeInfo : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDirChangeInfo();

        TDirChangeInfo
        (
            const tCIDLib::EDirChanges  eChange
        );

        TDirChangeInfo
        (
            const   TDirChangeInfo&     dchiSrc
        );

        ~TDirChangeInfo();


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TDirChangeInfo& operator=
        (
            const   TDirChangeInfo&     dchiSrc
        );


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_eChanges
        //      The change being reported
        //
        //  m_strName
        //      The (directory relative) name of the thing being reported on
        //
        //  m_strNew
        //      Only used for a renamed change, and indicates the new name, while
        //      m_strName is the original name.
        // -------------------------------------------------------------------
        tCIDLib::EDirChanges    m_eChange;
        TString                 m_strName;
        TString                 m_strNew;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDirChangeInfo,TObject)
};

#pragma CIDLIB_POPPACK
