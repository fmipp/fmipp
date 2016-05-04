function integerInputValues = getIntegerInputValues( backend, integerInputs, integerInputSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      integerInputValues = zeros( 1, integerInputSize );
	  return;
   end

   % Read current inputs.
   status = backend.getIntegerInputs( integerInputs, integerInputSize );
   if status ~= fmippex.fmiOK(); error( 'getIntegerInputs not successful' ); end
   
   integerInputValues = NaN( 1, integerInputSize );
   for i = 1 : integerInputSize
      integerInputValues(i) = fmippex.int_array_getitem( integerInputs, i-1 );
   end
end