function stringInputValues = getStringInputValues( backend, stringInputs, stringInputSize )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
      stringInputValues = cell( 1, stringInputSize );
	  return;
   end

   % Read current inputs.
   status = backend.getStringInputs( stringInputs, stringInputSize );
   if status ~= fmippex.fmiOK(); error( 'getStringInputs not successful' ); end
   
   stringInputValues = {}
   for i = 1 : stringInputSize
      stringInputValues(i) = cellstr( fmippex.string_array_getitem( stringInputs, i-1 ) );
   end
end