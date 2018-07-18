// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include "export/include/BackEndApplicationBase.h"
#include "export/include/HelperFunctions.h"

// Boost includes.
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

// Standard includes.
#include <fstream>
#include <iostream>


BackEndApplicationBase::BackEndApplicationBase() :
	readyToLoop_( false ), backend_( 0 )
{}


BackEndApplicationBase::~BackEndApplicationBase()
{
	if ( 0 != backend_ ) delete backend_;
}


int
BackEndApplicationBase::initializeBase( int argc, const char* argv[] )
{
	// Special usage of the backend: Only write the names of the scalar
	// variables (inputs, outputs, parameters) to separate files, then exit.
	if ( ( 2 == argc ) && ( 0 == strcmp( argv[1], "--only-write-variable-names" ) ) )
	{
		initializeScalarVariables();
		writeScalarVariableNamesToFiles();
	}
	// Special usage of the backend: Only write the names of the scalar
	// variables (inputs, outputs, parameters) to a single JSON file, then exit.
	else if ( ( 2 == argc ) && ( 0 == strcmp( argv[1], "--only-write-variable-names-json" ) ) )
	{
		initializeScalarVariables();
		std::string filename = std::string( argv[0] ) + std::string( ".json" );
		writeScalarVariableNamesToJSONFile( filename );
	}
	// No special usage, just start the standard initialization process.
	else
	{
		fmi2Status initParamsStatus = fmi2OK;
		fmi2Status initInputsStatus = fmi2OK;
		fmi2Status initOutputsStatus = fmi2OK;
		//fmi2Status getParamsStatus = fmi2OK;
		fmi2Status setParamsStatus = fmi2OK;
		fmi2Status setOutputsStatus = fmi2OK;

		try
		{
			// Instantiate new backend.
			backend_ = new FMIComponentBackEnd;

			// Init backend.
			backend_->startInitialization();

			// User defined initialization of scalar variables.
			initializeScalarVariables();

			initParamsStatus = initParameters();
			initInputsStatus = initInputs();
			initOutputsStatus = initOutputs();

			// User defined initialization of parameter values.
			initializeParameterValues();

			// User defined initialization of other stuff.
			initializeBackEnd( argc, argv );

			// Update the front-emd in case the initialization changed the value of parameters or outputs.
			setParamsStatus = setParameters();
			setOutputsStatus = setOutputs();

			// Initialize internal time representation.
			syncTime_ = backend_->getCurrentCommunicationPoint();
			lastSyncTime_ = syncTime_;

			backend_->endInitialization();
		}
		catch (...) { return -1; }

		if ( ( initParamsStatus != fmi2OK ) || ( initInputsStatus != fmi2OK ) ||
		     ( initOutputsStatus != fmi2OK ) || ( setParamsStatus != fmi2OK ) ||
		     ( setOutputsStatus != fmi2OK ) ) return -1;

		// The initialization has been carried out successfully, the backend
		// is ready to enter the simulation loop -> set flag accordingly.
		readyToLoop_ = true;
	}

	return 0;
}


bool
BackEndApplicationBase::readyToLoop()
{
	return readyToLoop_;
}


int
BackEndApplicationBase::doStepBase()
{
	static fmi2Status getParamsStatus = fmi2OK;
	static fmi2Status getInputsStatus = fmi2OK;
	static fmi2Status setOutputsStatus = fmi2OK;
	static fmi2Status resetInputsStatus = fmi2OK;
	static int stepStatus = 0;

	try
	{
		backend_->waitForMaster();

		syncTime_ = getCurrentCommunicationPoint() + getCommunicationStepSize();

		getParamsStatus = getParameters();
		getInputsStatus = getInputs();

		if ( 0 != ( stepStatus = doStep( syncTime_, lastSyncTime_ ) ) ) {
			logger( fmi2Error, "ERROR", "doStep failed" );
		}

		setOutputsStatus = setOutputs();
		resetInputsStatus = resetInputs();

		lastSyncTime_ = syncTime_;

		backend_->signalToMaster();
	}
	catch (...) { return -1; }

	if ( ( getParamsStatus != fmi2OK ) || ( getInputsStatus != fmi2OK ) || ( setOutputsStatus != fmi2OK ) ||
	     ( resetInputsStatus != fmi2OK ) || ( stepStatus != 0 ) )
		return -1;

	return 0;
}


const fmi2Real&
BackEndApplicationBase::getCurrentCommunicationPoint() const
{
	return backend_->getCurrentCommunicationPoint();
}


const fmi2Real&
BackEndApplicationBase::getCommunicationStepSize() const
{
	return backend_->getCommunicationStepSize();
}


const fmi2Real&
BackEndApplicationBase::getStopTime() const
{
	return backend_->getStopTime();
}


const bool&
BackEndApplicationBase::getStopTimeDefined() const
{
	return backend_->getStopTimeDefined();
}


void
BackEndApplicationBase::enforceTimeStep( const fmi2Real& fixedTimeStep )
{
	backend_->enforceTimeStep( fixedTimeStep );
}


bool
BackEndApplicationBase::loggingOn() const
{
	return backend_->loggingOn();
}


void
BackEndApplicationBase::logger( fmi2Status status, const std::string& category, const std::string& msg )
{
	backend_->logger( status, category, msg );
}


