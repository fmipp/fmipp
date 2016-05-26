function realParameterValues = getRealParameterValues( backend, realParameters, realParameterSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      realParameterValues = zeros( 1, realParameterSize );
	  return;
   end

   % Read current parameters like inputs (this needs to be fixed).
   status = backend.getRealInputs( realParameters, realParameterSize );
   if status ~= fmippex.fmiOK(); error( 'getRealInputs not successful' ); end
   
   realParameterValues = NaN( 1, realParameterSize );
   for i = 1 : realParameterSize
      realParameterValues(i) = fmippex.double_array_getitem( realParameters, i-1 );
   end
end