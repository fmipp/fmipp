function [stringOutputs, stringOutputSize] = defineStringOutputs( backend, outputVariableNames )
   % Define outputs (of type string).
   stringOutputSize = size( outputVariableNames, 2 );
   stringOutputs = fmippex.new_string_array( stringOutputSize );

   % Define output variable names (of type string).
   stringOutputLabels = fmippex.new_string_array( stringOutputSize );
   for i = 1 : stringOutputSize
      fmippex.string_array_setitem( stringOutputLabels, i-1, char( outputVariableNames(i) ) );
   end

   % Initialize outputs (of type string).
   status = backend.initializeStringOutputs( stringOutputLabels, stringOutputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeStringOutputs not successful' ); end
end