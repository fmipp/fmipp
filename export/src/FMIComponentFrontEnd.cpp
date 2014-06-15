/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentFrontEnd.cpp

#include <iostream> /// \FIXME Remove.

#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#ifdef WIN32 // Visual Studio C++ & MinGW GCC use both the same Windows APIs.

#include "Windows.h"
//#include "Shlwapi.h"
#include "TCHAR.h"

#else // Use POSIX functionalities for Linux.

#include <signal.h>
#include <csignal>

#endif


#include "export/include/FMIComponentFrontEnd.h"
#include "export/include/SHMMaster.h"
#include "export/include/ScalarVariable.h"
#include "export/include/HelperFunctions.h"


using namespace std;



FMIComponentFrontEnd::FMIComponentFrontEnd( const string& instanceName, const string& fmuGUID,
					    const string& fmuLocation, const string& mimeType,
					    fmiReal timeout, fmiBoolean visible )
{
	const string seperator( "/" );
	string fileUrl = fmuLocation + seperator + string( "modelDescription.xml" );

	ModelDescription modelDescription( HelperFunctions::getPathFromUrl( fileUrl ) );

	if ( modelDescription.getGUID() != fmuGUID )
		throw runtime_error( "[FMIComponentFrontEnd] Wrong GUID." ); /// \FIXME Call logger.

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
	/// \FIXME 'type' should refer to MIME type of application, not the name of the executable.
	startApplication( modelDescription.getMIMEType(),
			  modelDescription.getEntryPoint() );

	// Create shared memory segment.
	/// \FIXME Allow other types of inter process communication!
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( pid_ );

	/// \FIXME use more sensible estimate for the segment size
	long unsigned int shmSegmentSize = 2048
		+ nRealScalars*sizeof(RealScalar)
		+ nIntegerScalars*sizeof(IntegerScalar)
		+ nBooleanScalars*sizeof(BooleanScalar)
		+ nStringScalars*2048;

	ipcMaster_ = IPCMasterFactory::createIPCMaster<SHMMaster>( shmSegmentName, shmSegmentSize );

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	// Create variables used for internal frontend/backend syncing.
	ipcMaster_->createVariable( "master_time", masterTime_, 0. );
	ipcMaster_->createVariable( "next_step_size", nextStepSize_, 0. );
	ipcMaster_->createVariable( "enforce_step", enforceTimeStep_, false );
	ipcMaster_->createVariable( "reject_step", rejectStep_, false );
	ipcMaster_->createVariable( "slave_has_terminated", slaveHasTerminated_, false );

	// Create vector of real scalar variables.
	ipcMaster_->createScalars( "real_scalars", nRealScalars, realScalars );

	// Create vector of integer scalar variables.
	ipcMaster_->createScalars( "integer_scalars", nIntegerScalars, integerScalars );

	// Create vector of boolean scalar variables.
	ipcMaster_->createScalars( "boolean_scalars", nBooleanScalars, booleanScalars );

	// Create vector of string scalar variables.
	ipcMaster_->createScalars( "string_scalars", nStringScalars, stringScalars );

	initializeVariables( modelDescription, realScalars, integerScalars, booleanScalars, stringScalars );
}


FMIComponentFrontEnd::~FMIComponentFrontEnd()
{
	if ( false == *slaveHasTerminated_ ) killApplication();

	delete ipcMaster_;
}


fmiStatus
FMIComponentFrontEnd::setReal( const fmiValueReference& ref, const fmiReal& val )
{
	// Search for value reference.
	RealMap::iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
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
		/// \FIXME Call function logger.
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_.c_str(); // Attention: fmiString <-> std::string!!!
	return fmiOK;
}


fmiStatus
FMIComponentFrontEnd::initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	*masterTime_ = tStart;

	// Synchronization point - give control to the slave.
	ipcMaster_->signalToSlave();

	// Let the slave initialize.

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

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
	if ( true == *slaveHasTerminated_ ) {
		//cout << "\t slave has terminated" << endl; fflush(stdout);
		return fmiFatal;
	}

	/// \FIXME Nothing to do here?
	if ( 0. == stepSize ) return fmiOK; // This is an event.

	//cout << "\tcomPoint = " << comPoint << " - masterTime_ = " << *masterTime_ << endl; fflush(stdout);

	if ( *masterTime_ != comPoint )
		return fmiFatal; /// \FIXME issue logger message.

	//cout << "\tstepSize = " << stepSize << " - nextStepSize_ = " << *nextStepSize_ << endl; fflush(stdout);

	if ( true == *enforceTimeStep_ )
	{
		if ( stepSize != *nextStepSize_ ) {
			//cout << "\t enforceTimeStep_ failed" << endl; fflush(stdout);
			return fmiFatal; /// \FIXME issue logger message.
		}
		*enforceTimeStep_ = false; // Reset flag.
	} else {
		*nextStepSize_ = stepSize;
	}

	//cout << "\t before signalToSlave" << endl; fflush(stdout);

	// Synchronization point - give control to slave.
	ipcMaster_->signalToSlave();

	// Let the slave do its work ...

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	//cout << "\t after waitForSlave" << endl; fflush(stdout);

	if ( true == *rejectStep_ ) {
		*rejectStep_ = false; // Reset flag.
		//cout << "\tstep rejected" << endl; fflush(stdout);
		return fmiFatal;
	}

	*masterTime_ += stepSize; // Advance time.

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
FMIComponentFrontEnd::startApplication( const string& mimeType,
					const string& inputFileUrl )
{
	if ( mimeType.substr( 0, 14 ) != string( "application/x-" ) ) {
		string err = string( "Incompatible MIME type: " ) + mimeType;
		cerr << err << endl;
		throw runtime_error( err ); /// \FIXME Call logger.
	}

	string applicationName = mimeType.substr( 14 );

#ifdef WIN32
	string strFilePath( HelperFunctions::getPathFromUrl( inputFileUrl ) );
	string seperator( " " );
	string strCmdLine( applicationName + seperator + strFilePath );
	LPTSTR cmdLine = HelperFunctions::copyStringToTCHAR( strCmdLine );

	// Specifies the window station, desktop, standard handles, and appearance of
	// the main window for a process at creation time.
	STARTUPINFO startupInfo;
	memset( &startupInfo, 0, sizeof( startupInfo ) );

	// Contains information about a newly created process and its primary thread.
	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof( processInfo ) );

	// Create the process.
	if ( false == CreateProcess( NULL, cmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS,
				     NULL, NULL, &startupInfo, &processInfo ) )
	{
		// The process could not be started ...
                cerr << "CreateProcess() failed to start process. "
                     << "ERROR = " << GetLastError() << endl; /// \FIXME Call logger.

                cerr << "cmdLine: >>>" << cmdLine << "<<<" << endl; /// \FIXME Call logger.

                cerr << "applicationName: >>>" << applicationName << "<<<" << endl; /// \FIXME Call logger.


		throw runtime_error( "CreateProcess() failed to start process." ); /// \FIXME Call logger.

	}

	CloseHandle( processInfo.hProcess ); // This does not kill the process!
	CloseHandle( processInfo.hThread ); // This does not kill the thread!

	pid_ = static_cast<int>( processInfo.dwProcessId );

