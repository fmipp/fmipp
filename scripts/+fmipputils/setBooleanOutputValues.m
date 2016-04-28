function setBooleanOutputValues( backend, booleanOutputs, booleanOutputSize, booleanOutputValues )
   % Set output values.
   for i = 1 : booleanOutputSize
      fmippex.char_array_setitem( booleanOutputs, i-1, booleanOutputValues(i) );
   end
   
   % Write current outputs.
   status = backend.setBooleanOutputs( booleanOutputs, booleanOutputSize );
   if status ~= fmippex.fmiOK(); error( 'setBooleanOutputs not successful' ); end
end