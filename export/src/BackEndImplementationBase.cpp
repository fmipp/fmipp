#include "export/include/BackEndImplementationBase.h"
#include <fstream>
#include <iostream>


BackEndImplementationBase::BackEndImplementationBase() :
	backend_( 0 )
{}


BackEndImplementationBase::~BackEndImplementationBase()
{
	if ( 0 != backend_ ) delete backend_;
}


int
BackEndImplementationBase::initializeBase( int argc, const char* argv[] )
{
	// Special usage of the backend: Only write the names of the scalar
	// variables (inputs, outputs, parameters) to files, then exit.
	if ( ( 2 == argc ) && ( 0 == strcmp( argv[1], "--only-write-variable-names" ) ) )
	{
		initializeScalarVariables();
		writeScalarVariableNamesToFile();
		return 1;
	}
	
	fmiStatus initParamsStatus = fmiOK;
	fmiStatus initInputsStatus = fmiOK;
	fmiStatus initOutputsStatus = fmiOK;
	fmiStatus getParamsStatus = fmiOK;
	fmiStatus setParamsStatus = fmiOK;

	try
	{
		// Instantiate new backend.
		backend_ = new FMIComponentBackEnd;
		
		// Init backend.
		backend_->startInitialization();

		// User defined initialization.
		initializeScalarVariables();
		initializeBackEnd( argc, argv );
	
		initParamsStatus = initParameters();
		initInputsStatus = initInputs();
		initOutputsStatus = initOutputs();
		
		getParamsStatus = getParameters();
		initializeParameterValues();
		setParamsStatus = setParameters();

		syncTime_ = backend_->getCurrentCommunicationPoint();
		lastSyncTime_ = syncTime_;	

		backend_->endInitialization();
	}
	catch (...) { return -1; }
	
	if ( ( initParamsStatus != fmiOK ) || ( initInputsStatus != fmiOK ) || 
		 ( initOutputsStatus != fmiOK ) || ( getParamsStatus != fmiOK ) ||
		 ( setParamsStatus != fmiOK ) ) 
		return -1;
	
	return 0;
}


int
BackEndImplementationBase::doStepBase()
{
	static fmiStatus getParamsStatus = fmiOK;
	static fmiStatus getInputsStatus = fmiOK;
	static fmiStatus setOutputsStatus = fmiOK;
	static int stepStatus = 0;

	try
	{
		backend_->waitForMaster();

		syncTime_ = backend_->getCurrentCommunicationPoint() + backend_->getCommunicationStepSize();

		getParamsStatus = getParameters();
		getInputsStatus = getInputs();

		if ( 0 != ( stepStatus = doStep( syncTime_, lastSyncTime_ ) ) ) {
			logger( fmiError, "ERROR", "doStep failed" );
		}

		setOutputsStatus = setOutputs();
	
		lastSyncTime_ = syncTime_;	

		backend_->signalToMaster();
	}
	catch (...) { return -1; }

	if ( ( getParamsStatus != fmiOK ) || ( getInputsStatus != fmiOK ) || ( setOutputsStatus != fmiOK ) || ( stepStatus != 0 ) )
		return -1;
	
	return 0;
}


void
BackEndImplementationBase::enforceTimeStep( const fmiReal& fixedTimeStep )
{
	backend_->enforceTimeStep( fixedTimeStep );
}


void
BackEndImplementationBase::logger( fmiStatus status, const std::string& category, const std::string& msg )
{
	backend_->logger( status, category, msg );
}


