//
// FILE NAME: CIDXML_MixedContentModel.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/1999
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
//  This file implements the TXMLMixedCM class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TXMLMixedCM
// PREFIX: xcm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLMixedCM: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLMixedCM::TXMLMixedCM(const  tCIDLib::TCard4* const  pc4ChildIds
                        , const tCIDLib::TCard4         c4Count) :
    m_c4Count(c4Count)
    , m_pc4ChildIds(nullptr)
{
    m_pc4ChildIds = new tCIDLib::TCard4[c4Count];
    TRawMem::CopyMemBuf(m_pc4ChildIds, pc4ChildIds, c4Count * sizeof(tCIDLib::TCard4));
}

TXMLMixedCM::~TXMLMixedCM()
{
    delete [] m_pc4ChildIds;
}


// ---------------------------------------------------------------------------
//  TXMLMixedCM: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  Note that character data is always valid here, but its never checked via
//  this mechanism. Char data is checked when its actually seen during the
//  parse, since its easy to ask whether char data is legal or not (since its
//  never positionally dependent in the content model.) This is a roundabout
//  way of saying that the passed child array only contains real element ids.
//
tCIDXML::EValidRes
TXMLMixedCM::eValidate( const   tCIDLib::TCard4* const  pc4ChildIds
                        , const tCIDLib::TCard4         c4ChildCount
                        ,       tCIDLib::TCard4&        c4FailedAt) const
{
    // If there are no children, that's ok
    if (!c4ChildCount)
        return tCIDXML::EValidRes::Success;

    // Make sure that all the ids are in our list
    for (tCIDLib::TCard4 c4Outer = 0; c4Outer < c4ChildCount; c4Outer++)
    {
        const tCIDLib::TCard4 c4CurElem = pc4ChildIds[c4Outer];

        // Search our list for this current element
        tCIDLib::TCard4 c4Inner = 0;
        for (; c4Inner < m_c4Count; c4Inner++)
        {
            // If we find it, then break out
            if (c4CurElem == m_pc4ChildIds[c4Inner])
                break;
        }

        // If we did not find it, then report a mismatch
        if (c4Inner == m_c4Count)
        {
            // Return the index at which it failed
            c4FailedAt = c4Outer;
            return tCIDXML::EValidRes::Mismatch;
        }
    }

    // For now just return success
    return tCIDXML::EValidRes::Success;
}


tCIDLib::TVoid
TXMLMixedCM::FormatTo(          TTextOutStream& strmDest
                        , const TXMLValidator&  xvalSource) const
{
    strmDest << L"(#PCDATA";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Count; c4Index++)
    {
        const TXMLElemDecl* pxdeclCur = xvalSource.pxdeclFindElemById
        (
            m_pc4ChildIds[c4Index]
        );

        //
        //  If this happens, something really bad has gone wrong, so lets
        //  throw an exception. Its a general error, but the source position
        //  will tell us where it happened.
        //
        if (!pxdeclCur)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcGen_UnknownElementId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_pc4ChildIds[c4Index])
            );

            // Won't happen, but makes the analyzer happy
            return;
        }

        strmDest << L"|" << pxdeclCur->strFullName();
    }
    strmDest << L")*";
}