#else

	string filePath = HelperFunctions::getPathFromUrl( inputFileUrl );

	// Creation of a child process with known PID requires to use fork() under Linux.
	pid_ = fork();

	string errString;

	switch ( pid_ )
	{

	case -1: // Error.

		errString = string( "fork() failed." );
		throw runtime_error( errString ); /// \FIXME Call logger.

	case 0: // Child process.

		// Start the process. execl(...) replaces the current process image with the new process image.
		execlp( applicationName.c_str(), applicationName.c_str(), filePath.c_str(), NULL );

		// execl(...) should not return.
		errString = string( "execlp(...) failed. application name = " ) + applicationName;
		cout << errString << endl; /// \FIXME Call logger.
		//throw runtime_error( errString );

	default: // Parent process: pid_ now contains the child's PID.
		break;

	}

#endif
}


void
FMIComponentFrontEnd::killApplication() const
{
#ifdef WIN32

	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, static_cast<DWORD>( pid_ ) );

	if( 0 != hProcess )
	{
		UINT exitCode = 0;
		TerminateProcess( hProcess, exitCode );
	}

#else

	kill( pid_, SIGTERM ); /// \FIXME Is SIGTERM always the correct signal?

#endif
}


void
FMIComponentFrontEnd::initializeVariables( const ModelDescription& modelDescription,
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

	const ModelDescription::Properties& modelVariables = modelDescription.getModelVariables();

	BOOST_FOREACH( const ModelDescription::Properties::value_type &v, modelVariables )
	{
		if ( v.second.find( xmlRealTag ) != v.second.not_found() )
		{
			initializeScalar( *itRealScalar, v.second, xmlRealTag );
			realScalarMap_[(*itRealScalar)->valueReference_] = *itRealScalar;
			++itRealScalar;
			continue;
		}
		else if ( v.second.find( xmlIntegerTag ) != v.second.not_found() )
		{
			initializeScalar( *itIntegerScalar, v.second, xmlIntegerTag );
			integerScalarMap_[(*itIntegerScalar)->valueReference_] = *itIntegerScalar;
			++itIntegerScalar;
			continue;
		}
		else if ( v.second.find( xmlBooleanTag ) != v.second.not_found() )
		{
			initializeScalar( *itBooleanScalar, v.second, xmlBooleanTag );
			booleanScalarMap_[(*itBooleanScalar)->valueReference_] = *itBooleanScalar;
			++itBooleanScalar;
			continue;
		}
		else if ( v.second.find( xmlStringTag ) != v.second.not_found() )
		{
			initializeScalar( *itStringScalar, v.second, xmlStringTag );
			stringScalarMap_[(*itStringScalar)->valueReference_] = *itStringScalar;
			++itStringScalar;
			continue;
		} else {
			cerr << "[FMIComponentFrontEnd] Type not supported: " 
			     << v.second.back().first << endl; /// \FIXME Use logger;
		}
	}
}


template<typename T>
void
FMIComponentFrontEnd::initializeScalar( ScalarVariable<T>* scalar,
					const ModelDescription::Properties& description,
					const string& xmlTypeTag ) const
{
	using namespace ScalarVariableAttributes;
	using namespace ModelDescriptionUtilities;

	const Properties& attributes = getAttributes( description );

	scalar->setName( attributes.get<string>( "name" ) );
	scalar->valueReference_ = attributes.get<int>( "valueReference" );
	scalar->causality_ = getCausality( attributes.get<string>( "causality" ) );
	scalar->variability_ = getVariability( attributes.get<string>( "variability" ) );

	/// \FIXME Replace try/catch with 'optional' get.
	try { // Throws in case there are no xml attributes defined.
		const Properties& properties = getChildAttributes( description, xmlTypeTag );

		if ( properties.find( "start" ) != properties.not_found() )
			scalar->value_ = properties.get<T>( "start" );
	} catch ( ... ) {} // Do nothing ...

	/// \FIXME What about the remaining properties?
}
