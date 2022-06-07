// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INTEGRATORTYPE_H
#define _FMIPP_INTEGRATORTYPE_H

/**
 * \file IntegratorType.h
 * Enumeration of available integration methods.
 *
 * \enum IntegratorType IntegratorType.h
 * Enumeration of available integration methods.
 */ 
enum IntegratorType {
    eu,   ///< Forward Euler method.
    rk,   ///< 4th order Runge-Kutta method with constant step size.
    abm,  ///< Adams-Bashforth-Moulton multistep method with adjustable order and constant step size.
    ck,   ///< 5th order Runge-Kutta-Cash-Karp method with controlled step size.
    dp,   ///< 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
    fe,   ///< 8th order Runge-Kutta-Fehlberg method with controlled step size.
    bs,   ///< Bulirsch-Stoer method with controlled step size.
    ro,   ///< 4th Rosenbrock Method for stiff problems.
#ifdef USE_SUNDIALS
    bdf,  ///< Backwards Differentiation formula from Sundials. This stepper has adaptive step size,
          ///  error control and an internal algorithm for the event search loop. The order varies
          ///  between 1 and 5. Well suited for stiff problems.
    abm2, ///< Adams bashforth moulton method from sundials. This stepper has adaptive step size,
          ///  error control, and an internal algorithm for the event search loop. The order varies
          ///  between 1 and 12. Well suited for smooth problems.
#endif
    NSTEPPERS, ///< dummy stpper which counts the number of integrators. The actual value of this enum 
               /// member depends on the flag USE_SUNDIALS
};

#endif // _FMIPP_INTEGRATORTYPE_H
