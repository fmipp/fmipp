function setStringOutputValues( backend, stringOutputs, stringOutputSize, stringOutputValues )
   % Set output values.
   for i = 1 : stringOutputSize
      fmippex.string_array_setitem( stringOutputs, i-1, char( stringOutputValues(i) ) );
   end
   
   % Write current outputs.
   status = backend.setStringOutputs( stringOutputs, stringOutputSize );
   if status ~= fmippex.fmiOK(); error( 'setStringOutputs not successful' ); end
end