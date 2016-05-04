function [booleanOutputs, booleanOutputSize] = defineBooleanOutputs( backend, outputVariableNames )
   % Define outputs (of type boolean).
   booleanOutputSize = size( outputVariableNames, 2 );
   booleanOutputs = fmippex.new_char_array( booleanOutputSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexBooleanOutputNames;
      fmippexBooleanOutputNames = outputVariableNames;
	  return;
   end

   % Define output variable names (of type boolean).
   booleanOutputLabels = fmippex.new_string_array( booleanOutputSize );
   for i = 1 : booleanOutputSize
      fmippex.string_array_setitem( booleanOutputLabels, i-1, char( outputVariableNames(i) ) );
   end

   % Initialize outputs (of type boolean).
   status = backend.initializeBooleanOutputs( booleanOutputLabels, booleanOutputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeBooleanOutputs not successful' ); end
end