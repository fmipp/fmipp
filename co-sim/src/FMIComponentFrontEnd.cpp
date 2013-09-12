/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "Windows.h"
#include "Shlwapi.h"
#include "TCHAR.h"

#include "FMIComponentFrontEnd.h"
#include "SHMMaster.h"
#include "ScalarVariable.h"
#include "HelperFunctions.h"


using namespace std;



FMIComponentFrontEnd::FMIComponentFrontEnd( const string& instanceName, const string& fmuGUID,
					    const string& fmuLocation, const string& mimeType,
					    fmiReal timeout, fmiBoolean visible )
{
	Properties modelDescription;

	const string seperator( "/" );
	string fileUrl = fmuLocation + seperator + string( "modelDescription.xml" );

	// FIXME: Using c_str() here is a workaround, otherwise the conversion doesn't work properly ...
	string fileName = getPathFromUrl( fileUrl.c_str() );

	read_xml( fileName, modelDescription );

	Properties fmuDescription = modelDescription.get_child( "fmiModelDescription.<xmlattr>" );

	if ( fmuDescription.get<string>( "guid" ) != fmuGUID )
		throw runtime_error( "[FMIComponentFrontEnd] Wrong GUID." ); // FIXME: Call logger.

	const string modelVariablesTag( "fmiModelDescription.ModelVariables" );
	Properties& modelVariables = modelDescription.get_child( modelVariablesTag );

	size_t nRealScalars;
	RealCollection realScalars;

	size_t nIntScalars;
	IntCollection intScalars;

	// Parse number of model variables from model description.
	getNumberOfVariables( modelVariables, nRealScalars, nIntScalars );

	const string modelTag( "fmiModelDescription.Implementation.CoSimulation_Tool.Model.<xmlattr>" );
	Properties& model = modelDescription.get_child( modelTag );

	// Start application. FIXME: Allow to start applications remotely on other machines?
	// FIXME: 'type' should refer to MIME type of application, not the name of the executable.
	startApplication( model.get<string>( "type" ),
			  model.get<string>( "entryPoint" ) );

	// Create shared memory segment. FIXME: Allow other types of inter process communication!
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( pid_ );
	long unsigned int shmSegmentSize = 1024 + nRealScalars*sizeof(RealScalar) + nIntScalars*sizeof(IntScalar);
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
	ipcMaster_->createScalars( "int_scalars", nIntScalars, intScalars );

	initializeVariables( modelVariables, realScalars, intScalars );
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
		// FIXME: Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		// FIXME: Call function logger.
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
	IntMap::iterator itFind = intScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == intScalarMap_.end() )
	{
		// FIXME: Call function logger.
		return fmiWarning;
	}

	// Check if scalar is defined as input.
	if ( itFind->second->causality_ != ScalarVariableAttributes::input )
	{
		// FIXME: Call function logger.
		return fmiWarning;
	}

	// Set value.
	itFind->second->value_ = val;
	return fmiOK;
}


//fmiStatus FMIComponentFrontEnd::setBoolean( const fmiValueReference& ref, const fmiBoolean& val );
//fmiStatus FMIComponentFrontEnd::setString( const fmiValueReference& ref, const fmiString& val );


fmiStatus
FMIComponentFrontEnd::getReal( const fmiValueReference& ref, fmiReal& val ) const
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		// FIXME: Call function logger.
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
	IntMap::const_iterator itFind = intScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == intScalarMap_.end() )
	{
		// FIXME: Call function logger.
		val = 0;
		return fmiWarning;
	}

	// Get value.
	val = itFind->second->value_;
	return fmiOK;
}


//fmiStatus FMIComponentFrontEnd::getBoolean( const fmiValueReference& ref, fmiBoolean& val );
//fmiStatus FMIComponentFrontEnd::getString( const fmiValueReference& ref, fmiString& val );


fmiStatus
FMIComponentFrontEnd::initializeSlave()
{
	// Synchronization point - give control to the slave.
	ipcMaster_->signalToSlave();

	// Let the slave initialize.

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	return fmiOK;
}


//fmiStatus FMIComponentFrontEnd::terminateSlave(...) {} /// NOT NEEDED HERE? -> fmiFunctions.cpp
//fmiStatus FMIComponentFrontEnd::resetSlave(...) {}
//fmiStatus FMIComponentFrontEnd::freeSlaveInstance(...) {}
//fmiStatus FMIComponentFrontEnd::setRealInputDerivatives(...) {}
//fmiStatus FMIComponentFrontEnd::getRealOutputDerivatives(...) {}


fmiStatus
FMIComponentFrontEnd::doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep )
{
	if ( true == *slaveHasTerminated_ ) return fmiFatal;

	if ( 0. == stepSize ) return fmiOK; // This is an event. FIXME: Nothing to do here?

	//cout << "\tcomPoint = " << comPoint << " - masterTime_ = " << *masterTime_ << endl;

	if ( *masterTime_ != comPoint )
		return fmiFatal; // FIXME: issue logger message.

	//cout << "\tstepSize = " << stepSize << " - nextStepSize_ = " << *nextStepSize_ << endl;

	if ( true == *enforceTimeStep_ )
	{
		if ( stepSize != *nextStepSize_ ) return fmiFatal; // FIXME: issue logger message.
		*enforceTimeStep_ = false; // Reset flag.
	} else {
		*nextStepSize_ = stepSize;
	}

	// Synchronization point - give control to slave.
	ipcMaster_->signalToSlave();

	// Let the slave do its work ...

	// Synchronization point - take control back from slave.
	ipcMaster_->waitForSlave();

	if ( true == *rejectStep_ ) {
		*rejectStep_ = false; // Reset flag.
		return fmiFatal;
	}

	*masterTime_ += stepSize; // Advance time.

	return fmiOK;
}


