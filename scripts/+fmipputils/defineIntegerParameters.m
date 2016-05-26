function [integerParameters, integerParameterSize] = defineIntegerParameters( backend, parameterNames )
   % Define parameters (of type integer).
   integerParameterSize = size( parameterNames, 2 );
   integerParameters = fmippex.new_int_array( integerParameterSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexIntegerParameterNames;
      fmippexIntegerParameterNames = parameterNames;
	  return;
   end

   % Define parameter names (of type integer).
   integerParameterLabels = fmippex.new_string_array( integerParameterSize );
   for i = 1 : integerParameterSize
      fmippex.string_array_setitem( integerParameterLabels, i-1, char( parameterNames(i) ) );
   end

   % Initialize parameters of type integer as input variables (this needs to be fixed).
   status = backend.initializeIntegerInputs( integerParameterLabels, integerParameterSize );
   if status ~= fmippex.fmiOK(); error( 'initializeIntegerInputs not successful' ); end
end