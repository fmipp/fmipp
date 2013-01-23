#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <cassert>
#include <limits>

#include "FMILibraryFMU.h"
#include "ModelManager.h"
#include "FMUIntegrator.h"

static jm_callbacks callbacks = { malloc, calloc, realloc, free, FMILibraryFMU::jm_logger, jm_log_level_all, 0 };
static fmi1_callback_functions_t functions = { FMILibraryFMU::fmi_logger, calloc, free };

using namespace std;


FMILibraryFMU::FMILibraryFMU( const string& fmuPath,
			      const string& tmpPath )
{
#ifdef FMI_DEBUG
	cout << "[FMILibraryFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	// Create fmi_import_context_t structure.
	context_ = fmi_import_allocate_context( &callbacks );

	// Unzip FMU specified by 'fmuPath' into directory 'tmpPath' and parse XML to get FMI standard version.
	fmi_version_enu_t version = fmi_import_get_fmi_version( context_, fmuPath.c_str(), tmpPath.c_str() );

	// Check for correct version.
	assert( version == fmi_version_1_enu );

	// Parse FMI 1.0 XML file found in directory 'tmpPath'.
	fmu_ = fmi1_import_parse_xml( context_, tmpPath.c_str() );

	// Check if FMU supports Model Exchange.
	assert( fmi1_fmu_kind_enu_me == fmi1_import_get_fmu_kind( fmu_ ) );

	// Create a C-API struct. The C-API struct is a placeholder for the FMI DLL functions.
	jm_status_enu_t status = fmi1_import_create_dllfmu( fmu_, functions, 0 );

	// Check success C-API struct creation.
	assert( jm_status_error != status );

	// Retrieve information about the FMU.
	readModelDescription();

	// Initialize integrator.
	integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
	cout << "[FMILibraryFMU::ctor] DONE." << endl;
#endif
}


FMILibraryFMU::~FMILibraryFMU()
{
        if ( context_ ) {
		fmi_import_free_context( context_ );
	}

	if ( fmu_ ) {
		fmi1_import_terminate( fmu_ );
		fmi1_import_free_model_instance( fmu_ );
		fmi1_import_destroy_dllfmu( fmu_ );
		fmi1_import_free( fmu_ );
	}

	if ( integrator_ ) {
		delete integrator_;
	}
}


void FMILibraryFMU::readModelDescription()
{
	// Get number of continuous states.
 	nStateVars_ = fmi1_import_get_number_of_continuous_states( fmu_ );

	// Get number of event indicators.
	nEventInds_ = fmi1_import_get_number_of_event_indicators( fmu_ );

	// Get variable list.
	fmi1_import_variable_list_t * varList =	fmi1_import_get_variable_list( fmu_ );

	// Get number of variables.
 	nValueRefs_ = fmi1_import_get_variable_list_size( varList );

	// Fill map of variable names and value references.
	fmi1_import_variable_t* var;
	string varName;
	fmi1_value_reference_t varValRef;
	for ( size_t i = 0; i < nValueRefs_; ++i )
	{
		var = fmi1_import_get_variable( varList, i );
		varName = fmi1_import_get_variable_name( var );
		varValRef = fmi1_import_get_variable_vr( var );
		varMap_.insert( make_pair( varName, varValRef ) );
	}

	// Free variable list.
	fmi1_import_free_variable_list( varList );
}


