// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file History.cpp 
 */ 

#include <stdlib.h>

#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"
#include "import/utility/include/History.h"


HistoryEntry::HistoryEntry()
{
	time_ = INVALID_FMI_TIME;
	nStates_ = 0;
	nRealValues_ = 0;
	nIntegerValues_ = 0;
	nBooleanValues_ = 0;
	nStringValues_ = 0;
	state_ = NULL;
	realValues_ = NULL;
	integerValues_ = NULL;
	booleanValues_ = NULL;
	stringValues_ = NULL;
}


HistoryEntry::HistoryEntry( std::size_t nStates, std::size_t nRealValues , std::size_t nIntegerValues , std::size_t nBooleanValues , std::size_t nStringValues )
{
	time_ = INVALID_FMI_TIME;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates ? new fmiReal[nStates] : NULL;
	realValues_ = nRealValues ? new fmiReal[nRealValues] : NULL;
	integerValues_ = nIntegerValues ? new fmiInteger[nIntegerValues] : NULL;
	booleanValues_ = nBooleanValues ? new fmiBoolean[nBooleanValues] : NULL;
	stringValues_ = nStringValues ? new std::string[nStringValues] : NULL;
}


HistoryEntry::HistoryEntry( const fmiTime& t, std::size_t nStates, std::size_t nRealValues , std::size_t nIntegerValues , std::size_t nBooleanValues , std::size_t nStringValues )
{
	time_ = t;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates ? new fmiReal[nStates] : NULL;
	realValues_ = nRealValues ? new fmiReal[nRealValues] : NULL;
	integerValues_ = nIntegerValues ? new fmiInteger[nIntegerValues] : NULL;
	booleanValues_ = nBooleanValues ? new fmiBoolean[nBooleanValues] : NULL;
	stringValues_ = nStringValues ? new std::string[nStringValues] : NULL;
}


HistoryEntry::HistoryEntry( const fmiTime& t, fmiReal* s, std::size_t nStates, fmiReal* realValues, std::size_t nRealValues , fmiInteger* integerValues, std::size_t nIntegerValues , fmiBoolean* booleanValues, std::size_t nBooleanValues , std::string* stringValues, std::size_t nStringValues )
{
	time_ = t;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates_ ? new fmiReal[nStates_] : NULL;
	for ( std::size_t i = 0; i < nStates_; ++i ) {
		state_[i] = s[i];
	}
	realValues_ = nRealValues_ ? new fmiReal[nRealValues_] : NULL;
	for ( std::size_t i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = realValues[i];
	}
	integerValues_ = nIntegerValues_ ? new fmiInteger[nIntegerValues_] : NULL;
	for ( std::size_t i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = integerValues[i];
	}
	booleanValues_ = nBooleanValues_ ? new fmiBoolean[nBooleanValues_] : NULL;
	for ( std::size_t i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = booleanValues[i];
	}
	stringValues_ = nStringValues_ ? new std::string[nStringValues_] : NULL;
	for ( std::size_t i = 0; i < nStringValues_; ++i ) {
		stringValues_[i] = stringValues[i];
	}
}


HistoryEntry::HistoryEntry( const HistoryEntry& aHistoryEntry )
{
	time_ = aHistoryEntry.time_;
	nStates_ = aHistoryEntry.nStates_;
	nRealValues_ = aHistoryEntry.nRealValues_;
	nIntegerValues_ = aHistoryEntry.nIntegerValues_;
	nBooleanValues_ = aHistoryEntry.nBooleanValues_;
	nStringValues_ = aHistoryEntry.nStringValues_;
	state_ = nStates_ ? new fmiReal[nStates_] : NULL;
	for ( std::size_t i = 0; i < nStates_; ++i ) {
		state_[i] = aHistoryEntry.state_[i];
	}
	realValues_ = nRealValues_ ? new fmiReal[nRealValues_] : NULL;
	for ( std::size_t i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = aHistoryEntry.realValues_[i];
	}
	integerValues_ = nIntegerValues_ ? new fmiInteger[nIntegerValues_] : NULL;
	for ( std::size_t i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = aHistoryEntry.integerValues_[i];
	}
	booleanValues_ = nBooleanValues_ ? new fmiBoolean[nBooleanValues_] : NULL;
	for ( std::size_t i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = aHistoryEntry.booleanValues_[i];
	}
	stringValues_ = nStringValues_ ? new std::string[nStringValues_] : NULL;
	for ( std::size_t i = 0; i < nStringValues_; ++i ) {
		stringValues_[i] = aHistoryEntry.stringValues_[i];
	}
}


HistoryEntry& HistoryEntry::operator=( HistoryEntry aHistoryEntry )
{
	time_ = aHistoryEntry.time_;
	if ( nStates_ != aHistoryEntry.nStates_ ) {
		nStates_ = aHistoryEntry.nStates_;
		delete [] state_;
		state_ = nStates_ ? new fmiReal[nStates_] : NULL;
	}
	for ( std::size_t i = 0; i < nStates_; ++i ) {
		state_[i] = aHistoryEntry.state_[i];
	}

	if ( nRealValues_ != aHistoryEntry.nRealValues_ ) {
		nRealValues_ = aHistoryEntry.nRealValues_;
		delete [] realValues_;
		realValues_ = nRealValues_ ? new fmiReal[nRealValues_] : NULL;
	}
	for ( std::size_t i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = aHistoryEntry.realValues_[i];
	}

	if ( nIntegerValues_ != aHistoryEntry.nIntegerValues_ ) {
		nIntegerValues_ = aHistoryEntry.nIntegerValues_;
		delete [] integerValues_;
		integerValues_ = nIntegerValues_ ? new fmiInteger[nIntegerValues_] : NULL;
	}
	for ( std::size_t i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = aHistoryEntry.integerValues_[i];
	}

	if ( nBooleanValues_ != aHistoryEntry.nBooleanValues_ ) {
		nBooleanValues_ = aHistoryEntry.nBooleanValues_;
		delete [] booleanValues_;
		booleanValues_ = nBooleanValues_ ? new fmiBoolean[nBooleanValues_] : NULL;
	}
	for ( std::size_t i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = aHistoryEntry.booleanValues_[i];
	}

	if ( nStringValues_ != aHistoryEntry.nStringValues_ ) {
		nStringValues_ = aHistoryEntry.nStringValues_;
		delete [] stringValues_;
		stringValues_ = nStringValues_ ? new std::string[nStringValues_] : NULL;
	}
	for ( std::size_t i = 0; i < nStringValues_; ++i ) {
		stringValues_[i] = aHistoryEntry.stringValues_[i];
	}

	return *this;
}
