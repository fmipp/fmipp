function [stringInputs, stringInputSize] = defineStringInputs( backend, inputVariableNames )
   % Define inputs (of type string).
   stringInputSize = size( inputVariableNames, 2 );
   stringInputs = fmippex.new_string_array( stringInputSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexStringInputNames;
      fmippexStringInputNames = inputVariableNames;
	  return;
   end

   % Define input variable names (of type string).
   stringInputLabels = fmippex.new_string_array( stringInputSize );
   for i = 1 : stringInputSize
      fmippex.string_array_setitem( stringInputLabels, i-1, char( inputVariableNames(i) ) );
   end

   % Initialize inputs (of type string).
   status = backend.initializeStringInputs( stringInputLabels, stringInputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeStringInputs not successful' ); end
end