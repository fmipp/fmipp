/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryFrontEnd.cpp

// ATTENTION: Do not change the sequence of the include statements!

#ifndef _WIN32_WINDOWS // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7 // Target Windows 7 or later.
#endif

// Standard library includes.
#include <sstream>
#include <stdexcept>

// Boost library includes.
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

// PF API for RMS simulation.
#include "api/include/PowerFactoryRMS.h"

// Includes for PF frontend.
#include "frontend/include/PowerFactoryFrontEnd.h"
#include "frontend/include/PowerFactoryRealScalar.h"
#include "frontend/include/PowerFactoryTimeAdvance.h"
#include "frontend/include/PowerFactoryExtraOutput.h"

// Dependencies on FMI++.
#include "export/include/HelperFunctions.h"
#include "import/base/include/ModelDescription.h"

// PF API.
#include "api/include/PowerFactory.h"

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1800 )
#include "windows.h"
#include <Lmcons.h>
#else
#error This project requires Visual Studio 2013.
#endif


using namespace std;
using namespace pf_api;


/// \FIXME Why is this function not a member function of class PowerFactoryFrontEnd?
bool initializeScalar( PowerFactoryRealScalar* scalar,
		       const ModelDescription::Properties& description,
		       PowerFactoryFrontEnd* frontend,
		       PowerFactory* pf );


PowerFactoryFrontEnd::PowerFactoryFrontEnd() :
	pf_( 0 ), time_( 0 ), extraOutput_( 0 ), rmsEventCount_( 0 )
{}


PowerFactoryFrontEnd::~PowerFactoryFrontEnd()
{
	if ( 0 != pf_ ) {

		// Deactivate the project.
		if ( pf_->Ok != pf_->deactivateProject() )
			logger( fmi2Warning, "WARNING", "deactivation of project failed" );

		// Delete the project.
		string executeCmd = string( "del " ) + target_ + string( "\\" ) + projectName_;
		if ( pf_->Ok != pf_->execute( executeCmd.c_str() ) )
			logger( fmi2Warning, "WARNING", "could not delete project" );

		// Empty the recycle bin (delete the project once and forever).
		// Note: For PF 15.0.3 string( "\\Recycle Bin\\*" ) was used."
		executeCmd = string( "del " ) + target_ + string( "\\RecBin\\*" );
		if ( pf_->Ok != pf_->execute( executeCmd.c_str() ) )
			logger( fmi2Warning, "WARNING", "could not empty recycle bin" );

		// Exit PowerFactory.
		if ( pf_->Ok != pf_->execute( "exit" ) )
			logger( fmi2Warning, "WARNING", "exiting failed" );

		// Delete the wrappper-internal representation of the model variables.
		BOOST_FOREACH( RealMap::value_type& v, realScalarMap_ )
			delete v.second;

		/// \FIXME deallocation of object of type PowerFactory causes the program to halt
		//delete pf_;
	}
	
	if ( 0 != time_ ) delete time_;

	if ( 0 != extraOutput_ ) delete extraOutput_;
	
	// Reset logger for PF API.
	PowerFactory::setLogger( 0 );
}


fmi2Status
PowerFactoryFrontEnd::setReal( const fmi2ValueReference& ref, const fmi2Real& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		ostringstream err;
		err << "setReal -> unknown value reference = " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	const PowerFactoryRealScalar* scalar = itFind->second;
	// Check if scalar is defined as input.
	if ( scalar->causality_ != ScalarVariableAttributes::input )
	{
		ostringstream err;
		err << "setReal -> scalar is not an input variable, value reference = " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		return fmi2Warning;
	}

	// Set value in PowerFactory.
	if ( true == setValue( scalar, val ) ) return fmi2OK;

	string err = string( "setReal -> not able to set data: class name = " ) + 
		scalar->className_ + string( ", object name = " ) + scalar->objectName_ +
		string( ", parameter name = " ) + scalar->parameterName_;

	logger( fmi2Warning, "WARNING", err );

	return fmi2Warning;
}


