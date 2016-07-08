function integerParameterValues = getIntegerParameterValues( backend, integerParameters, integerParameterSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      integerParameterValues = zeros( 1, integerParameterSize );
	  return;
   end

   % Read parameters.
   status = backend.getIntegerParameters( integerParameters, integerParameterSize );
   if status ~= fmippex.fmiOK(); error( 'getIntegerParameters not successful' ); end
   
   integerParameterValues = NaN( 1, integerParameterSize );
   for i = 1 : integerParameterSize
      integerParameterValues(i) = fmippex.int_array_getitem( integerParameters, i-1 );
   end
end