// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_BACKENDAPPLICATIONBASE_H
#define _FMIPP_BACKENDAPPLICATIONBASE_H

#include <vector>

#include "common/FMIPPConfig.h"
#include "export/include/FMIComponentBackEnd.h"

class __FMI_DLL BackEndApplicationBase {

public:

	/// Constructor.
	BackEndApplicationBase();

	/// Destructor.
	virtual ~BackEndApplicationBase();

	/// Initialization method.
	int initializeBase( int argc, const char* argv[] );

	/// Check if the backend is ready to enter the simulation loop.
	bool readyToLoop();

	/// Simulation method.
	int doStepBase();

	/// Get current communication point from the front end.
	const fmippTime& getCurrentCommunicationPoint() const;

	/// Get next communication step size from the front end.
	const fmippTime& getCommunicationStepSize() const;

	/// Get simulation stop time.
	const fmippTime& getStopTime() const;

	/// Get flag indicating if simulation stop time has been defined.
	const bool& getStopTimeDefined() const;

	/// Enforce a specific time step length.
	void enforceTimeStep( const fmippTime& fixedTimeStep );

	/// Check if logging is activated.
	bool loggingOn() const;

	/// Call the logger.
	void logger( fmippStatus status, const fmippString& category, const fmippString& msg );

protected:

	/** This function initializes the backend's scalar variables (parameters, inputs, outputs).
	 *  Only calls to 'addRealInput(...)', 'addRealOutput(...)', etc. are allowed.
	 *  To be implemented by the inheriting class.
	 */
	virtual void initializeScalarVariables() = 0;

	/** This function will be called after 'initializeScalarVariables'.
	 *  It is intended to initialize the back end (i.e., everything except the scalar variables).
	 *  The function arguments are the command line arguments according to the model description.
	 *  To be implemented by the inheriting class.
	 */
	virtual int initializeBackEnd( int argc, const char* argv[] ) = 0;

	/** This function will be called after 'initializeBackEnd'.
	 *  It is intended to set the values of parameters. When this function is called, the values
	 *  defined in the model description are already applied to the scalar variables. Therefore,
	 *  care has to be taken that start values defined in the model description are not overwritten.
	 *  To be implemented by the inheriting class.
	 */
	virtual void initializeParameterValues() = 0;

	/** This function will be called whenever the front end's 'doStep(...)' methid is called.
	 *  To be implemented by the inheriting class.
	 */
	virtual int doStep( const fmippTime& syncTime, const fmippTime& lastSyncTime ) = 0;

protected:

	std::vector<fmippReal*> realParams_;
	std::vector<fmippString> realParamNames_;

	std::vector<fmippInteger*> integerParams_;
	std::vector<fmippString> integerParamNames_;

	std::vector<fmippBoolean*> booleanParams_;
	std::vector<fmippString> booleanParamNames_;

	std::vector<fmippString*> stringParams_;
	std::vector<fmippString> stringParamNames_;

	std::vector<fmippReal*> realInputs_;
	std::vector<fmippString> realInputNames_;

	std::vector<fmippInteger*> integerInputs_;
	std::vector<fmippString> integerInputNames_;

	std::vector<fmippBoolean*> booleanInputs_;
	std::vector<fmippString> booleanInputNames_;

	std::vector<fmippString*> stringInputs_;
	std::vector<fmippString> stringInputNames_;


	std::vector<fmippReal*> realOutputs_;
	std::vector<fmippString> realOutputNames_;

	std::vector<fmippInteger*> integerOutputs_;
	std::vector<fmippString> integerOutputNames_;

	std::vector<fmippBoolean*> booleanOutputs_;
	std::vector<fmippString> booleanOutputNames_;

	std::vector<fmippString*> stringOutputs_;
	std::vector<fmippString> stringOutputNames_;

	/// Set this flag to true in case the backend is initialized
	/// and ready to enter the simulation loop.
	bool readyToLoop_;

private:

	FMIComponentBackEnd* backend_; ///< Internal pointer to backend.

	fmippReal syncTime_;
	fmippReal lastSyncTime_;

	fmippStatus initParameters(); ///< Initialize paramters.
	fmippStatus getParameters(); ///< Get paramter values.
	fmippStatus setParameters(); ///< Set paramter values.

