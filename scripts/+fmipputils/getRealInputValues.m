function realInputValues = getRealInputValues( backend, realInputs, realInputSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      realInputValues = zeros( 1, realInputSize );
	  return;
   end

   % Read current inputs.
   status = backend.getRealInputs( realInputs, realInputSize );
   if status ~= fmippex.fmiOK(); error( 'getRealInputs not successful' ); end
   
   realInputValues = NaN( 1, realInputSize );
   for i = 1 : realInputSize
      realInputValues(i) = fmippex.double_array_getitem( realInputs, i-1 );
   end
end