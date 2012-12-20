#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jmi.h>
#include <jmi_block_residual.h>
#include <fmi.h>



#define MODEL_IDENTIFIER foo
#define C_GUID "609f7e85ff6514f3e7178e7d0dca32e9"
#include "fmiModelFunctions.h"

static const int N_real_ci = 0;
static const int N_real_cd = 0;
static const int N_real_pi = 6;
static const int N_real_pd = 0;

static const int N_integer_ci = 0 + 0;
static const int N_integer_cd = 0 + 0;
static const int N_integer_pi = 2 + 0;
static const int N_integer_pd = 0 + 0;

static const int N_boolean_ci = 0;
static const int N_boolean_cd = 0;
static const int N_boolean_pi = 7;
static const int N_boolean_pd = 0;

static const int N_string_ci = 0;
static const int N_string_cd = 0;
static const int N_string_pi = 0;
static const int N_string_pd = 0;

static const int N_real_dx = 2;
static const int N_real_x = 2;
static const int N_real_u = 0;
static const int N_real_w = 0;

static const int N_real_d = 0;

static const int N_integer_d = 0 + 0;
static const int N_integer_u = 0 + 0;

static const int N_boolean_d = 0;
static const int N_boolean_u = 0;

static const int N_string_d = 0;
static const int N_string_u = 0;

static const int N_ext_objs = 0;

static const int N_sw = 0;
static const int N_eq_F = 2;
static const int N_eq_R = 0;

static const int N_dae_blocks = 0;
static const int N_dae_init_blocks = 0;
static const int N_guards = 0;

static const int N_eq_F0 = 2 + 2;
static const int N_eq_F1 = 4;
static const int N_eq_Fp = 0;
static const int N_eq_R0 = 0 + 0;
static const int N_sw_init = 0;
static const int N_guards_init = 0;

static const int N_eq_J = 0;
static const int N_eq_L = 0;
static const int N_eq_opt_Ffdp = 0;
static const int N_eq_Ceq = 0;
static const int N_eq_Cineq = 0;
static const int N_eq_Heq = 0;
static const int N_eq_Hineq = 0;
static const int N_t_p = 0;

static const int Scaling_method = JMI_SCALING_NONE;

#define sf(i) (jmi->variable_scaling_factors[i])

static const int N_outputs = 0;
static const int Output_vrefs[1] = {-1};

static int CAD_dae_n_nz = 1;
static const int CAD_dae_nz_rows[1] = {-1};
static const int CAD_dae_nz_cols[1] = {-1};


static const int CAD_ODE_A_n_nz = 0;
static const int CAD_ODE_B_n_nz = 0;
static const int CAD_ODE_C_n_nz = 0;
static const int CAD_ODE_D_n_nz = 0;
static const int CAD_ODE_A_nz_rows[1] = {-1};
static const int CAD_ODE_A_nz_cols[1] = {-1};
static const int CAD_ODE_B_nz_rows[1] = {-1};
static const int CAD_ODE_B_nz_cols[1] = {-1};
static const int CAD_ODE_C_nz_rows[1] = {-1};
static const int CAD_ODE_C_nz_cols[1] = {-1};
static const int CAD_ODE_D_nz_rows[1] = {-1};
static const int CAD_ODE_D_nz_cols[1] = {-1};


#define _w_2 ((*(jmi->z))[jmi->offs_real_pi+0])
#define __events_default_tol_6 ((*(jmi->z))[jmi->offs_real_pi+1])
#define __events_tol_factor_7 ((*(jmi->z))[jmi->offs_real_pi+2])
#define __nle_solver_default_tol_10 ((*(jmi->z))[jmi->offs_real_pi+3])
#define __nle_solver_min_tol_12 ((*(jmi->z))[jmi->offs_real_pi+4])
#define __nle_solver_tol_factor_13 ((*(jmi->z))[jmi->offs_real_pi+5])
#define __log_level_8 ((*(jmi->z))[jmi->offs_integer_pi+0])
#define __nle_solver_log_level_11 ((*(jmi->z))[jmi->offs_integer_pi+1])
#define __enforce_bounds_5 ((*(jmi->z))[jmi->offs_boolean_pi+0])
#define __nle_solver_check_jac_cond_9 ((*(jmi->z))[jmi->offs_boolean_pi+1])
#define __rescale_after_singular_jac_14 ((*(jmi->z))[jmi->offs_boolean_pi+2])
#define __rescale_each_step_15 ((*(jmi->z))[jmi->offs_boolean_pi+3])
#define __use_Brent_in_1d_16 ((*(jmi->z))[jmi->offs_boolean_pi+4])
#define __use_automatic_scaling_17 ((*(jmi->z))[jmi->offs_boolean_pi+5])
#define __use_jacobian_scaling_18 ((*(jmi->z))[jmi->offs_boolean_pi+6])
#define _der_x_3 ((*(jmi->z))[jmi->offs_real_dx+0])
#define _der_y_4 ((*(jmi->z))[jmi->offs_real_dx+1])
#define _x_0 ((*(jmi->z))[jmi->offs_real_x+0])
#define _y_1 ((*(jmi->z))[jmi->offs_real_x+1])
#define _time ((*(jmi->z))[jmi->offs_t])
#define pre_der_x_3 ((*(jmi->z))[jmi->offs_pre_real_dx+0])
#define pre_der_y_4 ((*(jmi->z))[jmi->offs_pre_real_dx+1])
#define pre_x_0 ((*(jmi->z))[jmi->offs_pre_real_x+0])
#define pre_y_1 ((*(jmi->z))[jmi->offs_pre_real_x+1])


