#include "HistoryBase.h"

HistoryEntryBase::HistoryEntryBase()
{
  time = INVALID_FMI_TIME;
  nstates = 0;
  nvalues = 0;
  state = NULL;
  values = NULL;
}

HistoryEntryBase::HistoryEntryBase(std::size_t nStates, std::size_t nValues)
{
  time = INVALID_FMI_TIME;
  nstates = nStates;
  nvalues = nValues;
  state = nStates ? new fmiReal[nStates] : NULL;
  values = nValues ? new fmiReal[nValues] : NULL;
}


HistoryEntryBase::HistoryEntryBase(const fmiTime& t, std::size_t nStates, std::size_t nValues)
{
  time = t;
  nstates = nStates;
  nvalues = nValues;
  state = nStates ? new fmiReal[nStates] : NULL;
  values = nValues ? new fmiReal[nValues] : NULL;
}


HistoryEntryBase::HistoryEntryBase(const fmiTime& t, fmiReal* s, std::size_t nStates, fmiReal* v, std::size_t nValues)
{
  time = t;
  nstates = nStates;
  nvalues = nValues;
  state = new fmiReal[nStates];
  for(std::size_t i = 0; i < nStates; i++) {
    state[i] = s[i];
  }
  values = new fmiReal[nValues];
  for(std::size_t i = 0; i < nValues; i++) {
    values[i] = v[i];
  }
}

HistoryEntryBase::HistoryEntryBase(const HistoryEntryBase& aHistoryEntryBase)
{
  time = aHistoryEntryBase.time;
  nstates = aHistoryEntryBase.nstates;
  nvalues = aHistoryEntryBase.nvalues;
  state = nstates ? new fmiReal[nstates] : NULL;
  values = nvalues ? new fmiReal[nvalues] : NULL;
  for(std::size_t i = 0; i < nstates; i++) {
    state[i] = aHistoryEntryBase.state[i];
  }
  for(std::size_t i = 0; i < nvalues; i++) {
    values[i] = aHistoryEntryBase.values[i];
  }
}


HistoryEntryBase& HistoryEntryBase::operator=(HistoryEntryBase aHistoryEntryBase)
{
  time = aHistoryEntryBase.time;
  if(nstates != aHistoryEntryBase.nstates) {
    nstates = aHistoryEntryBase.nstates;
    delete [] state;
    state = nstates ? new fmiReal[nstates] : NULL;
  }
  for(std::size_t i = 0; i < nstates; i++) {
    state[i] = aHistoryEntryBase.state[i];
  }

  if(nvalues != aHistoryEntryBase.nvalues) {
    nvalues = aHistoryEntryBase.nvalues;
    delete [] values;
    values = nvalues ? new fmiReal[nvalues] : NULL;
  }
  for(std::size_t i = 0; i < nvalues; i++) {
    values[i] = aHistoryEntryBase.values[i];
  }
}
