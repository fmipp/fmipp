function setIntegerOutputValues( backend, integerOutputs, integerOutputSize, integerOutputValues )
   % Set output values.
   for i = 1 : integerOutputSize
      fmippex.int_array_setitem( integerOutputs, i-1, integerOutputValues(i) );
   end
   
   % Write current outputs.
   status = backend.setIntegerOutputs( integerOutputs, integerOutputSize );
   if status ~= fmippex.fmiOK(); error( 'setIntegerOutputs not successful' ); end
end