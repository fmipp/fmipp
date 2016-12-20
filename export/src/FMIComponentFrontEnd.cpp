/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentFrontEnd.cpp

// Platform-specific headers.
#ifdef WIN32 // Visual Studio C++ & MinGW GCC use both the same Windows APIs.
#include "Windows.h"
#include "TCHAR.h"
#else // Use POSIX functionalities for Linux.
#include <signal.h>
#include <csignal>
#include <cerrno>
#endif

// Standard includes.
#include <sstream>
#include <stdexcept>

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
	currentCommunicationPoint_( 0 ), communicationStepSize_( 0 ),
	enforceTimeStep_( 0 ), rejectStep_( 0 ),
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


fmiStatus
FMIComponentFrontEnd::setReal( const fmiValueReference& ref, const fmiReal& val )
{
	// Search for value reference.
	RealMap::iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		stringstream err;
		err << "setReal - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Check if scalar is defined as input or parameter.
	ScalarVariableAttributes::Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::input ) && 
		( causality != ScalarVariableAttributes::internal ) )
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::setInteger( const fmiValueReference& ref, const fmiInteger& val )
{
	// Search for value reference.
	IntegerMap::iterator itFind = integerScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == integerScalarMap_.end() )
	{
		stringstream err;
		err << "setInteger - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Check if scalar is defined as input or parameter.
	ScalarVariableAttributes::Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::input ) && 
		( causality != ScalarVariableAttributes::internal ) )
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::setBoolean( const fmiValueReference& ref, const fmiBoolean& val )
{
	// Search foreach value reference.
	BooleanMap::iterator itFind = booleanScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == booleanScalarMap_.end() )
	{
		stringstream err;
		err << "setBoolean - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Check if scalar is defined as input or parameter.
	ScalarVariableAttributes::Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::input ) && 
		( causality != ScalarVariableAttributes::internal ) )
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Set value.
	itFind->second->value_ = val;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::setString( const fmiValueReference& ref, const fmiString& val )
{
	// Search for value reference.
	StringMap::iterator itFind = stringScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == stringScalarMap_.end() )
	{
		stringstream err;
		err << "setString - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Check if scalar is defined as input or parameter.
	ScalarVariableAttributes::Causality causality = itFind->second->causality_;
	if ( ( causality != ScalarVariableAttributes::input ) && 
		( causality != ScalarVariableAttributes::internal ) )
	{
		stringstream err;
		err << "variable is not an input variable or internal parameter: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		return fmiWarning;
	}

	// Set value.
	itFind->second->value_ = val; // Attention: fmiString <-> std::string!!!

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::getReal( const fmiValueReference& ref, fmiReal& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		stringstream err;
		err << "getReal - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::getInteger( const fmiValueReference& ref, fmiInteger& val )
{
	// Search for value reference.
	IntegerMap::const_iterator itFind = integerScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == integerScalarMap_.end() )
	{
		stringstream err;
		err << "getInteger - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::getBoolean( const fmiValueReference& ref, fmiBoolean& val )
{
	// Search for value reference.
	BooleanMap::const_iterator itFind = booleanScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == booleanScalarMap_.end() )
	{
		stringstream err;
		err << "getBoolean - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_;

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::getString( const fmiValueReference& ref, fmiString& val )
{
	// Search for value reference.
	StringMap::const_iterator itFind = stringScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == stringScalarMap_.end() )
	{
		stringstream err;
		err << "getString - unknown value reference: " << ref;
		logger( fmiWarning, "WARNING", err.str() );
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_.c_str(); // Attention: fmiString <-> std::string!!!

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::instantiateSlave( const string& instanceName, const string& fmuGUID,
					const string& fmuLocation, const string& mimeType,
					fmiReal timeout, fmiBoolean visible )
{
	instanceName_ = instanceName;

	logger( fmiOK, "DEBUG", string( "build type = " ) + _FMIPP_BUILD_TYPE );

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
		logger( fmiFatal, "ABORT", err.str() );
		return fmiFatal;
	}

	logger( fmiOK, "DEBUG", string( "XML model description file path = " ) + filePath );

	// Parse the XML model description file.
	ModelDescription modelDescription( filePath );

	// Check if parsing was successfull.
	if ( false == modelDescription.isValid() ) {
                stringstream err;
		err << "unable to parse XML model description file: " << filePath;
		logger( fmiFatal, "ABORT", err.str() );
		return fmiFatal;
	}

	// Check if GUID is consistent.
	if ( modelDescription.getGUID() != fmuGUID ) {
		logger( fmiFatal, "ABORT", "wrong GUID" );
		return fmiFatal;
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
	if ( false == startApplication( &modelDescription, mimeType, fmuLocationTrimmed ) ) {
		logger( fmiFatal, "ABORT", "unable to start external simulator application" );
		return fmiFatal;
	}

	// Create shared memory segment.
	/// \FIXME Allow other types of inter process communication.
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( pid_ );

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
		logger( fmiFatal, "ABORT", "unable to create internal variable 'master_time'" );
		return fmiFatal;
	}

	if ( false == ipcMaster_->createVariable( "comm_step_size", communicationStepSize_, 0. ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal variable 'next_step_size'" );
		return fmiFatal;
	}

	if ( false == ipcMaster_->createVariable( "enforce_step", enforceTimeStep_, false ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal variable 'enforce_step'" );
		return fmiFatal;
	}

	if ( false == ipcMaster_->createVariable( "reject_step", rejectStep_, false ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal variable 'reject_step'" );
		return fmiFatal;
	}

	if ( false == ipcMaster_->createVariable( "slave_has_terminated", slaveHasTerminated_, false ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal variable 'slave_has_terminated'" );
		return fmiFatal;
	}

	// Create boolean variable that tells the backend if logging is on/off.
	bool* tmpLoggingOn = 0;
	if ( false == ipcMaster_->createVariable( "logging_on", tmpLoggingOn, loggingOn_ ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal variable 'logging_on'" );
		return fmiFatal;
	}

	// Create vector of real scalar variables.
	if ( false == ipcMaster_->createScalars( "real_scalars", nRealScalars, realScalars ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal vector 'real_scalars'" );
		return fmiFatal;
	}

	// Create vector of integer scalar variables.
	if ( false == ipcMaster_->createScalars( "integer_scalars", nIntegerScalars, integerScalars ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal vector 'integer_scalars'" );
		return fmiFatal;
	}

	// Create vector of boolean scalar variables.
	if ( false == ipcMaster_->createScalars( "boolean_scalars", nBooleanScalars, booleanScalars ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal vector 'boolean_scalars'" );
		return fmiFatal;
	}

	// Create vector of string scalar variables.
	if ( false == ipcMaster_->createScalars( "string_scalars", nStringScalars, stringScalars ) ) {
		logger( fmiFatal, "ABORT", "unable to create internal vector 'string_scalars'" );
		return fmiFatal;
	}

	initializeVariables( &modelDescription, realScalars, integerScalars, booleanScalars, stringScalars );

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	stringstream debugInfo;
	debugInfo << "initialize slave at time t = " << tStart;
	logger( fmiOK, "DEBUG", debugInfo.str().c_str() );

	*currentCommunicationPoint_ = tStart;

	// Synchronization point - give control to the slave.
	ipcMaster_->signalToSlave();

	// Let the slave initialize.

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	logger( fmiOK, "DEBUG", "initialization done" );
	
	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::resetSlave()
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::setRealInputDerivatives( const fmiValueReference vr[], size_t nvr,
					       const fmiInteger order[], const fmiReal value[])
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getRealOutputDerivatives( const fmiValueReference vr[], size_t nvr,
						const fmiInteger order[], fmiReal value[])
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep )
{
	stringstream debugInfo;
	debugInfo << "doStep" << " - communication point = " << comPoint << " - step size = " << stepSize;
	logger( fmiOK, "DEBUG", debugInfo.str().c_str() );

	if ( true == *slaveHasTerminated_ ) {
		logger( fmiFatal, "DEBUG", "slave has terminated" );
		callStepFinished( fmiFatal );
		return fmiFatal;
	}

	// if ( 0. == stepSize ) { // This is an event.
	// 	/// \FIXME Nothing else to be done here?
	// 	callStepFinished( fmiOK );
	// 	return fmiOK;
	// }

	//cout << "\tcomPoint = " << comPoint << " - currentCommunicationPoint_ = " << *currentCommunicationPoint_ << endl; fflush(stdout);

	if ( fabs( *currentCommunicationPoint_ - comPoint ) > comPointPrecision_ ) {
		debugInfo.str( string() );
		debugInfo << "internal time (" << *currentCommunicationPoint_ << ") "
			  << "does not match communication point (" << comPoint << ")";
		logger( fmiDiscard, "DISCARD STEP", debugInfo.str().c_str() );
		callStepFinished( fmiDiscard );
		return fmiDiscard;
	}

	//cout << "\tstepSize = " << stepSize << " - communicationStepSize_ = " << *communicationStepSize_ << endl; fflush(stdout);

	if ( true == *enforceTimeStep_ )
	{
		if ( stepSize != *communicationStepSize_ ) {
			logger( fmiDiscard, "DISCARD STEP", "enforce time step: wrong step size" );
			callStepFinished( fmiDiscard );
			return fmiDiscard;
		} else {
			logger( fmiOK, "DEBUG", "enforce time step: correct step size" );
		}
		*enforceTimeStep_ = false; // Reset flag.
	} else {
		*communicationStepSize_ = stepSize;
	}

	logger( fmiOK, "DEBUG", "start synchronization with slave ..." );

	// Synchronization point - give control to slave and let it do its work ...
	ipcMaster_->signalToSlave();

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	logger( fmiOK, "DEBUG", "... DONE" );

	if ( true == *rejectStep_ ) {
		*rejectStep_ = false; // Reset flag.
		logger( fmiDiscard, "DISCARD STEP", "step rejected by slave" );
		callStepFinished( fmiDiscard );
		return fmiDiscard;
	}

	// Advance time.
	*currentCommunicationPoint_ += stepSize;

	callStepFinished( fmiOK );

	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::cancelStep()
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getStatus( const fmiStatusKind s, fmiStatus* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getRealStatus( const fmiStatusKind s, fmiReal* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getIntegerStatus( const fmiStatusKind s, fmiInteger* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getBooleanStatus( const fmiStatusKind s, fmiBoolean* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
FMIComponentFrontEnd::getStringStatus( const fmiStatusKind s, fmiString* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


void
FMIComponentFrontEnd::logger( fmiStatus status, const string& category, const string& msg )
{
	if ( ( status == fmiOK ) && ( fmiFalse == loggingOn_ ) ) return;

	functions_->logger( static_cast<fmiComponent>( this ),
			    instanceName_.c_str(), status,
			    category.c_str(), msg.c_str() );
}


bool
FMIComponentFrontEnd::startApplication( const ModelDescription* modelDescription,
					const string& mimeType,
					const string& fmuLocation )
{
	using namespace ModelDescriptionUtilities;
	using namespace boost::filesystem;

	// Check if MIME type is consistent.
	if ( modelDescription->getMIMEType() != mimeType ) {
		string warning = string( "Wrong MIME type: " ) + mimeType +
			string( " --- expected: " ) + modelDescription->getMIMEType();
		logger( fmiWarning, "WARNING", warning );
	}

	if ( mimeType.substr( 0, 14 ) != string( "application/x-" ) ) {
		string err = string( "Incompatible MIME type: " ) + mimeType;
		logger( fmiFatal, "ABORT", err );
		return false;
	}

	// The input file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	string inputFileUrl = modelDescription->getEntryPoint();
	processURI( inputFileUrl, fmuLocation );

	// Copy additional input files (specified in XML description elements
	// of type  "Implementation.CoSimulation_Tool.Model.File").
	if ( false == copyAdditionalInputFiles( modelDescription, fmuLocation ) ) return false;

	// Check for additional command line arguments (as part of optional vendor annotations).
	string preArguments;
	string mainArguments;
	string postArguments;
	string executableUrl;
	parseAdditionalArguments( modelDescription, preArguments, mainArguments, postArguments, executableUrl );

	// The input file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	processURI( executableUrl, fmuLocation );

	// Extract application name from MIME type or special vendor annotation.
	string applicationName;
	if ( true == executableUrl.empty() ) {
		applicationName = mimeType.substr( 14 );
	} else {
		if ( false == HelperFunctions::getPathFromUrl( executableUrl, applicationName ) ) {
			logger( fmiFatal, "ABORT", "invalid input URI for executable" );
			return false;
		}
	}

	// Retrieve path to entry point.
	string strFilePath;
	if ( false == HelperFunctions::getPathFromUrl( inputFileUrl, strFilePath ) ) {
		logger( fmiFatal, "ABORT", "invalid input URL for input file (entry point)" );
		return false;
	}

	// Get path to entry point without file name (working directory for the 
	// external application).
	path entryPointPath( strFilePath );
	entryPointPath.remove_filename();
	
	// Check if entry point path exists. If not, issue a warning and use the
	// current directory.
	if ( false == exists( entryPointPath ) ) {
		string warning =
			"The path specified for the FMU's entry point does not exist.";
		warning += " Use current directory as working directory instead";
		logger( fmiWarning, "WARNING", warning );
		entryPointPath = current_path();
	}
	
#ifdef WIN32
	
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
	LPTSTR currDir = HelperFunctions::copyStringToTCHAR( entryPointPath.string() );

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
		// The process could not be started ...
		stringstream err;
		err << "CreateProcess() failed to start process"
		    << " - ERROR: " << GetLastError()
		    << " - cmdLine: >>>" << cmdLine << "<<<"
		    << " - applicationName: >>>" << applicationName << "<<<";
		logger( fmiFatal, "ABORT", err.str() );
		return false;
	}

	delete cmdLine;
	delete currDir;

	CloseHandle( processInfo.hProcess ); // This does not kill the process!
	CloseHandle( processInfo.hThread ); // This does not kill the thread!

	pid_ = static_cast<int>( processInfo.dwProcessId );

	stringstream info;
	info << "started external application. PID = " << pid_ << " - command = '" << strCmdLine << "'";
	logger( fmiOK, "DEBUG", info.str() );

#else

	// Creation of a child process with known PID requires to use fork() under Linux.
	pid_ = fork();

	string err;

	switch ( pid_ )
	{

	case -1: // Error.

		err = string( "fork() failed." );
		logger( fmiFatal, "ABORT", err );
		return false;

	case 0: // Child process.
	
		// Change to new working directory.
		try {
			current_path( entryPointPath );
		} catch( filesystem_error err ) {
			logger( fmiFatal, "ABORT", err.what() );
			return false;
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
		logger( fmiFatal, "ABORT", err );
		return false;

	default: // Parent process: pid_ now contains the child's PID.
		break;

	}

	stringstream info;
	info << "started external application. PID = " << pid_;
	logger( fmiOK, "DEBUG", info.str() );

#endif

	return true;
}


void
FMIComponentFrontEnd::killApplication()
{
#ifdef WIN32

	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, static_cast<DWORD>( pid_ ) );

	if( 0 != hProcess )
	{
		UINT exitCode = 0;
		TerminateProcess( hProcess, exitCode );

		stringstream info;
		info << "terminated external application. exit code = " << exitCode;
		logger( fmiOK, "DEBUG", info.str() );
	}

#else

	if ( -1 == kill( pid_, SIGTERM ) ) // Try to terminatethe process the noce way.
	{
		int errsv = errno;

		stringstream err;
		err << "unable to kill process (PID = " << pid_ << ") with SIGTERM. ERROR: "
		    << strerror( errsv ) << " --> process will be killed using SIGKILL signal.";
		logger( fmiWarning, "WARNING", err.str() );

		kill( pid_, SIGKILL ); // The nice way didn't work, hence we make short work of the process.

		logger( fmiOK, "DEBUG", "terminated external application." );
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
			logger( fmiFatal, "ABORT", err.str() );
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
	scalar->causality_ = getCausality( attributes.get<string>( "causality" ) );
	scalar->variability_ = getVariability( attributes.get<string>( "variability" ) );

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
	frontend->logger( fmiOK, "DEBUG", info.str() );

}
