#include "HistoryBase.h"

HistoryEntryBase::HistoryEntryBase()
{
  time_ = INVALID_FMI_TIME;
  nstates_ = 0;
  nvalues_ = 0;
  state_ = NULL;
  values_ = NULL;
}

HistoryEntryBase::HistoryEntryBase(std::size_t nStates, std::size_t nValues)
{
  time_ = INVALID_FMI_TIME;
  nstates_ = nStates;
  nvalues_ = nValues;
  state_ = nStates ? new fmiReal[nStates] : NULL;
  values_ = nValues ? new fmiReal[nValues] : NULL;
}


HistoryEntryBase::HistoryEntryBase(const fmiTime& t, std::size_t nStates, std::size_t nValues)
{
  time_ = t;
  nstates_ = nStates;
  nvalues_ = nValues;
  state_ = nStates ? new fmiReal[nStates] : NULL;
  values_ = nValues ? new fmiReal[nValues] : NULL;
}


HistoryEntryBase::HistoryEntryBase(const fmiTime& t, fmiReal* s, std::size_t nStates, fmiReal* v, std::size_t nValues)
{
  time_ = t;
  nstates_ = nStates;
  nvalues_ = nValues;
  state_ = nstates_ ? new fmiReal[nstates_] : NULL;
  for(std::size_t i = 0; i < nStates; ++i) {
    state_[i] = s[i];
  }
  values_ = nvalues_ ? new fmiReal[nvalues_] : NULL;
  for(std::size_t i = 0; i < nValues; ++i) {
    values_[i] = v[i];
  }
}

HistoryEntryBase::HistoryEntryBase(const HistoryEntryBase& aHistoryEntryBase)
{
  time_ = aHistoryEntryBase.time_;
  nstates_ = aHistoryEntryBase.nstates_;
  nvalues_ = aHistoryEntryBase.nvalues_;
  state_ = nstates_ ? new fmiReal[nstates_] : NULL;
  values_ = nvalues_ ? new fmiReal[nvalues_] : NULL;
  for(std::size_t i = 0; i < nstates_; ++i) {
    state_[i] = aHistoryEntryBase.state_[i];
  }
  for(std::size_t i = 0; i < nvalues_; ++i) {
    values_[i] = aHistoryEntryBase.values_[i];
  }
}


HistoryEntryBase& HistoryEntryBase::operator=(HistoryEntryBase aHistoryEntryBase)
{
  time_ = aHistoryEntryBase.time_;
  if(nstates_ != aHistoryEntryBase.nstates_) {
    nstates_ = aHistoryEntryBase.nstates_;
    delete [] state_;
    state_ = nstates_ ? new fmiReal[nstates_] : NULL;
  }
  for(std::size_t i = 0; i < nstates_; ++i) {
    state_[i] = aHistoryEntryBase.state_[i];
  }

  if(nvalues_ != aHistoryEntryBase.nvalues_) {
    nvalues_ = aHistoryEntryBase.nvalues_;
    delete [] values_;
    values_ = nvalues_ ? new fmiReal[nvalues_] : NULL;
  }
  for(std::size_t i = 0; i < nvalues_; ++i) {
    values_[i] = aHistoryEntryBase.values_[i];
  }

  return *this;
}
