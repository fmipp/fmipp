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
  ~HistoryEntryBase() {delete [] state; delete [] values; }
  HistoryEntryBase& operator=(HistoryEntryBase aHistoryEntryBase);
  fmiTime time;
  std::size_t nstates;
  std::size_t nvalues;
  fmiReal* state;
  fmiReal* values;
};
 

struct HistoryBase {
  typedef std::vector< HistoryEntryBase > History;
  typedef typename std::vector< HistoryEntryBase >::const_iterator const_iterator;
  typedef typename std::vector< HistoryEntryBase >::iterator iterator;
  typedef typename std::vector< HistoryEntryBase >::const_reverse_iterator const_reverse_iterator;
  typedef typename std::vector< HistoryEntryBase >::reverse_iterator reverse_iterator;
};

#endif // _HistoryBase_H
