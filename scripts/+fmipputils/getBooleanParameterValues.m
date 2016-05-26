function booleanParameterValues = getBooleanParameterValues( backend, booleanParameters, booleanParameterSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      booleanParameterValues = zeros( 1, booleanParameterSize );
	  return;
   end

   % Read current parameters like inputs (this needs to be fixed).
   status = backend.getBooleanInputs( booleanParameters, booleanParameterSize );
   if status ~= fmippex.fmiOK(); error( 'getBooleanInputs not successful' ); end
   
   booleanParameterValues = NaN( 1, booleanParameterSize );
   for i = 1 : booleanParameterSize
      booleanParameterValues(i) = fmippex.char_array_getitem( booleanParameters, i-1 );
   end
end