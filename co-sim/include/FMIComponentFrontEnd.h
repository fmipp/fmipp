#ifndef _FMI_COMPONENT_FRONTEND_H
#define _FMI_COMPONENT_FRONTEND_H

#include <map>
#include <vector>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "fmiModelFunctions.h"

class IPCMaster;
template<class T> class ScalarVariable;


class FMIComponentFrontEnd
{

public:

	FMIComponentFrontEnd( const std::string& instanceName, const std::string& fmuGUID,
			      const std::string& fmuLocation, const std::string& mimeType,
			      fmiReal timeout, fmiBoolean visible );

	~FMIComponentFrontEnd();

	///
	//  Functions for data exchange.
	///

	fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val );
	fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val );
	//fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val );
	//fmiStatus setString( const fmiValueReference& ref, const fmiString& val );

	fmiStatus getReal( const fmiValueReference& ref, fmiReal& val ) const;
	fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val );
	//fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val );
	//fmiStatus getString( const fmiValueReference& ref, fmiString& val );


	///
	//  Functions specific for FMI for Co-simulation.
	///

	fmiStatus initializeSlave();
	//fmiStatus terminateSlave(...); /// NOT NEEDED HERE? -> fmiFunctions.cpp
	//fmiStatus resetSlave(...);
	//fmiStatus freeSlaveInstance(...);

	//fmiStatus setRealInputDerivatives(...);
	//fmiStatus getRealOutputDerivatives(...);

	fmiStatus doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep );
	//fmiStatus cancelStep(...);

	//fmiStatus getStatus(...);
	//fmiStatus getRealStatus(...);
	//fmiStatus getIntegerStatus(...);
	//fmiStatus getBooleanStatus(...);
	//fmiStatus getStringStatus(...);


private:

	typedef ScalarVariable<fmiReal> RealScalar;
	typedef ScalarVariable<fmiInteger> IntScalar;

	typedef std::vector<RealScalar*> RealCollection;
	typedef std::vector<IntScalar*> IntCollection;

	typedef std::map<fmiValueReference, RealScalar*> RealMap;
	typedef std::map<fmiValueReference, IntScalar*> IntMap;

	typedef boost::property_tree::ptree Properties;

	RealMap realScalarMap_;
	IntMap intScalarMap_;

	IPCMaster* ipcMaster_;

	fmiReal* masterTime_;
	fmiReal* nextStepSize_;

	bool* enforceTimeStep_;
	bool* rejectStep_;

	bool* slaveHasTerminated_;

	/// Process ID of backend application.
	int pid_;


	std::string getPathFromUrl( const std::string& inputFileUrl );

	void startApplication( const std::string& applicationName,
			       const std::string& inputFileUrl );

	void killApplication() const;

	void getNumberOfVariables( const Properties& variableDescription,
				   size_t& nRealScalars,
				   size_t& nIntScalars ) const;

	void initializeVariables( const Properties& variableDescription,
				  RealCollection& realScalars,
				  IntCollection& intScalars );

	template<typename T>
	void initializeScalar( ScalarVariable<T>* scalar,
			       const Properties& description,
			       const std::string& xmlTypeTag ) const;
};



#endif // _FMI_COMPONENT_FRONTEND_H
