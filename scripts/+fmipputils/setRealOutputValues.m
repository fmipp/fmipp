function setRealOutputValues( backend, realOutputs, realOutputSize, realOutputValues )
   % Check if export interface is active.
   global fmippexActive;
   if fmippexActive == false
      warning( 'FMI++ export interface is not active.' );
	  return;
   end

   % Set output values.
   for i = 1 : realOutputSize
      fmippex.double_array_setitem( realOutputs, i-1, realOutputValues(i) );
   end
   
   % Write current outputs.
   status = backend.setRealOutputs( realOutputs, realOutputSize );
   if status ~= fmippex.fmiOK(); error( 'setRealOutputs not successful' ); end
end