//fmiStatus FMIComponentFrontEnd::cancelStep(...) {}

//fmiStatus FMIComponentFrontEnd::getStatus(...) {}
//fmiStatus FMIComponentFrontEnd::getRealStatus(...) {}
//fmiStatus FMIComponentFrontEnd::getIntegerStatus(...) {}
//fmiStatus FMIComponentFrontEnd::getBooleanStatus(...) {}
//fmiStatus FMIComponentFrontEnd::getStringStatus(...) {}



string
FMIComponentFrontEnd::getPathFromUrl( const std::string& inputFileUrl )
{
	LPCTSTR fileUrl = HelperFunctions::copyStringToTCHAR( inputFileUrl );
	LPTSTR filePath = new TCHAR[MAX_PATH];
	DWORD filePathSize, tmp;
	PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );

	delete fileUrl;

	return string( filePath );
}


void
FMIComponentFrontEnd::startApplication( const string& applicationName,
					const string& inputFileUrl )
{
	LPCTSTR fileUrl = HelperFunctions::copyStringToTCHAR( inputFileUrl );
	LPTSTR filePath = new TCHAR[MAX_PATH];
	DWORD filePathSize, tmp;
	PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );

	string seperator( " " );
	LPTSTR cmdLine = HelperFunctions::copyStringToTCHAR( applicationName + seperator, filePathSize );

	_tcscat( cmdLine, filePath );

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
		     << "ERROR = " << GetLastError() << endl;

		return;
	}

	CloseHandle( processInfo.hProcess ); // This does not kill the process!
	CloseHandle( processInfo.hThread ); // This does not kill the thread!

	pid_ = static_cast<int>( processInfo.dwProcessId );
}


void
FMIComponentFrontEnd::killApplication() const
{
	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, static_cast<DWORD>( pid_ ) );

	if( 0 != hProcess )
	{
		UINT exitCode;
		TerminateProcess( hProcess, exitCode );
	}
}


void
FMIComponentFrontEnd::getNumberOfVariables( const Properties& variableDescription,
					    size_t& nRealScalars,
					    size_t& nIntScalars ) const
{
	// Define XML tags to search for.
	const string xmlRealTag( "Real" );
	const string xmlIntTag( "Integer" );

	// Reset counters.
	nRealScalars = 0;
	nIntScalars = 0;

	BOOST_FOREACH( const Properties::value_type &v, variableDescription )
	{
		if ( v.second.find( xmlRealTag ) != v.second.not_found() ) { ++nRealScalars; continue; }
		if ( v.second.find( xmlIntTag ) != v.second.not_found() ) { ++nIntScalars; continue; }

		// FIXME: Include fmiBoolean, fmiString, ...
		throw runtime_error( "[FMIComponentFrontEnd] Type not supported." );
	}
}


void
FMIComponentFrontEnd::initializeVariables( const Properties& variableDescription,
					   RealCollection& realScalars,
					   IntCollection& intScalars )
{
	RealCollection::iterator itRealScalar = realScalars.begin();
	IntCollection::iterator itIntScalar = intScalars.begin();

	const string xmlRealTag( "Real" );
	const string xmlIntTag( "Integer" );

	BOOST_FOREACH( const Properties::value_type &v, variableDescription )
	{
		if ( v.second.find( xmlRealTag ) != v.second.not_found() )
		{
			initializeScalar( *itRealScalar, v.second, xmlRealTag );
			realScalarMap_[(*itRealScalar)->valueReference_] = *itRealScalar;
			++itRealScalar;
			continue;
		}

		if ( v.second.find( xmlIntTag ) != v.second.not_found() )
		{
			initializeScalar( *itIntScalar, v.second, xmlIntTag );
			intScalarMap_[(*itIntScalar)->valueReference_] = *itIntScalar;
			++itIntScalar;
			continue;
		}

		// FIXME: Include fmiBoolean, fmiString, ...
		throw runtime_error( "[FMIComponentFrontEnd] Type not supported" );
	}
}


template<typename T>
void
FMIComponentFrontEnd::initializeScalar( ScalarVariable<T>* scalar,
					const Properties& description,
					const string& xmlTypeTag ) const
{
	using namespace ScalarVariableAttributes;

	const Properties& attributes = description.get_child( "<xmlattr>" );

	scalar->setName( attributes.get<string>( "name" ) );
	scalar->valueReference_ = attributes.get<int>( "valueReference" );
	scalar->causality_ = getCausality( attributes.get<string>( "causality" ) );
	scalar->variability_ = getVariability( attributes.get<string>( "variability" ) );

	const Properties& properties = description.get_child( xmlTypeTag ).get_child( "<xmlattr>" );

	if ( properties.find( "start" ) != properties.not_found() )
		scalar->value_ = properties.get<T>( "start" );

	//FIXME: What about the remaining properties?
}