fmi2Status
PowerFactoryFrontEnd::setInteger( const fmi2ValueReference& ref, const fmi2Integer& val )
{
	return fmi2Fatal;
}


fmi2Status PowerFactoryFrontEnd::setBoolean( const fmi2ValueReference& ref, const fmi2Boolean& val )
{
	return fmi2Fatal;
}


fmi2Status PowerFactoryFrontEnd::setString( const fmi2ValueReference& ref, const fmi2String& val )
{
	return fmi2Fatal;
}


fmi2Status
PowerFactoryFrontEnd::getReal( const fmi2ValueReference& ref, fmi2Real& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		ostringstream err;
		err << "getReal -> unknown value reference = " << ref;
		logger( fmi2Warning, "WARNING", err.str() );
		val = 0;
		return fmi2Warning;
	}

	const PowerFactoryRealScalar* scalar = itFind->second;
	// Extract data from PowerFactory object using the parameter name.
	if (( 0 != scalar->apiDataObject_ ) &&
	    ( pf_->getAttributeDouble( scalar->apiDataObject_, scalar->parameterName_.c_str(), val ) == pf_->Ok ))
	{
		return fmi2OK;
	}

	string err = string( "getReal -> not able to read data: class name = " ) + 
		scalar->className_ + string( ", object name = " ) + scalar->objectName_ +
		string( ", parameter name = " ) + scalar->parameterName_;

	logger( fmi2Warning, "WARNING", err );
	return fmi2Warning;
}


fmi2Status
PowerFactoryFrontEnd::getInteger( const fmi2ValueReference& ref, fmi2Integer& val )
{
	return fmi2Fatal;
}


fmi2Status
PowerFactoryFrontEnd::getBoolean( const fmi2ValueReference& ref, fmi2Boolean& val )
{
	return fmi2Fatal;
}


fmi2Status
PowerFactoryFrontEnd::getString( const fmi2ValueReference& ref, fmi2String& val )
{
	return fmi2Fatal;
}


fmi2Status
PowerFactoryFrontEnd::getDirectionalDerivative( const fmi2ValueReference vUnknown_ref[],
					size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown,
					const fmi2Real dvKnown[], fmi2Real dvUnknown[] )
{
	return fmi2Fatal;
}


