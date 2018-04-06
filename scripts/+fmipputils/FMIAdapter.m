% -------------------------------------------------------------------
% Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
% All rights reserved. See file FMIPP_LICENSE for details.
% -------------------------------------------------------------------

classdef (Abstract) FMIAdapter < handle

	methods (Abstract)

		% Initialize the FMU (definition of input/output variables and parameters, enforce step size).
		init( obj, currentCommunicationPoint )

		% Make a simulation step.
		doStep( obj, currentCommunicationPoint, communicationStepSize )

	end % methods (Abstract)


	properties (SetAccess = private)

		% This flag indicates whether the FMI export interface is activated.
		fmippexActive_

		% FMI component backend.
		backend_

		% Flag to indicate that a fixed step size is to be enforced.
		enforceTimeStep_

		% Enforced step size.
		enforcedTimeStepSize_

		% Real parameters.
		realParameters_
		realParameterSize_
		realParameterNames_
		debugRealParameterValues_

		% Real input variables.
		realInputs_
		realInputSize_
		realInputNames_
		debugRealInputValues_

		% Real output variables.
		realOutputs_
		realOutputSize_
		realOutputNames_
		debugRealOutputValues_

		% Integer parameters.
		integerParameters_
		integerParameterSize_
		integerParameterNames_
		debugIntegerParameterValues_

		% Integer input variables.
		integerInputs_
		integerInputSize_
		integerInputNames_
		debugIntegerInputValues_

		% Integer output variables.
		integerOutputs_
		integerOutputSize_
		integerOutputNames_
		debugIntegerOutputValues_

		% Boolean parameters.
		booleanParameters_
		booleanParameterSize_
		booleanParameterNames_
		debugBooleanParameterValues_

		% Boolean input variables.
		booleanInputs_
		booleanInputSize_
		booleanInputNames_
		debugBooleanInputValues_

		% Boolean output variables.
		booleanOutputs_
		booleanOutputSize_
		booleanOutputNames_
		debugBooleanOutputValues_

		% String parameters.
		stringParameters_
		stringParameterSize_
		stringParameterNames_
		debugStringParameterValues_

		% String input variables.
		stringInputs_
		stringInputSize_
		stringInputNames_
		debugStringInputValues_

		% String output variables.
		stringOutputs_
		stringOutputSize_
		stringOutputNames_
		debugStringOutputValues_

	end % properties


	methods

		% Full constructor.
		function obj = FMIAdapter()
			obj.fmippexActive_ = false;
			obj.enforceTimeStep_ = false;
		end

		% Enforce time step, call from init(...) function.
		function enforceTimeStep( obj, stepSize )
			obj.enforceTimeStep_ = true;
			obj.enforcedTimeStepSize_ = stepSize;
		end

		% Initialize base. Call this function in order to activate the FMI export interface during co-simulation.
		function initBackEnd( obj )
			% Variable that indicates if FMI++ export interface is active.
			obj.fmippexActive_ = true;

			% Create a new FMI backend.
			obj.backend_ = fmippex.FMIComponentBackEnd();

			% Start the initialization of the backend.
			initStatus = obj.backend_.startInitialization();
			if initStatus ~= fmippex.fmi2OK()
				error( 'FMIAdapter:initBackEnd', 'start of initialization of FMI++ interface unsuccessful' );
			end

			obj.init( obj.backend_.getCurrentCommunicationPoint() );

			% Let's do fixed time steps!
			if obj.enforceTimeStep_ == true
				obj.backend_.enforceTimeStep( obj.enforcedTimeStepSize_ );
			end

			% End the initialization of the backend.
			initStatus = obj.backend_.endInitialization();

			if initStatus ~= fmippex.fmi2OK()
				error( 'FMIAdapter:initBackEnd', 'end of initialization of FMI++ interface unsuccessful' );
			end
		end % function initBackEnd( obj )


		% Iterate the FMU.
		function run( obj )
			while obj.fmippexActive_ == true
				% Wait for simulation master to hand over control.
				obj.backend_.waitForMaster();

				% Make a step.
				doStep( obj, obj.backend_.getCurrentCommunicationPoint(), obj.backend_.getCommunicationStepSize() );

				% Let's do fixed time steps!
				if obj.enforceTimeStep_ == true
					obj.backend_.enforceTimeStep( obj.enforcedTimeStepSize_ );
				end

				% Give back control to simulation master.
				obj.backend_.signalToMaster();
			end
		end % function run( obj )


		% Check if fixed time steps are enforced.
		function check = checkEnforceTimeStep( obj )
			check = obj.enforceTimeStep_;
		end % function checkEnforceTimeStep


		function defineRealParameters( obj, parameterNames )
			% Define parameters (of type real).
			obj.realParameterSize_ = size( parameterNames, 2 );
			obj.realParameters_ = fmippex.new_double_array( obj.realParameterSize_ );
			obj.realParameterNames_ = parameterNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugRealParameterValues_ = zeros( 1, obj.realParameterSize_ );
				return;
			end

			% Define parameter names (of type real).
			realParameterLabels = fmippex.new_string_array( obj.realParameterSize_ );
			for i = 1 : obj.realParameterSize_
				fmippex.string_array_setitem( realParameterLabels, i-1, char( parameterNames(i) ) );
			end

			% Initialize parameters of type real.
			status = obj.backend_.initializeRealParameters( realParameterLabels, obj.realParameters_, obj.realParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineRealParameters', 'initializeRealParameters not successful' ); end
		end % function defineRealParameters


		function defineRealInputs( obj, inputVariableNames )
			% Define inputs (of type real).
			obj.realInputSize_ = size( inputVariableNames, 2 );
			obj.realInputs_ = fmippex.new_double_array( obj.realInputSize_ );
			obj.realInputNames_ = inputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugRealInputValues_ = zeros( 1, obj.realInputSize_ );
				return;
			end

			% Define input variable names (of type real).
			realInputLabels = fmippex.new_string_array( obj.realInputSize_ );
			for i = 1 : obj.realInputSize_
				fmippex.string_array_setitem( realInputLabels, i-1, char( inputVariableNames(i) ) );
			end

			% Initialize inputs (of type real).
			status = obj.backend_.initializeRealInputs( realInputLabels, obj.realInputs_, obj.realInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineRealInputs', 'initializeRealInputs not successful' ); end
		end % function defineRealInputs


		function defineRealOutputs( obj, outputVariableNames )
			% Define outputs (of type real).
			obj.realOutputSize_ = size( outputVariableNames, 2 );
			obj.realOutputs_ = fmippex.new_double_array( obj.realOutputSize_ );
			obj.realOutputNames_ = outputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugRealOutputValues_ = zeros( 1, obj.realOutputSize_ );
				return;
			end

			% Define output variable names (of type real).
			realOutputLabels = fmippex.new_string_array( obj.realOutputSize_ );
			for i = 1 : obj.realOutputSize_
				fmippex.string_array_setitem( realOutputLabels, i-1, char( outputVariableNames(i) ) );
			end

			% Initialize outputs (of type real).
			status = obj.backend_.initializeRealOutputs( realOutputLabels, obj.realOutputs_, obj.realOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineRealOutputs', 'initializeRealOutputs not successful' ); end
		end % function defineRealOutputs


		function defineIntegerParameters( obj, parameterNames )
			% Define parameters (of type integer).
			obj.integerParameterSize_ = size( parameterNames, 2 );
			obj.integerParameters_ = fmippex.new_int_array( obj.integerParameterSize_ );
			obj.integerParameterNames_ = parameterNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugIntegerParameterValues_ = zeros( 1, obj.integerParameterSize_ );
				return;
			end

			% Define parameter names (of type integer).
			integerParameterLabels = fmippex.new_string_array( obj.integerParameterSize_ );
			for i = 1 : obj.integerParameterSize_
				fmippex.string_array_setitem( integerParameterLabels, i-1, char( parameterNames(i) ) );
			end

			% Initialize parameters of type integer.
			status = obj.backend_.initializeIntegerParameters( integerParameterLabels, obj.integerParameters_, obj.integerParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineIntegerParameters', 'initializeIntegerParameters not successful' ); end
		end % function defineIntegerParameters


		function defineIntegerInputs( obj, inputVariableNames )
			% Define inputs (of type integer).
			obj.integerInputSize_ = size( inputVariableNames, 2 );
			obj.integerInputs_ = fmippex.new_int_array( obj.integerInputSize_ );
			obj.integerInputNames_ = inputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugIntegerInputValues_ = zeros( 1, obj.integerInputSize_ );
				return;
			end

			% Define input variable names (of type integer).
			integerInputLabels = fmippex.new_string_array( obj.integerInputSize_ );
			for i = 1 : obj.integerInputSize_
				fmippex.string_array_setitem( integerInputLabels, i-1, char( inputVariableNames(i) ) );
			end

			% Initialize inputs (of type integer).
			status = obj.backend_.initializeIntegerInputs( integerInputLabels, obj.integerInputs_, obj.integerInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineIntegerInputs', 'initializeIntegerInputs not successful' ); end
		end % define defineIntegerInputs


		function defineIntegerOutputs( obj, outputVariableNames )
			% Define outputs (of type integer).
			obj.integerOutputSize_ = size( outputVariableNames, 2 );
			obj.integerOutputs_ = fmippex.new_int_array( obj.integerOutputSize_ );
			obj.integerOutputNames_ = outputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugIntegerOutputValues_ = zeros( 1, obj.integerOutputSize_ );
				return;
			end

			% Define output variable names (of type integer).
			integerOutputLabels = fmippex.new_string_array( obj.integerOutputSize_ );
			for i = 1 : obj.integerOutputSize_
				fmippex.string_array_setitem( integerOutputLabels, i-1, char( outputVariableNames(i) ) );
			end

			% Initialize outputs (of type integer).
			status = obj.backend_.initializeIntegerOutputs( integerOutputLabels, obj.integerOutputs_, obj.integerOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineIntegerOutputs', 'initializeIntegerOutputs not successful' ); end
		end % function defineIntegerOutputs


		function defineBooleanParameters( obj, parameterNames )
			% Define parameters (of type boolean).
			obj.booleanParameterSize_ = size( parameterNames, 2 );
			obj.booleanParameters_ = fmippex.new_char_array( obj.booleanParameterSize_ );
			obj.booleanParameterNames_ = parameterNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugBooleanParameterValues_ = zeros( 1, obj.booleanParameterSize_ );
				return;
			end

			% Define parameter names (of type boolean).
			booleanParameterLabels = fmippex.new_string_array( obj.booleanParameterSize_ );
			for i = 1 : obj.booleanParameterSize_
				fmippex.string_array_setitem( booleanParameterLabels, i-1, char( parameterNames(i) ) );
			end

			% Initialize parameters of type boolean.
			status = obj.backend_.initializeBooleanParameters( booleanParameterLabels, obj.booleanParameters_, obj.booleanParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineBooleanParameters', 'initializeBooleanParameters not successful' ); end
		end % function defineBooleanParameters


		function defineBooleanInputs( obj, inputVariableNames )
			% Define inputs (of type boolean).
			obj.booleanInputSize_ = size( inputVariableNames, 2 );
			obj.booleanInputs_ = fmippex.new_char_array( obj.booleanInputSize_ );
			obj.booleanInputNames_ = inputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugBooleanInputValues_ = zeros( 1, obj.booleanInputSize_ );
				return;
			end

			% Define input variable names (of type boolean).
			booleanInputLabels = fmippex.new_string_array( obj.booleanInputSize_ );
			for i = 1 : obj.booleanInputSize_
				fmippex.string_array_setitem( booleanInputLabels, i-1, char( inputVariableNames(i) ) );
			end

			% Initialize inputs (of type boolean).
			status = obj.backend_.initializeBooleanInputs( booleanInputLabels, obj.booleanInputs_, obj.booleanInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineBooleanInputs', 'initializeBooleanInputs not successful' ); end
		end % function defineBooleanInputs


		function defineBooleanOutputs( obj, outputVariableNames )
			% Define outputs (of type boolean).
			obj.booleanOutputSize_ = size( outputVariableNames, 2 );
			obj.booleanOutputs_ = fmippex.new_char_array( obj.booleanOutputSize_ );
			obj.booleanOutputNames_ = outputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugBooleanOutputValues_ = zeros( 1, obj.booleanOutputSize_ );
				return;
			end

			% Define output variable names (of type boolean).
			booleanOutputLabels = fmippex.new_string_array( obj.booleanOutputSize_ );
			for i = 1 : obj.booleanOutputSize_
				fmippex.string_array_setitem( booleanOutputLabels, i-1, char( outputVariableNames(i) ) );
			end

			% Initialize outputs (of type boolean).
			status = obj.backend_.initializeBooleanOutputs( booleanOutputLabels, obj.booleanOutputs_, obj.booleanOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineBooleanOutputs', 'initializeBooleanOutputs not successful' ); end
		end % function defineBooleanOutputs


		function defineStringParameters( obj, parameterNames )
			% Define Parameters (of type string).
			obj.stringParameterSize_ = size( parameterNames, 2 );
			obj.stringParameters_ = fmippex.new_string_array( obj.stringParameterSize_ );
			obj.stringParameterNames_ = parameterNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugStringParameterValues_ = cell( 1, obj.stringParameterSize_ );
				return;
			end

			% Define parameter names (of type string).
			stringParameterLabels = fmippex.new_string_array( obj.stringParameterSize_ );
			for i = 1 : obj.stringParameterSize_
				fmippex.string_array_setitem( stringParameterLabels, i-1, char( parameterNames(i) ) );
			end

			% Initialize parameters of type string.
			status = obj.backend_.initializeStringParameters( stringParameterLabels, obj.stringParameters_, obj.stringParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineStringParameters', 'initializeStringParameters not successful' ); end
		end % function defineStringParameters


		function defineStringInputs( obj, inputVariableNames )
			% Define inputs (of type string).
			obj.stringInputSize_ = size( inputVariableNames, 2 );
			obj.stringInputs_ = fmippex.new_string_array( obj.stringInputSize_ );
			obj.stringInputNames_ = inputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugStringInputValues_ = cell( 1, obj.stringInputSize_ );
				return;
			end

			% Define input variable names (of type string).
			stringInputLabels = fmippex.new_string_array( obj.stringInputSize_ );
			for i = 1 : obj.stringInputSize_
				fmippex.string_array_setitem( stringInputLabels, i-1, char( inputVariableNames(i) ) );
			end

			% Initialize inputs (of type string).
			status = obj.backend_.initializeStringInputs( stringInputLabels, obj.stringInputs_, obj.stringInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineStringInputs', 'initializeStringInputs not successful' ); end
		end % function defineStringInputs


		function defineStringOutputs( obj, outputVariableNames )
			% Define outputs (of type string).
			obj.stringOutputSize_ = size( outputVariableNames, 2 );
			obj.stringOutputs_ = fmippex.new_string_array( obj.stringOutputSize_ );
			obj.stringOutputNames_ = outputVariableNames;

			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugStringOutputValues_ = cell( 1, obj.stringOutputSize_ );
				return;
			end

			% Define output variable names (of type string).
			stringOutputLabels = fmippex.new_string_array( obj.stringOutputSize_ );
			for i = 1 : obj.stringOutputSize_
				fmippex.string_array_setitem( stringOutputLabels, i-1, char( outputVariableNames(i) ) );
			end

			% Initialize outputs (of type string).
			status = obj.backend_.initializeStringOutputs( stringOutputLabels, obj.stringOutputs_, obj.stringOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:defineStringOutputs', 'initializeStringOutputs not successful' ); end
		end % function defineStringOutputs


		function realParameterValues = getRealParameterValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				realParameterValues = obj.debugRealParameterValues_;
				return;
			end

			% Read parameters.
			status = obj.backend_.getRealParameters( obj.realParameters_, obj.realParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getRealParameterValues', 'getRealParameters not successful' ); end

			realParameterValues = NaN( 1, obj.realParameterSize_ );
			for i = 1 : obj.realParameterSize_
				realParameterValues(i) = fmippex.double_array_getitem( obj.realParameters_, i-1 );
			end
		end % function getRealParameterValues


		function integerParameterValues = getIntegerParameterValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				integerParameterValues = obj.debugIntegerParameterValues_;
				return;
			end

			% Read parameters.
			status = obj.backend_.getIntegerParameters( obj.integerParameters_, obj.integerParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getIntegerParameterValues', 'getIntegerParameters not successful' ); end

			integerParameterValues = NaN( 1, obj.integerParameterSize_ );
			for i = 1 : obj.integerParameterSize_
				integerParameterValues(i) = fmippex.int_array_getitem( obj.integerParameters_, i-1 );
			end
		end % function getIntegerParameterValues


		function booleanParameterValues = getBooleanParameterValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				booleanParameterValues = obj.debugBooleanParameterValues_;
				return;
			end

			% Read parameters.
			status = obj.backend_.getBooleanParameters( obj.booleanParameters_, obj.booleanParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getBooleanParameterValues', 'getBooleanParameters not successful' ); end

			booleanParameterValues = NaN( 1, obj.booleanParameterSize_ );
			for i = 1 : obj.booleanParameterSize_
				booleanParameterValues(i) = fmippex.char_array_getitem( obj.booleanParameters_, i-1 );
			end
		end % function getBooleanParameterValues


		function stringParameterValues = getStringParameterValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				stringParameterValues = obj.debugStringParameterValues_;
				return;
			end

			% Read parameters.
			status = obj.backend_.getStringParameters( obj.stringParameters_, obj.stringParameterSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getStringParameterValues', 'getStringParameters not successful' ); end

			stringParameterValues = {};
			for i = 1 : obj.stringParameterSize_
				stringParameterValues(i) = cellstr( fmippex.string_array_getitem( obj.stringParameters_, i-1 ) );
			end
		end % function getStringParameterValues


		function realInputValues = getRealInputValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				realInputValues = obj.debugRealInputValues_;
				return;
			end

			% Read current inputs.
			status = obj.backend_.getRealInputs( obj.realInputs_, obj.realInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getRealInputValues', 'getRealInputs not successful' ); end

			realInputValues = NaN( 1, obj.realInputSize_ );
			for i = 1 : obj.realInputSize_
				realInputValues(i) = fmippex.double_array_getitem( obj.realInputs_, i-1 );
			end
		end % function getRealInputValues


		function integerInputValues = getIntegerInputValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				integerInputValues = obj.debugIntegerInputValues_;
				return;
			end

			% Read current inputs.
			status = obj.backend_.getIntegerInputs( obj.integerInputs_, obj.integerInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getIntegerInputValues', 'getIntegerInputs not successful' ); end

			integerInputValues = NaN( 1, obj.integerInputSize_ );
			for i = 1 : obj.integerInputSize_
				integerInputValues(i) = fmippex.int_array_getitem( obj.integerInputs_, i-1 );
			end
		end % function getIntegerInputValues


		function booleanInputValues = getBooleanInputValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				booleanInputValues = obj.debugBooleanInputValues_;
				return;
			end

			% Read current inputs.
			status = obj.backend_.getBooleanInputs( obj.booleanInputs_, obj.booleanInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getBooleanInputValues', 'getBooleanInputs not successful' ); end

			booleanInputValues = NaN( 1, obj.booleanInputSize_ );
			for i = 1 : obj.booleanInputSize_
				booleanInputValues(i) = fmippex.char_array_getitem( obj.booleanInputs_, i-1 );
			end
		end % function getBooleanInputValues


		function stringInputValues = getStringInputValues( obj )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				stringInputValues = obj.debugStringInputValues_;
				return;
			end

			% Read current inputs.
			status = obj.backend_.getStringInputs( obj.stringInputs_, obj.stringInputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:getStringInputValues', 'getStringInputs not successful' ); end

			stringInputValues = {};
			for i = 1 : obj.stringInputSize_
				stringInputValues(i) = cellstr( fmippex.string_array_getitem( obj.stringInputs_, i-1 ) );
			end
		end % function getStringInputValues


		function setRealOutputValues( obj, realOutputValues )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugRealOutputValues_ = realOutputValues;
				return;
			end

			% Set output values.
			for i = 1 : obj.realOutputSize_
				fmippex.double_array_setitem( obj.realOutputs_, i-1, realOutputValues(i) );
			end

			% Write current outputs.
			status = obj.backend_.setRealOutputs( obj.realOutputs_, obj.realOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:setRealOutputValues', 'setRealOutputs not successful' ); end
		end % function setRealOutputValues


		function setIntegerOutputValues( obj, integerOutputValues )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugIntegerOutputValues_ = integerOutputValues;
				return;
			end

			% Set output values.
			for i = 1 : obj.integerOutputSize_
				fmippex.int_array_setitem( obj.integerOutputs_, i-1, integerOutputValues(i) );
			end

			% Write current outputs.
			status = obj.backend_.setIntegerOutputs( obj.integerOutputs_, obj.integerOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:setIntegerOutputValues', 'setIntegerOutputs not successful' ); end
		end % function setIntegerOutputValues


		function setBooleanOutputValues( obj, booleanOutputValues )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugBooleanOutputValues_ = booleanOutputValues;
				return;
			end

			% Set output values.
			for i = 1 : obj.booleanOutputSize_
				fmippex.char_array_setitem( obj.booleanOutputs_, i-1, booleanOutputValues(i) );
			end

			% Write current outputs.
			status = obj.backend_.setBooleanOutputs( obj.booleanOutputs_, obj.booleanOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:setBooleanOutputValues', 'setBooleanOutputs not successful' ); end
		end % function setBooleanOutputValues


		function setStringOutputValues( obj, stringOutputValues )
			% Check if export interface is active.
			if obj.fmippexActive_ == false
				warning( 'FMI++ export interface is not active.' );
				obj.debugStringOutputValues_ = stringOutputValues;
				return;
			end

			% Set output values.
			for i = 1 : obj.stringOutputSize_
				fmippex.string_array_setitem( obj.stringOutputs_, i-1, char( stringOutputValues(i) ) );
			end

			% Write current outputs.
			status = obj.backend_.setStringOutputs( obj.stringOutputs_, obj.stringOutputSize_ );
			if status ~= fmippex.fmi2OK(); error( 'FMIAdapter:setStringOutputValues', 'setStringOutputs not successful' ); end
		end % function setStringOutputValues


		function debugSetRealParameterValues( obj, realParameterValues )
			if size( realParameterValues, 2 ) ~= obj.realParameterSize_
				error( 'FMIAdapter:debugSetRealParameterValues', 'inconsistent number of parameter values' );
			end

			obj.debugRealParameterValues_ = realParameterValues;
		end % debugSetRealParameterValues


		function debugSetRealInputValues( obj, realInputValues )
			if size( realInputValues, 2 ) ~= obj.realInputSize_
				error( 'FMIAdapter:debugSetRealInputValues', 'inconsistent number of parameter values' );
			end

			obj.debugRealInputValues_ = realInputValues;
		end % debugSetRealInputValues


		function realOutputValues = debugGetRealOutputValues( obj )
			realOutputValues = obj.debugRealOutputValues_;
		end % debugGetRealOutputValues


		function debugSetIntegerParameterValues( obj, integerParameterValues )
			if size( integerParameterValues, 2 ) ~= obj.integerParameterSize_
				error( 'FMIAdapter:debugSetIntegerParameterValues', 'inconsistent number of parameter values' );
			end

			obj.debugIntegerParameterValues_ = integerParameterValues;
		end % debugSetIntegerParameterValues


		function debugSetIntegerInputValues( obj, integerInputValues )
			if size( integerInputValues, 2 ) ~= obj.integerInputSize_
				error( 'FMIAdapter:debugSetIntegerInputValues', 'inconsistent number of parameter values' );
			end

			obj.debugIntegerInputValues_ = integerInputValues;
		end % debugSetIntegerInputValues


		function integerOutputValues = debugGetIntegerOutputValues( obj )
			integerOutputValues = obj.debugIntegerOutputValues_;
		end % debugGetIntegerOutputValues


		function debugSetBooleanParameterValues( obj, booleanParameterValues )
			if size( booleanParameterValues, 2 ) ~= obj.booleanParameterSize_
				error( 'FMIAdapter:debugSetBooleanParameterValues', 'inconsistent number of parameter values' );
			end

			obj.debugBooleanParameterValues_ = booleanParameterValues;
		end % debugSetBooleanParameterValues


		function debugSetBooleanInputValues( obj, booleanInputValues )
			if size( booleanInputValues, 2 ) ~= obj.booleanInputSize_
				error( 'FMIAdapter:debugSetRealInputValues', 'inconsistent number of parameter values' );
			end

			obj.debugBooleanInputValues_ = booleanInputValues;
		end % debugSetBooleanInputValues


		function booleanOutputValues = debugGetBooleanOutputValues( obj )
			booleanOutputValues = obj.debugBooleanOutputValues_;
		end % debugGetBooleanOutputValues


		function debugSetStringParameterValues( obj, stringParameterValues )
			if size( stringParameterValues, 2 ) ~= obj.stringParameterSize_
				error( 'FMIAdapter:debugSetStringParameterValues', 'inconsistent number of parameter values' );
			end

			obj.debugStringParameterValues_ = StringParameterValues;
		end % debugSetStringParameterValues


		function debugSetStringInputValues( obj, stringInputValues )
			if size( stringInputValues, 2 ) ~= obj.stringInputSize_
				error( 'FMIAdapter:debugSetStringInputValues', 'inconsistent number of parameter values' );
			end

			obj.debugStringInputValues_ = stringInputValues;
		end % debugSetStringInputValues


		function stringOutputValues = debugGetStringOutputValues( obj )
			stringOutputValues = obj.debugStringOutputValues_;
		end % debugGetStringOutputValues

	end % methods

end % classdef