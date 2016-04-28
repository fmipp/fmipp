function [booleanInputs, booleanInputSize] = defineBooleanInputs( backend, inputVariableNames )
   % Define inputs (of type boolean).
   booleanInputSize = size( inputVariableNames, 2 );
   booleanInputs = fmippex.new_char_array( booleanInputSize );

   % Define input variable names (of type boolean).
   booleanInputLabels = fmippex.new_string_array( booleanInputSize );
   for i = 1 : booleanInputSize
      fmippex.string_array_setitem( booleanInputLabels, i-1, char( inputVariableNames(i) ) );
   end

   % Initialize inputs (of type boolean).
   status = backend.initializeBooleanInputs( booleanInputLabels, booleanInputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeBooleanInputs not successful' ); end
end