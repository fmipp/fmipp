function [realInputs, realInputSize] = defineRealInputs( backend, inputVariableNames )
   % Define inputs (of type real).
   realInputSize = size( inputVariableNames, 2 );
   realInputs = fmippex.new_double_array( realInputSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexRealInputNames;
      fmippexRealInputNames = inputVariableNames;
	  return;
   end

   % Define input variable names (of type real).
   realInputLabels = fmippex.new_string_array( realInputSize );
   for i = 1 : realInputSize
      fmippex.string_array_setitem( realInputLabels, i-1, char( inputVariableNames(i) ) );
   end

   % Initialize inputs (of type real).
   status = backend.initializeRealInputs( realInputLabels, realInputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeRealInputs not successful' ); end
end