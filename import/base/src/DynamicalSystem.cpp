#include "import/base/include/DynamicalSystem.h"
#include "import/base/include/NumericalJacobianCoefficients.icc"

void DynamicalSystem::getJac( real_type** J ){
	double t = getTime();
	double* x = new double( nStates() );
	double* dfdt = new double( nStates() );
	double* Jp = &J[0][0];                    // \TODO: check this for errors
	getNumericalJacobian( Jp, x, dfdt, t );
}

void DynamicalSystem::getNumericalJacobian( real_type* J, const real_type* x, real_type* dfdt, const real_type t )
{
	const int steps = 3;            // determines the order of accuracy for the Jacobian
	                                // and also influences the runtime
        NumericalJacobianCoefficients<steps> coefs;
	double* xp = (double*) x;       // using a copy would be safer
	double* Jp = J;
	const int N = nStates();
	double* dx = new double( N );   // \TODO: make dx a class member to prevent consecutive
	                                //        allocation/deallocation of memory
	setTime( t );
	setContinuousStates( xp );

	// step size for the finite difference
	double delta = 1.0e-5;   // \TODO: this value for delta is suited for a certain system.
	                         //        find a solution that is not hardware specific.
	double h = delta;

	for( int j = 0; j < N; j++ ){
		/// calculate the j-th column of the jacobian matrix

		/// clear the matrix column
		for( int i = 0; i < N; i++ ){
			Jp[N*i+j] = 0;
		}

		/// use a k step metod
		for( int k = 0; k < steps; k++ ){
			// add the vector
			//     coefs[k] * ( f(x+(k+1)ej*h)-f(x-(k+1)*ej)*h )/h
			// to the j-th column of J. where
			//     e1 = (1,0,0,0,...,0)
			//     e2 = (0,1,0,0,...,0)
			//     e3 = (0,0,1,0,....0)
			//     ....
			//     eN = (0,0,...,0,0,1)

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
	double t2 = t;
	for( int i = 0; i < N; i++ )
		dfdt[i] = 0.0;
	for( int k = 0; k < steps; k++ ){
		t2 += (k+1.0)*h;
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

bool DynamicalSystem::checkStateEvent( real_type* eventsind ){
	real_type preeventsind [ nEventInds() ];
	getEventIndicators( preeventsind );
	bool stateEvent = false;
	for ( size_t i = 0; i < nEventInds(); i++ )
		stateEvent = stateEvent || ( preeventsind[i] * eventsind[i] < 0 );
	return stateEvent;
};
