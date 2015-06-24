#include "import/base/include/DynamicalSystem.h"
#include "import/base/include/NumericalJacobianCoefficients.icc"
#include <iostream>


DynamicalSystem::DynamicalSystem()
{
	integrator_           = new Integrator( this );
	savedEventIndicators_ = 0;
}


DynamicalSystem::~DynamicalSystem()
{
	delete integrator_;
	if ( 0 != savedEventIndicators_ )
		delete savedEventIndicators_;
}


fmiStatus DynamicalSystem::getJac( fmiReal* J ){
	/* if this function is not overwiritten by derived classes, warn the user about the not
	   implemented functionality */
	return fmiWarning;
}


void DynamicalSystem::getNumericalJacobian( fmiReal* J, const fmiReal* x, fmiReal* dfdt, const fmiReal t )
{
	/**
	 * the method used is of 6th order and uses 6*NEQ rhs evaluations. for comparison - the forward
	 * differences method (1st order) uses NEQ+1 rhs evaluations.
	 */
	const int steps = 3;            // determines the order of accuracy for the Jacobian
	                                // and also influences the runtime
	NumericalJacobianCoefficients<steps> coefs;
	fmiReal* xp = (fmiReal*) x;     // using a copy would be safer
	fmiReal* Jp = J;
	const int N = nStates();
	fmiReal* dx = new double[ N ];  // \TODO: make dx a class member to prevent consecutive
	                                //        allocation/deallocation of memory
	setTime( t );
	setContinuousStates( xp );

	// step size for the finite difference
	fmiReal delta = 1.0e-5;   /// \todo this value for delta is only suited for a certain hardwares
	                          ///       and Models. Find a solution that is not hardware/model specific.
	fmiReal h = delta;

	for( int j = 0; j < N; j++ ){
		// calculate the j-th column of the jacobian matrix

		// clear the matrix column
		for( int i = 0; i < N; i++ ){
			Jp[N*i+j] = 0;
		}

		// use a k step metod
		for( int k = 0; k < steps; k++ ){
			/*
			 * add the vector
			 *     coefs[k] * ( f(x+(k+1)ej*h)-f(x-(k+1)*ej)*h )/h
			 * to the j-th column of J. where
			 *     e1 = (1,0,0,0,...,0)
			 *     e2 = (0,1,0,0,...,0)
			 *     e3 = (0,0,1,0,....0)
			 *     ....
			 *     eN = (0,0,...,0,0,1)
			 */

			xp[j] += ( k + 1.0 )*h;
			setContinuousStates( xp );
			getDerivatives( dx );
			for( int i = 0; i < N; i++ )
				{
					Jp[N*i+j] += dx[i]*coefs[k]/h;
				}
			xp[j] -= 2.0*( k + 1.0 )*h;
			setContinuousStates( xp );
			getDerivatives( dx );
			for( int i = 0; i < N; i++ )
				{
					Jp[N*i+j] -= dx[i]*coefs[k]/h;
				}
			xp[j] += ( k + 1.0 )*h;
		}
	}
	setContinuousStates( xp );

	// calculate the derivative with respect to time using the same stategy as before.
	fmiTime t2 = t;
	for( int i = 0; i < N; i++ )
		dfdt[i] = 0.0;
	for( int k = 0; k < steps; k++ ){
		t2 += ( k + 1.0 )*h;
		setTime( t2 );
		getDerivatives( dx );
		for( int i = 0; i < N; i++ )
			{
				dfdt[i] += dx[i]*coefs[k]/h;
			}
		t2 -= 2.0*( k + 1.0 )*h;
		setTime( t2 );
		getDerivatives( dx );
		for( int i = 0; i < N; i++ )
			{
				dfdt[i] -= dx[i]*coefs[k]/h;
			}
		t2 += (k+1.0)*h;
	}
	delete dx;
}


void DynamicalSystem::saveEventIndicators(){
	if ( 0 == savedEventIndicators_ )
		if ( 0 != nEventInds() )
			savedEventIndicators_ = new fmiReal[ nEventInds() ];
	getEventIndicators( savedEventIndicators_ );
}


bool DynamicalSystem::checkStateEvent(){
	if ( 0 == savedEventIndicators_ )
		return false;
	fmiReal* currentEventIndicators = new fmiReal[ nEventInds() ];
	getEventIndicators( currentEventIndicators );
	for ( size_t i = 0; i < nEventInds(); i++ )
		if ( currentEventIndicators[i] * savedEventIndicators_[i] < 0 ){
			delete[] currentEventIndicators;
			return true;
		}
	delete[] currentEventIndicators;
	return false;
}
