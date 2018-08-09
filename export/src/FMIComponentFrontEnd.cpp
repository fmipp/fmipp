// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIComponentFrontEnd.cpp

// Platform-specific headers.

#ifdef MINGW // Extra definitions for MINGW.
#define JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE		0x2000
#define _WIN32_WINNT 0x0502 // necessary for function CreateJobObject in Windows
#endif

#ifdef WIN32 // Visual Studio C++ & MinGW GCC use both the same Windows APIs.
#include <windows.h>
#include "TlHelp32.h"
#include "tchar.h"
#undef max // Bug fix for numeric_limits::max.
#else // Use POSIX functionalities for Linux.
#include <signal.h>
#include <csignal>
#include <cerrno>
#endif

// Standard includes.
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Boost includes.
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project-specific include files.
#include "export/include/FMIComponentFrontEnd.h"
#include "export/include/SHMMaster.h"
#include "export/include/ScalarVariable.h"
#include "export/include/HelperFunctions.h"
#include "export/include/IPCMasterLogger.h"
#include "import/base/include/ModelDescription.h"


using namespace std;

// Forward declaration.
template<typename T>
void initializeScalar( ScalarVariable<T>* scalar,
					const ModelDescription::Properties* description,
					const string& xmlTypeTag,
					FMIComponentFrontEnd* frontend );


FMIComponentFrontEnd::FMIComponentFrontEnd() :
	ipcMaster_( 0 ), ipcLogger_( 0 ),
	currentCommunicationPoint_( 0 ), communicationStepSize_( 0 ), stopTime_( 0 ),
	stopTimeDefined_( 0 ), enforceTimeStep_( 0 ), rejectStep_( 0 ),
	slaveHasTerminated_( 0 ), pid_( 0 ), comPointPrecision_( 1e-9 )
{}


FMIComponentFrontEnd::~FMIComponentFrontEnd()
{
	if ( ipcMaster_ ) {
		if ( false == *slaveHasTerminated_ ) killApplication();
		delete ipcMaster_;
	}

	if ( ipcLogger_ ) delete ipcLogger_;
}