	fmippStatus initInputs(); ///< Initialize input variables.
	fmippStatus getInputs(); ///< Get input variable values.
	fmippStatus resetInputs(); ///< Reset input variable values (i.e., overwrite values of input variables in the frontend with values provided by the backend.)

	fmippStatus initOutputs(); ///< Initialize output variables.
	fmippStatus setOutputs(); ///< Set output variable values.

	void writeScalarVariableNamesToJSONFile( const fmippString& filename ) ; ///< Write the names of all scalar variables to a single JSON file.
	void writeScalarVariableNamesToFiles(); ///< Write the names of all scalar variables to separate files.
	void writeVectorContentToFile( const std::vector<fmippString>& vec, const fmippString& filename ) const; ///< Write the contents of a vector of strings to file.
};

#define addRealParameter( var ) { realParamNames_.push_back( #var ); realParams_.push_back( &var ); }
#define addIntegerParameter( var ) { integerParamNames_.push_back( #var ); integerParams_.push_back( &var ); }
#define addBooleanParameter( var ) { booleanParamNames_.push_back( #var ); booleanParams_.push_back( &var ); }
#define addStringParameter( var ) { stringParamNames_.push_back( #var ); stringParams_.push_back( &var ); }


#define addRealInput( var ) { realInputNames_.push_back( #var ); realInputs_.push_back( &var ); }
#define addIntegerInput( var ) { integerInputNames_.push_back( #var ); integerInputs_.push_back( &var ); }
#define addBooleanInput( var ) { booleanInputNames_.push_back( #var ); booleanInputs_.push_back( &var ); }
#define addStringInput( var ) { stringInputNames_.push_back( #var ); stringInputs_.push_back( &var ); }


#define addRealOutput( var ) { realOutputNames_.push_back( #var ); realOutputs_.push_back( &var ); }
#define addIntegerOutput( var ) { integerOutputNames_.push_back( #var ); integerOutputs_.push_back( &var ); }
#define addBooleanOutput( var ) { booleanOutputNames_.push_back( #var ); booleanOutputs_.push_back( &var ); }
#define addStringOutput( var ) { stringOutputNames_.push_back( #var ); stringOutputs_.push_back( &var ); }


#define addRealParameterWithName( varname, var ) { realParamNames_.push_back( varname ); realParams_.push_back( &var ); }
#define addIntegerParameterWithName( varname, var ) { integerParamNames_.push_back( varname ); integerParams_.push_back( &var ); }
#define addBooleanParameterWithName( varname, var ) { booleanParamNames_.push_back( varname ); booleanParams_.push_back( &var ); }
#define addStringParameterWithName( varname, var ) { stringParamNames_.push_back( varname ); stringParams_.push_back( &var ); }


#define addRealInputWithName( varname, var ) { realInputNames_.push_back( varname ); realInputs_.push_back( &var ); }
#define addIntegerInputWithName( varname, var ) { integerInputNames_.push_back( varname ); integerInputs_.push_back( &var ); }
#define addBooleanInputWithName( varname, var ) { booleanInputNames_.push_back( varname ); booleanInputs_.push_back( &var ); }
#define addStringInputWithName( varname, var ) { stringInputNames_.push_back( varname ); stringInputs_.push_back( &var ); }


#define addRealOutputWithName( varname, var ) { realOutputNames_.push_back( varname ); realOutputs_.push_back( &var ); }
#define addIntegerOutputWithName( varname, var ) { integerOutputNames_.push_back( varname ); integerOutputs_.push_back( &var ); }
#define addBooleanOutputWithName( varname, var ) { booleanOutputNames_.push_back( varname ); booleanOutputs_.push_back( &var ); }
#define addStringOutputWithName( varname, var ) { stringOutputNames_.push_back( varname ); stringOutputs_.push_back( &var ); }

#define CREATE_BACKEND_APPLICATION( BACKENDTYPE ) \
int main( int argc, const char* argv[] ) { \
	BACKENDTYPE backend; \
	if ( 0 != backend.initializeBase( argc, argv ) ) { return -1; } \
	while ( true == backend.readyToLoop() ) { if ( 0 != backend.doStepBase() ) return -1; } \
	return 0; }

#endif // _FMIPP_BACKENDAPPLICATIONBASE_H
