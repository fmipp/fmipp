#ifndef _IncrementalFMU_H
#define _IncrementalFMU_H

#include <vector>
#include <string>
#include <limits>

#include "FMU.h"
#include "HistoryBase.h"


class __FMI_DLL IncrementalFMU
{

public:

  IncrementalFMU( const std::string& name )
    { fmu_ = new FMU(name); allocevmem(); }

  IncrementalFMU( const std::string& path,
		  const std::string& name )
    { fmu_ = new FMU(path, name); allocevmem(); }

  IncrementalFMU( const std::string& name,
		  const std::string inputs[],
		  const std::size_t nInputs,
		  const std::string outputs[],
		  const std::size_t nOutputs);

  IncrementalFMU( const std::string& name,
		  const std::size_t nInputs,
		  const std::size_t nOutputs );

  IncrementalFMU( const IncrementalFMU& aIncrementalFMU );

  ~IncrementalFMU()
    { delete fmu_; delete [] eventinds_; delete [] eventindspos_; }

  void setInputs( const std::string inputs[],
		  const std::size_t nInputs );

  void setOutputs( const std::string outputs[],
		   const std::size_t nOutputs );

  int init( const std::string& instanceName,
	    const std::string variableNames[],
	    const fmiReal* values,
	    const std::size_t nvars,
	    const fmiTime startTime,
	    const fmiTime horizon,
	    const fmiTime stepsize );

  fmiTime sync( fmiTime t0, fmiTime t1 );

  fmiTime sync( fmiTime t0, fmiTime t1, fmiReal* inputs );

  fmiReal* getCurrentState() const { return currentState_.state; }

  fmiReal* getCurrentOutputs() const { return currentState_.values; }

  // We should change these two functions, because in combination with the lookahead they cause problems. and just setting the time doesn't get it right !!!
  inline fmiStatus getValue( const std::string& name, fmiReal* val) { return fmu_->getValue(name, val); }

  inline fmiReal getValue( const std::string& name ) { fmiReal val; fmu_->getValue(name, &val); return val; }

  // this function should not even be in the interface
  inline fmiStatus setValue(const std::string& name, fmiReal val) { return fmu_->setValue(name, val); }

protected:

  /* A look-ahead prediction and its corresponding time are
     stored as "History" are stored in a containers called "History" (see typedef below)*/

  typedef HistoryEntryBase HistoryEntry;
  typedef typename HistoryBase::History History;
  typedef typename HistoryBase::const_iterator History_const_iterator;
  typedef typename HistoryBase::iterator       History_iterator;
  typedef typename HistoryBase::const_reverse_iterator History_const_reverse_iterator;
  typedef typename HistoryBase::reverse_iterator       History_reverse_iterator;

  /* Look-ahead horizon. */
  fmiTime lookAheadHorizon_;

  /* Intergrator step size. */
  fmiTime integratorStepSize_;

  /* Names of the inputs. */
  std::size_t* inputs_;

  /* Number of inputs. */
  std::size_t nInputs_;

  /* Names of the outputs. */
  std::size_t* outputs_;

  /* Number of outputs. */
  std::size_t nOutputs_;

  //NOT USED: fmiTime globalFMUSyncTime_;

  /* Interface to the FMU. */
  FMU* fmu_;

  /* Vector of state predictions. */
  History predictions_;

  /* The current state. */
  HistoryEntry currentState_;

  /* Define the initial inputs of the FMU (input states before initialization). */
  virtual void initialInputs(const std::string variableNames[], const fmiReal* values, std::size_t nvars);


  /* Define the initial state of the FMU (state before first integration). */
  virtual fmiReal* initialState() const;

  /* In case no look-ahead prediction is given for time t, this function is responsible to provide
   * an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
   * next prediction available AFTER time t is handed over to the function. */
  void interpolateState(fmiTime t, History_const_reverse_iterator& historyEntry, HistoryEntry& state);

  /* Check the latest prediction if an event has occured. If so, update the latest prediction accordingly. */
  //bool checkForEvent(History_const_iterator& historyEntry);
  virtual bool checkForEvent();

  /* Set initial values for integration (i.e. for each look-ahead). */
  virtual void initializeIntegration(fmiReal* initialState);

  /* Retrieve values after each integration step from FMU. */
  virtual void retrieveIntegrationResults(fmiReal* result, fmiReal* values, fmiReal* eventinds) const;

//private:

  IncrementalFMU() {}

  /* Compute state at time t from previous state predictions. */
  void getState(fmiTime t, HistoryEntry& state);

  // Helper function: linear value interpolation.
  double interpolateValue( fmiTime x, fmiTime x0, fmiReal y0, fmiTime x1, fmiReal y1 ) const;

  /* Don't need second version as long as fmiTime == fmiReal
  // Helper function: linear value interpolation.
  fmiTime interpolateValue( fmiReal x, fmiReal x0, fmiTime y0, fmiReal x1, fmiTime y1 ) const;
  */

  /* Update state at time t1, i.e. change the actual state using previous prediction(s). */
  fmiTime updateState( fmiTime t0, fmiTime t1 );

  /* Compute state predictions. */
  fmiTime predictState( fmiTime t1 );

  fmiStatus setCurrentInputs(fmiReal* inputs) const;

  fmiReal* eventinds_;

  bool* eventindspos_;

  void allocevmem() { eventinds_ = new fmiReal[fmu_->nEventInds()]; eventindspos_ = new bool[fmu_->nEventInds()]; }

};

#endif // _IncrementalFMU_H
