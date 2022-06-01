// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_HISTORY_H
#define _FMIPP_HISTORY_H

#include <vector>
#include <string>
#include <limits>

#include "common/FMIPPConfig.h"

/**
 * \file History.h 
 *
 * \class HistoryEntry History.h 
 * Helper class used to store FMU states (e.g., for predictions).
 **/

class HistoryEntry
{

public:

	HistoryEntry();
	HistoryEntry( fmippSize nStates, fmippSize nRealValues , fmippSize nIntegerValues , fmippSize nBooleanValues , fmippSize nStringValues );
	HistoryEntry( const fmippTime& t, fmippSize nStates, fmippSize nRealValues , fmippSize nIntegerValues , fmippSize nBooleanValues , fmippSize nStringValues );
	HistoryEntry( const fmippTime& t, fmippReal* s, fmippSize nStates, fmippReal* realValues, fmippSize nRealValues , fmippInteger* integerValues, fmippSize nIntegerValues , fmippBoolean* booleanValues, fmippSize nBooleanValues , fmippString* stringValues, fmippSize nStringValues );
	HistoryEntry( const HistoryEntry& aHistoryEntry );

	~HistoryEntry() { delete [] state_; delete [] realValues_;  delete [] integerValues_;  delete [] booleanValues_;  delete [] stringValues_; }

	HistoryEntry& operator=( HistoryEntry aHistoryEntry );

	fmippTime time_;
	fmippSize nStates_;
	fmippSize nRealValues_;
	fmippSize nIntegerValues_;
	fmippSize nBooleanValues_;
	fmippSize nStringValues_;
	fmippReal* state_;
	fmippReal* realValues_;
	fmippInteger* integerValues_;
	fmippBoolean* booleanValues_;
	fmippString* stringValues_;
};

/// This namespace contains typedefs that ease the use of class HistorEntry.
namespace History
{
	typedef std::vector< HistoryEntry > History;
	typedef std::vector< HistoryEntry >::const_iterator const_iterator;
	typedef std::vector< HistoryEntry >::iterator iterator;
	typedef std::vector< HistoryEntry >::const_reverse_iterator const_reverse_iterator;
	typedef std::vector< HistoryEntry >::reverse_iterator reverse_iterator;
};

#endif // _FMIPP_HISTORY_H