const char *fmi_runtime_options_map_names[] = {
    "_enforce_bounds",
    "_events_default_tol",
    "_events_tol_factor",
    "_log_level",
    "_nle_solver_check_jac_cond",
    "_nle_solver_default_tol",
    "_nle_solver_log_level",
    "_nle_solver_min_tol",
    "_nle_solver_tol_factor",
    "_rescale_after_singular_jac",
    "_rescale_each_step",
    "_use_Brent_in_1d",
    "_use_automatic_scaling",
    "_use_jacobian_scaling",
    NULL
};

const int fmi_runtime_options_map_vrefs[] = {
    536870920, 1, 2, 268435462, 536870921, 3, 268435463, 4, 5, 536870922,
    536870923, 536870924, 536870925, 536870926, 0
};

const int fmi_runtime_options_map_length = 14;

#define _real_ci(i) ((*(jmi->z))[jmi->offs_real_ci+i])
#define _real_cd(i) ((*(jmi->z))[jmi->offs_real_cd+i])
#define _real_pi(i) ((*(jmi->z))[jmi->offs_real_pi+i])
#define _real_pd(i) ((*(jmi->z))[jmi->offs_real_pd+i])
#define _real_dx(i) ((*(jmi->z))[jmi->offs_real_dx+i])
#define _real_x(i) ((*(jmi->z))[jmi->offs_real_x+i])
#define _real_u(i) ((*(jmi->z))[jmi->offs_real_u+i])
#define _real_w(i) ((*(jmi->z))[jmi->offs_real_w+i])
#define _t ((*(jmi->z))[jmi->offs_t])
#define _real_dx_p(j,i) ((*(jmi->z))[jmi->offs_real_dx_p + \
  j*(jmi->n_real_dx + jmi->n_real_x + jmi->n_real_u + jmi->n_real_w)+ i])
#define _real_real_x_p(j,i) ((*(jmi->z))[jmi->offs_real_x_p + \
  j*(jmi->n_real_dx + jmi->n_real_x + jmi->n_real_u + jmi->n_real_w) + i])
#define _real_u_p(j,i) ((*(jmi->z))[jmi->offs_real_u_p + \
  j*(jmi->n_real_dx + jmi->n_real_x + jmi->n_real_u + jmi->n_real_w) + i])
#define _real_w_p(j,i) ((*(jmi->z))[jmi->offs_real_w_p + \
  j*(jmi->n_real_dx + jmi->n_real_x + jmi->n_real_u + jmi->n_real_w) + i])

#define _real_d(i) ((*(jmi->z))[jmi->offs_real_d+i])
#define _integer_d(i) ((*(jmi->z))[jmi->offs_integer_d+i])
#define _integer_u(i) ((*(jmi->z))[jmi->offs_integer_u+i])
#define _boolean_d(i) ((*(jmi->z))[jmi->offs_boolean_d+i])
#define _boolean_u(i) ((*(jmi->z))[jmi->offs_boolean_u+i])

#define _pre_real_dx(i) ((*(jmi->z))[jmi->offs_pre_real_dx+i])
#define _pre_real_x(i) ((*(jmi->z))[jmi->offs_pre_real_x+i])
#define _pre_real_u(i) ((*(jmi->z))[jmi->offs_pre_real_u+i])
#define _pre_real_w(i) ((*(jmi->z))[jmi->offs_pre_real_w+i])

