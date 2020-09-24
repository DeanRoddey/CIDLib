//
// FILE NAME: CIDLib_FundStackStrmOps.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/25/2020
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
//  This header implements the stream operators for TFundStack.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

template <typename T> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundStack<T>& colToStream)
{
    // First we should get a stream marker
    strmToReadFrom.CheckForMarker
    (
        tCIDLib::EStreamMarkers::StartObject
        , CID_FILE
        , CID_LINE
    );

    // Get out the new max and count
    tCIDLib::TCard4 c4NewCount;
    tCIDLib::TCard4 c4NewMax;
    strmToReadFrom >> c4NewMax >> c4NewCount;

    // Validate these
    if ((c4NewCount > c4NewMax) || !c4NewMax)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFData_BadExtData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  If the current max size and the new max size are not the
    //  same, then reallocate.
    //
    if (colToStream.m_c4MaxElements != c4NewMax)
    {
        delete [] colToStream.m_ptElements;
        colToStream.m_ptElements = 0;
        colToStream.m_ptElements = new T[c4NewMax];
        colToStream.m_c4MaxElements = c4NewMax;
    }

    // Store the new top
    colToStream.m_c4Top = c4NewCount;

    // And read in the elements themselves.
    TBinInStream_ReadArray(strmToReadFrom, colToStream.m_ptElements, colToStream.m_c4Top);
    return strmToReadFrom;
}

template <typename T> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundStack<T>& colToStream)
{
    // Stream out our start marker, max elements and current element count
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject
                  << colToStream.m_c4MaxElements
                  << colToStream.m_c4Top;

    // And write out the elements themselves
    TBinOutStream_WriteArray(strmToWriteTo, colToStream.m_ptElements, colToStream.m_c4Top);
    return strmToWriteTo;
}

