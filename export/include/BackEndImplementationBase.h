#ifndef _FMIPP_BACKENDIMPLEMENTATIONBASE_H
#define _FMIPP_BACKENDIMPLEMENTATIONBASE_H

#include <string>
#include <vector>

#include "export/include/FMIComponentBackEnd.h"


class __FMI_DLL BackEndImplementationBase {
	
public:

	/// Constructor.
	BackEndImplementationBase();

	/// Destructor.
	virtual ~BackEndImplementationBase();

	/// Initialization method.
	int initializeBase( int argc, const char* argv[] );

	/// Simulation method.
	int doStepBase();

	/// Enforce a specific time step length.
	void enforceTimeStep( const fmiReal& fixedTimeStep );

	/// Call the logger.
	void logger( fmiStatus status, const std::string& category, const std::string& msg );

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
	virtual int doStep( const fmiReal& syncTime, const fmiReal& lastSyncTime ) = 0;

protected:

	std::vector<fmiReal*> realParams_;
	std::vector<std::string> realParamNames_;

	std::vector<fmiInteger*> integerParams_;
	std::vector<std::string> integerParamNames_;

	std::vector<fmiBoolean*> booleanParams_;
	std::vector<std::string> booleanParamNames_;

	std::vector<std::string*> stringParams_;
	std::vector<std::string> stringParamNames_;
	
	std::vector<fmiReal*> realInputs_;
	std::vector<std::string> realInputNames_;

	std::vector<fmiInteger*> integerInputs_;
	std::vector<std::string> integerInputNames_;

	std::vector<fmiBoolean*> booleanInputs_;
	std::vector<std::string> booleanInputNames_;

	std::vector<std::string*> stringInputs_;
	std::vector<std::string> stringInputNames_;


	std::vector<fmiReal*> realOutputs_;
	std::vector<std::string> realOutputNames_;

	std::vector<fmiInteger*> integerOutputs_;
	std::vector<std::string> integerOutputNames_;

	std::vector<fmiBoolean*> booleanOutputs_;
	std::vector<std::string> booleanOutputNames_;

	std::vector<std::string*> stringOutputs_;
	std::vector<std::string> stringOutputNames_;


private:

	FMIComponentBackEnd* backend_; ///< Internal pointer to backend.

	fmiReal syncTime_;
	fmiReal lastSyncTime_;

	fmiStatus initParameters(); ///< Initialize paramters.
	fmiStatus getParameters(); ///< Get paramter values.
	fmiStatus setParameters(); ///< Set paramter values.

	fmiStatus initInputs(); ///< Initialize input variables.
	fmiStatus getInputs(); ///< Get input variable values.

	fmiStatus initOutputs(); ///< Initialize output variables.
	fmiStatus setOutputs(); ///< Set output variable values.
	
	void writeScalarVariableNamesToFile(); ///< Write the names of all scalar variables to files.
	void writeVectorContentToFile( const std::vector<std::string>& vec, const std::string& filename ) const; ///< Write the contents of a vector of strings to file.
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


#define CREATE_FMU_BACKEND( BACKENDTYPE ) \
int main( int argc, const char* argv[] ) { \
	BACKENDTYPE backend; \
	if ( 0 != backend.initializeBase( argc, argv ) ) { return -1; } \
	while ( true ) { if ( 0 != backend.doStepBase() ) return -1; } \
	return 0; }

#endif
