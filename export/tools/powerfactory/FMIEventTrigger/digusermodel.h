/**
 * \file digusermodel.h
 * External C-Interface for dynamic models for PowerFactory (function prototypes).
 */
 
#ifndef _DIGUSERMODEL_HPP
#define _DIGUSERMODEL_HPP

// message types for printing to PF output window
#define MSG_NONE   0x0000
#define MSG_ERR    0x0001
#define MSG_WRNG   0x0002
#define MSG_INFO   0x0004
#define MSG_PCL    0x0010

extern const char* get_working_path();
extern const char* get_temp_path();
extern void print_pf(const char* msg, unsigned int msgType);
extern void stop_simulation();

typedef struct {
	const char* m_name;
	const char* m_unit;
	const char* m_desc;
} SignalDescription;

typedef struct {
	const char* m_name;
	const char* m_unit;
	const char* m_desc;
	const double m_initVal; // initial value
	const double m_minVal;  // min. value
	const double m_maxVal;  // max. value
} ParameterDescription;

typedef struct {
	const char* m_name;
	const char* m_unitx;  // x-unit
	const char* m_unity;  // y-unit
	const char* m_desc;
	const short m_matobj;    // 0 = values stored in DSL,  1 = stored in IntMat
} LookupDescription;

typedef struct {
	const char* m_description;
	const char* m_version;
	const char* m_created;
	const char* m_author;
	const char* m_company;
	const char* m_copyright;
	const char m_checksum[20];
} ModelInfo;

struct ModelDefinition {
	const ModelInfo* const m_info; // model information structure
	const int m_numInput;          // no. of input signals
	const int m_numOutput;         // no. of output signals
	const int m_numState;          // no. of state variables
	const int m_numLimState;       // no. of limited state variables (DSL_limstate)
	const int m_numParameter;      // no. of parameter
	const int m_numInternalParam;  // no. of parameter
	const int m_numInternalSig;    // no. of internal signals
	const int m_numDisState;       // no. of discrete states
	const int m_numDelay;          // no. of delays (DSL_delay)
	const int m_numPicDrop;        // no. of picdrop (DSL_picdro)
	const int m_numSelect;         // no. of selects (DSL_select)
	const int m_numSelfix;         // no. of selfixes (DSL_selfix)
	const int m_numLimit;          // no. of limits (DSL_lim)
	const int m_numFlipFlop;       // no. of flipflops (DSL_flipflop and DSL_aflipflop)
	const int m_numLookupArray;    // no. of look-up arrays
	const int m_numLookupTable;    // no. of look-up tables
	const int m_solver;            // solver type (not supported)");
	const int m_interface;         // interface type (not supported)");
	const int m_prediction;        // prediction type (not supported)");
	const double m_integrationstep;// user-defined integration step (not supported)");
	const SignalDescription* const m_inputSigInfo;
	const SignalDescription* const m_outputSigInfo;
	const SignalDescription* const m_stateSigInfo;
	const SignalDescription* const m_limitStateSigInfo;
	const SignalDescription* const m_internalSigInfo;
	const ParameterDescription* const m_paramInfo;
	const ParameterDescription* const m_internalParaInfo;
	const LookupDescription* const m_lookupArrayInfo;
	const LookupDescription* const m_lookupTableInfo;
};

typedef struct {
	double* m_value;
} InternalSignal;

typedef struct {
	short* m_init;       // 0 = not initialised, 1 = initialised
	short m_isconnected; // 0 = not connected,   1 = connected
	short m_issrcinit;   // 0 = connected object is not initialised,  1 = connected object is initalised
	short m_propinc;     //  0 = signal will not be initialised
	                     //  1 = signal will be only initialised if signal (input or output) is not connected
	                     //  2 = signal will be initialised
	                     //  3 = incfix for autoinc
} InitInfo;

typedef struct {
	double** m_valadr;
	InitInfo m_initInfo;
} InputSignal;

typedef struct {
	double* m_value;
	InitInfo m_initInfo;
} OutputSignal;

#define GETIN(pINS,I) (*(*(pINS->m_inputSigs[I].m_valadr)))
#define SETIN(pINS,I,VAL) *(*(pINS->m_inputSigs[I].m_valadr)) = VAL

typedef struct {
	double* m_value;   // state signal (xs)
	double* m_fsval;   // derivative (fsval)
} StateSignal;

typedef struct {
	short m_state;
	int   m_control;
} DiscreteState;

typedef struct {
	double* m_value;
	double  m_newval;
} DelaySignal;

typedef struct {
	double* m_value;   // state signal (xs)
	double* m_fsval;   // derivative (fsval)
	double* m_dsigmin;
	double* m_dsigmax;
	double* m_dsigder;
	double* m_dsiglow;
	double* m_dsigup;
	double* m_dsigdlow;
	double* m_dsigdup;
	short m_state;
	int   m_control;
} LimStateSignal;

typedef struct {
	double* m_din;     // in (first argument)
	double* m_dTpick;  // Tpick (second argument)
	double* m_dTdrop;  // Tdrop (third argument)
	double m_dtset;    // internal (time --> state wait
	short m_state;
	int   m_control;
} PicDropExpression;

typedef struct {
	double* m_din;   // in (first argument)
	short m_state;
	int   m_control;
} SelectExpression;

typedef struct {
	short m_state;
} SelfixExpression;

typedef struct {
	double* m_din;   // in (first argument)
	double* m_dmin;  // dmin (second argument)
	double* m_dmax;  // dmax (third argument)
	short m_state;
	int   m_control;
} LimitExpression;

