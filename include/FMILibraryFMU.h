#ifndef _FMILIBRARYFMU_H
#define _FMILIBRARYFMU_H


#include <map>

#include "FMI/fmi_import_context.h"
#include "FMI1/fmi1_import.h"
#include "FMI1/fmi1_import_type.h"

#include "FMUBase.h"



class FMUIntegrator;

/** 
 *  The FMI standard requires to define the macro MODEL_IDENTIFIER for each
 *  type of FMU seperately. This is not done here, because this class links
 *  dynamically during run-time.
 */


class __FMI_DLL FMILibraryFMU : public FMUBase
{

public:

	FMILibraryFMU( const std::string& fmuPath,
		       const std::string& tmpPath );

	~FMILibraryFMU();

	// Instantiate the FMU
	fmiStatus instantiate( const std::string& instanceName,
			       fmiBoolean loggingOn = fmi1_true );

	fmiStatus initialize();

	const fmiReal& getTime() const;
	void setTime( fmiReal time );
	void rewindTime( fmiReal deltaRewindTime );

	fmiStatus setValue( fmiValueReference valref, fmiReal& val );
	fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	//fmiStatus setValue( fmiValueReference valref, fmiBoolean& val );

	fmiStatus setValue( const std::string& name,  fmiReal val ); 

	fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const;
	fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const;

	//fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const;

	fmiStatus getValue( const std::string& name,  fmiReal& val ) const; 

	fmiValueReference getValueRef( const std::string& name ) const;

	fmiStatus getContinuousStates( fmiReal* val ) const;
	fmiStatus setContinuousStates( const fmiReal* val );

	fmiStatus getDerivatives( fmiReal* val ) const;

	fmiStatus getEventIndicators( fmiReal* eventsind ) const; 
  
	fmiStatus integrate( fmiReal tend, unsigned int nsteps );
	fmiStatus integrate( fmiReal tend, double deltaT=1E-5 );

	void raiseEvent();
	void handleEvents( fmiTime tstop, bool completedIntegratorStep );

	std::size_t nStates() const;
	std::size_t nEventInds() const;
	std::size_t nValueRefs() const;

// 	void logger(fmiStatus status, const std::string& msg) const;
// 	void logger(fmiStatus status, const char* msg) const;

	static void jm_logger( jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message );
	static void fmi_logger( fmi1_component_t m, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ... );

private:

	// Prevent calling the default constructor.
	FMILibraryFMU() {}

	// FMI version independent library context. 
	fmi_import_context_t* context_;

	// FMU version 1.0 object.
	fmi1_import_t* fmu_;

	// Name of the FMU instance.
 	std::string instanceName_;

	// Integrator.
 	FMUIntegrator* integrator_;

	std::size_t nStateVars_; // Number of state variables.
	std::size_t nEventInds_; // Number of event indivators.
	std::size_t nValueRefs_; // Number of value references.

	// Map of variable names and value references.
	typedef std::map< std::string, fmi1_value_reference_t > ValueReferenceMap;
	ValueReferenceMap varMap_; 

	// For internal timing.
	fmi1_real_t time_;
	fmi1_real_t tnextevent_;

	// For event handling.
	fmi1_event_info_t* eventinfo_; 
	fmi1_real_t* eventsind_;
	fmi1_real_t* preeventsind_;
	
	// For event handling.
	fmi1_boolean_t callEventUpdate_;
	fmi1_boolean_t stateEvent_;
	fmi1_boolean_t timeEvent_;

 	void readModelDescription();

 	static const unsigned int maxEventIterations_ = 5;

};

#endif