fmi2Status
PowerFactoryFrontEnd::getFMUState( fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::setFMUState( fmi2FMUstate fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::freeFMUState( fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::serializedFMUStateSize( fmi2FMUstate fmuState, size_t* size )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::serializeFMUState( fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::deserializeFMUState( const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
					const std::string& fmuLocation, fmi2Real timeout, fmi2Boolean visible )
{
	instanceName_ = instanceName;

	const string seperator( "/" );
	// Trim FMU location path (just to be sure).
	const string fmuLocationTrimmed = boost::trim_copy( fmuLocation );
	// Construct URI of XML model description file.
	const string modelDescriptionUrl = fmuLocationTrimmed + seperator + string( "modelDescription.xml" );

	// Get the path of the XML model description file.
	string modelDescriptionPath;
	if ( false == HelperFunctions::getPathFromUrl( modelDescriptionUrl, modelDescriptionPath ) ) {
                ostringstream err;
		err << "invalid input URL for XML model description file: " << modelDescriptionUrl;
		logger( fmi2Fatal, "URL", err.str() );
		return fmi2Fatal;
	}

	// Parse the XML model description file.
	ModelDescription modelDescription( modelDescriptionPath );

	// Check if parsing was successfull.
	if ( false == modelDescription.isValid() ) {
                ostringstream err;
		err << "unable to parse XML model description file: " << modelDescriptionPath;
		logger( fmi2Fatal, "MODEL-DESCRIPTION", err.str() );
		return fmi2Fatal;
	}

	// Check if GUID matches.
	if ( modelDescription.getGUID() != fmuGUID ) { // Check if GUID is consistent.
		string err = string( "wrong GUID: " ) + fmuGUID +
			string(" --- expected: " ) + modelDescription.getGUID();
		logger( fmi2Fatal, "GUID", err );
		return fmi2Fatal;
	}

	// For FMI CS 1.0 compatibility.
	mimeType_ = modelDescription.getMIMEType();

	// Copy additional input files (specified in XML description elements
	// of type  "Implementation.CoSimulation_Tool.Model.File").
	if ( false == copyAdditionalInputFiles( &modelDescription, fmuLocationTrimmed ) ) {
		logger( fmi2Fatal, "FILE-COPY", "not able to copy additional input files" );
		return fmi2Fatal;
	}

	// The input file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	string inputFileUrl = modelDescription.getEntryPoint();
	string inputFilePath;
	processURI( inputFileUrl, fmuLocationTrimmed );
	if ( false == HelperFunctions::getPathFromUrl( inputFileUrl, inputFilePath ) ) {
                ostringstream err;
		err << "invalid URL for input file (entry point): " << inputFileUrl;
		logger( fmi2Fatal, "URL", err.str() );
		return fmi2Fatal;
	}

	// Extract PowerFactory project name.
	projectName_ = modelDescription.getModelAttributes().get<string>( "modelName" );
	// Extract PowerFactory target.
	if ( false == parseTarget( &modelDescription ) )
	{
		logger( fmi2Fatal, "ABORT", "could not parse target" );
		return fmi2Fatal;
	}

	// Parse number of model variables from model description.
	size_t nRealScalars;
	size_t nIntegerScalars;
	size_t nBooleanScalars;
	size_t nStringScalars;
	modelDescription.getNumberOfVariables( nRealScalars, nIntegerScalars, nBooleanScalars, nStringScalars );
	if ( ( 0 != nIntegerScalars ) && ( 0 != nBooleanScalars ) && ( 0 != nStringScalars ) ) {
		logger( fmi2Fatal, "ABORT", "only variables of type 'fmi2Real' supported" );
		return fmi2Fatal;
	}

	PowerFactory::setLogger( this );

	// All preliminary checks done, create the actual wrapper now.
	try {
		pf_ = PowerFactory::create();
	} catch (...) {
		logger( fmi2Fatal, "ABORT", "Creation of PowerFactory API wrapper failed. Has PowerFactory's installation directory been added to the Windows path?" );
		return fmi2Fatal;
	}

	if ( 0 == pf_ ) {
		logger( fmi2Fatal, "ABORT", "Creation of PowerFactory API wrapper failed. Has PowerFactory's installation directory been added to the Windows path?" );
		return fmi2Fatal;
	}

	// Sanity check on user name.
	if ( target_.substr(1) != pf_->getCurrentUser() ) {
		// Check if this is only an inconsistency between upper/lower case (which makes no difference
		// when logging on to Windows, but is important for PowerFactory).
		string lowerCaseTarget = target_.substr(1);
		transform( lowerCaseTarget.begin(), lowerCaseTarget.end(), lowerCaseTarget.begin(), ::tolower );
		string lowerCaseUserName = pf_->getCurrentUser();
		transform( lowerCaseUserName.begin(), lowerCaseUserName.end(), lowerCaseUserName.begin(), ::tolower );

		if ( lowerCaseTarget == lowerCaseUserName ) {
			// Okay, nothing serious, just use the current user name as target.
			target_ = string( "\\" ) + pf_->getCurrentUser();

			ostringstream msg;
			msg << "redefine project target: " << target_.substr(1);
			logger( fmi2OK, "DEBUG", msg.str() );
		} else {
			// Ooops! This is serious. Display an error message and abort the initialization.
			ostringstream err;
			err << "inconsistent setup: current user name in PowerFactory (" << pf_->getCurrentUser()
				<< ") and specified target (" << target_.substr(1) << ") are inconsistent.\n\n"
				<< "HINT: You may specify a target via the XML model description.\n";
			logger( fmi2Fatal, "ABORT", err.str() );
			return fmi2Fatal;
		}
	}

	// Set visibility of PowerFactory GUI.
	if ( pf_->Ok != pf_->showUI( static_cast<bool>( visible ) ) ) {
		logger( fmi2Fatal, "ABORT", "could not set UI visibility" );
		return fmi2Fatal;
	}

	// In case there is already a project in PowerFactory's cash with the same name delete it.
	const string deleteCmd = string( "del " ) + target_ + string( "\\" ) + projectName_;
	pf_->execute( deleteCmd.c_str() );

	// Import project file into PowerFactory.
	const string importCmd = string( "pfdimport g_target=" ) + target_ + string( " g_file=" ) + inputFilePath;
	if ( pf_->Ok != pf_->execute( importCmd.c_str() ) )  {
		logger( fmi2Fatal, "ABORT", "could not import project" );
		return fmi2Fatal;
	}

	// Actiavte PowerFactory project.
	if ( pf_->Ok != pf_->activateProject( projectName_ ) ) {
		logger( fmi2Fatal, "ABORT", "could not activate project" );
		return fmi2Fatal;
	}

	// Instantiate the mechanism for time advance.
	if ( false == instantiateTimeAdvanceMechanism( &modelDescription ) ) {
		return fmi2Fatal;
	}

	// Initialize wrapper-internal representation of variables.
	if ( false == initializeVariables( &modelDescription ) ) {
		return fmi2Fatal;
	}

	// Initialize output of extra simulation results to file.
	extraOutput_ = new PowerFactoryExtraOutput( fmiFunctions_ );
	if ( false == extraOutput_->initializeExtraOutput( pf_ ) ) {
		return fmi2Fatal;
	}

	return fmi2OK;
}


fmi2Status
PowerFactoryFrontEnd::initializeSlave( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop )
{
	// Initialize starting time.
	fmi2Status status = time_->initialize( tStart, stopTimeDefined, tStop );
	if ( fmi2OK != status ) return status;

	// Make a power flow calculation (triggers calculation of "flexible data").
	if ( pf_->calculatePowerFlow() != pf_->Ok ) {
		logger( fmi2Fatal, "ABORT", "power flow calculation failed" );
		return fmi2Fatal;
	}

	// Check if power flow is valid.
	if ( pf_->isPowerFlowValid() != pf_->Ok ) {
		logger( fmi2Discard, "DISCARD", "power flow calculation not valid" );
		return fmi2Discard;
	}

	// Write extra simulation results.
	if ( false == extraOutput_->writeExtraOutput( tStart, pf_ ) ) {
		string err( "not able to write extra simulation results" );
		logger( fmi2Warning, "WARNING", err );
		return fmi2Warning;
	}

	return fmi2OK;
}


fmi2Status
PowerFactoryFrontEnd::resetSlave()
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::setRealInputDerivatives( const fmi2ValueReference vr[], size_t nvr,
					       const fmi2Integer order[], const fmi2Real value[])
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getRealOutputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						const fmi2Integer order[], fmi2Real value[])
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::doStep( fmi2Real comPoint, fmi2Real stepSize, fmi2Boolean newStep )
{
	// Advance time in simulation.
	fmi2Status status = time_->advanceTime( comPoint, stepSize );
	if ( fmi2OK != status ) return status;

	if ( fmi2True == time_->calculatePowerFlow() ) {
		// Make a power flow calculation (triggers calculation of "flexible data").
		if ( pf_->calculatePowerFlow() != pf_->Ok ) {
			logger( fmi2Fatal, "ABORT", "power flow calculation failed" );
			return fmi2Fatal;
		}

		// Check if power flow is valid.
		if ( pf_->isPowerFlowValid() != pf_->Ok ) {
			logger( fmi2Discard, "DISCARD", "power flow calculation not valid" );
			return fmi2Discard;
		}
	}
		
	// Write extra simulation results.
	if ( false == extraOutput_->writeExtraOutput( comPoint + stepSize, pf_ ) ) {
		string err( "not able to write extra simulation results" );
		logger( fmi2Warning, "WARNING", err );
		return fmi2Warning;
	}

	return fmi2OK;
}


fmi2Status
PowerFactoryFrontEnd::cancelStep()
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getStatus( const fmi2StatusKind s, fmi2Status* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getRealStatus( const fmi2StatusKind s, fmi2Real* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getIntegerStatus( const fmi2StatusKind s, fmi2Integer* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getBooleanStatus( const fmi2StatusKind s, fmi2Boolean* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


fmi2Status
PowerFactoryFrontEnd::getStringStatus( const fmi2StatusKind s, fmi2String* value )
{
	return fmi2Fatal; /// \FIXME Replace dummy implementation.
}


bool
PowerFactoryFrontEnd::instantiateTimeAdvanceMechanism( const ModelDescription* modelDescription )
{
	// Check if vendor annotations are available.
	using namespace ModelDescriptionUtilities;
	if ( modelDescription->hasVendorAnnotations() )
	{
		// Extract current application name from MIME type.
		string applicationName = modelDescription->getMIMEType().substr( 14 );
		// Extract vendor annotations.
		const Properties& vendorAnnotations = modelDescription->getVendorAnnotations();

		// Check if vendor annotations according to current application are available.
		if ( hasChild( vendorAnnotations, applicationName ) )
		{
			const Properties& annotations = vendorAnnotations.get_child( applicationName );

			// Count numbers of Trigger and DPLScript nodes.
			unsigned int numTriggerNodes = annotations.count( "Trigger" );
			unsigned int numDPLScriptNodes = annotations.count( "DPLScript" );
			unsigned int numRmsSimNodes = annotations.count( "RMSSimulation" );
			
			if ( numRmsSimNodes > 1 ) {
				// There must not be more than one RMS simulation setup.
				logger( fmi2Fatal, "TIME-ADVANCE", "more than one RMS simulation setup defined" );
				return false;
			}

			// Choose time advance mechanism.
			if ( ( numTriggerNodes > 0 ) && ( numDPLScriptNodes == 0 ) && ( numRmsSimNodes == 0 ) ) {
				// Initialize trigger mechanism.
				time_ = new TriggerTimeAdvance( this, pf_ );
				logger( fmi2OK, "TIME-ADVANCE", "use triggers" );
			} else if ( ( numTriggerNodes == 0 ) && ( numDPLScriptNodes > 0 ) && ( numRmsSimNodes == 0 ) ) {
				// Initialize DPL script mechanism.
				time_ = new DPLScriptTimeAdvance( this, pf_ );
				logger( fmi2OK, "TIME-ADVANCE", "use DPL script" );
			} else if ( ( numTriggerNodes == 0 ) && ( numDPLScriptNodes == 0 ) && ( numRmsSimNodes == 1 ) ) {
				// Initialize RMS simulation.
				time_ = new RMSTimeAdvance( this, pf_ );
				logger( fmi2OK, "TIME-ADVANCE", "use RMS simulation" );
			} else if ( ( numTriggerNodes == 0 ) && ( numDPLScriptNodes == 0 ) && ( numRmsSimNodes == 0 ) ) {
				// Neither triggers nor DPL scripts nor RMS simulation defined, issue message and abort.
				logger( fmi2Fatal, "TIME-ADVANCE", "no time advance mechanism defined (triggers, DPL scipts, RMS simulation)" );
				return false;
			} else {
				// Inconsistent setup.
				ostringstream err;
				err << "inconsistent setup: " << numTriggerNodes << " triggers, " 
				    << numDPLScriptNodes << " DPL scripts, "
					<< numRmsSimNodes << "RMS simulation setups";
				logger( fmi2Fatal, "TIME-ADVANCE", err.str() );
				return false;
			}

			// Instantiate time advance mechanism.
			if ( fmi2OK != time_->instantiate( annotations ) ) return false;

		} else {
			string err( "vendor annotations contain no node called '" );
			err += applicationName + string( "'");
			logger( fmi2Fatal, "ABORT", err );
			return false;
		}

	} else {
		logger( fmi2Fatal, "ABORT", "no vendor annotations found in model description" );
		return false;
	}

	return true;
}


bool
PowerFactoryFrontEnd::initializeVariables( const ModelDescription* modelDescription )
{
	// Check if model description is available.
	if ( false == modelDescription->hasModelVariables() ) {
		logger( fmi2Warning, "WARNING", "model variable description missing" );
		return false;
	}

	// Get variable description.
	const ModelDescription::Properties& modelVariables = modelDescription->getModelVariables();

	PowerFactoryRealScalar* scalar;

	// Iterate through variable decriptions.
	BOOST_FOREACH( const ModelDescription::Properties::value_type &v, modelVariables )
	{
		// Create new scalar for internal representation of variables.
		scalar = new PowerFactoryRealScalar;
		// Initialize scalar according to variable description.
		if ( false == initializeScalar( scalar, v.second, this, pf_ ) ) {
			delete scalar;
			return false;
		}

		// Add scalar to internal map.
		realScalarMap_[scalar->valueReference_] = scalar;
	}

	return true;
}


bool
PowerFactoryFrontEnd::parseTarget( const ModelDescription* modelDescription )
{
	using namespace ModelDescriptionUtilities;

	// Check if vendor annotations are available.
	if ( modelDescription->hasVendorAnnotations() )
	{
		// Extract current application name from MIME type.
		string applicationName = modelDescription->getMIMEType().substr( 14 );
		// Extract vendor annotations.
		const Properties& vendorAnnotations = modelDescription->getVendorAnnotations();

		// Check if vendor annotations according to current application are available.
		if ( hasChild( vendorAnnotations, applicationName ) )
		{
			if ( hasChildAttributes( vendorAnnotations, applicationName ) )
			{
				// Extract target from XML description.
				const Properties& attributes =
					getChildAttributes( vendorAnnotations, applicationName );

				if ( hasChild( attributes, "target" ) ) {
					target_ = attributes.get<string>( "target" );
					return true;
				}
			}

			// Alternatively, get current user name via WIN32 API and use it as target.
			char username[UNLEN+1];
			DWORD username_len = UNLEN+1;
			GetUserName( username, &username_len );
			target_ = string( "\\" ) + username;

			ostringstream log;
			log << "no project target defined in vendor annotations, "
			    << "will use login name: " << username;
			logger( fmi2OK, "TARGET", log.str() );

			return true;
		} else {
			ostringstream err;
			err << "vendor annotations do not contain information specific to PowerFactory "
			    << "(XML node '" << applicationName << "' is missing)";
			logger( fmi2Fatal, "XML", err.str() );
		}
	} else {
		string err( "no vendor annotations found in model description" );
		logger( fmi2Fatal, "XML", err );
	}

	return false;
}


bool
PowerFactoryFrontEnd::setValue( const PowerFactoryRealScalar* scalar, const double& value )
{
	if ( false == scalar->isRMSEvent_ )
	{
		// Set value of parameter of PowerFactory object using the parameter name.
		if ( ( 0 != scalar->apiDataObject_ ) &&
		     ( pf_->setAttributeDouble( scalar->apiDataObject_, scalar->parameterName_.c_str(), value ) == pf_->Ok ) ) 
		{
			return true;
		}
	}
	else
	{
		// Since this wrapper uses PF for off-line co-simulation (no real-time simulation), the function 
		// call to advance time in the RMS simulation has to be non-blocking (the queue will not be resolved
		// before the next call to function doStep() is issued).
		const bool blocking = false;
		
		// Construct event string.
		stringstream eventName;
		eventName << "FMIEvent" << ( rmsEventCount_ + 1 );

		stringstream event;
		event << "create=" << scalar->className_
		      << " name=" << eventName.str()
		      << " target=" << scalar->objectName_
		      << " dtime=0.0 variable=" << scalar->parameterName_
		      << " value=" << value;

		// Send the event to PF.
		bool isDuplicate = false;
		if ( pf_->Ok == pf_->rms()->rmsSendEvent( eventName.str(), scalar->className_,
		     scalar->objectName_, event.str(), blocking, isDuplicate ) )
		{
			if ( false == isDuplicate ) ++rmsEventCount_; // Increment event counter.
			return true;
		}
	}
	
	return false;
}


void
PowerFactoryFrontEnd::logger( fmi2Status status, const string& category, const string& msg )
{
	if ( ( status == fmi2OK ) && ( fmi2False == loggingOn_ ) ) return;

	fmiFunctions_->logger( static_cast<fmi2Component>( this ),
			    instanceName_.c_str(), static_cast<fmiStatus>( status ),
			    category.c_str(), msg.c_str() );
}


const std::string
PowerFactoryFrontEnd::getMIMEType() const
{
	return mimeType_;
}


void 
PowerFactoryFrontEnd::logger( const PowerFactoryLoggerBase::LogLevel& l, const string& category, const string& msg )
{
	switch ( l ) {
		case PowerFactoryLoggerBase::OK:
			logger( fmi2OK, category, msg );
			break;
		case PowerFactoryLoggerBase::Warning:
			logger( fmi2Warning, category, msg );
			break;
		case PowerFactoryLoggerBase::Error:
			logger( fmi2Error, category, msg );
			break;
	}
}

bool
initializeScalar( PowerFactoryRealScalar* scalar,
		  const ModelDescription::Properties& description,
		  PowerFactoryFrontEnd* frontend,
		  PowerFactory* pf )
{
	using namespace ScalarVariableAttributes;
	using namespace ModelDescriptionUtilities;

	// Get XML attributes from scalar description.
	const Properties& attributes = getAttributes( description );

	// Parse class name, object name and parameter name from description.
	bool parseStatus =
		PowerFactoryRealScalar::parseFMIVariableName( attributes.get<string>( "name" ),
							      scalar->className_,
							      scalar->objectName_,
							      scalar->parameterName_,
							      scalar->isRMSEvent_ );

	if ( false == parseStatus ) {
		ostringstream err;
		err << "bad variable name: " << attributes.get<string>( "name" );
		frontend->logger( fmi2Warning, "WARNING", err.str() );
		return false;
	}

	// Extract information regarding value reference, causality and variability.
	scalar->valueReference_ = attributes.get<int>( "valueReference" );
	scalar->causality_ = getCausality( attributes.get<string>( "causality" ) );
	scalar->variability_ = getVariability( attributes.get<string>( "variability" ) );

	if ( false == scalar->isRMSEvent_ ) { // Model variable corresponds to standard PF object.
		api::v1::DataObject* dataObj = 0;
		int check = -1;
		// Search for PowerFactory object by class name and object name.
		check = pf->getCalcRelevantObject( scalar->className_, scalar->objectName_, dataObj );
		if ( check != pf->Ok )
		{
			ostringstream err;
			err << "unable to get object: " << scalar->objectName_
				<< " (type " << scalar->className_ << ")";
			frontend->logger( fmi2Warning, "WARNING", err.str() );
			return false;
		} else if ( 0 != dataObj ) {
			scalar->apiDataObject_ = dataObj;
		}
	} else { // Model variable corresponds to RMS simulation input event.
		scalar->apiDataObject_ = 0;
		
		if ( ScalarVariableAttributes::input != scalar->causality_ ) {
			ostringstream err;
			err << "RMS simulation events have to be declared as input variables: FMIEvent." << scalar->parameterName_;
			frontend->logger( fmi2Warning, "WARNING", err.str() );
			return false;
		}
	}
		
	if ( hasChildAttributes( description, "Real" ) )
	{
		// This wrapper handles only variables of type 'fmi2Real'!
		const Properties& properties = getChildAttributes( description, "Real" );

		// Check if a start value has been defined.
		if ( properties.find( "start" ) != properties.not_found() ) {

			// // Check if scalar is defined as input.
			// if ( scalar->causality_ != ScalarVariableAttributes::input ) {
			// 	ostringstream err;
			// 	err << "not an input: " << attributes.get<string>( "name" );
			// 	frontend->logger( fmi2Warning, "WARNING", err.str() );
			// 	return false;
			// }
 
			fmi2Real startValue = properties.get<fmi2Real>( "start" );

			// Set value of parameter of PowerFactory object using the parameter name.
			if ( false == frontend->setValue( scalar, startValue ) )
			{
				ostringstream err;
				err << "unable to set attribute: " << attributes.get<string>( "name" );
				frontend->logger( fmi2Warning, "WARNING", err.str() );
				return false;
			}
		}
	}

	/// \FIXME What about the remaining properties?

	return true;
}
