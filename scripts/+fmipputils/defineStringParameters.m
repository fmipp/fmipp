function [stringParameters, stringParameterSize] = defineStringParameters( backend, parameterNames )
   % Define Parameters (of type string).
   stringParameterSize = size( parameterNames, 2 );
   stringParameters = fmippex.new_string_array( stringParameterSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexStringParameterNames;
      fmippexStringParameterNames = parameterNames;
	  return;
   end

   % Define Parameter variable names (of type string).
   stringParameterLabels = fmippex.new_string_array( stringParameterSize );
   for i = 1 : stringParameterSize
      fmippex.string_array_setitem( stringParameterLabels, i-1, char( parameterNames(i) ) );
   end

   % Initialize parameters of type string as input variables (this needs to be fixed).
   status = backend.initializeStringInputs( stringParameterLabels, stringParameterSize );
   if status ~= fmippex.fmiOK(); error( 'initializeStringInputs not successful' ); end
end