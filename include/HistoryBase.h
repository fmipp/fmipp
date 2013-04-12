#ifndef _HistoryBase_H
#define _HistoryBase_H

#include <vector>
#include <string>
#include <limits>

#include "fmiModelTypes.h"

#define INVALID_FMI_TIME std::numeric_limits<fmiTime>::infinity()


class HistoryEntryBase {
 public:
  HistoryEntryBase();
  HistoryEntryBase(std::size_t nStates, std::size_t nValues);
  HistoryEntryBase(const fmiTime& t, std::size_t nStates, std::size_t nValues);
  HistoryEntryBase(const fmiTime& t, fmiReal* s, std::size_t nStates, fmiReal* v, std::size_t nValues);
  HistoryEntryBase(const HistoryEntryBase& aHistoryEntryBase);

  ~HistoryEntryBase() {delete [] state_; delete [] values_; }

  HistoryEntryBase& operator=(HistoryEntryBase aHistoryEntryBase);
 
  fmiTime time_;
  std::size_t nstates_;
  std::size_t nvalues_;
  fmiReal* state_;
  fmiReal* values_;
};


struct HistoryBase {
  typedef std::vector< HistoryEntryBase > History;
  typedef std::vector< HistoryEntryBase >::const_iterator const_iterator;
  typedef std::vector< HistoryEntryBase >::iterator iterator;
  typedef std::vector< HistoryEntryBase >::const_reverse_iterator const_reverse_iterator;
  typedef std::vector< HistoryEntryBase >::reverse_iterator reverse_iterator;
};

#endif // _HistoryBase_H
