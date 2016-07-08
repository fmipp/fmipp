function [realParameters, realParameterSize] = defineRealParameters( backend, parameterNames )
   % Define parameters (of type real).
   realParameterSize = size( parameterNames, 2 );
   realParameters = fmippex.new_double_array( realParameterSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexRealParameterNames;
      fmippexRealParameterNames = parameterNames;
	  return;
   end

   % Define parameter names (of type real).
   realParameterLabels = fmippex.new_string_array( realParameterSize );
   for i = 1 : realParameterSize
      fmippex.string_array_setitem( realParameterLabels, i-1, char( parameterNames(i) ) );
   end

   % Initialize parameters of type real.
   status = backend.initializeRealParameters( realParameterLabels, realParameterSize );
   if status ~= fmippex.fmiOK(); error( 'initializeRealParameters not successful' ); end
end