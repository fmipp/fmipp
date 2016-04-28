function [realOutputs, realOutputSize] = defineRealOutputs( backend, outputVariableNames )
   % Define outputs (of type real).
   realOutputSize = size( outputVariableNames, 2 );
   realOutputs = fmippex.new_double_array( realOutputSize );

   % Define output variable names (of type real).
   realOutputLabels = fmippex.new_string_array( realOutputSize );
   for i = 1 : realOutputSize
      fmippex.string_array_setitem( realOutputLabels, i-1, char( outputVariableNames(i) ) );
   end

   % Initialize outputs (of type real).
   status = backend.initializeRealOutputs( realOutputLabels, realOutputSize );
   if status ~= fmippex.fmiOK(); error( 'initializeRealOutputs not successful' ); end
end