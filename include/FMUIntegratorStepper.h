#ifndef _MFIPP_FMUINTEGRATORSTEPPER_H
#define _MFIPP_FMUINTEGRATORSTEPPER_H

#include "FMUIntegrator.h"


class FMUIntegratorStepper
{

public:

	virtual ~FMUIntegratorStepper();

	typedef FMUIntegrator::IntegratorType IntegratorType;

	virtual void invokeMethod( FMUIntegrator* fmuint, FMUIntegrator::state_type& states,
				   fmiReal time, fmiReal step_size, size_t n_steps ) = 0;

	virtual IntegratorType type() const = 0;

	static FMUIntegratorStepper* createStepper( IntegratorType type );
};


#endif
