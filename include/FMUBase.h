#ifndef _FMIPP_FMUBASE_H
#define _FMIPP_FMUBASE_H


#include <string>
#include <vector>

#include "FMIPPConfig.h"
#include "fmi_me.h"


class FMUBase
{

public:

	virtual ~FMUBase() {}

	// Instantiate the FMU
	virtual fmiStatus instantiate( const std::string& instanceName, fmiBoolean loggingOn ) = 0;

	virtual fmiStatus initialize() = 0;

	virtual const fmiReal& getTime() const = 0;
	virtual void setTime( fmiReal time ) = 0;
	virtual void rewindTime( fmiReal deltaRewindTime ) = 0;

	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val ) = 0;
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) = 0;

	//virtual fmiStatus setValue(fmiValueReference valref, fmiBoolean& val ) = 0;

	virtual fmiStatus setValue( const std::string& name,  fmiReal val ) = 0; 

	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const = 0;
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const = 0;

	//virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const = 0;

	virtual fmiStatus getValue( const std::string& name,  fmiReal& val ) const = 0; 

	virtual fmiValueReference getValueRef( const std::string& name ) const = 0;

	virtual fmiStatus getContinuousStates( fmiReal* val ) const = 0;
	virtual fmiStatus setContinuousStates( const fmiReal* val ) = 0;

	virtual fmiStatus getDerivatives( fmiReal* val ) const = 0;

	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) const = 0; 
  
	virtual fmiStatus integrate( fmiReal tend, unsigned int nsteps ) = 0;
	virtual fmiStatus integrate( fmiReal tend, double deltaT ) = 0;

	virtual void raiseEvent() = 0;
	virtual void handleEvents( fmiTime tstop, bool completedIntegratorStep ) = 0;

	virtual std::size_t nStates() const = 0;
	virtual std::size_t nEventInds() const = 0;
	virtual std::size_t nValueRefs() const = 0;

// 	static void logger( fmiComponent m, fmiString instanceName,
// 			    fmiStatus status, fmiString category,
// 			    fmiString message, ... );

};




#endif // _FMIPP_FMUBASE_H