fmiStatus
BackEndImplementationBase::initParameters()
{
	fmiStatus init;
	
	if ( fmiOK != ( init = backend_->initializeRealParameters( realParamNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeRealParameters failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeIntegerParameters( integerParamNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeIntegerParameters failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeBooleanParameters( booleanParamNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeBooleanParameters failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeStringParameters( stringParamNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeStringParameters failed" );
		return init;
	}
	
	return fmiOK;
}

fmiStatus
BackEndImplementationBase::getParameters()
{
	static fmiStatus status;
	
	if ( fmiOK != ( status = backend_->getRealParameters( realParams_ ) ) ) {
		logger( fmiError, "ERROR", "getRealParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getIntegerParameters( integerParams_ ) ) ) {
		logger( fmiError, "ERROR", "getIntegerParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getBooleanParameters( booleanParams_ ) ) ) {
		logger( fmiError, "ERROR", "getBooleanParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getStringParameters( stringParams_ ) ) ) {
		logger( fmiError, "ERROR", "getStringParameters failed" );
		return status;
	}

	return fmiOK;
}


fmiStatus
BackEndImplementationBase::setParameters()
{
	static fmiStatus status;
	
	if ( fmiOK != ( status = backend_->setRealParameters( realParams_ ) ) ) {
		logger( fmiError, "ERROR", "setRealParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setIntegerParameters( integerParams_ ) ) ) {
		logger( fmiError, "ERROR", "setIntegerParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setBooleanParameters( booleanParams_ ) ) ) {
		logger( fmiError, "ERROR", "setBooleanParameters failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setStringParameters( stringParams_ ) ) ) {
		logger( fmiError, "ERROR", "setStringParameters failed" );
		return status;
	}

	return fmiOK;
}


fmiStatus
BackEndImplementationBase::initInputs()
{
	fmiStatus init;

	if ( fmiOK != ( init = backend_->initializeRealInputs( realInputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeRealInputs failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeIntegerInputs( integerInputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeIntegerInputs failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeBooleanInputs( booleanInputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeBooleanInputs failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeStringInputs( stringInputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeStringInputs failed" );
		return init;
	}
	
	return fmiOK;
}


fmiStatus
BackEndImplementationBase::getInputs()
{
	static fmiStatus status;
	
	if ( fmiOK != ( status = backend_->getRealInputs( realInputs_ ) ) ) {
		logger( fmiError, "ERROR", "getRealInputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getIntegerInputs( integerInputs_ ) ) ) {
		logger( fmiError, "ERROR", "getIntegerInputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getBooleanInputs( booleanInputs_ ) ) ) {
		logger( fmiError, "ERROR", "getBooleanInputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->getStringInputs( stringInputs_ ) ) ) {
		logger( fmiError, "ERROR", "getStringInputs failed" );
		return status;
	}

	return fmiOK;
}


fmiStatus
BackEndImplementationBase::initOutputs()
{
	fmiStatus init;
	
	if ( fmiOK != ( init = backend_->initializeRealOutputs( realOutputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeRealOutputs failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeIntegerOutputs( integerOutputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeIntegerOutputs failed" );
		return init;
	}
	
	if ( fmiOK != ( init = backend_->initializeBooleanOutputs( booleanOutputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeBooleanOutputs failed" );
	}
	
	if ( fmiOK != ( init = backend_->initializeStringOutputs( stringOutputNames_ ) ) ) {
		logger( fmiError, "ERROR", "initializeStringOutputs failed" );
		return init;
	}

	return fmiOK;
}


fmiStatus
BackEndImplementationBase::setOutputs()
{
	static fmiStatus status;
	
	if ( fmiOK != ( status = backend_->setRealOutputs( realOutputs_ ) ) ) {
		logger( fmiError, "ERROR", "setRealOutputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setIntegerOutputs( integerOutputs_ ) ) ) {
		logger( fmiError, "ERROR", "setIntegerOutputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setBooleanOutputs( booleanOutputs_ ) ) ) {
		logger( fmiError, "ERROR", "setBooleanOutputs failed" );
		return status;
	}

	if ( fmiOK != ( status = backend_->setStringOutputs( stringOutputs_ ) ) ) {
		logger( fmiError, "ERROR", "setStringOutputs failed" );
		return status;
	}

	return fmiOK;
}


void
BackEndImplementationBase::writeScalarVariableNamesToFile()
{
	// Write parameter names.
	writeVectorContentToFile( realParamNames_, "real.param" );
	writeVectorContentToFile( integerParamNames_, "integer.param" );
	writeVectorContentToFile( booleanParamNames_, "boolean.param" );
	writeVectorContentToFile( stringParamNames_, "string.param" );

	// Write input names.
	writeVectorContentToFile( realInputNames_, "real.in" );
	writeVectorContentToFile( integerInputNames_, "integer.in" );
	writeVectorContentToFile( booleanInputNames_, "boolean.in" );
	writeVectorContentToFile( stringInputNames_, "string.in" );

	// Write output names.
	writeVectorContentToFile( realOutputNames_, "real.out" );
	writeVectorContentToFile( integerOutputNames_, "integer.out" );
	writeVectorContentToFile( booleanOutputNames_, "boolean.out" );
	writeVectorContentToFile( stringOutputNames_, "string.out" );
}


/// Write the contents of a vector of strings to file.
void
BackEndImplementationBase::writeVectorContentToFile( const std::vector<std::string>& vec, const std::string& filename ) const
{
	// Check if vector is empty.
	if ( true == vec.empty() ) return;
	
	std::ofstream out( filename.c_str() ); // New output file.
	
	std::vector<std::string>::const_iterator begin = vec.begin();
	std::vector<std::string>::const_iterator end = vec.end();
	std::vector<std::string>::const_iterator it;
	// Iterate through vector of strings and write each element into a separate line.
	for ( it = begin; it != end; ++it ) out << (*it) << std::endl;
}