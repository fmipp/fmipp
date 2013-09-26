/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_HISTORY_H
#define _FMIPP_HISTORY_H


#include <vector>
#include <string>
#include <limits>

#include "fmiModelTypes.h"

/**
 * \file History.h 
 * \class HistoryEntry History.h 
 * Helpers used internally by IncrementalFMU to manage predictions.
 **/

class HistoryEntry
{

public:

	HistoryEntry();
	HistoryEntry( std::size_t nStates, std::size_t nRealValues , std::size_t nIntegerValues , std::size_t nBooleanValues , std::size_t nStringValues );
	HistoryEntry( const fmiTime& t, std::size_t nStates, std::size_t nRealValues , std::size_t nIntegerValues , std::size_t nBooleanValues , std::size_t nStringValues );
	HistoryEntry( const fmiTime& t, fmiReal* s, std::size_t nStates, fmiReal* realValues, std::size_t nRealValues , fmiInteger* integerValues, std::size_t nIntegerValues , fmiBoolean* booleanValues, std::size_t nBooleanValues , std::string* stringValues, std::size_t nStringValues );
	HistoryEntry( const HistoryEntry& aHistoryEntry );

	~HistoryEntry() { delete [] state_; delete [] realValues_;  delete [] integerValues_;  delete [] booleanValues_;  delete [] stringValues_; }

	HistoryEntry& operator=( HistoryEntry aHistoryEntry );

	fmiTime time_;
	std::size_t nStates_;
	std::size_t nRealValues_;
	std::size_t nIntegerValues_;
	std::size_t nBooleanValues_;
	std::size_t nStringValues_;
	fmiReal* state_;
	fmiReal* realValues_;
	fmiInteger* integerValues_;
	fmiBoolean* booleanValues_;
	std::string* stringValues_;
};


namespace History
{
	typedef std::vector< HistoryEntry > History;
	typedef std::vector< HistoryEntry >::const_iterator const_iterator;
	typedef std::vector< HistoryEntry >::iterator iterator;
	typedef std::vector< HistoryEntry >::const_reverse_iterator const_reverse_iterator;
	typedef std::vector< HistoryEntry >::reverse_iterator reverse_iterator;
};


#endif // _FMIPP_HISTORY_H
