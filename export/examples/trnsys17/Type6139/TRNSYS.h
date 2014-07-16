// ---------------------------------------------------------------------------------------------------------------
// TRNSYS.h: C++ Header file for TRNSYS 16
// This file declares all the global functions available to C / C++ TRNSYS Types
// ---------------------------------------------------------------------------------------------------------------

/// \file TRNSYS.h

// --- Kernel subroutines ----------------------------------------------------------------------------------------

// Aliases according to documentation are defined below.

extern "C" __declspec(dllimport) void	__cdecl ADDEXTERNALFILE( int*, char* );
extern "C" __declspec(dllimport) void	__cdecl ASHRAE_COEFS( int*, int*, int*, int*, int*, double[], double[], double[] );
extern "C" __declspec(dllimport) void	__cdecl CALLPROGRAM( char[], bool*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl ENCLOSURE_17( double[], double[], double[], int*, int* );
extern "C" __declspec(dllimport) void	__cdecl FLUID_PROPERTIES( char[], double[], int*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl FOUNDBADINPUT( int*, char*, char* );
extern "C" __declspec(dllimport) void	__cdecl FOUNDBADPARAMETER( int*, char*, char* );
extern "C" __declspec(dllimport) void	__cdecl GETINCIDENTRADIATION( double*, int*, double[], double*, double*, double*, int*, int*, double*, double*, double*, int*, double*, double*, double*, double[], int* );
// GETLABEL2: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl GETLABEL2( char*, int*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl GETSTORAGEVARS( double[], int*, int[] );
extern "C" __declspec(dllimport) void	__cdecl INITTRNSYS( int* );
extern "C" __declspec(dllimport) void	__cdecl INTERPOLATEDATA( int*, int*, int[], int*, double[], double[] );
extern "C" __declspec(dllimport) void	__cdecl INVERTMATRIX( int*, int*, double**, int* );
extern "C" __declspec(dllimport) void	__cdecl LINEARREGRESSION( int*, int*, int*, int*, double**, double[], double[], int* );
extern "C" __declspec(dllimport) void	__cdecl LINKCK( char*, char*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl MESSAGES( int*, char*, char*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl MOISTAIRPROPERTIES( int*, int*, int*, int*, int*, double[], int*, int* );
//extern "C" __declspec(dllimport) void	__cdecl PARREAD( ... );
extern "C" __declspec(dllimport) void	__cdecl RCHECK( int[], char*, char* );
extern "C" __declspec(dllimport) void	__cdecl READNEXTCHAR( int* );
extern "C" __declspec(dllimport) void	__cdecl RESETERRORS( void );
extern "C" __declspec(dllimport) void	__cdecl REWIND( int*, int*, int*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl SETCURRENTUNIT( int* );
extern "C" __declspec(dllimport) void	__cdecl SETDESIREDDISCRETECONTROLSTATE( int*, int* );
extern "C" __declspec(dllimport) void	__cdecl SETDYNAMICARRAYINITIALVALUE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETDYNAMICARRAYVALUETHISITERATION( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETINPUTUNITS( int*, char* );
extern "C" __declspec(dllimport) void	__cdecl SETINPUTVALUE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETITERATIONMODE( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBEROFDERIVATIVES( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBEROFDISCRETECONTROLS( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBEROFINPUTS( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBEROFOUTPUTS( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBEROFPARAMETERS( int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMBERSTOREDVARIABLES( int*, int* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMERICALDERIVATIVE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETNUMERICALSOLUTION( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETOUTPUTUNITS( int*, char* );
extern "C" __declspec(dllimport) void	__cdecl SETOUTPUTVALUE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETPARAMETERVALUE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETPREVIOUSCONTROLSTATE( int*, int* );
extern "C" __declspec(dllimport) void	__cdecl SETSIMULATIONSTARTTIME( double* );
extern "C" __declspec(dllimport) void	__cdecl SETSIMULATIONSTOPTIME( double* );
extern "C" __declspec(dllimport) void	__cdecl SETSIMULATIONTIME( double* );
extern "C" __declspec(dllimport) void	__cdecl SETSIMULATIONTIMESTEP( double* );
extern "C" __declspec(dllimport) void	__cdecl SETSTATICARRAYVALUE( int*, double* );
extern "C" __declspec(dllimport) void	__cdecl SETSTORAGESIZE( int*, int* );
extern "C" __declspec(dllimport) void	__cdecl SETSTORAGEVARS( double[], int*, int[] );
extern "C" __declspec(dllimport) void	__cdecl SETTIMESTEPITERATION( int* );
extern "C" __declspec(dllimport) void	__cdecl SETTYPEVERSION( int* );
// extern "C" __declspec(dllimport) void	__cdecl SOLARCELLPERFORMANCE( ... );
extern "C" __declspec(dllimport) void	__cdecl SOLVEDIFFEQ( double*, double*, double*, double*, double* );
extern "C" __declspec(dllimport) void	__cdecl STEAM_PROPERTIES( char*, double[], int*, int* );
extern "C" __declspec(dllimport) double	__cdecl TAU_ALPHA( int*, double*, double*, double*, double*, double* );
extern "C" __declspec(dllimport) void	__cdecl TYPECK( int*, int[], int*, int*, int* );
extern "C" __declspec(dllimport) double	__cdecl VIEW_FACTORS( int*, double*, double*, double*, double*, double*, double*, double*, double*, double*, double*, double*, int*, int* );
extern "C" __declspec(dllimport) void	__cdecl WRITETOLIST( char* );


// --- Functions to access TRNSYS global variables ---------------------------------------------------------------

// Function names are decorated with the Fortran module name. Aliases according to documentation are defined below.

extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_CHECKSTABILITY( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_CLOSEFILEIVF( int* );
extern "C" __declspec(dllimport) bool	__cdecl TRNSYSFUNCTIONS_mp_ERRORFOUND( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDOUTPUTNUMBER( int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDOUTPUTNUMBERS1( int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDTYPENUMBER( int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDTYPENUMBERS1( int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDUNITNUMBER( int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDUNITNUMBERS1( int*, int* );
// GETCONNECTEDVARIABLETYPE: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLETYPE( char*, int*, int*, int* );
// GETCONNECTEDVARIABLETYPES1: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLETYPES1( char*, int*, int*, int* );
// GETCONNECTEDVARIABLEUNIT
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLEUNIT( char*, int*, int*, int* );
// GETCONNECTEDVARIABLEUNITS1: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLEUNITS1( char*, int*, int*, int* );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETCONVERGENCETOLERANCE( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCURRENTTYPE( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETCURRENTUNIT( void );
// GETDECKFILENAME: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETDECKFILENAME( char*, int* );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETDYNAMICARRAYVALUELASTTIMESTEP( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETEXTFILEPATH( int*, int*, char* );
// GETFORMAT: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETFORMAT( char*, int*, int*, int* );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETINPUTVALUE( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISCONVERGENCEREACHED( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISENDOFTIMESTEP( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISFIRSTCALLOFSIMULATION( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISFIRSTTIMESTEP( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISLASTCALLOFSIMULATION( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISREREADPARAMETERS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISSTARTTIME( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETISVERSIONSIGNINGTIME( void );
// GETLABEL: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETLABEL( char*, int*, int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETLISTINGFILELOGICALUNIT( void );
// GETLUFILENAME: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETLUFILENAME( char*, int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETMAXDESCRIPLENGTH( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETMAXFILEWIDTH( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETMAXLABELLENGTH( void); 
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETMAXPATHLENGTH( void );
extern "C" __declspec(dllimport) double	__cdecl TRNSYSFUNCTIONS_mp_GETMINIMUMTIMESTEP( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETMINORVERSIONNUMBER( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNEXTAVAILABLELOGICALUNIT( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNFILES( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNMAXITERATIONS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNMAXSTORAGESPOTS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNMAXWARNINGS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNTIMESTEPS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFDERIVATIVES( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFERRORS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFINPUTS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFOUTPUTS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFPARAMETERS( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMBEROFWARNINGS( void );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETNUMERICALDERIVATIVE( int* );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETNUMERICALSOLUTION( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETNUMERICALSOLVER( void );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETOUTPUTVALUE( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETOVERALLITERATION( int* );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETPARAMETERVALUE( int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETPREVIOUSCONTROLSTATE( int* );
extern "C" __declspec(dllimport) double	__cdecl TRNSYSFUNCTIONS_mp_GETSIMULATIONSTARTTIME( void );
extern "C" __declspec(dllimport) double	__cdecl TRNSYSFUNCTIONS_mp_GETSIMULATIONSTARTTIMEV15( void );
extern "C" __declspec(dllimport) double	__cdecl TRNSYSFUNCTIONS_mp_GETSIMULATIONSTOPTIME( void );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETSIMULATIONTIME( void );
extern "C" __declspec(dllimport) double	__cdecl TRNSYSFUNCTIONS_mp_GETSIMULATIONTIMESTEP( void );
extern "C" __declspec(dllimport) double __cdecl TRNSYSFUNCTIONS_mp_GETSTATICARRAYVALUE( int* );
extern "C" __declspec(dllimport) bool	__cdecl TRNSYSFUNCTIONS_mp_GETTIMEREPORT( void );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETTIMESTEPITERATION( int* );
// GETTRNSYSDEBUGLIBDIR: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETTRNSYSDEBUGLIBDIR( char*, int* );
// GETTRNSYSEXEDIR: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETTRNSYSEXEDIR( char*, int* );
// GETTRNSYSINPUTFILEDIR: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETTRNSYSINPUTFILEDIR( char*, int* );
// GETTRNSYSROOTDIR: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETTRNSYSROOTDIR( char*, int* );
// GETTRNSYSUSERLIBDIR: C routine needs two initial extra arguments (pointer to result string & length of string).
extern "C" __declspec(dllimport) char*	__cdecl TRNSYSFUNCTIONS_mp_GETTRNSYSUSERLIBDIR( char*, int* );
extern "C" __declspec(dllimport) bool	__cdecl TRNSYSFUNCTIONS_mp_GETTYPEVERSION( void );
// GETVARIABLEDESCRIPTION: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETVARIABLEDESCRIPTION( char*, int*, int*, int* );
// GETVARIABLEUNITS: C routine needs two initial extra arguments (pointer to result string & length of string), remaining arguments acoording to TRNSYS documentation.
extern "C" __declspec(dllimport) void	__cdecl TRNSYSFUNCTIONS_mp_GETVARIABLEUNITS( char*, int*, int*, int* );
extern "C" __declspec(dllimport) int	__cdecl TRNSYSFUNCTIONS_mp_GETVERSIONNUMBER( void );
extern "C" __declspec(dllimport) bool	__cdecl TRNSYSFUNCTIONS_mp_LOGICALUNITISOPEN( int* );


// --- Aliases according to documentation for functions included in module TrnsysFunctions + kernel routines ----

#define addExternalFile ADDEXTERNALFILE
#define ASHRAE_Coefs ASHRAE_COEFS
#define CallProgram CALLPROGRAM
#define Enclosure_17 ENCLOSURE_17
#define Fluid_Properties FLUID_PROPERTIES
#define foundBadInput FOUNDBADINPUT
#define foundBadParameter FOUNDBADPARAMETER
#define getIncidentRadiation GETINCIDENTRADIATION
#define getLabel2 GETLABEL2
#define getStorageVars GETSTORAGEVARS
#define initTrnsys INITTRNSYS
#define InterpolateData INTERPOLATEDATA
#define InvertMatrix INVERTMATRIX
#define LinearRegression LINEARREGRESSION
#define LINKCK LINKCK
#define Messages MESSAGES
#define MoisAirProperties MOISTAIRPROPERTIES
//#define ParRead PARREAD
#define Rcheck RCHECK
#define readNextChar READNEXTCHAR
#define resetErrors RESETERRORS
#define rewind REWIND
#define setCurentUnit SETCURRENTUNIT
#define setDesiredDiscreteControlState SETDESIREDDISCRETECONTROLSTATE
#define setDynamicArrayInitialValue SETDYNAMICARRAYINITIALVALUE
#define setDynamicArrayValueThisIteration SETDYNAMICARRAYVALUETHISITERATION
#define setInputUnits SETINPUTUNITS
#define setInputValue SETINPUTVALUE
#define setIterationMode SETITERATIONMODE
#define setNumberOfDerivatives SETNUMBEROFDERIVATIVES
#define setNumberOfDiscreteControls SETNUMBEROFDISCRETECONTROLS
#define setNumberOfInputs SETNUMBEROFINPUTS
#define setNumberOfOutputs SETNUMBEROFOUTPUTS
#define setNumberOfParameters SETNUMBEROFPARAMETERS
#define setNumberStoredVariables SETNUMBERSTOREDVARIABLES
#define setNumericalDerivative SETNUMERICALDERIVATIVE
#define setNumericalSolution SETNUMERICALSOLUTION
#define setOutputUnits SETOUTPUTUNITS
#define setOutputValue SETOUTPUTVALUE
#define setParameterValue SETPARAMETERVALUE
#define setPreviousControlState SETPREVIOUSCONTROLSTATE
#define setSimulationStartTime SETSIMULATIONSTARTTIME
#define setSimulationStopTime SETSIMULATIONSTOPTIME
#define setSimulationTime SETSIMULATIONTIME
#define setSimulationTimeStep SETSIMULATIONTIMESTEP
#define setStaticArrayValue SETSTATICARRAYVALUE
#define setStorageSize SETSTORAGESIZE
#define setStorageVars SETSTORAGEVARS
#define setTimestepIteration SETTIMESTEPITERATION
#define setTypeVersion SETTYPEVERSION
//#define SolarCellPerformance SOLARCELLPERFORMANCE
#define SolveDiffEq SOLVEDIFFEQ
#define Steam_Properties STEAM_PROPERTIES
#define Tau_Alpha TAU_ALPHA
#define TYPECK TYPECK
#define View_Factors VIEW_FACTORS
#define writeToList WRITETOLIST

#define CheckStability TRNSYSFUNCTIONS_mp_CHECKSTABILITY
#define closeFileIVF TRNSYSFUNCTIONS_mp_CLOSEFILEIVF
#define ErrorFound TRNSYSFUNCTIONS_mp_ERRORFOUND
#define getConnectedOutputNumber TRNSYSFUNCTIONS_mp_GETCONNECTEDOUTPUTNUMBER
#define getConnectedOutputNumberS1 TRNSYSFUNCTIONS_mp_GETCONNECTEDOUTPUTNUMBERS1
#define getConnectedTypeNumber TRNSYSFUNCTIONS_mp_GETCONNECTEDTYPENUMBER
#define getConnectedTypeNumberS1 TRNSYSFUNCTIONS_mp_GETCONNECTEDTYPENUMBERS1
#define getConnectedUnitNumber TRNSYSFUNCTIONS_mp_GETCONNECTEDUNITNUMBER
#define getConnectedUnitNumberS1 TRNSYSFUNCTIONS_mp_GETCONNECTEDUNITNUMBERS1
#define getConnectedVariableType TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLETYPE
#define getConnectedVariableTypeS1 TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLETYPES1
#define getConnectedVariableUnit TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLEUNIT
#define getConnectedVariableUnitS1 TRNSYSFUNCTIONS_mp_GETCONNECTEDVARIABLEUNITS1
#define getConvergenceTolerance TRNSYSFUNCTIONS_mp_GETCONVERGENCETOLERANCE
#define getCurrentType TRNSYSFUNCTIONS_mp_GETCURRENTTYPE
#define getCurrentUnit TRNSYSFUNCTIONS_mp_GETCURRENTUNIT
#define getDeckFileName TRNSYSFUNCTIONS_mp_GETDECKFILENAME
#define getDynamicArrayValueLastTimestep TRNSYSFUNCTIONS_mp_GETDYNAMICARRAYVALUELASTTIMESTEP
#define getExtFilePath TRNSYSFUNCTIONS_mp_GETEXTFILEPATH
#define getFormat TRNSYSFUNCTIONS_mp_GETFORMAT
#define getInputValue TRNSYSFUNCTIONS_mp_GETINPUTVALUE
#define getIsConvergenceReached TRNSYSFUNCTIONS_mp_GETISCONVERGENCEREACHED
#define getIsEndOfTimestep TRNSYSFUNCTIONS_mp_GETISENDOFTIMESTEP
#define getIsFirstCallOfSimulation TRNSYSFUNCTIONS_mp_GETISFIRSTCALLOFSIMULATION
#define getIsFirstTimestep TRNSYSFUNCTIONS_mp_GETISFIRSTTIMESTEP
#define getIsLastCallOfSimulation TRNSYSFUNCTIONS_mp_GETISLASTCALLOFSIMULATION
#define getIsReReadParameters TRNSYSFUNCTIONS_mp_GETISREREADPARAMETERS
#define getIsStartTime TRNSYSFUNCTIONS_mp_GETISSTARTTIME
#define getIsVersionSigningTime TRNSYSFUNCTIONS_mp_GETISVERSIONSIGNINGTIME
#define getLabel TRNSYSFUNCTIONS_mp_GETLABEL
#define getListingFileLogicalUnit TRNSYSFUNCTIONS_mp_GETLISTINGFILELOGICALUNIT
#define getLUFileName TRNSYSFUNCTIONS_mp_GETLUFILENAME
#define getMaxDescripLength TRNSYSFUNCTIONS_mp_GETMAXDESCRIPLENGTH
#define getmaxFileWidth TRNSYSFUNCTIONS_mp_GETMAXFILEWIDTH
#define getMaxLabelLength TRNSYSFUNCTIONS_mp_GETMAXLABELLENGTH 
#define getMaxPathLength TRNSYSFUNCTIONS_mp_GETMAXPATHLENGTH
#define getMinimumTimestep TRNSYSFUNCTIONS_mp_GETMINIMUMTIMESTEP
#define getMinorVersionNumber TRNSYSFUNCTIONS_mp_GETMINORVERSIONNUMBER
#define getNextAvailableLogicalUnit TRNSYSFUNCTIONS_mp_GETNEXTAVAILABLELOGICALUNIT
#define getNFiles TRNSYSFUNCTIONS_mp_GETNFILES
#define getnMaxIterations TRNSYSFUNCTIONS_mp_GETNMAXITERATIONS
#define getnMaxStorageSpots TRNSYSFUNCTIONS_mp_GETNMAXSTORAGESPOTS
#define getnMaxWarnings TRNSYSFUNCTIONS_mp_GETNMAXWARNINGS
#define getnTimeSteps TRNSYSFUNCTIONS_mp_GETNTIMESTEPS
#define getNumberOfDerivatives TRNSYSFUNCTIONS_mp_GETNUMBEROFDERIVATIVES
#define getNumberOfErros TRNSYSFUNCTIONS_mp_GETNUMBEROFERRORS
#define getNumberOfInputs TRNSYSFUNCTIONS_mp_GETNUMBEROFINPUTS
#define getNumberOfOutputs TRNSYSFUNCTIONS_mp_GETNUMBEROFOUTPUTS
#define getNumberOfParameters TRNSYSFUNCTIONS_mp_GETNUMBEROFPARAMETERS
#define getNumberOfWarnings TRNSYSFUNCTIONS_mp_GETNUMBEROFWARNINGS
#define getNumericalDerivative TRNSYSFUNCTIONS_mp_GETNUMERICALDERIVATIVE
#define getNumericalSolution TRNSYSFUNCTIONS_mp_GETNUMERICALSOLUTION
#define getNumericalSolver TRNSYSFUNCTIONS_mp_GETNUMERICALSOLVER
#define getOutputValue TRNSYSFUNCTIONS_mp_GETOUTPUTVALUE
#define getOverallIteration TRNSYSFUNCTIONS_mp_GETOVERALLITERATION
#define getParameterValue TRNSYSFUNCTIONS_mp_GETPARAMETERVALUE
#define getPreviousControlState TRNSYSFUNCTIONS_mp_GETPREVIOUSCONTROLSTATE
#define getSimulationStartTime TRNSYSFUNCTIONS_mp_GETSIMULATIONSTARTTIME
#define getSimulationStartTimeV15 TRNSYSFUNCTIONS_mp_GETSIMULATIONSTARTTIMEV15
#define getSimulationStopTime TRNSYSFUNCTIONS_mp_GETSIMULATIONSTOPTIME
#define getSimulationTime TRNSYSFUNCTIONS_mp_GETSIMULATIONTIME
#define getSimulationTimeStep TRNSYSFUNCTIONS_mp_GETSIMULATIONTIMESTEP
#define getStaticArrayValue TRNSYSFUNCTIONS_mp_GETSTATICARRAYVALUE
#define getTimestepIteration TRNSYSFUNCTIONS_mp_GETTIMESTEPITERATION
#define getTrnsysDebugLibDir TRNSYSFUNCTIONS_mp_GETTRNSYSDEBUGLIBDIR
#define getTrnsysReleaseLibDir TRNSYSFUNCTIONS_mp_GETTRNSYSEXEDIR
#define getTrnsysInputFileDir TRNSYSFUNCTIONS_mp_GETTRNSYSINPUTFILEDIR
#define getTrnsysRootDir TRNSYSFUNCTIONS_mp_GETTRNSYSROOTDIR
#define getTrnsysUserLibDir TRNSYSFUNCTIONS_mp_GETTRNSYSUSERLIBDIR
#define getTypeVersion TRNSYSFUNCTIONS_mp_GETTYPEVERSION
#define getVariableDescription TRNSYSFUNCTIONS_mp_GETVARIABLEDESCRIPTION
#define getVariableUnit TRNSYSFUNCTIONS_mp_GETVARIABLEUNITS
#define getVersionNumber TRNSYSFUNCTIONS_mp_GETVERSIONNUMBER
#define LogicalUnitIsOpen TRNSYSFUNCTIONS_mp_LOGICALUNITISOPEN
