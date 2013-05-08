#ifndef _ROLLBACKFMU_H
#define _ROLLBACKFMU_H


#include "FMU.h"
#include "HistoryBase.h"


/** 
 *  This class allows to perform rollbacks to times not longer ago than the previous update.
 */


class __FMI_DLL RollbackFMU : public FMU
{

public:

  RollbackFMU( const std::string& modelName );

  RollbackFMU( const std::string& fmuPath,
       const std::string& modelName );

  RollbackFMU( const std::string& xmlPath,
       const std::string& dllPath,
       const std::string& modelName );

  RollbackFMU( const RollbackFMU& aRollbackFMU );

  ~RollbackFMU();

  virtual fmiReal integrate( fmiReal tstop, unsigned int nsteps );
  virtual fmiReal integrate( fmiReal tstop, double deltaT=1E-5 );

  /** Saves the current state of the FMU as internal rollback
      state. This rollback state will not be overwritten until
      "releaseRollbackState()" is called; **/
  void saveCurrentStateForRollback();

  /** Realease an internal rollback state, that was previously
      saved via "saveCurrentStateForRollback()". **/
  void releaseRollbackState();

protected:

  // Make a rollback.
  fmiStatus rollback( fmiTime time );

private:

  /**  prevent calling the default constructor */ 
  RollbackFMU();

  HistoryEntryBase rollbackState_;

  bool rollbackStateSaved_;

};

#endif
