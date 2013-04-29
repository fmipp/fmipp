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

protected:

  // Make a rollback.
  fmiStatus rollback( fmiTime time );

private:

  /**  prevent calling the default constructor */ 
  RollbackFMU();

  HistoryEntryBase rollbackState_;

};

#endif
