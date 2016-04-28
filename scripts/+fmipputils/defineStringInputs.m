function [stringInputs, stringInputSize] = defineStringInputs( backend, inputVariableNames )
   % Define inputs (of type string).
   stringInputSize = size( inputVariableNames, 2 );
   stringInputs = fmippex.new_string_array( stringInputSize );

   % Define input variable names (of type string).
   stringInputLabels = fmippex.new_string_array( stringInputSize );
   for i = 1 : stringInputSize
      fmippex.string_array_setitem( stringInputLabels, i-1, char( inputVariableNames(i) ) );
   end

   % Initialize inputs (of type string).
   status = backend.initializeStringInputs( stringInputLabels, stringInputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeStringInputs not successful' ); end
end