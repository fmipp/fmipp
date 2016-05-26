function stringParameterValues = getStringParameterValues( backend, stringParameters, stringParameterSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      stringParameterValues = cell( 1, stringParameterSize );
	  return;
   end

   % Read current parameters like inputs (this needs to be fixed).
   status = backend.getStringInputs( stringParameters, stringParameterSize );
   if status ~= fmippex.fmiOK(); error( 'getStringInputs not successful' ); end
   
   stringParameterValues = {}
   for i = 1 : stringParameterSize
      stringParameterValues(i) = cellstr( fmippex.string_array_getitem( stringParameters, i-1 ) );
   end
end