fmi2Status
BackEndApplicationBase::initParameters()
{
	fmi2Status init;

	if ( fmi2OK != ( init = backend_->initializeRealParameters( realParamNames_, realParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeRealParameters failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeIntegerParameters( integerParamNames_, integerParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeIntegerParameters failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeBooleanParameters( booleanParamNames_, booleanParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeBooleanParameters failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeStringParameters( stringParamNames_, stringParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeStringParameters failed" );
		return init;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::getParameters()
{
	static fmi2Status status;

	if ( fmi2OK != ( status = backend_->getRealParameters( realParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "getRealParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getIntegerParameters( integerParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "getIntegerParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getBooleanParameters( booleanParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "getBooleanParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getStringParameters( stringParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "getStringParameters failed" );
		return status;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::setParameters()
{
	static fmi2Status status;

	if ( fmi2OK != ( status = backend_->setRealParameters( realParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "setRealParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setIntegerParameters( integerParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "setIntegerParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setBooleanParameters( booleanParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "setBooleanParameters failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setStringParameters( stringParams_ ) ) ) {
		logger( fmi2Error, "ERROR", "setStringParameters failed" );
		return status;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::initInputs()
{
	fmi2Status init;

	if ( fmi2OK != ( init = backend_->initializeRealInputs( realInputNames_, realInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeRealInputs failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeIntegerInputs( integerInputNames_, integerInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeIntegerInputs failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeBooleanInputs( booleanInputNames_, booleanInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeBooleanInputs failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeStringInputs( stringInputNames_, stringInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeStringInputs failed" );
		return init;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::getInputs()
{
	static fmi2Status status;

	if ( fmi2OK != ( status = backend_->getRealInputs( realInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "getRealInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getIntegerInputs( integerInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "getIntegerInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getBooleanInputs( booleanInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "getBooleanInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->getStringInputs( stringInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "getStringInputs failed" );
		return status;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::resetInputs()
{
	static fmi2Status status;

	if ( fmi2OK != ( status = backend_->resetRealInputs( realInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "resetRealInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->resetIntegerInputs( integerInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "resetIntegerInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->resetBooleanInputs( booleanInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "resetBooleanInputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->resetStringInputs( stringInputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "resetStringInputs failed" );
		return status;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::initOutputs()
{
	fmi2Status init;

	if ( fmi2OK != ( init = backend_->initializeRealOutputs( realOutputNames_, realOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeRealOutputs failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeIntegerOutputs( integerOutputNames_, integerOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeIntegerOutputs failed" );
		return init;
	}

	if ( fmi2OK != ( init = backend_->initializeBooleanOutputs( booleanOutputNames_, booleanOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeBooleanOutputs failed" );
	}

	if ( fmi2OK != ( init = backend_->initializeStringOutputs( stringOutputNames_, stringOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "initializeStringOutputs failed" );
		return init;
	}

	return fmi2OK;
}


fmi2Status
BackEndApplicationBase::setOutputs()
{
	static fmi2Status status;

	if ( fmi2OK != ( status = backend_->setRealOutputs( realOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "setRealOutputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setIntegerOutputs( integerOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "setIntegerOutputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setBooleanOutputs( booleanOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "setBooleanOutputs failed" );
		return status;
	}

	if ( fmi2OK != ( status = backend_->setStringOutputs( stringOutputs_ ) ) ) {
		logger( fmi2Error, "ERROR", "setStringOutputs failed" );
		return status;
	}

	return fmi2OK;
}


void
BackEndApplicationBase::writeScalarVariableNamesToJSONFile( const std::string& filename )
{
	using namespace HelperFunctions;

	boost::property_tree::ptree tree;

	// Write parameter names.
	if ( 0 != realParamNames_.size() ) addVectorToTree( tree, realParamNames_, "RealParameters" );
	if ( 0 != integerParamNames_.size() ) addVectorToTree( tree, integerParamNames_, "IntegerParameters" );
	if ( 0 != booleanParamNames_.size() ) addVectorToTree( tree, booleanParamNames_, "BooleanParameters" );
	if ( 0 != stringParamNames_.size() ) addVectorToTree( tree, stringParamNames_, "StringParameters" );

	// Write input names.
	if ( 0 != realInputNames_.size() ) addVectorToTree( tree, realInputNames_, "RealInputs" );
	if ( 0 != integerInputNames_.size() ) addVectorToTree( tree, integerInputNames_, "IntegerInputs" );
	if ( 0 != booleanInputNames_.size() ) addVectorToTree( tree, booleanInputNames_, "BooleanInputs" );
	if ( 0 != stringInputNames_.size() ) addVectorToTree( tree, stringInputNames_, "StringInputs" );

	// Write output names.
	if ( 0 != realOutputNames_.size() ) addVectorToTree( tree, realOutputNames_, "RealOutputs" );
	if ( 0 != integerOutputNames_.size() ) addVectorToTree( tree, integerOutputNames_, "IntegerOutputs" );
	if ( 0 != booleanOutputNames_.size() ) addVectorToTree( tree, booleanOutputNames_, "BooleanOutputs" );
	if ( 0 != stringOutputNames_.size() ) addVectorToTree( tree, stringOutputNames_, "StringOutputs" );

	write_json( filename, tree );
}


void
BackEndApplicationBase::writeScalarVariableNamesToFiles()
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
BackEndApplicationBase::writeVectorContentToFile( const std::vector<std::string>& vec, const std::string& filename ) const
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
