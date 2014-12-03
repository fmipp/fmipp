/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INTEGRATORTYPE_H
#define _FMIPP_INTEGRATORTYPE_H

/**
 * \file IntegratorType.h 
 *
 * \enum IntegratorType IntegratorType.h 
 * Enumeration of available integration methods.
 */ 
enum IntegratorType {
	eu, ///< Forward Euler method. 
	rk, ///< 4th order Runge-Kutta method with constant step size.
	ck, ///< 5th order Runge-Kutta-Cash-Karp method with controlled step size.
	dp, ///< 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
	fe, ///< 7th order Runge-Kutta-Fehlberg method with controlled step size. 
	bs, ///< Bulirsch-Stoer method with controlled step size.
#ifdef USE_SUNDIALS
	bdf, ///< Solver from CVode (Sundials)
#endif
	abm ///< Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
};


#endif // _FMIPP_INTEGRATORTYPE_H
