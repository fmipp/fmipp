// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file History.cpp 
 */ 

#include <stdlib.h>

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

HistoryEntry::HistoryEntry( fmippSize nStates, fmippSize nRealValues, fmippSize nIntegerValues, fmippSize nBooleanValues, fmippSize nStringValues )
{
	time_ = INVALID_FMI_TIME;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates ? new fmippReal[nStates] : NULL;
	realValues_ = nRealValues ? new fmippReal[nRealValues] : NULL;
	integerValues_ = nIntegerValues ? new fmippInteger[nIntegerValues] : NULL;
	booleanValues_ = nBooleanValues ? new fmippBoolean[nBooleanValues] : NULL;
	stringValues_ = nStringValues ? new fmippString[nStringValues] : NULL;
}

HistoryEntry::HistoryEntry( const fmippTime& t, fmippSize nStates, fmippSize nRealValues, fmippSize nIntegerValues, fmippSize nBooleanValues, fmippSize nStringValues )
{
	time_ = t;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates ? new fmippReal[nStates] : NULL;
	realValues_ = nRealValues ? new fmippReal[nRealValues] : NULL;
	integerValues_ = nIntegerValues ? new fmippInteger[nIntegerValues] : NULL;
	booleanValues_ = nBooleanValues ? new fmippBoolean[nBooleanValues] : NULL;
	stringValues_ = nStringValues ? new fmippString[nStringValues] : NULL;
}

HistoryEntry::HistoryEntry( const fmippTime& t, fmippReal* s, fmippSize nStates, fmippReal* realValues, fmippSize nRealValues, fmippInteger* integerValues, fmippSize nIntegerValues, fmippBoolean* booleanValues, fmippSize nBooleanValues, fmippString* stringValues, fmippSize nStringValues )
{
	time_ = t;
	nStates_ = nStates;
	nRealValues_ = nRealValues;
	nIntegerValues_ = nIntegerValues;
	nBooleanValues_ = nBooleanValues;
	nStringValues_ = nStringValues;
	state_ = nStates_ ? new fmippReal[nStates_] : NULL;
	for ( fmippSize i = 0; i < nStates_; ++i ) {
		state_[i] = s[i];
	}
	realValues_ = nRealValues_ ? new fmippReal[nRealValues_] : NULL;
	for ( fmippSize i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = realValues[i];
	}
	integerValues_ = nIntegerValues_ ? new fmippInteger[nIntegerValues_] : NULL;
	for ( fmippSize i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = integerValues[i];
	}
	booleanValues_ = nBooleanValues_ ? new fmippBoolean[nBooleanValues_] : NULL;
	for ( fmippSize i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = booleanValues[i];
	}
	stringValues_ = nStringValues_ ? new fmippString[nStringValues_] : NULL;
	for ( fmippSize i = 0; i < nStringValues_; ++i ) {
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
	state_ = nStates_ ? new fmippReal[nStates_] : NULL;
	for ( fmippSize i = 0; i < nStates_; ++i ) {
		state_[i] = aHistoryEntry.state_[i];
	}
	realValues_ = nRealValues_ ? new fmippReal[nRealValues_] : NULL;
	for ( fmippSize i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = aHistoryEntry.realValues_[i];
	}
	integerValues_ = nIntegerValues_ ? new fmippInteger[nIntegerValues_] : NULL;
	for ( fmippSize i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = aHistoryEntry.integerValues_[i];
	}
	booleanValues_ = nBooleanValues_ ? new fmippBoolean[nBooleanValues_] : NULL;
	for ( fmippSize i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = aHistoryEntry.booleanValues_[i];
	}
	stringValues_ = nStringValues_ ? new fmippString[nStringValues_] : NULL;
	for ( fmippSize i = 0; i < nStringValues_; ++i ) {
		stringValues_[i] = aHistoryEntry.stringValues_[i];
	}
}

HistoryEntry& HistoryEntry::operator=( HistoryEntry aHistoryEntry )
{
	time_ = aHistoryEntry.time_;
	if ( nStates_ != aHistoryEntry.nStates_ ) {
		nStates_ = aHistoryEntry.nStates_;
		delete [] state_;
		state_ = nStates_ ? new fmippReal[nStates_] : NULL;
	}
	for ( fmippSize i = 0; i < nStates_; ++i ) {
		state_[i] = aHistoryEntry.state_[i];
	}

	if ( nRealValues_ != aHistoryEntry.nRealValues_ ) {
		nRealValues_ = aHistoryEntry.nRealValues_;
		delete [] realValues_;
		realValues_ = nRealValues_ ? new fmippReal[nRealValues_] : NULL;
	}
	for ( fmippSize i = 0; i < nRealValues_; ++i ) {
		realValues_[i] = aHistoryEntry.realValues_[i];
	}

	if ( nIntegerValues_ != aHistoryEntry.nIntegerValues_ ) {
		nIntegerValues_ = aHistoryEntry.nIntegerValues_;
		delete [] integerValues_;
		integerValues_ = nIntegerValues_ ? new fmippInteger[nIntegerValues_] : NULL;
	}
	for ( fmippSize i = 0; i < nIntegerValues_; ++i ) {
		integerValues_[i] = aHistoryEntry.integerValues_[i];
	}

	if ( nBooleanValues_ != aHistoryEntry.nBooleanValues_ ) {
		nBooleanValues_ = aHistoryEntry.nBooleanValues_;
		delete [] booleanValues_;
		booleanValues_ = nBooleanValues_ ? new fmippBoolean[nBooleanValues_] : NULL;
	}
	for ( fmippSize i = 0; i < nBooleanValues_; ++i ) {
		booleanValues_[i] = aHistoryEntry.booleanValues_[i];
	}

	if ( nStringValues_ != aHistoryEntry.nStringValues_ ) {
		nStringValues_ = aHistoryEntry.nStringValues_;
		delete [] stringValues_;
		stringValues_ = nStringValues_ ? new fmippString[nStringValues_] : NULL;
	}
	for ( fmippSize i = 0; i < nStringValues_; ++i ) {
		stringValues_[i] = aHistoryEntry.stringValues_[i];
	}

	return *this;
}
