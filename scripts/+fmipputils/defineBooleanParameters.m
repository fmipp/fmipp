function [booleanParameters, booleanParameterSize] = defineBooleanParameters( backend, parameterNames )
   % Define parameters (of type boolean).
   booleanParameterSize = size( parameterNames, 2 );
   booleanParameters = fmippex.new_char_array( booleanParameterSize );

   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  global fmippexBooleanParameterNames;
      fmippexBooleanParameterNames = parameterNames;
	  return;
   end

   % Define parameter names (of type boolean).
   booleanParameterLabels = fmippex.new_string_array( booleanParameterSize );
   for i = 1 : booleanParameterSize
      fmippex.string_array_setitem( booleanParameterLabels, i-1, char( parameterNames(i) ) );
   end

   % Initialize parameters of type boolean.
   status = backend.initializeBooleanParameters( booleanParameterLabels, booleanParameterSize );
   if status ~= fmippex.fmiOK(); error( 'initializeBooleanParameters not successful' ); end
end