#define _pre_real_d(i) ((*(jmi->z))[jmi->offs_pre_real_d+i])
#define _pre_integer_d(i) ((*(jmi->z))[jmi->offs_pre_integer_d+i])
#define _pre_integer_u(i) ((*(jmi->z))[jmi->offs_pre_integer_u+i])
#define _pre_boolean_d(i) ((*(jmi->z))[jmi->offs_pre_boolean_d+i])
#define _pre_boolean_u(i) ((*(jmi->z))[jmi->offs_pre_boolean_u+i])

#define _sw(i) ((*(jmi->z))[jmi->offs_sw + i])
#define _sw_init(i) ((*(jmi->z))[jmi->offs_sw_init + i])
#define _pre_sw(i) ((*(jmi->z))[jmi->offs_pre_sw + i])
#define _pre_sw_init(i) ((*(jmi->z))[jmi->offs_pre_sw_init + i])
#define _guards(i) ((*(jmi->z))[jmi->offs_guards + i])
#define _guards_init(i) ((*(jmi->z))[jmi->offs_guards_init + i])
#define _pre_guards(i) ((*(jmi->z))[jmi->offs_pre_guards + i])
#define _pre_guards_init(i) ((*(jmi->z))[jmi->offs_pre_guards_init + i])

#define _atInitial (jmi->atInitial)






















static int model_ode_guards(jmi_t* jmi) {
  
  return 0;
}

static int model_ode_next_time_event(jmi_t* jmi, jmi_real_t* nextTime) {
  jmi_real_t nextTimeEvent;
  jmi_real_t nextTimeEventTmp;
  jmi_real_t nSamp;
  nextTimeEvent = JMI_INF;
  *nextTime = nextTimeEvent;

  return 0;
}

static int model_ode_derivatives(jmi_t* jmi) {
  int ef = 0;
      model_ode_guards(jmi);
/************* ODE section *********/
  _der_x_3 = (  - ( _w_2 ) ) * ( _y_1 );
  _der_y_4 = ( _w_2 ) * ( _x_0 );
/************ Real outputs *********/
/****Integer and boolean outputs ***/
/**** Other variables ***/

  return ef;
}

static int model_ode_derivatives_dir_der(jmi_t* jmi) {
  int ef = 0;
  
  return ef;
}

static int model_ode_outputs(jmi_t* jmi) {
  int ef = 0;
  
  return ef;
}

static int model_ode_guards_init(jmi_t* jmi) {
  
  return 0;
}

static int model_ode_initialize(jmi_t* jmi) {
  int ef = 0;
      model_ode_guards(jmi);
  _y_1 = 1;
  _der_x_3 = (  - ( _w_2 ) ) * ( _y_1 );
  _x_0 = 0;
  _der_y_4 = ( _w_2 ) * ( _x_0 );

  return ef;
}


static int model_ode_initialize_dir_der(jmi_t* jmi) {
  int ef = 0;
  /* This function is not needed - no derivatives of the initialization system is exposed.*/
  return ef;
}


/*
 * The res argument is of type pointer to a vector. This means that
 * in the case of no AD, the type of res is double**. This is
 * necessary in order to accommodate the AD case, in which case
 * the type of res is vector< CppAD::AD<double> >. In C++, it would
 * be ok to pass such an object by reference, using the & operator:
 * 'vector< CppAD::AD<double> > &res'. However, since we would like
 * to be able to compile the code both with a C and a C++ compiler
 * this solution does not work. Probably not too bad, since we can use
 * macros.
 */
static int model_dae_F(jmi_t* jmi, jmi_ad_var_vec_p res) {

	return 0;
}

static int model_dae_dir_dF(jmi_t* jmi, jmi_ad_var_vec_p res, jmi_ad_var_vec_p dF, jmi_ad_var_vec_p dz) {

	return 0;
}

static int model_dae_R(jmi_t* jmi, jmi_ad_var_vec_p res) {

	return 0;
}

static int model_init_F0(jmi_t* jmi, jmi_ad_var_vec_p res) {

	return 0;
}

static int model_init_F1(jmi_t* jmi, jmi_ad_var_vec_p res) {
   (*res)[0] = 0 - _x_0;
   (*res)[1] = 1 - _y_1;

	return 0;
}

static int model_init_Fp(jmi_t* jmi, jmi_ad_var_vec_p res) {
  /* C_DAE_initial_dependent_parameter_residuals */
	return -1;
}

static int model_init_eval_parameters(jmi_t* jmi) {

        return 0;
}

static int model_init_R0(jmi_t* jmi, jmi_ad_var_vec_p res) {

	return 0;
}