fmi2Status
FMIComponentFrontEnd::setReal( const fmi2ValueReference& ref, const fmi2Real& val )
{
	// Search for value reference.
	RealMap::iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		stringstream err;
		err << "setReal - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Check if scalar is defined as input or parameter.
	Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::Causality::input ) &&      // FMI 1.0 & 2.0
	     ( causality != ScalarVariableAttributes::Causality::parameter ) &&  // FMI 2.0 only
	     ( causality != ScalarVariableAttributes::Causality::internal ) )    // FMI 1.0 only
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::setInteger( const fmi2ValueReference& ref, const fmi2Integer& val )
{
	// Search for value reference.
	IntegerMap::iterator itFind = integerScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == integerScalarMap_.end() )
	{
		stringstream err;
		err << "setInteger - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Check if scalar is defined as input or parameter.
	Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::Causality::input ) &&      // FMI 1.0 & 2.0
	     ( causality != ScalarVariableAttributes::Causality::parameter ) &&  // FMI 2.0 only
	     ( causality != ScalarVariableAttributes::Causality::internal ) )    // FMI 1.0 only
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::setBoolean( const fmi2ValueReference& ref, const fmi2Boolean& val )
{
	// Search foreach value reference.
	BooleanMap::iterator itFind = booleanScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == booleanScalarMap_.end() )
	{
		stringstream err;
		err << "setBoolean - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Check if scalar is defined as input or parameter.
	Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::Causality::input ) &&      // FMI 1.0 & 2.0
	     ( causality != ScalarVariableAttributes::Causality::parameter ) &&  // FMI 2.0 only
	     ( causality != ScalarVariableAttributes::Causality::internal ) )    // FMI 1.0 only
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::setString( const fmi2ValueReference& ref, const fmi2String& val )
{
	// Search for value reference.
	StringMap::iterator itFind = stringScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == stringScalarMap_.end() )
	{
		stringstream err;
		err << "setString - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Check if scalar is defined as input or parameter.
	Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::Causality::input ) &&      // FMI 1.0 & 2.0
	     ( causality != ScalarVariableAttributes::Causality::parameter ) &&  // FMI 2.0 only
	     ( causality != ScalarVariableAttributes::Causality::internal ) )    // FMI 1.0 only
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Set value.
	itFind->second->value_ = val; // Attention: fmi2String <-> string!!!

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::getReal( const fmi2ValueReference& ref, fmi2Real& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		stringstream err;
		err << "getReal - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		val = 0;
		return fmi2Warning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::getInteger( const fmi2ValueReference& ref, fmi2Integer& val )
{
	// Search for value reference.
	IntegerMap::const_iterator itFind = integerScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == integerScalarMap_.end() )
	{
		stringstream err;
		err << "getInteger - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		val = 0;
		return fmi2Warning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::getBoolean( const fmi2ValueReference& ref, fmi2Boolean& val )
{
	// Search for value reference.
	BooleanMap::const_iterator itFind = booleanScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == booleanScalarMap_.end() )
	{
		stringstream err;
		err << "getBoolean - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		val = 0;
		return fmi2Warning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::getString( const fmi2ValueReference& ref, fmi2String& val )
{
	// Search for value reference.
	StringMap::const_iterator itFind = stringScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == stringScalarMap_.end() )
	{
		stringstream err;
		err << "getString - unknown value reference: " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		val = 0;
		return fmi2Warning;
	}

	// Get value.
	val = itFind->second->value_.c_str(); // Attention: fmi2String <-> string!!!

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::getDirectionalDerivative( const fmi2ValueReference vUnknown_ref[],
					    size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown,
					    const fmi2Real dvKnown[], fmi2Real dvUnknown[] )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::instantiateSlave( const string& instanceName, const string& fmuGUID,
					const string& fmuLocation, fmi2Real timeout, fmi2Boolean visible )
{
	instanceName_ = instanceName;

	//logger( fmi2OK, "DEBUG", string( "build type = " ) + _FMIPP_BUILD_TYPE );

	// Trim FMU location path (just to be sure).
	string fmuLocationTrimmed = boost::trim_copy( fmuLocation );

	const string separator( "/" );
	string filePath;
	// Construct URI of XML model description file.
	string fileUrl = fmuLocationTrimmed + separator + string( "modelDescription.xml" );

	// Get the path of the XML model description file.
	if ( false == HelperFunctions::getPathFromUrl( fileUrl, filePath ) ) {
                stringstream err;
		err << "invalid input URL for XML model description file: " << fileUrl;
		logger( fmi2Fatal, "ABORT", err.str() );
		return fmi2Fatal;
	}

	logger( fmi2OK, "DEBUG", string( "XML model description file path = " ) + filePath );

	// Parse the XML model description file.
	ModelDescription modelDescription( filePath );

	// Check if parsing was successfull.
	if ( false == modelDescription.isValid() ) {
                stringstream err;
		err << "unable to parse XML model description file: " << filePath;
		logger( fmi2Fatal, "ABORT", err.str() );
		return fmi2Fatal;
	}


	// Check if GUID is consistent.
	if ( modelDescription.getGUID() != fmuGUID ) {
		logger( fmi2Fatal, "ABORT", "wrong GUID" );
		return fmi2Fatal;
	}

	size_t nRealScalars;
	RealCollection realScalars;

	size_t nIntegerScalars;
	IntegerCollection integerScalars;

	size_t nBooleanScalars;
	BooleanCollection booleanScalars;

	size_t nStringScalars;
	StringCollection stringScalars;

	// Parse number of model variables from model description.
	modelDescription.getNumberOfVariables( nRealScalars, nIntegerScalars, nBooleanScalars, nStringScalars );

	// Start application.
	/// \FIXME Allow to start applications remotely on other machines?
	if ( false == startApplication( &modelDescription, fmuLocationTrimmed ) ) {
		logger( fmi2Fatal, "ABORT", "unable to start external simulator application" );
		return fmi2Fatal;
	}

	// Create shared memory segment.
	/// \FIXME Allow other types of inter process communication.
#ifdef SHM_SEGMENT_NAME
	// If this flag is set, the name specified along with it is used for the name of the shared memory segment.
	string shmSegmentName = string( SHM_SEGMENT_NAME );
#else
	// Otherwise, use the process ID of the started application to generate the shared memory segment name.
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( pid_ );
#endif

	/// \FIXME Use more sensible estimate for the segment size.
	long unsigned int shmSegmentSize = 2048
		+ nRealScalars*sizeof(RealScalar)
		+ nIntegerScalars*sizeof(IntegerScalar)
		+ nBooleanScalars*sizeof(BooleanScalar)
		+ nStringScalars*2048;

	ipcLogger_ = new IPCMasterLogger( this );
	ipcMaster_ = IPCMasterFactory::createIPCMaster<SHMMaster>( shmSegmentName, shmSegmentSize, ipcLogger_ );

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	// Create variables used for internal frontend/backend syncing.
	if ( false == ipcMaster_->createVariable( "current_comm_point", currentCommunicationPoint_, 0. ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'master_time'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "comm_step_size", communicationStepSize_, 0. ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'next_step_size'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "stop_time", stopTime_, numeric_limits<fmi2Real>::max() ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'stop_time'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "stop_time_defined", stopTimeDefined_, false ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'stop_time_defined'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "enforce_step", enforceTimeStep_, false ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'enforce_step'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "reject_step", rejectStep_, false ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'reject_step'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "slave_has_terminated", slaveHasTerminated_, false ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'slave_has_terminated'" );
		return fmi2Fatal;
	}

	if ( false == ipcMaster_->createVariable( "fmu_type", fmuType_, static_cast<int>( modelDescription.getFMUType() ) ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'fmu_type'" );
		return fmi2Fatal;
	}

	// Create boolean variable that tells the backend if logging is on/off.
	bool* tmpLoggingOn = 0;
	if ( false == ipcMaster_->createVariable( "logging_on", tmpLoggingOn, loggingOn_ ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal variable 'logging_on'" );
		return fmi2Fatal;
	}

	// Create vector of real scalar variables.
	if ( false == ipcMaster_->createScalars( "real_scalars", nRealScalars, realScalars ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal vector 'real_scalars'" );
		return fmi2Fatal;
	}

	// Create vector of integer scalar variables.
	if ( false == ipcMaster_->createScalars( "integer_scalars", nIntegerScalars, integerScalars ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal vector 'integer_scalars'" );
		return fmi2Fatal;
	}

	// Create vector of boolean scalar variables.
	if ( false == ipcMaster_->createScalars( "boolean_scalars", nBooleanScalars, booleanScalars ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal vector 'boolean_scalars'" );
		return fmi2Fatal;
	}

	// Create vector of string scalar variables.
	if ( false == ipcMaster_->createScalars( "string_scalars", nStringScalars, stringScalars ) ) {
		logger( fmi2Fatal, "ABORT", "unable to create internal vector 'string_scalars'" );
		return fmi2Fatal;
	}

	initializeVariables( &modelDescription, realScalars, integerScalars, booleanScalars, stringScalars );

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::instantiate( const string& instanceName, const string& fmuGUID,
	const string& fmuResourceLocation, fmi2Boolean visible )
{
	// Trim FMU location path (just to be sure).
	string fmuResourceLocationTrimmed = boost::trim_copy( fmuResourceLocation );
	string fmuLocation;

	// Check if last character of resources directory location is a separator.
	// Then extract FMU location (assuming that the resources directory is a subdirectory of the unzipped FMU).
	const string separator( "/" );
	if ( 0 == separator.compare( fmuResourceLocationTrimmed.substr( fmuResourceLocationTrimmed.size() - 1 ) ) )
	{
		fmuLocation = fmuResourceLocationTrimmed.substr( 0, fmuResourceLocationTrimmed.size() - 11 );
	} else {
		fmuLocation = fmuResourceLocationTrimmed.substr( 0, fmuResourceLocationTrimmed.size() - 10 );
	}

	// Instantiate FMU.
	return instantiateSlave( instanceName, fmuGUID, fmuLocation, numeric_limits<fmi2Real>::quiet_NaN(), visible );
}


fmi2Status
FMIComponentFrontEnd::initializeSlave( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop )
{
	stringstream debugInfo;
	debugInfo << "initialize slave at time t = " << tStart;
	logger( fmi2OK, "DEBUG", debugInfo.str().c_str() );

	*currentCommunicationPoint_ = tStart;
	*stopTimeDefined_ = stopTimeDefined;
	*stopTime_ = tStop;

	// Synchronization point - give control to the slave.
	ipcMaster_->signalToSlave();

	// Let the slave initialize.

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	logger( fmi2OK, "DEBUG", "initialization done" );

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::resetSlave()
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::setRealInputDerivatives( const fmi2ValueReference vr[], size_t nvr,
					       const fmi2Integer order[], const fmi2Real value[])
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getRealOutputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						const fmi2Integer order[], fmi2Real value[])
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::doStep( fmi2Real comPoint, fmi2Real stepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint )
{
	stringstream debugInfo;
	debugInfo << "doStep" << " - communication point = " << comPoint << " - step size = " << stepSize;
	logger( fmi2OK, "DEBUG", debugInfo.str().c_str() );

	if ( true == *slaveHasTerminated_ ) {
		logger( fmi2Fatal, "DEBUG", "slave has terminated" );
		callStepFinished( fmi2Fatal );
		return fmi2Fatal;
	}

	// if ( 0. == stepSize ) { // This is an event.
	// 	/// \FIXME Nothing else to be done here?
	// 	callStepFinished( fmi2OK );
	// 	return fmi2OK;
	// }

	//cout << "\tcomPoint = " << comPoint << " - currentCommunicationPoint_ = " << *currentCommunicationPoint_ << endl; fflush(stdout);

	if ( fabs( *currentCommunicationPoint_ - comPoint ) > comPointPrecision_ ) {
		debugInfo.str( string() );
		debugInfo << "internal time (" << *currentCommunicationPoint_ << ") "
			  << "does not match communication point (" << comPoint << ")";
		logger( fmi2Discard, "DISCARD STEP", debugInfo.str().c_str() );
		callStepFinished( fmi2Discard );
		return fmi2Discard;
	}

	//cout << "\tstepSize = " << stepSize << " - communicationStepSize_ = " << *communicationStepSize_ << endl; fflush(stdout);

	if ( true == *enforceTimeStep_ )
	{
		if ( stepSize != *communicationStepSize_ ) {
			logger( fmi2Discard, "DISCARD STEP", "enforce time step: wrong step size" );
			callStepFinished( fmi2Discard );
			return fmi2Discard;
		} else {
			logger( fmi2OK, "DEBUG", "enforce time step: correct step size" );
		}
		*enforceTimeStep_ = false; // Reset flag.
	} else {
		*communicationStepSize_ = stepSize;
	}

	logger( fmi2OK, "DEBUG", "start synchronization with slave ..." );

	// Synchronization point - give control to slave and let it do its work ...
	ipcMaster_->signalToSlave();

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	logger( fmi2OK, "DEBUG", "... DONE" );

	if ( true == *rejectStep_ ) {
		*rejectStep_ = false; // Reset flag.
		logger( fmi2Discard, "DISCARD STEP", "step rejected by slave" );
		callStepFinished( fmi2Discard );
		return fmi2Discard;
	}

	// Advance time.
	*currentCommunicationPoint_ += stepSize;

	callStepFinished( fmi2OK );

	return fmi2OK;
}


fmi2Status
FMIComponentFrontEnd::cancelStep()
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getStatus( const fmi2StatusKind s, fmi2Status* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getRealStatus( const fmi2StatusKind s, fmi2Real* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getIntegerStatus( const fmi2StatusKind s, fmi2Integer* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getBooleanStatus( const fmi2StatusKind s, fmi2Boolean* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getStringStatus( const fmi2StatusKind s, fmi2String* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::getFMUState( fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::setFMUState( fmi2FMUstate fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::freeFMUState( fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::serializedFMUStateSize( fmi2FMUstate fmuState, size_t* size )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::serializeFMUState( fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
FMIComponentFrontEnd::deserializeFMUState( const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


void
FMIComponentFrontEnd::logger( fmi2Status status, const string& category, const string& msg )
{
	if ( ( fmi2OK == status ) && ( fmi2False == loggingOn_ ) ) return;

	if ( 0 != fmiFunctions_ && 0 != fmiFunctions_->logger )
		fmiFunctions_->logger( static_cast<fmiComponent>( this ), instanceName_.c_str(),
			static_cast<fmiStatus>( status ), category.c_str(), msg.c_str() );

	if ( 0 != fmi2Functions_ && 0 != fmi2Functions_->logger )
		fmi2Functions_->logger( fmi2Functions_->componentEnvironment,
			instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}


const string
FMIComponentFrontEnd::getMIMEType() const
{
	return mimeType_;
}


bool
FMIComponentFrontEnd::startApplication( const ModelDescription* modelDescription, const string& fmuLocation )
{
	using namespace ModelDescriptionUtilities;
	using namespace boost::filesystem;

	// Retrieve the "entryPoint" (FMI 1.0 only).
	// The input file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	string inputFileUrl = modelDescription->getEntryPoint();

	if ( 0 != inputFileUrl.size() ) HelperFunctions::processURI( inputFileUrl, fmuLocation );

	// Copy additional input files (specified in XML description elements
	// of type  "Implementation.CoSimulation_Tool.Model.File").
	if ( false == copyAdditionalInputFiles( modelDescription, fmuLocation ) ) return false;

	// Check for additional command line arguments (as part of optional vendor annotations).
	string preArguments;
	string mainArguments;
	string postArguments;
	string executableUrl;
	string entryPointUrl;
	if ( false == parseAdditionalArguments( modelDescription, preArguments, mainArguments, postArguments, executableUrl, entryPointUrl ) ) {
		logger( fmi2Fatal, "ABORT", "incompatible model description" );
		return false;
	}

	// The input file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	HelperFunctions::processURI( executableUrl, fmuLocation );

	if ( 0 != entryPointUrl.size() ) { // Overwrite previous entry point.
		HelperFunctions::processURI( entryPointUrl, fmuLocation );
		inputFileUrl = entryPointUrl;
	}

	// Extract application name from MIME type or special vendor annotation.
	string applicationName( "unknown_application" );
	if ( 1 == modelDescription->getVersion() && true == executableUrl.empty() ) {

		const string& mimeType = modelDescription->getMIMEType();
		if ( mimeType.substr( 0, 14 ) != string( "application/x-" ) ) {
			string err = string( "Incompatible MIME type: " ) + mimeType;
			logger( fmi2Fatal, "ABORT", err );
			return false;
		}
		applicationName = mimeType.substr( 14 );
	} else {
		if ( false == HelperFunctions::getPathFromUrl( executableUrl, applicationName ) ) {
			logger( fmi2Fatal, "ABORT", "invalid input URI for executable" );
			return false;
		}
	}

	// Retrieve path to entry point.
	string strFilePath;
	if ( 0 != inputFileUrl.size() && false == HelperFunctions::getPathFromUrl( inputFileUrl, strFilePath ) ) {
		logger( fmi2Fatal, "ABORT", "invalid input URL for input file (entry point):" );
		return false;
	}

	// Get working directory for the external application (by default, path to entry point without file name.
	path workingDirectoryPath( strFilePath );
	workingDirectoryPath.remove_filename();

	// Check if entry point path exists and use it as working directory.
	if ( false == exists( workingDirectoryPath ) ) {
		string warning = "The path specified for the FMU's entry point does not exist: ";
		warning += string( "\"" ) + workingDirectoryPath.string() + string( "\"\n" );

		// Try to use the path of the executable as working directory instead.
		path executablePath( applicationName );
		executablePath.remove_filename();
		if ( true == exists( executablePath ) ) {
			warning += "Use directory of main application as working directory instead.";
			logger( fmi2Warning, "WARNING", warning );
			workingDirectoryPath = executablePath;
		} else { // If all else fails, use the current directory as working directory.
			warning += "Use current directory as working directory instead.";
			logger( fmi2Warning, "WARNING", warning );
			workingDirectoryPath = current_path();
		}
	}

#ifdef WIN32

	job_ = CreateJobObject( 0, 0 );

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
	info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	SetInformationJobObject( job_, JobObjectExtendedLimitInformation, &info, sizeof( info ) );

	string separator( " " );
	string quotationMark( "\"" ); // The file path has to be put bewteen quotation marks, in case it contains spaces!
	string strCmdLine;
	if ( true == mainArguments.empty() ) {
		// Unless there is no main argument provided explicitely, use the input file path as main command line argument.
		strCmdLine = applicationName + separator + preArguments + separator +
			quotationMark + strFilePath + quotationMark + separator + postArguments;
	} else {
		strCmdLine = applicationName + separator + preArguments + separator +
			mainArguments + separator + postArguments;
	}

	LPTSTR cmdLine = HelperFunctions::copyStringToTCHAR( strCmdLine );

	// The full path to the current directory for the process.
	LPTSTR currDir = HelperFunctions::copyStringToTCHAR( workingDirectoryPath.string() );

	// Specifies the window station, desktop, standard handles, and appearance of
	// the main window for a process at creation time.
	STARTUPINFO startupInfo;
	memset( &startupInfo, 0, sizeof( startupInfo ) );

	// Contains information about a newly created process and its primary thread.
	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof( processInfo ) );

	// Create the process.
	if ( false == CreateProcess( NULL, cmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS,
				     NULL, currDir, &startupInfo, &processInfo ) )
	{
		CloseHandle( job_ );

		// The process could not be started ...
		stringstream err;
		err << "CreateProcess() failed to start process"
		    << " - ERROR: " << GetLastError()
		    << " - cmdLine: >>>" << cmdLine << "<<<"
		    << " - applicationName: >>>" << applicationName << "<<<";
		logger( fmi2Fatal, "ABORT", err.str() );
		return false;
	} else {
		BOOL status = AssignProcessToJobObject( job_, processInfo.hProcess );
        if ( ( false == status ) &&
		     ( ResumeThread( processInfo.hThread) == (DWORD)-1 ) ) {
			logger( fmi2Fatal, "ABORT", "Assigning process to job object failed." );
			return false;
        }
	}

	delete cmdLine;
	delete currDir;

	CloseHandle( processInfo.hProcess ); // This does not kill the process!
	CloseHandle( processInfo.hThread ); // This does not kill the thread!

	pid_ = static_cast<int>( processInfo.dwProcessId );

	stringstream debug;
	debug << "started external application. PID = " << pid_ << " - command = '" << strCmdLine << "'";
	logger( fmi2OK, "DEBUG", debug.str() );

#else
	// Creation of a child process with known PID requires to use fork() under Linux.
	pid_ = fork();

	string err;

	switch ( pid_ )
	{

	case -1: // Error.

		err = string( "fork() failed." );
		logger( fmi2Fatal, "ABORT", err );
		return false;

	case 0: // Child process.

		// Move this process into its own process group.
		if ( ( -1 == setpgid( 0, 0 ) ) && ( true == loggingOn_ ) ) {
			int err = errno;
			stringstream msg;
			msg << "setpgid failed with error code " << err
			    << " (EACCES = " << EACCES << ", EINVAL = " << EINVAL
			    << ", EPERM = " << EPERM << ", ESRCH = " << ESRCH << ")";
			logger( fmi2Warning, "WARNING", msg.str() );
		}

		// Change to new working directory.
		try {
			current_path( workingDirectoryPath );
		} catch( filesystem_error err ) {
			logger( fmi2Fatal, "ABORT", err.what() );
			return false;
		}

		// After unzipping the FMU, the executable's permission may be wrong (i.e., no execute permission).
		// Therefore, it is a good idea to change the executable's permission before starting the process.
		try {
			path applicationPath( applicationName );
			permissions( applicationPath, owner_all );
		} catch( filesystem_error err ) {
			logger( fmi2Warning, "WARNING", err.what() );
			//return false;
		}

		// Start the process. execl(...) replaces the current process image with the new process image.
		if ( true == mainArguments.empty() ) {
		// Unless there is no main argument provided explicitely, use the input file path as main command line argument.
			if ( preArguments.empty() && postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(), strFilePath.c_str(), NULL );
			} else if ( preArguments.empty() && !postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					strFilePath.c_str(), postArguments.c_str(), NULL );
			} else if ( !preArguments.empty() && postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					preArguments.c_str(), strFilePath.c_str(), NULL );
			} else if ( !preArguments.empty() && !postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					preArguments.c_str(), strFilePath.c_str(), postArguments.c_str(), NULL );
			}
		} else {
			if ( preArguments.empty() && postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(), mainArguments.c_str(), NULL );
			} else if ( preArguments.empty() && !postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					mainArguments.c_str(), postArguments.c_str(), NULL );
			} else if ( !preArguments.empty() && postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					preArguments.c_str(), mainArguments.c_str(), NULL );
			} else if ( !preArguments.empty() && !postArguments.empty() ) {
				execlp( applicationName.c_str(), applicationName.c_str(),
					preArguments.c_str(), mainArguments.c_str(), postArguments.c_str(), NULL );
			}
		}

		// execl(...) should not return.
		err = string( "execlp(...) failed. application name = " ) + applicationName;
		logger( fmi2Fatal, "ABORT", err );
		return false;

	default: // Parent process: pid_ now contains the child's PID.
		break;

	}

	stringstream info;
	info << "started external application. PID = " << pid_;
	logger( fmi2OK, "DEBUG", info.str() );

#endif

	return true;
}


void
FMIComponentFrontEnd::killApplication()
{
#ifdef WIN32

	CloseHandle( job_ );

#else

	// Try to terminate the child process (and all other processes in its process group) the nice way.
	if ( -1 == kill( -pid_, SIGTERM ) )
	{
		int errsv = errno;
		stringstream err;
		err << "unable to kill process (PID = " << pid_ << ") with SIGTERM. ERROR: "
		    << strerror( errsv ) << " --> process will be killed using SIGKILL signal.";
		logger( fmi2Warning, "WARNING", err.str() );

		// The nice way didn't work, hence we make short work of the process (SIGKILL).
		kill( -pid_, SIGKILL );

		logger( fmi2OK, "DEBUG", "terminated external application." );
	}

#endif
}


void
FMIComponentFrontEnd::initializeVariables( const ModelDescription* modelDescription,
					   RealCollection& realScalars,
					   IntegerCollection& integerScalars,
					   BooleanCollection& booleanScalars,
					   StringCollection& stringScalars )
{
	RealCollection::iterator itRealScalar = realScalars.begin();
	IntegerCollection::iterator itIntegerScalar = integerScalars.begin();
	BooleanCollection::iterator itBooleanScalar = booleanScalars.begin();
	StringCollection::iterator itStringScalar = stringScalars.begin();

	const string xmlRealTag( "Real" );
	const string xmlIntegerTag( "Integer" );
	const string xmlBooleanTag( "Boolean" );
	const string xmlStringTag( "String" );

	const ModelDescription::Properties& modelVariables = modelDescription->getModelVariables();

	BOOST_FOREACH( const ModelDescription::Properties::value_type &v, modelVariables )
	{
		if ( v.second.find( xmlRealTag ) != v.second.not_found() )
		{
			initializeScalar( *itRealScalar, &v.second, xmlRealTag, this );
			realScalarMap_[(*itRealScalar)->valueReference_] = *itRealScalar;
			++itRealScalar;
			continue;
		}
		else if ( v.second.find( xmlIntegerTag ) != v.second.not_found() )
		{
			initializeScalar( *itIntegerScalar, &v.second, xmlIntegerTag, this );
			integerScalarMap_[(*itIntegerScalar)->valueReference_] = *itIntegerScalar;
			++itIntegerScalar;
			continue;
		}
		else if ( v.second.find( xmlBooleanTag ) != v.second.not_found() )
		{
			initializeScalar( *itBooleanScalar, &v.second, xmlBooleanTag, this );
			booleanScalarMap_[(*itBooleanScalar)->valueReference_] = *itBooleanScalar;
			++itBooleanScalar;
			continue;
		}
		else if ( v.second.find( xmlStringTag ) != v.second.not_found() )
		{
			initializeScalar( *itStringScalar, &v.second, xmlStringTag, this );
			stringScalarMap_[(*itStringScalar)->valueReference_] = *itStringScalar;
			++itStringScalar;
			continue;
		} else {
			stringstream err;
			err << "[FMIComponentFrontEnd] Type not supported: " << v.second.back().first;
			logger( fmi2Fatal, "ABORT", err.str() );
		}
	}
}


template<typename T>
void initializeScalar( ScalarVariable<T>* scalar,
	const ModelDescription::Properties* description,
	const string& xmlTypeTag,
	FMIComponentFrontEnd* frontend )
{
	using namespace ScalarVariableAttributes;
	using namespace ModelDescriptionUtilities;

	const Properties& attributes = getAttributes( *description );

	scalar->setName( attributes.get<string>( "name" ) );
	scalar->valueReference_ = attributes.get<int>( "valueReference" );
	scalar->causality_ = hasChild( attributes, "causality" ) ?
		getCausality( attributes.get<string>( "causality" ) ) : defaultCausality();
	scalar->variability_ = hasChild( attributes, "variability" ) ?
		getVariability( attributes.get<string>( "variability" ) ) : defaultVariability();

	if ( hasChildAttributes( *description, xmlTypeTag ) )
	{
		const Properties& properties = getChildAttributes( *description, xmlTypeTag );

		if ( properties.find( "start" ) != properties.not_found() )
			scalar->value_ = properties.get<T>( "start" );
	}

	/// \FIXME What about the remaining properties?

	stringstream info;
	info << "initialized scalar variable." <<
		" name = " << scalar->name_ <<
		" - type = " << xmlTypeTag <<
		" - valueReference = " << scalar->valueReference_ <<
		" - causality = " << scalar->causality_ <<
		" - variability = " << scalar->variability_ <<
		" - value = " << scalar->value_;
	frontend->logger( fmi2OK, "DEBUG", info.str() );

}
