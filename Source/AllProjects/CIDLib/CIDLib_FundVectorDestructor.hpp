//
// FILE NAME: CIDLib_FundVectorDestructor.hpp
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
//  This header defines the destructor of TFundVector.
//  Since this depends on TError it has to be included after the rest
//  of the class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


template <typename TElem, typename TIndex>
TFundVector<TElem, TIndex>::~TFundVector()
{
    try
    {
	delete [] m_ptElements;
	m_ptElements = nullptr;
    }

    catch(TError& errToCatch)
    {
	errToCatch.AddStackLevel(CID_FILE, CID_LINE);
	TModule::LogEventObj(errToCatch);
    }
}

