function [integerInputs, integerInputSize] = defineIntegerInputs( backend, inputVariableNames )
   % Define inputs (of type integer).
   integerInputSize = size( inputVariableNames, 2 );
   integerInputs = fmippex.new_int_array( integerInputSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexIntegerInputNames;
      fmippexIntegerInputNames = inputVariableNames;
	  return;
   end

   % Define input variable names (of type integer).
   integerInputLabels = fmippex.new_string_array( integerInputSize );
   for i = 1 : integerInputSize
      fmippex.string_array_setitem( integerInputLabels, i-1, char( inputVariableNames(i) ) );
   end

   % Initialize inputs (of type integer).
   status = backend.initializeIntegerInputs( integerInputLabels, integerInputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeIntegerInputs not successful' ); end
end