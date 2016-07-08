function stringParameterValues = getStringParameterValues( backend, stringParameters, stringParameterSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      stringParameterValues = cell( 1, stringParameterSize );
	  return;
   end

   % Read parameters.
   status = backend.getStringParameters( stringParameters, stringParameterSize );
   if status ~= fmippex.fmiOK(); error( 'getStringParameters not successful' ); end
   
   stringParameterValues = {}
   for i = 1 : stringParameterSize
      stringParameterValues(i) = cellstr( fmippex.string_array_getitem( stringParameters, i-1 ) );
   end
end