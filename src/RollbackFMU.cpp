#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <limits>

#include "RollbackFMU.h"


using namespace std;


RollbackFMU::RollbackFMU( const string& modelName ) :
	FMU( modelName ),
	rollbackState_( getTime(), nStates(), 0 )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif
}


RollbackFMU::RollbackFMU( const string& fmuPath,
			  const string& modelName ) :
	FMU( fmuPath, modelName ),
	rollbackState_( getTime(), nStates(), 0 )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif
}

RollbackFMU::RollbackFMU( const string& xmlPath,
			  const std::string& dllPath,
			  const string& modelName ) :
	FMU( xmlPath, dllPath, modelName ),
	rollbackState_( getTime(), nStates(), 0 )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif
}


RollbackFMU::RollbackFMU( const RollbackFMU& aRollbackFMU ) :
	FMU( aRollbackFMU ),
	rollbackState_( aRollbackFMU.rollbackState_ )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor]" << endl; fflush( stdout );
#endif
}


RollbackFMU::~RollbackFMU() {}


fmiStatus RollbackFMU::rollback( fmiTime time )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::rollback]" << endl; fflush( stdout );
#endif
	if ( time < rollbackState_.time_ ) return fmiFatal;

	setContinuousStates( rollbackState_.state_ );
	setTime( rollbackState_.time_ );

	raiseEvent();
	handleEvents( time, true );

	return fmiOK;
}

fmiReal RollbackFMU::integrate( fmiReal tstop, unsigned int nsteps )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::integrate]" << endl; fflush( stdout );
#endif
	fmiTime now = getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmiOK != rollback( tstop ) ) return now;
	} else { // Retrieve current state and store it as rollback state.
		rollbackState_.time_ = now;
		getContinuousStates( rollbackState_.state_ );
	}

	// Integrate.
	assert( nsteps > 0 );
	double deltaT = ( tstop - rollbackState_.time_ ) / nsteps;
	return FMU::integrate( tstop, deltaT );
}


fmiReal RollbackFMU::integrate( fmiReal tstop, double deltaT )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::integrate]" << endl; fflush( stdout );
#endif
	fmiTime now = getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmiOK != rollback( tstop ) ) return now;
	} else { // Retrieve current state and store it as rollback state.
		rollbackState_.time_ = now;
		getContinuousStates( rollbackState_.state_ );
	}

	// Integrate.
	return FMU::integrate( tstop, deltaT );
}