static int model_opt_Ffdp(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_J(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_L(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_Ceq(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_Cineq(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_Heq(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

static int model_opt_Hineq(jmi_t* jmi, jmi_ad_var_vec_p res) {
	return -1;
}

int jmi_new(jmi_t** jmi) {

  jmi_init(jmi, N_real_ci, N_real_cd, N_real_pi, N_real_pd,
	   N_integer_ci, N_integer_cd, N_integer_pi, N_integer_pd,
	   N_boolean_ci, N_boolean_cd, N_boolean_pi, N_boolean_pd,
	   N_string_ci, N_string_cd, N_string_pi, N_string_pd,
	   N_real_dx,N_real_x, N_real_u, N_real_w,N_t_p,
	   N_real_d,N_integer_d,N_integer_u,N_boolean_d,N_boolean_u,
	   N_string_d,N_string_u, N_outputs,(int (*))Output_vrefs,
           N_sw,N_sw_init,N_guards,N_guards_init,
	   N_dae_blocks,N_dae_init_blocks,
	   Scaling_method, N_ext_objs);

  

  

  

  

	/* Initialize the DAE interface */
	jmi_dae_init(*jmi, *model_dae_F, N_eq_F, NULL, 0, NULL, NULL,
                     *model_dae_dir_dF,
        		     CAD_dae_n_nz,(int (*))CAD_dae_nz_rows,(int (*))CAD_dae_nz_cols,
        		     CAD_ODE_A_n_nz, (int (*))CAD_ODE_A_nz_rows, (int(*))CAD_ODE_A_nz_cols,
        		     CAD_ODE_B_n_nz, (int (*))CAD_ODE_B_nz_rows, (int(*))CAD_ODE_B_nz_cols,
        		     CAD_ODE_C_n_nz, (int (*))CAD_ODE_C_nz_rows, (int(*))CAD_ODE_C_nz_cols,
        		     CAD_ODE_D_n_nz, (int (*))CAD_ODE_D_nz_rows, (int(*))CAD_ODE_D_nz_cols,
		     *model_dae_R, N_eq_R, NULL, 0, NULL, NULL,*model_ode_derivatives,
		     	 *model_ode_derivatives_dir_der,
                     *model_ode_outputs,*model_ode_initialize,*model_ode_guards,
                     *model_ode_guards_init,*model_ode_next_time_event);

	/* Initialize the Init interface */
	jmi_init_init(*jmi, *model_init_F0, N_eq_F0, NULL,
		      0, NULL, NULL,
		      *model_init_F1, N_eq_F1, NULL,
		      0, NULL, NULL,
		      *model_init_Fp, N_eq_Fp, NULL,
		      0, NULL, NULL,
		      *model_init_eval_parameters,
		      *model_init_R0, N_eq_R0, NULL,
		      0, NULL, NULL);

	/* Initialize the Opt interface */
	jmi_opt_init(*jmi, *model_opt_Ffdp, N_eq_opt_Ffdp, NULL, 0, NULL, NULL,
		     *model_opt_J, N_eq_L, NULL, 0, NULL, NULL,
		     *model_opt_L, N_eq_L, NULL, 0, NULL, NULL,
		     *model_opt_Ceq, N_eq_Ceq, NULL, 0, NULL, NULL,
		     *model_opt_Cineq, N_eq_Cineq, NULL, 0, NULL, NULL,
		     *model_opt_Heq, N_eq_Heq, NULL, 0, NULL, NULL,
		     *model_opt_Hineq, N_eq_Hineq, NULL, 0, NULL, NULL);

	return 0;
}

int jmi_set_start_values(jmi_t* jmi) {
    _w_2 = (10);
    __enforce_bounds_5 = (JMI_FALSE);
    __events_default_tol_6 = (1.0E-10);
    __events_tol_factor_7 = (1.0E-4);
    __log_level_8 = (3);
    __nle_solver_check_jac_cond_9 = (JMI_FALSE);
    __nle_solver_default_tol_10 = (1.0E-10);
    __nle_solver_log_level_11 = (0);
    __nle_solver_min_tol_12 = (1.0E-12);
    __nle_solver_tol_factor_13 = (1.0E-4);
    __rescale_after_singular_jac_14 = (JMI_TRUE);
    __rescale_each_step_15 = (JMI_FALSE);
    __use_Brent_in_1d_16 = (JMI_FALSE);
    __use_automatic_scaling_17 = (JMI_TRUE);
    __use_jacobian_scaling_18 = (JMI_FALSE);
    model_init_eval_parameters(jmi);
    _x_0 = (0);
    _y_1 = (1);
    _der_x_3 = (0.0);
    _der_y_4 = (0.0);

    jmi_copy_z_to_zval(jmi);
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
/* FMI Functions*/

/* Inquire version numbers of header files */
DllExport const char* fmiGetModelTypesPlatform() {
    return fmi_get_model_types_platform();
}
DllExport const char* fmiGetVersion() {
    return fmi_get_version();
}

/* Creation and destruction of model instances and setting debug status */
DllExport fmiComponent fmiInstantiateModel(fmiString instanceName, fmiString GUID, fmiCallbackFunctions functions, fmiBoolean loggingOn) {

    return fmi_instantiate_model(instanceName, GUID, functions, loggingOn);
}

DllExport void fmiFreeModelInstance(fmiComponent c) {
    fmi_free_model_instance(c);
}

DllExport fmiStatus fmiSetDebugLogging(fmiComponent c, fmiBoolean loggingOn) {
    return fmi_set_debug_logging(c, loggingOn);
}


/* Providing independent variables and re-initialization of caching */
DllExport fmiStatus fmiSetTime(fmiComponent c, fmiReal fmitime) {
    return fmi_set_time(c, fmitime);
}

DllExport fmiStatus fmiSetContinuousStates(fmiComponent c, const fmiReal x[], size_t nx) {
    return fmi_set_continuous_states(c, x, nx);
}

DllExport fmiStatus fmiCompletedIntegratorStep(fmiComponent c, fmiBoolean* callEventUpdate) {
    return fmi_completed_integrator_step(c, callEventUpdate);
}

DllExport fmiStatus fmiSetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[]) {
    return fmi_set_real(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[]) {
    return fmi_set_integer(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[]) {
    return fmi_set_boolean(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[]) {
    return fmi_set_string(c, vr, nvr, value);
}


/* Evaluation of the model equations */
DllExport fmiStatus fmiInitialize(fmiComponent c, fmiBoolean toleranceControlled, fmiReal relativeTolerance, fmiEventInfo* eventInfo) {
    return fmi_initialize(c, toleranceControlled, relativeTolerance, eventInfo);
}

DllExport fmiStatus fmiGetDerivatives(fmiComponent c, fmiReal derivatives[] , size_t nx) {
    return fmi_get_derivatives(c, derivatives, nx);
}

DllExport fmiStatus fmiGetEventIndicators(fmiComponent c, fmiReal eventIndicators[], size_t ni) {
    return fmi_get_event_indicators(c, eventIndicators, ni);
}

DllExport fmiStatus fmiGetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[]) {
    return fmi_get_real(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[]) {
    return fmi_get_integer(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[]) {
    return fmi_get_boolean(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[]) {
    return fmi_get_string(c, vr, nvr, value);
}

DllExport jmi_t* fmiGetJMI(fmiComponent c) {
    return fmi_get_jmi_t(c);
}

DllExport fmiStatus fmiEventUpdate(fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo) {
    return fmi_event_update(c, intermediateResults, eventInfo);
}

DllExport fmiStatus fmiGetContinuousStates(fmiComponent c, fmiReal states[], size_t nx) {
    return fmi_get_continuous_states(c, states, nx);
}

DllExport fmiStatus fmiGetNominalContinuousStates(fmiComponent c, fmiReal x_nominal[], size_t nx) {
    return fmi_get_nominal_continuous_states(c, x_nominal, nx);
}

DllExport fmiStatus fmiGetStateValueReferences(fmiComponent c, fmiValueReference vrx[], size_t nx) {
    return fmi_get_state_value_references(c, vrx, nx);
}

DllExport fmiStatus fmiTerminate(fmiComponent c) {
    return fmi_terminate(c);
}

DllExport fmiStatus fmiExtractDebugInfo(fmiComponent c) {
    return fmi_extract_debug_info(c);
}

/* Experimental Jacobian interface */
DllExport fmiStatus fmiGetJacobian(fmiComponent c, int independents, int dependents, fmiReal jac[], size_t njac) {
	return fmi_get_jacobian(c, independents, dependents, jac, njac);
}

DllExport fmiStatus fmiGetDirectionalDerivative(fmiComponent c, const fmiValueReference z_vref[], size_t nzvr, const fmiValueReference v_vref[], size_t nvvr, fmiReal dz[], const fmiReal dv[]) {
	return fmi_get_directional_derivative(c, z_vref, nzvr, v_vref, nvvr, dz, dv);

}

DllExport fmiStatus fmiGetPartialDerivatives(fmiComponent c, fmiStatus (*setMatrixElement)(void* data, fmiInteger row, fmiInteger col, fmiReal value), void* A, void* B, void* C, void* D) {
	return fmi_get_partial_derivatives(c, setMatrixElement, A, B, C, D);
}


#ifdef __cplusplus
}
#endif
