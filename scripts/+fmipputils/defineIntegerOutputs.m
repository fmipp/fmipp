function [integerOutputs, integerOutputSize] = defineIntegerOutputs( backend, outputVariableNames )
   % Define outputs (of type integer).
   integerOutputSize = size( outputVariableNames, 2 );
   integerOutputs = fmippex.new_int_array( integerOutputSize );

   % Define output variable names (of type integer).
   integerOutputLabels = fmippex.new_string_array( integerOutputSize );
   for i = 1 : integerOutputSize
      fmippex.string_array_setitem( integerOutputLabels, i-1, char( outputVariableNames(i) ) );
   end

   % Initialize outputs (of type integer).
   status = backend.initializeIntegerOutputs( integerOutputLabels, integerOutputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeIntegerOutputs not successful' ); end
end