fmiStatus FMILibraryFMU::instantiate( const string& instanceName,
				      fmiBoolean loggingOn )
{
	instanceName_ = instanceName;

#ifdef FMI_DEBUG
	// General information ...
	cout << "[FMILibraryFMU::instantiate] Types Platform: " << fmi_import_get_model_types_platform( fmu_)
	     << ", FMI Version:  " << fmi1_import_get_model_version( fmu_ ) << endl;
	fflush( stdout );
#endif

	// Basic settings: @todo from a menu.
	time_ = 0.;
	tnextevent_ = numeric_limits<fmiTime>::infinity();

	// Memory allocation.
#ifdef FMI_DEBUG
	cout << "[FMILibraryFMU::instantiate] nStateVars_ = " << nStateVars_
	     << " -  nEventInds_ = " << nEventInds_ << endl;
	fflush( stdout );
#endif

	eventsind_    = new fmi1_real_t[nEventInds_];
	preeventsind_ = new fmi1_real_t[nEventInds_];

	for ( size_t i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmi1_event_info_t;

	const char* guid = fmi1_import_get_GUID( fmu_ );

#ifdef FMI_DEBUG
	cout << "[FMILibraryFMU::instantiate] GUID = " << guid << endl; fflush( stdout );
	cout << "[FMILibraryFMU::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
#endif

	jm_status_enu_t jm_status = fmi1_import_instantiate_model( fmu_, instanceName_.c_str() );

	if ( jm_status_error == jm_status ) {
#ifdef FMI_DEBUG
		cout << "[FMILibraryFMU::instantiate] instantiateModel failed. " << endl; fflush( stdout );
#endif
		return fmiError;
	}

	fmi1_status_t fmi_status = fmi1_import_set_debug_logging( fmu_, loggingOn );

#ifdef FMI_DEBUG
	cout << "[FMILibraryFMU::instantiate] DONE. status = " << fmi_status << endl; fflush( stdout );
#endif

	return static_cast<fmiStatus>( fmi_status );
}


fmiStatus FMILibraryFMU::initialize()
{
	if( 0 == fmu_ ) {
		return fmiError;
	}

	// Basic settings.
	fmi1_import_set_time( fmu_, time_ );
	fmi1_status_t status = fmi1_import_initialize( fmu_, fmi1_false, 1e-5, eventinfo_ );

	stateEvent_ = fmi1_false;
	timeEvent_ = fmi1_false;
	callEventUpdate_ = fmi1_false;

	return static_cast<fmiStatus>( status );
}


const fmiReal& FMILibraryFMU::getTime() const
{
	return time_;
}


void FMILibraryFMU::setTime( fmiReal time )
{
	time_ = time;
	fmi1_import_set_time( fmu_, time_ );
}


void FMILibraryFMU::rewindTime( fmiReal deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmi1_import_set_time( fmu_, time_ );
}


fmiStatus FMILibraryFMU::setValue( fmiValueReference valref, fmiReal& val )
{
	fmi1_status_t status = fmi1_import_set_real( fmu_, &valref, 1, &val );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::setValue( fmiValueReference* valref, fmiReal* val, size_t ival )
{
	fmi1_status_t status = fmi1_import_set_real( fmu_, valref, ival, val );
	return static_cast<fmiStatus>( status );
}


// fmiStatus FMILibraryFMU::setValue( fmiValueReference valref, fmiBoolean& val )
// {
// 	fmi1_status_t status = fmi1_import_set_real( fmu_, &valref, 1, &val );
// 	return static_cast<fmiStatus>( status );
// }


fmiStatus FMILibraryFMU::setValue( const string& name, fmiReal val )
{
	ValueReferenceMap::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		fmi1_status_t status = fmi1_import_set_real( fmu_, &it->second, 1, &val );
		return static_cast<fmiStatus>( status );
	} else {
		return fmiDiscard;
	}
}


fmiStatus FMILibraryFMU::getValue( fmiValueReference valref, fmiReal& val ) const
{
	fmi1_status_t status = fmi1_import_get_real( fmu_, &valref, 1, &val );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const
{
	fmi1_status_t status = fmi1_import_get_real( fmu_, valref, ival, val );
	return static_cast<fmiStatus>( status );
}


// fmiStatus FMILibraryFMU::getValue( fmiValueReference valref, fmiBoolean& val ) const
// {
// 	fmi1_status_t status = fmi1_import_get_boolean( fmu_, &valref, 1, &val );
// 	return static_cast<fmiStatus>( status );
// }


fmiStatus FMILibraryFMU::getValue( const string& name,  fmiReal& val ) const
{
	ValueReferenceMap::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		fmi1_status_t status = fmi1_import_get_real( fmu_, &it->second, 1, &val );
		return static_cast<fmiStatus>( status );
	} else {
		return fmiDiscard;
	}
}


fmiValueReference FMILibraryFMU::getValueRef( const string& name ) const
{
	ValueReferenceMap::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		return static_cast<fmiValueReference>( it->second );
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMILibraryFMU::getContinuousStates( fmiReal* val ) const
{
	fmi1_status_t status = fmi1_import_get_continuous_states( fmu_, val, nStateVars_ );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::setContinuousStates( const fmiReal* val )
{
	fmi1_status_t status = fmi1_import_set_continuous_states( fmu_, val, nStateVars_ );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::getDerivatives( fmiReal* val ) const
{
	fmi1_status_t status = fmi1_import_get_derivatives( fmu_, val, nStateVars_ );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::getEventIndicators( fmiReal* eventsind ) const
{
	fmi1_status_t status = fmi1_import_get_event_indicators( fmu_, eventsind, nEventInds_ );
	return static_cast<fmiStatus>( status );
}


fmiStatus FMILibraryFMU::integrate( fmiReal tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	double deltaT = ( tstop - time_ ) / nsteps;
	return integrate( tstop, deltaT );
}


fmiStatus FMILibraryFMU::integrate( fmiReal tstop, double deltaT )
{
	assert( deltaT > 0 );
	handleEvents( 0, false );

	fmiStatus status = fmiOK;

	integrator_->integrate( ( tstop - time_ ), ( tstop - time_ )/deltaT );

	setTime( tstop );

	return status;
}


void FMILibraryFMU::raiseEvent()
{
 	stateEvent_ = fmi1_true;
}


void FMILibraryFMU::handleEvents( fmiTime tStop, bool completedIntegratorStep )
{
	// Get event indicators.
	for( size_t i = 0; i < nEventInds_; ++i ) preeventsind_[i] = eventsind_[i];

	getEventIndicators( eventsind_ );

	for( size_t i = 0; i < nEventInds_; ++i ) stateEvent_ = stateEvent_ || (preeventsind_[i] * eventsind_[i] < 0);

	timeEvent_ = ( time_ > tnextevent_ );

	// Inform the model about an accepted step.
	if( true == completedIntegratorStep ) fmi1_import_completed_integrator_step( fmu_, &callEventUpdate_ );

	if( callEventUpdate_ || stateEvent_ || timeEvent_ ) {
		eventinfo_->iterationConverged = fmiFalse;

		// Event time is identified and stored values get updated.
		unsigned int cnt = 0;
		while( ( fmiFalse == eventinfo_->iterationConverged ) && ( cnt < maxEventIterations_ ) )
		{
			fmi1_import_eventUpdate( fmu_, fmi1_true, eventinfo_ );
			cnt++;
		}

		// Next time event is identified.
		if( eventinfo_->upcomingTimeEvent ) {
			tnextevent_ = ( eventinfo_->nextEventTime < tStop ) ? eventinfo_->nextEventTime : tStop;
		}
		stateEvent_ = fmiFalse;
	}
}


std::size_t FMILibraryFMU::nStates() const
{
 	return nStateVars_;
}


std::size_t FMILibraryFMU::nEventInds() const
{
 	return nEventInds_;
}


std::size_t FMILibraryFMU::nValueRefs() const
{
 	return nValueRefs_;
}


void FMILibraryFMU::fmi_logger( fmi1_component_t m, fmi1_string_t instanceName,
				fmi1_status_t status, fmi1_string_t category,
				fmi1_string_t message, ... )
{
	char msg[4096];
	char buf[4096];
	int len;
	int capacity;

	va_list ap;
	va_start(ap, message);
	capacity = sizeof(buf) - 1;
#if defined(_MSC_VER) && _MSC_VER>=1400
	len = _snprintf_s(msg, capacity, _TRUNCATE, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf_s(buf, capacity, _TRUNCATE, msg, ap);
	if (len < 0) goto fail;
#elif defined(WIN32)
	len = _snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#else
	len = snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#endif
	/* append line break */
	buf[len] = '\n';
	buf[len + 1] = 0;
	va_end(ap);

	switch (status) {
	case fmiFatal:
		printf(buf);
		break;
	default:
		printf(buf);
		break;
	}
	return;

fail:
	printf("logger failed, message too long?");
}


void FMILibraryFMU::jm_logger( jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message )
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}