typedef struct {
	int m_inc_done;   // init flag -1: faulty, 0: not done, 1: successfully done
	double m_xa;      // output
	int m_x1;         // state: 1 or 0
} FlipFlopExpression;

typedef struct {
	double* m_xval;  // x value
	double* m_yval;  // y value
  int m_nvals;       // no. of values
	void* m_interpolation; // internal
} LookupArray;

typedef struct {
	double** m_val;      // m_val[irow][icol]
	double* m_valblock;
	int m_ncols; // no. of columns (y-values)
	int m_nrows; // no. of rows (x-values)
	void* m_interpolation; // internal
} LookupTable;

typedef struct {
	InputSignal*         m_inputSigs;      // input signals
	OutputSignal*        m_outputSigs;     // output signals
	InternalSignal*      m_internalSigs;   // internal signals
	StateSignal*         m_stateSigs;      // state variable/derivatives
	LimStateSignal*      m_limStateSigs;   // limited state signal/derivatives
	DiscreteState*       m_disStates;      // discrete states
	DelaySignal*         m_delaySigs;      // delay signal
	PicDropExpression*   m_picDropExpr;    // picdrop expression
	SelectExpression*    m_selectExpr;     // select expression
	SelfixExpression*    m_selfixExpr;     // selfix expression
	LimitExpression*     m_limitExpr;      // limit expression
	FlipFlopExpression*  m_flipFlopExpr;   // flipflop expression
	double*              m_parameters;     // for parameter
	double*              m_internalParams; // internal parameter 
	LookupArray*         m_lookupArrays;   // look-up arrays
	LookupTable*         m_lookupTables;   // look-up tables
	void*                m_userData;       // user data
	void*                m_pfModel;        // reference to internal PowerFactory Model
} ModelInstance;

// extern int set_delay_time(ModelInstance* pInstance,int index,double time);
// extern void init_delay(ModelInstance* pInstance, int index, double din);
// extern double eval_delay(ModelInstance* pInstance, int index, double din);

// extern void init_input(ModelInstance* pInstance,int index, double value);
extern void init_output(ModelInstance* pInstance,int index, double value);

// extern void init_limstate(ModelInstance* pInstance, int index, double din, double dmin, double dmax);
// extern double eval_limstate(ModelInstance* pInstance, int index, double dmin, double dmax);
// extern void upd_limstate(ModelInstance* pInstance, int index, double dval);

// extern void init_picdro(ModelInstance* pInstance, int index, double din, double dTpick, double dTdrop);
// extern double eval_picdro(ModelInstance* pInstance, int index, double din, double dTpick, double dTdrop);

// extern void init_select(ModelInstance* pInstance, int index, double din);
// extern double eval_select(ModelInstance* pInstance, int index, double din, double don, double doff);

// extern void init_selfix(ModelInstance* pInstance, int index, double din);
// extern double eval_selfix(ModelInstance* pInstance, int index, double din, double don, double doff);

// extern void init_lim(ModelInstance* pInstance, int index, double din, double dmin, double dmax);
// extern double eval_lim(ModelInstance* pInstance, int index, double din, double dmin, double dmax);

// // flipflop(boolset,boolreset)
extern void init_flipflop(ModelInstance* pInstance, int index, double dset, double dreset);
extern double eval_flipflop(ModelInstance* pInstance, int index, double dset, double dreset);

// // aflipflop(x,boolset,boolreset)
// extern void init_aflipflop(ModelInstance* pInstance, int index, double din, double dset, double dreset);
// extern double eval_aflipflop(ModelInstance* pInstance, int index, double din, double dset, double dreset);

// extern void print_model_link(ModelInstance* pInstance);
// extern void emit_fault(ModelInstance* pInstance, const char* evtname, const char* evtstr, int trigcount);
// extern void emit_fault_target(ModelInstance* pInstance, const char* targetslot, const char* evtname, const char* evtstr, int trigcount);
// extern void emit_fault_create(ModelInstance* pInstance, const char* eventtype, const char* targetslot, const char* evtname, const char* evtstr, int trigcount);
// extern void emit_event(ModelInstance* pInstance, const char* evtname, double tdelay, const char* evtstr, int trigcount);
// extern void emit_event_target(ModelInstance* pInstance, const char* targetslot, const char* evtname, double tdelay, const char* evtstr, int trigcount);
// extern void emit_event_create(ModelInstance* pInstance, const char* eventtype, const char* targetslot, const char* evtname, double tdelay, const char* evtstr, int trigcount);

// // look-up array functions (x,y vector)
// extern double eval_lapprox(ModelInstance* pInstance, int index, double x);
// extern double eval_lapproxext(ModelInstance* pInstance, int index, double x);

// extern double eval_invlapprox(ModelInstance* pInstance, int index, double x);
// extern double eval_invlapproxext(ModelInstance* pInstance, int index, double x);

// extern double eval_sapprox(ModelInstance* pInstance, int index, double x);
// extern double eval_invsapprox(ModelInstance* pInstance, int index, double x);

// // look-up table function (matrix)
// extern double eval_lapprox2(ModelInstance* pInstance, int index, double x, double y);
// extern double eval_sapprox2(ModelInstance* pInstance, int index, double x, double y);

// extern double dsl_modulo(double arg1, double arg2);
// extern double min3(double arg1, double arg2, double arg3);
// extern double min4(double arg1, double arg2, double arg3, double arg4);
// extern double min5(double arg1, double arg2, double arg3, double arg4, double arg5);
// extern double max3(double arg1, double arg2, double arg3);
// extern double max4(double arg1, double arg2, double arg3, double arg4);
// extern double max5(double arg1, double arg2, double arg3, double arg4, double arg5);

#endif
